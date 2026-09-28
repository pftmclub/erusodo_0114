"""Phase 5 (RANOX_LUA_DECOMPILE_FIX_PLAN.md): turn phase5_worker.py's per-file
JSON outputs into june2017_re/decompile_verdicts.tsv (one row per (file,
function)), plus a clobber census and a regression check.

    python phase5_aggregate.py --outdir tmp/phase5_out --map june2017_re/phase5_files.tsv
        --out june2017_re/decompile_verdicts.tsv --clobber june2017_re/phase5_clobber_census.tsv
        [--runs tmp/phase5_verdicts_small.tsv tmp/phase5_verdicts_big.tsv]

Verdict vocabulary (per file+function; see the plan's Phase 5 "Work" item 3):
  clean / dropped-assignment / other-ljd / clobber / static-only /
  port-divergence / unreached / no-june-original / DEEPCOPY-EXEMPT (see below)

DEEPCOPY-EXEMPT: an rt-side POISON_READ whose slot name is also a `for slotN,
slotM in pairs(...)` loop variable elsewhere in the SAME function's fixed
decompile text. That is the structural signature of the *separate*, already
catalogued "ljd caveat" deepcopy mis-structuring defect (README.md; Phase 1
correction 6; RANOX_LUA_DECOMPILE_BUG.md "Root cause" note on site (a)-(d)
being a DIFFERENT defect class from this one) -- NOT the dropped-assignment
class Phase 4 fixed. An rt-side function that trips 'dropped-assignment' and
does NOT match this signature is a genuine Phase 4 regression candidate and is
called out at the top of the run's printed report, unmissably.
"""
import argparse
import csv
import glob
import json
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
TRUNK = os.path.dirname(HERE)

PRIORITY_RT = ['harness', 'dropped-assignment', 'clobber', 'upvalue', 'reorder', 'other-ljd', 'pass']
PRIORITY_PORT = ['harness', 'dropped-assignment', 'clobber', 'upvalue', 'reorder', 'other', 'pass']

FOR_LOOP_VAR = re.compile(r'\bfor\s+([\w,\s]+?)\s+in\s+pairs\(')
SLOTNAME_SITE = re.compile(r'^slot\d+@L\d+$')


def dominant(verdicts, priority):
    for v in priority:
        if verdicts.get(v):
            return v
    return 'pass' if not verdicts else sorted(verdicts)[0]


FUNC_START = re.compile(r'^function \w+\(')


def function_blocks(rt_src_text):
    """[(start, end)] 0-based line-index ranges of every top-level `function
    NAME(...)` ... `end` block (named or anonymous -- ljd prints an anonymous
    function assigned to a register the same way, e.g. `function slot0(...)`,
    so name matching can't tell them apart; this only needs the range)."""
    lines = rt_src_text.splitlines()
    blocks = []
    start = None
    for i, ln in enumerate(lines):
        if start is None and FUNC_START.match(ln):
            start = i
        elif start is not None and ln.rstrip('\r\n') == 'end':
            blocks.append((start, i))
            start = None
    return blocks


def deepcopy_exempt_at_line(rt_src_text, line, slot_name):
    """True iff `slot_name` is a for-in loop variable in the same scope as the
    1-based `line` (the enclosing function block if `line` falls inside one,
    else the file's top-level/main-chunk text) -- the deepcopy hoist's
    structural signature (see module docstring)."""
    if not rt_src_text:
        return False
    lines = rt_src_text.splitlines()
    idx = line - 1
    blocks = function_blocks(rt_src_text)
    enclosing = [(s, e) for s, e in blocks if s <= idx <= e]
    if enclosing:
        s, e = max(enclosing, key=lambda se: se[0])  # innermost (latest start)
        body = '\n'.join(lines[s:e + 1])
    else:
        # top-level/main-chunk text: everything NOT inside a function block
        top_lines = [ln for i, ln in enumerate(lines) if not any(s <= i <= e for s, e in blocks)]
        body = '\n'.join(top_lines)
    for m in FOR_LOOP_VAR.finditer(body):
        names = [n.strip() for n in m.group(1).split(',')]
        if slot_name in names:
            return True
    return False


def fn_static_counts(static_rows, fn):
    confirmed = sum(1 for r in static_rows if r['fn'] == fn and r['status'] == 'confirmed')
    only = sum(1 for r in static_rows if r['fn'] == fn and r['status'] != 'confirmed')
    return confirmed, only


