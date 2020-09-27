//===========================================================================
// dmthhelp.h
//
// DirectMusic test harness helper functions
//
// History:
//  03/24/1998 - davidkl - created
//===========================================================================

#pragma once
//---------------------------------------------------------------------------

LPSTR dmthXlatHRESULT(HRESULT hRes);
LPSTR dmthXlatGUID(REFGUID rguid);
LPSTR dmthXlatDMUS_APATH(DWORD dwStandardPath);
LPSTR dmthXlatDMUS_STAGE(DWORD dwStage);
void dmthGUIDtoString(REFGUID rguid, LPSTR szBuf);
LPWSTR dmth_lstrcpyW(LPWSTR pwszDest, LPCWSTR pcwszSrc);
LPSTR Log_DMUS_SEGF(DWORD dwFlags);

// allow the old names to continue to work...
LPSTR tdmXlatHRESULT(HRESULT hRes);
LPSTR tdmXlatGUID(REFGUID rguid);
LPSTR tdmXlatDMUS_APATH(DWORD dwStandardPath);
void tdmGUIDtoString(REFGUID rguid, LPSTR szBuf);


//One COM helper function.
HRESULT dmthDirectMusicCreateInstance(REFCLSID rclsid, 
                            LPUNKNOWN pUnkOuter,
                            REFIID riid,
                            LPVOID *ppv);




