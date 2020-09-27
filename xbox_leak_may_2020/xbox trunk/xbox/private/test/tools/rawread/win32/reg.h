#ifndef _INC_REG
#define _INC_REG

#include <windows.h>
#include "..\defs.h"
#include "..\image.h"

#define REG_KEY_PARENT          HKEY_LOCAL_MACHINE
#define REG_KEY                 "Software\\Microsoft\\"APPTITLE

#define REG_VAL_DRIVE           "Drive"
#define REG_TYPE_DRIVE          REG_BINARY

#define REG_VAL_FILE            "File"
#define REG_TYPE_FILE           REG_SZ

#define REG_VAL_MAXFRAGSIZE     "Maximum Fragment Size"
#define REG_TYPE_MAXFRAGSIZE    REG_BINARY

#define REG_VAL_WINDOWPOS       "Window Position"
#define REG_TYPE_WINDOWPOS      REG_BINARY

#define REG_VAL_OPTIONS         "Options"
#define REG_TYPE_OPTIONS        REG_BINARY

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

extern void GetRegWindowPos(HWND);
extern void PutRegWindowPos(HWND);
extern void GetRegOptions(LPIMAGESTRUCT lpImage);
extern void SetRegOptions(LPIMAGESTRUCT lpImage);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // _INC_REG