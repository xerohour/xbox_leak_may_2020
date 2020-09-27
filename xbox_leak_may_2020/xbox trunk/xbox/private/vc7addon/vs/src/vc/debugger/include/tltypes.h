

// OSD-based types for Transport Layer
// Split our from odtypes.h so that the shell can use it

typedef
LONG (OSDAPI *
LPGETSETPROFILEPROC)(
    LPTSTR      KeyName,        // SubKey name (must be relative)
    LPTSTR      ValueName,      // value name
    DWORD*      dwType,         // type of data (valid only in the case of Set)
    BYTE*       Data,           // pointer to data
    DWORD       cbData,         // size of data (in bytes)
    BOOL        fSet            // TRUE = setting, FALSE = getting
    );

//
// Transport Layer Management
//

#define TLISINFOSIZE 80
typedef struct _TLIS {
    DWORD fCanSetup;
    DWORD dwMaxPacket;
    DWORD dwOptPacket;
    DWORD dwInfoSize;
    DWORD fRemote;
    MPT   mpt;
    MPT   mptRemote;
    TCHAR rgchInfo[TLISINFOSIZE];
} TLIS;
typedef TLIS * LPTLIS;

typedef struct _TLSS {
    DWORD fLoad;
    DWORD fInteractive;
    DWORD fSave;
    LPVOID lpvPrivate;
    LPARAM lParam;
    LPGETSETPROFILEPROC lpfnGetSet;
    MPT mpt;
    BOOL fRMAttached;
} TLSS;
typedef TLSS * LPTLSS;
