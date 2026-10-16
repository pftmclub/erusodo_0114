#!/usr/bin/env python3
"""Query and mutate the offline save (els_db.sql) without writing a throwaway script.

  python tools/elsdb.py info                          save path, schema version, table row counts, game running?
  python tools/elsdb.py schema [table]                CREATE statements (all tables, or one)
  python tools/elsdb.py q "SELECT ..." [--tsv|--json] [--limit N]     read-only SQL (never writes, safe while the game runs)
  python tools/elsdb.py x "UPDATE ..." [--dry-run]    mutating SQL: backup, one transaction, integrity_check
  python tools/elsdb.py x --file stmts.sql            same, statements from a file
  python tools/elsdb.py chars                         every unit: uid, class, nickname, level, ed, sp, item count
  python tools/elsdb.py inv <unit> [--cat N]          a unit's items, with names from item.kim
  python tools/elsdb.py addchar <nick> <class> <level> [--sp N] [--ed N]    delegates to createchar.py
  python tools/elsdb.py setchar <unit> [--level N] [--exp N] [--ed N] [--sp N] [--nick S] [--class C]
  python tools/elsdb.py additem <unit> <id>[:count] ... [--enchant N] [--equip [--replace]] [--cat N --slot N]
  python tools/elsdb.py delitem <unit> <item_uid|item_id> ... [--by-item-id]
  python tools/elsdb.py setting [name [value]]        the `settings` table (cash_start, auth_level, ...)
  python tools/elsdb.py backup [--tag NAME]           snapshot into <data>/db_backup/ (no restore command on purpose)
  python tools/elsdb.py backups                       list snapshots, newest first

<unit> is a unit_uid or a nickname (case-insensitive). <class> is a UNIT_CLASS id or name fragment.
Every writing command: refuses while X2_offline.exe runs, takes a full backup first, runs in one
transaction (rolled back on any error or with --dry-run), and ends with PRAGMA integrity_check.
`x` additionally refuses DROP/ALTER/CREATE without --allow-ddl (the emulator migrates the schema
itself) and UPDATE/DELETE with no WHERE without --allow-all.

Item placement mirrors CX2OfflineInventory::InsertItem: the bag comes from the item's kim type
(weapon/armor -> 1, accessory -> 2, material -> 6..., fashion -> 7), quantity items top up existing
stacks then open new ones, slots are the first free one under inventory_size. kim defaults to
`staged` (what data036.kom ships); `--kim march|june|<path>` overrides. Stdlib only.
Env X2_GAME_DIR overrides the game directory (same variable build.py reads).
"""
import argparse
import json
import os
import re
import sqlite3
import subprocess
import sys
import time

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)

DATA = os.environ.get('X2_GAME_DIR', r'F:\elsword stuff\elsword_2014\els_2014\237311\24965799\data')

# CX2Inventory::SORT_TYPE (X2Lib/X2Inventory.h)
CATS = {1: 'equip', 2: 'accessory', 3: 'material', 4: 'special', 5: 'quest', 6: 'quick(consumable)', 7: 'avatar',
        8: 'bank', 9: 'EQUIPPED', 10: 'e_skill', 11: 'e_quickslot', 12: 'pc_bang', 13: 'pet', 14: 'share_bank',
        15: 'e_defence_quick'}
ST_EQUIP, ST_ACCESSORY, ST_MATERIAL, ST_SPECIAL, ST_QUEST, ST_QUICK, ST_AVATAR, ST_E_EQUIP, ST_PC_BANG = 1, 2, 3, 4, 5, 6, 7, 9, 12

