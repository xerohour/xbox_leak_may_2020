#pragma once

#ifdef _XBOX
    #include <xtl.h>
#else
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
#endif

//
//Major/Minor version numbers
//
#define VERMAJOR   1L
#define VERMINOR   3L

#define DIBFLAGS_MOVIE		 0x00000001
#define DIBGLAGS_MOVIETEASER 0x00000100
#define DIBFLAGS_KIOSK		 0x00010000

#define nMAX_STRINGLEN 128

#pragma warning(push)
#pragma warning(disable : 4200)  //zero-size array in class
#pragma pack(1)
//
//Demo information block
//
class DIB
{
public:
	union 
    {
	    DWORD m_dwSize;
		DIB  *m_pNext;
	};

    DWORD m_dwFlags;
    DWORD m_dwPriority;
	char *m_pszTitle;
	char *m_pszFolder;
    char *m_pszXBE;
	char *m_pszTeaser;
    char *m_pszPersist;
	char *m_pszAddi;
    char *m_pszAuthor;
    char *m_pszDemoType;
	BYTE  m_data[];
};

#pragma warning(pop)

class XDIHEADER
{
public:
    BYTE m_bSignature[3];
    BYTE m_bPadding;
    LONG m_lVerMajor;
    LONG m_lVerMinor;
    LONG m_lNumDemos;
	LONG m_lCheckSum;
    BOOL m_fKiosk;
    char m_szGameName[nMAX_STRINGLEN];
    char m_szPersist[nMAX_STRINGLEN];

    XDIHEADER() 
    {
        m_bSignature[0] = 'X';
        m_bSignature[1] = 'D';
        m_bSignature[2] = 'I';
        m_lVerMajor = VERMAJOR;
        m_lVerMinor = VERMINOR;
        m_lNumDemos = 0L;
		m_lCheckSum = 0L;
        *m_szGameName = '\0';
        *m_szPersist = '\0';
    };
};
#pragma pack()

int VerifyContents(BYTE *pbDemos, DWORD dwFileSize);
void WriteChecksum(HANDLE hFile);

//
//headers for functions exported by XDIFile.dll
//

#ifdef _XBOX
    DWORD XDIReadFile(const char *pszXDIFile, void *pvDemos, DWORD dwFileSize);
#else
    extern "C"
    {
        _declspec(dllexport) long __stdcall XDIGetDllVersion(void);
        _declspec(dllexport) long __stdcall XDIGetDemoHeader(char *pszXDIFile, XDIHEADER *pxdi);
        _declspec(dllexport) long __stdcall XDIGetDemoInfo(char *pszXDIFile, DIB *pdib, LONG lIdx);
        _declspec(dllexport) long __stdcall XDIWriteDemoInfo(char *pszXDIFile, DIB *pdib);
        _declspec(dllexport) long __stdcall XDIWriteHeader(LPSTR pszXDIFile, LONG lNumDemos, LONG lKiosk,
                                                           char *pszCallingXBE, char *pszPersist);
    }
#endif

