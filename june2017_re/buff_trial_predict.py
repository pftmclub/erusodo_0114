"""June buff port Phase 3: predict what March's buff loader does with June's buff files, and check a
trial's ErrorLog.txt against that prediction.

    python june2017_re/buff_trial_predict.py                     # prediction only
    python june2017_re/buff_trial_predict.py --log ErrorLog.txt  # + compare a trial's [JUNE_BUFF] lines
    python june2017_re/buff_trial_predict.py --source DIR        # other buff files: DIR/BattleFieldBuff*.src.lua
                                                                 # (e.g. a scratch decompile of March's live ones)

The prediction walks every entry of June's BattleFieldBuffTemplet(_INT) / BattleFieldBuffFactor(_INT)
(june_src_fixed) the way March's C++ does (X2BuffTemplet.cpp ParsingScript, X2BuffFactor.cpp
ParsingBuffFactor, the three templet factories), with the type lists buff_census.py resolves from the
C++ under US_SERVICE flags, and names resolved through the staged March Enum (the one the trial loads):

  templet:  BUFF_TEMPLET_ID, IS_WORLD_BUFF, IS_DEBUF, ACCUMULATION_LIMIT (+ ACCUMULATION_MULTIPLIER when > 1),
            REGIST_TYPE required; BEHAVIOR and FINALIZER tables required, DISPLAYER optional. Per section,
            COMBINATION in order (a nil member ends the list); BBT_EMPTY returns at once (templet factory);
            an unhandled type fails ("unsupported BBT n"); a handled type without its sub-table fails.
            A section that ends with no templet built fails ("... Templet is empty").
  factor:   BUFF_TEMPLET_ID and IGNORE_REGIST_FOR_RATE required; BEHAVIOR table required (no COMBINATION
            there is accepted), FINALIZER table and its COMBINATION required. BBT_EMPTY is skipped. Same
            unhandled / missing-sub-table rules.
  ordinal:  the Phase 3 loader keeps a placeholder for every entry that passes size == id-1 (main) or
            size == id-10001 (_INT, ids >= 10000) and fails to parse, so failures do not cascade.

Keys inside a sub-table are not modelled (a missing required key there shows up in the trial as an
unpredicted failure, with the key name in the log's error text).

"census set" = entries listing at least one type the C++ switch lacks, the census's own definition of
unsupported. The trial matches the census when every such entry fails with an "unsupported <kind> <n>"
reason that is one of its unhandled types, and no other entry fails that way.
"""
import argparse, collections, os, re, sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import buff_census as BC

KIND = {'BEHAVIOR': 'BBT', 'FINALIZER': 'BFT', 'DISPLAYER': 'BDT'}
ENUM_TABLE = {'BEHAVIOR': 'BUFF_BEHAVIOR_TYPE', 'FINALIZER': 'BUFF_FINALIZER_TYPE', 'DISPLAYER': 'BUFF_DISPLAYER_TYPE'}


def march_cases():
    """{(section, role): set(type names)} from the C++, flags resolved the way buff_census.main() does."""
    cache = BC.load_flag_cache()
    alw = open(os.path.join(BC.TRUNK, 'KTDXLIB', 'Always.h'), 'rb').read().decode('latin1')
    need = set()
    for f in BC.CPP_FILES + BC.CPP_HEADERS + ['X2GameUnit.cpp']:      # load_march_cpp reads X2GameUnit.cpp too
        need |= BC.conditional_flags(os.path.join(BC.X2LIB, f))
    for f in need:
        if f.startswith('SERV_IRUHADEV_'):
            cache[f] = bool(re.search(r'^\s*#\s*define\s+' + f + r'\b', alw, re.M))
    unknown = set()
    m = BC.load_march_cpp(unknown)
    if unknown:
        sys.stderr.write('NOTE: unprobed conditionals treated OFF: %s\n' % ', '.join(sorted(unknown)))
    return m['cases']


