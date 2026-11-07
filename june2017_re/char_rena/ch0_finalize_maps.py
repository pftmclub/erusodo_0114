r"""char-rena CH0 items 7 / 8: ch0_nf_draft.tsv (subagent digest, spot-checked) -> ch0_nf.tsv with an owner column. Copy of the idea of
char_raven/ch0_finalize_maps.py (rage / overheat), for Rena's natural force (ADD_RENA_SYSTEM).
    python june2017_re/char_rena/ch0_finalize_maps.py
Owner rules:
  * a row whose June counterpart is a class function (address in Rena's range) takes that function's ch0_class_map.tsv phase (the
    ADD_RENA_SYSTEM-only March functions are hand-routed to CH1q there, so their rows say CH1q);
  * gauge data / UI, the gauge manager, the emulator, the packet and the June engine callers of the class (ApplyHyperModeBuff 32298590,
    CX2GUNPC::AttackResultByType 32773ee0, the F7 training key 322ed400) -> CH1q2;
  * every other engine arm the flag flips (X2GUUser, X2DamageManager, X2DamageEffect, X2EffectSet, X2Game, X2Define) -> CH1q;
  * "absent in June" -> n/a (absent in June: CH1q drops it or keeps it dead under the flag, never live).
"""
import os, re, bisect
HERE = os.path.dirname(os.path.abspath(__file__))
LO, HI = 0x32c6a8d0, 0x32cb6c70
cm = {}
for l in open(os.path.join(HERE, 'ch0_class_map.tsv'), encoding='utf-8', errors='replace'):
    c = l.rstrip('\n').split('\t')
    if len(c) > 6 and re.match(r'^[0-9a-f]{8}$', c[0]):
        cm[int(c[0], 16)] = c[6]
S = sorted(cm)
Q2_FILES = ('X2GageData', 'X2GageUI', 'X2GageManager', 'X2OfflineStatTable', 'CommonPacket.h', 'ServerDefine.h')
Q2_ADDR = re.compile(r'\b(3249[0-9a-f]{4}|32a3[0-9a-f]{4}|32e[5-7][0-9a-f]{4}|3248e[0-9a-f]{3}|32773ee0|327745[0-9a-f]{2}|32298[0-9a-f]{3}|322ed[0-9a-f]{3})\b')


def class_phase(text):
    for a in re.findall(r'\b(32c[6-9a-f][0-9a-f]{4}|32cb[0-6][0-9a-f]{3})\b', text):
        v = int(a, 16)
        if LO <= v < HI:
            i = bisect.bisect_right(S, v) - 1
            if i >= 0:
                return cm[S[i]], a
    return None, None


rows = [l.rstrip('\n').split('\t') for l in open(os.path.join(HERE, 'ch0_nf_draft.tsv'), encoding='utf-8', errors='replace') if l.strip()]
out = ['\t'.join(rows[0] + ['owner'])]
tally = {}
for r in rows[1:]:
    r = (r + [''] * 7)[:7]
    f, june = r[0], r[4]
    ph, a = class_phase(june)
    if june.startswith('absent'):
        own = 'n/a (absent in June)' if 'training' not in june else 'CH1q2'   # the F7 key row: June's own key 322ed400 is CH1q2's
    elif any(x in f for x in Q2_FILES) or (Q2_ADDR.search(june) and not ph):
        own = 'CH1q2'
    elif ph:
        own = ph
    else:
        own = 'CH1q'
    tally[own] = tally.get(own, 0) + 1
    out.append('\t'.join(r + [own]))
open(os.path.join(HERE, 'ch0_nf.tsv'), 'w', encoding='utf-8', newline='\n').write('\n'.join(out) + '\n')
print('ch0_nf.tsv rows %d; owners: %s' % (len(out) - 1, ', '.join('%s %d' % kv for kv in sorted(tally.items(), key=lambda kv: -kv[1]))))
