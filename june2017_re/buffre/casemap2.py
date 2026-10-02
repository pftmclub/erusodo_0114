import re,sys,gh
src=sys.argv[1]; tab=sys.argv[2]
e=open(r'F:/elsword stuff/elsword_2014/els_2014/ElswordFiles/ready_and_built/ProjectX2_SVN/ProjectX2_SVN/source/EU_CN_US/Trunk/june_src_fixed/data036_kom/Enum.src.lua',encoding='utf-8',errors='replace').read()
m=re.search(tab+r'\s*=\s*\{(.*?)\}',e,re.S); names={int(v):n for n,v in re.findall(r'(\w+)\s*=\s*(\d+)',m.group(1))}
t=gh.dec(src)
parts=re.split(r'\n\s*(?:case (0x[0-9a-f]+|\d+):|default:)',t)
for i in range(1,len(parts),2):
    if parts[i] is None: print('default', parts[i+1][:300].replace('\n',' ')); continue
    n=int(parts[i],0); body=parts[i+1]
    calls=re.findall(r'(FUN_[0-9a-f]{8})\(([^;]*?)\)',body)
    print(n, names.get(n,'?'), ' ; '.join(f'{c}({a[:60]})' for c,a in calls[:3]) or body.strip()[:120].replace('\n',' '))