def fn_lint_counts(lint_rows, fn):
    if lint_rows is None:
        return None, None
    total = sum(1 for h in lint_rows if h['fn'] == fn)
    confirmed = sum(1 for h in lint_rows if h['fn'] == fn and h['confirmed'])
    return total, confirmed


def first_str(first):
    if not first:
        return ''
    s, v, labels, i, xa, xb = first
    tail = xb[-1] if xb else ''
    return 'seed %d %s[%s] ev%d: %s' % (s, v, ','.join(labels), i, tail)


def classify_row(side_name, priority, fr, static_rows, fn, lint_rows, rt_src_text, regressions):
    verdicts = fr['verdicts']
    dom = dominant(verdicts, priority)
    static_confirmed, static_only = fn_static_counts(static_rows, fn)
    lint_total, lint_confirmed = fn_lint_counts(lint_rows, fn)

    if dom == 'dropped-assignment' and side_name == 'rt':
        # every rt-side dropped-assignment site must be the known deepcopy
        # exemption, or it is a Phase 4 regression candidate. A call's `sites`
        # set spans its whole call tree (nested FCALLs included, see the
        # trace format doc); deepcopy_exempt_at_line resolves the site's own
        # enclosing scope directly from its line number, so it works for a
        # named function OR the outer call's own scope regardless of `fn`.
        def site_line(site):
            _, _, lineno = site.partition('@L')
            return int(lineno) if lineno else 0
        poison_sites = [s for s in fr['sites'] if SLOTNAME_SITE.match(s)]
        unexplained = [s for s in poison_sites
                        if not deepcopy_exempt_at_line(rt_src_text, site_line(s), s.split('@')[0])]
        if unexplained:
            regressions.append((fn, unexplained))
            verdict = 'dropped-assignment'
        else:
            verdict = 'other-ljd (deepcopy, exempt)'
    elif dom == 'dropped-assignment':
        verdict = 'dropped-assignment'
    elif dom == 'clobber':
        verdict = 'clobber'
    elif dom in ('other', 'other-ljd'):
        verdict = 'port-divergence' if side_name == 'port' else 'other-ljd'
    elif dom in ('reorder', 'upvalue', 'harness'):
        verdict = dom
    elif static_only:
        verdict = 'static-only'
    else:
        verdict = 'clean'

    return {
        'verdict': verdict, 'dominant_raw': dom, 'seeds': fr['seeds'],
        'verdicts_breakdown': ' '.join('%s=%d' % kv for kv in sorted(verdicts.items())),
        'cov_orig': fr['cov_orig'], 'cov_side': fr['cov_side'],
        'static_confirmed': static_confirmed, 'static_only': static_only,
        'clobber_events': len(fr['clobber']), 'lint_total': lint_total, 'lint_confirmed': lint_confirmed,
        'first_divergence': first_str(fr['first']),
    }


