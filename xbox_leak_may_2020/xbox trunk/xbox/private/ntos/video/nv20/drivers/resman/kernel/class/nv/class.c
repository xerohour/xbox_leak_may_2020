/***************************************************************************\
|*                                                                           *|
|*        Copyright (c) 1993-2000 NVIDIA, Corp.  All rights reserved.        *|
|*                                                                           *|
|*     NOTICE TO USER:   The source code  is copyrighted under  U.S. and     *|
|*     international laws.   NVIDIA, Corp. of Sunnyvale, California owns     *|
|*     the copyright  and as design patents  pending  on the design  and     *|
|*     interface  of the NV chips.   Users and possessors of this source     *|
|*     code are hereby granted  a nonexclusive,  royalty-free  copyright     *|
|*     and  design  patent license  to use this code  in individual  and     *|
|*     commercial software.                                                  *|
|*                                                                           *|
|*     Any use of this source code must include,  in the user documenta-     *|
|*     tion and  internal comments to the code,  notices to the end user     *|
|*     as follows:                                                           *|
|*                                                                           *|
|*     Copyright (c) 1993-2000  NVIDIA, Corp.    NVIDIA  design  patents     *|
|*     pending in the U.S. and foreign countries.                            *|
|*                                                                           *|
|*     NVIDIA, CORP.  MAKES  NO REPRESENTATION ABOUT  THE SUITABILITY OF     *|
|*     THIS SOURCE CODE FOR ANY PURPOSE.  IT IS PROVIDED "AS IS" WITHOUT     *|
|*     EXPRESS OR IMPLIED WARRANTY OF ANY KIND.  NVIDIA, CORP. DISCLAIMS     *|
|*     ALL WARRANTIES  WITH REGARD  TO THIS SOURCE CODE,  INCLUDING  ALL     *|
|*     IMPLIED   WARRANTIES  OF  MERCHANTABILITY  AND   FITNESS   FOR  A     *|
|*     PARTICULAR  PURPOSE.   IN NO EVENT SHALL NVIDIA, CORP.  BE LIABLE     *|
|*     FOR ANY SPECIAL, INDIRECT, INCIDENTAL,  OR CONSEQUENTIAL DAMAGES,     *|
|*     OR ANY DAMAGES  WHATSOEVER  RESULTING  FROM LOSS OF USE,  DATA OR     *|
|*     PROFITS,  WHETHER IN AN ACTION  OF CONTRACT,  NEGLIGENCE OR OTHER     *|
|*     TORTIOUS ACTION, ARISING OUT  OF OR IN CONNECTION WITH THE USE OR     *|
|*     PERFORMANCE OF THIS SOURCE CODE.                                      *|
|*                                                                           *|
 \***************************************************************************/

/***************************** Object Manager *****************************\
*                                                                           *
* Module: CLASS.C                                                           *
*     Object Classes are defined in this module.                            *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*                                                                           *
\***************************************************************************/
#include <nv_ref.h>
#include <nvrm.h>
#include <class.h>
#include <vblank.h>
#include <fifo.h>
#include <dma.h>
#include <mc.h>
#include <gr.h>
#include <dac.h>
#include <tmr.h>
#include <os.h>
#include <modular.h>
#include <nv32.h>


//---------------------------------------------------------------------------
//
//  Class methods.
//
//---------------------------------------------------------------------------

METHOD NvPatchcordMethods[] =
{
    {mthdSetNotifyCtxDma,           0x0100, 0x0103},
    {mthdSetNotify,                 0x0104, 0x0107}
    //{mthdValidate,                  0x0300, 0x0303}
};
METHOD NvClassMethods[] =
{
    {mthdNoOperation,               0x0100, 0x0103},
    {mthdSetNotify,                 0x0104, 0x0107},
    {mthdSetNotifyCtxDma,           0x0180, 0x0183},
    {mthdCreateLegacy,              0x0300, 0x0303},
    {mthdDestroy,                   0x0304, 0x0307},
};
METHOD NvDmaFromMemoryMethods[] =
{
    {mthdNoOperation,               0x0100, 0x0103},
    {mthdSetNotify,                 0x0104, 0x0107},
    {mthdSetNotifyCtxDma,           0x0180, 0x0183},
    {mthdSetDmaSpecifier,           0x0300, 0x030b}
};
METHOD NvDmaToMemoryMethods[] =
{
    {mthdNoOperation,               0x0100, 0x0103},
    {mthdSetNotify,                 0x0104, 0x0107},
    {mthdSetNotifyCtxDma,           0x0180, 0x0183},
    {mthdSetDmaSpecifier,           0x0300, 0x030b}
};
METHOD NvDmaInMemoryMethods[] =
{
    {mthdNoOperation,               0x0100, 0x0103},
    {mthdSetNotify,                 0x0104, 0x0107},
    {mthdSetNotifyCtxDma,           0x0180, 0x0183},
    {mthdSetDmaSpecifier,           0x0300, 0x030b}
};
METHOD NvVideoSinkMethods[] =
{
    {mthdNoOperation,               0x0100, 0x0103},
    {mthdSetNotify,                 0x0104, 0x0107},
    {mthdSetNotifyCtxDma,           0x0180, 0x0183},
    {mthdSetVideoInput,             0x0200, 0x02ff}
};
METHOD NvVideoColormapMethods[] =
{
    {mthdNoOperation,               0x0100, 0x0103},
    {mthdSetNotify,                 0x0104, 0x0107},
    {mthdSetNotifyCtxDma,           0x0180, 0x0183},
    {mthdSetColormapCtxDma,         0x0184, 0x0187},
    {mthdSetVideoOutput,            0x0200, 0x0203},
    {mthdSetVideoInput,             0x0204, 0x0207},
    {mthdSetColormapStart,          0x0308, 0x030b},
    {mthdSetColormapLength,         0x030c, 0x030f},
    {mthdColormapDirtyStart,        0x0310, 0x0313},
    {mthdColormapDirtyLength,       0x0314, 0x0317}
};
METHOD Nv04VideoLUTCursorDACMethods[] =
{
    {mthdNoOperation,               0x0100, 0x0103},
    {class046SetNotify,             0x0104, 0x0107},
    {class046StopImage,             0x0108, 0x010f},
    {class046StopLUT,               0x0110, 0x0117},
    {class046StopCursorImage,       0x0118, 0x011f},
    {class046StopDAC,               0x0120, 0x0128},
    {class046SetNotifyCtxDma,       0x0180, 0x0183},
    {class046SetImageCtxDma,        0x0184, 0x018b},
    {class046SetLUTCtxDma,          0x018c, 0x0193},
    {class046SetCursorCtxDma,       0x0194, 0x019b},
    {class046SetPanOffset,          0x0220, 0x0223},
    {class046GetOffset,             0x02fc, 0x02ff},
    {class046SetImageValues,        0x0300, 0x030f},
    {class046SetLUTValues,          0x0310, 0x031f},
    {class046SetCursorImageValues,  0x0320, 0x032f},
    {class046SetCursorPoint,        0x0330, 0x0333},
    {class046SetDACValues,          0x0340, 0x037c}
};

METHOD Nv04VideoOverlayMethods[] =
{
    {mthdNoOperation,               0x0100, 0x0103},
    {class047SetNotify,             0x0104, 0x0107},
    {class047StopOverlay,           0x0120, 0x0127},
    {class047SetNotifyCtxDma,       0x0180, 0x0183},
    {class047SetOverlayCtxDma,      0x019c, 0x01a3},
    {class047GetOffset,             0x02fc, 0x02ff},
    {class047SetOverlayValues,      0x0400, 0x04ff},
    {class047SetOverlayPointOutNow, 0x0500, 0x0503}
};

METHOD Nv05VideoLUTCursorDACMethods[] =
{
    {mthdNoOperation,               0x0100, 0x0103},
    {class049SetNotify,             0x0104, 0x0107},
    {class049StopImage,             0x0108, 0x010f},
    {class049StopLUT,               0x0110, 0x0117},
    {class049StopCursorImage,       0x0118, 0x011f},
    {class049StopDAC,               0x0120, 0x0128},
    {class049SetNotifyCtxDma,       0x0180, 0x0183},
    {class049SetImageCtxDma,        0x0184, 0x018b},
    {class049SetLUTCtxDma,          0x018c, 0x0193},
    {class049SetCursorCtxDma,       0x0194, 0x019b},
    {class049SetPanOffset,          0x0220, 0x0223},
    {class049GetOffset,             0x02fc, 0x02ff},
    {class049SetImageValues,        0x0300, 0x030f},
    {class049SetLUTValues,          0x0310, 0x031f},
    {class049SetCursorImageValues,  0x0320, 0x032f},
    {class049SetCursorPoint,        0x0330, 0x0333},
    {class049SetDACValues,          0x0340, 0x037c}
};

METHOD NvVideoFromMemoryMethods[] =
{
    {mthdNoOperation,               0x0100, 0x0103},
    {class63SetNotify,              0x0104, 0x0107},
    {class63StopTransfer,           0x0108, 0x010b},
    {class63SetNotifyCtxDma,        0x0180, 0x0183},
    {class63SetImageCtxDma,         0x0184, 0x018b},
    {class63SetVideoOutput,         0x0200, 0x0203},
    {class63ImageScan,              0x0300, 0x0327},
    {class63GetOffsetNotify,        0x0328, 0x032f}
};
METHOD NvVideoScalerMethods[] =
{
    {mthdNoOperation,               0x0100, 0x0103},
    {class64SetNotify,              0x0104, 0x0107},
    {class64SetNotifyCtxDma,        0x0180, 0x0183},
    {class64SetVideoOutput,         0x0200, 0x0203},
    {class64SetVideoInput,          0x0204, 0x0207},
    {class64SetDeltaDuDx,           0x0300, 0x0303},
    {class64SetDeltaDvDy,           0x0304, 0x0307},
    {class64SetPoint,               0x0308, 0x030b}
};
METHOD NvVideoColorKeyMethods[] =
{
    {mthdNoOperation,               0x0100, 0x0103},
    {class65SetNotify,              0x0104, 0x0107},
    {class65SetNotifyCtxDma,        0x0180, 0x0183},
    {class65SetVideoOutput,         0x0200, 0x0203},
    {class65SetVideoInput,          0x0204, 0x020f},
    {class65SetColorFormat,         0x0300, 0x0303},
    {class65SetColorKey,            0x0304, 0x0307},
    {class65SetPoint,               0x0308, 0x030b},
    {class65SetSize,                0x030c, 0x030f}
};

/*
METHOD NvVideoCaptureToMemoryMethods[] =
{
    {class66SetNotifyCtxDma,        0x0100, 0x0103},
    {class66SetNotify,              0x0104, 0x0107},
    {class66StopTransfer,           0x0108, 0x010b},
    {class66SetVideoInput,          0x0200, 0x0203},
    {class66SetImageCtxDma,         0x0300, 0x0307},
    {class66SetImageNotifyCtxDma,   0x0308, 0x030f},
    {class66ImageCapture,           0x0310, 0x0337},
    {class66SetVbiCtxDma,           0x0400, 0x0407},
    {class66SetVbiNotifyCtxDma,     0x0408, 0x040f},
    {class66VbiCapture,             0x0410, 0x0437}
};
*/

METHOD Nv10VideoLUTCursorDACMethods[] =
{
    {mthdNoOperation,               0x0100, 0x0103},
    {class067SetNotify,             0x0104, 0x0107},
    {class067StopImage,             0x0108, 0x010f},
    {class067StopLUT,               0x0110, 0x0117},
    {class067StopCursorImage,       0x0118, 0x011f},
    {class067StopDAC,               0x0120, 0x0128},
    {class067SetNotifyCtxDma,       0x0180, 0x0183},
    {class067SetImageCtxDma,        0x0184, 0x018b},
    {class067SetLUTCtxDma,          0x018c, 0x0193},
    {class067SetCursorCtxDma,       0x0194, 0x019b},
    {class067SetPanOffset,          0x0220, 0x0223},
    {class067GetOffset,             0x02fc, 0x02ff},
    {class067SetImageValues,        0x0300, 0x030f},
    {class067SetLUTValues,          0x0310, 0x031f},
    {class067SetCursorImageValues,  0x0320, 0x032f},
    {class067SetCursorPoint,        0x0330, 0x0333},
    {class067SetDACValues,          0x0340, 0x037c}
};

METHOD Nv10VideoOverlayMethods[] =
{
    {mthdNoOperation,                  0x0100, 0x0103},
    {class07ASetNotify,                0x0104, 0x0107},
    {class07AStopOverlay,              0x0120, 0x0127},
    {class07ASetNotifyCtxDma,          0x0180, 0x0183},
    {class07ASetOverlayCtxDma,         0x019c, 0x01a3},
    {class07AGetOffset,                0x02fc, 0x02ff},
    {class07ASetOverlayValues,         0x0400, 0x04ff},
    {class07ASetOverlayPointOutNow,    0x0500, 0x0503},
    {class07ASetOverlayLuminanceNow,   0x0504, 0x0507},
    {class07ASetOverlayChrominanceNow, 0x0508, 0x050b}
};

