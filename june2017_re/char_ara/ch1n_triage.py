"""CH1n: join the CX2GUUser callbacks Ara's registration names (ch0_reg.tsv, targets outside her range)
with the earlier characters' CH1n triages by June VA. Writes ch1n_user_callback_triage.tsv."""
import csv, os, re
HERE = os.path.dirname(os.path.abspath(__file__))
LO, HI = 0x32b90b70, 0x32bd86e0
vas = {}
for r in csv.DictReader(open(os.path.join(HERE, 'ch0_reg.tsv'), encoding='utf-8'), delimiter='\t'):
    for slot, thunk, tgt in re.findall(r'(\w+)=([0-9a-f]+)>([0-9a-f]+)', r['callbacks']):
        v = int(tgt, 16)
        if not (LO <= v <= HI):
            vas.setdefault(tgt, set()).add(f"{r['fn']}:{r['table']}.{slot}")
known = {}
for ch in ('char_raven', 'char_chung', 'char_elsword', 'char_rena'):
    p = os.path.join(HERE, '..', ch, 'ch1n_user_callback_triage.tsv')
    for r in csv.DictReader(open(p, encoding='utf-8'), delimiter='\t'):
        va = r['june_va'].lower()
        if va in known:
            continue
        if 'outcome' in r:
            known[va] = (r['member'], r['covered_by'] if ch == 'char_raven' else f'{ch}/ch1n_user_callback_triage.tsv', r['verdict'], r['outcome'][:80])
        else:
            known[va] = (r['member'], f'{ch}/ch1n_user_callback_triage.tsv', r['verdict'], r['detail'][:80])
rows, missing = [], []
for va in sorted(vas):
    if va in known:
        m, src, v, o = known[va]
        rows.append((m, va, src, v, o, len(vas[va])))
    elif all(x.startswith('3202b110:USI_EVENT_') for x in vas[va]):
        rows.append(('-', va, 'Effect C9h-4h', 'n/a', 'registered by shared InitEventStates 3202b110 (SERV_IRUHADEV_JUNE_EVENT_STATES)', len(vas[va])))
    else:
        missing.append((va, sorted(vas[va])[:4]))
with open(os.path.join(HERE, 'ch1n_user_callback_triage.tsv'), 'w', encoding='utf-8', newline='\n') as f:
    f.write('member\tjune_va\tcovered_by\tverdict\toutcome\tara_slots\n')
    for r in sorted(rows):
        f.write('\t'.join(map(str, r)) + '\n')
    for va, sl in missing:
        f.write(f'?\t{va}\tCH1n\tNEW\t{" ".join(sl)}\t\n')
from collections import Counter
print('targets', len(vas), 'rows', len(rows), 'missing', len(missing))
print(Counter(r[3] for r in rows))
print(Counter(r[4] if r[3]!='SAME' else 'same' for r in rows).most_common())
for m in missing: print('MISSING', m)
