#!/usr/bin/env python3
"""statflat.py - StatTable.lua: one base-stat curve (June's UT_AIN row) for every class, levels 1-99.

    python tools/statflat.py show [--level N ...]   print the curve
    python tools/statflat.py check                  prove every SetUnitStat row equals the curve (exit 1 if not)
    python tools/statflat.py write                  rewrite every SetUnitStat row to the curve, nothing else changes
    python tools/statflat.py restore                undo: put back the per-class rows from the commit named in the header

Curve (June CalculatePhysicalAndMagicalStat / CalculateHPStat, CharacterStatTable.src.lua, row UT_AIN = 400/400/100/100/10000):
    Atk, Def = floor(row * (0.8 + 0.0225 * (L - 1)) + 0.5)        HP = 10000 + 1500 * (L - 1)
Level 99 = 1202 / 1202 / 301 / 301 / 157000 (the cobodex.eu calculator's bare base stat of a level-99 character).

Only lines that start with "StatTable:SetUnitStat(" change; BOM, CRLF, comments and ReserveMemory lines stay byte-exact.
Server Lua: the user XORs and packs the result into data036.kom.
"""
import math
import os
import re
import subprocess
import sys

TRUNK = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
REL = 'KncWX2Server/ServerResource/US/StatTable.lua'
PATH = os.path.join(TRUNK, *REL.split('/'))

ROW = (400, 400, 100, 100, 10000)     # UT_AIN: AtkPhysic, AtkMagic, DefPhysic, DefMagic, BaseHP
HP_PER_LEVEL = 1500
MAX_LEVEL = 99
TAG = '-- FLAT99'
HEADER_MARK = '-- FLAT99: every SetUnitStat row below is one curve for all classes'

LINE = re.compile(rb'^(StatTable:SetUnitStat\(\s*)(\d+)(\s*,\s*)(\d+)(\s*,\s*)\{[^}]*\}(\s*\))(.*)$')


def stat(level):
    f = lambda b: math.floor(b * (0.8 + 0.0225 * (level - 1)) + 0.5)
    return f(ROW[0]), f(ROW[1]), f(ROW[2]), f(ROW[3]), ROW[4] + HP_PER_LEVEL * (level - 1)


def table_text(level):
    a, m, dp, dm, hp = stat(level)
    return '{ AtkPhysic = %d, AtkMagic = %d, DefPhysic = %d, DefMagic = %d, HP = %d }' % (a, m, dp, dm, hp)


def read():
    with open(PATH, 'rb') as fh:
        return fh.read()


def head():
    return subprocess.check_output(['git', '-C', TRUNK, 'rev-parse', '--short', 'HEAD']).decode().strip()


def rewrite(data):
    out, n = [], 0
    for line in data.split(b'\r\n'):
        m = LINE.match(line)
        if m:
            lv = int(m.group(4))
            if not 1 <= lv <= MAX_LEVEL:
                raise SystemExit('row outside 1-%d: %r' % (MAX_LEVEL, line))
            line = (m.group(1) + m.group(2) + m.group(3) + m.group(4) + m.group(5) + table_text(lv).encode()
                    + m.group(6) + b' ' + TAG.encode())
            n += 1
        out.append(line)
    return b'\r\n'.join(out), n


ENUM_LUA = os.path.join(TRUNK, 'june_src_fixed', 'data036_kom', 'Enum.src.lua')


def enum_classes():
    """UC_ name -> id for every playable class in June's Enum.lua (UC_NONE excluded)."""
    with open(ENUM_LUA, 'rb') as fh:
        s = fh.read().decode('utf-8-sig')
    i = s.index('UNIT_CLASS')
    body = re.search(r'UNIT_CLASS\s*=\s*\{(.*?)\n\}', s[i:], re.S).group(1)
    return dict((n, int(v)) for n, v in re.findall(r'(UC_\w+)\s*=\s*(\d+)', body) if int(v) > 0)


def cmd_check():
    bad, n, levels = 0, 0, {}
    for line in read().split(b'\r\n'):
        m = LINE.match(line)
        if not m:
            continue
        n += 1
        levels.setdefault(int(m.group(2)), set()).add(int(m.group(4)))
        if table_text(int(m.group(4))).encode() not in line:
            bad += 1
            print('MISMATCH', line.decode('utf-8', 'replace')[:140])
    want = set(range(1, MAX_LEVEL + 1))
    short = sorted(c for c in enum_classes().values() if levels.get(c) != want)
    if short:
        bad += len(short)
        print('NO FULL ROWS for enum classes:', short)
    print('STATFLAT %s: %d rows, %d classes, %d enum classes, %d problems'
          % ('OK' if bad == 0 else 'FAIL', n, len(levels), len(enum_classes()), bad))
    return 1 if bad else 0


def missing_block(data):
    have = set(int(m.group(2)) for m in (LINE.match(l) for l in data.split(b'\r\n')) if m)
    names = dict((v, k) for k, v in enum_classes().items())
    out = []
    for c in sorted(c for c in names if c not in have):
        out.append('StatTable:ReserveMemory( %d, %d ) -- %s' % (c, MAX_LEVEL, names[c]))
        out += ['StatTable:SetUnitStat( %d, %d, %s ) %s' % (c, lv, table_text(lv), TAG) for lv in range(1, MAX_LEVEL + 1)]
        out.append('')
    return out


def cmd_write():
    data = read()
    new, n = rewrite(data)
    block = missing_block(new)
    if block:
        new = new + b'\r\n\r\n' + '-- FLAT99: classes the studio file had no rows for (June Enum.lua UNIT_CLASS)\r\n'.encode() \
            + '\r\n'.join(block).rstrip('\r\n').encode()
        n += sum(1 for l in block if l.startswith('StatTable:SetUnitStat'))
    if HEADER_MARK.encode() not in new:
        note = ('-- FLAT99: every SetUnitStat row below is one curve for all classes (June UT_AIN row, tools/statflat.py).\r\n'
                '-- Restore the per-class rows: python tools/statflat.py restore  (they are in git at %s).\r\n\r\n' % head())
        key = b'StatTable:ReserveMemory( 1, 99 )'
        i = new.index(key)
        new = new[:i] + note.encode() + new[i:]
    with open(PATH, 'wb') as fh:
        fh.write(new)
    print('wrote %d rows' % n)
    return cmd_check()


def cmd_restore():
    text = read().decode('utf-8-sig')
    m = re.search(r'\(they are in git at ([0-9a-f]+)\)', text)
    if not m:
        raise SystemExit('no restore commit in the header')
    old = subprocess.check_output(['git', '-C', TRUNK, 'show', '%s:%s' % (m.group(1), REL)])
    with open(PATH, 'wb') as fh:
        fh.write(old)
    print('restored from', m.group(1))
    return 0


def cmd_show(levels):
    print('level  AtkPhysic AtkMagic DefPhysic DefMagic HP')
    for lv in (levels or [1, 10, 20, 30, 40, 50, 60, 70, 80, 90, 98, 99]):
        print('%5d  %9d %8d %9d %8d %d' % ((lv,) + stat(lv)))
    return 0


if __name__ == '__main__':
    a = sys.argv[1:]
    if not a or a[0] not in ('show', 'check', 'write', 'restore'):
        print(__doc__)
        sys.exit(2)
    if a[0] == 'show':
        sys.exit(cmd_show([int(x) for x in a[1:] if x.isdigit()]))
    sys.exit({'check': cmd_check, 'write': cmd_write, 'restore': cmd_restore}[a[0]]())