METHOD Nv15VideoLUTCursorDACMethods[] =
{
    {mthdNoOperation,               0x0100, 0x0103},
    {class07CSetNotify,             0x0104, 0x0107},
    {class07CStopImage,             0x0108, 0x010f},
    {class07CStopLUT,               0x0110, 0x0117},
    {class07CStopCursorImage,       0x0118, 0x011f},
    {class07CStopDAC,               0x0120, 0x0128},
    {class07CSetNotifyCtxDma,       0x0180, 0x0183},
    {class07CSetImageCtxDma,        0x0184, 0x018b},
    {class07CSetLUTCtxDma,          0x018c, 0x0193},
    {class07CSetCursorCtxDma,       0x0194, 0x019b},
    {class07CSetSemaphoreCtxDma,    0x019c, 0x019f},
    {class07CSetHotPlug,            0x01a0, 0x01a3},
    {class07CSetPanOffset,          0x0220, 0x0223},
    {class07CSetSemaphoreOffset,    0x02f0, 0x02f3},
    {class07CSetSemaphoreRelease,   0x02f4, 0x02f7},
    {class07CSetOffsetRange,        0x02f8, 0x02fb},
    {class07CGetOffset,             0x02fc, 0x02ff},
    {class07CSetImageValues,        0x0300, 0x030f},
    {class07CSetLUTValues,          0x0310, 0x031f},
    {class07CSetCursorImageValues,  0x0320, 0x032f},
    {class07CSetCursorPoint,        0x0330, 0x0333},
    {class07CSetDACValues,          0x0340, 0x037c},
};

METHOD Nv20VideoLUTCursorDACMethods[] =
{
    {mthdNoOperation,               0x0100, 0x0103},
    {class07CSetNotify,             0x0104, 0x0107},
    {class07CStopImage,             0x0108, 0x010f},
    {class07CStopLUT,               0x0110, 0x0117},
    {class07CStopCursorImage,       0x0118, 0x011f},
    {class07CStopDAC,               0x0120, 0x0128},
    {class07CSetNotifyCtxDma,       0x0180, 0x0183},
    {class07CSetImageCtxDma,        0x0184, 0x018b},
    {class07CSetLUTCtxDma,          0x018c, 0x0193},
    {class07CSetCursorCtxDma,       0x0194, 0x019b},
    {class07CSetSemaphoreCtxDma,    0x019c, 0x019f},
    {class07CSetHotPlug,            0x01a0, 0x01a3},
    {class07CSetPanOffset,          0x0220, 0x0223},
    {class07CSetSemaphoreOffset,    0x02f0, 0x02f3},
    {class07CSetSemaphoreRelease,   0x02f4, 0x02f7},
    {class07CSetOffsetRange,        0x02f8, 0x02fb},
    {class07CGetOffset,             0x02fc, 0x02ff},
    {class07CSetImageValues,        0x0300, 0x030f},
    {class07CSetLUTValues,          0x0310, 0x031f},
    {class07CSetCursorImageValues,  0x0320, 0x032f},
    {class07CSetCursorPoint,        0x0330, 0x0333},
    {class07CSetDACValues,          0x0340, 0x037c},
};

/*
METHOD NvVideoSourceMethods[] =
{
    {class74SetNotifyCtxDma,        0x0100, 0x0103},
    {class74SetNotify,              0x0104, 0x0107},
    {class74SetVideoOutput,         0x0200, 0x0203},
};
*/