# item_type name -> bag, as CX2OfflineInventory::SuitableCategory switches (fashion/pc-bang are tested first).
TYPE_TO_CAT = {'IT_WEAPON': ST_EQUIP, 'IT_DEFENCE': ST_EQUIP, 'IT_ACCESSORY': ST_ACCESSORY, 'IT_MATERIAL': ST_MATERIAL,
               'IT_SPECIAL': ST_SPECIAL, 'IT_QUEST': ST_QUEST, 'IT_QICK_SLOT': ST_QUICK, 'IT_SKILL_MEMO': ST_SPECIAL,
               'IT_CUSTOM': ST_SPECIAL}

# CX2Unit::NOW_EQIP_SLOT_ID (X2Lib/X2Unit.h) and the EQIP_POSITION that fits each slot: (position name, fashion?)
# fashion None = the slot ignores the fashion flag (accessories, as IsSuitableSlot does).
EQUIP_SLOTS = {
    0: ('EP_DEFENCE_HAIR', True), 1: ('EP_DEFENCE_BODY', False), 2: ('EP_DEFENCE_BODY', True),
    3: ('EP_DEFENCE_LEG', False), 4: ('EP_DEFENCE_LEG', True), 5: ('EP_DEFENCE_HAND', False),
    6: ('EP_DEFENCE_HAND', True), 7: ('EP_DEFENCE_FOOT', False), 8: ('EP_DEFENCE_FOOT', True),
    9: ('EP_WEAPON_HAND', False), 10: ('EP_WEAPON_HAND', True), 11: ('EP_AC_FACE1', None),
    12: ('EP_AC_FACE2', None), 13: ('EP_AC_FACE3', None), 14: ('EP_AC_BODY', None), 15: ('EP_AC_LEG', None),
    16: ('EP_AC_ARM', None), 17: ('EP_AC_RING', None), 18: ('EP_AC_NECKLESS', None), 19: ('EP_AC_WEAPON', None),
    20: ('EP_ONEPIECE_FASHION', True),
}


def die(msg):
    raise SystemExit('elsdb: ' + msg)


def say(s=''):
    sys.stdout.write(s + '\n')


def game_running():
    return 'X2_offline.exe' in subprocess.run(['tasklist'], capture_output=True, text=True).stdout


def db_path(a):
    p = os.path.join(a.data, 'els_db.sql')
    if not os.path.exists(p):
        die('no save at ' + p)
    return p


def open_ro(a):
    p = db_path(a).replace('\\', '/')
    c = sqlite3.connect('file:%s?mode=ro' % p, uri=True)
    c.text_factory = lambda b: b.decode('utf-8', 'replace')
    return c


def backup(a, c, tag):
    """Full snapshot of connection c into db_backup/; never overwrites an earlier one."""
    d = os.path.join(a.data, 'db_backup')
    os.makedirs(d, exist_ok=True)
    stamp = time.strftime('%Y%m%d_%H%M%S')
    path, n = os.path.join(d, 'els_db_%s_pre_%s.sql' % (stamp, tag)), 1
    while os.path.exists(path):
        n += 1
        path = os.path.join(d, 'els_db_%s_pre_%s_%d.sql' % (stamp, tag, n))
    dst = sqlite3.connect(path)
    c.backup(dst)
    dst.close()
    say('backup ' + path)
    return path


class Writer:
    """Context for every mutating command: guards, backup, one transaction, integrity check."""

    def __init__(self, a, tag):
        self.a, self.tag = a, tag

    def __enter__(self):
        a = self.a
        p = db_path(a)
        # --data pointing elsewhere is a copy (testing); the emulator cannot be holding that one open.
        if os.path.normcase(os.path.abspath(a.data)) == os.path.normcase(os.path.abspath(DATA)) and game_running():
            die('X2_offline.exe is running - close the game first (it holds the DB open). Reads (q/chars/inv) still work.')
        self.c = sqlite3.connect(p, isolation_level=None)  # we issue BEGIN/COMMIT ourselves
        self.c.text_factory = lambda b: b.decode('utf-8', 'replace')
        self.bak = None if a.dry_run else backup(a, self.c, self.tag)
        self.c.execute('BEGIN IMMEDIATE')
        return self.c

    def __exit__(self, et, ev, tb):
        c = self.c
        if et is not None:
            c.execute('ROLLBACK')
            c.close()
            if self.bak:  # nothing was written, so the snapshot only clutters db_backup/
                os.remove(self.bak)
                say('rolled back, nothing written (backup removed)')
            if et is SystemExit:
                return False
            say('ROLLED BACK: %s' % ev)
            raise SystemExit(1)
        if self.a.dry_run:
            c.execute('ROLLBACK')
            say('dry run - rolled back, nothing written')
        else:
            c.execute('COMMIT')
            say('committed | integrity_check: %s' % c.execute('PRAGMA integrity_check').fetchone()[0])
        c.close()
        return False


