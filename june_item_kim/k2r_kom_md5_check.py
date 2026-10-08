import os,sys,glob,json,hashlib,subprocess,csv,shutil,faulthandler
faulthandler.dump_traceback_later(3000,exit=True)
TRUNK=os.getcwd()
B=r"F:/elsword stuff/elsword_2014/els_2014/237311/24965799"
TMP=B+"/k2_review_tmp"
EL=os.path.join(TRUNK,"elstools","elstools.exe")
KOMS="data036 data015 data016 data017 data018 data019 data067 data130 data131 data143 data148 data158 data159 data185 data191 data192 data211 datai004 datai005 datai006 datai007 datai009 datai010 datai011 datai012 datai023".split()
staged={}
for r in csv.DictReader((l for l in open("june_item_kim/k2_assets_staged.tsv",encoding="utf-8") if not l.startswith("#")),delimiter="\t"):
    if r["state"]!="NOT_IN_ANY_JUNE_KOM": staged.setdefault(r["source_kom"],set()).add(r["name"].lower())
def ex(kom,tag):
    out=os.path.join(TMP,tag)
    os.makedirs(out,exist_ok=True)
    r=subprocess.run([EL,"extract","-q","-v","mar-2014","-i",kom,"-o",out],capture_output=True,text=True)
    d=os.path.join(out,os.path.splitext(os.path.basename(kom))[0]+"_kom")
    if r.returncode!=0 or not os.path.isdir(d): raise SystemExit("extract failed %s %s"%(kom,r.stdout[-300:]+r.stderr[-300:]))
    m={}
    for f in os.listdir(d):
        if f=="_manifest.json": continue
        m[f.lower()]=hashlib.md5(open(os.path.join(d,f),"rb").read()).hexdigest()
    shutil.rmtree(d)
    return m
def algos(kom):
    r=subprocess.run([EL,"inspect","-q","-i",kom],capture_output=True,text=True)
    j=json.loads(r.stdout[r.stdout.index("{"):])
    return {f["name"].lower():f["algo"] for f in j["files"]}
allok=True
for k in KOMS:
    live=B+"/data/%s.kom"%k
    bks=sorted(glob.glob(B+"/kom_backup_tools/%s.kom.20261008_06*"%k))
    lm=ex(live,"live")
    bm=ex(bks[-1],"base") if bks else {}
    add=set(lm)-set(bm); rem=set(bm)-set(lm); chg={n for n in set(lm)&set(bm) if lm[n]!=bm[n]}
    exp=staged.get(k,set()) if k!="data036" else set()
    al=algos(live)
    badalgo=sorted(n for n in add if al.get(n)!="0")
    if k=="data036": ok = not add and not rem and chg=={"item.kim"}
    else: ok = add==exp and not rem and not chg and not badalgo
    allok&=ok
    print("%s %-9s base=%-26s entries live=%d base=%d +%d -%d ~%d staged=%d %s%s"%("OK  " if ok else "FAIL",k,os.path.basename(bks[-1]) if bks else "(new kom)",len(lm),len(bm),len(add),len(rem),len(chg),len(exp),
        "" if ok else "add-exp=%s exp-add=%s rem=%s chg=%s"%(sorted(add-exp)[:5],sorted(exp-add)[:5],sorted(rem)[:5],sorted(chg)[:5]),
        (" badalgo=%s"%badalgo[:5]) if badalgo else ""),flush=True)
print("RESULT", "PASS" if allok else "FAIL")
shutil.rmtree(TMP,ignore_errors=True)