METHOD Nv1TimerMethods[] =
{
    {mthdNoOperation,               0x0100, 0x0103},
    {mthdSetNotify,                 0x0104, 0x0107},
    {tmrStopTransfer,               0x0108, 0x010b},
    {mthdSetNotifyCtxDma,           0x0180, 0x0183},
    {mthdSetAlarmTime,              0x0300, 0x0307},
    {mthdSetAlarmTimeRelative,      0x0308, 0x030f},
    {mthdSetAlarmNotify,            0x0320, 0x0323}
};
METHOD Nv1BetaSolidMethods[] =
{
    {mthdNoOperation,               0x0100, 0x0103},
    {class012Notify,                0x0104, 0x0107},
    {class012SetContextDmaNotifies, 0x0180, 0x0183},
    {class012SetBeta,               0x0300, 0x0303}
};
METHOD Nv1ImageSolidMethods[] =
{
    {mthdNoOperation,               0x0100, 0x0103},
    {class017Notify,                0x0104, 0x0107},
    {class017SetContextDmaNotifies, 0x0180, 0x0183},
    {class017SetColorFormat,        0x0300, 0x0303},
    {class017SetColor,              0x0304, 0x0307}
};
METHOD Nv1ImagePatternMethods[] =
{
    {mthdNoOperation,               0x0100, 0x0103},
    {class018Notify,                0x0104, 0x0107},
    {class018SetContextDmaNotifies, 0x0180, 0x0183},
    {class018SetColorFormat,        0x0300, 0x0303},
    {class018SetMonochromeFormat,   0x0304, 0x0307},
    {class018SetPatternShape,       0x0308, 0x030b},
    {class018SetColor0,             0x0310, 0x0313},
    {class018SetColor1,             0x0314, 0x0317},
    {class018SetPattern,            0x0318, 0x031f}
};
METHOD Nv1ImageRectangleBlackMethods[] =
{
    {mthdNoOperation,               0x0100, 0x0103},
    {class019Notify,                0x0104, 0x0107},
    {class019SetContextDmaNotifies, 0x0180, 0x0183},
    {class019SetRectangle,          0x0300, 0x0307}
};
METHOD Nv1RenderSolidLinMethods[] =
{
    {mthdNoOperation,               0x0100, 0x0103},
    {class01CNotify,                0x0104, 0x0107},
    {class01CSetContextDmaNotifies, 0x0180, 0x0183},
    {class01CSetContextClip,        0x0184, 0x0187},
    {class01CSetContextPattern,     0x0188, 0x018b},
    {class01CSetContextRop,         0x018c, 0x018f},
    {class01CSetContextBeta1,       0x0190, 0x0193},
    {class01CSetContextSurface,     0x0194, 0x0197},
    {class01CSetOperation,          0x02fc, 0x02ff},
    {class01CSetColorFormat,        0x0300, 0x0303},
    {class01CColor,                 0x0304, 0x0307},
    {class01CLin,                   0x0400, 0x047f},
    {class01CLin32,                 0x0480, 0x04ff},
    {class01CPolyLin,               0x0500, 0x057f},
    {class01CPolyLin32,             0x0580, 0x05ff},
    {class01CColorPolyLin,          0x0600, 0x067f}
};
METHOD Nv1RenderSolidTriangleMethods[] =
{
    {mthdNoOperation,               0x0100, 0x0103},
    {class01DNotify,                0x0104, 0x0107},
    {class01DSetContextDmaNotifies, 0x0180, 0x0183},
    {class01DSetContextClip,        0x0184, 0x0187},
    {class01DSetContextPattern,     0x0188, 0x018b},
    {class01DSetContextRop,         0x018c, 0x018f},
    {class01DSetContextBeta1,       0x0190, 0x0193},
    {class01DSetContextSurface,     0x0194, 0x0197},
    {class01DSetOperation,          0x02fc, 0x02ff},
    {class01DSetColorFormat,        0x0300, 0x0303},
    {class01DColor,                 0x0304, 0x0307},
    {class01DTriangle,              0x0310, 0x031b},
    {class01DTriangle32,            0x0320, 0x0337},
    {class01DTrimesh,               0x0400, 0x047f},
    {class01DTrimesh32,             0x0480, 0x04ff},
    {class01DColorTriangle,         0x0500, 0x057f},
    {class01DColorTrimesh,          0x0580, 0x05ff}
};
METHOD Nv1RenderSolidRectangleMethods[] =
{
    {mthdNoOperation,               0x0100, 0x0103},
    {class01ENotify,                0x0104, 0x0107},
    {class01ESetContextDmaNotifies, 0x0180, 0x0183},
    {class01ESetContextClip,        0x0184, 0x0187},
    {class01ESetContextPattern,     0x0188, 0x018b},
    {class01ESetContextRop,         0x018c, 0x018f},
    {class01ESetContextBeta1,       0x0190, 0x0193},
    {class01ESetContextSurface,     0x0194, 0x0197},
    {class01ESetOperation,          0x02fc, 0x02ff},
    {class01ESetColorFormat,        0x0300, 0x0303},
    {class01EColor,                 0x0304, 0x0307},
    {class01ERectangle,             0x0400, 0x047f}
};
METHOD Nv1ImageBlitMethods[] =
{
    {mthdNoOperation,               0x0100, 0x0103},
    {class01FNotify,                0x0104, 0x0107},
    {class01FSetContextDmaNotifies, 0x0180, 0x0183},
    {class01FSetContextColorKey,    0x0184, 0x0187},
    {class01FSetContextClip,        0x0188, 0x018b},
    {class01FSetContextPattern,     0x018c, 0x018f},
    {class01FSetContextRop,         0x0190, 0x0193},
    {class01FSetContextBeta1,       0x0194, 0x0197},
    {class01FSetContextSurfaceSource, 0x0198, 0x019b},
    {class01FSetContextSurface,     0x019c, 0x019f},
    {class01FSetOperation,          0x02fc, 0x02ff},
    {class01FPointIn,               0x0300, 0x0303},
    {class01FPointOut,              0x0304, 0x0307},
    {class01FSize,                  0x0308, 0x030b}
};
METHOD Nv1ImageFromCpuMethods[] =
{
    {mthdNoOperation,               0x0100, 0x0103},
    {class021Notify,                0x0104, 0x0107},
    {class021SetContextDmaNotifies, 0x0180, 0x0183},
    {class021SetContextColorKey,    0x0184, 0x0187},
    {class021SetContextClip,        0x0188, 0x018b},
    {class021SetContextPattern,     0x018c, 0x018f},
    {class021SetContextRop,         0x0190, 0x0193},
    {class021SetContextBeta1,       0x0194, 0x0197},
    {class021SetContextSurface,     0x0198, 0x019b},
    {class021SetOperation,          0x02fc, 0x02ff},
    {class021SetColorFormat,        0x0300, 0x0303},
    {class021Point,                 0x0304, 0x0307},
    {class021Size,                  0x0308, 0x030b},
    {class021SizeIn,                0x030c, 0x030f},
    {class021Color,                 0x0400, 0x047f}
};
METHOD Nv3StretchedImageFromCPUMethods[] =
{
    {mthdNoOperation,               0x0100, 0x0103},
    {class036Notify,                0x0104, 0x0107},
    {class036SetContextDmaNotifies, 0x0180, 0x0183},
    {class036SetContextColorKey,    0x0184, 0x0187},
    {class036SetContextPattern,     0x0188, 0x018b},
    {class036SetContextRop,         0x018c, 0x018f},
    {class036SetContextBeta1,       0x0190, 0x0193},
    {class036SetContextSurface,     0x0194, 0x0197},
    {class036SetOperation,          0x02fc, 0x02ff},
    {class036SetColorFormat,        0x0300, 0x0303},
    {class036SizeIn,                0x0304, 0x0307},
    {class036DeltaDxDu,             0x0308, 0x030b},
    {class036DeltaDyDv,             0x030c, 0x030f},
    {class036Clip,                  0x0310, 0x0317},
    {class036Point12d4,             0x0318, 0x031b},
    {class036Color,                 0x0400, 0x1fff}
};
METHOD Nv3ScaledImageFromMemoryMethods[] =
{
    {mthdNoOperation,               0x0100, 0x0103},
    {class037Notify,                0x0104, 0x0107},
    {class037SetContextDmaNotifies, 0x0180, 0x0183},
    {class037SetContextDmaImage,    0x0184, 0x0187},
    {class037SetContextPattern,     0x0188, 0x018b},
    {class037SetContextRop,         0x018c, 0x018f},
    {class037SetContextBeta1,       0x0190, 0x0193},
    {class037SetContextSurface,     0x0194, 0x0197},
    {class037SetColorFormat,        0x0300, 0x0303},
    {class037SetOperation,          0x0304, 0x0307},
    {class037Clip,                  0x0308, 0x030f},
    {class037RectangleOut,          0x0310, 0x0317},
    {class037DeltaDuDx,             0x0318, 0x031b},
    {class037DeltaDvDy,             0x031c, 0x031f},
    {class037ImageIn,               0x0400, 0x040f}
};
METHOD Nv3MemoryToMemoryFormatMethods[] =
{
    {mthdNoOperation,               0x0100, 0x0103},
    {class039Notify,                0x0104, 0x0107},
    {class039SetContextDmaNotifies, 0x0180, 0x0183},
    {class039SetContextDmaBufferIn, 0x0184, 0x0187},
    {class039SetContextDmaBufferOut, 0x0188, 0x018b},
    {class039BufferTransfer,        0x030c, 0x032b}
};
METHOD Nv3DX3TexturedTriangleMethods[] =
{
    {mthdNoOperation,               0x0100, 0x0103},
    {class048Notify,                0x0104, 0x0107},
    {class048SetContextDmaNotifies, 0x0180, 0x0183},
    {class048SetTextureCtxDma,      0x0184, 0x0187},
    {class048SetContextClip,        0x0188, 0x018b},
    {class048SetContextSurfaceColor, 0x018c, 0x018f},
    {class048SetContextSurfaceZeta, 0x0190, 0x0193},
    {class048TextureOffset,         0x0304, 0x0307},
    {class048TextureFormat,         0x0308, 0x030b},
    {class048Filter,                0x030c, 0x030f},
    {class048FogColor,              0x0310, 0x0313},
    {class048ControlOut0,           0x0314, 0x0317},
    {class048ControlOut1,           0x0318, 0x031b},
    {class048ControlPoint,          0x1000, 0x1fff}
};
METHOD Nv3GDIRectangleTextMethods[] =
{
    {mthdNoOperation,               0x0100, 0x0103},
    {class04BNotify,                0x0104, 0x0107},
    {class04BSetContextDmaNotifies, 0x0180, 0x0183},
    {class04BSetContextPattern,     0x0184, 0x0187},
    {class04BSetContextRop,         0x0188, 0x018b},
    {class04BSetContextBeta1,       0x018c, 0x018f},
    {class04BSetContextSurface,     0x0190, 0x0193},
    {class04BSetOperation,          0x02fc, 0x02ff},
    {class04BSetColorFormat,        0x0300, 0x0303},
    {class04BSetMonochromeFormat,   0x0304, 0x0307},
    {class04BColor1A,               0x03fc, 0x03ff},
    {class04BUnclippedRectangle,    0x0400, 0x05ff},
    {class04BClipB,                 0x07f4, 0x07fb},
    {class04BColor1B,               0x07fc, 0x07ff},
    {class04BClippedRectangle,      0x0800, 0x09ff},
    {class04BClipC,                 0x0bec, 0x0bf3},
    {class04BColor1C,               0x0bf4, 0x0bf7},
    {class04BSizeC,                 0x0bf8, 0x0bfb},
    {class04BPointC,                0x0bfc, 0x0bff},
    {class04BMonochromeColor1C,     0x0c00, 0x0dff},
    {class04BClipD,                 0x0fe8, 0x0fef},
    {class04BColor1D,               0x0ff0, 0x0ff3},
    {class04BSizeInD,               0x0ff4, 0x0ff7},
    {class04BSizeOutD,              0x0ff8, 0x0ffb},
    {class04BPointD,                0x0ffc, 0x0fff},
    {class04BMonochromeColor1D,     0x1000, 0x11ff},
    {class04BClipE,                 0x13e4, 0x13eb},
    {class04BColor0E,               0x13ec, 0x13ef},
    {class04BColor1E,               0x13f0, 0x13f3},
    {class04BSizeInE,               0x13f4, 0x13f7},
    {class04BSizeOutE,              0x13f8, 0x13fb},
    {class04BPointE,                0x13fc, 0x13ff},
    {class04BMonochromeColor01E,    0x1400, 0x15ff}
};
METHOD Nv3ExternalVideoDecoderMethods[] =
{
    {mthdNoOperation,               0x0100, 0x0103},
    {class04DNotify,                0x0104, 0x0107},
    {class04DStopTransferVbi,       0x0108, 0x010b},
    {class04DStopTransferImage,     0x010c, 0x010f},
    {class04DSetContextDmaNotifies, 0x0180, 0x0183},
    {class04DSetContextDmaVbi,      0x0184, 0x018b},
    {class04DSetContextDmaImage,    0x018c, 0x0193},
    {class04DSetImageConfig,        0x02f8, 0x02fb},
    {class04DSetImageStartLine,     0x02fc, 0x02ff},
    {class04DSetVbi,                0x0300, 0x0317},
    {class04DGetVbiOffsetNotify,    0x0318, 0x031f},
    {class04DSetImage,              0x0320, 0x033f},
    {class04DGetImageOffsetNotify,  0x0340, 0x0347}
};
METHOD Nv3ExternalVideoDecompressorMethods[] =
{
    {mthdNoOperation,               0x0100, 0x0103},
    {class04ENotify,                0x0104, 0x0107},
    {class04EStopTransfer,          0x0108, 0x010b},
    {class04ESetContextDmaNotifies, 0x0180, 0x0183},
    {class04ESetContextDmaData,     0x0184, 0x018b},
    {class04ESetContextDmaImage,    0x018c, 0x0193},
    {class04EReadData,              0x0300, 0x0317},
    {class04EWriteImage,            0x0318, 0x033f}
};
METHOD Nv1ExternalParallelBusMethods[] =
{
    {mthdNoOperation,               0x0100, 0x0103},
    {class04FNotify,                0x0104, 0x0107},
    {class04FStopTransfer,          0x0108, 0x010b},
    {class04FSetContextDmaNotifies, 0x0180, 0x0183},
    {class04FWrite,                 0x0300, 0x030b},
    {class04FRead,                  0x030c, 0x0317},
    {class04FSetInterruptNotify,    0x0318, 0x031b}
};
METHOD Nv3ExternalMonitorBusMethods[] =
{
    {mthdNoOperation,               0x0100, 0x0103},
    {class050Notify,                0x0104, 0x0107},
    {class050StopTransfer,          0x0108, 0x010b},
    {class050SetContextDmaNotifies, 0x0180, 0x0183},
    {class050Write,                 0x0300, 0x030b},
    {class050Read,                  0x030c, 0x0317},
    {class050SetInterruptNotify,    0x0318, 0x031b}
};
METHOD Nv4ExternalSerialBusMethods[] =
{
    {mthdNoOperation,               0x0100, 0x0103},
    {class051Notify,                0x0104, 0x0107},
    {class051StopTransfer,          0x0108, 0x010b},
    {class051SetContextDmaNotifies, 0x0180, 0x0183},
    {class051Write,                 0x0300, 0x030b},
    {class051Read,                  0x030c, 0x0317},
    {class051SetInterruptNotify,    0x0318, 0x031b}
};
METHOD Nv3ContextSurface0Methods[] =
{
    {mthdNoOperation,               0x0100, 0x0103},
    {class058Notify,                0x0104, 0x0107},
    {class058SetContextDmaNotifies, 0x0180, 0x0183},
    {class058SetContextDmaImage,    0x0184, 0x0187},
    {class058SetColorFormat,        0x0300, 0x0303},
    {class058SetImagePitch,         0x0308, 0x030b},
    {class058SetImageOffset,        0x030c, 0x030f}
};
METHOD Nv3ContextSurface1Methods[] =
{
    {mthdNoOperation,               0x0100, 0x0103},
    {class059Notify,                0x0104, 0x0107},
    {class059SetContextDmaNotifies, 0x0180, 0x0183},
    {class059SetContextDmaImage,    0x0184, 0x0187},
    {class059SetColorFormat,        0x0300, 0x0303},
    {class059SetImagePitch,         0x0308, 0x030b},
    {class059SetImageOffset,        0x030c, 0x030f}
};
METHOD Nv3ContextSurface2Methods[] =
{
    {mthdNoOperation,               0x0100, 0x0103},
    {class05ANotify,                0x0104, 0x0107},
    {class05ASetContextDmaNotifies, 0x0180, 0x0183},
    {class05ASetContextDmaImage,    0x0184, 0x0187},
    {class05ASetColorFormat,        0x0300, 0x0303},
    {class05ASetImagePitch,         0x0308, 0x030b},
    {class05ASetImageOffset,        0x030c, 0x030f}
};
METHOD Nv3ContextSurface3Methods[] =
{
    {mthdNoOperation,               0x0100, 0x0103},
    {class05BNotify,                0x0104, 0x0107},
    {class05BSetContextDmaNotifies, 0x0180, 0x0183},
    {class05BSetContextDmaImage,    0x0184, 0x0187},
    {class05BSetColorFormat,        0x0300, 0x0303},
    {class05BSetImagePitch,         0x0308, 0x030b},
    {class05BSetImageOffset,        0x030c, 0x030f}
};
METHOD Nv3ContextRopMethods[] =
{
    {mthdNoOperation,               0x0100, 0x0103},
    {class043Notify,                0x0104, 0x0107},
    {class043SetContextDmaNotifies, 0x0180, 0x0183},
    {class043SetRop5,               0x0300, 0x0303}
};
METHOD Nv4DvdSubpictureMethods[] =
{
    {mthdNoOperation,               0x0100, 0x0103},
    {class038Notify,                0x0104, 0x0107},
    {class038SetContextDmaNotifies, 0x0180, 0x0183},
    {class038SetContextDmaOverlay,  0x0184, 0x0187},
    {class038SetContextDmaImageIn,  0x0188, 0x018b},
    {class038SetContextDmaImageOut, 0x018c, 0x018f},
    {class038ImageOutPoint,         0x0300, 0x0303},
    {class038ImageOutSize,          0x0304, 0x0307},
    {class038ImageOutFormat,        0x0308, 0x030b},
    {class038ImageOutOffset,        0x030c, 0x030f},
    {class038ImageInDeltaDuDx,      0x0310, 0x0313},
    {class038ImageInDeltaDvDy,      0x0314, 0x0317},
    {class038ImageInSize,           0x0318, 0x031b},
    {class038ImageInFormat,         0x031c, 0x031f},
    {class038ImageInOffset,         0x0320, 0x0323},
    {class038ImageInPoint,          0x0324, 0x0317},
    {class038OverlayDeltaDuDx,      0x0328, 0x032b},
    {class038OverlayDeltaDvDy,      0x032c, 0x032f},
    {class038OverlaySize,           0x0330, 0x0333},
    {class038OverlayFormat,         0x0334, 0x0337},
    {class038OverlayOffset,         0x0338, 0x033b},
    {class038OverlayPoint,          0x033c, 0x033f}
};
METHOD Nv4ContextSurfaces2DMethods[] =
{
    {mthdNoOperation,               0x0100, 0x0103},
    {class042Notify,                0x0104, 0x0107},
    {class042SetContextDmaNotifies, 0x0180, 0x0183},
    {class042SetContextDmaSource,   0x0184, 0x0187},
    {class042SetContextDmaDestin,   0x0188, 0x018b},
    {class042SetColorFormat,        0x0300, 0x0303},
    {class042SetPitch,              0x0304, 0x0307},
    {class042SetOffsetSource,       0x0308, 0x030b},
    {class042SetOffsetDestin,       0x030c, 0x030f}
};
METHOD Nv4ContextPatternMethods[] =
{
    {mthdNoOperation,               0x0100, 0x0103},
    {class044Notify,                0x0104, 0x0107},
    {class044SetContextDmaNotifies, 0x0180, 0x0183},
    {class044SetColorFormat,        0x0300, 0x0303},
    {class044SetMonochromeFormat,   0x0304, 0x0307},
    {class044SetMonochromeShape,    0x0308, 0x030b},
    {class044SetPatternSelect,      0x030c, 0x030f},
    {class044SetMonochromeColor,    0x0310, 0x0317},
    {class044SetMonochromePattern,  0x0318, 0x031f},
    {class044SetPatternY8,          0x0400, 0x043f},
    {class044SetPatternR5G6B5,      0x0500, 0x057f},
    {class044SetPatternX8R8G8B8,    0x0700, 0x07ff}
};
METHOD Nv4GDIRectangleTextMethods[] =
{
    {mthdNoOperation,               0x0100, 0x0103},
    {class04ANotify,                0x0104, 0x0107},
    {class04ASetContextDmaNotifies, 0x0180, 0x0183},
    {class04ASetContextFonts,       0x0184, 0x0187},
    {class04ASetContextPattern,     0x0188, 0x018b},
    {class04ASetContextRop,         0x018c, 0x018f},
    {class04ASetContextBeta1,       0x0190, 0x0193},
    {class04ASetContextBeta4,       0x0194, 0x0197},
    {class04ASetContextSurface,     0x0198, 0x019b},
    {class04ASetOperation,          0x02fc, 0x02ff},
    {class04ASetColorFormat,        0x0300, 0x0303},
    {class04ASetMonochromeFormat,   0x0304, 0x0307},
    {class04AColor1A,               0x03fc, 0x03ff},
    {class04AUnclippedRectangle,    0x0400, 0x04ff},
    {class04AClipB,                 0x05f4, 0x05fb},
    {class04AColor1B,               0x05fc, 0x05ff},
    {class04AClippedRectangle,      0x0600, 0x06ff},
    {class04AClipC,                 0x07ec, 0x07f3},
    {class04AColor1C,               0x07f4, 0x07f7},
    {class04ASizeC,                 0x07f8, 0x07fb},
    {class04APointC,                0x07fc, 0x07ff},
    {class04AMonochromeColor1C,     0x0800, 0x09ff},
    {class04AClipE,                 0x0be4, 0x0beb},
    {class04AColor0E,               0x0bec, 0x0bef},
    {class04AColor1E,               0x0bf0, 0x0bf3},
    {class04ASizeInE,               0x0bf4, 0x0bf7},
    {class04ASizeOutE,              0x0bf8, 0x0bfb},
    {class04APointE,                0x0bfc, 0x0bff},
    {class04AMonochromeColor01E,    0x0c00, 0x0dff},
    {class04AFontF,                 0x0ff0, 0x0ff3},
    {class04AClipF,                 0x0ff4, 0x0ffb},
    {class04AColor1F,               0x0ffc, 0x0fff},
    {class04ACharacterColor1F,      0x1000, 0x13ff},
    {class04AFontG,                 0x17f0, 0x17f3},
    {class04AClipG,                 0x17f4, 0x17fb},
    {class04AColor1G,               0x17fc, 0x17ff},
    {class04ACharacterColor1G,      0x1800, 0x1fff}
};
METHOD Nv4ContextSurfaceSwizzledMethods[] =
{
    {mthdNoOperation,               0x0100, 0x0103},
    {class052Notify,                0x0104, 0x0107},
    {class052SetContextDmaNotifies, 0x0180, 0x0183},
    {class052SetContextDmaImage,    0x0184, 0x0187},
    {class052SetFormat,             0x0300, 0x0303},
    {class052SetOffset,             0x0304, 0x0307}
};
METHOD Nv4ContextSurfacesArgbZsMethods[] =
{
    {mthdNoOperation,               0x0100, 0x0103},
    {class053Notify,                0x0104, 0x0107},
    {class053SetContextDmaNotifies, 0x0180, 0x0183},
    {class053SetContextDmaColor,    0x0184, 0x0187},
    {class053SetContextDmaZeta,     0x0188, 0x018b},
    {class053SetClipHorizontal,     0x02f8, 0x02fb},
    {class053SetClipVertical,       0x02fc, 0x02ff},
    {class053SetFormat,             0x0300, 0x0303},
    {class053SetClipSize,           0x0304, 0x0307},
    {class053SetPitch,              0x0308, 0x030b},
    {class053SetOffsetColor,        0x030c, 0x030f},
    {class053SetOffsetZeta,         0x0310, 0x0313}
};
METHOD Nv4DX5TexturedTriangleMethods[] =
{
    {mthdNoOperation,               0x0100, 0x0103},
    {class054Notify,                0x0104, 0x0107},
    {class054SetContextDmaNotifies, 0x0180, 0x0183},
    {class054SetContextDmaA,        0x0184, 0x0187},
    {class054SetContextDmaB,        0x0188, 0x018b},
    {class054SetContextSurface,     0x018c, 0x018f},
    {class054ColorKey,              0x0300, 0x0303},
    {class054Offset,                0x0304, 0x0307},
    {class054Format,                0x0308, 0x030b},
    {class054Filter,                0x030c, 0x030f},
    {class054Blend,                 0x0310, 0x0313},
    {class054Control,               0x0314, 0x0317},
    {class054FogColor,              0x0318, 0x031b},
    {class054Tlvertex,              0x0400, 0x05ff},
    {class054DrawPrimitive,         0x0600, 0x06ff}
};
METHOD Nv4DX6MultiTextureTriangleMethods[] =
{
    {mthdNoOperation,               0x0100, 0x0103},
    {class055Notify,                0x0104, 0x0107},
    {class055SetContextDmaNotifies, 0x0180, 0x0183},
    {class055SetContextDmaA,        0x0184, 0x0187},
    {class055SetContextDmaB,        0x0188, 0x018b},
    {class055SetContextSurface,     0x018c, 0x018f},
    {class055Offset,                0x0308, 0x030f},
    {class055Format,                0x0310, 0x0317},
    {class055Filter,                0x0318, 0x031f},
    {class055Combine0Alpha,         0x0320, 0x0323},
    {class055Combine0Color,         0x0324, 0x0327},
    {class055Combine1Alpha,         0x032c, 0x032f},
    {class055Combine1Color,         0x0330, 0x0333},
    {class055CombineFactor,         0x0334, 0x0337},
    {class055Blend,                 0x0338, 0x033b},
    {class055Control,               0x033c, 0x0347},
    {class055FogColor,              0x0348, 0x034b},
    {class055Tlmtvertex,            0x0400, 0x053f},
    {class055DrawPrimitive,         0x0540, 0x05ff}
};
METHOD Nv10CelsiusPrimitiveMethods[] =
{
    {mthdNoOperation,               0x0100, 0x0103},
    {class056Notify,                0x0104, 0x0107},
    {class056SetWarningEnable,      0x0108, 0x010b},
    //{class056GetState,              0x010c, 0x010f},
    {class056WaitForIdle,           0x0110, 0x0113},
    {class056PMTrigger,             0x0140, 0x0143},
    {class056SetContextDmaNotifies, 0x0180, 0x0183},
    {class056SetContextDmaA,        0x0184, 0x0187},
    {class056SetContextDmaB,        0x0188, 0x018b},
    {class056SetContextDmaVertex,   0x018c, 0x018f},
    {class056SetContextDmaState,    0x0190, 0x0193},
    {class056SetContextDmaColor,    0x0194, 0x0197},
    {class056SetContextDmaZeta,     0x0198, 0x019b},
    {class056MethodCatcher,         0x019c, 0x17ff},    // catch any in between methods
    //{class056SetMaterialEmission,   0x1628, 0x1633},    // HW workaround for NV10
    //{class056MethodCatcher,         0x1634, 0x17ff},    // catch any in between methods
    {class056InlineArray,           0x1800, 0x1fff},
};
METHOD Nv4ContextColorKeyMethods[] =
{
    {mthdNoOperation,               0x0100, 0x0103},
    {class057Notify,                0x0104, 0x0107},
    {class057SetContextDmaNotifies, 0x0180, 0x0183},
    {class057SetColorFormat,        0x0300, 0x0303},
    {class057SetColor,              0x0304, 0x0307}
};
METHOD Nv4RenderSolidLinMethods[] =
{
    {mthdNoOperation,               0x0100, 0x0103},
    {class05CNotify,                0x0104, 0x0107},
    {class05CSetContextDmaNotifies, 0x0180, 0x0183},
    {class05CSetContextClip,        0x0184, 0x0187},
    {class05CSetContextPattern,     0x0188, 0x018b},
    {class05CSetContextRop,         0x018c, 0x018f},
    {class05CSetContextBeta1,       0x0190, 0x0193},
    {class05CSetContextBeta4,       0x0194, 0x0197},
    {class05CSetContextSurface,     0x0198, 0x019b},
    {class05CSetOperation,          0x02fc, 0x02ff},
    {class05CSetColorFormat,        0x0300, 0x0303},
    {class05CColor,                 0x0304, 0x0307},
    {class05CLin,                   0x0400, 0x047f},
    {class05CLin32,                 0x0480, 0x04ff},
    {class05CPolyLin,               0x0500, 0x057f},
    {class05CPolyLin32,             0x0580, 0x05ff},
    {class05CColorPolyLin,          0x0600, 0x067f}
};
METHOD Nv4RenderSolidTriangleMethods[] =
{
    {mthdNoOperation,               0x0100, 0x0103},
    {class05DNotify,                0x0104, 0x0107},
    {class05DSetContextDmaNotifies, 0x0180, 0x0183},
    {class05DSetContextClip,        0x0184, 0x0187},
    {class05DSetContextPattern,     0x0188, 0x018b},
    {class05DSetContextRop,         0x018c, 0x018f},
    {class05DSetContextBeta1,       0x0190, 0x0193},
    {class05DSetContextBeta4,       0x0194, 0x0197},
    {class05DSetContextSurface,     0x0198, 0x019b},
    {class05DSetOperation,          0x02fc, 0x02ff},
    {class05DSetColorFormat,        0x0300, 0x0303},
    {class05DColor,                 0x0304, 0x0307},
    {class05DTriangle,              0x0310, 0x031b},
    {class05DTriangle32,            0x0320, 0x0337},
    {class05DTrimesh,               0x0400, 0x047f},
    {class05DTrimesh32,             0x0480, 0x04ff},
    {class05DColorTriangle,         0x0500, 0x057f},
    {class05DColorTrimesh,          0x0580, 0x05ff}
};
METHOD Nv4RenderSolidRectangleMethods[] =
{
    {mthdNoOperation,               0x0100, 0x0103},
    {class05ENotify,                0x0104, 0x0107},
    {class05ESetContextDmaNotifies, 0x0180, 0x0183},
    {class05ESetContextClip,        0x0184, 0x0187},
    {class05ESetContextPattern,     0x0188, 0x018b},
    {class05ESetContextRop,         0x018c, 0x018f},
    {class05ESetContextBeta1,       0x0190, 0x0193},
    {class05ESetContextBeta4,       0x0194, 0x0197},
    {class05ESetContextSurface,     0x0198, 0x019b},
    {class05ESetOperation,          0x02fc, 0x02ff},
    {class05ESetColorFormat,        0x0300, 0x0303},
    {class05EColor,                 0x0304, 0x0307},
    {class05ERectangle,             0x0400, 0x047f}
};
METHOD Nv4ImageBlitMethods[] =
{
    {mthdNoOperation,               0x0100, 0x0103},
    {class05FNotify,                0x0104, 0x0107},
    {class05FSetContextDmaNotifies, 0x0180, 0x0183},
    {class05FSetContextColorKey,    0x0184, 0x0187},
    {class05FSetContextClip,        0x0188, 0x018b},
    {class05FSetContextPattern,     0x018c, 0x018f},
    {class05FSetContextRop,         0x0190, 0x0193},
    {class05FSetContextBeta1,       0x0194, 0x0197},
    {class05FSetContextBeta4,       0x0198, 0x019b},
    {class05FSetContextSurfaces2d,  0x019c, 0x019f},
    {class05FSetOperation,          0x02fc, 0x02ff},
    {class05FControlPointIn,        0x0300, 0x0303},
    {class05FControlPointOut,       0x0304, 0x0307},
    {class05FSize,                  0x0308, 0x030b}
};
METHOD Nv4IndexedImageFromCpuMethods[] =
{
    {mthdNoOperation,               0x0100, 0x0103},
    {class060Notify,                0x0104, 0x0107},
    {class060SetContextDmaNotifies, 0x0180, 0x0183},
    {class060SetContextDmaLut,      0x0184, 0x0187},
    {class060SetContextColorKey,    0x0188, 0x018b},
    {class060SetContextClip,        0x018c, 0x018f},
    {class060SetContextPattern,     0x0190, 0x0193},
    {class060SetContextRop,         0x0194, 0x0197},
    {class060SetContextBeta1,       0x0198, 0x019b},
    {class060SetContextBeta4,       0x019c, 0x019f},
    {class060SetContextSurface,     0x01a0, 0x01a3},
    {class060SetColorConversion,    0x03e0, 0x03e3},
    {class060SetOperation,          0x03e4, 0x03e7},
    {class060SetColorFormat,        0x03e8, 0x03eb},
    {class060IndexFormat,           0x03ec, 0x03ef},
    {class060LutOffset,             0x03f0, 0x03f3},
    {class060Point,                 0x03f4, 0x03f7},
    {class060SizeOut,               0x03f8, 0x03fb},
    {class060SizeIn,                0x03fc, 0x03ff},
    {class060Indices,               0x0400, 0x1fff}
};
METHOD Nv4ImageFromCpuMethods[] =
{
    {mthdNoOperation,               0x0100, 0x0103},
    {class061Notify,                0x0104, 0x0107},
    {class061SetContextDmaNotifies, 0x0180, 0x0183},
    {class061SetContextColorKey,    0x0184, 0x0187},
    {class061SetContextClip,        0x0188, 0x018b},
    {class061SetContextPattern,     0x018c, 0x018f},
    {class061SetContextRop,         0x0190, 0x0193},
    {class061SetContextBeta1,       0x0194, 0x0197},
    {class061SetContextBeta4,       0x0198, 0x019b},
    {class061SetContextSurface,     0x019c, 0x019f},
    {class061SetOperation,          0x02fc, 0x02ff},
    {class061SetColorFormat,        0x0300, 0x0303},
    {class061Point,                 0x0304, 0x0307},
    {class061SizeOut,               0x0308, 0x030b},
    {class061SizeIn,                0x030c, 0x030f},
    {class061Color,                 0x0400, 0x1fff}
};

