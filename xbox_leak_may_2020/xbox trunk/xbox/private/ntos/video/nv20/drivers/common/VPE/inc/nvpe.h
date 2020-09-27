/*
==============================================================================

    NVIDIA Corporation, (C) Copyright 
    1996,1997,1998,1999. All rights reserved.

    nvpe.h

    NVIDIA's VPE driver header

    Jan 27, 1999

==============================================================================
*/

#ifndef _NVPE_H
#define _NVPE_H

// NVP defines
#define NVPAPI  __stdcall

//limit values for VP config
#define NV_MAX_FIELDWIDTH 2000	//since max now is 1920
#define NV_MAX_VBIWIDTH (0x1000)
#define NV_MAX_BUFFERLENGTH (0x80000)
#define NV_MAX_PIXELRATE ( 27000000)
#define NV_MAX_FIELDHEIGHT	1200	//since max now is 1080i

// provides access to the global data structures in ddmini.h
//extern GLOBALDATA *pDriverData;

// NVP function prototypes
DWORD NVPAPI CanCreateVideoPort32 (LPDDHAL_CANCREATEVPORTDATA lpInput);
DWORD NVPAPI CreateVideoPort32 (LPDDHAL_CREATEVPORTDATA lpInput);
DWORD NVPAPI FlipVideoPort32 (LPDDHAL_FLIPVPORTDATA lpInput);
DWORD NVPAPI GetVideoPortBandwidth32 (LPDDHAL_GETVPORTBANDWIDTHDATA lpInput);
DWORD NVPAPI GetVideoPortInputFormat32 (LPDDHAL_GETVPORTINPUTFORMATDATA lpInput);
DWORD NVPAPI GetVideoPortOutputFormat32 (LPDDHAL_GETVPORTOUTPUTFORMATDATA lpInput);
DWORD NVPAPI GetVideoPortField32 (LPDDHAL_GETVPORTFIELDDATA lpInput);
DWORD NVPAPI GetVideoPortLine32 (LPDDHAL_GETVPORTLINEDATA lpInput);
DWORD NVPAPI GetVideoPortConnectInfo (LPDDHAL_GETVPORTCONNECTDATA lpInput);
DWORD NVPAPI DestroyVideoPort32 (LPDDHAL_DESTROYVPORTDATA lpInput);
DWORD NVPAPI GetVideoPortFlipStatus32 (LPDDHAL_GETVPORTFLIPSTATUSDATA lpInput);
DWORD NVPAPI UpdateVideoPort32 (LPDDHAL_UPDATEVPORTDATA lpInput);
DWORD NVPAPI WaitForVideoPortSync32 (LPDDHAL_WAITFORVPORTSYNCDATA lpInput);
DWORD NVPAPI GetVideoSignalStatus32 ( LPDDHAL_GETVPORTSIGNALDATA lpInput);	

//
// NVIDIA VP caps
// 
const DDVIDEOPORTCAPS g_ddNVideoPortCaps =
{
    sizeof(DDVIDEOPORTCAPS),    // size of the DDVIDEOPORTCAPS structure
    DDVPD_WIDTH             |
    DDVPD_HEIGHT            |
    DDVPD_ID                |
    DDVPD_CAPS              |
    DDVPD_FX                |
    DDVPD_AUTOFLIP          |
    DDVPD_ALIGN             |
    0,
    4096,                       // VP max width
    4096,                       // max VBI data width
    640,                        // VP max height
    0,                          // VP ID (0 - (dwMaxVideoPorts-1))
    DDVPCAPS_AUTOFLIP       |   // suport autoflip buffers
    DDVPCAPS_INTERLACED     |
    DDVPCAPS_NONINTERLACED  |
    DDVPCAPS_READBACKFIELD  |
//  DDVPCAPS_READBACKLINE   |   // not supported
//  DDVPCAPS_SHAREABLE      |   // not supported 
    DDVPCAPS_SKIPEVENFIELDS |   // can skip even fields
    DDVPCAPS_SKIPODDFIELDS  |   // can skip odd fields
//  DDVPCAPS_SYNCMASTER     |   // not supported
    DDVPCAPS_VBISURFACE     |   // VBI support
	DDVPCAPS_OVERSAMPLEDVBI |
	DDVPCAPS_VBIANDVIDEOINDEPENDENT |	
//  DDVPCAPS_COLORCONTROL   |   // not supported
    0,
    DDVPFX_CROPTOPDATA      |   // 
//  DDVPFX_CROPX            |
//  DDVPFX_CROPY            |
    DDVPFX_INTERLEAVE       |
//  DDVPFX_MIRRORLEFTRIGHT  |   // not supported
//  DDVPFX_MIRRORUPDOWN     |   // not supported
//  DDVPFX_PRESHRINKX       |   // not supported
    DDVPFX_PRESHRINKY       |   // not supported
//  DDVPFX_PRESHRINKXS      |   // not supported
//  DDVPFX_PRESHRINKYS      |   // not supported
    DDVPFX_PRESHRINKXB      |   // down to 1/2, 1/4, 1/8 or 1/16
//  DDVPFX_PRESHRINKYB      |   // 
//  DDVPFX_PRESTRETCHX      |   // LPB never stretches
//  DDVPFX_PRESTRETCHY      |   // LPB never stretches
//  DDVPFX_PRESTRETCHXN     |   // LPB never stretches
//  DDVPFX_PRESTRETCHYN     |   // LPB never stretches
    DDVPFX_VBICONVERT       |
    DDVPFX_VBINOSCALE       |
    DDVPFX_VBINOINTERLEAVE  |
    DDVPFX_IGNOREVBIXCROP   |
    0,
    5,                          // Number of autoflippable surfaces
    8,                          // Byte restriction of placement within surface    ////// BUGBUG must change to NV_BYTE_ALIGNMENT
    1,                          // Byte restriction of width after prescaling
    1,                          // Byte restriction of left cropping
    1,                          // Byte restriction of cropping width
    24,                         // shrink in steps of original width
    0x400,                      // shrink in steps of original height
    5,							// Max number of VBI autoflippable surfaces allowed
    3,							// Optimal number of autoflippable surfaces for hardware
    2,                          // Number of taps the prescaler uses in the X direction (0 - no prescale, 1 - replication, etc.)
    1                           // Number of taps the prescaler uses in the Y direction (0 - no prescale, 1 - replication, etc.)

};

