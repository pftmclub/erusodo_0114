"""Phase 11: add the test characters the name-contract play-test needs, straight into els_db.sql.

    python phase11_test_units.py            # dry run: prints what it would insert
    python phase11_test_units.py --apply    # backs up els_db.sql into db_backup/, then inserts

Refuses to run while X2_offline.exe is running (the emulator holds the DB open).
Mirrors CX2OfflineDB::CreateUnit (X2OfflineDB.cpp:1199): one unit row plus the default
inventory_size rows (SeedInventorySizes), copied from an untouched level-1 unit so they are the
current BaseSlotSize values. Level 80 with the level-80 exp the existing characters carry, and
SP to learn the skills in game. Skills and hotkeys are set in game.
"""
import os
import sqlite3
import subprocess
import sys
import time

DATA = r'F:\elsword stuff\elsword_2014\els_2014\237311\24965799\data'
DB = os.path.join(DATA, 'els_db.sql')

# (unit_class, nickname, why). CX2Unit::UNIT_CLASS values, X2Unit.h.
UNITS = [
    (100, 'jtLordKnight', 'ELK Sand Storm -> LORD_KNIGHT_SAND_STORM (X2GUElsword_SwordMan.cpp:15421)'),
    # 2026-10-03: first inserted as 101 jtRuneSlayer by mistake; Double Slash is Sheath Knight's.
    (112, 'jtInfinitySword', 'ESK Double Slash in awakening -> EffectSet_ELSWORD_DOUBLE_SLASH_HYPER '
                             '(ESSI_SA_ESK_DOUBLE_SLASH_HYPER_FrameMove, X2GUElsword_SwordMan.cpp:15239)'),
    (109, 'jtCodeEmpress', 'dash combo Z -> EVE_EP_DASH_COMBO_Z (X2GuEve.cpp:11409)'),
    (105, 'jtVoidPrincess', 'ADM Death Field -> EffectSet_Aisha_DeathField_* '
                            '(AVSI_SA_ADM_DEATHFIELD_FrameMove, X2GUArme_VioletMage.cpp:15620)'),
]
LEVEL, EXP, SP, ED = 80, 866438000, 600, 1000000


def running():
    out = subprocess.run(['tasklist', '/FI', 'IMAGENAME eq X2_offline.exe'], capture_output=True,
                         text=True).stdout
    return 'X2_offline.exe' in out


def main(apply):
    if running():
        raise SystemExit('X2_offline.exe is running - close the game first.')
    c = sqlite3.connect(DB)
    user = c.execute('SELECT user_uid FROM account ORDER BY user_uid LIMIT 1').fetchone()[0]
    seed = c.execute('SELECT u.unit_uid FROM unit u WHERE u.level = 1 ORDER BY u.unit_uid LIMIT 1').fetchone()
    if seed is None:
        raise SystemExit('no level-1 unit to copy default inventory sizes from')
    sizes = c.execute('SELECT category, size FROM inventory_size WHERE unit_uid = ?', seed).fetchall()
    have = {r[0] for r in c.execute('SELECT nickname FROM unit')}
    todo = [u for u in UNITS if u[1] not in have]
    for u in todo:
        print('insert class %d %s  (%s)' % u)
    print('inventory_size seed from unit %d: %s' % (seed[0], sizes))
    if not apply or not todo:
        return
    bak = os.path.join(DATA, 'db_backup', 'els_db_%s_pre_phase11_units.sql' % time.strftime('%Y%m%d_%H%M%S'))
    os.makedirs(os.path.dirname(bak), exist_ok=True)
    dst = sqlite3.connect(bak)
    c.backup(dst)
    dst.close()
    print('backup', bak)
    now = int(time.time())
    with c:
        for cls, nick, _ in todo:
            cur = c.execute('INSERT INTO unit( user_uid, unit_class, nickname, level, exp, ed, sp, spirit, '
                            'last_pos, reg_date, del_date, last_date ) '
                            'VALUES( ?, ?, ?, ?, ?, ?, ?, 0, 20000, ?, ?, ? )',
                            (user, cls, nick, LEVEL, EXP, ED, SP, now, now, now))
            uid = cur.lastrowid
            c.executemany('INSERT OR REPLACE INTO inventory_size( unit_uid, category, size ) VALUES( ?, ?, ? )',
                          [(uid, cat, size) for cat, size in sizes])
            print('inserted unit_uid %d class %d %s' % (uid, cls, nick))
    c.close()


if __name__ == '__main__':
    main('--apply' in sys.argv)
