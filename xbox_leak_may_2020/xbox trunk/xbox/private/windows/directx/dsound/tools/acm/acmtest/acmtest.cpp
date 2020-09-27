#include <windows.h>
#include <stdio.h>
#include <mmreg.h>
#include <msacm.h>

BOOL CALLBACK 
acmFormatEnumCallback
(
    HACMDRIVERID            hadid,
    LPACMFORMATDETAILSA     pafd,
    DWORD                   dwInstance,
    DWORD                   fdwSupport
)
{
    printf("%s\n", pafd->szFormat);
    
    return TRUE;
}


BOOL CALLBACK
acmDriverEnumCallback
(
    HACMDRIVERID            hadid,
    DWORD                   dwInstance,
    DWORD                   fdwSupport
)
{
    ACMDRIVERDETAILS        add;
    HACMDRIVER              had;
    ACMFORMATDETAILS        afd;
    WAVEFORMATEX            wfxSrc;
    IMAADPCMWAVEFORMAT      wfxDst;
    MMRESULT                mmr;

    ZeroMemory(&add, sizeof(add));
    add.cbStruct = sizeof(add);
    
    ZeroMemory(&afd, sizeof(afd));
    afd.cbStruct = sizeof(afd);
    afd.dwFormatTag = 0x69;
    afd.pwfx = &wfxDst.wfx;
    afd.cbwfx = sizeof(wfxDst);

    ZeroMemory(&wfxDst, sizeof(wfxDst));
    wfxDst.wfx.wFormatTag = 0x69;

    wfxSrc.wFormatTag = WAVE_FORMAT_PCM;
    wfxSrc.nChannels = 2;
    wfxSrc.nSamplesPerSec = 22050;
    wfxSrc.wBitsPerSample = 16;
    wfxSrc.nBlockAlign = 4;
    wfxSrc.nAvgBytesPerSec = wfxSrc.nSamplesPerSec * wfxSrc.nBlockAlign;

    mmr = acmDriverDetails(hadid, &add, 0);
    mmr = acmDriverOpen(&had, hadid, 0);
    mmr = acmFormatEnum(had, &afd, acmFormatEnumCallback, 0, ACM_FORMATENUMF_WFORMATTAG);
    mmr = acmFormatSuggest(had, &wfxSrc, &wfxDst.wfx, sizeof(wfxDst), 0);
    mmr = acmDriverClose(had, 0);

    return TRUE;
}


void
main
(
    void
)
{
    MMRESULT                mmr;

    mmr = acmDriverEnum(acmDriverEnumCallback, 0, 0);
}