//
// NVIDIA VP Callbacks
//
const DDHAL_DDVIDEOPORTCALLBACKS LPBVideoPortCallbacks =
{
    sizeof(DDHAL_DDVIDEOPORTCALLBACKS),
    DDHAL_VPORT32_CANCREATEVIDEOPORT |
    DDHAL_VPORT32_CREATEVIDEOPORT |
    DDHAL_VPORT32_FLIP |
    DDHAL_VPORT32_GETBANDWIDTH |
    DDHAL_VPORT32_GETINPUTFORMATS |
    DDHAL_VPORT32_GETOUTPUTFORMATS |
    DDHAL_VPORT32_GETFIELD |
    DDHAL_VPORT32_GETLINE |
    DDHAL_VPORT32_GETCONNECT |
    DDHAL_VPORT32_DESTROY |
    DDHAL_VPORT32_GETFLIPSTATUS |
    DDHAL_VPORT32_UPDATE |
    DDHAL_VPORT32_WAITFORSYNC |
    DDHAL_VPORT32_GETSIGNALSTATUS |
    //DDHAL_VPORT32_COLORCONTROL |
    0,
    &CanCreateVideoPort32,
    &CreateVideoPort32,
    &FlipVideoPort32,
    &GetVideoPortBandwidth32,
    &GetVideoPortInputFormat32,
    &GetVideoPortOutputFormat32,
    NULL,
    &GetVideoPortField32,
    &GetVideoPortLine32,
    &GetVideoPortConnectInfo,
    &DestroyVideoPort32,
    &GetVideoPortFlipStatus32,
    &UpdateVideoPort32,
    &WaitForVideoPortSync32,
    &GetVideoSignalStatus32,
    NULL
};

//
// Kernel callbacks table exported through the GetDriverInfo interface
//
const DDHAL_DDKERNELCALLBACKS KernelCallbacks =
{
    sizeof(DDHAL_DDKERNELCALLBACKS),
    // DDHAL_KERNEL_SYNCSURFACEDATA | 
    // DDHAL_KERNEL_SYNCVIDEOPORTDATA | 
    0,
    NULL,	// SafeSyncSurfaceData
    NULL	// SafeSyncVideoPortData
};

//
// NVIDIA VP connection caps
// 
const DWORD g_dwNVPConnectionCaps = { 
	//DDVPCONNECT_DOUBLECLOCK			|
	//DDVPCONNECT_VACT					|
	DDVPCONNECT_INVERTPOLARITY		|
	/* we discard at least until the V bit goes low, and potentially more depending
	  on the setting of YCROP  as well as the current VBI situation  */
	//DDVPCONNECT_DISCARDSVREFDATA	|
	DDVPCONNECT_HALFLINE			|
	/*  these next are "client" only 
	//DDVPCONNECT_INTERLACED			|
	//DDVPCONNECT_SHAREEVEN			|
	//DDVPCONNECT_SHAREODD				|
	*/
	0 
};

// video format
DDPIXELFORMAT g_aNVPVideoFormat[] = {
	{sizeof(DDPIXELFORMAT),DDPF_FOURCC, mmioFOURCC('U','Y','V','Y'), 16, (DWORD)-1, (DWORD)-1, (DWORD)-1},
};

// VBI format
DDPIXELFORMAT g_aNVPVBIFormat[] = {
	{sizeof(DDPIXELFORMAT),DDPF_FOURCC, mmioFOURCC('R','A','W','8'), 8, (DWORD)-1, (DWORD)-1, (DWORD)-1},
};

// driver global data
//extern 		GLOBALDATA*			pDriverData;

#endif