# ---- helpers -------------------------------------------------------------------------------------------------

def resolve_unit(c, arg):
    if arg.isdigit():
        r = c.execute('SELECT unit_uid, nickname FROM unit WHERE unit_uid=?', (int(arg),)).fetchone()
    else:
        r = c.execute('SELECT unit_uid, nickname FROM unit WHERE nickname=? COLLATE NOCASE', (arg,)).fetchone()
    if r is None:
        die('no unit %r (see: elsdb.py chars)' % arg)
    return r


def class_name(cls):
    import createchar
    return createchar.CLASSES.get(cls, '?')


_kim = {}


def kim(a):
    if 'k' not in _kim:
        import kim as kimmod
        _kim['k'] = kimmod.Kim(a.kim)
        _kim['m'] = kimmod
    return _kim['k']


def item_rec(a, item_id):
    """(record, item_type name) from the kim, or die: an id the client has no templet for is a broken row."""
    k = kim(a)
    r = k.record(item_id)
    if r is None:
        die('item %d is not in kim %s (use --kim march|june|<path>, or find it: python tools/kim.py find --ids %d)'
            % (item_id, a.kim, item_id))
    era = k.L['era']
    return r, _kim['m'].enum_label('item_type', r['item_type'], era), _kim['m'].enum_label('period_type', r['period_type'], era), \
        _kim['m'].enum_label('eqip_position', r['eqip_position'], era) if 'eqip_position' in r else None


def item_name(a, item_id):
    try:
        r = kim(a).record(item_id)
        return r['name'] if r else '(not in kim)'
    except SystemExit:
        return '?'


def free_slots(c, unit, cat):
    size = c.execute('SELECT size FROM inventory_size WHERE unit_uid=? AND category=?', (unit, cat)).fetchone()
    if size is None:
        die('unit %d has no inventory_size row for category %d (%s)' % (unit, cat, CATS.get(cat, '?')))
    used = {s for (s,) in c.execute('SELECT slot FROM item WHERE unit_uid=? AND category=?', (unit, cat))}
    return [s for s in range(size[0]) if s not in used]


def insert_item(c, unit, cat, slot, item_id, count, enchant, usage, endurance):
    return c.execute(
        'INSERT INTO item(unit_uid,category,slot,item_id,count,equipped,endurance,enchant_level,seal_data,usage_type,'
        'item_state,period,attrib0,attrib1,attrib2,sockets,random_sockets) VALUES(?,?,?,?,?,?,?,?,0,?,1,0,0,0,0,\'\',\'\')',
        (unit, cat, slot, item_id, count, 1 if cat == ST_E_EQUIP else 0, endurance, enchant, usage)).lastrowid


def table(cols, rows, width=60):
    def cell(v):
        if v is None:
            return 'NULL'
        if isinstance(v, bytes):
            return '<blob %d B>' % len(v)
        s = str(v).replace('\n', '\\n')
        return s if len(s) <= width else s[:width - 1] + '~'
    rows = [[cell(v) for v in r] for r in rows]
    w = [max([len(c)] + [len(r[i]) for r in rows]) for i, c in enumerate(cols)]
    say('  '.join(c.ljust(w[i]) for i, c in enumerate(cols)).rstrip())
    say('  '.join('-' * x for x in w))
    for r in rows:
        say('  '.join(v.ljust(w[i]) for i, v in enumerate(r)).rstrip())


