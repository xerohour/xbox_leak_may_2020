/*
 * (C) Copyright NVIDIA Corporation Inc., 1996,1997,1998. All rights reserved.
 */
/********************************* Direct 3D *******************************\
*                                                                           *
* Module: nvMini.cpp                                                        *
* Hardware specific driver setup routines.                                  *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Charles Inman               01/31/98 - created.                     *
*                                                                           *
\***************************************************************************/
#include "nvprecomp.h"

#if (NVARCH >= 0x04)

#if IS_WIN9X
#include <stdio.h> // For sprintf
#endif

#include "nvReg.h"

//---------------------------------------------------------------------------

/*
 * Developer Identification String
 * Before sending a pre-release driver to a developer, this data can be modified in the binary to identify
 * the developer it is being sent to.  This will give us an identification mechanism if the driver mysteriously
 * finds it's way on the the net.
 */
BYTE bDeveloperID[32 + 32] =
{
    0x0F, 0xAD, 0xCA, 0xFE, 0xDE, 0xAF, 0xFE, 0xED,
    0xDE, 0xAF, 0xBA, 0xBE, 0xDE, 0xAD, 0xF0, 0x0D,
    0x0B, 0xAD, 0xCA, 0xFE, 0x0D, 0xEA, 0xDF, 0xED,
    0xDE, 0xAD, 0xBA, 0xBE, 0x0B, 0xAD, 0xF0, 0x0D,

    0x61, 0x34, 0x42, 0x34, 0xf6, 0xf4, 0x82, 0xe1,
    0xb7, 0xec, 0x5c, 0x23, 0x82, 0x00, 0x60, 0x84,
    0x67, 0x8c, 0x45, 0x2c, 0x47, 0x31, 0x85, 0x32,
    0x30, 0x05, 0x06, 0x20, 0x93, 0x10, 0x40, 0x92,
};

//---------------------------------------------------------------------------

//
// Query a value in the D3D driver registry key
//
#ifdef WINNT
BOOL  NvD3DRegQueryDwordValueNT(PDEV *pdev, LPCTSTR valuename, LPDWORD value)
{

   CHAR  newvaluename[256];      // This should be more then enough space
   WCHAR tmpStrBuf[256];
   ULONG  inpStrLen, outStrLen;
   DWORD retStatus;
   DWORD keyValSize;

   // Make sure that we have something with meaning
   if (!valuename)
       return (FALSE);

   // We need to prepend this for NT
   strcpy(newvaluename,"D3D_");

   // Tac on the value name given to us
   strcat(newvaluename,valuename);

   // Get the combined length for conversion to unicode
   inpStrLen = strlen(newvaluename);

   // Make sure it fits in our non-dynamically allocated buffer
   if ((inpStrLen+1) > 256)
       return (FALSE);

   // The miniport only likes unicode
   EngMultiByteToUnicodeN((LPWSTR)tmpStrBuf, 256*sizeof(WCHAR), &outStrLen,
                          (char *)newvaluename, (inpStrLen+1)*sizeof(CHAR));

   // Ask the miniport for help on this one
   retStatus = EngDeviceIoControl(pdev->hDriver,
                               IOCTL_VIDEO_QUERY_REGISTRY_VAL,
                               (LPVOID)tmpStrBuf,
                               outStrLen,
                               (LPVOID) value,
                               sizeof(DWORD),
                               &keyValSize);


   // Make a final decision on this whole thing
   if(retStatus == NO_ERROR) {
      return (TRUE);
   }
   else {
      return (FALSE);
   }

}
#endif

//---------------------------------------------------------------------------

BOOL nvReadRegistryDWORD
(
#ifdef WINNT
    PDEV   *pDev,
#else
    HKEY    hKey,
#endif
    LPCTSTR lpValueName,
    LPDWORD lpData
)
{
#ifdef WINNT
    return NvD3DRegQueryDwordValueNT(pDev,lpValueName,lpData);
#else
    DWORD dwType  = REG_DWORD;
    DWORD dwCount = sizeof(DWORD);
    return (RegQueryValueEx (hKey,lpValueName,NULL,&dwType,(BYTE*)lpData,&dwCount) == ERROR_SUCCESS);
#endif
}

//---------------------------------------------------------------------------

BOOL nvReadRegistryString
(
#ifdef WINNT
    PDEV   *pDev,
#else
    HKEY    hKey,
#endif
    LPCTSTR lpValueName,
    LPBYTE  lpData
)
{
#ifdef WINNT
    return (FALSE);
#else
    DWORD dwType  = REG_SZ;
    DWORD dwCount = D3D_REG_STRING_LENGTH;
    return (RegQueryValueEx (hKey,lpValueName,NULL,&dwType,lpData,&dwCount) == ERROR_SUCCESS);
#endif
}

//---------------------------------------------------------------------------

// open the D3D registry key for the device associated with this
// instance of DirectX

HKEY OpenLocalD3DRegKey (void)
{
#ifdef WINNT    // We do nothing on NT
    return NULL;
#else           // !WINNT
    HKEY        hKey, hPrimaryKey;
    char        lpLocalRegPath[128];
    char        lpD3DSubKey[256];
    static BOOL bPrintedKeyBefore = FALSE;

    // get the registry path for this particular device
    // (of the many devices possible in a multi-mon configuration)
    MyExtEscape (pDXShare->dwHeadNumber, NV_ESC_GET_LOCAL_REGISTRY_PATH, 0, NULL, 128, lpLocalRegPath);

    // the first DWORD of the value returned is the primary key (e.g. HKEY_LOCAL_MACHINE)
    hPrimaryKey = (HKEY)(*(DWORD *)lpLocalRegPath);

    // the NV subkey for this device starts after the first four bytes.
    // concatenate this with D3D's subtree's name
    sprintf (lpD3DSubKey, "%s\\%s", lpLocalRegPath+4, NV4_REG_DIRECT3D_SUBKEY);

    //
    // for the mere mortals among us, print this damn key so that we know where to look
    //
    if (!bPrintedKeyBefore)
    {
        DPF ("---------------- NVDD32.DLL ----------------------");
        DPF ("REG: %s", lpD3DSubKey);
        bPrintedKeyBefore = TRUE;
    }

    //
    // open it
    //
    if (RegOpenKeyEx(hPrimaryKey, lpD3DSubKey, 0, KEY_QUERY_VALUE, &hKey) != ERROR_SUCCESS) {
        hKey = NULL;
    }

    return (hKey);
#endif  // !WINNT
}

