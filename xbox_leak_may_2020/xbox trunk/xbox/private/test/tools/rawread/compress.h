#ifndef _INC_COMPRESS
#define _INC_COMPRESS

#include "defs.h"

#ifdef __cplusplus
extern "C" {
#endif

extern BOOL CompressData(LPVOID, WORD, LPVOID, LPWORD);
extern BOOL UncompressData(LPVOID, LPWORD, LPVOID, WORD);

#ifdef __cplusplus
}
#endif

#endif // _INC_COMPRESS