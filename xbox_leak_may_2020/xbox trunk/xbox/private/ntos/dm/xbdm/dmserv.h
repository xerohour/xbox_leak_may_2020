/*
 *
 * dmserv.h
 *
 * Random stuff for the server
 *
 */

#define MAX_CONNECTIONS 4
#define MAX_CONN_BUF 512
#define CONN_BINARY 0x80000000
#define CONN_OVERFLOW 0x40000000
#define CONN_BINSEND 0x20000000
#define CONN_BINERROR 0x10000000
#define CONN_MULTILINE 0x08000000
#define CONN_DHKEY 0x02000000
#define CONN_RECEIVED 0xffff
#define DISK_BUF_SIZE 0x4000

/* Command continue stuff */
typedef struct _CCS {
    union {
        FILE_NETWORK_OPEN_INFORMATION fba;
        struct {
            HANDLE h;
            int i;
        };
        BYTE *pb;
        DM_THREADINFO dmti;
        DM_XTLDATA dmxd;
        DM_COUNTDATA dmcd;
        struct {
            HANDLE hFile;
            BYTE *rgbBuf;
            int ib;
            BOOL fError:1,
            fDidSize:1,
            unused:30;
        };
        PDM_WALK_MODULES pdmwm;
        PDM_WALK_MODSECT pdmws;
        PDM_WALK_COUNTERS pdmwc;
        PDM_WALK_USERS pdmwu;
        struct {
            PDM_XBE pxbe;
            int iiXbe;
        };
        struct {
            PBYTE  ImageBuffer;
            SIZE_T ImageSize;
            SIZE_T BytesReceived;
            DWORD  Crc;
            BOOL   IgnoreVersionChecking;
        } KernelImage;
        struct {
            PBYTE FileBuffer;
            HANDLE FileHandle;
            OBJECT_STRING FileName;
            DWORD Crc, ExpectedCrc;
            SIZE_T BytesReceived;
        } sysupd;
        struct {
            DWORD Width, Height, Pitch, Format;
            BOOL fFirstTime;
            BYTE *pbyFrameBuffer;
        } Screenshot;
    };
} CCS;

/* Extended command handler */
typedef struct _ECH {
    char szExtName[64];
    PDM_CMDPROC pfnHandler;
    HANDLE (*pfnCreateThread)();
} ECH, *PECH;

#define MAX_ECH 16

/* Command handler */
typedef struct _CHH {
    LPCSTR szCommand;
    DWORD dmplMask;
    PDM_CMDPROC pfnHandler;
    DWORD dmplCombined;
    DWORD dwFlags;
} CHH, *PCHH;

#define CHH_ANYTHREAD 1

/* Connection state */
typedef struct _CST {
    SOCKET s;
    DWORD dwStatus;
    DWORD dmplCur;
    PECH pechDedicated;
    PETHREAD pthrDedicated;
    ULARGE_INTEGER luDHKey;
    ULARGE_INTEGER luConnectNonce;
    char szBuf[MAX_CONN_BUF];
    DM_CMDCONT dmcc;
    CCS ccs;
} CST, *PCST;