METHOD Nv5ScaledImageFromMemoryMethods[] =
{
    {mthdNoOperation,               0x0100, 0x0103},
    {class063Notify,                0x0104, 0x0107},
    {class063SetContextDmaNotifies, 0x0180, 0x0183},
    {class063SetContextDmaImage,    0x0184, 0x0187},
    {class063SetContextPattern,     0x0188, 0x018b},
    {class063SetContextRop,         0x018c, 0x018f},
    {class063SetContextBeta1,       0x0190, 0x0193},
    {class063SetContextBeta4,       0x0194, 0x0197},
    {class063SetContextSurface,     0x0198, 0x019b},
    {class063SetColorConversion,    0x02fc, 0x02ff},
    {class063SetColorFormat,        0x0300, 0x0303},
    {class063SetOperation,          0x0304, 0x0307},
    {class063Clip,                  0x0308, 0x030f},
    {class063RectangleOut,          0x0310, 0x0317},
    {class063DeltaDuDx,             0x0318, 0x031b},
    {class063DeltaDvDy,             0x031c, 0x031f},
    {class063ImageIn,               0x0400, 0x040f}
};

METHOD Nv5IndexedImageFromCpuMethods[] =
{
    {mthdNoOperation,               0x0100, 0x0103},
    {class064Notify,                0x0104, 0x0107},
    {class064SetContextDmaNotifies, 0x0180, 0x0183},
    {class064SetContextDmaLut,      0x0184, 0x0187},
    {class064SetContextColorKey,    0x0188, 0x018b},
    {class064SetContextClip,        0x018c, 0x018f},
    {class064SetContextPattern,     0x0190, 0x0193},
    {class064SetContextRop,         0x0194, 0x0197},
    {class064SetContextBeta1,       0x0198, 0x019b},
    {class064SetContextBeta4,       0x019c, 0x019f},
    {class064SetContextSurface,     0x01a0, 0x01a3},
    {class064SetColorConversion,    0x03e0, 0x03e3},
    {class064SetOperation,          0x03e4, 0x03e7},
    {class064SetColorFormat,        0x03e8, 0x03eb},
    {class064IndexFormat,           0x03ec, 0x03ef},
    {class064LutOffset,             0x03f0, 0x03f3},
    {class064Point,                 0x03f4, 0x03f7},
    {class064SizeOut,               0x03f8, 0x03fb},
    {class064SizeIn,                0x03fc, 0x03ff},
    {class064Indices,               0x0400, 0x1fff}
};

