// Do *not* ship this code to external developers!
// Microsoft Confidential and private

// Dirty way to get and change title ID

// This code can be useful for testing online access

struct CT // First portion of certification struct
{
    ULONG a;
    ULONG b;
    ULONG dwTitleID;
};
typedef CT* PCT;

struct HDR // First portion of XBE header
{
    ULONG a;
    UCHAR b[256];
    ULONG c;
    ULONG d;
    ULONG e;
    ULONG f;
    ULONG g;
    PCT   ct;
};

// Location of XBE header
#define XeImageHeader() ((struct HDR*)0x00010000)

// Dirty way to change title ID
// XeImageHeader()->ct->dwTitleID = dwTitleID;

// Dirty way to get title ID
// DWORD dwTitleID = XeImageHeader()->ct->dwTitleID;