def main(argv):
    ap = argparse.ArgumentParser()
    ap.add_argument('--outdir', default=os.path.join(TRUNK, 'tmp', 'phase5_out'))
    ap.add_argument('--map', default=os.path.join(HERE, 'phase5_files.tsv'))
    ap.add_argument('--out', default=os.path.join(HERE, 'decompile_verdicts.tsv'))
    ap.add_argument('--clobber', default=os.path.join(HERE, 'phase5_clobber_census.tsv'))
    args = ap.parse_args(argv)

    file_rows = list(csv.DictReader(open(args.map, encoding='utf-8'), delimiter='\t'))
    by_key = {(r['kom_dir'], r['kom_name']): r for r in file_rows}

    out_rows = []
    clobber_rows = []
    regressions_by_file = {}

    for fr_meta in file_rows:
        key = (fr_meta['kom_dir'], fr_meta['kom_name'])
        flat = '%s__%s' % key
        jpath = os.path.join(args.outdir, flat + '.json')
        if fr_meta['map_status'] != 'mapped':
            out_rows.append({
                'scope': 'ported', 'port_path': fr_meta['port_path'], 'kom_dir': '', 'kom_name': '',
                'fn': '-', 'verdict': fr_meta['map_status'], 'dominant_raw': '', 'seeds': '',
                'verdicts_breakdown': '', 'cov_orig': '', 'cov_side': '', 'static_confirmed': '',
                'static_only': '', 'clobber_events': '', 'lint_total': '', 'lint_confirmed': '',
                'first_divergence': '', 'notes': fr_meta.get('candidates', ''),
            })
            continue
        if not os.path.exists(jpath):
            out_rows.append({
                'scope': 'ported', 'port_path': fr_meta['port_path'], 'kom_dir': key[0], 'kom_name': key[1],
                'fn': '-', 'verdict': 'not-run', 'dominant_raw': '', 'seeds': '', 'verdicts_breakdown': '',
                'cov_orig': '', 'cov_side': '', 'static_confirmed': '', 'static_only': '', 'clobber_events': '',
                'lint_total': '', 'lint_confirmed': '', 'first_divergence': '', 'notes': 'no json at ' + jpath,
            })
            continue

        j = json.load(open(jpath, encoding='utf-8'))
        rt = j['results'].get('rt', {})
        port = j['results'].get('port', {})
        rt_fns = rt.get('fns', {})
        port_fns = port.get('fns', {})
        rt_static = rt.get('static', [])
        port_static = port.get('static', [])
        port_lint = port.get('lint')
        rt_src_path = os.path.join(TRUNK, 'tmp', 'ljdiff_%s.src.lua' % flat)
        rt_src_text = open(rt_src_path, encoding='utf-8', errors='replace').read() if os.path.exists(rt_src_path) else ''

        regressions = []
        # Coverage-independent regression check: the static must-define pass
        # lists every GGET slotN some path reaches unwritten, whether or not
        # any of the 32 seeds actually took that path (calibration: "a 32-seed
        # sweep is not proof against stale-value instances" -- BW-3 showed on
        # ~3% of seeds). An untagged row (not main-written/nested/upvalue) not
        # matching the deepcopy signature is a Phase 4 regression the dynamic
        # check alone could miss.
        static_regressions = []
        for row in rt_static:
            if row['tags']:
                continue
            if not deepcopy_exempt_at_line(rt_src_text, row['line'], row['name']):
                static_regressions.append(row)
        if static_regressions:
            regressions_by_file.setdefault(fr_meta['port_path'], []).append(
                ('<static>', ['%s@L%d(status=%s,seeds=%d)' % (r['name'], r['line'], r['status'], r['seeds'])
                               for r in static_regressions]))

        all_fns = sorted(set(rt_fns) | set(port_fns))
        for fn in all_fns:
            rt_row = classify_row('rt', PRIORITY_RT, rt_fns[fn], rt_static, fn, None, rt_src_text, regressions) \
                if fn in rt_fns else None
            port_row = classify_row('port', PRIORITY_PORT, port_fns[fn], port_static, fn, port_lint, rt_src_text,
                                     regressions) if fn in port_fns else None
            # the port comparison is the one that matters for repair triage;
            # rt is the Phase 4 regression check, folded into 'notes'.
            row = port_row or {'verdict': 'not-exercised', 'dominant_raw': '', 'seeds': '', 'verdicts_breakdown': '',
                                'cov_orig': '', 'cov_side': '', 'static_confirmed': '', 'static_only': '',
                                'clobber_events': '', 'lint_total': '', 'lint_confirmed': '', 'first_divergence': ''}
            notes = []
            if rt_row:
                notes.append('rt=%s(%s)' % (rt_row['verdict'], rt_row['verdicts_breakdown']))
            if fn in rt.get('only_orig', []) or fn in port.get('only_orig', []):
                notes.append('unreached-by-harness(only in June trace)')
            for cl, n in (port_fns.get(fn, {}) or {}).get('clobber', {}).items():
                clobber_rows.append({'port_path': fr_meta['port_path'], 'side': 'port', 'fn': fn, 'event': cl,
                                      'seeds': n})
            for cl, n in (rt_fns.get(fn, {}) or {}).get('clobber', {}).items():
                clobber_rows.append({'port_path': fr_meta['port_path'], 'side': 'rt', 'fn': fn, 'event': cl,
                                      'seeds': n})
            out_rows.append(dict(row, scope='ported', port_path=fr_meta['port_path'], kom_dir=key[0],
                                  kom_name=key[1], fn=fn, notes='; '.join(notes)))
        if regressions:
            regressions_by_file[fr_meta['port_path']] = regressions

        # <main>: every mapped file gets one row even with zero functions
        # (a pure data/table file), so "every ported file has a verdict row"
        # holds file-wide, not just per-function. rt vs June is the
        # regression check (should always be 'pass'); the port's main_values
        # diff count is reported as a plain fact, not classified as a decompile
        # defect -- for table files (Enum, EnumString, ...) it is expected
        # content drift already tracked by JUNE2017_EFFECT_REBUILD_PLAN.md,
        # not something this plan's Phase 6 repairs.
        rt_main = rt.get('main')
        port_main = port.get('main')
        rt_mv = rt.get('main_values', {})
        port_mv = port.get('main_values', {})
        main_notes = []
        if rt_main:
            main_notes.append('rt_main=%s' % rt_main['verdict'])
        if rt_mv:
            main_notes.append('rt_main_values differ=%d only_orig=%d only_side=%d(of %d)' % (
                len(rt_mv.get('differ', [])), len(rt_mv.get('only_orig', [])), len(rt_mv.get('only_side', [])),
                rt_mv.get('n', 0)))
        if port_mv:
            main_notes.append('port_main_values differ=%d only_orig=%d only_side=%d(of %d)' % (
                len(port_mv.get('differ', [])), len(port_mv.get('only_orig', [])), len(port_mv.get('only_side', [])),
                port_mv.get('n', 0)))
        main_verdict = 'clean'
        if rt_main and 'dropped-assignment' in rt_main['labels']:
            # jsonify_res() doesn't carry 'main''s raw `sites` (only verdict+
            # labels) -- read it back from the raw rt trace ljdiff.py itself
            # left on disk (tmp/ljdiff_<flat>.rt.json), same as ljdiff's own
            # --diff CLI path would use.
            rt_trace_path = os.path.join(TRUNK, 'tmp', 'ljdiff_%s.rt.json' % flat)
            main_sites = []
            if os.path.exists(rt_trace_path):
                main_sites = json.load(open(rt_trace_path, encoding='utf-8')).get('main', {}).get('sites', [])
            # 'sites' mixes POISON_READ (slotN) and UPVALUE_READ (uvN) marks
            # (see ljdiff.lua's site()); only slotN sites are the dropped-
            # assignment class this exemption is about -- a uvN read is the
            # separate 'upvalue' label, already surfaced via rt_main['labels'].
            poison_sites = [s for s in main_sites if SLOTNAME_SITE.match(s)]
            unexplained = [s for s in poison_sites
                            if not deepcopy_exempt_at_line(rt_src_text, int(s.partition('@L')[2] or 0),
                                                            s.split('@')[0])]
            if unexplained:
                regressions_by_file.setdefault(fr_meta['port_path'], []).append(('<main>', unexplained))
                main_verdict = 'dropped-assignment'
            else:
                main_verdict = 'other-ljd (deepcopy, exempt)'
        elif rt_main and rt_main['verdict'] not in ('pass',):
            main_verdict = 'other-ljd'
        elif port_main and port_main['verdict'] != 'pass':
            main_verdict = 'port-divergence'
        out_rows.append({
            'scope': 'ported', 'port_path': fr_meta['port_path'], 'kom_dir': key[0], 'kom_name': key[1],
            'fn': '<main>', 'verdict': main_verdict, 'dominant_raw': (port_main or {}).get('verdict', ''),
            'seeds': '', 'verdicts_breakdown': '', 'cov_orig': '', 'cov_side': '', 'static_confirmed': '',
            'static_only': '', 'clobber_events': '', 'lint_total': '', 'lint_confirmed': '',
            'first_divergence': '', 'notes': '; '.join(main_notes),
        })

    cols = ['scope', 'port_path', 'kom_dir', 'kom_name', 'fn', 'verdict', 'dominant_raw', 'seeds',
            'verdicts_breakdown', 'cov_orig', 'cov_side', 'static_confirmed', 'static_only', 'clobber_events',
            'lint_total', 'lint_confirmed', 'first_divergence', 'notes']
    with open(args.out, 'w', encoding='utf-8', newline='\n') as fh:
        w = csv.DictWriter(fh, fieldnames=cols, delimiter='\t', lineterminator='\n')
        w.writeheader()
        for r in out_rows:
            w.writerow({k: r.get(k, '') for k in cols})

    with open(args.clobber, 'w', encoding='utf-8', newline='\n') as fh:
        w = csv.DictWriter(fh, fieldnames=['port_path', 'side', 'fn', 'event', 'seeds'], delimiter='\t',
                            lineterminator='\n')
        w.writeheader()
        for r in clobber_rows:
            w.writerow(r)

    print('wrote %s (%d rows)' % (args.out, len(out_rows)))
    print('wrote %s (%d rows)' % (args.clobber, len(clobber_rows)))
    if regressions_by_file:
        print('\n!!! POSSIBLE PHASE 4 REGRESSION: rt-side dropped-assignment NOT matching the deepcopy '
              'exemption in %d file(s):' % len(regressions_by_file))
        for p, regs in regressions_by_file.items():
            for fn, sites in regs:
                print('   %s :: %s -> %s' % (p, fn, sites))
        return 1
    print('\nno unexplained rt-side dropped-assignment: Phase 4 fix holds across the sweep.')
    return 0


if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
