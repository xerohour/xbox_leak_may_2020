/*
 * Copyright 1993-1998 NVIDIA, Corporation.  All rights reserved.
 *
 * THE INFORMATION CONTAINED HEREIN IS PROPRIETARY AND CONFIDENTIAL TO
 * NVIDIA, CORPORATION.  USE, REPRODUCTION OR DISCLOSURE TO ANY THIRD PARTY
 * IS SUBJECT TO WRITTEN PRE-APPROVAL BY NVIDIA, CORPORATION.
 */
/********************************* Direct 3D *******************************\
*                                                                           *
* Module: Global.cpp                                                        *
*                                                                           *
*****************************************************************************
*                                                                           *
* History: ?                                                                *
*                                                                           *
\***************************************************************************/
#include "nvprecomp.h"

#if (NVARCH >= 0x04)

#include "x86.h"
void ILCCompile_move (DWORD,DWORD,DWORD);

//#pragma data_seg("GLOBAL")
#pragma pack(push,1) // we control alignment

global_struc global={{0}};
DWORD dwDXRuntimeVersion; // version of DirectX runtime installed (inferred from GUIDs)

#pragma pack(pop)

void __stdcall init_globals (void) {

    memset (&g_adapterData, 0, sizeof(g_adapterData));
    memset (&global,        0, sizeof(global));

    // init aligned KNI state
    global.pKNI             = (KATMAI_STATE*)((((DWORD)&global.kni) + 15) & ~15);
    global.pKNI->fFogC1[0]  = global.pKNI->fFogC1[1]
                            = global.pKNI->fFogC1[2]
                            = global.pKNI->fFogC1[3]
                            = 2.0f;
    global.pKNI->fFogC2[0]  = global.pKNI->fFogC2[1]
                            = global.pKNI->fFogC2[2]
                            = global.pKNI->fFogC2[3]
                            = -1.44269504f * (float)0x800000;
    global.pKNI->fZero[0]   = global.pKNI->fZero[1]
                            = global.pKNI->fZero[2]
                            = global.pKNI->fZero[3]
                            = 0.0f;
    global.pKNI->fOne[0]    = global.pKNI->fOne[1]
                            = global.pKNI->fOne[2]
                            = global.pKNI->fOne[3]
                            = 1.0f;
    global.pKNI->f255[0]    = global.pKNI->f255[1]
                            = global.pKNI->f255[2]
                            = global.pKNI->f255[3]
                            = 255.0f;
    global.pKNI->fRHW[3]    = 1.0f;

    global.fCTC1      = 0.5f;

    global.dwILCData  = NULL;
    global.dwILCCount = 0;
    global.dwILCMax   = 0;

#if (NVARCH >= 0x010)
    global.celsius.qwBegin3 = ((unsigned __int64)NV056_SET_BEGIN_END3_OP_TRIANGLES << 32)
                            | (((sizeSetNv10CelsiusBeginEnd3MthdCnt << 2) << 16) | ((NV_DD_CELSIUS) << 13) | NV056_SET_BEGIN_END3);
    global.celsius.qwEnd3   = ((unsigned __int64)NV056_SET_BEGIN_END3_OP_END << 32)
                            | (((sizeSetNv10CelsiusBeginEnd3MthdCnt << 2) << 16) | ((NV_DD_CELSIUS) << 13) | NV056_SET_BEGIN_END3);
#endif  // NVARCH >= 0x010
#if (NVARCH >= 0x020)
    global.kelvin.qwBegin = ((unsigned __int64)NV097_SET_BEGIN_END_OP_TRIANGLES << 32)
                            | (((sizeSetNv20KelvinBeginEndMthdCnt << 2) << 16) | ((NV_DD_KELVIN) << 13) | NV097_SET_BEGIN_END);
    global.kelvin.qwEnd   = ((unsigned __int64)NV097_SET_BEGIN_END_OP_END << 32)
                            | (((sizeSetNv20KelvinBeginEndMthdCnt << 2) << 16) | ((NV_DD_KELVIN) << 13) | NV097_SET_BEGIN_END);
#endif  // NVARCH >= 0x010

    global.pKNI->dwTriDispatch[0] = ((((3) << 2) << 16) | ((NV_DD_CELSIUS) << 13) | (NVPUSHER_NOINC(NV056_ARRAY_ELEMENT32(0))));

#if (NVARCH >= 0x020)
    global.pKNI->dwKelvinTriDispatch[0] = ((((3) << 2) << 16) | ((NV_DD_KELVIN) << 13) | (NVPUSHER_NOINC(NV097_ARRAY_ELEMENT32)));
#endif
    // init delay loop until we get calibrate it properlty
    global.dwDelayCount = 400;

    global.b16BitCode = FALSE;

#ifndef WINNT
    // get and cache pointer to the device enumeration function
    // we used to do this on demand, but calls to LoadLibrary were sporadically hanging. lovely.
    HMODULE hModule = LoadLibrary("user32");
#ifdef UNICODE
    global.pfEnumDisplayDevices = (BOOL (WINAPI*)(LPCSTR, DWORD, PDISPLAY_DEVICE, DWORD)) GetProcAddress(hModule, "EnumDisplayDevicesW");
#else
    global.pfEnumDisplayDevices = (BOOL (WINAPI*)(LPCSTR, DWORD, PDISPLAY_DEVICE, DWORD)) GetProcAddress(hModule, "EnumDisplayDevicesA");
#endif // !UNICODE

    hModule = LoadLibrary("kernel32.dll");
    global.pfOpenVxDHandle = (HANDLE (WINAPI *)(HANDLE))GetProcAddress(hModule, "OpenVxDHandle");
#endif // !WINNT

#ifdef WINNT
    // force runtime version to be at least 7.00 for Win2K
    //check for existence of DX8:
    {
        HANDLE temp;
        CHAR name[9] = "D3D8.DLL";
        WCHAR uniname[9];
        ULONG outStrLen;

        //grr.... convert to Unicode.
        EngMultiByteToUnicodeN((LPWSTR)uniname, 9*sizeof(WCHAR), &outStrLen,
                       (char *)name, (9)*sizeof(CHAR));


        temp = EngLoadModule( uniname );
        if (temp)
        {
            EngFreeModule(temp);
            global.dwDXRuntimeVersion = 0x0800;
        }
        else
        {
            global.dwDXRuntimeVersion = 0x0700;
        }
    }
#else // WINNT
    // force runtime version to be at least 3.00 for Win9x
    global.dwDXRuntimeVersion = 0x0300;
#endif // !WINNT
}

