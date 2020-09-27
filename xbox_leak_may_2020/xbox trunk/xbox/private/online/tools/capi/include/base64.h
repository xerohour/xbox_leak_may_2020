#ifndef __BASE64_H__
#define __BASE64_H__

#ifdef __cplusplus
extern "C" {
#endif


#ifdef UNICODE
#define Base64Decode  Base64DecodeW
#else
#define Base64Decode  Base64DecodeA
#endif // !UNICODE

DWORD			// ERROR_*
Base64DecodeA(
    IN CHAR const *pchIn,
    IN DWORD cchIn,
    OUT BYTE *pbOut,
    OUT DWORD *pcbOut);

DWORD			// ERROR_*
Base64DecodeW(
    IN WCHAR const *pchIn,
    IN DWORD cchIn,
    OUT BYTE *pbOut,
    OUT DWORD *pcbOut);

    
#ifdef UNICODE
#define Base64Encode  Base64EncodeW
#else
#define Base64Encode  Base64EncodeA
#endif // !UNICODE

DWORD			// ERROR_*
Base64EncodeA(
    IN BYTE const *pbIn,
    IN DWORD cbIn,
    OUT CHAR *pchOut,
    OUT DWORD *pcchOut);

DWORD			// ERROR_*
Base64EncodeW(
    IN BYTE const *pbIn,
    IN DWORD cbIn,
    OUT WCHAR *pchOut,
    OUT DWORD *pcchOut);

    
#ifdef __cplusplus
}       // Balance extern "C" above
#endif

#endif // BASE64

