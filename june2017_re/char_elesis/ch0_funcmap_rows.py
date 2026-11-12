"""char-elesis CH0: append the `Char Elesis CH0` rows to june2017_re/funcmap.tsv (idempotent: skips when the tag is already there).
    python june2017_re/char_elesis/ch0_funcmap_rows.py [--dry]
Rows: the CX2GUEL vtable overrides (ch0_vtable_slots.tsv + the class map's names / phases), the InitState wrapper 32b87a60 and its two parts,
SetEquippedSkillFuncBySkillSlotIndex 32b8c470, SetEquippedSkillLevel 32b4e790, the 7 thunk-table delegate adapters, and the identified
unmapped callees funcmap lacks (ch0_unmapped_identified.tsv). Append only.
"""
import os, re, sys
HERE = os.path.dirname(os.path.abspath(__file__))
FM = os.path.join(HERE, '..', 'funcmap.tsv')
TAG = 'Char Elesis CH0'
C = 'CX2GUEL::'


def tsv(name):
    p = os.path.join(HERE, name)
    return [l.rstrip('\r\n').split('\t') for l in open(p, encoding='utf-8') if l.strip() and l[0] != '#']


cm = {r[0]: r for r in tsv('ch0_class_map.tsv') if r[0] != 'june_fn'}
slots = [r for r in tsv('ch0_vtable_slots.tsv') if r[0] != 'slot_off']
ui = [r for r in tsv('ch0_unmapped_identified.tsv') if r[0] != 'callee']

# hand rows: address -> (march symbol, status, evidence)
HAND = {
 '32b87a60': (C + 'InitState wrapper (vslot 0x7a4)', 'confirmed', 'class registrar entry: calls the parts 32b81be0 (111 names) and 32b864f0 (57 names) and base 3202b110 (ch0c_listing_check 177 / 177); phase CH1m'),
 '32b81be0': (C + 'InitState part 1 (registrar)', 'confirmed', '111 registered names (incl. the hyper row id 23); called by the wrapper 32b87a60; none in March as a function; phase CH1m'),
 '32b864f0': (C + 'InitState part 2 (registrar)', 'confirmed', '57 registered names; called by the wrapper 32b87a60; none in March as a function; phase CH1m'),
 '32b8c470': (C + 'SetEquippedSkillFuncBySkillSlotIndex (vslot 0x7b4)', 'confirmed', 'four switches: (a) 31 entries from 28031, (b) types 5..16, (c) 28000..28236 237 entries 55 targets, (d) 28601..28605; compares 28600 / 900; phase CH1n'),
 '32b4e790': (C + 'SetEquippedSkillLevel (vslot 0x7b8)', 'confirmed', '22 id blocks; phase CH1i'),
 '32b8ecf0': (C + 'InitPassiveSkillState delegate adapter (thunk table)', 'confirmed', 'push edx; call 32b52470; ret; pushed by InitPassiveSkillState 32b88300 @32b88342; delegate 32b52470; phase CH4a'),
 '32b8ed00': (C + 'InitPassiveSkillState delegate adapter (thunk table)', 'confirmed', 'push edx; call 32b524b0; ret; pushed by InitPassiveSkillState 32b88300 @32b88a76; delegate 32b524b0; phase CH4a'),
 '32b8ffd0': (C + 'InitPassiveSkillState delegate adapter (thunk table)', 'confirmed', 'push edx; call 32b6fd50; ret; pushed by InitPassiveSkillState 32b88300 @32b8835e; delegate 32b6fd50; phase CH4a'),
 '32b8ffe0': (C + 'InitPassiveSkillState delegate adapter (thunk table)', 'confirmed', 'push edx; call 32b6f820; ret; pushed by InitPassiveSkillState 32b88300 @32b88aac; delegate 32b6f820; phase CH4a'),
 '32b906d0': (C + 'InitPassiveSkillState delegate adapter (thunk table)', 'confirmed', 'push edx; call 32b7e3b0; ret; pushed by InitPassiveSkillState 32b88300 @32b8837a; delegate 32b7e3b0; phase CH4a'),
 '32b90ab0': (C + 'InitPassiveSkillState delegate adapter (thunk table)', 'confirmed', 'push edx; call 32b872d0; ret; pushed by InitPassiveSkillState 32b88300 @32b88c21; delegate 32b872d0; phase CH4a'),
 '32b90ac0': (C + 'InitPassiveSkillState delegate adapter (thunk table)', 'confirmed', 'push edx; call 32b87880; ret; pushed by InitPassiveSkillState 32b88300 @32b8903c; delegate 32b87880; phase CH4a (adapter), delegate body CH4c'),
}


def rows():
    out = []
    seen = set()
    for a, (n, s, e) in HAND.items():
        out.append((a, n, s, '%s: %s' % (TAG, e)))
        seen.add(a)
    for r in slots:
        fn = r[2]
        if fn in seen:
            continue
        seen.add(fn)
        c = cm.get(fn)
        name, march, ph, ins, note, src = r[4], r[6], '?', '?', '', 'slots'
        if c:
            march, ph, ins = c[3], c[6], c[4]
            for k in (2, 3, 5):
                x = c[k].split(' ')[0] if k == 3 else c[k]
                if x and x[0] not in '?-':
                    name, src = x, ('class map' if k != 5 else 'proposed')
                    break
            else:
                name = None
            note = re.sub(r'^base [0-9a-f]+; ', '', c[7]) if len(c) > 7 else ''
        if not name or name[0] in '?-':
            name = None
        if march.startswith('- (name unsettled)'):
            march = 'no March name settled'
        conf = bool(name) and (r[5].startswith('CONF') or src == 'class map')
        sym = C + name if name else 'CX2GUEL vslot %s (name unsettled)' % r[0]
        ev = '%s: override %s; %s; phase %s; %s; base %s' % (TAG, r[0], march, ph, ins, r[3])
        if not conf and note:
            ev += '; ' + (note if len(note) <= 160 else note[:160].rsplit(' ', 1)[0])
        out.append((fn, sym, 'confirmed' if conf else 'candidate', ev))
    return out


def callee_rows(have):
    out = []
    for r in ui:
        if r[0] in have:
            continue
        ev = r[4] if len(r[4]) <= 400 else r[4][:400].rsplit(' ', 1)[0]
        used = r[8] if len(r) > 8 and r[8] and r[8] != '-' else (r[7] if len(r) > 7 else '')
        out.append((r[0], r[2] if r[2] != '-' else 'unidentified callee (see evidence)', r[3] if r[3] in ('confirmed', 'candidate') else 'candidate',
                    '%s: %s; used by %s' % (TAG, ev, used)))
    return out


def main():
    s = open(FM, 'rb').read()
    if TAG.encode() in s:
        print('already has %s rows' % TAG)
        return
    have = set()
    for l in s.decode('utf-8', 'replace').split('\n'):
        for t in l.split('\t')[0].split():
            have.add(t.lower())
    R = rows()
    R += callee_rows(have | {a for a, _, _, _ in R})
    add = ''.join('%s\t%s\t%s\t%s\n' % x for x in R)
    add = add.replace('\r', '')
    if '--dry' in sys.argv:
        print(add)
        print(len(R), 'rows')
        return
    if not s.endswith(b'\n'):
        s += b'\n'
    open(FM, 'wb').write(s + add.encode('utf-8'))
    print('funcmap.tsv: %d rows %s' % (len(R), TAG))


if __name__ == '__main__':
    main()