METHOD Nv5ImageFromCpuMethods[] =
{
    {mthdNoOperation,               0x0100, 0x0103},
    {class065Notify,                0x0104, 0x0107},
    {class065SetContextDmaNotifies, 0x0180, 0x0183},
    {class065SetContextColorKey,    0x0184, 0x0187},
    {class065SetContextClip,        0x0188, 0x018b},
    {class065SetContextPattern,     0x018c, 0x018f},
    {class065SetContextRop,         0x0190, 0x0193},
    {class065SetContextBeta1,       0x0194, 0x0197},
    {class065SetContextBeta4,       0x0198, 0x019b},
    {class065SetContextSurface,     0x019c, 0x019f},
    {class065SetColorConversion,    0x02f8, 0x02fb},
    {class065SetOperation,          0x02fc, 0x02ff},
    {class065SetColorFormat,        0x0300, 0x0303},
    {class065Point,                 0x0304, 0x0307},
    {class065SizeOut,               0x0308, 0x030b},
    {class065SizeIn,                0x030c, 0x030f},
    {class065Color,                 0x0400, 0x1fff}
};
METHOD Nv5StretchedImageFromCpuMethods[] =
{
    {mthdNoOperation,               0x0100, 0x0103},
    {class066Notify,                0x0104, 0x0107},
    {class066SetContextDmaNotifies, 0x0180, 0x0183},
    {class066SetContextColorKey,    0x0184, 0x0187},
    {class066SetContextPattern,     0x0188, 0x018b},
    {class066SetContextRop,         0x018c, 0x018f},
    {class066SetContextBeta1,       0x0190, 0x0193},
    {class066SetContextBeta4,       0x0194, 0x0197},
    {class066SetContextSurface,     0x0198, 0x019b},
    {class066SetColorConversion,    0x02f8, 0x02fb},
    {class066SetOperation,          0x02fc, 0x02ff},
    {class066SetColorFormat,        0x0300, 0x0303},
    {class066SizeIn,                0x0304, 0x0307},
    {class066DeltaDxDu,             0x0308, 0x030b},
    {class066DeltaDyDv,             0x030c, 0x030f},
    {class066Clip,                  0x0310, 0x0317},
    {class066Point12d4,             0x0318, 0x031b},
    {class066Color,                 0x0400, 0x1fff}
};
METHOD Nv4ContextBetaMethods[] =
{
    {mthdNoOperation,               0x0100, 0x0103},
    {class072Notify,                0x0104, 0x0107},
    {class072SetContextDmaNotifies, 0x0180, 0x0183},
    {class072SetBetaFactor,         0x0300, 0x0303}
};
METHOD Nv4StretchedImageFromCpuMethods[] =
{
    {mthdNoOperation,               0x0100, 0x0103},
    {class076Notify,                0x0104, 0x0107},
    {class076SetContextDmaNotifies, 0x0180, 0x0183},
    {class076SetContextColorKey,    0x0184, 0x0187},
    {class076SetContextPattern,     0x0188, 0x018b},
    {class076SetContextRop,         0x018c, 0x018f},
    {class076SetContextBeta1,       0x0190, 0x0193},
    {class076SetContextBeta4,       0x0194, 0x0197},
    {class076SetContextSurface,     0x0198, 0x019b},
    {class076SetOperation,          0x02fc, 0x02ff},
    {class076SetColorFormat,        0x0300, 0x0303},
    {class076SizeIn,                0x0304, 0x0307},
    {class076DeltaDxDu,             0x0308, 0x030b},
    {class076DeltaDyDv,             0x030c, 0x030f},
    {class076Clip,                  0x0310, 0x0317},
    {class076Point12d4,             0x0318, 0x031b},
    {class076Color,                 0x0400, 0x1fff}
};
METHOD Nv4ScaledImageFromMemoryMethods[] =
{
    {mthdNoOperation,               0x0100, 0x0103},
    {class077Notify,                0x0104, 0x0107},
    {class077SetContextDmaNotifies, 0x0180, 0x0183},
    {class077SetContextDmaImage,    0x0184, 0x0187},
    {class077SetContextPattern,     0x0188, 0x018b},
    {class077SetContextRop,         0x018c, 0x018f},
    {class077SetContextBeta1,       0x0190, 0x0193},
    {class077SetContextBeta4,       0x0194, 0x0197},
    {class077SetContextSurface,     0x0198, 0x019b},
    {class077SetColorFormat,        0x0300, 0x0303},
    {class077SetOperation,          0x0304, 0x0307},
    {class077Clip,                  0x0308, 0x030f},
    {class077RectangleOut,          0x0310, 0x0317},
    {class077DeltaDuDx,             0x0318, 0x031b},
    {class077DeltaDvDy,             0x031c, 0x031f},
    {class077ImageIn,               0x0400, 0x040f}
};
METHOD Nv10ContextSurfaces2DMethods[] =
{
    {mthdNoOperation,               0x0100, 0x0103},
    {class062Notify,                0x0104, 0x0107},
    {class062SetContextDmaNotifies, 0x0180, 0x0183},
    {class062SetContextDmaSource,   0x0184, 0x0187},
    {class062SetContextDmaDestin,   0x0188, 0x018b},
    {class062SetColorFormat,        0x0300, 0x0303},
    {class062SetPitch,              0x0304, 0x0307},
    {class062SetOffsetSource,       0x0308, 0x030b},
    {class062SetOffsetDestin,       0x030c, 0x030f}
};
METHOD Nv10TextureFromCpuMethods[] =
{
    {mthdNoOperation,               0x0100, 0x0103},
    {class07BNotify,                0x0104, 0x0107},
    {class07BSetContextDmaNotifies, 0x0180, 0x0183},
    {class07BSetContextSurface,     0x0184, 0x0187},
    {class07BSetColorFormat,        0x0300, 0x0303},
    {class07BPoint,                 0x0304, 0x0307},
    {class07BSize,                  0x0308, 0x030b},
    {class07BClipHorizontal,        0x030c, 0x030f},
    {class07BClipVertical,          0x0310, 0x0313},
    {class07BColor,                 0x0400, 0x1fff}
};
METHOD Nv10DvdSubpictureMethods[] =
{
    {mthdNoOperation,               0x0100, 0x0103},
    {class088Notify,                0x0104, 0x0107},
    {class088SetContextDmaNotifies, 0x0180, 0x0183},
    {class088SetContextDmaOverlay,  0x0184, 0x0187},
    {class088SetContextDmaImageIn,  0x0188, 0x018b},
    {class088SetContextDmaImageOut, 0x018c, 0x018f},
    {class088ImageOutPoint,         0x0300, 0x0303},
    {class088ImageOutSize,          0x0304, 0x0307},
    {class088ImageOutFormat,        0x0308, 0x030b},
    {class088ImageOutOffset,        0x030c, 0x030f},
    {class088ImageInDeltaDuDx,      0x0310, 0x0313},
    {class088ImageInDeltaDvDy,      0x0314, 0x0317},
    {class088ImageInSize,           0x0318, 0x031b},
    {class088ImageInFormat,         0x031c, 0x031f},
    {class088ImageInOffset,         0x0320, 0x0323},
    {class088ImageInPoint,          0x0324, 0x0317},
    {class088OverlayDeltaDuDx,      0x0328, 0x032b},
    {class088OverlayDeltaDvDy,      0x032c, 0x032f},
    {class088OverlaySize,           0x0330, 0x0333},
    {class088OverlayFormat,         0x0334, 0x0337},
    {class088OverlayOffset,         0x0338, 0x033b},
    {class088OverlayPoint,          0x033c, 0x033f}
};
METHOD Nv10ScaledImageFromMemoryMethods[] =
{
    {mthdNoOperation,               0x0100, 0x0103},
    {class089Notify,                0x0104, 0x0107},
    {class089SetContextDmaNotifies, 0x0180, 0x0183},
    {class089SetContextDmaImage,    0x0184, 0x0187},
    {class089SetContextPattern,     0x0188, 0x018b},
    {class089SetContextRop,         0x018c, 0x018f},
    {class089SetContextBeta1,       0x0190, 0x0193},
    {class089SetContextBeta4,       0x0194, 0x0197},
    {class089SetContextSurface,     0x0198, 0x019b},
    {class089SetColorConversion,    0x02fc, 0x02ff},
    {class089SetColorFormat,        0x0300, 0x0303},
    {class089SetOperation,          0x0304, 0x0307},
    {class089Clip,                  0x0308, 0x030f},
    {class089RectangleOut,          0x0310, 0x0317},
    {class089DeltaDuDx,             0x0318, 0x031b},
    {class089DeltaDvDy,             0x031c, 0x031f},
    {class089ImageIn,               0x0400, 0x040f}
};
METHOD Nv10ImageFromCpuMethods[] =
{
    {mthdNoOperation,               0x0100, 0x0103},
    {class08ANotify,                0x0104, 0x0107},
    {class08ASetContextDmaNotifies, 0x0180, 0x0183},
    {class08ASetContextColorKey,    0x0184, 0x0187},
    {class08ASetContextClip,        0x0188, 0x018b},
    {class08ASetContextPattern,     0x018c, 0x018f},
    {class08ASetContextRop,         0x0190, 0x0193},
    {class08ASetContextBeta1,       0x0194, 0x0197},
    {class08ASetContextBeta4,       0x0198, 0x019b},
    {class08ASetContextSurface,     0x019c, 0x019f},
    {class08ASetColorConversion,    0x02f8, 0x02fb},
    {class08ASetOperation,          0x02fc, 0x02ff},
    {class08ASetColorFormat,        0x0300, 0x0303},
    {class08APoint,                 0x0304, 0x0307},
    {class08ASizeOut,               0x0308, 0x030b},
    {class08ASizeIn,                0x030c, 0x030f},
    {class08AColor,                 0x0400, 0x1fff}
};
METHOD Nv10ContextSurfaces3DMethods[] =
{
    {mthdNoOperation,               0x0100, 0x0103},
    {class093Notify,                0x0104, 0x0107},
    {class093SetContextDmaNotifies, 0x0180, 0x0183},
    {class093SetContextDmaColor,    0x0184, 0x0187},
    {class093SetContextDmaZeta,     0x0188, 0x018b},
    {class093SetClipHorizontal,     0x02f8, 0x02fb},
    {class093SetClipVertical,       0x02fc, 0x02ff},
    {class093SetFormat,             0x0300, 0x0303},
    {class093SetClipSize,           0x0304, 0x0307},
    {class093SetPitch,              0x0308, 0x030b},
    {class093SetOffsetColor,        0x030c, 0x030f},
    {class093SetOffsetZeta,         0x0310, 0x0313}
};
METHOD Nv10DX5TexturedTriangleMethods[] =
{
    {mthdNoOperation,               0x0100, 0x0103},
    {class094Notify,                0x0104, 0x0107},
    {class094SetContextDmaNotifies, 0x0180, 0x0183},
    {class094SetContextDmaA,        0x0184, 0x0187},
    {class094SetContextDmaB,        0x0188, 0x018b},
    {class094SetContextSurface,     0x018c, 0x018f},
    {class094ColorKey,              0x0300, 0x0303},
    {class094Offset,                0x0304, 0x0307},
    {class094Format,                0x0308, 0x030b},
    {class094Filter,                0x030c, 0x030f},
    {class094Blend,                 0x0310, 0x0313},
    {class094Control,               0x0314, 0x0317},
    {class094FogColor,              0x0318, 0x031b},
    {class094Tlvertex,              0x0400, 0x05ff},
    {class094DrawPrimitive,         0x0600, 0x06ff}
};
METHOD Nv10DX6MultiTextureTriangleMethods[] =
{
    {mthdNoOperation,               0x0100, 0x0103},
    {class095Notify,                0x0104, 0x0107},
    {class095SetContextDmaNotifies, 0x0180, 0x0183},
    {class095SetContextDmaA,        0x0184, 0x0187},
    {class095SetContextDmaB,        0x0188, 0x018b},
    {class095SetContextSurface,     0x018c, 0x018f},
    {class095Offset,                0x0308, 0x030f},
    {class095Format,                0x0310, 0x0317},
    {class095Filter,                0x0318, 0x031f},
    {class095Combine0Alpha,         0x0320, 0x0323},
    {class095Combine0Color,         0x0324, 0x0327},
    {class095Combine1Alpha,         0x032c, 0x032f},
    {class095Combine1Color,         0x0330, 0x0333},
    {class095CombineFactor,         0x0334, 0x0337},
    {class095Blend,                 0x0338, 0x033b},
    {class095Control,               0x033c, 0x0347},
    {class095FogColor,              0x0348, 0x034b},
    {class095Tlmtvertex,            0x0400, 0x053f},
    {class095DrawPrimitive,         0x0540, 0x05ff}
};
METHOD Nv15CelsiusPrimitiveMethods[] =
{
    {mthdNoOperation,               0x0100, 0x0103},
    {class096Notify,                0x0104, 0x0107},
    {class096SetWarningEnable,      0x0108, 0x010b},
    //{class096GetState,              0x010c, 0x010f},
    {class096WaitForIdle,           0x0110, 0x0113},
    {class096SyncSetRead,           0x0120, 0x0123},
    {class096SyncSetWrite,          0x0124, 0x0127},
    {class096SyncSetModulo,         0x0128, 0x012b},
    {class096SyncIncrementWrite,    0x012c, 0x012f},
    {class096SyncStall,             0x0130, 0x0133},
    {class096PMTrigger,             0x0140, 0x0143},
    {class096SetContextDmaNotifies, 0x0180, 0x0183},
    {class096SetContextDmaA,        0x0184, 0x0187},
    {class096SetContextDmaB,        0x0188, 0x018b},
    {class096SetContextDmaVertex,   0x018c, 0x018f},
    {class096SetContextDmaState,    0x0190, 0x0193},
    {class096SetContextDmaColor,    0x0194, 0x0197},
    {class096SetContextDmaZeta,     0x0198, 0x019b},
    //...
    // for now, catch anything in between
    {class096MethodCatcher,         0x0200, 0x17ff},
    //...
    {class096InlineArray,           0x1800, 0x1fff},
    {class096DebugInit,             0x2000, 0x2027}
};
METHOD Nv15ImageBlitMethods[] =
{
    {mthdNoOperation,               0x0100, 0x0103},
    {class09FNotify,                0x0104, 0x0107},
    {class09FSyncSetRead,           0x0120, 0x0123},
    {class09FSyncSetWrite,          0x0124, 0x0127},
    {class09FSyncSetModulo,         0x0128, 0x012b},
    {class09FSyncIncrementWrite,    0x012c, 0x012f},
    {class09FSyncStall,             0x0130, 0x0133},
    {class09FSyncIncrementReadRange,0x0134, 0x0137},
    {class09FSetContextDmaNotifies, 0x0180, 0x0183},
    {class09FSetContextColorKey,    0x0184, 0x0187},
    {class09FSetContextClip,        0x0188, 0x018b},
    {class09FSetContextPattern,     0x018c, 0x018f},
    {class09FSetContextRop,         0x0190, 0x0193},
    {class09FSetContextBeta1,       0x0194, 0x0197},
    {class09FSetContextBeta4,       0x0198, 0x019b},
    {class09FSetContextSurfaces2d,  0x019c, 0x019f},
    {class09FSetOperation,          0x02fc, 0x02ff},
    {class09FControlPointIn,        0x0300, 0x0303},
    {class09FControlPointOut,       0x0304, 0x0307},
    {class09FSize,                  0x0308, 0x030b}
};