def value_of(expr, enum, table):
    """'BUFF_X.NAME' -> int via the staged Enum; a bare number -> int; else None (nil in Lua)."""
    if not isinstance(expr, str):
        return None
    expr = expr.strip()
    if re.match(r'^-?\d+(\.\d+)?$', expr):
        return int(float(expr))
    name = BC.enum_member(expr, table)
    return enum.get(table, {}).get(name) if name else None


def is_true(expr):
    return isinstance(expr, str) and expr.strip() not in ('', 'nil', 'FALSE', 'false', '0')


def section_types(rec, section, enum):
    """(list of (value, name) in COMBINATION order up to the first nil, has COMBINATION, sub-table names)"""
    sec = rec.get(section)
    if not isinstance(sec, BC.Table):
        return None
    comb = sec.get('COMBINATION')
    out = []
    if isinstance(comb, BC.Table):
        for v in comb.positional():
            val = value_of(v, enum, ENUM_TABLE[section])
            if val is None:
                break
            out.append((val, BC.enum_member(v, ENUM_TABLE[section]) or str(val)))
    return out, isinstance(comb, BC.Table) and bool(out), set(sec.keys()) - {'COMBINATION'}


def walk_section(rec, section, enum, cases, role, required):
    """-> None if the section parses, else a reason string."""
    st = section_types(rec, section, enum)
    if st is None:
        return ('%s table missing' % section) if required else None
    types, has_comb, subs = st
    handled = cases[(section, role)]
    names = {v: n for v, n in types}
    built = 0
    for val, name in types:
        if name == 'BBT_EMPTY':
            if role == 'TEMPLET':
                return None              # factory: `case BBT_EMPTY: return true;`
            continue                     # factor parse: `case BBT_EMPTY: break;`
        if name not in handled:
            return 'unsupported %s %d' % (KIND[section], val)
        if name not in subs:
            return 'missing sub-table %s' % name
        built += 1
    if role == 'FACTOR':
        # ParsingCombinationBehavior returns true when GetCombination fails; ...Finalizer returns false
        if section == 'FINALIZER' and not has_comb:
            return 'FINALIZER COMBINATION missing'
        return None
    # Templet factories, after the loop: `if ( vec.empty() [&& !bExceptionCase] ) "... Templet is empty"`.
    # Every listed type either builds, returns (BBT_EMPTY) or fails, so vec is empty only when no type
    # was listed. EXCEPTION_BUFF_FACTOR_VER2 (ON) starts bExceptionCase true for BEHAVIOR and DISPLAYER
    # and clears it only when a type is seen, so those pass empty; FINALIZER has no such guard
    # (EXCEPTION_BUFF_FACTOR is OFF).
    if built == 0 and section == 'FINALIZER':
        return 'FINALIZER Templet is empty'
    return None


def predict_templet(t, enum, cases):
    if value_of(t.get('BUFF_TEMPLET_ID'), enum, 'BUFF_TEMPLET_ID') is None:
        return 'BUFF_TEMPLET_ID'
    for k in ('IS_WORLD_BUFF', 'IS_DEBUF', 'ACCUMULATION_LIMIT'):
        if t.get(k) is None:
            return k
    lim = t.get('ACCUMULATION_LIMIT')
    try:
        if float(lim) > 1 and t.get('ACCUMULATION_MULTIPLIER') is None:
            return 'ACCUMULATION_MULTIPLIER'
    except ValueError:
        pass
    if t.get('REGIST_TYPE') is None:
        return 'REGIST_TYPE'
    for section, required in (('BEHAVIOR', True), ('FINALIZER', True), ('DISPLAYER', False)):
        r = walk_section(t, section, enum, cases, 'TEMPLET', required)
        if r:
            return r
    return None


def predict_factor(rec, enum, cases):
    if not isinstance(rec, BC.Table):
        return 'BUFF_FACTOR table missing'
    if value_of(rec.get('BUFF_TEMPLET_ID'), enum, 'BUFF_TEMPLET_ID') is None:
        return 'BUFF_TEMPLET_ID'
    if rec.get('IGNORE_REGIST_FOR_RATE') is None:
        return 'IGNORE_REGIST_FOR_RATE'
    for section in ('BEHAVIOR', 'FINALIZER'):
        r = walk_section(rec, section, enum, cases, 'FACTOR', True)
        if r:
            return r
    return None


