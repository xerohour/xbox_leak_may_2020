#ifndef _INC_MRCICODE
#define _INC_MRCICODE

#include "defs.h"

#ifdef __cplusplus
extern "C" {
#endif

extern WORD Mrci1MaxCompress(LPBYTE, WORD, LPBYTE, WORD);
extern WORD Mrci1Decompress(LPBYTE, WORD, LPBYTE, WORD);
extern WORD Mrci2MaxCompress(LPBYTE, WORD, LPBYTE, WORD);
extern WORD Mrci2Decompress(LPBYTE, WORD, LPBYTE, WORD);

#ifdef __cplusplus
}
#endif

#endif // _INC_MRCICODE