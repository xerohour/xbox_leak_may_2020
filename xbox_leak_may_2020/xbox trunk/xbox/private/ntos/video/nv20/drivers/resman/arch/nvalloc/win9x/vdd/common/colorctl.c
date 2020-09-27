#ifndef NV3_HW
#define COMPILEDINVDD
#else
#undef COMPILEDINVDD
#endif 

#ifdef COMPILEDINVDD


#ifdef NV4_HW
#define NV4
#endif
#ifdef NV10_HW
#define NV4
#endif


#ifdef DEBUG
#define TIMEOUT_DMA_XFER 500
#else
#define TIMEOUT_DMA_XFER 42
#endif

// ------------------------------------------------------------------------------------------
// YUV422ColourControl
//      Simulates colour controls on YUV422 surfaces.
//      Returns TRUE if success

#ifdef COMPILEDINVDD //------------------------------------------------------
#define NV_WIN_DEVICE                            0x00002002

#define NV4
#include "windows.h"
#include "ddraw.h"
#include "Ddrawp.h"
#include "ddrawi.h"
// #include "nvPusher.h"
#include "ddmini.h"
#include "dvp.h"
#include "dmemmgr.h"
//  vdd specific MS includes
#include "minivdd.h"
#include "ddkmmini.h"

#include "vmm.h"

// NV includes
#include "nvwin32.h"
#include "nvhw.h"
#include <nv_ref.h>
#include "nvrmr0api.h"

#include "nvddobj.h"
#include "surfaces.h"
#include "timing.h"


extern vpSurfaces             MySurfaces;

extern DWORD vwin32WaitSingleObject( DWORD, DWORD, DWORD );
extern DWORD vwin32ResetWin32Event(DWORD);
extern DWORD vwin32TimeSliceSleep(DWORD);

// requires just the handle passed in

//#undef POLLINGMODE
#define POLLINGMODE

#ifdef POLLINGMODE
#define R0SLEEP(a)        \
{                         \
     _asm { \
    emms \
   }; \
vwin32TimeSliceSleep(a);   \
}
#endif


#define PDRIVERDATA (MySurfaces.pDriverData)

#define WAIT_FOR_FIFO(n) { \
        while (videoFreeCount < n) { \
            NvRmR0Interrupt (((GLOBALDATA*)(MySurfaces.pDriverData))->ROOTHANDLE, NV_WIN_DEVICE); \
            videoFreeCount = NvGetFreeCount(npDevVideo, NV_DD_SPARE); \
        } \
        videoFreeCount -= n; }

#ifdef POLLINGMODE
#define RESET_OCC_EVENT { \
       while(pPioColourControlEventNotifier[1].status == NV_IN_PROGRESS)  { \
             NvRmR0Interrupt (((GLOBALDATA*)(MySurfaces.pDriverData))->ROOTHANDLE, NV_WIN_DEVICE); \
             R0SLEEP(0)  \
       } \
        pPioColourControlEventNotifier[1].status = NV_IN_PROGRESS; }

#else
#define RESET_OCC_EVENT { \
   _asm { \
    emms \
   }; \
  (vwin32ResetWin32Event((U032)lpProcInfo->hOCCSyncEvent0)); \
  }
#endif

#ifdef POLLINGMODE

#if 1
#define  COLORCTL_039_NOTIFYVALUE     NV039_BUFFER_NOTIFY_WRITE_ONLY
#define WAIT_FOR_OCC_EVENT  while(pPioColourControlEventNotifier[1].status == NV_IN_PROGRESS)  { \
     NvRmR0Interrupt (((GLOBALDATA*)(MySurfaces.pDriverData))->ROOTHANDLE, NV_WIN_DEVICE); \
     R0SLEEP(0)  \
} 
#else
#define  COLORCTL_039_NOTIFYVALUE     NV039_BUFFER_NOTIFY_WRITE_ONLY
#define WAIT_FOR_OCC_EVENT  while(pPioColourControlEventNotifier[1].status == NV_IN_PROGRESS)  { \
      \
} 
#endif

#else
#define  COLORCTL_039_NOTIFYVALUE     NV039_BUFFER_NOTIFY_WRITE_THEN_AWAKEN
#define WAIT_FOR_OCC_EVENT { \
  NvRmR0Interrupt(((GLOBALDATA*)(MySurfaces.pDriverData))->ROOTHANDLE,NV_WIN_DEVICE); \
   _asm { \
    emms \
   }; \
}
#endif


//MarkTiming (WAITOCCCOLORCTL, vwin32WaitSingleObject((U032)lpProcInfo->hOCCSyncEvent0,TIMEOUT_DMA_XFER,0), 1); 
  





BOOL YUV422ColourControl(DWORD  dwPitch, DWORD dwWidth, DWORD dwHeight, DWORD dwSrcOffset, DWORD dwDstOffset, LPDDCOLORCONTROL lpDDCC)
#else  //COMPILEDINVDD ------------------------------------------------------

extern DWORD WaitForIdle(BOOL bWait, BOOL bZeroFreeCount, BOOL bUseEvent);

#define PDRIVERDATA (pDriverData)
#define WAIT_FOR_FIFO(n) { \
        while (videoFreeCount < n) { \
            videoFreeCount = NvGetFreeCount(npDevVideo, NV_DD_SPARE); \
            if (videoFreeCount < n) NV_SLEEP; \
        } \
        videoFreeCount -= n; }
#define RESET_OCC_EVENT (ResetEvent((HANDLE)lpProcInfo->hOCCSyncEvent3))
#define WAIT_FOR_OCC_EVENT (WaitForSingleObject((HANDLE)lpProcInfo->hOCCSyncEvent3, TIMEOUT_DMA_XFER))

BOOL __stdcall YUV422ColourControl(LPDDRAWI_DDRAWSURFACE_GBL lpGbl, DWORD dwWidth, DWORD dwHeight, DWORD dwSrcOffset, DWORD dwDstOffset, LPDDCOLORCONTROL lpDDCC)

