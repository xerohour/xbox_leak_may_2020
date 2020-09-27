#ifndef _INC_FILE
#define _INC_FILE

#include "defs.h"
#include "stdio.h"

#define INVALID_FILE_HANDLE     ((FILEHANDLE)-1)

#define OPEN_ACCESS_READONLY    0x0000
#define OPEN_ACCESS_WRITEONLY   0x0001
#define OPEN_ACCESS_READWRITE   0x0002

#define OPEN_SHARE_COMPAT       0x0000
#define OPEN_SHARE_DENYREADWRITE 0x0010
#define OPEN_SHARE_DENYWRITE    0x0020
#define OPEN_SHARE_DENYREAD     0x0030
#define OPEN_SHARE_DENYNONE     0x0040

#define OPEN_FLAGS_NOINHERIT    0x0080
#define OPEN_FLAGS_NOCRIT_ERR   0x2000
#define OPEN_FLAGS_COMMIT       0x4000

#define ATTR_NORMAL             0x0000
#define ATTR_READONLY           0x0001
#define ATTR_HIDDEN             0x0002
#define ATTR_SYSTEM             0x0004
#define ATTR_ARCHIVE            0x0020

#define ACTION_FILE_OPEN        0x0001
#define ACTION_FILE_TRUNCATE    0x0002
#define ACTION_FILE_CREATE      0x0010 

#define ACTION_OPENED           0x0001
#define ACTION_CREATED_OPENED   0x0002
#define ACTION_REPLACED_OPENED  0x0003
 
#ifndef WIN32

#define ERROR_INVALID_FUNCTION  0x0001
#define ERROR_FILE_NOT_FOUND    0x0002
#define ERROR_PATH_NOT_FOUND    0x0002
#define ERROR_TOO_MANY_OPEN_FILES 0x0004
#define ERROR_ACCESS_DENIED     0x0005

#endif // WIN32

#ifdef __cplusplus
extern "C" {
#endif

// Prototypes
FILEHANDLE FileOpen(LPSTR, WORD, WORD, WORD);
DWORD FileRead(FILEHANDLE, LPVOID, DWORD);
DWORD FileWrite(FILEHANDLE, LPVOID, DWORD);
void FileClose(FILEHANDLE);
void FileCommit(FILEHANDLE);
DWORD FileGetPos(FILEHANDLE);
DWORD FileSetPos(FILEHANDLE, long, BYTE);

#ifdef __cplusplus
}
#endif

#endif // _INC_FILE