//////////////////////////////////////////////////////////////////////////////
// registry (encrypted as a post-build step)
// keep this in sync with the defintion of reg_struc in global.h!

reg_struc reg =
{
    // signature
    {
        0x0badbeef,0x0badcafe,0xdeadbeef,0x420352ec,
        0x420352ec,0xdeadbeef,0x0badcafe,0x0badbeef,
    },
    // size
    sizeof(reg_struc) - 4 * (8 + 1),
    // to be encrypted registry key strings.
    // booleans
    D3D_REG_ANTIALIASENABLE_STRING,
    D3D_REG_ANTIALIASFORCEENABLE_STRING,
    D3D_REG_ANTIALIASDYNAMICENABLE_STRING,
    D3D_REG_ANTIALIASCONVOLUTIONFASTMODE_STRING,
    D3D_REG_CKCOMPATABILITYENABLE_STRING,
    D3D_REG_DIRECTMAPENABLE_STRING,
    D3D_REG_ENFORCESTRICTTRILINEAR_STRING,
    D3D_REG_FLUSHAFTERBLITENABLE_STRING,
    D3D_REG_FOGTABLEENABLE_STRING,
    D3D_REG_FORCEBLITWAITFLAGENABLE_STRING,
    D3D_REG_LIMITQUEUEDFBBLITSENABLE_STRING,
    D3D_REG_LOGOENABLE_STRING,
#ifndef TEXFORMAT_CRD
    D3D_REG_PALETTEENABLE_STRING,
#endif  // !TEXFORMAT_CRD
    D3D_REG_SQUASHW_STRING,
    D3D_REG_SSYNCENABLE_STRING,
#ifndef TEXFORMAT_CRD
    D3D_REG_TEXTURECOMPRESSIONENABLE_STRING,
#endif  // !TEXFORMAT_CRD
    D3D_REG_TEXTUREMANAGEMENTENABLE_STRING,
    D3D_REG_TILINGENABLE_STRING,
    D3D_REG_ZCULLENABLE_STRING,
    D3D_REG_USERMIPMAPENABLE_STRING,
    D3D_REG_VIDEOTEXTUREENABLE_STRING,
    D3D_REG_VS_HOS_EMULATION_STRING,
    D3D_REG_WBUFFERENABLE_STRING,
    D3D_REG_Z24ENABLE_STRING,
    D3D_REG_ZCOMPRESSENABLE_STRING,
    0xdeadbeef,
    // non-boolean enumerated types
    D3D_REG_ANTIALIASQUALITY_STRING,
    D3D_REG_ANTIALIASDYNAMICFPS_STRING,
    D3D_REG_CAPTURECONFIG_STRING,
    D3D_REG_CAPTUREENABLE_STRING,
    D3D_REG_CKREF_STRING,
    D3D_REG_ANISOTROPICLEVEL_STRING,
    D3D_REG_MIPMAPDITHERMODE_STRING,
#ifdef TEXFORMAT_CRD
    D3D_REG_SURFACEFORMATSDX7_STRING,
    D3D_REG_SURFACEFORMATSDX8_STRING,
#endif  // TEXFORMAT_CRD
    D3D_REG_TEXELALIGNMENT_STRING,
    D3D_REG_TEXTUREMANAGESTRATEGY_STRING,
    D3D_REG_VALIDATEZMETHOD_STRING,
    D3D_REG_VSYNCMODE_STRING,
    D3D_REG_WFORMAT16_STRING,
    D3D_REG_WFORMAT32_STRING,
    0xdeadbeef,
    // non-boolean non-enumerated types
    D3D_REG_AACOMPATIBILITYBITS_STRING,
    D3D_REG_AAREADCOMPATIBILITYFILE_STRING,
    D3D_REG_AGPTEXCUTOFF_STRING,
    D3D_REG_CAPTUREPLAYFILENUM_STRING,
    D3D_REG_CAPTURERECORDFILENUM_STRING,
    D3D_REG_D3DCONTEXTMAX_STRING,
    D3D_REG_D3DTEXTUREMAX_STRING,
    D3D_REG_DEBUGLEVEL_STRING,
    D3D_REG_LODBIAS_STRING,
    D3D_REG_MINVIDTEXSIZE_STRING,
    D3D_REG_PCITEXHEAPSIZE_STRING,
    D3D_REG_PERFSTRATEGYOR_STRING,
    D3D_REG_PERFSTRATEGYAND_STRING,
    D3D_REG_PRERENDERLIMIT_STRING,
    D3D_REG_PUSHBUFFERSIZEMAX_STRING,
    D3D_REG_WSCALE16_STRING,
    D3D_REG_WSCALE24_STRING,
    D3D_REG_ZBIAS_STRING,
    0xdeadbeef,
    // strings
    D3D_REG_CAPTUREPLAYFILENAME_STRING,
    D3D_REG_CAPTUREPLAYPATH_STRING,
    D3D_REG_CAPTURERECORDFILENAME_STRING,
    D3D_REG_CAPTURERECORDPATH_STRING,
    0xdeadbeef
};

void decryptRegistryKeys
(
    void
)
{
    if (reg.dwSize == ~0) // encrypted?
    {
        DWORD i;
        BYTE  *p = ((BYTE*)&reg.dwSize) + 4; // 1st entry
        for (i = 0; i < sizeof(reg_struc) - 4 * (8 + 1); i++,p++)
        {
            *p ^= 0x42 ^ (BYTE)i;
        }

        reg.dwSize = 0; // tag as decrypted
    }
#ifdef DEBUG
//    else
//    {
//        if (reg.dwSize)
//        {
            /*
             * not encrypted - warn
             */
//            DPF ("Registry not encrypted!!!!!");
///        }
//    }
#endif
}

#endif  // NVARCH >= 0x04

