016f:7cc61156 e822010000     call      7cc6127d
016f:7cc6115b d94508         fld       dword ptr [ebp+08]
016f:7cc6115e d80de422c47c   fmul      dword ptr [7cc422e4]
016f:7cc61164 8b45fc         mov       eax,dword ptr [ebp-04]
016f:7cc61167 3bc3           cmp       eax,ebx
016f:7cc61169 d95d08         fstp      dword ptr [ebp+08]
016f:7cc6116c d94514         fld       dword ptr [ebp+14]
016f:7cc6116f d80de86cc47c   fmul      dword ptr [7cc46ce8]
25df##u
016f:7cc61175 7e37           jle       7cc611ae
016f:7cc61177 8b7510         mov       esi,dword ptr [ebp+10]
016f:7cc6117a 894514         mov       dword ptr [ebp+14],eax
016f:7cc6117d 8bd6           mov       edx,esi
016f:7cc6117f 8b0e           mov       ecx,dword ptr [esi]				;== TOP OF lOOP
016f:7cc61181 3b4d0c         cmp       ecx,dword ptr [ebp+0c]
016f:7cc61184 7d20           jge       7cc611a6
016f:7cc61186 d9848d20feffff fld       dword ptr [ebp+ecx*4+fffffe20]
016f:7cc6118d d80c8f         fmul      dword ptr [edi+ecx*4]
016f:7cc61190 d85d08         fcomp     dword ptr [ebp+08]
016f:7cc61193 dfe0           fstsw     ax
016f:7cc61195 9e             sahf
25df##u
016f:7cc61196 720e           jc        7cc611a6
016f:7cc61198 d8148f         fcom      dword ptr [edi+ecx*4]
016f:7cc6119b dfe0           fstsw     ax
016f:7cc6119d 9e             sahf
016f:7cc6119e 7306           jnc       7cc611a6
016f:7cc611a0 890a           mov       dword ptr [edx],ecx
016f:7cc611a2 43             inc       ebx
016f:7cc611a3 83c204         add       edx,+04
016f:7cc611a6 83c604         add       esi,+04
016f:7cc611a9 ff4d14         dec       dword ptr [ebp+14]
016f:7cc611ac 75d1           jnz       7cc6117f
016f:7cc611ae 8b4510         mov       eax,dword ptr [ebp+10]
