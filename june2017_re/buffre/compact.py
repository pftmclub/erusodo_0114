import re,sys,gh,csv
X=list(csv.DictReader(open(r'F:/elsword stuff/elsword_2014/els_2014/ElswordFiles/ready_and_built/ProjectX2_SVN/ProjectX2_SVN/source/EU_CN_US/Trunk/june2017_re/xorstrings.tsv',encoding='utf-8'),delimiter='\t'))
def keys(f):
    r=sorted([(x['use_va'],x['plaintext'],x['buf_va']) for x in X if x['func_va']==f])
    return r
def compact(f,maxl=400):
    s=gh.dec(f); out=[]
    bufs={x[2] for x in keys(f)}
    for l in s.splitlines():
        t=l.strip()
        if not t or t.startswith('/*') : continue
        if re.match(r'^(local_|uStack|puStack|iStack|fStack)\w* = (DAT|_DAT|\(\w+\)DAT|CONCAT)',t): continue
        if re.match(r'^_?DAT_[0-9a-f]+(\._\d_\d_)? = (0x)?[0-9a-f]+;',t) or re.match(r'^_?DAT_[0-9a-f]+ = _?DAT_[0-9a-f]+ \| ',t): continue
        if re.match(r'^(local_|u|b)\w* = .*DAT_337[0-9a-f]+.* \^ ',t): continue
        if re.match(r'^(undefined|int|uint|char|float|byte|bool|ushort|short|code|float10|undefined\d)\s\*?\w+(\s?\[\d+\])?;$',t): continue
        if re.match(r'^(\*unaff_FS_OFFSET|local_c = \*unaff|puStack_8 = |local_4 = |uStack_4 = |local_10 = DAT_3374)',t): continue
        out.append(t)
    return out
if __name__=='__main__':
    for f in sys.argv[1:]:
        print('#####',f,'keys:',' '.join(k[1] for k in keys(f)))
        print('\n'.join(compact(f)))
