#include "std.h"
#include "XApp.h"
#include <share.h>

#include "..\wmaenc\wmfencode.h"

#if defined(XBOX) && defined(DBG)
// #define DETECT_MEMORY_LEAKS
// #define COMPILE_MEMORY_LEAK_FUNCTIONS
#else
#endif

#ifdef COMPILE_MEMORY_LEAK_FUNCTIONS
#include <crtdbg.h>
#endif

static HANDLE hFile;
static short* g_NormalizeMap;

DWORD g_dwMusicCompressPos;
DWORD g_dwMusicCompressLength;

DWORD g_dwEncodeReadBufferPos;
DWORD g_dwEncodeReadBufferLength;
LPBYTE g_lpbEncodeReadBuffer;

#ifdef COMPILE_MEMORY_LEAK_FUNCTIONS

typedef struct _MemAlloc {
    long lRequest;
    long size;
    unsigned char bAllocated;
} MemAlloc;

#define MAX_TRACE 100000
MemAlloc gTrace[MAX_TRACE];
long gLastAlloc;
long gAllocationOffset;
long gPassIndex;


// Once you know which allocations are leaking, the gBreakOnList can be used to automatically break when the allocation
// is made. Then you can look at the stack to figure out who’s causing the leak.

// #define BREAK_ON_LEAK_ALLOCATION
#ifdef BREAK_ON_LEAK_ALLOCATION
long gBreakOnList[] = {
#if 0
    // Pass 0
    544, // 16820
    545, // 206220
#endif
    // Pass 1
442, // 30
463, // 30
594, // 30
615, // 30

};
#endif


unsigned long GetHeapUsed(){
    MEMORYSTATUS mem;
    GlobalMemoryStatus(&mem);
    return mem.dwTotalPhys - mem.dwAvailPhys;
}

int __cdecl My_CrtDefaultAllocHook(
        int nAllocType,
        void * pvData,
        size_t nSize,
        int nBlockUse,
        long lRequest,
        const unsigned char * szFileName,
        int nLine
        )
{
    const char* sAllocType;
    int blockIndex;
    switch(nAllocType){
    case _HOOK_ALLOC : sAllocType = "alloc"; break;
    case _HOOK_REALLOC : sAllocType = "realloc"; break;
    case _HOOK_FREE : sAllocType = "free";
        lRequest = * (long*) (((char*) pvData) - 0x8); // Recover lRequest number from debug heap header
        break;
    default:  sAllocType = "??"; break;
    }
#if 0
        DbgPrint("%s: %x %d %d %d\n", sAllocType , pvData, nSize, nBlockUse, lRequest);
#endif

    if(gAllocationOffset == -1){
        gAllocationOffset = lRequest;
    }

    blockIndex = lRequest - gAllocationOffset;
    if(0 <= blockIndex && blockIndex < MAX_TRACE){
        MemAlloc* pAlloc = &gTrace[blockIndex];
        switch(nAllocType){
        case _HOOK_ALLOC:
            gLastAlloc = lRequest;
            pAlloc->lRequest = lRequest;
            pAlloc->size = nSize;
            pAlloc->bAllocated = 1;
#ifdef BREAK_ON_LEAK_ALLOCATION
            if(gPassIndex == 1){
                int i;
                for(i = 0; i < sizeof(gBreakOnList) / sizeof(gBreakOnList[0]); i++){
                    if(lRequest-gAllocationOffset == gBreakOnList[i]){
                        DbgPrint("This allocation was leaked in a previous run.\n");
                        _asm int 3;
                    }
                }
            }
#endif
            break;
        case _HOOK_FREE:
            pAlloc->bAllocated = 0;
            break;
        }
    }
    return 1; /* allow all allocs/reallocs/frees */
}

void ReportAllocated(){
    int high;
    if(gAllocationOffset == -1){
        high = 0;
    }
    else {
        high = gLastAlloc - gAllocationOffset;
    }
    DbgPrint("Pass %d alloc was called %d times.\n", gPassIndex, high);
    int i;
    int garbageCount = 0;
    for(i = 0; i <= high; i++){
        MemAlloc* pAlloc = &gTrace[i];
        if(pAlloc->bAllocated){
            DbgPrint("%d, // %d\n", pAlloc->lRequest-gAllocationOffset, pAlloc->size);
            garbageCount++;
        }
    }
    DbgPrint("%d blocks still allocated.\n", garbageCount);
    gPassIndex++;
}

void ResetLeakDetector(){
    int i;
    gAllocationOffset = -1;
    for(i = 0; i < MAX_TRACE; i++){
        gTrace[i].bAllocated = 0;
    }
}