#endif //COMPILEDINVDD ------------------------------------------------------
{

NvNotification *pPioColourControlEventNotifier = PDRIVERDATA->pPioColourControlEventNotifierFlat;
#ifndef COMPILEDINVDD
DDPIXELFORMAT *lpPixelFormat = &lpGbl->ddpfSurface;
#endif //COMPILEDINVDD
DWORD overlayFourCC = 0;
BOOL bOverContrast;
DWORD dwContrast, dwBrightness, cnt, brt;
short hue, sat;

FAST Nv3ChannelPio *npDevVideo = (Nv3ChannelPio *)PDRIVERDATA->NvDevVideoFlatPio;
FAST long videoFreeCount = NvGetFreeCount(npDevVideo, NV_DD_SPARE);
DWORD dwYPhase, dwSrcPitch, dwDstPitch;
BOOL doContrastBlt, doLuma, doChroma;
DWORD dwChromaOffset;
LPPROCESSINFO lpProcInfo = NULL;

#ifndef COMPILEDINVDD
    lpProcInfo = pmGetProcess(GetCurrentProcessId());    
#endif
    
#ifdef POLLINGMODE
    if (!PDRIVERDATA->vpp.regOverlayColourControlEnable ||
        !PDRIVERDATA->vpp.fpOverlayShadow)
#else
    if (!PDRIVERDATA->vpp.regOverlayColourControlEnable ||
        !PDRIVERDATA->vpp.fpOverlayShadow ||
        !lpProcInfo ||
        !lpProcInfo->hOCCSyncEvent3)
#endif
        return FALSE;

#ifdef COMPILEDINVDD
    if( (PDRIVERDATA->vpp.regRing0ColourCtlInterlockFlags & 0x01 ) != 0x01) {
      // if bit zero is zero this means we should simply return... ie not run
      return 0;
    }
    
    // note there is an assumption here that there is no other way to exit this routine between
    // here and the final return!!!!!!
    // set bit 1 meaning we're now in operation
    PDRIVERDATA->vpp.regRing0ColourCtlInterlockFlags = PDRIVERDATA->vpp.regRing0ColourCtlInterlockFlags | 0x02;
#endif   

    MarkTiming(ENTERCOLORCTL,dwSrcOffset + MySurfaces.pDriverData->BaseAddress,0);
        
    if (lpDDCC->lContrast > 0xFF) {
        cnt = lpDDCC->lContrast >> 1;
        bOverContrast = TRUE;
    } else {
        cnt = lpDDCC->lContrast;
        bOverContrast = FALSE;
    }
    if(lpDDCC->lBrightness  < 0 ) {
      brt = -lpDDCC->lBrightness;
    } else {
      brt = lpDDCC->lBrightness;
    }
    hue = (short) lpDDCC->lHue;
    sat = (short) lpDDCC->lSaturation;

#ifndef COMPILEDINVDD
    dwSrcPitch = lpGbl->lPitch;
#else
    dwSrcPitch = dwPitch;
#endif
    
#ifndef COMPILEDINVDD
    if (lpPixelFormat->dwFlags & DDPF_FOURCC) {
        overlayFourCC = lpPixelFormat->dwFourCC;
        if (overlayFourCC == MAKEFOURCC('U','Y','V','Y') ||
            overlayFourCC == MAKEFOURCC('U','Y','N','V')) {
            dwContrast   = (cnt << 8) | (cnt << 24) | 0x00FF00FF;
            dwBrightness = brt | (brt << 8) | (brt << 16) | (brt << 24);
            dwYPhase = 1;
        } else {
            dwContrast   = cnt | (cnt << 16) | 0xFF00FF00;
            dwBrightness = brt | (brt << 8) | (brt << 16) | (brt << 24);
            dwYPhase = 0;
        }
        if (overlayFourCC == MAKEFOURCC('Y','V','1','2') ||
            overlayFourCC == MAKEFOURCC('Y','V','U','9') ||
            overlayFourCC == MAKEFOURCC('I','F','0','9') ||
            overlayFourCC == MAKEFOURCC('I','V','3','1') ||
            overlayFourCC == MAKEFOURCC('I','V','3','2')) {
            dwSrcPitch <<= 1;
            WaitForIdle(TRUE, FALSE, TRUE);   // wait for prior ConvertOverlay to finish
        }
    } else {
        dwContrast   = 0xFFFFFFFF;
        dwBrightness = 0;
        bOverContrast = FALSE;
        dwYPhase = 0;
    }
#else
    dwContrast   = (cnt << 8) | (cnt << 24) | 0x00FF00FF;
    dwBrightness = brt | (brt << 8) | (brt << 16) | (brt << 24);
    // dwYPhase is used to act on information about which field we're on in interleaved surfaces
    // So, we need to program it appropriately
    dwYPhase = 1;
#endif //COMPILEDINVDD

    dwDstPitch = dwSrcPitch >> 1;
    dwWidth = (dwWidth + 1) & ~1;      // width must be even
    //if (dwWidth > dwDstPitch)
    //   dwWidth -= 2;

    doContrastBlt = (dwContrast != 0xFFFFFFFF);
    doLuma        = (dwBrightness != 0 || bOverContrast);
    doChroma      = (hue != 0 || sat != 0x100);

#ifndef COMPILEDINVDD
    NV_DD_DMA_PUSHER_SYNC();
#else
    // perhaps should do something here to sync up the dma pushers.... ?
#endif
    
    
    if (pPioColourControlEventNotifier[1].status == NV_IN_PROGRESS)
        WAIT_FOR_OCC_EVENT;
    
    if (doChroma) {
        // Apply a chroma vector scale and rotation
        // [U'] = [ sat*cos(hue)  sat*sin(hue)] [U]
        // [V']   [-sat*sin(hue)  sat*cos(hue)] [V]
        NvNotification *NvPioBufferNotifier = PDRIVERDATA->pDmaBufferNotifierFlat;
        short sinhue, coshue;
        __int64 c1, c2;
        static const __int64 bOneTwentyEight = 0x8080808080808080;
        static const __int64 evenMask        = 0x00FF00FF00FF00FF;
        static const __int64 oddMask         = 0xFF00FF00FF00FF00;
        static const __int64 zero            = 0;
        DWORD dwAdjWidth, j;
        LPBYTE qPtr;
        // sin table normalized to 256
        
        static const short sintable[91] = {  0,  4,  9, 13, 18, 22, 27, 31,
                                            36, 40, 44, 49, 53, 58, 62, 66,
                                            71, 75, 79, 83, 88, 92, 96,100,
                                           104,108,112,116,120,124,128,132,
                                           136,139,143,147,150,154,158,161,
                                           165,168,171,175,178,181,184,187,
                                           190,193,196,199,202,204,207,210,
                                           212,215,217,219,222,224,226,228,
                                           230,232,234,236,237,239,241,242,
                                           243,245,246,247,248,249,250,251,
                                           252,253,254,254,255,255,255,256,
                                           256,256,256 };

        dwChromaOffset = dwDstPitch * dwHeight;

        // move chroma data to system memory
        RESET_OCC_EVENT;
        WAIT_FOR_FIFO(48);

        pPioColourControlEventNotifier[1].status = NV_IN_PROGRESS;

        npDevVideo->dDrawSpare.SetObject = NV_VPP_V2OSH_FORMAT_IID;
        npDevVideo->dDrawSpare.nv3MemoryToMemoryFormat.SetContextDmaNotifies = NV_DD_PIO_COLOUR_CONTROL_EVENT_NOTIFIER;
        npDevVideo->dDrawSpare.nv3MemoryToMemoryFormat.SetContextDmaBufferIn = NV_DD_PIO_CONTEXT_DMA_FROM_VIDEO_MEMORY;
        npDevVideo->dDrawSpare.nv3MemoryToMemoryFormat.SetContextDmaBufferOut = NV_DD_PIO_FLOATING_CONTEXT_DMA_IN_OVERLAY_SHADOW;
        npDevVideo->dDrawSpare.nv3MemoryToMemoryFormat.OffsetIn     = dwSrcOffset + (dwYPhase ^ 1);
        npDevVideo->dDrawSpare.nv3MemoryToMemoryFormat.OffsetOut    = dwChromaOffset;
        npDevVideo->dDrawSpare.nv3MemoryToMemoryFormat.PitchOut     = dwDstPitch;
        npDevVideo->dDrawSpare.nv3MemoryToMemoryFormat.LineLengthIn = dwWidth;
        npDevVideo->dDrawSpare.nv3MemoryToMemoryFormat.LineCount    = dwHeight;
        npDevVideo->dDrawSpare.nv3MemoryToMemoryFormat.PitchIn      = dwSrcPitch;
        npDevVideo->dDrawSpare.nv3MemoryToMemoryFormat.Format       = 0x102;
        npDevVideo->dDrawSpare.nv3MemoryToMemoryFormat.BufferNotify = COLORCTL_039_NOTIFYVALUE;

        // coefficient calculations
        // possible MMX multiply overflow later, so halve the coefficients now
        // and double the result later with saturation
        if (lpDDCC->lHue <= 90) {
            sinhue = ((long)sat * sintable[hue]) >> 9;
            coshue = ((long)sat * sintable[90 - hue]) >> 9;
        } else if (lpDDCC->lHue <= 180) {
            sinhue = ((long)sat * sintable[180 - hue]) >> 9;
            coshue = ((long)sat * -sintable[hue - 90]) >> 9;
        } else if (lpDDCC->lHue <= 270) {
            sinhue = ((long)sat * -sintable[hue - 180]) >> 9;
            coshue = ((long)sat * -sintable[270 - hue]) >> 9;
        } else {
            sinhue = ((long)sat * -sintable[360 - hue]) >> 9;
            coshue = ((long)sat * sintable[hue - 270]) >> 9;
        }

        // c1 = sat*cos(hue), sat*cos(hue), sat*cos(hue), sat*cos(hue)
        c1 = (__int64)coshue & 0xFFFF;
        c1 |= c1 << 16;
        c1 |= c1 << 32;
        // c2 = -sat*sin(hue), sat*sin(hue), -sat*sin(hue), sat*sin(hue)
        c2 = (__int64)sinhue & 0xFFFF;
        c2 |= ((__int64)-sinhue & 0xFFFF) << 16;
        c2 |= c2 << 32;

        // init MMX const data
        qPtr       = (LPBYTE) (PDRIVERDATA->vpp.fpOverlayShadow + dwChromaOffset);
        dwAdjWidth = (dwWidth + 7) / 8;
        _asm {
            movq        mm0, zero
            movq        mm1, bOneTwentyEight 
		    }

        // while we are doing some host MMX things, overlap with some luma operations
        // since we are main memory throughput bound, DO NOT overlap with operations that touch main memory
        if (doContrastBlt) {
            WAIT_FOR_FIFO(40);

            /* Trash spare subchannel */
            npDevVideo->dDrawSpare.SetObject = NV_DD_CONTEXT_BETA4;
            npDevVideo->dDrawSpare.nv4ContextBeta.SetBetaFactor = dwContrast;
            npDevVideo->dDrawSpare.SetObject = NV_DD_SURFACES_2D_A8R8G8B8;
            npDevVideo->dDrawSpare.nv4ContextSurfaces2D.SetPitch = (dwSrcPitch << 16) | dwSrcPitch;
            npDevVideo->dDrawSpare.nv4ContextSurfaces2D.SetOffsetSource = dwSrcOffset;
            if (doLuma)
                // more stuff to do later, do this in place
                npDevVideo->dDrawSpare.nv4ContextSurfaces2D.SetOffsetDestin = dwSrcOffset;
            else
                npDevVideo->dDrawSpare.nv4ContextSurfaces2D.SetOffsetDestin = dwDstOffset;
            npDevVideo->dDrawSpare.SetObject = NV_DD_SRCCOPY_PREMULT_A8R8G8B8_IMAGE_BLIT;
            npDevVideo->dDrawSpare.nv4ImageBlit.ControlPointIn = 0;
            npDevVideo->dDrawSpare.nv4ImageBlit.ControlPointOut = 0;
            npDevVideo->dDrawSpare.nv4ImageBlit.Size = (dwHeight << 16) | (dwWidth >> 1);
            PDRIVERDATA->dDrawSpareSubchannelObject = 0;
        }

        if (!doContrastBlt && !doLuma) {
            // luma has not and will not move, do that now
            WAIT_FOR_FIFO(36);

            npDevVideo->dDrawSpare.SetObject = NV_DD_VIDEO_MEMORY_UV_TO_VIDEO_MEMORY_FORMAT;
            npDevVideo->dDrawSpare.nv3MemoryToMemoryFormat.OffsetIn     = dwSrcOffset + dwYPhase;
            npDevVideo->dDrawSpare.nv3MemoryToMemoryFormat.OffsetOut    = dwDstOffset + dwYPhase;
            npDevVideo->dDrawSpare.nv3MemoryToMemoryFormat.PitchOut     = dwSrcPitch;
            npDevVideo->dDrawSpare.nv3MemoryToMemoryFormat.LineLengthIn = dwWidth;
            npDevVideo->dDrawSpare.nv3MemoryToMemoryFormat.LineCount    = dwHeight;
            npDevVideo->dDrawSpare.nv3MemoryToMemoryFormat.PitchIn      = dwSrcPitch;
            npDevVideo->dDrawSpare.nv3MemoryToMemoryFormat.Format       = 0x202;
            npDevVideo->dDrawSpare.nv3MemoryToMemoryFormat.BufferNotify = 0;
        }

        // wait for chroma to arrive in system memory
        WAIT_FOR_OCC_EVENT;
        //MarkTiming(WAITCHROMAARRIVE,dwSrcOffset + MySurfaces.pDriverData->BaseAddress,10);
        
/*      // commented out because this appears to cause a cache coherency problem (bug in BX?)
        // preload the first 8 scan lines into the cache
        for (j=0; j<8; j++) {
            _asm {
                        mov     esi, qPtr
                        mov     ecx, dwAdjWidth
                        shr     ecx, 2
                preld1: mov     eax, [esi]
                        mov     eax, 0
                        add     esi, 32
                        dec     ecx
                        jnz     preld1
            }
            qPtr += dwDstPitch;
        }
*/
        // do the chroma vector scale and rotation, 2.25 cycles/pixel
        // mm0 = 0
        // mm1 = 128
        // mm2 = sign mask
        // mm3 = data quad / expanded data / result high
        // mm4 = expanded data / result low
        // mm5 = expanded data, UV swapped
        // mm6 = UV swapped quad / expanded swapped data
        // mm7 = work space
        if (PDRIVERDATA->vpp.regOverlayColourControlEnable == 2) {
            // full chroma resolution
            for (j=0; j<dwHeight; j++) {
                _asm {
                            mov         ecx, dwAdjWidth
                            mov         esi, qPtr
                            mov         edi, qPtr
                            mov         ebx, dwDstPitch
                    ;        shl         ebx, 3
                            movq        mm3, [esi]
                            psubb       mm3, mm1
                            movq        mm2, mm0
                            pcmpgtb     mm2, mm3
                            movq        mm4, mm3
                            movq        mm7, mm3
                            punpcklbw   mm4, mm2
                    nextc:  mov         eax, [esi+ebx]  ; U  ; preload cache for next scan line
                            mov         eax, 0          ; V  ; ensure OOE works
                            pmullw      mm4, c1         ; U  ; mm4 = c1 * low(UV)
                            movq        mm6, mm3        ; V  ; mm6 = copy of quad data-128
                            add         esi, 8          ; U  ; next source address
                            psrlq       mm6, 8          ; V  ; mm6 >>= 8
                            pand        mm6, evenMask   ; U  ; mm6 &= 0x00FF00FF00FF00FF
                            psllq       mm7, 8          ; V  ; mm7 <<= 8
                            pand        mm7, oddMask    ; U  ; mm7 &= 0xFF00FF00FF00FF00
                            punpckhbw   mm3, mm2        ; V  ; mm3 = unpacked signed high data
                            pmullw      mm3, c1         ; U  ; mm3 = c1 * high(UV)
                            por         mm6, mm7        ; V  ; mm6 |= mm7, UV are now swapped
                            movq        mm5, mm6        ; U  ; mm5 = quad data with UV swapped
                            movq        mm2, mm0        ; V  ; mm2 = 0
                            add         edi, 8          ; U  ; next destination address
                            pcmpgtb     mm2, mm6        ; V  ; mm2 = sign bits of swapped UV
                            punpcklbw   mm5, mm2        ; U  ; mm5 = unpacked signed low VU data
                            movq        mm7, mm3        ; V  ; move result to mm7 so we can reuse mm3 early
                            movq        mm3, [esi]      ; U  ; mm3 = new data quad
                            punpckhbw   mm6, mm2        ; V  ; mm6 = unpacked signed high VU data
                            pmullw      mm5, c2         ; U  ; mm5 = c2 * low(VU)
                            movq        mm2, mm0        ; V  ; mm2 = 0
                            pmullw      mm6, c2         ; U  ; mm6 = c2 * high(VU)
                            psubb       mm3, mm1        ; V  ; mm3 -= 128
                            paddsw      mm5, mm4        ; U  ; mm5 += mm4
                            paddsw      mm6, mm7        ; V  ; mm6 += mm7
                            dec         ecx             ; U  ; decrement loop count
                            psraw       mm5, 7          ; V  ; downshift result and multiply by 2
                            psraw       mm6, 7          ; U  ; downshift result and multiply by 2
                            pcmpgtb     mm2, mm3        ; V  ; mm2 = sign bits
                            packsswb    mm5, mm6        ; U  ; mm5 = packed(mm5,mm6)
                            paddb       mm5, mm1        ; V  ; mm5 += 128
                            movq        mm7, mm3        ; U  ; mm7 = copy of quad data-128
                            movq        mm4, mm3        ; V  ; mm4 = copy of quad data-128
                            movq        [edi-8], mm5    ; U  ; store data from mm5
                            punpcklbw   mm4, mm2        ; V  ; mm4 = unpacked signed low UV data
                            jnz         nextc
                }
                qPtr += dwDstPitch;
            }
        } else {
            // vertical chroma subsample
            // in this case, ESI is used for source and in-place destination, and EDI is used for next line in destination
            for (j=0; j<dwHeight; j+=2) {
                _asm {
                            mov         ecx, dwAdjWidth
                            mov         esi, qPtr
                            mov         edi, qPtr
                            add         edi, dwDstPitch
                            mov         ebx, dwDstPitch
                            shl         ebx, 1
                    ;        shl         ebx, 3
                            movq        mm3, [esi]
                            psubb       mm3, mm1
                            movq        mm2, mm0
                            pcmpgtb     mm2, mm3
                            movq        mm4, mm3
                            movq        mm7, mm3
                            punpcklbw   mm4, mm2
                    nextc2: mov         eax, [esi+ebx]  ; U  ; preload cache for next scan line
                            mov         eax, 0          ; V  ; ensure OOE works
                            pmullw      mm4, c1         ; U  ; mm4 = c1 * low(UV)
                            movq        mm6, mm3        ; V  ; mm6 = copy of quad data-128
                            add         esi, 8          ; U  ; next source address
                            psrlq       mm6, 8          ; V  ; mm6 >>= 8
                            pand        mm6, evenMask   ; U  ; mm6 &= 0x00FF00FF00FF00FF
                            psllq       mm7, 8          ; V  ; mm7 <<= 8
                            pand        mm7, oddMask    ; U  ; mm7 &= 0xFF00FF00FF00FF00
                            punpckhbw   mm3, mm2        ; V  ; mm3 = unpacked signed high data
                            pmullw      mm3, c1         ; U  ; mm3 = c1 * high(UV)
                            por         mm6, mm7        ; V  ; mm6 |= mm7, UV are now swapped
                            movq        mm5, mm6        ; U  ; mm5 = quad data with UV swapped
                            movq        mm2, mm0        ; V  ; mm2 = 0
                            add         edi, 8          ; U  ; next destination address
                            pcmpgtb     mm2, mm6        ; V  ; mm2 = sign bits of swapped UV
                            movq        mm7, mm3        ; U  ; move result to mm7 so we can reuse mm3 early
                            punpcklbw   mm5, mm2        ; V  ; mm5 = unpacked signed low VU data
                            movq        mm3, [esi]      ; U  ; mm3 = new data quad
                            punpckhbw   mm6, mm2        ; V  ; mm6 = unpacked signed high VU data
                            pmullw      mm5, c2         ; U  ; mm5 = c2 * low(VU)
                            movq        mm2, mm0        ; V  ; mm2 = 0
                            pmullw      mm6, c2         ; U  ; mm6 = c2 * high(VU)
                            psubb       mm3, mm1        ; V  ; mm3 -= 128
                            paddsw      mm5, mm4        ; U  ; mm5 += mm4
                            paddsw      mm6, mm7        ; V  ; mm6 += mm7
                            dec         ecx             ; U  ; decrement loop count
                            psraw       mm5, 7          ; V  ; downshift result and multiply by 2
                            psraw       mm6, 7          ; U  ; downshift result and multiply by 2
                            pcmpgtb     mm2, mm3        ; V  ; mm2 = sign bits
                            packsswb    mm5, mm6        ; U  ; mm5 = packed(mm5,mm6)
                            paddb       mm5, mm1        ; V  ; mm5 += 128
                            movq        mm7, mm3        ; U  ; mm7 = copy of quad data-128
                            movq        mm4, mm3        ; V  ; mm4 = copy of quad data-128
                            movq        [edi-8], mm5    ; U  ; store data from mm5
                            punpcklbw   mm4, mm2        ; V  ; mm4 = unpacked signed low UV data
                            movq        [esi-8], mm5    ; U  ; store same data on next line
                            jnz         nextc2          ; V
                }
                qPtr += dwDstPitch << 1;
            }
        }

        if (!doLuma) {
          // move chroma data to back to video memory
          RESET_OCC_EVENT;
          WAIT_FOR_FIFO(48);
  
          PDRIVERDATA->dDrawSpareSubchannelObject = 0;
          pPioColourControlEventNotifier[1].status = NV_IN_PROGRESS;
  
          npDevVideo->dDrawSpare.SetObject = NV_VPP_OSH2V_FORMAT_IID;
          npDevVideo->dDrawSpare.nv3MemoryToMemoryFormat.SetContextDmaNotifies = NV_DD_PIO_COLOUR_CONTROL_EVENT_NOTIFIER;
          npDevVideo->dDrawSpare.nv3MemoryToMemoryFormat.SetContextDmaBufferIn = NV_DD_PIO_FLOATING_CONTEXT_DMA_IN_OVERLAY_SHADOW;
          npDevVideo->dDrawSpare.nv3MemoryToMemoryFormat.SetContextDmaBufferOut = NV_DD_PIO_CONTEXT_DMA_TO_VIDEO_MEMORY;
          npDevVideo->dDrawSpare.nv3MemoryToMemoryFormat.OffsetIn     = dwChromaOffset;
          npDevVideo->dDrawSpare.nv3MemoryToMemoryFormat.OffsetOut    = dwDstOffset + (dwYPhase ^ 1);
          npDevVideo->dDrawSpare.nv3MemoryToMemoryFormat.PitchOut     = dwSrcPitch;
          npDevVideo->dDrawSpare.nv3MemoryToMemoryFormat.LineLengthIn = dwWidth;
          npDevVideo->dDrawSpare.nv3MemoryToMemoryFormat.LineCount    = dwHeight;
          npDevVideo->dDrawSpare.nv3MemoryToMemoryFormat.PitchIn      = dwDstPitch;
          npDevVideo->dDrawSpare.nv3MemoryToMemoryFormat.Format       = 0x201;
          if (!doLuma) {
              npDevVideo->dDrawSpare.nv3MemoryToMemoryFormat.BufferNotify = COLORCTL_039_NOTIFYVALUE;
              WAIT_FOR_OCC_EVENT;
              //MarkTiming(WAITCHROMARETURN,dwSrcOffset + MySurfaces.pDriverData->BaseAddress,20);
            
          } else {
              npDevVideo->dDrawSpare.nv3MemoryToMemoryFormat.BufferNotify = 0;
          }
        } 
        
    } else if (doContrastBlt) {
        // Contrast Blit, since the hardware can only do multiplies up to one (0xFF),
        // and if the contrast is from 100% to 200%, then divide contrast by 2 and worry
        // about left shift and saturation in the MMX routines below
        WAIT_FOR_FIFO(40);

        /* Trash spare subchannel */
        npDevVideo->dDrawSpare.SetObject = NV_DD_CONTEXT_BETA4;
        npDevVideo->dDrawSpare.nv4ContextBeta.SetBetaFactor = dwContrast;
        npDevVideo->dDrawSpare.SetObject = NV_DD_SURFACES_2D_A8R8G8B8;
        npDevVideo->dDrawSpare.nv4ContextSurfaces2D.SetPitch = (dwSrcPitch << 16) | dwSrcPitch;
        npDevVideo->dDrawSpare.nv4ContextSurfaces2D.SetOffsetSource = dwSrcOffset;
        if (doLuma)
            // more stuff to do later, do this in place
            npDevVideo->dDrawSpare.nv4ContextSurfaces2D.SetOffsetDestin = dwSrcOffset;
        else
            npDevVideo->dDrawSpare.nv4ContextSurfaces2D.SetOffsetDestin = dwDstOffset;
        npDevVideo->dDrawSpare.SetObject = NV_DD_SRCCOPY_PREMULT_A8R8G8B8_IMAGE_BLIT;
        npDevVideo->dDrawSpare.nv4ImageBlit.ControlPointIn = 0;
        npDevVideo->dDrawSpare.nv4ImageBlit.ControlPointOut = 0;
        npDevVideo->dDrawSpare.nv4ImageBlit.Size = (dwHeight << 16) | (dwWidth >> 1);
        PDRIVERDATA->dDrawSpareSubchannelObject = 0;
    }

    // OverContrast and Brightness calculations
    if (doLuma) {
        static const __int64 shiftMask   = 0xFEFEFEFEFEFEFEFE;
        static const __int64 cmpOverflow = 0xFFFFFFFFFFFFFFFF;
        DWORD j, dwAdjWidth;
        LPBYTE qPtr;

        // move Y data to system memory
        RESET_OCC_EVENT;
        WAIT_FOR_FIFO(48);

        pPioColourControlEventNotifier[1].status = NV_IN_PROGRESS;

        npDevVideo->dDrawSpare.SetObject = NV_VPP_V2OSH_FORMAT_IID;
        npDevVideo->dDrawSpare.nv3MemoryToMemoryFormat.SetContextDmaNotifies = NV_DD_PIO_COLOUR_CONTROL_EVENT_NOTIFIER;
        npDevVideo->dDrawSpare.nv3MemoryToMemoryFormat.SetContextDmaBufferIn = NV_DD_PIO_CONTEXT_DMA_FROM_VIDEO_MEMORY;
        npDevVideo->dDrawSpare.nv3MemoryToMemoryFormat.SetContextDmaBufferOut = NV_DD_PIO_FLOATING_CONTEXT_DMA_IN_OVERLAY_SHADOW;
        npDevVideo->dDrawSpare.nv3MemoryToMemoryFormat.OffsetIn     = dwSrcOffset + dwYPhase;
        npDevVideo->dDrawSpare.nv3MemoryToMemoryFormat.OffsetOut    = 0;
        npDevVideo->dDrawSpare.nv3MemoryToMemoryFormat.PitchOut     = dwDstPitch;
        npDevVideo->dDrawSpare.nv3MemoryToMemoryFormat.LineLengthIn = dwWidth;
        npDevVideo->dDrawSpare.nv3MemoryToMemoryFormat.LineCount    = dwHeight;
        npDevVideo->dDrawSpare.nv3MemoryToMemoryFormat.PitchIn      = dwSrcPitch;
        npDevVideo->dDrawSpare.nv3MemoryToMemoryFormat.Format       = 0x102;
        npDevVideo->dDrawSpare.nv3MemoryToMemoryFormat.BufferNotify = COLORCTL_039_NOTIFYVALUE;

        if (!doChroma) {
            // chroma has not and will not move, do that now
            WAIT_FOR_FIFO(36);

            npDevVideo->dDrawSpare.SetObject = NV_DD_VIDEO_MEMORY_UV_TO_VIDEO_MEMORY_FORMAT;
            npDevVideo->dDrawSpare.nv3MemoryToMemoryFormat.OffsetIn     = dwSrcOffset + (dwYPhase ^ 1);
            npDevVideo->dDrawSpare.nv3MemoryToMemoryFormat.OffsetOut    = dwDstOffset + (dwYPhase ^ 1);
            npDevVideo->dDrawSpare.nv3MemoryToMemoryFormat.PitchOut     = dwSrcPitch;
            npDevVideo->dDrawSpare.nv3MemoryToMemoryFormat.LineLengthIn = dwWidth;
            npDevVideo->dDrawSpare.nv3MemoryToMemoryFormat.LineCount    = dwHeight;
            npDevVideo->dDrawSpare.nv3MemoryToMemoryFormat.PitchIn      = dwSrcPitch;
            npDevVideo->dDrawSpare.nv3MemoryToMemoryFormat.Format       = 0x202;
            npDevVideo->dDrawSpare.nv3MemoryToMemoryFormat.BufferNotify = 0;
        }

        // init MMX const data
        qPtr       = (LPBYTE) PDRIVERDATA->vpp.fpOverlayShadow;
        dwAdjWidth = (dwWidth + 15) / 16;
        _asm {
            movd        mm0, dwBrightness
            punpckldq   mm0, dwBrightness
            movq        mm2, shiftMask
            movq        mm3, cmpOverflow 
		    }

        // wait for luma data to arrive in system memory
        WAIT_FOR_OCC_EVENT;
        //MarkTiming(WAITLUMAARIVE,dwSrcOffset + MySurfaces.pDriverData->BaseAddress,30);
        
        
        if(doChroma) {  
          // move chroma data to back to video memory
          RESET_OCC_EVENT;
          WAIT_FOR_FIFO(48);
  
          PDRIVERDATA->dDrawSpareSubchannelObject = 0;
          pPioColourControlEventNotifier[1].status = NV_IN_PROGRESS;
  
          npDevVideo->dDrawSpare.SetObject = NV_VPP_OSH2V_FORMAT_IID;
          npDevVideo->dDrawSpare.nv3MemoryToMemoryFormat.SetContextDmaNotifies = NV_DD_PIO_COLOUR_CONTROL_EVENT_NOTIFIER;
          npDevVideo->dDrawSpare.nv3MemoryToMemoryFormat.SetContextDmaBufferIn = NV_DD_PIO_FLOATING_CONTEXT_DMA_IN_OVERLAY_SHADOW;
          npDevVideo->dDrawSpare.nv3MemoryToMemoryFormat.SetContextDmaBufferOut = NV_DD_PIO_CONTEXT_DMA_TO_VIDEO_MEMORY;
          npDevVideo->dDrawSpare.nv3MemoryToMemoryFormat.OffsetIn     = dwChromaOffset;
          npDevVideo->dDrawSpare.nv3MemoryToMemoryFormat.OffsetOut    = dwDstOffset + (dwYPhase ^ 1);
          npDevVideo->dDrawSpare.nv3MemoryToMemoryFormat.PitchOut     = dwSrcPitch;
          npDevVideo->dDrawSpare.nv3MemoryToMemoryFormat.LineLengthIn = dwWidth;
          npDevVideo->dDrawSpare.nv3MemoryToMemoryFormat.LineCount    = dwHeight;
          npDevVideo->dDrawSpare.nv3MemoryToMemoryFormat.PitchIn      = dwDstPitch;
          npDevVideo->dDrawSpare.nv3MemoryToMemoryFormat.Format       = 0x201;
          if (!doLuma) {
              npDevVideo->dDrawSpare.nv3MemoryToMemoryFormat.BufferNotify = COLORCTL_039_NOTIFYVALUE;
              WAIT_FOR_OCC_EVENT;
              //MarkTiming(WAITCHROMARETURN,dwSrcOffset + MySurfaces.pDriverData->BaseAddress,20);
            
          } else {
              npDevVideo->dDrawSpare.nv3MemoryToMemoryFormat.BufferNotify = 0;
          }
        }
        
/*
        // preload the first 8 scan lines into the cache
        for (j=0; j<8; j++) {
            _asm {
                        mov     esi, qPtr
                        mov     ecx, dwAdjWidth
                        shr     ecx, 1
                preld2: mov     eax, [esi]
                        mov     eax, 0
                        add     esi, 32
                        dec     ecx
                        jnz     preld2
            }
            qPtr += dwDstPitch;
        }
*/
        // do brightness and/or overcontrast
        // mm0 = brightness
        // mm1 = quad data chunk
        // mm2 = shift mask
        // mm3 = overflow compare value
        // mm4 = compare register
        // mm5 = next quad data chunk
        // mm6 = next compare register
        if (lpDDCC->lBrightness >= 0) {
            if (bOverContrast) {
                // overcontrast and positive brightness, 0.6875 cycles/pixel
                // AND is commented out for performance -> noise in the lowest bit of precision
                for (j=0; j<dwHeight; j++) {
                    _asm {
                                mov     ecx, dwAdjWidth
                                mov     esi, qPtr
                                mov     edi, qPtr
                                mov     ebx, dwDstPitch
                       ;         shl     ebx, 3
                                movq    mm1, [esi]
                                movq    mm4, mm3
                        nextq1: movq    mm5, [esi+8]    ; Ui+   ; mm5 = data[i+1]
                                pcmpgtb mm4, mm1        ; Vi    ; mm4 = sign bit of data[i]
                                add     esi, 16         ; U     ; next source address
                                movq    mm6, mm3        ; Vi+   ; mm6 = overflow compare value
                                add     edi, 16         ; U     ; next destination address
                                psll    mm1, 1          ; Vi    ; mm1 <<= 1
                                pcmpgtb mm6, mm5        ; Ui+   ; mm6 = sign bit of data[i+1]
                       ;         pand    mm1, mm2        ; i    ; mm1 &= 0xFE (since there is no PSLLB)
                                por     mm4, mm1        ; Vi    ; mm4 |= mm1 (saturated result[i])

                                psll    mm5, 1          ; Ui+   ; mm5 <<= 1
                                paddusb mm4, mm0        ; Vi    ; mm4 += brightness
                       ;         pand    mm5, mm2        ; i+   ; mm5 &= 0xFE
                                movq    [edi-16], mm4   ; Ui    ; store result[i]
                                por     mm6, mm5        ; Vi+   ; mm6 |= mm5 (saturated result[i+1])
                                movq    mm1, [esi]      ; Ui    ; mm1 = data[i]
                                paddusb mm6, mm0        ; Vi+   ; mm6 += brightness
                                dec     ecx             ; U     ; decrement loop count
                                movq    mm4, mm3        ; Vi    ; mm4 = overflow compare value
                                movq    [edi-8], mm6    ; Ui+   ; store result[i+1]
                                jnz     nextq1          ; V
                    }
                    qPtr += dwDstPitch;
                }
            } else {
                // positive brightness only
                for (j=0; j<dwHeight; j++) {
                    _asm {
                                mov     ecx, dwAdjWidth
                                mov     esi, qPtr
                                mov     edi, qPtr
                                mov     ebx, dwDstPitch
                        ;        shl     ebx, 3
                                movq    mm1, [esi]
                        nextq2: movq    mm5, [esi+8]    ; Ui+   ; mm5 = data[i+1]
                                paddusb mm1, mm0        ; Vi    ; mm1 += brightness
                                add     esi, 16         ; U     ; next source address
                                add     edi, 16         ; V     ; next destination address
                                dec     ecx             ; U     ; decrement loop count
                                movq    [edi-16], mm1   ; Ui    ; store result[i]
                                paddusb mm5, mm0        ; Vi+   ; mm5 += brightness
                                movq    [edi-8], mm5    ; Ui+   ; store result[i+1]
                                movq    mm1, [esi]      ; Ui    ; mm1 = data[i]
                                jnz     nextq2          ; V
                    }
                    qPtr += dwDstPitch;
                }
            }
        } else {
            if (bOverContrast) {
                // overcontrast and negative brightness
                for (j=0; j<dwHeight; j++) {
                    _asm {
                                mov     ecx, dwAdjWidth
                                mov     esi, qPtr
                                mov     edi, qPtr
                                mov     ebx, dwDstPitch
                        ;        shl     ebx, 3
                                movq    mm1, [esi]
                                movq    mm4, mm3
                        nextq3: movq    mm5, [esi+8]    ; Ui+   ; mm5 = data[i+1]
                                pcmpgtb mm4, mm1        ; Vi    ; mm4 = sign bit of data[i]
                                add     esi, 16         ; U     ; next source address
                                movq    mm6, mm3        ; Vi+   ; mm6 = overflow compare value
                                add     edi, 16         ; U     ; next destination address
                                psll    mm1, 1          ; Vi    ; mm1 <<= 1
                                pcmpgtb mm6, mm5        ; Ui+   ; mm6 = sign bit of data[i+1]
                       ;         pand    mm1, mm2        ; i    ; mm1 &= 0xFE (since there is no PSLLB)
                                por     mm4, mm1        ; Vi    ; mm4 |= mm1 (saturated result[i])

                                psll    mm5, 1          ; Ui+   ; mm5 <<= 1
                                psubusb mm4, mm0        ; Vi    ; mm4 -= brightness
                       ;         pand    mm5, mm2        ; i+   ; mm5 &= 0xFE
                                movq    [edi-16], mm4   ; Ui    ; store result[i]
                                por     mm6, mm5        ; Vi+   ; mm6 |= mm5 (saturated result[i+1])
                                movq    mm1, [esi]      ; Ui    ; mm1 = data[i]
                                psubusb mm6, mm0        ; Vi+   ; mm6 -= brightness
                                dec     ecx             ; U     ; decrement loop count
                                movq    mm4, mm3        ; Vi    ; mm4 = overflow compare value
                                movq    [edi-8], mm6    ; Ui+   ; store result[i+1]
                                jnz     nextq3          ; V
                    }
                    qPtr += dwDstPitch;
                }
            } else {
                // negative brightness only
                for (j=0; j<dwHeight; j++) {
                    _asm {
                                mov     ecx, dwAdjWidth
                                mov     esi, qPtr
                                mov     edi, qPtr
                                mov     ebx, dwDstPitch
                        ;        shl     ebx, 3
                                movq    mm1, [esi]
                        nextq4: movq    mm5, [esi+8]    ; Ui+   ; mm5 = data[i+1]
                                psubusb mm1, mm0        ; Vi    ; mm1 -= brightness
                                add     esi, 16         ; U     ; next source address
                                add     edi, 16         ; V     ; next destination address
                                dec     ecx             ; U     ; decrement loop count
                                movq    [edi-16], mm1   ; Ui    ; store result[i]
                                psubusb mm5, mm0        ; Vi+   ; mm5 -= brightness
                                movq    [edi-8], mm5    ; Ui+   ; store result[i+1]
                                movq    mm1, [esi]      ; Ui    ; mm1 = data[i]
                                jnz     nextq4          ; V
                    }
                    qPtr += dwDstPitch;
                }
            }
        }

        // move Y data to back to video memory
        RESET_OCC_EVENT;
        WAIT_FOR_FIFO(48);

        pPioColourControlEventNotifier[1].status = NV_IN_PROGRESS;

        npDevVideo->dDrawSpare.SetObject = NV_VPP_OSH2V_FORMAT_IID;
        npDevVideo->dDrawSpare.nv3MemoryToMemoryFormat.SetContextDmaNotifies = NV_DD_PIO_COLOUR_CONTROL_EVENT_NOTIFIER;
        npDevVideo->dDrawSpare.nv3MemoryToMemoryFormat.SetContextDmaBufferIn = NV_DD_PIO_FLOATING_CONTEXT_DMA_IN_OVERLAY_SHADOW;
        npDevVideo->dDrawSpare.nv3MemoryToMemoryFormat.SetContextDmaBufferOut = NV_DD_PIO_CONTEXT_DMA_TO_VIDEO_MEMORY;
        npDevVideo->dDrawSpare.nv3MemoryToMemoryFormat.OffsetIn     = 0;
        npDevVideo->dDrawSpare.nv3MemoryToMemoryFormat.OffsetOut    = dwDstOffset + dwYPhase;
        npDevVideo->dDrawSpare.nv3MemoryToMemoryFormat.PitchOut     = dwSrcPitch;
        npDevVideo->dDrawSpare.nv3MemoryToMemoryFormat.LineLengthIn = dwWidth;
        npDevVideo->dDrawSpare.nv3MemoryToMemoryFormat.LineCount    = dwHeight;
        npDevVideo->dDrawSpare.nv3MemoryToMemoryFormat.PitchIn      = dwDstPitch;
        npDevVideo->dDrawSpare.nv3MemoryToMemoryFormat.Format       = 0x201;
        npDevVideo->dDrawSpare.nv3MemoryToMemoryFormat.BufferNotify = COLORCTL_039_NOTIFYVALUE;
        PDRIVERDATA->dDrawSpareSubchannelObject = 0;

        WAIT_FOR_OCC_EVENT;
        //MarkTiming(WAITLUMARETURN,dwSrcOffset + MySurfaces.pDriverData->BaseAddress,40);
        
    }

    _asm { 
		  emms 
	  };

#ifdef COMPILEDINVDD
    // reset bit 1 meaning we're no longer in operation
    PDRIVERDATA->vpp.regRing0ColourCtlInterlockFlags = PDRIVERDATA->vpp.regRing0ColourCtlInterlockFlags & 0xFFFFFFFD;
#endif   
    MarkTiming(LEAVECOLORCTL,dwSrcOffset + MySurfaces.pDriverData->BaseAddress,50);
    
    return doContrastBlt || doLuma || doChroma;
}