# ---- read commands -------------------------------------------------------------------------------------------

def cmd_info(a):
    c = open_ro(a)
    say('save    ' + db_path(a))
    say('game    ' + ('X2_offline.exe RUNNING (writes refused)' if game_running() else 'not running'))
    say('schema  user_version %d, journal %s' % (c.execute('PRAGMA user_version').fetchone()[0],
                                                  c.execute('PRAGMA journal_mode').fetchone()[0]))
    n = len(os.listdir(os.path.join(a.data, 'db_backup'))) if os.path.isdir(os.path.join(a.data, 'db_backup')) else 0
    say('backups %d in db_backup/' % n)
    tabs = [t for (t,) in c.execute("SELECT name FROM sqlite_master WHERE type='table' AND name NOT LIKE 'sqlite_%' ORDER BY name")]
    table(['table', 'rows'], [(t, c.execute('SELECT COUNT(*) FROM "%s"' % t).fetchone()[0]) for t in tabs])


def cmd_schema(a):
    c = open_ro(a)
    q = "SELECT name, sql FROM sqlite_master WHERE sql IS NOT NULL" + (" AND name=?" if a.table else '') + " ORDER BY type, name"
    rows = c.execute(q, (a.table,) if a.table else ()).fetchall()
    if not rows:
        die('no table %r' % a.table)
    for n, s in rows:
        say(s + ';\n')


def cmd_q(a):
    c = open_ro(a)
    cur = c.execute(a.sql)
    if cur.description is None:
        say('(statement returned no rows)')
        return
    cols = [d[0] for d in cur.description]
    rows = cur.fetchmany(a.limit + 1) if a.limit else cur.fetchall()
    more = bool(a.limit) and len(rows) > a.limit
    rows = rows[:a.limit] if a.limit else rows
    if a.json:
        say(json.dumps([dict(zip(cols, [v if not isinstance(v, bytes) else v.hex() for v in r])) for r in rows], ensure_ascii=False))
    elif a.tsv:
        say('\t'.join(cols))
        for r in rows:
            say('\t'.join('' if v is None else (v.hex() if isinstance(v, bytes) else str(v)) for v in r))
    else:
        table(cols, rows)
        say('(%d row%s%s)' % (len(rows), '' if len(rows) == 1 else 's', ', more not shown - raise --limit, 0 = all' if more else ''))


def cmd_chars(a):
    c = open_ro(a)
    rows = c.execute('SELECT u.unit_uid, u.user_uid, u.unit_class, u.nickname, u.level, u.ed, u.sp, '
                     '(SELECT COUNT(*) FROM item i WHERE i.unit_uid=u.unit_uid), u.del_date<>u.reg_date '
                     'FROM unit u ORDER BY u.unit_uid').fetchall()
    table(['uid', 'acct', 'class', 'name', 'nick', 'lv', 'ed', 'sp', 'items', 'deleted'],
          [(r[0], r[1], r[2], class_name(r[2]), r[3], r[4], r[5], r[6], r[7], 'DEL' if r[8] else '') for r in rows])
    acct = c.execute('SELECT user_uid, unit_slots FROM account').fetchall()
    say('(%d units; account slots: %s)' % (len(rows), ', '.join('acct %d = %d' % x for x in acct)))