static unsigned long gHeapSizeStart;
static _CRT_ALLOC_HOOK gOldHook;
extern "C" void EndLeakTest();

extern "C" void StartLeakTest(){
    if(gHeapSizeStart){
        EndLeakTest();
    }
    gOldHook = _CrtSetAllocHook(My_CrtDefaultAllocHook);
    ResetLeakDetector();
    gHeapSizeStart = GetHeapUsed();
    DbgPrint("Heap size at start of leak test: %d bytes\n", gHeapSizeStart);
}

extern "C" void EndLeakTest(){
    if(gHeapSizeStart){
        unsigned long heapSizeEnd = GetHeapUsed();
        DbgPrint("Heap size at end: %d bytes. Delta = %d\n", heapSizeEnd, heapSizeEnd - gHeapSizeStart);
        DbgPrint("Still-allocated blocks (possible leaks):\n");
        ReportAllocated();
        _CrtSetAllocHook(gOldHook);
        DbgPrint("End of leak test.\n");
        gHeapSizeStart = 0;
    }
}

#endif // COMPILE_MEMORY_LEAK_FUNCTIONS

bool CompressAudio(DWORD dwSongID, short sMax)
{
#ifdef DETECT_MEMORY_LEAKS
    StartLeakTest();
#endif

    char szSrcPath [MAX_PATH];
    char szDestPath [MAX_PATH];

    strcpy(szSrcPath, XappTempPcmFileA);
    strcpy(szDestPath, XappTempWmaFileA);

    hFile = CreateFileA(szSrcPath, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        TRACE(_T("CompressAudio: Cannot open source file: %s\n"), szSrcPath);
        return false;
    }

    g_dwMusicCompressLength = GetFileSize(hFile, NULL);

    tWMFEncodeStatus tStatus;
    U32_WMF cbBuffer;
    char szAlteredPath [MAX_PATH];

    HWMFENCODER hWMFEnc = WMFCreateEncoderUsingProfile(&tStatus, szDestPath,
        Profile_AudioOnly_CDAudiophileQuality_128K,
        2, 44100, 16,
        g_dwMusicCompressLength, &cbBuffer, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, WMF_NoPreProcess, szAlteredPath);

    if (tStatus != WMF_Succeeded)
    {
        TRACE(_T("\001CompressAudio: WMFCreateEncoderUsingProfile failed (%d)\n"), tStatus);
        g_dwMusicCompressLength = 0;
        return false;
    }

    ASSERT(hWMFEnc != NULL);

    BYTE* pbBuffer = new U8_WMF [cbBuffer];
    if (pbBuffer == NULL)
    {
        TRACE(_T("\001Not enough memory to compress audio!\n"));
        g_dwMusicCompressLength = 0;
        CloseHandle(hFile);
        WMFClose(hWMFEnc);
        return false;
    }

    ASSERT(g_lpbEncodeReadBuffer == NULL);

    g_dwEncodeReadBufferPos = 0;
    g_dwEncodeReadBufferLength = 0;
    g_lpbEncodeReadBuffer = new BYTE [cbBuffer];

    if (g_lpbEncodeReadBuffer == NULL)
    {
        TRACE(_T("\001Not enough memory to compress audio!\n"));
        g_dwMusicCompressLength = 0;
        CloseHandle(hFile);
        WMFClose(hWMFEnc);
        delete [] pbBuffer;
        return false;
    }

    if (sMax > 0 && sMax < 32767)
    {
        g_NormalizeMap = new short [65536];
        if (g_NormalizeMap != NULL)
        {
            float f = 32767.0f / (float)sMax;
            for (int i = 0; i < 65536; i += 1)
            {
                float s = (float)(i - 32768) / 32767.0f;
                g_NormalizeMap[i] = (short)((s * f) * 32767.0f);
            }
        }
    }

    tStatus = WMFEncode(hWMFEnc, pbBuffer, cbBuffer, 0, 0);

    g_dwMusicCompressLength = 0;
    g_dwMusicCompressPos = 0;

    if (g_NormalizeMap != NULL)
    {
        delete [] g_NormalizeMap;
        g_NormalizeMap = NULL;
    }

    if (g_lpbEncodeReadBuffer != NULL)
    {
        delete [] g_lpbEncodeReadBuffer;
        g_lpbEncodeReadBuffer = NULL;
    }

    if (tStatus != WMF_Succeeded)
    {
        TRACE(_T("\001CompressAudio: WMFEncode failed (%d)\n"), tStatus);
        CloseHandle(hFile);
        WMFClose(hWMFEnc);
        delete [] pbBuffer;
        return false;
    }

    CloseHandle(hFile);
    DeleteFile(szSrcPath);
    WMFClose(hWMFEnc);
    delete [] pbBuffer;

#ifdef DETECT_MEMORY_LEAKS
    EndLeakTest();
#endif

//  TRACE(_T("CompressAudio: complete!\n"));

    return true;
}

