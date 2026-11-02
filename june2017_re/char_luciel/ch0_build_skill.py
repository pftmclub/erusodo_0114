r"""char-luciel CH0: build ch0_skill.tsv (the skill setter 32b46580, Chung's ch0_skill.tsv columns) from the listing emulator.

    python june2017_re/char_luciel/ch0_skill_listing_emu.py      (writes ch0_skill_listing_rows.tsv and _emu_outcome.pkl: which ids have an event)
    python june2017_re/char_luciel/ch0_build_skill.py

A case = one entry site: the first MakeTableReference call (32fe9a40) a skill id reaches. Every id that reaches the same entry site is one case
(a base id and its upgrade ids share it through m_DeleteSkillIDByUpgrade; 30035 / 30082 enter the V != 0 path at 32b466d7). Columns as Chung's:
fn, line (= entry call va), cases (skill ids), normal_table, hyper_table, normal_callbacks, hyper_callbacks, extra_states (id:table of every
other state the case registers through 31fd89b0), calls (delegate makers the case calls), extra_callbacks (id:callbacks of each extra state),
types (m_eType of the ids). Callbacks are slot=thunk>target.
"""
import os, sys, re, pickle, collections
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import ch0_lu_data as D
import ch0_skill_listing_emu as E

HERE = D.HERE


def main():
    ins, order = E.load(E.LISTING)
    tm = D.templets()
    ids, outcome, noev = pickle.load(open(os.path.join(HERE, '_emu_outcome.pkl'), 'rb'))
    live = [i for i in ids if i not in noev]
    cases = collections.OrderedDict()
    for sid in live:
        typs = [tm[sid][0]] if sid in tm and tm[sid][0] else [5, 6, 7, 12, 13, 14, 15, 16]
        seen = set()
        for V in (0, sid):
            for typ in typs:
                for r, res in E.run_id(ins, order, sid, typ, V, (0, 0), tm):
                    if not r.events:
                        continue
                    ev = r.events
                    entry = ev[0][1]
                    key = (entry, tuple((e[0], e[1], e[2]) for e in ev))
                    if key in seen:
                        continue
                    seen.add(key)
                    c = cases.setdefault(entry, {'ids': set(), 'variants': {}, 'types': set()})
                    c['ids'].add(sid)
                    c['types'].add(typ)
                    c['variants'].setdefault(key[1], (ev, dict(r.obj['NORM']), dict(r.obj['HYP'])))
    rows = []
    for entry, c in cases.items():
        # the longest variant gives the extra states; all variants are merged by (call va, name)
        mtrs, regs = [], collections.OrderedDict()
        for key, (ev, nrm, hyp) in c['variants'].items():
            for e in ev:
                if e[0] == 'mtr':
                    t = (e[3], e[2], E.cbstr(nrm if e[3] == 1 else hyp))
                    if t not in mtrs:
                        mtrs.append(t)
                else:
                    regs.setdefault((e[1], e[2], e[3]), E.cbstr(e[4]))
        nt = [m for m in mtrs if m[0] == 1]
        ht = [m for m in mtrs if m[0] == 2]
        ntab = ';'.join(dict.fromkeys(m[1] for m in nt if m[1]))
        htab = ';'.join(dict.fromkeys(m[1] for m in ht if m[1]))
        ncb = ' | '.join(dict.fromkeys(m[2] for m in nt))
        hcb = ' | '.join(dict.fromkeys(m[2] for m in ht))
        extra = ' '.join('%s:%s' % (sid_ if sid_ is not None else '-', nm) for (a, nm, sid_), cb in regs.items())
        xcb = ' | '.join('%s:%s' % (sid_, cb or '-') for (a, nm, sid_), cb in regs.items())
        rows.append((entry, sorted(c['ids']), ntab, htab, ncb, hcb, extra, xcb, sorted(c['types'])))
    rows.sort(key=lambda r: r[0])
    with open(os.path.join(HERE, 'ch0_skill.tsv'), 'w', encoding='utf-8', newline='\n') as o:
        o.write('fn\tline\tcases\tnormal_table\thyper_table\tnormal_callbacks\thyper_callbacks\textra_states\tcalls\textra_callbacks\ttypes\n')
        for entry, idl, ntab, htab, ncb, hcb, extra, xcb, types in rows:
            o.write('32b46580\t%08x\t%s\t%s\t%s\t%s\t%s\t%s\t-\t%s\t%s\n' % (entry, ','.join(map(str, idl)), ntab, htab, ncb, hcb, extra, xcb, ','.join(map(str, types))))
        allids = sorted({i for r in rows for i in r[1]})
        o.write('# cases (distinct entry sites) %d; skill ids with a case %d (%d in the Lu/Ciel tree or templets, %d other); ids tried %d, no event %d\n' % (
            len(rows), len(allids), sum(1 for i in allids if i in tm), sum(1 for i in allids if i not in tm), len(ids), len(noev)))
        o.write('# type switch: [templet+4] - 5 in 0..11 -> 32b4a834 table (4 targets): type 5 -> 32b4677d, 6 7 13-16 -> 32b469d6 (id compare tree), 12 -> 32b4a731, 8-11 -> exit; before it V = 31fdbcc0( id ) != 0 and 32207210( V ) == 0x7553 -> 32b466c5\n')
        o.write('# slot state ids (31fd88f0): Lu normal 334ed540 = 50 52 54 56 58 (USI_SPECIAL_ATTACK_1-5), Lu hyper 334ed554 = 51 53 55 57 59, Ciel normal 334ed568 = 60 62 64 66 68 (USI_SPECIAL_ATTACK_SLOTB_1-5), Ciel hyper 334ed57c = 61 63 65 67 69\n')
    print(len(rows), 'cases;', len(allids), 'ids with a case')
    print('multi-id cases:', sum(1 for r in rows if len(r[1]) > 1), 'single:', sum(1 for r in rows if len(r[1]) == 1))


if __name__ == '__main__':
    main()