def cmd_inv(a):
    c = open_ro(a)
    uid, nick = resolve_unit(c, a.unit)
    q = ('SELECT item_uid, category, slot, item_id, count, equipped, enchant_level, endurance, usage_type, period, sockets '
         'FROM item WHERE unit_uid=?' + (' AND category=?' if a.cat else '') + ' ORDER BY category, slot')
    rows = c.execute(q, (uid, a.cat) if a.cat else (uid,)).fetchall()
    say('unit %d %s' % (uid, nick))
    table(['item_uid', 'cat', 'slot', 'item_id', 'name', 'n', 'enc', 'end', 'sockets'],
          [(r[0], '%d %s' % (r[1], CATS.get(r[1], '?')), r[2], r[3], item_name(a, r[3]), r[4], r[6], r[7], r[10] or '') for r in rows])
    sizes = c.execute('SELECT category, size FROM inventory_size WHERE unit_uid=? ORDER BY category', (uid,)).fetchall()
    say('(%d items; bag sizes: %s)' % (len(rows), ', '.join('%d=%d' % x for x in sizes)))


def cmd_backups(a):
    d = os.path.join(a.data, 'db_backup')
    fs = sorted((f for f in os.listdir(d) if f.endswith('.sql')), key=lambda f: os.path.getmtime(os.path.join(d, f)), reverse=True) \
        if os.path.isdir(d) else []
    for f in fs[:a.limit]:
        p = os.path.join(d, f)
        say('%s  %6.1f MB  %s' % (time.strftime('%Y-%m-%d %H:%M', time.localtime(os.path.getmtime(p))), os.path.getsize(p) / 1e6, f))
    say('(%d backups, newest first%s; restore by hand: close the game, copy one over els_db.sql, delete -wal/-shm)'
        % (len(fs), '' if len(fs) <= a.limit else ', showing %d' % a.limit))


def cmd_backup(a):
    c = open_ro(a)
    backup(a, c, a.tag)


# ---- write commands ------------------------------------------------------------------------------------------

DDL = re.compile(r'^\s*(DROP|ALTER|CREATE)\b', re.I)
BULK = re.compile(r'^\s*(UPDATE|DELETE)\b(?!.*\bWHERE\b)', re.I | re.S)


def split_sql(text):
    """Split on ';' where sqlite says the statement is complete (so ';' inside strings/triggers survives)."""
    out, cur = [], ''
    for ch in text:
        cur += ch
        if ch == ';' and sqlite3.complete_statement(cur):
            out.append(cur.strip())
            cur = ''
    if cur.strip():
        out.append(cur.strip())
    return out


def cmd_x(a):
    text = open(a.file, encoding='utf-8').read() if a.file else a.sql
    if not text:
        die('give SQL or --file')
    comment = re.compile(r'--[^\n]*')
    stmts = [s for s in split_sql(text) if comment.sub('', s).strip(' \n\r\t;')]
    for s in stmts:
        if DDL.match(s) and not a.allow_ddl:
            die('refusing DDL (%s...): the emulator migrates the schema itself; --allow-ddl to override' % s[:30])
        if BULK.match(s) and not a.allow_all:
            die('refusing %s with no WHERE (%s...); --allow-all if you mean every row' % (s.split()[0].upper(), s[:40]))
    with Writer(a, 'sql') as c:
        for s in stmts:
            cur = c.execute(s)
            if cur.description is not None:
                rows = cur.fetchall()
                say('%s -> %d row(s) returned' % (s[:60], len(rows)))
                table([d[0] for d in cur.description], rows[:50])
            else:
                say('%s -> %d row(s) changed' % (s[:60].replace('\n', ' '), cur.rowcount))


def cmd_addchar(a):
    cmd = [sys.executable, os.path.join(HERE, 'createchar.py'), a.nickname, a.unit_class, str(a.level), '--data', a.data]
    for flag in ('sp', 'ed'):
        if getattr(a, flag) is not None:
            cmd += ['--' + flag, str(getattr(a, flag))]
    if a.dry_run:
        cmd.append('--dry-run')
    sys.exit(subprocess.run(cmd).returncode)