//
// These classes are supported across all chips.
//
CLASSDESCRIPTOR Common_ClassDescriptors[] =
{
    MKCLASSDECL(TMR_ENGINE_TAG, 0, NV01_TIMER),

    MKCLASSDECL(VIDEO_ENGINE_TAG, NO_LOCK, NV_VIDEO_COLOR_KEY),
    MKCLASSDECL(VIDEO_ENGINE_TAG, NO_LOCK, NV_VIDEO_SCALER),
    MKCLASSDECL(VIDEO_ENGINE_TAG, NO_LOCK, NV_VIDEO_FROM_MEMORY),
    MKCLASSDECL(VIDEO_ENGINE_TAG, NO_LOCK, NV_VIDEO_COLORMAP),
    MKCLASSDECL(VIDEO_ENGINE_TAG, NO_LOCK, NV_VIDEO_SINK),
    MKCLASSDECL(VIDEO_ENGINE_TAG, NO_LOCK, NV_PATCHCORD_VIDEO),

    MKCLASSDECL(DMA_ENGINE_TAG, 0, NV01_CONTEXT_DMA_FROM_MEMORY),
    MKCLASSDECL(DMA_ENGINE_TAG, 0, NV01_CONTEXT_DMA_TO_MEMORY),
    MKCLASSDECL(DMA_ENGINE_TAG, 0, NV01_CONTEXT_DMA_IN_MEMORY),

    MKCLASSDECL(NO_ENGINE, 0, NV01_MEMORY_SYSTEM),
    MKCLASSDECL(NO_ENGINE, 0, NV01_MEMORY_LOCAL_PRIVILEGED),
    MKCLASSDECL(NO_ENGINE, 0, NV01_MEMORY_LOCAL_USER),

    // MAKE THIS LAST !!!!
    MKCLASSDECL(CLASS_PSEUDO_ENGINE_TAG, 0, NV_CLASS),
};

#define NUM_COMMONCLASSDESCRIPTORS          (sizeof (Common_ClassDescriptors) / sizeof (CLASSDESCRIPTOR))

