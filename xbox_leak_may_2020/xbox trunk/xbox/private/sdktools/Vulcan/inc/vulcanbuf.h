/***********************************************************************
* Microsoft Vulcan
*
* Microsoft Confidential.  Copyright 1997-1999 Microsoft Corporation.
*
* File: vulcanbuf.h
*
* File Comments:
*
*
***********************************************************************/

#ifndef __VBUF__
#define __VBUF__

#define STRICT
#define WIN32_LEAN_AND_MEAN
#include "windows.h"

typedef void  (__cdecl *VBUF_FP)();

void VULCANCALL VBuf_VPro(LPVOID addr)
{
    DWORD temp;

    VirtualProtect(addr, 4 , PAGE_EXECUTE_READWRITE, &temp);
}


#define WIN9X_TLS 0x2c
int TLS_CONST = ((GetVersion() & 0x80000000) == 0)?0xe10:0;
int VBUF_ISWINNT = ((GetVersion() & 0x80000000) == 0)?1:0;

#define VBuf_MOVEARG1 \
        __asm mov eax,[esp+20+4*0]\
        __asm mov [ebx+4*0],eax\

#define VBuf_MOVEARG2 \
        VBuf_MOVEARG1 \
        __asm mov eax,[esp+20+4*1]\
        __asm mov [ebx+4*1],eax\

#define VBuf_MOVEARG3 \
        VBuf_MOVEARG2 \
        __asm mov eax,[esp+20+4*2]\
        __asm mov [ebx+4*2],eax\

#define VBuf_MOVEARG4 \
        VBuf_MOVEARG3 \
        __asm mov eax,[esp+20+4*3]\
        __asm mov [ebx+4*3],eax\

#define VBuf_MOVEARG5 \
        VBuf_MOVEARG4 \
        __asm mov eax,[esp+20+4*4]\
        __asm mov [ebx+4*4],eax\


//  This #define creates 3 functions.  Function NAME is a stub which will
//  change the import of the caller to either the winnt or win9x function.
//  The other two functions basically pull the arguments off the stack
//  and place them in a buffer.  When the buffer is full it calls flush.
//  For optimization reason this was written in asm.  Mainly to avoid using pushfd.
//  This prevents any instruction from touching the flags register in the general case.
//  For Instance LEAs are used instead of ADD instructions.