def cmd_setchar(a):
    import createchar
    sets = {}
    if a.level is not None:
        sets['level'] = a.level
        sets['exp'] = a.exp if a.exp is not None else createchar.exp_for(a.level)  # June TOTAL_EXP (Atlas A2), as addchar
    elif a.exp is not None:
        sets['exp'] = a.exp
    for f in ('ed', 'sp'):
        if getattr(a, f) is not None:
            sets[f] = getattr(a, f)
    if a.nick is not None:
        if not re.fullmatch(r'[A-Za-z0-9]{2,16}', a.nick):
            die('nickname must be 2-16 ASCII letters/digits')
        sets['nickname'] = a.nick
    if a.unit_class is not None:
        sets['unit_class'] = createchar.resolve_class(a.unit_class)
    if not sets:
        die('nothing to set (--level --exp --ed --sp --nick --class)')
    with Writer(a, 'setchar') as c:
        uid, nick = resolve_unit(c, a.unit)
        if 'nickname' in sets and c.execute('SELECT 1 FROM unit WHERE nickname=? COLLATE NOCASE AND unit_uid<>?', (a.nick, uid)).fetchone():
            die('nickname %r is already taken' % a.nick)
        before = c.execute('SELECT unit_class,nickname,level,exp,ed,sp FROM unit WHERE unit_uid=?', (uid,)).fetchone()
        c.execute('UPDATE unit SET %s WHERE unit_uid=?' % ', '.join('%s=?' % k for k in sets), list(sets.values()) + [uid])
        after = c.execute('SELECT unit_class,nickname,level,exp,ed,sp FROM unit WHERE unit_uid=?', (uid,)).fetchone()
        say('unit %d (%s)' % (uid, nick))
        for n, b, f in zip(('class', 'nick', 'level', 'exp', 'ed', 'sp'), before, after):
            if b != f:
                say('  %-6s %s -> %s' % (n, b, f))


def parse_item_args(items):
    out = []
    for t in items:
        m = re.fullmatch(r'(\d+)(?::(\d+))?', t)
        if not m:
            die('item %r: want <item_id> or <item_id>:<count>' % t)
        out.append((int(m.group(1)), int(m.group(2) or 1)))
    return out