I32_WMF WMFCBGetAudioData(U8_WMF** ppbAudioDataBuffer, U32_WMF nInputBufferLen, I64_WMF iSampleOffset, I32_WMF iNumSamplesWanted)
{
    DWORD nBytesNeeded;
    DWORD nBytesRead;
    DWORD nThrowAwayBytes;
    LPVOID lpvReadData;

    TRACE(_T("WMFCBGetAudioData: %d %d\n"), (int)iSampleOffset, (int)iNumSamplesWanted);

    g_dwMusicCompressPos = (DWORD)iSampleOffset * sizeof (I16_WMF) * 2;

    //
    // The WMA encoder ends up re-reading the same regions of the file over and
    // over, so to reduce I/O overhead, we add a buffer to keep the portions of
    // the file that we've already read.  The encoder appears to only read the
    // file in increasing sample offsets, so the cache only moves in one
    // direction.
    //

    nBytesNeeded = iNumSamplesWanted * sizeof (I16_WMF) * 2;

    if ((g_dwMusicCompressPos >= g_dwEncodeReadBufferPos) &&
        (g_dwMusicCompressPos < g_dwEncodeReadBufferPos + g_dwEncodeReadBufferLength)) {

        nThrowAwayBytes = g_dwMusicCompressPos - g_dwEncodeReadBufferPos;

        g_dwEncodeReadBufferLength -= nThrowAwayBytes;

        MoveMemory(g_lpbEncodeReadBuffer, g_lpbEncodeReadBuffer + nThrowAwayBytes,
            g_dwEncodeReadBufferLength);

    } else {
        g_dwEncodeReadBufferLength = 0;
    }

    g_dwEncodeReadBufferPos = g_dwMusicCompressPos;

    if (g_dwEncodeReadBufferLength < nBytesNeeded) {

        if (SetFilePointer(hFile, g_dwEncodeReadBufferPos +
            g_dwEncodeReadBufferLength, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) {
            return 0;
        }

        lpvReadData = g_lpbEncodeReadBuffer + g_dwEncodeReadBufferLength;

        if (!ReadFile(hFile, lpvReadData, nBytesNeeded - g_dwEncodeReadBufferLength,
            &nBytesRead, NULL)) {
            return 0;
        }

        nBytesRead &= ~1;
        g_dwEncodeReadBufferLength += nBytesRead;

        if (g_NormalizeMap != NULL) {

            short* ps = (short*)lpvReadData;
            short* map = &g_NormalizeMap[32768];
            for (UINT i = 0; i < nBytesRead; i += 2, ps += 1)
                *ps = map[*ps];
        }
    }

    nBytesRead = nBytesNeeded;

    if (nBytesRead > g_dwEncodeReadBufferLength) {
        nBytesRead = g_dwEncodeReadBufferLength;
    }

    CopyMemory(*ppbAudioDataBuffer, g_lpbEncodeReadBuffer, nBytesRead);

    return (I32_WMF) nBytesRead;
}

extern "C" HWMFFILE WMFOPEN( const char *filename, const char *mode )
{  FILE * st;
   st = _fsopen(filename, mode, _SH_DENYRW);
   if (st != NULL) {
#ifdef COOL_XDASH
      setvbuf(st, NULL, _IOFBF, 10*1024*1024);
#else
      setvbuf(st, NULL, _IOFBF, 64*1024);
#endif
   }
   return st;
}

extern "C" U32_WMF WMFWRITE(const void *buffer, U32_WMF size, U32_WMF count, HWMFFILE stream )
{  FILE * st;
   st = (FILE *) stream;
   return fwrite(buffer, size, count, st);
}

extern "C" I64_WMF WMFSEEK( HWMFFILE stream, U64_WMF offset, I32_WMF origin )
{  FILE * st;
   st = (FILE *) stream;
   return fseek(st, (long)offset, origin);
}

extern "C" U64_WMF WMFTELL( HWMFFILE stream)
{  FILE * st;
   st = (FILE *) stream;
   return ftell(st);
}

extern "C" I32_WMF WMFCLOSE( HWMFFILE  stream)
{  FILE * st;
   st = (FILE *) stream;
   return fclose(st);
}