def unhandled_types(rec, enum, cases, role):
    """census definition: every listed type the C++ switch for this role lacks -> set of 'BBT n'"""
    out = set()
    if not isinstance(rec, BC.Table):
        return out
    for section in ('BEHAVIOR', 'FINALIZER', 'DISPLAYER'):
        if role == 'FACTOR' and section == 'DISPLAYER':
            continue
        st = section_types(rec, section, enum)
        if st is None:
            continue
        for val, name in st[0]:
            if name != 'BBT_EMPTY' and name not in cases[(section, role)]:
                out.add('%s %d' % (KIND[section], val))
    return out


def predict(enum, cases, files):
    """-> (templets {bti: reason|None}, factors {('main'|'int', bfi): reason|None}, rejects [bfi], census sets)"""
    templets, factors, rejects = collections.OrderedDict(), collections.OrderedDict(), []
    census_t, census_f = {}, {}
    for key in ('templet', 'templet_int'):
        for t in files[key]['templets']:
            bti = value_of(t.get('BUFF_TEMPLET_ID'), enum, 'BUFF_TEMPLET_ID')
            templets[bti] = predict_templet(t, enum, cases)
            u = unhandled_types(t, enum, cases, 'TEMPLET')
            if u:
                census_t[bti] = u
    size = {'main': 0, 'int': 0}
    for key in ('factor', 'factor_int'):
        for f in files[key]['factors']:
            bfi = value_of(f.get('BUFF_FACTOR_ID'), enum, 'BUFF_FACTOR_ID') or 0
            vec = 'int' if bfi >= 10000 else 'main'
            need = bfi - 10001 if vec == 'int' else bfi - 1
            if size[vec] != need:
                rejects.append(bfi)
                continue
            size[vec] += 1
            rec = f.get('BUFF_FACTOR')
            factors[bfi] = predict_factor(rec, enum, cases)
            u = unhandled_types(rec, enum, cases, 'FACTOR')
            if u:
                census_f[bfi] = u
    return templets, factors, rejects, census_t, census_f


_LINE = re.compile(r'\[JUNE_BUFF\] (.*)')
_UNSUP = re.compile(r'^unsupported (.+?) in (BTI|BFI) (-?\d+)(?: \(BTI -?\d+\))? -> (\S+): (.*)$')
_SUMMARY = re.compile(r'^load summary: (.*)$')