def cmd_additem(a):
    wants = parse_item_args(a.items)
    with Writer(a, 'additem') as c:
        uid, nick = resolve_unit(c, a.unit)
        say('unit %d (%s)' % (uid, nick))
        for item_id, count in wants:
            r, itype, ptype, epos = item_rec(a, item_id)
            name = r['name']
            quantity = ptype == 'PT_QUANTITY'
            stack = r['quantity'] if quantity and r['quantity'] > 0 else 1
            usage = {'PT_INFINITY': 0, 'PT_ENDURANCE': 1, 'PT_QUANTITY': 2}.get(ptype, 0)
            endurance = r['endurance'] if ptype == 'PT_ENDURANCE' else 0

            if a.equip:  # straight into the worn category at the slot that takes this item's position
                fashion = bool(r['fashion'])
                slots = [s for s, (pos, f) in EQUIP_SLOTS.items() if pos == epos and (f is None or f == fashion)]
                if a.slot is not None:
                    slots = [a.slot]
                if not slots:
                    die('item %d %s (%s, fashion=%d) has no worn slot (equip position %s)' % (item_id, name, itype, fashion, epos))
                slot = slots[0]
                size = c.execute('SELECT size FROM inventory_size WHERE unit_uid=? AND category=9', (uid,)).fetchone()
                if size is None or slot >= size[0]:
                    die('unit %d has no worn slot %d (category 9 size %s)' % (uid, slot, size[0] if size else None))
                old = c.execute('SELECT item_uid,item_id FROM item WHERE unit_uid=? AND category=9 AND slot=?', (uid, slot)).fetchone()
                if old:
                    if not a.replace:
                        die('worn slot %d already holds item %d (%s); --replace moves it to the bag' % (slot, old[1], item_name(a, old[1])))
                    orec, otype, _, _ = item_rec(a, old[1])
                    ocat = ST_PC_BANG if orec['is_pc_bang'] else ST_AVATAR if orec['fashion'] else TYPE_TO_CAT.get(otype)
                    free = free_slots(c, uid, ocat) if ocat else []
                    if not free:
                        die('no free bag slot to move item %d (%s) into' % (old[1], item_name(a, old[1])))
                    c.execute('UPDATE item SET category=?, slot=?, equipped=0 WHERE item_uid=?', (ocat, free[0], old[0]))
                    say('  moved old %d %s -> bag %d slot %d' % (old[1], item_name(a, old[1]), ocat, free[0]))
                iu = insert_item(c, uid, ST_E_EQUIP, slot, item_id, 1, a.enchant, usage, endurance)
                say('  %d %s -> worn slot %d (item_uid %d)' % (item_id, name, slot, iu))
                continue

            cat = a.cat or (ST_PC_BANG if r['is_pc_bang'] else ST_AVATAR if r['fashion'] else TYPE_TO_CAT.get(itype))
            if not cat:
                die('item %d %s is %s: the emulator has no bag for that type (SuitableCategory -> none); use --cat N to force one'
                    % (item_id, name, itype))
            remaining, placed = count, []
            if quantity:  # top up existing stacks, lowest slot first (KInventory::FillTheUnFullSlot)
                for iu, slot, n in c.execute('SELECT item_uid,slot,count FROM item WHERE unit_uid=? AND category=? AND item_id=? ORDER BY slot',
                                             (uid, cat, item_id)).fetchall():
                    if remaining <= 0:
                        break
                    add = min(remaining, stack - n)
                    if add > 0:
                        c.execute('UPDATE item SET count=count+? WHERE item_uid=?', (add, iu))
                        remaining -= add
                        placed.append('+%d on slot %d' % (add, slot))
            free = [a.slot] if a.slot is not None else free_slots(c, uid, cat)
            if a.slot is not None and c.execute('SELECT 1 FROM item WHERE unit_uid=? AND category=? AND slot=?', (uid, cat, a.slot)).fetchone():
                die('bag %d slot %d is occupied' % (cat, a.slot))
            while remaining > 0 and free:
                s = free.pop(0)
                n = min(remaining, stack) if quantity else 1
                insert_item(c, uid, cat, s, item_id, n, a.enchant, usage, endurance)
                remaining -= n
                placed.append('%dx on slot %d' % (n, s))
            if remaining > 0:
                die('bag %d (%s) is full: %d of %d x item %d had nowhere to go (nothing written)'
                    % (cat, CATS.get(cat, '?'), remaining, count, item_id))
            say('  %d %s x%d -> bag %d %s: %s' % (item_id, name, count, cat, CATS.get(cat, '?'), ', '.join(placed)))


def cmd_delitem(a):
    with Writer(a, 'delitem') as c:
        uid, nick = resolve_unit(c, a.unit)
        say('unit %d (%s)' % (uid, nick))
        for t in a.ids:
            col = 'item_id' if a.by_item_id else 'item_uid'
            rows = c.execute('SELECT item_uid,category,slot,item_id,count FROM item WHERE unit_uid=? AND %s=?' % col, (uid, int(t))).fetchall()
            if not rows:
                die('unit %d has no item with %s=%s (nothing written)' % (uid, col, t))
            for r in rows:
                c.execute('DELETE FROM item WHERE item_uid=?', (r[0],))
                say('  deleted item_uid %d: %d %s x%d from bag %d slot %d' % (r[0], r[3], item_name(a, r[3]), r[4], r[1], r[2]))