//---------------------------------------------------------------------------

/*
 * Read the current driver settings from the registry.
 */
#pragma warning(disable: 4296)

void D3DReadRegistry
(
    GLOBALDATA *pDriverData
)
{
    dbgTracePush("D3DReadRegistry");

    /*
     * Set the defaults first.
     */
    // booleans
    getDC()->nvD3DRegistryData.regD3DEnableBits1       =     D3D_REG_DEFAULT_ALTERNATEZENABLE
                                                           | D3D_REG_DEFAULT_ANTIALIASENABLE
                                                           | D3D_REG_DEFAULT_ANTIALIASFORCEENABLE
                                                           | D3D_REG_DEFAULT_ANTIALIASCONVOLUTIONFASTMODE
                                                           | D3D_REG_DEFAULT_CKCOMPATABILITYENABLE
                                                           | D3D_REG_DEFAULT_CONTROLTRAFFICENABLE
                                                           | D3D_REG_DEFAULT_DIRECTMAPENABLE
                                                           | D3D_REG_DEFAULT_ENFORCESTRICTTRILINEAR
                                                           | D3D_REG_DEFAULT_FLUSHAFTERBLITENABLE
                                                           | D3D_REG_DEFAULT_FOGTABLEENABLE
                                                           | D3D_REG_DEFAULT_FORCEBLITWAITFLAGENABLE
                                                           | D3D_REG_DEFAULT_LIMITQUEUEDFBBLITSENABLE
                                                           | D3D_REG_DEFAULT_LOGOENABLE
#ifndef TEXFORMAT_CRD
                                                           | D3D_REG_DEFAULT_PALETTEENABLE
#endif  // !TEXFORMAT_CRD
                                                           | D3D_REG_DEFAULT_SQUASHW
                                                           | D3D_REG_DEFAULT_SSYNCENABLE
#ifndef TEXFORMAT_CRD
                                                           | D3D_REG_DEFAULT_TEXTURECOMPRESSIONENABLE
#endif  // !TEXFORMAT_CRD
                                                           | D3D_REG_DEFAULT_TEXTUREMANAGEMENTENABLE
                                                           | D3D_REG_DEFAULT_TILINGENABLE
                                                           | D3D_REG_DEFAULT_ZCULLENABLE

                                                           | D3D_REG_DEFAULT_USERMIPMAPENABLE
                                                           | D3D_REG_DEFAULT_VIDEOTEXTUREENABLE
                                                           | D3D_REG_VS_HOS_EMULATION_DISABLE
                                                           | D3D_REG_DEFAULT_WBUFFERENABLE
                                                           | D3D_REG_DEFAULT_Z24ENABLE
                                                           | D3D_REG_DEFAULT_ZCOMPRESSENABLE;

    // non-boolean enumerated values
    getDC()->nvD3DRegistryData.regCaptureConfig           = D3D_REG_DEFAULT_CAPTURCONFIG;
    getDC()->nvD3DRegistryData.regCaptureEnable           = D3D_REG_DEFAULT_CAPTURENABLE;
    getDC()->nvD3DRegistryData.regMipMapDitherMode        = D3D_REG_DEFAULT_MIPMAPDITHERMODE;
#ifdef TEXFORMAT_CRD
    getDC()->nvD3DRegistryData.regSurfaceFormatsDX7       = D3D_REG_DEFAULT_SURFACEFORMATSDX7;
    getDC()->nvD3DRegistryData.regSurfaceFormatsDX8       = D3D_REG_DEFAULT_SURFACEFORMATSDX8;
#endif  // TEXFORMAT_CRD
    getDC()->nvD3DRegistryData.regTexelAlignment          = D3D_REG_DEFAULT_TEXELALIGNMENT;
    getDC()->nvD3DRegistryData.regTextureManageStrategy   = D3D_REG_DEFAULT_TEXTUREMANAGESTRATEGY;
    getDC()->nvD3DRegistryData.regValidateZMethod         = D3D_REG_DEFAULT_VALIDATEZMETHOD;
    getDC()->nvD3DRegistryData.regVSyncMode               = D3D_REG_DEFAULT_VSYNCMODE;
    getDC()->nvD3DRegistryData.regW16Format               = D3D_REG_DEFAULT_W16FORMAT;
    getDC()->nvD3DRegistryData.regW32Format               = D3D_REG_DEFAULT_W32FORMAT;

    // non-boolean non-enumerated values
    getDC()->nvD3DRegistryData.regAACompatibilityBits     = D3D_REG_DEFAULT_AACOMPATIBILITYBITS;
    getDC()->nvD3DRegistryData.regAAReadCompatibilityFile = D3D_REG_DEFAULT_AAREADCOMPATIBILITYFILE;
    getDC()->nvD3DRegistryData.regAntiAliasQuality        = D3D_REG_DEFAULT_ANTIALIASQUALITY;
    getDC()->nvD3DRegistryData.regAntiAliasDynamicMSpF    = D3D_REG_DEFAULT_ANTIALIASDYNAMICFPS;
    getDC()->nvD3DRegistryData.regAGPTexCutOff            = D3D_REG_DEFAULT_AGPTEXCUTOFF;
    getDC()->nvD3DRegistryData.regCapturePlayFilenum      = D3D_REG_DEFAULT_CAPTUREPLAYFILENUM;
    getDC()->nvD3DRegistryData.regCaptureRecordFilenum    = D3D_REG_DEFAULT_CAPTURERECORDFILENUM;
    getDC()->nvD3DRegistryData.regColorkeyRef             = D3D_REG_DEFAULT_CKREF;
    getDC()->nvD3DRegistryData.regD3DContextMax           = D3D_REG_DEFAULT_D3DCONTEXTMAX;
    getDC()->nvD3DRegistryData.regD3DTextureMax           = D3D_REG_DEFAULT_D3DTEXTUREMAX;
    getDC()->nvD3DRegistryData.regDebugLevel              = D3D_REG_DEFAULT_DEBUGLEVEL;
    getDC()->nvD3DRegistryData.regAnisotropicLevel        = D3D_REG_DEFAULT_ANISOTROPICLEVEL;
    getDC()->nvD3DRegistryData.regLODBiasAdjust           = D3D_REG_DEFAULT_LODBIASADJUST;
    getDC()->nvD3DRegistryData.regMinVideoTextureSize     = D3D_REG_DEFAULT_MINVIDEOTEXSIZE;
    getDC()->nvD3DRegistryData.regPCITexHeapSize          = D3D_REG_DEFAULT_PCITEXHEAPSIZE;
    getDC()->nvD3DRegistryData.regPreRenderLimit          = D3D_REG_DEFAULT_PRERENDERLIMIT;
    getDC()->nvD3DRegistryData.regPushBufferSizeMax       = D3D_REG_DEFAULT_PUSHBUFFERSIZEMAX;
    getDC()->nvD3DRegistryData.regfWScale16               = (float)((DWORD)D3D_REG_DEFAULT_WSCALE16);
    getDC()->nvD3DRegistryData.regfWScale24               = (float)((DWORD)D3D_REG_DEFAULT_WSCALE24);
    getDC()->nvD3DRegistryData.regfZBias                  = D3D_REG_DEFAULT_ZBIAS;

    // strings
    nvStrNCpy (getDC()->nvD3DRegistryData.regszCapturePlayFilename,   D3D_REG_DEFAULT_CAPTUREPLAYFILENAME,   D3D_REG_STRING_LENGTH);
    nvStrNCpy (getDC()->nvD3DRegistryData.regszCapturePlayPath,       D3D_REG_DEFAULT_CAPTUREPLAYPATH,       D3D_REG_STRING_LENGTH);
    nvStrNCpy (getDC()->nvD3DRegistryData.regszCaptureRecordFilename, D3D_REG_DEFAULT_CAPTURERECORDFILENAME, D3D_REG_STRING_LENGTH);
    nvStrNCpy (getDC()->nvD3DRegistryData.regszCaptureRecordPath,     D3D_REG_DEFAULT_CAPTURERECORDPATH,     D3D_REG_STRING_LENGTH);

    // DirectX VA default flags
    pDriverData->dwDXVAFlags = DXVA_ENABLED_IN_REGISTRY;

    // Next try reading the overrides from the registry.
    DWORD dwValue;
    char  szString[D3D_REG_STRING_LENGTH];

#ifdef WINNT
    PDEV *hKey = ppdev;
#else
    HKEY  hKey = OpenLocalD3DRegKey();
    if (hKey)
#endif
    {
        decryptRegistryKeys();

        // booleans
        if (nvReadRegistryDWORD(hKey,reg.szAntiAliasEnableString,&dwValue))
        {
            getDC()->nvD3DRegistryData.regD3DEnableBits1 &= ~D3D_REG_ANTIALIASENABLE_MASK;
            getDC()->nvD3DRegistryData.regD3DEnableBits1 |= (dwValue) ? D3D_REG_ANTIALIASENABLE_ENABLE : D3D_REG_ANTIALIASENABLE_DISABLE;
        }

        if (nvReadRegistryDWORD(hKey,reg.szAntiAliasForceEnableString,&dwValue))
        {
            getDC()->nvD3DRegistryData.regD3DEnableBits1 &= ~D3D_REG_ANTIALIASFORCEENABLE_MASK;
            getDC()->nvD3DRegistryData.regD3DEnableBits1 |= (dwValue) ? D3D_REG_ANTIALIASFORCEENABLE_ENABLE : D3D_REG_ANTIALIASFORCEENABLE_DISABLE;
        }

        if (nvReadRegistryDWORD(hKey,reg.szAntiAliasDynamicEnableString,&dwValue))
        {
            getDC()->nvD3DRegistryData.regD3DEnableBits1 &= ~D3D_REG_ANTIALIASDYNAMICENABLE_MASK;
            getDC()->nvD3DRegistryData.regD3DEnableBits1 |= (dwValue) ? D3D_REG_ANTIALIASDYNAMICENABLE_ENABLE : D3D_REG_ANTIALIASDYNAMICENABLE_DISABLE;
        }

        if (nvReadRegistryDWORD(hKey,reg.szAntiAliasConvolutionFastModeString,&dwValue))
        {
            getDC()->nvD3DRegistryData.regD3DEnableBits1 &= ~D3D_REG_ANTIALIASCONVOLUTIONFASTMODE_MASK;
            getDC()->nvD3DRegistryData.regD3DEnableBits1 |= (dwValue) ? D3D_REG_ANTIALIASCONVOLUTIONFASTMODE_ENABLE : D3D_REG_ANTIALIASCONVOLUTIONFASTMODE_DISABLE;
        }

        if (nvReadRegistryDWORD(hKey,reg.szAntiAliasQualityString,&dwValue))
        {
            if (dwValue < D3D_REG_ANTIALIASQUALITY_MIN) dwValue = D3D_REG_ANTIALIASQUALITY_MIN;
            if (dwValue > D3D_REG_ANTIALIASQUALITY_MAX) dwValue = D3D_REG_ANTIALIASQUALITY_MAX;
            getDC()->nvD3DRegistryData.regAntiAliasQuality = dwValue;
        }

        if (nvReadRegistryDWORD(hKey,reg.szAntiAliasDynamicFPS,&dwValue))
        {
            if (dwValue < D3D_REG_ANTIALIASDYNAMICFPS_MIN) dwValue = D3D_REG_ANTIALIASDYNAMICFPS_MIN;
            if (dwValue > D3D_REG_ANTIALIASDYNAMICFPS_MAX) dwValue = D3D_REG_ANTIALIASDYNAMICFPS_MAX;
            getDC()->nvD3DRegistryData.regAntiAliasDynamicMSpF = 1000 / dwValue;
        }

        if (nvReadRegistryDWORD(hKey,reg.szCKCompatabilityEnableString,&dwValue))
        {
            getDC()->nvD3DRegistryData.regD3DEnableBits1 &= ~D3D_REG_CKCOMPATABILITYENABLE_MASK;
            getDC()->nvD3DRegistryData.regD3DEnableBits1 |= (dwValue) ? D3D_REG_CKCOMPATABILITYENABLE_ENABLE : D3D_REG_CKCOMPATABILITYENABLE_DISABLE;
        }

        if (nvReadRegistryDWORD(hKey,reg.szDirectMapEnableString,&dwValue))
        {
            getDC()->nvD3DRegistryData.regD3DEnableBits1 &= ~D3D_REG_DIRECTMAPENABLE_MASK;
            getDC()->nvD3DRegistryData.regD3DEnableBits1 |= (dwValue) ? D3D_REG_DIRECTMAPENABLE_ENABLE : D3D_REG_DIRECTMAPENABLE_DISABLE;
        }

        if (nvReadRegistryDWORD(hKey,reg.szEnforceStrictTrilinearString,&dwValue))
        {
            getDC()->nvD3DRegistryData.regD3DEnableBits1 &= ~D3D_REG_ENFORCESTRICTTRILINEAR_MASK;
            getDC()->nvD3DRegistryData.regD3DEnableBits1 |= (dwValue) ? D3D_REG_ENFORCESTRICTTRILINEAR_ENABLE : D3D_REG_ENFORCESTRICTTRILINEAR_DISABLE;
        }

        if (nvReadRegistryDWORD(hKey,reg.szFlushAfterBlitEnableString,&dwValue))
        {
            getDC()->nvD3DRegistryData.regD3DEnableBits1 &= ~D3D_REG_FLUSHAFTERBLITENABLE_MASK;
            getDC()->nvD3DRegistryData.regD3DEnableBits1 |= (dwValue) ? D3D_REG_FLUSHAFTERBLITENABLE_ENABLE : D3D_REG_FLUSHAFTERBLITENABLE_DISABLE;
        }

        if (nvReadRegistryDWORD(hKey,reg.szFogTableString,&dwValue))
        {
            getDC()->nvD3DRegistryData.regD3DEnableBits1 &= ~D3D_REG_FOGTABLEENABLE_MASK;
            getDC()->nvD3DRegistryData.regD3DEnableBits1 |= (dwValue) ? D3D_REG_FOGTABLEENABLE_ENABLE : D3D_REG_FOGTABLEENABLE_DISABLE;
        }

        if (nvReadRegistryDWORD(hKey,reg.szForceBlitWaitFlagEnableString,&dwValue))
        {
            getDC()->nvD3DRegistryData.regD3DEnableBits1 &= ~D3D_REG_FORCEBLITWAITFLAGENABLE_MASK;
            getDC()->nvD3DRegistryData.regD3DEnableBits1 |= (dwValue) ? D3D_REG_FORCEBLITWAITFLAGENABLE_ENABLE : D3D_REG_FORCEBLITWAITFLAGENABLE_DISABLE;
        }

        if (nvReadRegistryDWORD(hKey,reg.szLimitQueuedFBBlitsEnableString,&dwValue))
        {
            getDC()->nvD3DRegistryData.regD3DEnableBits1 &= ~D3D_REG_LIMITQUEUEDFBBLITSENABLE_MASK;
            getDC()->nvD3DRegistryData.regD3DEnableBits1 |= (dwValue) ? D3D_REG_LIMITQUEUEDFBBLITSENABLE_ENABLE : D3D_REG_LIMITQUEUEDFBBLITSENABLE_DISABLE;
        }

        if (nvReadRegistryDWORD(hKey,reg.szLogoEnableString,&dwValue))
        {
            getDC()->nvD3DRegistryData.regD3DEnableBits1 &= ~D3D_REG_LOGOENABLE_MASK;
            getDC()->nvD3DRegistryData.regD3DEnableBits1 |= (dwValue) ? D3D_REG_LOGOENABLE_ENABLE : D3D_REG_LOGOENABLE_DISABLE;
        }

#ifndef TEXFORMAT_CRD
        if (nvReadRegistryDWORD(hKey,reg.szPaletteEnableString,&dwValue))
        {
            getDC()->nvD3DRegistryData.regD3DEnableBits1 &= ~D3D_REG_PALETTEENABLE_MASK;
            getDC()->nvD3DRegistryData.regD3DEnableBits1 |= (dwValue) ? D3D_REG_PALETTEENABLE_ENABLE : D3D_REG_PALETTEENABLE_DISABLE;
        }
#endif  // !TEXFORMAT_CRD

        if (nvReadRegistryDWORD(hKey,reg.szSquashWString,&dwValue))
        {
            getDC()->nvD3DRegistryData.regD3DEnableBits1 &= ~D3D_REG_SQUASHW_MASK;
            getDC()->nvD3DRegistryData.regD3DEnableBits1 |= (dwValue) ? D3D_REG_SQUASHW_ENABLE : D3D_REG_SQUASHW_DISABLE;
        }

        if (nvReadRegistryDWORD(hKey,reg.szSceneSyncString,&dwValue))
        {
            getDC()->nvD3DRegistryData.regD3DEnableBits1 &= ~D3D_REG_SSYNCENABLE_MASK;
            getDC()->nvD3DRegistryData.regD3DEnableBits1 |= (dwValue) ? D3D_REG_SSYNCENABLE_ENABLE : D3D_REG_SSYNCENABLE_DISABLE;
        }

#ifndef TEXFORMAT_CRD
        if (nvReadRegistryDWORD(hKey,reg.szTextureCompressionEnableString,&dwValue))
        {
            getDC()->nvD3DRegistryData.regD3DEnableBits1 &= ~D3D_REG_TEXTURECOMPRESSIONENABLE_MASK;
            getDC()->nvD3DRegistryData.regD3DEnableBits1 |= (dwValue) ? D3D_REG_TEXTURECOMPRESSIONENABLE_ENABLE : D3D_REG_TEXTURECOMPRESSIONENABLE_DISABLE;
        }
#endif  // !TEXFORMAT_CRD

        if (nvReadRegistryDWORD(hKey,reg.szTextureManagementEnableString,&dwValue))
        {
            getDC()->nvD3DRegistryData.regD3DEnableBits1 &= ~D3D_REG_TEXTUREMANAGEMENTENABLE_MASK;
            getDC()->nvD3DRegistryData.regD3DEnableBits1 |= (dwValue) ? D3D_REG_TEXTUREMANAGEMENTENABLE_ENABLE : D3D_REG_TEXTUREMANAGEMENTENABLE_DISABLE;
        }

        if (nvReadRegistryDWORD(hKey,reg.szTilingString,&dwValue))
        {
            getDC()->nvD3DRegistryData.regD3DEnableBits1 &= ~D3D_REG_TILINGENABLE_MASK;
            getDC()->nvD3DRegistryData.regD3DEnableBits1 |= (dwValue) ? D3D_REG_TILINGENABLE_ENABLE : D3D_REG_TILINGENABLE_DISABLE;
        }


        if (nvReadRegistryDWORD(hKey,reg.szZCullEnableString,&dwValue))
        {
            getDC()->nvD3DRegistryData.regD3DEnableBits1 &= ~D3D_REG_ZCULLENABLE_MASK;
            getDC()->nvD3DRegistryData.regD3DEnableBits1 |= (dwValue) ? D3D_REG_ZCULLENABLE_ENABLE : D3D_REG_ZCULLENABLE_DISABLE;
        }


        if (nvReadRegistryDWORD(hKey,reg.szUserMipMapsString,&dwValue))
        {
            getDC()->nvD3DRegistryData.regD3DEnableBits1 &= ~D3D_REG_USERMIPMAPENABLE_MASK;
            getDC()->nvD3DRegistryData.regD3DEnableBits1 |= (dwValue) ? D3D_REG_USERMIPMAPENABLE_ENABLE : D3D_REG_USERMIPMAPENABLE_DISABLE;
        }

        if (nvReadRegistryDWORD(hKey,reg.szVideoTextureString,&dwValue))
        {
            getDC()->nvD3DRegistryData.regD3DEnableBits1 &= ~D3D_REG_VIDEOTEXTUREENABLE_MASK;
            getDC()->nvD3DRegistryData.regD3DEnableBits1 |= (dwValue) ? D3D_REG_VIDEOTEXTUREENABLE_ENABLE : D3D_REG_VIDEOTEXTUREENABLE_DISABLE;
        }

        if (nvReadRegistryDWORD(hKey,reg.szVS_HOS_EmulationString,&dwValue))
        {
            getDC()->nvD3DRegistryData.regD3DEnableBits1 &= ~D3D_REG_VS_HOS_EMULATION_MASK ;
            getDC()->nvD3DRegistryData.regD3DEnableBits1 |= (dwValue) ? D3D_REG_VS_HOS_EMULATION_ENABLE  : D3D_REG_VS_HOS_EMULATION_DISABLE;
        }

        if (nvReadRegistryDWORD(hKey,reg.szWEnableString,&dwValue))
        {
            getDC()->nvD3DRegistryData.regD3DEnableBits1 &= ~D3D_REG_WBUFFERENABLE_MASK;
            getDC()->nvD3DRegistryData.regD3DEnableBits1 |= (dwValue) ? D3D_REG_WBUFFERENABLE_ENABLE : D3D_REG_WBUFFERENABLE_DISABLE;
        }

        if (nvReadRegistryDWORD(hKey,reg.szZ24EnableString,&dwValue))
        {
            getDC()->nvD3DRegistryData.regD3DEnableBits1 &= ~D3D_REG_Z24ENABLE_MASK;
            getDC()->nvD3DRegistryData.regD3DEnableBits1 |= (dwValue) ? D3D_REG_Z24ENABLE_ENABLE : D3D_REG_Z24ENABLE_DISABLE;
        }

        if (nvReadRegistryDWORD(hKey,reg.szZCompressEnableString,&dwValue))
        {
            getDC()->nvD3DRegistryData.regD3DEnableBits1 &= ~D3D_REG_ZCOMPRESSENABLE_MASK;
            getDC()->nvD3DRegistryData.regD3DEnableBits1 |= (dwValue) ? D3D_REG_ZCOMPRESSENABLE_ENABLE : D3D_REG_ZCOMPRESSENABLE_DISABLE;
        }

        // Entry to enable/disable DirectX VA.  The default is enabled.
        if (nvReadRegistryDWORD(hKey, "DXVAEnable", &dwValue))
        {
            pDriverData->dwDXVAFlags = (dwValue) ? DXVA_ENABLED_IN_REGISTRY : 0;
        }

        // non-boolean enumerated types

        if (nvReadRegistryDWORD(hKey,reg.szAntiAliasQualityString,&dwValue))
        {
            if (dwValue < D3D_REG_ANTIALIASQUALITY_MIN) dwValue = D3D_REG_ANTIALIASQUALITY_MIN;
            if (dwValue > D3D_REG_ANTIALIASQUALITY_MAX) dwValue = D3D_REG_ANTIALIASQUALITY_MAX;
            getDC()->nvD3DRegistryData.regAntiAliasQuality = dwValue;
        }

        if (nvReadRegistryDWORD(hKey,reg.szCaptureConfigString,&dwValue))
        {
            getDC()->nvD3DRegistryData.regCaptureConfig = dwValue;
        }

        if (nvReadRegistryDWORD(hKey,reg.szCaptureEnableString,&dwValue))
        {
            getDC()->nvD3DRegistryData.regCaptureEnable = dwValue;
        }

        if (nvReadRegistryDWORD(hKey,reg.szCKRefString,&dwValue))
        {
            if (dwValue > D3D_REG_CKREF_MAX)
                dwValue = D3D_REG_DEFAULT_CKREF;
            getDC()->nvD3DRegistryData.regColorkeyRef = dwValue;
        }

        if (nvReadRegistryDWORD(hKey,reg.szAnisotropicLevelString,&dwValue))
        {
            getDC()->nvD3DRegistryData.regAnisotropicLevel = dwValue;
        }

        if (nvReadRegistryDWORD(hKey,reg.szMipMapDitherEnableString,&dwValue))
        {
            if (dwValue > D3D_REG_MIPMAPDITHERMODE_MAX)
                dwValue = D3D_REG_DEFAULT_MIPMAPDITHERMODE;
            getDC()->nvD3DRegistryData.regMipMapDitherMode = dwValue;
        }

#ifdef TEXFORMAT_CRD
        if (nvReadRegistryDWORD(hKey,reg.szSurfaceFormatsDX7String,&dwValue))
        {
            getDC()->nvD3DRegistryData.regSurfaceFormatsDX7 = dwValue;
        }

        if (nvReadRegistryDWORD(hKey,reg.szSurfaceFormatsDX8String,&dwValue))
        {
            getDC()->nvD3DRegistryData.regSurfaceFormatsDX8 = dwValue;
        }
#endif  // TEXFORMAT_CRD

        if (nvReadRegistryDWORD(hKey,reg.szTexelAlignmentString,&dwValue))
        {
            if (dwValue > D3D_REG_TEXELALIGNMENT_MAX)
                dwValue = D3D_REG_DEFAULT_TEXELALIGNMENT;
            getDC()->nvD3DRegistryData.regTexelAlignment = dwValue;
        }

        if (nvReadRegistryDWORD(hKey,reg.szTextureManageStrategyString,&dwValue))
        {
            if (dwValue > D3D_REG_TEXTUREMANAGESTRATEGY_AGGRESSIVE)
                dwValue = D3D_REG_TEXTUREMANAGESTRATEGY_AGGRESSIVE;
            getDC()->nvD3DRegistryData.regTexelAlignment = dwValue;
        }

        if (nvReadRegistryDWORD(hKey,reg.szValidateZMethodString,&dwValue))
        {
            if (dwValue > D3D_REG_VALIDATEZMETHOD_MAX)
                dwValue = D3D_REG_DEFAULT_VALIDATEZMETHOD;
            getDC()->nvD3DRegistryData.regValidateZMethod = dwValue;
        }

        if (nvReadRegistryDWORD(hKey,reg.szVSyncModeString,&dwValue))
        {
            if (dwValue > D3D_REG_VSYNCMODE_MAX)
                dwValue = D3D_REG_DEFAULT_VSYNCMODE;
            getDC()->nvD3DRegistryData.regVSyncMode = dwValue;
        }

        if (nvReadRegistryDWORD(hKey,reg.szW16FormatString,&dwValue))
        {
            if (dwValue < D3D_REG_WFORMAT_MIN)
                dwValue = D3D_REG_DEFAULT_W16FORMAT;
            if (dwValue > D3D_REG_WFORMAT_MAX)
                dwValue = D3D_REG_DEFAULT_W16FORMAT;
            getDC()->nvD3DRegistryData.regW16Format = (WORD)dwValue;
        }

        if (nvReadRegistryDWORD(hKey,reg.szW32FormatString,&dwValue))
        {
            if (dwValue < D3D_REG_WFORMAT_MIN)
                dwValue = D3D_REG_DEFAULT_W32FORMAT;
            if (dwValue > D3D_REG_WFORMAT_MAX)
                dwValue = D3D_REG_DEFAULT_W32FORMAT;
            getDC()->nvD3DRegistryData.regW32Format = (WORD)dwValue;
        }

        // non-boolean non-enumerated types

        if (nvReadRegistryDWORD(hKey,reg.szAACompatibilityBitsString,&dwValue))
        {
            getDC()->nvD3DRegistryData.regAACompatibilityBits = dwValue;
        }

        if (nvReadRegistryDWORD(hKey,reg.szAAReadCompatibilityFileString,&dwValue))
        {
            getDC()->nvD3DRegistryData.regAAReadCompatibilityFile = dwValue;
        }

        if (nvReadRegistryDWORD(hKey,reg.szAGPTextureCutoff,&dwValue))
        {
            getDC()->nvD3DRegistryData.regAGPTexCutOff = dwValue;
        }

        if (nvReadRegistryDWORD(hKey,reg.szCapturePlayFilenum,&dwValue))
        {
            getDC()->nvD3DRegistryData.regCapturePlayFilenum = dwValue;
        }

        if (nvReadRegistryDWORD(hKey,reg.szCaptureRecordFilenum,&dwValue))
        {
            getDC()->nvD3DRegistryData.regCaptureRecordFilenum = dwValue;
        }

        if (nvReadRegistryDWORD(hKey,reg.szD3DContextMaxString,&dwValue))
        {
            getDC()->nvD3DRegistryData.regD3DContextMax = dwValue;
        }

        if (nvReadRegistryDWORD(hKey,reg.szD3DTextureMaxString,&dwValue))
        {
            getDC()->nvD3DRegistryData.regD3DTextureMax = dwValue;
        }

        if (nvReadRegistryDWORD(hKey,reg.szDebugLevelString,&dwValue))
        {
            getDC()->nvD3DRegistryData.regDebugLevel = dwValue;
            #ifdef DEBUG
            dbgLevel = dwValue;
            #endif
        }

        if (nvReadRegistryDWORD(hKey,reg.szLodBiasAdjustString,&dwValue))
        {
            //if (dwValue & 0xFFFFFF00)
            //    dwValue = D3D_REG_DEFAULT_LODBIASADJUST;
            //this need to be a float value -- don't do that comparison
            getDC()->nvD3DRegistryData.regLODBiasAdjust = dwValue;
        }

        if (nvReadRegistryDWORD(hKey,reg.szMinVideoTexSizeString,&dwValue))
        {
            getDC()->nvD3DRegistryData.regMinVideoTextureSize = dwValue;
        }

        if (nvReadRegistryDWORD(hKey,reg.szPCITexHeapSizeString,&dwValue))
        {
            getDC()->nvD3DRegistryData.regPCITexHeapSize = dwValue;
        }

        if (nvReadRegistryDWORD(hKey,reg.szPreRenderLimitString,&dwValue))
        {
            if (dwValue > D3D_REG_PRERENDERLIMIT_MAX)
                dwValue = D3D_REG_PRERENDERLIMIT_MAX;
            if (dwValue < D3D_REG_PRERENDERLIMIT_MIN)
                dwValue = D3D_REG_PRERENDERLIMIT_MIN;
            getDC()->nvD3DRegistryData.regPreRenderLimit = dwValue;
        }

        if (nvReadRegistryDWORD(hKey,reg.szPushBufferSizeMaxString,&dwValue))
        {
            if (dwValue)
            {
                getDC()->nvD3DRegistryData.regPushBufferSizeMax = dwValue;
            }
            else
            {
                getDC()->nvD3DRegistryData.regPushBufferSizeMax = D3D_REG_DEFAULT_PUSHBUFFERSIZEMAX;
            }
        }

        if (nvReadRegistryDWORD(hKey,reg.szWScale16String,&dwValue))
        {
            if (dwValue > D3D_REG_WSCALE16_MAX)
                dwValue = D3D_REG_DEFAULT_WSCALE16;
            getDC()->nvD3DRegistryData.regfWScale16 = (float)dwValue;
        }

        if (nvReadRegistryDWORD(hKey,reg.szWScale24String,&dwValue))
        {
            if (dwValue > D3D_REG_WSCALE24_MAX)
                dwValue = D3D_REG_DEFAULT_WSCALE24;
            getDC()->nvD3DRegistryData.regfWScale24 = (float)dwValue;
        }

        if (nvReadRegistryDWORD(hKey,reg.szZBiasString,&dwValue))
        {
            getDC()->nvD3DRegistryData.regfZBias = (float)dwValue;
        }

        // strings
        if (nvReadRegistryString(hKey,reg.szCapturePlayFilenameString,(LPBYTE)szString))
        {
            nvStrNCpy (getDC()->nvD3DRegistryData.regszCapturePlayFilename, szString, D3D_REG_STRING_LENGTH);
        }

        if (nvReadRegistryString(hKey,reg.szCapturePlayPathString,(LPBYTE)szString))
        {
            nvStrNCpy (getDC()->nvD3DRegistryData.regszCapturePlayPath, szString, D3D_REG_STRING_LENGTH);
        }

        if (nvReadRegistryString(hKey,reg.szCaptureRecordPathString,(LPBYTE)szString))
        {
            nvStrNCpy (getDC()->nvD3DRegistryData.regszCaptureRecordPath, szString, D3D_REG_STRING_LENGTH);
        }

        if (nvReadRegistryString(hKey,reg.szCaptureRecordFilenameString,(LPBYTE)szString))
        {
            nvStrNCpy (getDC()->nvD3DRegistryData.regszCaptureRecordFilename, szString, D3D_REG_STRING_LENGTH);
        }

#ifndef WINNT
        RegCloseKey(hKey);
#endif
    }

#ifdef  STEREO_SUPPORT
    LoadStereoRegistry();
#endif  //STEREO_SUPPORT

    dbgTracePop();
    return;
}

