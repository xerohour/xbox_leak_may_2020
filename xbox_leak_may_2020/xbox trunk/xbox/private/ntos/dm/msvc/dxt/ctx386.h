/*
 *
 * ctx386.h
 *
 * Context definition for win32 x86
 *
 */

#ifndef _CTX386_h
#define _CTX386_h

typedef CONTEXT XBDMCTX;
typedef FLOATING_SAVE_AREA XBDMFLOATSAVE;

#define CONTEXT CTX86
#define _CONTEXT _CTX86
#define PCONTEXT PCTX86
#define LPCONTEXT PCONTEXT
#define FLOATING_SAVE_AREA FSAVEX86
#define _FLOATING_SAVE_AREA _FSAVEX86
#define PFLOATING_SAVE_AREA PFSAVEX86

typedef struct _FLOATING_SAVE_AREA {
    DWORD   ControlWord;
    DWORD   StatusWord;
    DWORD   TagWord;
    DWORD   ErrorOffset;
    DWORD   ErrorSelector;
    DWORD   DataOffset;
    DWORD   DataSelector;
    BYTE    RegisterArea[80];
    DWORD   Cr0NpxState;
} FLOATING_SAVE_AREA;

typedef FLOATING_SAVE_AREA *PFLOATING_SAVE_AREA;

typedef struct _CONTEXT {
    DWORD ContextFlags;
    DWORD   Dr0;
    DWORD   Dr1;
    DWORD   Dr2;
    DWORD   Dr3;
    DWORD   Dr6;
    DWORD   Dr7;
    FLOATING_SAVE_AREA FloatSave;
    DWORD   SegGs;
    DWORD   SegFs;
    DWORD   SegEs;
    DWORD   SegDs;
    DWORD   Edi;
    DWORD   Esi;
    DWORD   Ebx;
    DWORD   Edx;
    DWORD   Ecx;
    DWORD   Eax;
    DWORD   Ebp;
    DWORD   Eip;
    DWORD   SegCs;
    DWORD   EFlags;
    DWORD   Esp;
    DWORD   SegSs;
    BYTE    ExtendedRegisters[512];
} CONTEXT, *PCONTEXT;

#endif // _CTX386_h
