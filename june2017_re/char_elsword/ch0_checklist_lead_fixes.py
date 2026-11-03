r"""char-elsword CH0: the lead's verdicts on ch0_checklist_rows.tsv rows the draft left open (`?`) or that the class map changed.
    python june2017_re/char_elsword/ch0_checklist_lead_fixes.py
Idempotent: sets col 5 (phase) and col 6 (note) of the rows keyed by col 2 (checklist line).
"""
import os

HERE = os.path.dirname(os.path.abspath(__file__))
P = os.path.join(HERE, 'ch0_checklist_rows.tsv')

FIX = {
    'L256': ('CH1q', "32d058a0 (0x740) saves GetWayOfSwordPoint at +0x92b8, 32d058d0 (0x744) restores it via SetWayOfSwordPoint (ch0_wos.tsv); "
                     "the gauge state is re-derived by WayOfSwordStateChange. Crest of Time's cast stays char-add"),
    'L352': ('CH1g CH1h CH1q CH2d CH3c', "per function (ch0_func_rows.tsv x ch0_class_map.tsv col 7): CH1g 3, CH1h 1 (32d0d7a0), CH1q 1 (32d0f300 x2), "
                                         "CH2d 1 (32d14100 x9), CH3c 2; 6 functions (32d0d660, 32d10610, 32d108e0, 32d10a00, 32d10cd0, 32d10df0 x7; 12 sites) "
                                         "are dead in June (no call, jmp, pointer bytes or Ghidra xref): n/a"),
    'L420': ('CH1d2 CH1e CH1f CH2b CH3a CH3b CH3c', "per function (ch0_func_rows.tsv x ch0_class_map.tsv col 7): CH1d2 1, CH1e 10, CH1f 7, CH2b 1, "
                                                    "CH3a 2, CH3b 2, CH3c 1. A [reg+0x8ca4] byte scan finds 24 functions (48 sites), not 25"),
    'L549': ('n/a', "June registers no ESSI_THROW_WOODEN_PIECE (dead table, not in 32d1f8f0 / 32d23ff0): CH1m drops March's registration and "
                    "the 1.5 stop goes with it (c3e3 row 23)"),
    'L827': ('CH1q2', "UpdateElswordSkillSlotEffect (June driver 322cd5c0) reads the gauge state: ported with the gauge pieces; "
                      "CH1q2 reads it against X2Game.cpp:13535"),
    'L901': ('CH4a', "the 8 registrations in 32d1c560 are CH4a; bodies: 6 CH4a, 32d1b9d0 CH1h (class map; also called from 32d27641), "
                     "32d08ba0 CH1o (no phased caller)"),
    'L930': ('CH1q', "engine block in CX2GUUser::AttackResult 32012b50 reads the gauge state: CH1q with the gauge math"),
}

with open(P, 'rb') as f:
    raw = f.read()
nl = '\r\n' if b'\r\n' in raw else '\n'
lines = raw.decode('utf-8').split(nl)
out, hit = [], set()
for l in lines:
    c = l.split('\t')
    if len(c) > 5 and c[1] in FIX:
        c[4], c[5] = FIX[c[1]]
        hit.add(c[1])
        l = '\t'.join(c)
    out.append(l)
text = nl.join(out).encode('utf-8')
with open(P, 'wb') as f:
    f.write(text)
print('fixed', sorted(hit), 'not found', sorted(set(FIX) - hit))