CLASSMETHODS classMethodsTable[] =
{
    {NV1_CLASS,                             NULL, 0},

    {NV1_CONTEXT_DMA_FROM_MEMORY,           NULL, 0},

    {NV1_CONTEXT_DMA_TO_MEMORY,             NULL, 0},

    {NV1_TIMER,                             Nv1TimerMethods,
                                            sizeof(Nv1TimerMethods)/sizeof(METHOD)},

    {NV1_BETA_SOLID,                        Nv1BetaSolidMethods,
                                            sizeof(Nv1BetaSolidMethods)/sizeof(METHOD)},

    {NV1_IMAGE_SOLID,                       Nv1ImageSolidMethods,
                                            sizeof(Nv1ImageSolidMethods)/sizeof(METHOD)},

    {NV1_IMAGE_PATTERN,                     Nv1ImagePatternMethods,
                                            sizeof(Nv1ImagePatternMethods)/sizeof(METHOD)},

    {NV1_IMAGE_BLACK_RECTANGLE,             Nv1ImageRectangleBlackMethods,
                                            sizeof(Nv1ImageRectangleBlackMethods)/sizeof(METHOD)},

    {NV1_RENDER_SOLID_LIN,                  Nv1RenderSolidLinMethods,
                                            sizeof(Nv1RenderSolidLinMethods)/sizeof(METHOD)},

    {NV1_RENDER_SOLID_TRIANGLE,             Nv1RenderSolidTriangleMethods,
                                            sizeof(Nv1RenderSolidTriangleMethods)/sizeof(METHOD)},

    {NV1_RENDER_SOLID_RECTANGLE,            Nv1RenderSolidRectangleMethods,
                                            sizeof(Nv1RenderSolidRectangleMethods)/sizeof(METHOD)},

    {NV1_IMAGE_BLIT,                        Nv1ImageBlitMethods,
                                            sizeof(Nv1ImageBlitMethods)/sizeof(METHOD)},

    {NV1_IMAGE_FROM_CPU,                    Nv1ImageFromCpuMethods,
                                            sizeof(Nv1ImageFromCpuMethods)/sizeof(METHOD)},

    {NV1_NULL,                              NULL, 0},

    {NV3_STRETCHED_IMAGE_FROM_CPU,          Nv3StretchedImageFromCPUMethods,
                                            sizeof(Nv3StretchedImageFromCPUMethods)/sizeof(METHOD)},


    {NV3_SCALED_IMAGE_FROM_MEMORY,          Nv3ScaledImageFromMemoryMethods,
                                            sizeof(Nv3ScaledImageFromMemoryMethods)/sizeof(METHOD)},

                                            
    {NV3_MEMORY_TO_MEMORY_FORMAT,           Nv3MemoryToMemoryFormatMethods,
                                            sizeof(Nv3MemoryToMemoryFormatMethods)/sizeof(METHOD)},
                                            
    {NV1_CONTEXT_DMA_IN_MEMORY,             NULL, 0},

    {NV_CONTEXT_ERROR_TO_MEMORY,            NULL, 0},

    {NV1_CONTEXT_ERROR_TO_MEMORY,           NULL, 0},

    {NV1_MEMORY_PRIVILEGED,                 NULL, 0},

    {NV1_MEMORY_USER,                       NULL, 0},

    {NV3_CONTEXT_ROP,                       Nv3ContextRopMethods,
                                            sizeof(Nv3ContextRopMethods)/sizeof(METHOD)},

    {NV04_VIDEO_LUT_CURSOR_DAC,             Nv04VideoLUTCursorDACMethods,
                                            sizeof(Nv04VideoLUTCursorDACMethods)/sizeof(METHOD)},

    {NV04_VIDEO_OVERLAY,                    Nv04VideoOverlayMethods,
                                            sizeof(Nv04VideoOverlayMethods)/sizeof(METHOD)},

    {NV3_DX3_TEXTURED_TRIANGLE,             Nv3DX3TexturedTriangleMethods,
                                            sizeof(Nv3DX3TexturedTriangleMethods)/sizeof(METHOD)},

    {NV05_VIDEO_LUT_CURSOR_DAC,             Nv05VideoLUTCursorDACMethods,        
                                            sizeof(Nv05VideoLUTCursorDACMethods)/sizeof(METHOD)},

    {NV3_GDI_RECTANGLE_TEXT,                Nv3GDIRectangleTextMethods,   
                                            sizeof(Nv3GDIRectangleTextMethods)/sizeof(METHOD)},

    {NV03_EXTERNAL_VIDEO_DECODER,           Nv3ExternalVideoDecoderMethods,
                                            sizeof(Nv3ExternalVideoDecoderMethods)/sizeof(METHOD)},

    {NV3_EXTERNAL_VIDEO_DECOMPRESSOR,       Nv3ExternalVideoDecompressorMethods,   
                                            sizeof(Nv3ExternalVideoDecompressorMethods)/sizeof(METHOD)},

    {NV1_EXTERNAL_PARALLEL_BUS,             Nv1ExternalParallelBusMethods,
                                            sizeof(Nv1ExternalParallelBusMethods)/sizeof(METHOD)},

    {NV3_EXTERNAL_MONITOR_BUS,              Nv3ExternalMonitorBusMethods,
                                            sizeof(Nv3ExternalMonitorBusMethods)/sizeof(METHOD)},

    {NV3_EXTERNAL_SERIAL_BUS,               Nv4ExternalSerialBusMethods,
                                            sizeof(Nv4ExternalSerialBusMethods)/sizeof(METHOD)},

    {NV3_CONTEXT_SURFACE_0,                 Nv3ContextSurface0Methods,
                                            sizeof(Nv3ContextSurface0Methods)/sizeof(METHOD)},

    {NV3_CONTEXT_SURFACE_1,                 Nv3ContextSurface1Methods,   
                                            sizeof(Nv3ContextSurface1Methods)/sizeof(METHOD)},

    {NV3_CONTEXT_SURFACE_2,                 Nv3ContextSurface2Methods,   
                                            sizeof(Nv3ContextSurface2Methods)/sizeof(METHOD)},

    {NV3_CONTEXT_SURFACE_3,                 Nv3ContextSurface3Methods,   
                                            sizeof(Nv3ContextSurface3Methods)/sizeof(METHOD)},

    {NV4_DVD_SUBPICTURE,                    Nv4DvdSubpictureMethods,
                                            sizeof(Nv4DvdSubpictureMethods)/sizeof(METHOD)},

    {NV4_CONTEXT_SURFACES_2D,               Nv4ContextSurfaces2DMethods,
                                            sizeof(Nv4ContextSurfaces2DMethods)/sizeof(METHOD)},

    {NV4_CONTEXT_PATTERN,                   Nv4ContextPatternMethods,
                                            sizeof(Nv4ContextPatternMethods)/sizeof(METHOD)},

    {NV4_GDI_RECTANGLE_TEXT,                Nv4GDIRectangleTextMethods,
                                            sizeof(Nv4GDIRectangleTextMethods)/sizeof(METHOD)},

    {NV4_CONTEXT_SURFACE_SWIZZLED,          Nv4ContextSurfaceSwizzledMethods,
                                            sizeof(Nv4ContextSurfaceSwizzledMethods)/sizeof(METHOD)},

    {NV4_CONTEXT_SURFACES_ARGB_ZS,          Nv4ContextSurfacesArgbZsMethods,
                                            sizeof(Nv4ContextSurfacesArgbZsMethods)/sizeof(METHOD)},

    {NV4_DX5_TEXTURED_TRIANGLE,             Nv4DX5TexturedTriangleMethods,
                                            sizeof(Nv4DX5TexturedTriangleMethods)/sizeof(METHOD)},

    {NV4_DX6_MULTI_TEXTURE_TRIANGLE,        Nv4DX6MultiTextureTriangleMethods,
                                            sizeof(Nv4DX6MultiTextureTriangleMethods)/sizeof(METHOD)},

    {NV10_CELSIUS_PRIMITIVE,                Nv10CelsiusPrimitiveMethods,
                                            sizeof(Nv10CelsiusPrimitiveMethods)/sizeof(METHOD)},

    {NV4_CONTEXT_COLOR_KEY,                 Nv4ContextColorKeyMethods,
                                            sizeof(Nv4ContextColorKeyMethods)/sizeof(METHOD)},

    {NV4_RENDER_SOLID_LIN,                  Nv4RenderSolidLinMethods,
                                            sizeof(Nv4RenderSolidLinMethods)/sizeof(METHOD)},

    {NV4_RENDER_SOLID_TRIANGLE,             Nv4RenderSolidTriangleMethods,
                                            sizeof(Nv4RenderSolidTriangleMethods)/sizeof(METHOD)},

    {NV4_RENDER_SOLID_RECTANGLE,            Nv4RenderSolidRectangleMethods,
                                            sizeof(Nv4RenderSolidRectangleMethods)/sizeof(METHOD)},

    {NV4_IMAGE_BLIT,                        Nv4ImageBlitMethods,
                                            sizeof(Nv4ImageBlitMethods)/sizeof(METHOD)},

    {NV4_INDEXED_IMAGE_FROM_CPU,            Nv4IndexedImageFromCpuMethods,   
                                            sizeof(Nv4IndexedImageFromCpuMethods)/sizeof(METHOD)},

    {NV4_IMAGE_FROM_CPU,                    Nv4ImageFromCpuMethods,   
                                            sizeof(Nv4ImageFromCpuMethods)/sizeof(METHOD)},

    {NV05_SCALED_IMAGE_FROM_MEMORY,         Nv5ScaledImageFromMemoryMethods,   
                                            sizeof(Nv5ScaledImageFromMemoryMethods)/sizeof(METHOD)},

    {NV05_INDEXED_IMAGE_FROM_CPU,           Nv5IndexedImageFromCpuMethods,
                                            sizeof(Nv5IndexedImageFromCpuMethods)/sizeof(METHOD)},

    {NV05_IMAGE_FROM_CPU,                   Nv5ImageFromCpuMethods,
                                            sizeof(Nv5ImageFromCpuMethods)/sizeof(METHOD)},

    {NV05_STRETCHED_IMAGE_FROM_CPU,         Nv5StretchedImageFromCpuMethods,
                                            sizeof(Nv5StretchedImageFromCpuMethods)/sizeof(METHOD)},

    {NV10_VIDEO_LUT_CURSOR_DAC,             Nv10VideoLUTCursorDACMethods,
                                            sizeof(Nv10VideoLUTCursorDACMethods)/sizeof(METHOD)},

    {NV4_CONTEXT_BETA,                      Nv4ContextBetaMethods,
                                            sizeof(Nv4ContextBetaMethods)/sizeof(METHOD)},

    {NV4_STRETCHED_IMAGE_FROM_CPU,          Nv4StretchedImageFromCpuMethods,
                                            sizeof(Nv4StretchedImageFromCpuMethods)/sizeof(METHOD)},

    {NV4_SCALED_IMAGE_FROM_MEMORY,          Nv4ScaledImageFromMemoryMethods,
                                            sizeof(Nv4ScaledImageFromMemoryMethods)/sizeof(METHOD)},

    {NV_VIDEO_COLOR_KEY,                    NvVideoColorKeyMethods,
                                            sizeof(NvVideoColorKeyMethods)/sizeof(METHOD)},

    {NV_VIDEO_SCALER,                       NvVideoScalerMethods,
                                            sizeof(NvVideoScalerMethods)/sizeof(METHOD)},

    {NV_VIDEO_FROM_MEMORY,                  NvVideoFromMemoryMethods,
                                            sizeof(NvVideoFromMemoryMethods)/sizeof(METHOD)},

    {NV_VIDEO_COLORMAP,                     NvVideoColormapMethods,   
                                            sizeof(NvVideoColormapMethods)/sizeof(METHOD)},

    {NV_VIDEO_SINK,                         NvVideoSinkMethods,   
                                            sizeof(NvVideoSinkMethods)/sizeof(METHOD)},

    {NV_PATCHCORD_VIDEO,                    NvPatchcordMethods,
                                            sizeof(NvPatchcordMethods)/sizeof(METHOD)},

    {NV_CONTEXT_DMA_IN_MEMORY,              NvDmaInMemoryMethods,
                                            sizeof(NvDmaInMemoryMethods)/sizeof(METHOD)},
                                            
    {NV_CONTEXT_DMA_TO_MEMORY,              NvDmaToMemoryMethods,
                                            sizeof(NvDmaToMemoryMethods)/sizeof(METHOD)},
                                            
    {NV_CONTEXT_DMA_FROM_MEMORY,            NvDmaFromMemoryMethods,
                                            sizeof(NvDmaFromMemoryMethods)/sizeof(METHOD)},
                                            
    {NV10_CONTEXT_SURFACES_2D,              Nv10ContextSurfaces2DMethods,
                                            sizeof(Nv10ContextSurfaces2DMethods)/sizeof(METHOD)},

    {NV10_VIDEO_OVERLAY,                    Nv10VideoOverlayMethods,
                                            sizeof(Nv10VideoOverlayMethods)/sizeof(METHOD)},

    {NV10_TEXTURE_FROM_CPU,                 Nv10TextureFromCpuMethods,
                                            sizeof(Nv10TextureFromCpuMethods)/sizeof(METHOD)},

    {NV15_VIDEO_LUT_CURSOR_DAC,             Nv15VideoLUTCursorDACMethods,
                                            sizeof(Nv15VideoLUTCursorDACMethods)/sizeof(METHOD)},

    {NV10_DVD_SUBPICTURE,                   Nv10DvdSubpictureMethods,
                                            sizeof(Nv10DvdSubpictureMethods)/sizeof(METHOD)},
                                            
    {NV10_SCALED_IMAGE_FROM_MEMORY,         Nv10ScaledImageFromMemoryMethods,
                                            sizeof(Nv10ScaledImageFromMemoryMethods)/sizeof(METHOD)},
                                            
    {NV10_IMAGE_FROM_CPU,                   Nv10ImageFromCpuMethods,
                                            sizeof(Nv10ImageFromCpuMethods)/sizeof(METHOD)},

    {NV10_CONTEXT_SURFACES_3D,              Nv10ContextSurfaces3DMethods,   
                                            sizeof(Nv10ContextSurfaces3DMethods)/sizeof(METHOD)},

    {NV10_DX5_TEXTURED_TRIANGLE,            Nv10DX5TexturedTriangleMethods,
                                            sizeof(Nv10DX5TexturedTriangleMethods)/sizeof(METHOD)},

    {NV10_DX6_MULTI_TEXTURE_TRIANGLE,       Nv10DX6MultiTextureTriangleMethods,
                                            sizeof(Nv10DX6MultiTextureTriangleMethods)/sizeof(METHOD)},

    {NV15_CELSIUS_PRIMITIVE,                Nv15CelsiusPrimitiveMethods,
                                            sizeof(Nv15CelsiusPrimitiveMethods)/sizeof(METHOD)},

    {NV15_IMAGE_BLIT,                       Nv15ImageBlitMethods,
                                            sizeof(Nv15ImageBlitMethods)/sizeof(METHOD)},

    // NV11 and NV15 Celsius share the same method struct.
    {NV11_CELSIUS_PRIMITIVE,                Nv15CelsiusPrimitiveMethods,
                                            sizeof(Nv15CelsiusPrimitiveMethods)/sizeof(METHOD)},

    {NV20_VIDEO_LUT_CURSOR_DAC,             Nv20VideoLUTCursorDACMethods,
                                            sizeof(Nv20VideoLUTCursorDACMethods)/sizeof(METHOD)},

    //
    // MAKE THIS LAST!!!
    //                                     
    {NV_CLASS,                              NvClassMethods,
                                            sizeof(NvClassMethods)/sizeof(METHOD)},
};

#define CLASSMETHODSTABLE_SIZE              (sizeof(classMethodsTable) / sizeof(CLASSMETHODS))

//---------------------------------------------------------------------------
//
//  Class table access.
//
//---------------------------------------------------------------------------

RM_STATUS classGetClass
(
    PCLASSDB pClassDB,
	U032 classNum, 
	PCLASS *ppClass
)
{
    U032 i;

	for (i = 0; i < pClassDB->numClasses; i++)
    {
		if (pClassDB->pClasses[i].Type == classNum)
        {
			*ppClass = &pClassDB->pClasses[i];
			return RM_OK;
        }
    }

	return RM_ERROR;
}

//
// classGetInfo
//
// This routine sets up method handling for the specified class
// It also binds the class to a a particular engine.
//
RM_STATUS classGetInfo
(
    PENGINEDB pEngineDB, 
    PCLASSDB pClassDB,
    PCLASS pClass
)
{
    U032 classNum;
    PENGINEINFO pEngineInfo;
    PENGINEDESCRIPTOR pEngineDecl;
    U032 engine;
	U032 i;

    //
    // Get class number and engine tag.
    //
    classNum = CLASSDECL_FIELD(*pClass->ClassDecl, _CLASS);
    engine = CLASSDECL_FIELD(*pClass->ClassDecl, _TAG);

    //
    // Wire up engine data first.
    //
    if (engineGetInfo(pEngineDB, engine, &pEngineInfo, &pEngineDecl) != RM_OK)
    {
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM: classGetInfo: bad engine tag ", engine);
        return RM_ERROR;
    }

    pClass->EngineDecl = pEngineDecl;
    pClass->EngineInfo = pEngineInfo;

    //
    // Now wire up method routines.
    //
    for (i = 0; i < CLASSMETHODSTABLE_SIZE; i++)
    {
        if (classMethodsTable[i].Type == classNum)
        {
            pClass->Method = classMethodsTable[i].Method;
            pClass->MethodMax = classMethodsTable[i].MethodMax;
            break;
        }
    }

    //
    // If we don't have an entry in the resman method table for the
    // given class, then use the engine-specific default table
    // (usually just filled with a hardware method stub).
    //
    if (i == CLASSMETHODSTABLE_SIZE)
    {
        pClass->Method = pEngineInfo->Methods;
        pClass->MethodMax = pEngineInfo->MethodMax;
    }
    
	return RM_OK;
}

