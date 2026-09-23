import struct,re,sys
exec(open(sys.argv[2]).read().split('# loads + xor pairing')[0])
# dword/word loads with xor imm32/imm16
cnt=0
for m in re.finditer(rb'(?:\x8b([\x05\x0d\x15\x1d\x2d\x35\x3d])|(\xa1))(....)',text,re.S):
    a=struct.unpack('<I',m.group(3))[0]
    if not(LO<=a<HI) or a not in enc: continue
    r=0 if m.group(2) else (m.group(1)[0]>>3)&7
    w=text[m.end():m.end()+48]
    for j in range(len(w)-5):
        if (w[j]==0x81 and w[j+1]==0xf0+r) or (r==0 and w[j]==0x35):
            off=j+2 if w[j]==0x81 else j+1
            k=w[off:off+4]
            s=bytes(enc.get(a+i,0)^k[i] for i in range(4))
            cnt+=1
            if cnt<40: print(hex(va(m.start())),hex(a),s)
            break
print('dword xor sites',cnt)
# SSE: movdqa/movups xmm,[abs] then pxor/xorps
c2=len(re.findall(rb'\x66\x0f\xef',text)); c3=len(re.findall(rb'\x0f\x57',text))
print('pxor',c2,'xorps',c3)
