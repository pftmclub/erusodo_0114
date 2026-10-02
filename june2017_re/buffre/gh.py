import urllib.request,urllib.parse,os
H='http://127.0.0.1:8080'
op=urllib.request.build_opener(urllib.request.ProxyHandler({}))
D=os.path.join(os.path.dirname(os.path.abspath(__file__)),'dec')
def get(ep,**kw):
    return op.open(H+'/'+ep+'?'+urllib.parse.urlencode(kw),timeout=300).read().decode('utf-8','replace')
def dec(a):
    a=a.replace('0x','');f=os.path.join(D,a+'.c')
    if not os.path.exists(f) or os.path.getsize(f)==0:
        open(f,'w',encoding='utf-8').write(get('decompile_function',address='0x'+a))
    return open(f,encoding='utf-8').read()
def dis(a):
    a=a.replace('0x','');f=os.path.join(D,a+'.s')
    if not os.path.exists(f) or os.path.getsize(f)==0:
        open(f,'w',encoding='utf-8').write(get('disassemble_function',address='0x'+a))
    return open(f,encoding='utf-8').read()
def xto(a,limit=200): return get('xrefs_to',address='0x'+a.replace('0x',''),limit=limit)
