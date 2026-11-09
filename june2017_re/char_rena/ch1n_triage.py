"""CH1n: join the CX2GUUser callbacks Rena's registration names (ch1m_reg_by_class.tsv, targets outside her range)
with the earlier characters' CH1n triages by June VA. Writes ch1n_user_callback_triage.tsv."""
import csv, os, re
HERE = os.path.dirname(os.path.abspath(__file__))
LO, HI = 0x32c6a8d0, 0x32cb6c6d
vas = {}
for r in csv.DictReader(open(os.path.join(HERE, 'ch1m_reg_by_class.tsv'), encoding='utf-8'), delimiter='\t'):
    for slot, thunk, tgt in re.findall(r'(\w+)=([0-9a-f]+)>([0-9a-f]+)', r['callbacks']):
        v = int(tgt, 16)
        if not (LO <= v <= HI):
            vas.setdefault(tgt, set()).add(f"{r['fn']}:{r['table']}.{slot}")
known = {}
for ch in ('char_raven', 'char_chung', 'char_elsword'):
    p = os.path.join(HERE, '..', ch, 'ch1n_user_callback_triage.tsv')
    for r in csv.DictReader(open(p, encoding='utf-8'), delimiter='	'):
        va = r['june_va'].lower()
        if va in known:
            continue
        if ch == 'char_raven':
            known[va] = (r['member'], r['covered_by'], r['verdict'], r['outcome'])
        else:
            known[va] = (r['member'], f'{ch}/ch1n_user_callback_triage.tsv', r['verdict'], r['detail'][:80])
rows, missing = [], []
for va in sorted(vas):
    if va in known:
        m, src, v, o = known[va]
        rows.append((m, va, src, v, o, len(vas[va])))
    else:
        why = None
        sl = ' '.join(sorted(vas[va]))
        if 'RIDING' in sl or va == '32002f70':
            why = 'not registered: riding 93-96 / empty FrameMove, Stage 17 (CH1m comment lines)'
        elif va == '3202a7d0':
            why = 'not registered: die StateEnd runs in CX2GUUser::StateChange (CH1m comment lines)'
        elif 'PHOENIX' in sl:
            why = 'not registered: PvP amusement mode, no PvP offline (CH1m comment lines)'
        elif 'DANCE' in sl:
            why = 'not registered: dance state, Stage 10 (CH1m comment lines)'
        elif 'BOUNDING' in sl:
            why = 'registered by shared InitBoundingStates (SERV_IRUHADEV_JUNE_BOUNDING_STATE)'
        elif 'WAIT_STATE_CHANGE' in sl:
            why = 'registered by shared InitWaitStateChange (SERV_IRUHADEV_JUNE_WAIT_STATE_CHANGE)'
        elif va == '32adf320':
            why = 'vslot 0x600 thunk = Rena HyperModeFrameMove 32ca5310 (class override, CH1i)'
        if why:
            rows.append(('-', va, 'CH1m', 'n/a', why, len(vas[va])))
        else:
            missing.append((va, sorted(vas[va])[:3]))
with open(os.path.join(HERE, 'ch1n_user_callback_triage.tsv'), 'w', encoding='utf-8', newline='\n') as f:
    f.write('member\tjune_va\tcovered_by\tverdict\toutcome\trena_slots\n')
    for r in sorted(rows):
        f.write('\t'.join(map(str, r)) + '\n')
from collections import Counter
print('targets', len(vas), 'rows', len(rows), 'missing', len(missing))
print(Counter(r[3] for r in rows))
print(Counter(r[4] if r[3]!='SAME' else 'same' for r in rows).most_common())
for m in missing: print('MISSING', m)
