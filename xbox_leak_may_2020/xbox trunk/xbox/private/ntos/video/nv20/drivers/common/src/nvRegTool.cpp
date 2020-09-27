 /***************************************************************************\
|*                                                                           *|
|*       Copyright 1993-2001 NVIDIA, Corporation.  All rights reserved.      *|
|*                                                                           *|
|*     NOTICE TO USER:   The source code  is copyrighted under  U.S. and     *|
|*     international laws.  Users and possessors of this source code are     *|
|*     hereby granted a nonexclusive,  royalty-free copyright license to     *|
|*     use this code in individual and commercial software.                  *|
|*                                                                           *|
|*     Any use of this source code must include,  in the user documenta-     *|
|*     tion and  internal comments to the code,  notices to the end user     *|
|*     as follows:                                                           *|
|*                                                                           *|
|*     NVIDIA, CORPORATION MAKES NO REPRESENTATION ABOUT THE SUITABILITY     *|
|*     OF  THIS SOURCE  CODE  FOR ANY PURPOSE.  IT IS  PROVIDED  "AS IS"     *|
|*     WITHOUT EXPRESS OR IMPLIED WARRANTY OF ANY KIND.  NVIDIA, CORPOR-     *|
|*     ATION DISCLAIMS ALL WARRANTIES  WITH REGARD  TO THIS SOURCE CODE,     *|
|*     INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY, NONINFRINGE-     *|
|*     MENT,  AND FITNESS  FOR A PARTICULAR PURPOSE.   IN NO EVENT SHALL     *|
|*     NVIDIA, CORPORATION  BE LIABLE FOR ANY SPECIAL,  INDIRECT,  INCI-     *|
|*     DENTAL, OR CONSEQUENTIAL DAMAGES,  OR ANY DAMAGES  WHATSOEVER RE-     *|
|*     SULTING FROM LOSS OF USE,  DATA OR PROFITS,  WHETHER IN AN ACTION     *|
|*     OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,  ARISING OUT OF     *|
|*     OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOURCE CODE.     *|
|*                                                                           *|
|*     U.S. Government  End  Users.   This source code  is a "commercial     *|
|*     item,"  as that  term is  defined at  48 C.F.R. 2.101 (OCT 1995),     *|
|*     consisting  of "commercial  computer  software"  and  "commercial     *|
|*     computer  software  documentation,"  as such  terms  are  used in     *|
|*     48 C.F.R. 12.212 (SEPT 1995)  and is provided to the U.S. Govern-     *|
|*     ment only as  a commercial end item.   Consistent with  48 C.F.R.     *|
|*     12.212 and  48 C.F.R. 227.7202-1 through  227.7202-4 (JUNE 1995),     *|
|*     all U.S. Government End Users  acquire the source code  with only     *|
|*     those rights set forth herein.                                        *|
|*                                                                           *|
 \***************************************************************************/
#include "nvprecomp.h"
#include "nvRegTool.h"

NvU8 CRegTool::Open(NV_DRIVER_HANDLE driverHandle, int logicalHeadID, char *subKeyName)
{
#if IS_WIN9X
    char regStr[NV_MAX_REG_KEYNAME_LEN];
    MyExtEscape (logicalHeadID, NV_ESC_GET_LOCAL_REGISTRY_PATH, 0, NULL, NV_MAX_REG_KEYNAME_LEN, regStr);
    nvStrCat(&(regStr[4]),"\\");
    nvStrCat(&(regStr[4]), subKeyName);

    if (RegOpenKeyEx((HKEY)(*((LPDWORD)&regStr[0])), &regStr[4], 0, KEY_QUERY_VALUE, (HKEY *)&m_hKey) != ERROR_SUCCESS) {
        m_hKey = 0;
    }
    return (m_hKey!=0);
#elif IS_WINNT
    m_hDriver = driverHandle;
    return TRUE;
#endif
    dbgError("Unhandled case\n");
    return FALSE;
}


NvU8 CRegTool::Read(char *keyName, NvU32 &val)
{
    NvU32 readValue;
    NvU32 retStatus;
    NvU32 valueSize = sizeof(NvU32);
#if IS_WIN9X
    NvU32  dwType = REG_DWORD;

    retStatus = RegQueryValueEx((HKEY)m_hKey, keyName, NULL, &dwType, (LPBYTE)&readValue, &valueSize);

#elif IS_WINNT
    WCHAR cUniKeyName[NV_MAX_REG_KEYNAME_LEN];
    NvU32 dwUniKeyLen;
    NvU32 dwRetSize;

    EngMultiByteToUnicodeN((LPWSTR)cUniKeyName, NV_MAX_REG_KEYNAME_LEN*sizeof(WCHAR), &dwUniKeyLen,
                           keyName, (nvStrLen(keyName)+1)*sizeof(CHAR));
    retStatus = EngDeviceIoControl(m_hDriver, IOCTL_VIDEO_QUERY_REGISTRY_VAL, (LPVOID)cUniKeyName,
                                   dwUniKeyLen, (LPVOID) &readValue, valueSize, &dwRetSize);
#endif
    if (retStatus == ERROR_SUCCESS)
    {
        val = readValue;
    }
    return (retStatus == ERROR_SUCCESS);
}

NvU8 CRegTool::Read(char *keyName, NvS32 &signedVal)
{
    NvU32 unsignedVal;
    NvU8  bSuccess;

    bSuccess = Read(keyName, unsignedVal);
    if (bSuccess)
    {
        signedVal = (NvS32)unsignedVal;
    }
    return bSuccess;
}

void CRegTool::Close()
{
#if IS_WIN9X
    RegCloseKey((HKEY)m_hKey);
    m_hKey = 0;
#else
    m_hDriver = 0;
#endif
}
