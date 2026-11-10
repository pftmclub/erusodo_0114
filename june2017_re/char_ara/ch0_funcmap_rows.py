"""char-ara CH0: append the `Char Ara CH0` rows to june2017_re/funcmap.tsv (idempotent: skips when the tag is already there).
    python june2017_re/char_ara/ch0_funcmap_rows.py [--dry]
Rows: the 77 vtable overrides (ch0_vtable_slots.tsv + the class map's names / phases), the registrars 32bcf4a0 / 32bca580 / 32bcced0,
SetEquippedSkillFuncBySkillSlotIndex 32bd09d0, InitAquiredActiveSkillState 32bd34c0, the empty device-list appenders 32bd86b0 / 32bd86e0,
the unreferenced stub 32bd86c0, and the identified unmapped callees funcmap lacks (ch0_unmapped_identified.tsv). Append only.
"""
import os, re, sys
HERE = os.path.dirname(os.path.abspath(__file__))
FM = os.path.join(HERE, '..', 'funcmap.tsv')
TAG = 'Char Ara CH0'
C = 'CX2GUAra::'


def tsv(name):
    p = os.path.join(HERE, name)
    return [l.rstrip('\r\n').split('\t') for l in open(p, encoding='utf-8') if l.strip() and l[0] != '#']


cm = {r[0]: r for r in tsv('ch0_class_map.tsv') if r[0] != 'june_fn'}
slots = [r for r in tsv('ch0_vtable_slots.tsv') if r[0] != 'slot_off']
ui = [r for r in tsv('ch0_unmapped_identified.tsv') if r[0] != 'callee']

# hand rows: address -> (march symbol, status, evidence)
HAND = {
 '32bcf4a0': (C + 'InitState wrapper (vslot 0x7a4)', 'confirmed', 'class registrar entry: calls InitState body 32bca580 @32bcf53e, then the per-class part 32bcced0 and base 3202b110 (ch0_reg.tsv; ch0c_listing_check 162 / 162); phase CH1m'),
 '32bca580': (C + 'InitState body (registrar)', 'confirmed', '112 registrations + the hyper row id 23 + the berserk row id 155; called by the wrapper 32bcf4a0 @32bcf53e; none in March as a function; phase CH1m'),
 '32bcced0': (C + 'InitState per-class part (registrar)', 'confirmed', '41 registrations of the combo / quick-stand / brutal-piercing states; called from the InitState wrapper 32bcf4a0; none in March as a function; phase CH1m'),
 '32bd09d0': (C + 'SetEquippedSkillFuncBySkillSlotIndex (vslot 0x7b4)', 'confirmed', 'pre-switch @32bd0af5, type switch (byte 32bd282c / jump 32bd281c), id switch (id - 0x6978, 244 entries, byte 32bd2930 / jump 32bd2838), 27600 by compare @32bd2685, hyper switch (jump 32bd2a24); 86 cases call class helpers; ch0_skill_listing_rows.tsv; phase CH1n'),
 '32bd34c0': (C + 'InitAquiredActiveSkillState (vslot 0x68c)', 'confirmed', 'second skill switch: 56 states (id - 0x6979, 242 entries, byte table 32bd5150 / jump table 32bd50f4); March X2GUUser.h:3039, X2GUAra.cpp:16262, caller X2ComboAndSkillChainSystem.cpp:98; head phase CH1n, bodies with their state phases'),
 '32bd86b0': (C + 'AppendDeviceToDeviceList', 'confirmed', 'empty body in June and in March: nothing to port'),
 '32bd86e0': (C + 'AppendComponentToDeviceList', 'confirmed', 'empty body in June and in March: nothing to port'),
 '32bd86c0': ('none in March: unreferenced stub', 'confirmed', 'no reference anywhere in the exe (dead); sits between AppendDeviceToDeviceList and AppendComponentToDeviceList'),
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
        sym = C + name if name else 'CX2GUAra vslot %s (name unsettled)' % r[0]
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