//
// classBuildDB
//
// This routine generates the class database for the
// specified device.
//
RM_STATUS classBuildDB
(
    PENGINEDB pEngineDB,
    PCLASSDB pClassDB
)
{
    PCLASS pClass;
    PCLASSDESCRIPTOR pClassDesc;
    U032 numClasses;
    U032 i, j;
    RM_STATUS status;

    //
    // Calculate number of classes supported by this device.
    //
    // For now, just add the size of the common class descriptor
    // table to the number of class descriptors in the pClassDB table (we
    // should probably allow for overlaps between the two).
    //
    j = 0;
    for (i = 0; i < pClassDB->numClassDescriptors; i++)
    {
        if (CLASSDECL_FIELD(pClassDB->pClassDescriptors[i], _OPCODE) == CLASSDECL_INSTR)
            j++;
            
    }

    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM: nvInfo class descriptors ",  j);

    numClasses = NUM_COMMONCLASSDESCRIPTORS + j;

    //
    // Allocate space for correct number of entries.
    //
    status = osAllocMem((VOID **)&pClass, sizeof (CLASS) * numClasses);
    if (status)
        return status;

    //
    // Load up the fixed (common) classes.
    //
    pClassDesc = &Common_ClassDescriptors[0];
    for (i = 0; i < NUM_COMMONCLASSDESCRIPTORS; i++)
    {
        pClass[i].Type = CLASSDECL_FIELD(pClassDesc[i], _CLASS);
        pClass[i].ClassDecl = &pClassDesc[i];
        status = classGetInfo(pEngineDB, pClassDB, &pClass[i]);
        if (status != RM_OK)
        {
            //
            // This shouldn't happen; treat as a fatal error.
            //
            osFreeMem((VOID *)pClass);
            return (RM_ERROR);
        }
    }

    //
    // Load up chip-dependent classes.
    //
    pClassDesc = &pClassDB->pClassDescriptors[0];
    for (i = NUM_COMMONCLASSDESCRIPTORS, j = 0; i < numClasses; i++)
    {
        //
        // Advance to next class descriptor header
        // (i.e. skip over any class extension descriptors).
        //
        while (CLASSDECL_FIELD(pClassDesc[j], _OPCODE) != CLASSDECL_INSTR)
            j++;
        pClass[i].Type = CLASSDECL_FIELD(pClassDesc[j], _CLASS);
        pClass[i].ClassDecl = &pClassDesc[j];
        if (CLASSDECL_FIELD(pClassDesc[++j], _OPCODE) == CLASSINSTMALLOC_INSTR)
            pClass[i].ClassInstMalloc = &pClassDesc[j];
        else
            pClass[i].ClassInstMalloc = NULL;
        
        status = classGetInfo(pEngineDB, pClassDB, &pClass[i]);
        if (status != RM_OK)
        {
            //
            // This shouldn't happen; treat as a fatal error.
            //
            osFreeMem((VOID *)pClass);
            return (RM_ERROR);
        }
    }

    pClassDB->pClasses = pClass;
    pClassDB->numClasses = numClasses;

    return (RM_OK);
}

RM_STATUS classDestroyDB
(
    PCLASSDB  pClassDB
)
{
    osFreeMem((VOID *)pClassDB->pClasses);
    return (RM_OK);
}

//---------------------------------------------------------------------------
//
//  Class object initial creation.
//
//---------------------------------------------------------------------------

RM_STATUS classCreateWellKnownObjects
(
    PHWINFO pDev,
    U032    ChID
)
{
    RM_STATUS    status;
    U032         i;
    U032         j;
    PCLASSDB pClassDB;
    PCLASSOBJECT ClassObject;
    PCLASSOBJECT *ClassObjects;
    PRENDERCOMMONOBJECT nullObject;
    PCLASS classTable;
#ifdef LEGACY_ARCH
    PDMAOBJECT   ErrNotifier;
#endif

    pClassDB = &pDev->classDB;

    //
    // Create and initialize all CLASS objects.  These are the master
    // class instances for this particular channel.  All user objects
    // created in this channel will be children of these master class
    // objects.
    //
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM: numClasses ", pClassDB->numClasses);

    //
    // Allocate ClassObjects[] table.
    //
    status = osAllocMem((VOID **)&ClassObjects, sizeof (PCLASSOBJECT) * pClassDB->numClasses);
    if (status)
        return (status);

    classTable = pClassDB->pClasses;
    for (i = 0; i < pClassDB->numClasses; i++)
    {
        status = osAllocMem((VOID **)&ClassObject, sizeof (CLASSOBJECT));
        if (status)
        {
            while (i)
                osFreeMem(ClassObjects[--i]);
            osFreeMem(ClassObjects);
            return status;
        }

        //
        // Init global state.
        //
        ClassObjects[i]                 = ClassObject;
        ClassObject->Base.Name          = classTable[i].Type;
        //ClassObject->Base.Class         = &(classTable[i]);
        ClassObject->Base.Class         = &(classTable[NUM_COMMONCLASSDESCRIPTORS-1]);
        ClassObject->Base.ThisClass     = &(classTable[i]);
        ClassObject->Base.ChID          = ChID;
        ClassObject->Base.NotifyXlate   = NULL;
        ClassObject->Base.NotifyAction  = 0;
        ClassObject->Base.NotifyTrigger = 0;
        ClassObject->Base.NotifyEvent   = NULL;

        //
        // Add object into FIFO object tree.
        //
        status = fifoAddObject(pDev, (POBJECT)ClassObject);
        if (status != RM_OK)
        {
            while (i)
                osFreeMem(ClassObjects[--i]);
            osFreeMem(ClassObjects);
            return (status);
        }
    }

    //
    // Create and initialize NV_ERROR_NOTIFIER.
    //
    // jsw: this is no longer necessary with the new architecture
    //      error notifiers are explicitly created.
#ifdef LEGACY_ARCH
    // Find the DMA_TO_MEM master class object first.
    //
    j = 0;
    while ((j < pClassDB->numClasses) && (ClassObjects[j]->Base.Name != NV_CONTEXT_DMA_TO_MEMORY))
        j++;
    if (j == pClassDB->numClasses)
    {
        // Kinda silly error, but just in case we can't find the class
        while (i)
            osFreeMem(ClassObjects[--i]);
        osFreeMem(ClassObjects);
        return (status);
    }
    //
    // Now create our instance
    //
    status = dmaCreate(pDev, ClassObjects[j], NV_CONTEXT_ERROR_TO_MEMORY, (POBJECT *)&ErrNotifier, NULL);
    if (status)
    {
        while (i)
            osFreeMem(ClassObjects[--i]);
        osFreeMem(ClassObjects);
        dmaDelete((VOID*)pDev, (POBJECT)ErrNotifier);
        return (status);
    }
    ErrNotifier->Base.Name          = NV_CONTEXT_ERROR_TO_MEMORY;
    ErrNotifier->Base.Class         = &(classTable[j]);
    ErrNotifier->Base.ThisClass     = &(classTable[j]);
    ErrNotifier->Base.ChID          = ChID;
    ErrNotifier->Base.NotifyXlate   = NULL;
    ErrNotifier->Base.NotifyAction  = 0;
    ErrNotifier->Base.NotifyTrigger = 0;
    ErrNotifier->Base.NotifyEvent   = NULL;
    if ((status = fifoAddObject(pDev, (POBJECT)ErrNotifier)) != RM_OK)
    {
        dmaDelete((VOID*)pDev, (POBJECT)ErrNotifier);
        //return (status);
    }
#endif

    //
    // Create and initialize NV1_NULL_OBJECT.
    //
    // Find the NV1_NULL master class object first.
    //
    j = 0;
    while ((j < pClassDB->numClasses) && (ClassObjects[j]->Base.Name != NV1_NULL))
        j++;

    if (j == pClassDB->numClasses)
    {
        // Kinda silly error, but just in case we can't find the class
        while (i)
            osFreeMem(ClassObjects[--i]);
        osFreeMem(ClassObjects);
        return (status);
    }       

    //
    // Now create our instance
    // 
    status = grCreateObj(pDev, ClassObjects[j], 0, (POBJECT *)&nullObject, NULL);
    if (status)
    {
        while (i)
            osFreeMem(ClassObjects[--i]);
        osFreeMem(ClassObjects);
        grDestroyObj(pDev, (POBJECT)nullObject);
        return (status);
    }

    if ((status = fifoAddObject(pDev, (POBJECT)nullObject)) != RM_OK)
    {
        grDestroyObj(pDev, (POBJECT)nullObject);
        //return (status);
    }

    osFreeMem(ClassObjects);

    return (RM_OK);
}

RM_STATUS classDeleteWellKnownObjects
(
    VOID*   pDevHandle,
    POBJECT Object
)
{
    return (osFreeMem(Object));
}

RM_STATUS classInitCommonObject
(
    POBJECT pObject,
    PCLASSOBJECT pClassObject,
    U032 Name
)
{
    PCOMMONOBJECT pCommonObject;

    //
    // Initialize base object data.
    //
    pCommonObject = (PCOMMONOBJECT)pObject;

    pCommonObject->Base.Name = Name;
    pCommonObject->Base.ChID = pClassObject->Base.ChID;
    pCommonObject->Base.Class = pClassObject->Base.ThisClass;
    pCommonObject->Base.ThisClass = pClassObject->Base.ThisClass;
    pCommonObject->Base.NotifyXlate   = NULL;
    pCommonObject->Base.NotifyAction  = 0;
    pCommonObject->Base.NotifyTrigger = 0;
    pCommonObject->Base.NotifyEvent   = NULL;

    //
    // Initialize common object data.
    //
    pCommonObject->Valid            = FALSE;
    pCommonObject->NotifyPending    = FALSE;
    pCommonObject->Instance         = 0;
    pCommonObject->NotifyObject     = NULL;
    pCommonObject->Dma0Object       = NULL;
    pCommonObject->Dma1Object       = NULL;

    return (RM_OK);
}

//---------------------------------------------------------------------------
//
//  NOP methods.
//
//---------------------------------------------------------------------------

RM_STATUS nopCreate
(
    VOID*        pDevHandle,
    PCLASSOBJECT ClassObject,
    U032         UserName,
    POBJECT     *UserObject,
    VOID*        pCreateParms
)
{
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_USERERRORS, "NVRM: Class not implemented yet: ", ClassObject->Base.Class->Type);
    return (RM_ERR_CREATE_BAD_CLASS);
}
RM_STATUS nopDelete
(
    PHWINFO pDev,
    POBJECT Object
)
{
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_USERERRORS, "NVRM: Class not implemented yet: ", Object->Class->Type);
    return (RM_ERR_DELETE_BAD_CLASS);
}
RM_STATUS mthdNoOperation
(
    PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_USERERRORS, "NVRM: Method NoOperation", Object->Class->Type);
    return (RM_OK);
}

//---------------------------------------------------------------------------
//
//  Create object.
//
//---------------------------------------------------------------------------

RM_STATUS classCreate
(
    VOID*        pDev,
    PCLASSOBJECT ClassObject,
    U032         UserName,
    POBJECT     *UserObject,
    VOID*        pCreateParms
)
{
    //
    // Class specific create procedure.
    //

    DBG_VAL_PTR(ClassObject);
    return ((*(ClassObject->Base.ThisClass->EngineInfo->CreateObject))(pDev, ClassObject, UserName, UserObject, pCreateParms)); //KJK YUCK!!
}

//---------------------------------------------------------------------------
//
//  Destroy object.
//
//---------------------------------------------------------------------------

RM_STATUS classDestroy
(
    VOID*   pDev,
    POBJECT Object
)
{
    //
    // Class specific delete procedure.
    //
    DBG_VAL_PTR(Object);
    return ((*(Object->Class->EngineInfo->DestroyObject))(pDev, Object));    //KJK YUCK!!
}

//---------------------------------------------------------------------------
//
//  Software methods.
//  Emulate the device access in software.
//
//---------------------------------------------------------------------------

RM_STATUS classSoftwareMethod
(
    PHWINFO pDev,
    POBJECT Object,
    U032    Offset,
    V032    Data
)
{
    RM_STATUS  status;
    U032       i;

    NVRM_TRACE3('SWMT',
                (Object->ChID << 24) | (Object->Subchannel << 16) | (*Object->Class->ClassDecl & 0xffff),
                Object->Name);
    NVRM_TRACE2(Offset, Data);

#if DEBUG
    //
    // Reject unaligned accesses.
    //
    if (Offset & 0x0003)
        return (RM_ERR_FIFO_BAD_ACCESS);
#endif // DEBUG
    if (Object != NULL)
    {
        //
        // Search for object method.
        //
        DBG_VAL_PTR(Object);
        for (i = 0; i < Object->Class->MethodMax; i++)
        {
            if ((Offset >= Object->Class->Method[i].Low)
            &&  (Offset <  Object->Class->Method[i].High))
            {

                status = Object->Class->Method[i].Proc(pDev,
                                                       Object,
                                                       &(Object->Class->Method[i]),
                                                       Offset,
                                                       Data);
                //
                // Send completion notification.
                //
                notifyMethodComplete(pDev, Object, Offset, Data, status);
                NVRM_TRACE2('swmt', status);
                return (status);
            }
        }
        //
        // Method not found, so report the error if notify pending.
        //
        NVRM_TRACE_ERROR('swmt', RM_ERR_FIFO_BAD_ACCESS);
        notifyMethodComplete(pDev, Object, Offset, Data, RM_ERR_FIFO_BAD_ACCESS);
    }
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_USERERRORS, "NVRM: invalid access ", RM_ERROR);
    if (Object)
    {
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_USERERRORS, "              object ", Object->Class->Type);
    }
    else
    {
        DBG_PRINT_STRING(DEBUGLEVEL_USERERRORS, "              object NULL");
    }
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_USERERRORS, "              offset ", Offset);
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_USERERRORS, "                data ", Data);
    return (RM_ERR_FIFO_BAD_ACCESS);
}

//
// SMP wrapper for classSoftwareMethod
//
RM_STATUS classDirectSoftwareMethod
(
    PHWINFO pDev,
    POBJECT Object,
    U032    Offset,
    V032    Data
)
{
    RM_STATUS status;

    osEnterCriticalCode(pDev);
    status = classSoftwareMethod(pDev, Object, Offset, Data);
    osExitCriticalCode(pDev);

    return status;
}