def read_log(path):
    fails = {'BTI': {}, 'BFI': {}}
    summary, other = None, []
    for raw in open(path, 'rb').read().decode('latin1').splitlines():
        m = _LINE.search(raw)
        if not m:
            continue
        s = m.group(1).strip()
        u = _UNSUP.match(s)
        if u:
            fails[u.group(2)][int(u.group(3))] = (u.group(1), u.group(5))
            continue
        sm = _SUMMARY.match(s)
        if sm:
            summary = sm.group(1)
            continue
        other.append(s)
    return fails, summary, other


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('--log', help="a trial's ErrorLog.txt to compare")
    ap.add_argument('--source', help='read BattleFieldBuff{Templet,Factor}[_INT].src.lua from DIR instead of june_src_fixed')
    ap.add_argument('--enum', help='resolve names with this Enum.src.lua instead of the staged March one')
    ap.add_argument('-v', '--verbose', action='store_true')
    a = ap.parse_args()

    enum = BC.parse_enum_file(a.enum or BC.MARCH_ENUM)
    cases = march_cases()
    if a.source:
        files = {}
        for key, fn in (('templet', 'BattleFieldBuffTemplet'), ('templet_int', 'BattleFieldBuffTemplet_INT'),
                        ('factor', 'BattleFieldBuffFactor'), ('factor_int', 'BattleFieldBuffFactor_INT')):
            p = os.path.join(a.source, fn + '.src.lua')
            files[key] = BC.scan_lua(p) if os.path.exists(p) else BC.scan_text('')
    else:
        files = BC.load_june()
    templets, factors, rejects, census_t, census_f = predict(enum, cases, files)

    pt = {k: v for k, v in templets.items() if v}
    pf = {k: v for k, v in factors.items() if v}
    print('PREDICTION (%s on the Phase 3 loader)' % (a.source or 'June buff files'))
    print('  templets: %d entries, %d fail (%d of them on an unhandled type)' % (
        len(templets), len(pt), sum(1 for v in pt.values() if v.startswith('unsupported'))))
    print('  factors:  %d slots (%d main + %d _INT), %d placeholders (%d on an unhandled type), %d ordinal rejects' % (
        len(factors), sum(1 for k in factors if k < 10000), sum(1 for k in factors if k >= 10000), len(pf),
        sum(1 for v in pf.values() if v.startswith('unsupported')), len(rejects)))
    print('  census set: %d templets / %d factors list a type the C++ lacks' % (len(census_t), len(census_f)))
    by = collections.Counter(v.split(' ', 2)[0] if not v.startswith('unsupported') else ' '.join(v.split()[:2])
                             for v in list(pt.values()) + list(pf.values()))
    reasons = collections.Counter()
    for v in list(pt.values()) + list(pf.values()):
        reasons[re.sub(r'\d+$', 'n', v) if v.startswith('unsupported') else re.sub(r' \S+$', ' X', v) if v.startswith('missing') else v] += 1
    for r, n in reasons.most_common():
        print('    %4d  %s' % (n, r))
    if a.verbose:
        for k, v in sorted(pt.items()):
            print('  BTI %d: %s' % (k, v))
        for k, v in sorted(pf.items()):
            print('  BFI %d: %s' % (k, v))

    if not a.log:
        return
    fails, summary, other = read_log(a.log)
    print('\nTRIAL (%s)' % a.log)
    print('  summary: %s' % summary)
    print('  failed: %d templets, %d factors; other [JUNE_BUFF] lines: %d' % (len(fails['BTI']), len(fails['BFI']), len(other)))
    ok = True
    for label, pred, got, census in (('templet', pt, fails['BTI'], census_t), ('factor', pf, fails['BFI'], census_f)):
        miss = sorted(set(pred) - set(got))
        extra = sorted(set(got) - set(pred))
        wrong = sorted(k for k in set(pred) & set(got)
                       if pred[k].startswith('unsupported') and pred[k][len('unsupported '):] != got[k][0])
        print('  %s: predicted %d, got %d, predicted-not-failed %d, failed-not-predicted %d, other reason %d' % (
            label, len(pred), len(got), len(miss), len(extra), len(wrong)))
        for k in miss[:20]:
            print('    predicted, did not fail: %d (%s)' % (k, pred[k]))
        for k in extra[:20]:
            print('    failed, not predicted: %d: %s | %s' % (k, got[k][0], got[k][1][:160]))
        for k in wrong[:20]:
            print('    reason differs: %d predicted %s, got %s' % (k, pred[k], got[k][0]))
        # census match
        got_unsup = {k for k, (r, _) in got.items() if re.match(r'^B[BFD]T \d+$', r)}
        c_miss = sorted(k for k in census if k not in got_unsup and not (k in pred and not pred[k].startswith('unsupported')))
        c_extra = sorted(k for k in got_unsup if k not in census)
        c_bad = sorted(k for k in got_unsup & set(census) if got[k][0] not in census[k])
        print('  %s vs census: census %d, failed on an unhandled type %d; census entry not failing that way %d, '
              'failing that way outside census %d, logged type not in entry %d' % (
                  label, len(census), len(got_unsup), len(c_miss), len(c_extra), len(c_bad)))
        for k in c_miss[:20]:
            print('    census, no unsupported line: %d %s (got: %s)' % (k, sorted(census[k]), got.get(k, ('-',))[0]))
        for k in c_extra[:20]:
            print('    unsupported line outside census: %d %s' % (k, got[k][0]))
        if miss or extra or wrong or c_miss or c_extra or c_bad:
            ok = False
    if a.verbose:
        for s in other:
            print('  other: ' + s)
    print('\nRESULT: %s' % ('trial matches the prediction and the census' if ok else 'MISMATCH, see above'))


if __name__ == '__main__':
    main()