#define VBUFMT(NAME,RTNAME,NUMARGS,BUFSIZE) \
int NAME##VBuf_SlotIndex = TlsAlloc();\
int NAME##VBuf_SlotData = TlsAlloc();\
int NAME##VBuf_SLOTINDEX = NAME##VBuf_SlotIndex * 4 + TLS_CONST;\
int NAME##VBuf_SLOTDATA  = NAME##VBuf_SlotData  * 4 + TLS_CONST;\
extern "C" void NAME##_VBuf_Flush() {\
        DWORD dw = GetLastError();\
        if (TlsGetValue(NAME##VBuf_SlotData)) {\
            RTNAME(TlsGetValue(NAME##VBuf_SlotData),(void*)(BUFSIZE-((int)TlsGetValue(NAME##VBuf_SlotIndex))/(4*NUMARGS) ));\
        } else {\
            void* foo = (void*)new int[NUMARGS*BUFSIZE*4]; \
            TlsSetValue( NAME##VBuf_SlotData,foo );\
        }\
        TlsSetValue(NAME##VBuf_SlotIndex,(void*)(BUFSIZE*NUMARGS*4));\
        SetLastError(dw);\
}\
extern "C" __declspec(dllexport) __declspec(naked) void NAME##WINNT() {\
    __asm {\
        __asm push edx\
        __asm push ecx\
        __asm push ebx\
        __asm push eax\
        __asm mov edx,[NAME##VBuf_SLOTINDEX]\
        __asm mov ecx,fs:[edx]\
        __asm jecxz flush\
    __asm copyargs:  \
        __asm mov eax,[NAME##VBuf_SLOTDATA]\
        __asm mov ebx,fs:[eax]\
\
        __asm lea eax,[ecx-(BUFSIZE*NUMARGS*4)]\
        __asm not eax\
        __asm lea eax,[eax+1]\
        __asm lea ebx,[ebx+eax]\
\
        VBuf_MOVEARG##NUMARGS \
\
        __asm lea ecx,[ecx+(-NUMARGS*4)]\
        __asm mov fs:[edx],ecx\
\
        __asm pop eax\
        __asm pop ebx\
        __asm pop ecx\
        __asm pop edx\
        __asm ret\
    __asm flush: \
        __asm pushad\
        __asm pushfd\
        __asm cld\
        __asm call NAME##_VBuf_Flush\
        __asm popfd\
        __asm popad\
        __asm mov ecx,[NAME##VBuf_SLOTINDEX]\
        __asm mov ecx,fs:[ecx]\
        __asm jmp copyargs\
    }\
}\
extern "C" __declspec(dllexport) __declspec(naked) void NAME##WIN9X() {\
    __asm {\
        __asm push edx\
        __asm push ecx\
        __asm push ebx\
        __asm push eax\
        __asm mov edx,[NAME##VBuf_SLOTINDEX]\
        __asm mov ecx,fs:[WIN9X_TLS]\
        __asm lea ecx,[ecx+edx]\
        __asm mov ecx,[ecx]\
        __asm jecxz flush\
    __asm copyargs:  \
        __asm mov eax,[NAME##VBuf_SLOTDATA]\
        __asm mov ebx,fs:[WIN9X_TLS]\
        __asm lea ebx,[ebx+eax]\
        __asm mov ebx,[ebx]\
\
        __asm lea eax,[ecx-(BUFSIZE*NUMARGS*4)]\
        __asm not eax\
        __asm lea eax,[eax+1]\
        __asm lea ebx,[ebx+eax]\
\
        VBuf_MOVEARG##NUMARGS \
\
        __asm lea ecx,[ecx+(-NUMARGS*4)]\
        __asm mov ebx,fs:[WIN9X_TLS]\
        __asm lea edx,[ebx+edx]\
        __asm mov [edx],ecx\
\
        __asm pop eax\
        __asm pop ebx\
        __asm pop ecx\
        __asm pop edx\
        __asm ret\
    __asm flush: \
        __asm pushad\
        __asm pushfd\
        __asm cld\
        __asm call NAME##_VBuf_Flush\
        __asm popfd\
        __asm popad\
        __asm mov edx,[NAME##VBuf_SLOTINDEX]\
        __asm mov ecx,fs:[WIN9X_TLS]\
        __asm lea ecx,[ecx+edx]\
        __asm mov ecx,[ecx]\
        __asm jmp copyargs\
    }\
}\
VBUF_FP NAME##_VBUF_WINNT = NAME##WINNT;\
VBUF_FP NAME##_VBUF_WIN9X = NAME##WIN9X;\
extern "C" __declspec(dllexport) __declspec(naked) void NAME() {\
    __asm {\
        __asm push eax\
        __asm mov eax,[esp+4]\
        __asm lea eax,[eax-4]\
        __asm mov eax,[eax]\
        __asm pushad\
        __asm pushfd\
        __asm push eax\
        __asm push eax\
        __asm call VBuf_VPro\
        __asm pop eax\
        __asm mov ecx,[VBUF_ISWINNT]\
        __asm mov ebx,[NAME##_VBUF_WIN9X]\
        __asm jecxz iswin9x\
        __asm mov ebx,[NAME##_VBUF_WINNT]\
        __asm mov [eax],ebx\
        __asm popfd\
        __asm popad\
        __asm pop eax\
        __asm jmp [NAME##_VBUF_WINNT]\
    __asm iswin9x:\
        __asm mov [eax],ebx\
        __asm popfd\
        __asm popad\
        __asm pop eax\
        __asm jmp [NAME##_VBUF_WIN9X]\
    }\
}\

#endif
