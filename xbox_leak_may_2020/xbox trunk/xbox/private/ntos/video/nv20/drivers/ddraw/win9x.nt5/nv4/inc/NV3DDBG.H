#ifndef _NV3DDBG_H_
#define _NV3DDBG_H_
/*
 * (C) Copyright NVIDIA Corporation Inc., 1996,1997. All rights reserved.
 */
/********************************* Direct 3D *******************************\
*                                                                           *
* Module: NV3DDBG.H                                                         *
*   Define debug related values.                                            *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Charles Inman (chasi)       11/18/97 - created                      *
*                                                                           *
\***************************************************************************/

/*
 * Bit definitions for dbgLevel debug output level flags.
 */
#define NVDBG_LEVEL_ERROR           0x80000000
#define NVDBG_LEVEL_DDI_ENTRY       0x40000000
#define NVDBG_LEVEL_FUNCTION_CALL   0x20000000
#define NVDBG_LEVEL_INFO            0x10000000
#define NVDBG_LEVEL_INFO2           0x08000000
#define NVDBG_LEVEL_COLORKEY_INFO   0x04000000
#define NVDBG_LEVEL_AA_INFO         0x02000000
#define NVDBG_LEVEL_FLIP            0x01000000
#define NVDBG_LEVEL_FASTLOOPS       0x00800000

/*
 * Bit definitions for the dbgShowState debug flags.
 */
#define NVDBG_SHOW_RENDER_STATE     0x80000000
#define NVDBG_SHOW_DX5_CLASS_STATE  0x40000000
#define NVDBG_SHOW_SURFACE_STATE    0x20000000
#define NVDBG_SHOW_VERTICES         0x10000000
#define NVDBG_SHOW_MTS_STATE        0x0F000000  // Selects number of multi-texture stage state values to display.
#define NVDBG_SHOW_MTS_STATE_MAX    8
#define NVDBG_SHOW_MTS_STATE_SHIFT  24
#define NVDBG_SHOW_DX6_CLASS_STATE  0x00800000
#define NVDBG_SHOW_DRAWPRIM2_INFO   0x00400000

/*
 * Define externs and constants when DEBUG is enabled.
 */
#ifdef  DEBUG
#define DBGFRONTRENDER  1
#define DBGPREVIEWTEX   1
#define DMAVALIDATE     1

/*
 * Bit definitions for the dbgFlush debug flags.
 */
#define NVDBG_FLUSH_TRIANGLE        0x80000000
#define NVDBG_FLUSH_PRIMITIVE       0x40000000
#define NVDBG_FLUSH_DDI             0x20000000

extern BOOL     dbgShowState;
extern BOOL     dbgFlush;
extern DWORD    dbgLeve;
void dbgD3DError(void);
void dbgDisplayContextState(DWORD, PNVD3DCONTEXT);
void dbgDisplayDrawPrimitives2Info(BYTE,WORD);
void dbgDisplayVertexData(DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD);
BOOL dbgTestPushBuffer(PNVD3DCONTEXT);
BOOL dbgValidateControlRegisters(PNVD3DCONTEXT);
extern void __cdecl DPF_LEVEL(DWORD dwLeve, LPSTR szFormat, ...);
#define dbgFlushType(pctxt, type)                                       \
{                                                                       \
    if (dbgFlush & type)                                                \
    {                                                                   \
        NV_D3D_GLOBAL_SAVE();                                           \
        nvFlushDmaBuffers();                                            \
        NV_D3D_GLOBAL_SETUP();                                          \
    }                                                                   \
}
/*
 * Legacy flush routines.
 */
#define dbgFlushDmaBuffers(pctxt)   dbgFlushType(pctxt, NVDBG_FLUSH_PRIMITIVE);
#define dbgFlushTriangle(pctxt)     dbgFlushType(pctxt, NVDBG_FLUSH_TRIANGLE);
#define dbgFlushDDI(pctxt)          dbgFlushType(pctxt, NVDBG_FLUSH_DDI);
#else   // DEBUG
#define dbgShowState                        FALSE
#define dbgTestPushBuffer(noop)             TRUE
#define dbgValidateControlRegisters(noop)   TRUE
#define dbgD3DError()
#define dbgDisplayContextState(f,x)
#define dbgDisplayDrawPrimitives2Info(x,y);
#define dbgDisplayVertexData(x, y, z, w, c, s, u, v);
#define DPF_LEVEL   1 ? (void)0 : (void)
#define dbgFlushDmaBuffers(noop)
#define dbgFlushTriangle(noop)
#define dbgFlushDDI(noop)
#endif  // DEBUG

/*
 * Define externs and constants when DBGFRONTRENDER is defined.
 */
#ifdef  DBGFRONTRENDER
extern BOOL     dbgFrontRender;
extern DWORD    dbgFrontRenderPitch;
#else   // DBGFRONTRENDER
#define dbgFrontRender      FALSE
#define dbgFrontRenderPitch 0
#endif  // DBGFRONTRENDER

/*
 * Define externs and constants when DBGPREVIEWTEX is defined.
 */
#ifdef  DBGPREVIEWTEX
extern BOOL     dbgPreviewTexture;
extern BOOL     dbgPreviewLinearTexture;
extern DWORD    dbgPreviewTexturePitch;
extern BOOL     dbgPreviewTextureClearFirst;
extern DWORD    dbgPreviewTextureMagX;
extern DWORD    dbgPreviewTextureMagY;
void dbgDisplayTexture(PNVD3DCONTEXT, PNVD3DTEXTURE);
void dbgDisplayLinearTexture(LPDDRAWI_DDRAWSURFACE_LCL, LPDDRAWI_DDRAWSURFACE_LCL, DWORD);
#else   // DBGPREVIEWTEX
#define dbgPreviewTexture       FALSE
#define dbgPreviewLinearTexture FALSE
#define dbgPreviewTexturePitch  0
#define dbgPreviewTextureClearFirst FALSE
#define dbgPreviewTextureMagX 1
#define dbgPreviewTextureMagY 1
#define dbgDisplayTexture(x,y)
#define dbgDisplayLinearTexture(x,y,z)
#endif  // DBGPREVIEWTEX

/*
 * Define externs and constants when DMAVALIDATE is defined.
 */
#ifdef  DMAVALIDATE
extern BOOL     dbgValidatePushBuffer;
void dbgValidatePushBufferError(void);
#ifdef  NEW_DMAPUSH
void dbgValidatePushBufferData(DWORD, DWORD);
#else   // OLD_DMAPUSH
void dbgValidatePushBufferData(void);
#endif  // OLD_DMAPUSH
#else   // DMAVALIDATE
#define dbgValidatePushBuffer   FALSE
#define dbgValidatePushBufferError()
#ifdef  NEW_DMAPUSH
#define dbgValidatePushBufferData(x,y)
#else   // OLD_DMAPUSH
#define dbgValidatePushBufferData() 
#endif  // OLD_DMAPUSH
#endif  // DMAVALIDATE

#endif // _NV3DDBG_H_