#pragma warning(default: 4296)

//---------------------------------------------------------------------------

/*
 * Read the current driver settings from the registry.
 */

void D3DModifyCapabilities
(
    D3DHAL_GLOBALDRIVERDATA  *pNvGlobal,
    GLOBALDATA               *pDriverData
)
{
    // Set the Fog Table Caps bit based on the registry setting.
    pNvGlobal->hwCaps.dpcLineCaps.dwRasterCaps &= ~D3DPRASTERCAPS_FOGTABLE;
    pNvGlobal->hwCaps.dpcTriCaps.dwRasterCaps  &= ~D3DPRASTERCAPS_FOGTABLE;
    if (getDC()->nvD3DRegistryData.regD3DEnableBits1 & D3D_REG_FOGTABLEENABLE_ENABLE) {
        pNvGlobal->hwCaps.dpcLineCaps.dwRasterCaps |= D3DPRASTERCAPS_FOGTABLE;
        pNvGlobal->hwCaps.dpcTriCaps.dwRasterCaps  |= D3DPRASTERCAPS_FOGTABLE;
    }

    // Set anti-aliasing capabilities based on the registry setting.
    pNvGlobal->hwCaps.dpcLineCaps.dwRasterCaps &= ~(D3DPRASTERCAPS_ANTIALIASSORTDEPENDENT | D3DPRASTERCAPS_ANTIALIASSORTINDEPENDENT | D3DPRASTERCAPS_ANTIALIASEDGES);
    pNvGlobal->hwCaps.dpcTriCaps.dwRasterCaps  &= ~(D3DPRASTERCAPS_ANTIALIASSORTDEPENDENT | D3DPRASTERCAPS_ANTIALIASSORTINDEPENDENT | D3DPRASTERCAPS_ANTIALIASEDGES);

    if (getDC()->nvD3DPerfData.dwHaveAALines) {
        pNvGlobal->hwCaps.dpcLineCaps.dwRasterCaps |= D3DPRASTERCAPS_ANTIALIASEDGES;
        pNvGlobal->hwCaps.dpcTriCaps.dwRasterCaps  |= D3DPRASTERCAPS_ANTIALIASEDGES;
        getDC()->nvD3DDevCaps.dwD3DCap8.RasterCaps |= D3DPRASTERCAPS_ANTIALIASEDGES;
    }

    if (((getDC()->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_CELSIUS) ||
        (getDC()->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN)) &&
        (getDC()->nvD3DRegistryData.regD3DEnableBits1 & D3D_REG_ANTIALIASENABLE_ENABLE)) {
        /* for now we only support scene aa
        pNvGlobal->hwCaps.dpcLineCaps.dwRasterCaps |= (D3DPRASTERCAPS_ANTIALIASSORTDEPENDENT | D3DPRASTERCAPS_ANTIALIASSORTINDEPENDENT | D3DPRASTERCAPS_ANTIALIASEDGES);
        pNvGlobal->hwCaps.dpcTriCaps.dwRasterCaps  |= (D3DPRASTERCAPS_ANTIALIASSORTDEPENDENT | D3DPRASTERCAPS_ANTIALIASSORTINDEPENDENT | D3DPRASTERCAPS_ANTIALIASEDGES);
        */
        pNvGlobal->hwCaps.dpcLineCaps.dwRasterCaps |= (D3DPRASTERCAPS_ANTIALIASSORTDEPENDENT | D3DPRASTERCAPS_ANTIALIASSORTINDEPENDENT);
        pNvGlobal->hwCaps.dpcTriCaps.dwRasterCaps  |= (D3DPRASTERCAPS_ANTIALIASSORTDEPENDENT | D3DPRASTERCAPS_ANTIALIASSORTINDEPENDENT);
    }

    // Set w-buffer capabilities based on the registry setting.
    pNvGlobal->hwCaps.dpcLineCaps.dwRasterCaps &= ~D3DPRASTERCAPS_WBUFFER;
    pNvGlobal->hwCaps.dpcTriCaps.dwRasterCaps  &= ~D3DPRASTERCAPS_WBUFFER;
    if (getDC()->nvD3DRegistryData.regD3DEnableBits1 & D3D_REG_WBUFFERENABLE_ENABLE) {
        pNvGlobal->hwCaps.dpcLineCaps.dwRasterCaps |= D3DPRASTERCAPS_WBUFFER;
        pNvGlobal->hwCaps.dpcTriCaps.dwRasterCaps  |= D3DPRASTERCAPS_WBUFFER;
    }

#ifndef TEXFORMAT_CRD
    // Remove 8 bit texture formats if the registry says so, and if we have palette support
    if (pDriverData->nvD3DPerfData.dwNVClasses & (NVCLASS_FAMILY_CELSIUS | NVCLASS_FAMILY_KELVIN)) {
        if ((global.dwDXRuntimeVersion < 0x0800) ||
            ((getDC()->nvD3DRegistryData.regD3DEnableBits1 & D3D_REG_PALETTEENABLE_MASK) == D3D_REG_PALETTEENABLE_DISABLE)) {
            pNvGlobal->dwNumTextureFormats -= NV_NUM_8BIT_TEXTURE_FORMATS;
        }
    }
#endif  // !TEXFORMATS_CRD

    // Export only 16bit z-buffer formats when the registry says so.
    if (!(getDC()->nvD3DRegistryData.regD3DEnableBits1 & D3D_REG_Z24ENABLE_ENABLE)) {
        pNvGlobal->hwCaps.dwDeviceZBufferBitDepth &= ~(DDBD_24 | DDBD_32);
    }

    // enable the direct-mapped TSS extension
    if (getDC()->nvD3DRegistryData.regD3DEnableBits1 & D3D_REG_DIRECTMAPENABLE_ENABLE) {
        getDC()->nvD3DDevCaps.d3dExtCaps.wMaxTextureBlendStages = 8;
    }

    // enable emulation of Higher order surface and vertex shaders on celsius class devices.
    if ((pDriverData->nvD3DPerfData.dwNVClasses & (NVCLASS_FAMILY_CELSIUS | NVCLASS_FAMILY_KELVIN)) == (NVCLASS_FAMILY_CELSIUS)) {
        if(getDC()->nvD3DRegistryData.regD3DEnableBits1 & D3D_REG_VS_HOS_EMULATION_ENABLE){
            getDC()->nvD3DDevCaps.dwD3DCap8.DevCaps |= (D3DDEVCAPS_QUINTICRTPATCHES | D3DDEVCAPS_RTPATCHES);
            getDC()->nvD3DDevCaps.dwD3DCap8.VertexShaderVersion = D3DVS_VERSION(1,1);
        }
    }

    return;
}

#endif  // NVARCH >= 0x04
