//
// zxenc.h
//
// Define common structs used by Edit and Continue communications between
// the IDE and the C/C++ compiler.
//

#ifndef _ZXENC_INC
#define _ZXENC_INC

//
// Header at the start of the memory-mapped buffer holding the text to
// compile from memory.
//
// [in]   Set by IDE, read by compiler
// [out]  Set by compiler, read by IDE

struct EncMemBufHeader {
    DWORD   dwVersion;      // [in]  version control number
    DWORD   dwOff;          // [in]  offset of text buffer start from header
    DWORD   dwLen;          // [in]  length of text buffer
    DWORD   dwOffRead;      // [out] offset in buffer of PCH read point
    BOOL    bSetProcessWorkingSetSizeOK;
                            // [out] TRUE if SetProcessWorkingSetSize in
                            //       compiler succeeded
    HANDLE  hBufDoneEvent;  // [in]  event fired by compiler when preparing
                            //       to read the PCH, after dwOffRead set
    HANDLE  hKillEvent;     // [in]  event fired by IDE to kill the compile
                            //       after compiler has read PCH and paused
    HANDLE  hContinueEvent; // [in]  event fired by IDE to allow compile to
                            //       continue after pausing once PCH read
};

#define ENC_MEMBUF_VERNO    0x0001

#endif  // _ZXENC_INC