BOOL WillDoYUV422ColourControl( LPDDCOLORCONTROL lpDDCC)
{

  BOOL bOverContrast;
  DWORD dwContrast, dwBrightness, cnt, brt;
  short hue, sat;
  
  BOOL doContrastBlt, doLuma, doChroma;
  LPPROCESSINFO lpProcInfo = NULL;

#ifndef COMPILEDINVDD
  lpProcInfo = pmGetProcess(GetCurrentProcessId());    
#endif
    
#ifdef POLLINGMODE
  if (!PDRIVERDATA->vpp.regOverlayColourControlEnable ||
      !PDRIVERDATA->vpp.fpOverlayShadow)
#else
  if (!PDRIVERDATA->vpp.regOverlayColourControlEnable ||
      !PDRIVERDATA->vpp.fpOverlayShadow ||
      !lpProcInfo ||
      !lpProcInfo->hOCCSyncEvent3)
#endif
    return FALSE;
  
  if (lpDDCC->lContrast > 0xFF) {
    cnt = lpDDCC->lContrast >> 1;
    bOverContrast = TRUE;
  } else {
    cnt = lpDDCC->lContrast;
    bOverContrast = FALSE;
  }
  if(lpDDCC->lBrightness  < 0 ) {
    brt = -lpDDCC->lBrightness;
  } else {
    brt = lpDDCC->lBrightness;
  }
  hue = (short) lpDDCC->lHue;
  sat = (short) lpDDCC->lSaturation;
  
  
  dwContrast   = (cnt << 8) | (cnt << 24) | 0x00FF00FF;
  dwBrightness = brt | (brt << 8) | (brt << 16) | (brt << 24);
  
  doContrastBlt = (dwContrast != 0xFFFFFFFF);
  doLuma        = (dwBrightness != 0 || bOverContrast);
  doChroma      = (hue != 0 || sat != 0x100);
  
  return doContrastBlt || doLuma || doChroma;
}


#endif  // NVARCH >= 0x04