def cmd_setting(a):
    if a.name is None:
        c = open_ro(a)
        table(['name', 'value'], c.execute('SELECT name, value FROM settings ORDER BY name').fetchall())
        return
    if a.value is None:
        c = open_ro(a)
        r = c.execute('SELECT value FROM settings WHERE name=?', (a.name,)).fetchone()
        say('%s = %s' % (a.name, r[0] if r else '(not set)'))
        return
    with Writer(a, 'setting') as c:
        old = c.execute('SELECT value FROM settings WHERE name=?', (a.name,)).fetchone()
        c.execute('INSERT OR REPLACE INTO settings(name,value) VALUES(?,?)', (a.name, a.value))
        say('%s: %s -> %d' % (a.name, old[0] if old else '(not set)', a.value))


def main():
    ap = argparse.ArgumentParser(description=__doc__.split('\n')[0], formatter_class=argparse.RawDescriptionHelpFormatter, epilog=__doc__)
    ap.add_argument('--data', default=DATA, help='game data directory (default: %(default)s)')
    sub = ap.add_subparsers(dest='cmd', required=True)

    def add(name, fn, writes=False):
        s = sub.add_parser(name)
        s.set_defaults(fn=fn)
        if writes:
            s.add_argument('--dry-run', action='store_true', help='do everything, then roll back (no backup is taken)')
        return s

    add('info', cmd_info)
    s = add('schema', cmd_schema); s.add_argument('table', nargs='?')
    s = add('q', cmd_q); s.add_argument('sql'); s.add_argument('--tsv', action='store_true'); s.add_argument('--json', action='store_true')
    s.add_argument('--limit', type=int, default=200, help='rows to print, 0 = all (default 200)')
    add('chars', cmd_chars)
    s = add('inv', cmd_inv); s.add_argument('unit'); s.add_argument('--cat', type=int); s.add_argument('--kim', default='staged')
    s = add('backups', cmd_backups); s.add_argument('--limit', type=int, default=15)
    s = add('backup', cmd_backup); s.add_argument('--tag', default='manual')

    s = add('x', cmd_x, True); s.add_argument('sql', nargs='?'); s.add_argument('--file')
    s.add_argument('--allow-ddl', action='store_true'); s.add_argument('--allow-all', action='store_true')
    s = add('addchar', cmd_addchar, True); s.add_argument('nickname'); s.add_argument('unit_class'); s.add_argument('level', type=int)
    s.add_argument('--sp', type=int); s.add_argument('--ed', type=int)
    s = add('setchar', cmd_setchar, True); s.add_argument('unit')
    s.add_argument('--level', type=int); s.add_argument('--exp', type=int); s.add_argument('--ed', type=int)
    s.add_argument('--sp', type=int); s.add_argument('--nick'); s.add_argument('--class', dest='unit_class')
    s = add('additem', cmd_additem, True); s.add_argument('unit'); s.add_argument('items', nargs='+', help='<item_id> or <item_id>:<count>')
    s.add_argument('--enchant', type=int, default=0); s.add_argument('--equip', action='store_true', help='put it in the worn slot, not the bag')
    s.add_argument('--replace', action='store_true', help='with --equip: move whatever is worn there to the bag')
    s.add_argument('--cat', type=int, help='force the bag (default: from the kim type)'); s.add_argument('--slot', type=int)
    s.add_argument('--kim', default='staged')
    s = add('delitem', cmd_delitem, True); s.add_argument('unit'); s.add_argument('ids', nargs='+')
    s.add_argument('--by-item-id', action='store_true', help='ids are item_ids (every copy), not item_uids'); s.add_argument('--kim', default='staged')
    s = add('setting', cmd_setting, True); s.add_argument('name', nargs='?'); s.add_argument('value', nargs='?', type=int)

    a = ap.parse_args()
    if sys.stdout.encoding and sys.stdout.encoding.lower() != 'utf-8':
        sys.stdout.reconfigure(errors='replace')
    if not hasattr(a, 'kim'):
        a.kim = 'staged'
    try:
        a.fn(a)
    except sqlite3.Error as e:
        die('sqlite: %s' % e)


if __name__ == '__main__':
    main()
