#include <stdio.h>
#include <string.h>

//////////////////////////////////////////////////////////////////////////////
// constants

// output files
#define VXMAC_FILE      "nv4vxmac.h"
#define VXIMP_FILE      "nv4vx.c"

// combinations
typedef enum { DX5,DX6 } MTClass;
typedef enum { None,ExpFog,Exp2Fog,LinFog } MTFogMode;
typedef enum { NoAA,AA } MTScaleMode;
typedef enum { FixedVF,FlexVF } MTVFormat;
//typedef enum { List,Strip,Fan,Ind } MTGeometry;
typedef enum { NonIndexed,Indexed,NA } MTIndexType;
typedef enum { w0,w1 } MTW;

// strings
char *szMTClass[]     = { "DX5","DX6" };
char *szMTFogMode[]   = { "","ExpFog","Exp2Fog","LinearFog" };
char *szMTScaleMode[] = { "","AACapture" };
char *szMTVFormat[]   = { "","Flex" };
//char *szMTGeometry[]  = { "List","Strip","Fan","" } ;
char *szMTIndexType[] = { "","Indexed","" } ;
char *szMTW[]         = { "","WBuf"} ;


//////////////////////////////////////////////////////////////////////////////
// macros
#define SUCC(x,y)       (x)(y+1)
#define FOR_ALL_COMBINATIONS_VX\
    for (mtClass=DX5; mtClass<=DX6; mtClass=SUCC(MTClass,mtClass)) {\
      for (mtVFormat=(mtClass==DX6)?FlexVF:FixedVF; mtVFormat<=FlexVF; mtVFormat=SUCC(MTVFormat,mtVFormat)) {\
        for (mtScaleMode=NoAA; mtScaleMode<=AA; mtScaleMode=SUCC(MTScaleMode,SUCC(MTScaleMode,mtScaleMode))) {\
          mtIndexType = NA;/*for (mtIndexType=NonIndexed; mtIndexType<=Indexed; mtIndexType=SUCC(MTIndexType,mtIndexType)) */{\
          for (mtW=w0; mtW<=w1; mtW=SUCC(MTW,mtW)) {\
            for (mtFogMode=None; mtFogMode<=LinFog; mtFogMode=SUCC(MTFogMode,mtFogMode)) {\
              {\
                init();
#define FOR_ALL_COMBINATIONS_PROC\
    for (mtClass=DX5; mtClass<=DX6; mtClass=SUCC(MTClass,mtClass)) {\
      for (mtVFormat=(mtClass==DX6)?FlexVF:FixedVF; mtVFormat<=FlexVF; mtVFormat=SUCC(MTVFormat,mtVFormat)) {\
        for (mtScaleMode=NoAA; mtScaleMode<=AA; mtScaleMode=SUCC(MTScaleMode,mtScaleMode)) {\
          for (mtIndexType=NonIndexed; mtIndexType<=Indexed; mtIndexType=SUCC(MTIndexType,mtIndexType)) {\
            for (mtW=w0; mtW<=w1; mtW=SUCC(MTW,mtW)) {\
              for (mtFogMode=None; mtFogMode<=((mtScaleMode!=NoAA)?None:LinFog); mtFogMode=SUCC(MTFogMode,mtFogMode)) {\
                {/*for (mtGeometry=(mtIndexType==Indexed?Ind:List); mtGeometry<=(mtIndexType==Indexed?Ind:Fan); mtGeometry=SUCC(MTGeometry,mtGeometry)) {*/\
                  init();
#define NEXT    } } } } } } }

//////////////////////////////////////////////////////////////////////////////
// globals
FILE       *fHdr;
FILE       *fImp;
MTClass     mtClass;
MTVFormat   mtVFormat;
MTScaleMode mtScaleMode;
MTFogMode   mtFogMode;
MTW         mtW;
//MTGeometry  mtGeometry;
MTIndexType mtIndexType;
char        szTableName[512];
char        szProcName[512];
char        szVertexName[512];
char        szILParameter[512];

/*
typedef void  (*LPNVINDEXEDPRIM) (DWORD, LPWORD, DWORD,DWORD, LPD3DTLVERTEX);
typedef void  (*LPNVDRAWPRIM) (WORD, DWORD,DWORD, LPD3DTLVERTEX);

typedef void  (*LPNVFVFINDEXEDPRIM) (DWORD, LPWORD, DWORD,DWORD, LPBYTE);
typedef void  (*LPNVFVFDRAWPRIM) (WORD, DWORD,DWORD, LPBYTE);
*/

char *szHeader = 
"/*\n"
" * **** MACHINE GENERATED - DO NOT EDIT ****\n"
" * **** MACHINE GENERATED - DO NOT EDIT ****\n"
" * **** MACHINE GENERATED - DO NOT EDIT ****\n"
" *\n"
" * Copyright 1993-1998 NVIDIA, Corporation.  All rights reserved.\n"
" *\n"
" * THE INFORMATION CONTAINED HEREIN IS PROPRIETARY AND CONFIDENTIAL TO\n"
" * NVIDIA, CORPORATION.  USE, REPRODUCTION OR DISCLOSURE TO ANY THIRD PARTY\n"
" * IS SUBJECT TO WRITTEN PRE-APPROVAL BY NVIDIA, CORPORATION.\n"
" */\n\n"
"/********************************* Direct 3D *******************************\\\n"
"*                                                                           *\n"
"* Module: NV4VX.C                                                           *\n"
"*   NV4 Triangle Inner Loops. (Originally Machine Generated)                *\n"
"*                                                                           *\n"
"*****************************************************************************\n"
"*                                                                           *\n"
"* History:                                                                  *\n"
"*       Ben de Waal                 05/23/98 - NV4 development.             *\n"
"*                                                                           *\n"
"\\***************************************************************************/\n"
"#include <math.h>\n"
"#include <windows.h>\n"
"#include \"ddrawi.h\"\n"
"#include \"ddrvmem.h\"\n"
"#include \"ddmini.h\"\n"
"#include \"nv432.h\"\n"
"#include \"nvddobj.h\"\n"
"#include \"d3d.h\"\n"
"#include \"d3dhal.h\"\n"
"#include \"d3dinc.h\"\n"
"#include \"nvd3dmac.h\"\n"
"#include \"nv3ddbg.h\"\n"
"#include \"nv4vxmac.h\"\n"
"\n"
"#ifdef NV_FASTLOOPS\n"
"\n"
"DWORD dwDrawPrimitiveTable[] =\n"
"{\n"
"   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,\n"
"   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,\n"
"   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,\n"
"   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, /*0x040*/\n"
"   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,\n"
"   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,\n"
"   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,\n"
"   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, /*0x080*/\n"
"   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,\n"
"   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,\n"
"   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,\n"
"   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, /*0x0c0*/\n"
"   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,\n"
"   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,\n"
"   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,\n"
"   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, /*0x100*/\n"
"   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,\n"
"   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,\n"
"   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,\n"
"   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, /*0x140*/\n"
"   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,\n"
"   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,\n"
"   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,\n"
"   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, /*0x180*/\n"
"   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,\n"
"   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,\n"
"   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,\n"
"   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, /*0x1c0*/\n"
"   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,\n"
"   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,\n"
"   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,\n"
"   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, /*0x200*/\n"
"   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,\n"
"   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,\n"
"   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,\n"
"   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, /*0x240*/\n"
"   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,\n"
"   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,\n"
"   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,\n"
"   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, /*0x280*/\n"
"   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,\n"
"   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,\n"
"   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,\n"
"   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, /*0x2c0*/\n"
"   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,\n"
"   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,\n"
"   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,\n"
"   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, /*0x300*/\n"
"   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,\n"
"   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,\n"
"   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,\n"
"   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, /*0x340*/\n"
"   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,\n"
"   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,\n"
"   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,\n"
"   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, /*0x380*/\n"
"   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,\n"
"   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,\n"
"   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,\n"
"   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, /*0x3c0*/\n"
"   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,\n"
"   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,\n"
"   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,\n"
"   0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, /*0x400*/\n"
"   (DWORD)nvAACapturePrimitive,\n"
"   (DWORD)nvDX5TriangleSetup,\n"
"   (DWORD)nvAACapturePrimitive,\n"
"   (DWORD)nvDX6TriangleSetup,\n"
"};\n"
"\n"
"#else //~NV_FASTLOOPS\n"
"\n";

//////////////////////////////////////////////////////////////////////////////
// create names
void init (void) {
// inner loops
    sprintf(szProcName,"nv%s%sTriangle%s%s%s%s",
                        szMTClass[mtClass],
                        szMTVFormat[mtVFormat],
                        szMTFogMode[mtFogMode],
                        szMTIndexType[mtIndexType],
                        //szMTGeometry[mtGeometry],
                        szMTScaleMode[mtScaleMode],
                        szMTW[mtW]);
    while (strlen(szProcName)<40) strcat (szProcName," ");
    if (mtVFormat == FlexVF) {
        if (mtIndexType == Indexed) {
            strcpy (szILParameter,"DWORD, LPWORD, DWORD, LPBYTE");
        } else {
            strcpy (szILParameter," WORD,         DWORD, LPBYTE");
        }
    } else {
        if (mtIndexType == Indexed) {
            strcpy (szILParameter,"DWORD, LPWORD, DWORD, LPD3DTLVERTEX");
        } else {
            strcpy (szILParameter," WORD,         DWORD, LPD3DTLVERTEX");
        }
    }
// vertex macro
    sprintf(szVertexName,"nvgl%s%sTriangleVertex%s%s%s",
                        szMTClass[mtClass],
                        szMTVFormat[mtVFormat],
                        /*szMTIndexType[mtIndexType],*/
                        szMTScaleMode[mtScaleMode],
                        szMTFogMode[mtFogMode],
                        szMTW[mtW]);
}

//////////////////////////////////////////////////////////////////////////////
// main
int main (void) {
// create files
    fHdr = fopen(VXMAC_FILE,"wt");
    if (!fHdr) return 1;
    fImp = fopen(VXIMP_FILE,"wt");
    if (!fImp) return 1;

// emit basic stuff
    fprintf (fHdr,"/*\n");
    fprintf (fHdr," * **** MACHINE GENERATED - DO NOT EDIT ****\n");
    fprintf (fHdr," * **** MACHINE GENERATED - DO NOT EDIT ****\n");
    fprintf (fHdr," * **** MACHINE GENERATED - DO NOT EDIT ****\n");
    fprintf (fHdr," *\n");
    fprintf (fHdr," * Copyright 1993-1998 NVIDIA, Corporation.  All rights reserved.\n");
    fprintf (fHdr," *\n");
    fprintf (fHdr," * THE INFORMATION CONTAINED HEREIN IS PROPRIETARY AND CONFIDENTIAL TO\n");
    fprintf (fHdr," * NVIDIA, CORPORATION.  USE, REPRODUCTION OR DISCLOSURE TO ANY THIRD PARTY\n");
    fprintf (fHdr," * IS SUBJECT TO WRITTEN PRE-APPROVAL BY NVIDIA, CORPORATION.\n");
    fprintf (fHdr," */\n\n");
    fprintf (fHdr,"/*\n * Inner Loop Prototypes and Vertex Macros\n */\n\n");
    fprintf (fHdr,"/*\n * General Constants\n */\n");
    fprintf (fHdr,"#define NV_FORCE_TRI_SETUP(_ctx) (_ctx)->dwFunctionLookup = NV_SETUP_FUNCTION_INDEX;\n\n");
    fprintf (fHdr,"#ifdef NV_FASTLOOPS\n");
    fprintf (fHdr,"#define NV_DX6_FUNCTION_ADJUST      2\n");
    fprintf (fHdr,"#define NV_FIXED_FUNCTION_INDEX     1024\n");
    fprintf (fHdr,"#define NV_AA_FUNCTION_INDEX        1024\n");
    fprintf (fHdr,"#define NV_SETUP_FUNCTION_INDEX     1025\n");
    fprintf (fHdr,"#else //!NV_FASTLOOPS\n");
    fprintf (fHdr,"#define NV_AA_FUNCTION_INDEX        8\n");
    fprintf (fHdr,"#define NV_SETUP_FUNCTION_INDEX     10\n");
    fprintf (fHdr,"#endif //!NV_FASTLOOPS\n");
    fprintf (fHdr,"\n");
    fprintf (fHdr,"#define sizeDX5TriangleVertex       9\n");
    fprintf (fHdr,"#define sizeDx5TriangleTLVertex     sizeDX5TriangleVertex\n");
    fprintf (fHdr,"\n");
    fprintf (fHdr,"#define sizeDX6TriangleVertex       11\n");
    fprintf (fHdr,"#define sizeDx6TriangleTLVertex     sizeDX6TriangleVertex\n");
    fprintf (fHdr,"\n");
    fprintf (fHdr,"#define LIST_STRIDES                0x00000303\n");
    fprintf (fHdr,"#define STRIP_STRIDES               0x01010101\n");
    fprintf (fHdr,"#define FAN_STRIDES                 0x02000001\n");
    fprintf (fHdr,"#define LEGACY_STRIDES              0x03000404\n");
    fprintf (fHdr,"\n");

    fprintf (fImp,"%s",szHeader);

// emit inner loop prototypes
    fprintf (fHdr,"#ifdef NV_FASTLOOPS\n\n");
    fprintf (fHdr,"void __stdcall nvTriangleDispatch (DWORD, LPWORD, DWORD, LPBYTE);\n");
    fprintf (fHdr,"void __stdcall nvDX5TriangleSetup (DWORD, LPWORD, DWORD, LPBYTE);\n");
    fprintf (fHdr,"void __stdcall nvDX6TriangleSetup (DWORD, LPWORD, DWORD, LPBYTE);\n\n");
    fprintf (fHdr,"extern DWORD dwDrawPrimitiveTable[];\n\n");
    fprintf (fHdr,"#else //!NV_FASTLOOPS\n\n");

    fprintf (fHdr,"/*\n * Inner Loop Prototypes\n */\n");
    FOR_ALL_COMBINATIONS_PROC
        fprintf (fHdr,"void %s (%s);\n",szProcName,szILParameter);
    NEXT
    fprintf (fHdr,"\n");

// emit inner loop tables
    fprintf (fImp,"/*\n * Function Tables\n */\n");
    for (mtClass=DX5; mtClass<=DX6; mtClass=SUCC(MTClass,mtClass)) {\
      for (mtVFormat=(mtClass==DX6)?FlexVF:FixedVF; mtVFormat<=FlexVF; mtVFormat=SUCC(MTVFormat,mtVFormat)) {\
        for (mtIndexType=NonIndexed; mtIndexType<=Indexed; mtIndexType=SUCC(MTIndexType,mtIndexType)) {\
            /*for (mtGeometry=(mtIndexType==Indexed?Ind:List); mtGeometry<=(mtIndexType==Indexed?Ind:Fan); mtGeometry=SUCC(MTGeometry,mtGeometry)) */{\
            char sz[256];
            sprintf(sz,"%s%s%s",
                        szMTClass[mtClass],
                        szMTVFormat[mtVFormat],
                        szMTIndexType[mtIndexType]/*,
                        szMTGeometry[mtGeometry]*/);
            char *rt = (mtIndexType == Indexed) ? ((mtVFormat == FixedVF) ? "LPNVINDEXEDPRIM" : "LPNVFVFINDEXEDPRIM")
                                                : ((mtVFormat == FixedVF) ? "LPNVDRAWPRIM" : "LPNVFVFDRAWPRIM");
            sprintf(szTableName,"%s fn%sTable",rt,sz);
            fprintf (fImp,"%s[] =\n{\n",szTableName);
            for (mtScaleMode=NoAA; mtScaleMode<=AA; mtScaleMode=SUCC(MTScaleMode,mtScaleMode)) {\
              for (mtW=w0; mtW<=w1; mtW=SUCC(MTW,mtW)) {\
                for (mtFogMode=None; mtFogMode<=((mtScaleMode!=NoAA)?None:LinFog); mtFogMode=SUCC(MTFogMode,mtFogMode)) {\
                  init();
                  fprintf (fImp,"    %s,\n",szProcName);
                }
              }
            }
            fprintf (fImp,"    nv%sTriangleSetup\n",sz);
            fprintf (fImp,"};\n\n");

                        fprintf (fHdr,"extern %s fn%sTable[];\n",rt,sz);
                        fprintf (fHdr,"void nv%sTriangleSetup  (%s);\n",sz,szILParameter);
          }
        }
      }
    }
    fprintf (fImp,"\n");
                        fprintf (fHdr,"\n");

// emit inner loops
    fprintf (fImp,"/*\n * Function declarations\n */\n\n");
    FOR_ALL_COMBINATIONS_PROC
        char *size = (mtClass == DX5) ? "sizeDX5TriangleVertex" : "sizeDX6TriangleVertex";
                                    fprintf (fImp,"// %s\n",szProcName);
                                    fprintf (fImp,"#define  PROC_        \"%s\"\n",szProcName);
                                    fprintf (fImp,"#define  PROC         %s\n",szProcName);
                                    fprintf (fImp,"#define  VERTEX       %s\n",szVertexName);
        if (mtClass == DX6)         fprintf (fImp,"#define  DX6\n");
                                    fprintf (fImp,"#define  VXSIZE       %s\n",size);
        if (mtFogMode != None)      fprintf (fImp,"#define  FOG          %d\n",mtFogMode);
        if (mtW == w1)              fprintf (fImp,"#define  WBUFFER\n");
        if (mtIndexType == Indexed) fprintf (fImp,"#define  INDEXED\n");
        if (mtVFormat == FlexVF)    fprintf (fImp,"#define  FVF\n");
        if (mtScaleMode == AA)      fprintf (fImp,"#define  ANTIALIAS\n");
        //if (mtIndexType != Indexed) fprintf (fImp,"#define  GEOMETRY     %d\n",mtGeometry);
                                    //fprintf (fImp,"#ifdef NV_FASTLOOPS2\n");
                                    //fprintf (fImp,"#include \"nv4fast.c\"\n");
                                    //fprintf (fImp,"#else //!NV_FASTLOOPS2\n");
                                    fprintf (fImp,"#include \"nv4loop.c\"\n");
                                    //fprintf (fImp,"#endif //!NV_FASTLOOPS2\n");
        //if (mtIndexType != Indexed) fprintf (fImp,"#undef   GEOMETRY\n");
        if (mtScaleMode == AA)      fprintf (fImp,"#undef   ANTIALIAS\n");
        if (mtVFormat == FlexVF)    fprintf (fImp,"#undef   FVF\n");
        if (mtIndexType == Indexed) fprintf (fImp,"#undef   INDEXED\n");
        if (mtW == w1)              fprintf (fImp,"#undef   WBUFFER\n");
        if (mtFogMode != None)      fprintf (fImp,"#undef   FOG\n");
                                    fprintf (fImp,"#undef   VXSIZE\n");
        if (mtClass == DX6)         fprintf (fImp,"#undef   DX6\n");
                                    fprintf (fImp,"#undef   VERTEX\n");
                                    fprintf (fImp,"#undef   PROC\n");
                                    fprintf (fImp,"#undef   PROC_\n");
        fprintf (fImp,"\n");
    NEXT

// emit vertex macros
    fprintf (fHdr,"#endif //!NV_FASTLOOPS\n");

    fprintf (fHdr,"/*\n * Vertex Macros\n */\n\n");
    FOR_ALL_COMBINATIONS_VX
        char *size = (mtClass == DX5) ? "sizeDX5TriangleVertex" : "sizeDX6TriangleVertex";
            //fprintf (fHdr,"// vertex macro for %s\n",szProcName);
            fprintf (fHdr,"#define %s(fifo,freecount,ch,alias,vertex%s)\\\n",szVertexName,
                            (mtVFormat == FlexVF) ? ",fvf,uvoff" : "");

        if (!strcmp(szVertexName,"nvglDX5TriangleVertex"))
        {
            fprintf (fHdr,"{\\\n");
            fprintf (fHdr,"    *(DWORD*)(fifo +  0) = ((sizeDX5TriangleVertex-1)<<18) | ((ch)<<13) | NV054_TLVERTEX((alias));\\\n");
            fprintf (fHdr,"    memcpy ((void*)(fifo + 4),(void*)(vertex),32);\\\n");
            /*
            fprintf (fHdr,"    __asm { mov eax,[vertex]              }\\\n");
            fprintf (fHdr,"    __asm { mov ebx,[fifo]                }\\\n");
            fprintf (fHdr,"    __asm { movq mm0,qword ptr [eax+ 0]   }\\\n");
            fprintf (fHdr,"    __asm { movq mm1,qword ptr [eax+ 8]   }\\\n");
            fprintf (fHdr,"    __asm { movq mm2,qword ptr [eax+16]   }\\\n");
            fprintf (fHdr,"    __asm { movq mm3,qword ptr [eax+24]   }\\\n");
            fprintf (fHdr,"    __asm { movq qword ptr [ebx+ 0+4],mm0 }\\\n");
            fprintf (fHdr,"    __asm { movq qword ptr [ebx+ 8+4],mm1 }\\\n");
            fprintf (fHdr,"    __asm { movq qword ptr [ebx+16+4],mm2 }\\\n");
            fprintf (fHdr,"    __asm { movq qword ptr [ebx+24+4],mm3 }\\\n");
            fprintf (fHdr,"    __asm { emms                          }\\\n");
            */

        }
        else
        {
            fprintf (fHdr,"{\\\n");
            fprintf (fHdr,"    DWORD dwTemp;\\\n");
            if (mtFogMode != None) {
                fprintf (fHdr,"    DWORD zmask,dwrhw,dwz;\\\n");
                fprintf (fHdr,"    int   ifog;\\\n");
                fprintf (fHdr,"    float ffog,fz,fw;\\\n");
            }
            if ((mtW == w1) || (mtFogMode != None)) {
                fprintf (fHdr,"    float frhw;\\\n");
            }
            // placement
            fprintf (fHdr,"    *(DWORD*)(fifo +  0) = ((%s-1)<<18) | ((ch)<<13) | %s((alias));\\\n",size,(mtClass == DX5) ? "NV054_TLVERTEX" : "NV055_TLMTVERTEX");
            // sx & sy
            if (mtScaleMode == AA) {
                fprintf (fHdr,"    *(float*)(fifo +  4) = (*(float*)(vertex +  0)) * fAAScaleX + fAAOffsetX;\\\n");
                fprintf (fHdr,"    *(float*)(fifo +  8) = (*(float*)(vertex +  4)) * fAAScaleY + fAAOffsetY;\\\n");
            } else {
                fprintf (fHdr,"    *(DWORD*)(fifo +  4) = (*(DWORD*)(vertex +  0));\\\n");
                fprintf (fHdr,"    *(DWORD*)(fifo +  8) = (*(DWORD*)(vertex +  4));\\\n");
            }
            // sz
            fprintf (fHdr,"    *(DWORD*)(fifo + 12) = ");
            if (mtFogMode != None) fprintf (fHdr,"dwz = ");
            fprintf (fHdr,"(*(DWORD*)(vertex +  8));\\\n");
            // rhw
            if (mtVFormat == FlexVF) {
                fprintf (fHdr,"    dwTemp = *(DWORD*)((vertex & fvf.dwRHWMask) + fvf.dwRHWOffset);\\\n");
            } else {
                fprintf (fHdr,"    dwTemp = (*(DWORD*)(vertex + 12));\\\n");
            }
            if (mtW == w1) {
                fprintf (fHdr,"    frhw = (*(float*)&dwTemp) * pCurrentContext->dvRWFar;\\\n");
                fprintf (fHdr,"    *(DWORD*)(fifo + 16) = ");
                if (mtFogMode != None) fprintf (fHdr,"dwrhw = ");
                fprintf (fHdr,"*(DWORD*)&frhw;\\\n");
            }
            else {
                fprintf (fHdr,"    *(DWORD*)(fifo + 16) = ");
                if (mtFogMode != None) fprintf (fHdr,"dwrhw = ");
                fprintf (fHdr,"dwTemp;\\\n");
            }
            // color
            if (mtVFormat == FlexVF) {
                fprintf (fHdr,"    *(DWORD*)(fifo + 20) = *(DWORD*)((vertex & fvf.dwARGBMask) + fvf.dwARGBOffset);\\\n");
            } else {
                fprintf (fHdr,"    *(DWORD*)(fifo + 20) = (*(DWORD*)(vertex + 16));\\\n");
            }
            // specular
            if (mtVFormat == FlexVF) {
                fprintf (fHdr,"    dwTemp = *(DWORD*)((vertex & fvf.dwSpecularMask) + fvf.dwSpecularOffset);\\\n");
            } else {
                fprintf (fHdr,"    dwTemp = (*(DWORD*)(vertex + 20));\\\n");
            }
            // fog
            if (mtFogMode != None) {
                // wnear and wfar both equal to 1.0 means the projection is affine
                fprintf (fHdr,"    zmask = ((*(DWORD *)&(pCurrentContext->dvWNear) == 0x3f800000) && (*(DWORD *)&(pCurrentContext->dvWFar) == 0x3f800000)) ? 0xffffffff : 0;\\\n");
                fprintf (fHdr,"    frhw = *(float*)&dwrhw;\\\n");
                fprintf (fHdr,"    FP_INV (fw, frhw);\\\n");
                fprintf (fHdr,"    dwz = (dwz & zmask) | (*(DWORD *)&fw & ~zmask);\\\n");  // select between z and 1/(1/rhw)
                fprintf (fHdr,"    fz = *(float *)&dwz;\\\n");
                if (mtFogMode == LinFog) {
                    fprintf (fHdr,"    ffog = (fz >= pCurrentContext->fFogTableEnd) ? 0.0f :\\\n");
                    fprintf (fHdr,"           (fz <= pCurrentContext->fFogTableStart) ? 1.0f :\\\n");
                    fprintf (fHdr,"           (pCurrentContext->fFogTableEnd - fz) * pCurrentContext->fFogTableLinearScale;\\\n");
                } else {
                    fprintf (fHdr,"    ffog  = fz * pCurrentContext->fFogTableDensity;\\\n");
                    if (mtFogMode == Exp2Fog) {
                        fprintf (fHdr,"    ffog *= ffog;\\\n");
                    }
                    fprintf (fHdr,"    FP_EXP (ffog,ffog);\\\n");
                }
                fprintf (fHdr,"    FP_NORM_TO_BIT24 (ifog,ffog);\\\n");
                fprintf (fHdr,"    dwTemp = (dwTemp & 0x00ffffff) | ifog;\\\n");
            }
            fprintf (fHdr,"    *(DWORD*)(fifo + 24) = dwTemp;\\\n");
            // uv (uv2)
            if (mtVFormat == FlexVF) {
                fprintf (fHdr,"    dwTemp = (vertex & fvf.dwUVMask) + fvf.dwUVOffset;\\\n");
                fprintf (fHdr,"    *(DWORD*)(fifo + 28) = *(DWORD*)(dwTemp + (uvoff)[0] + 0);\\\n");
                fprintf (fHdr,"    *(DWORD*)(fifo + 32) = *(DWORD*)(dwTemp + (uvoff)[0] + 4);\\\n");
                if (mtClass == DX6) {
                    fprintf (fHdr,"    *(DWORD*)(fifo + 36) = *(DWORD*)(dwTemp + (uvoff)[1] + 0);\\\n");
                    fprintf (fHdr,"    *(DWORD*)(fifo + 40) = *(DWORD*)(dwTemp + (uvoff)[1] + 4);\\\n");
                }
            } else {
                fprintf (fHdr,"    *(DWORD*)(fifo + 28) = (*(DWORD*)(vertex + 24));\\\n");
                fprintf (fHdr,"    *(DWORD*)(fifo + 32) = (*(DWORD*)(vertex + 28));\\\n");
                if (mtClass == DX6) {
                    fprintf (fHdr,"    *(DWORD*)(fifo + 36) = 0;\\\n");
                    fprintf (fHdr,"    *(DWORD*)(fifo + 40) = 0;\\\n");
                }
            }
        }
            fprintf (fHdr,"    dbgDisplayVertexData(((DWORD*)nvFifo)[1],((DWORD*)nvFifo)[2],((DWORD*)nvFifo)[3],\\\n");
            fprintf (fHdr,"                         ((DWORD*)nvFifo)[4],((DWORD*)nvFifo)[5],((DWORD*)nvFifo)[6],\\\n");
            fprintf (fHdr,"                         ((DWORD*)nvFifo)[7],((DWORD*)nvFifo)[8]);\\\n");
            fprintf (fHdr,"    fifo      += %s * 4;\\\n",size);
            fprintf (fHdr,"    freecount -= %s;\\\n",size);
            fprintf (fHdr,"}\n\n");
    NEXT

    fprintf (fImp,"#endif //!NV_FASTLOOPS\n");

// close files
    fclose (fHdr);
    fclose (fImp);

// done
    return 0;
}
