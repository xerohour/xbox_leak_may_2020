#include "precomp.h"

VOID HostToWireFormat16(WORD wHostFormat, PBYTE pWireFormat)
{
    *((PBYTE)(pWireFormat)+0) = (BYTE)((DWORD)(wHostFormat) >> 8);
    *((PBYTE)(pWireFormat)+1) = (BYTE)(wHostFormat);
}

VOID HostToWireFormat32(DWORD dwHostFormat, PBYTE pWireFormat)
{
    *((PBYTE)(pWireFormat)+0) = (BYTE)((DWORD)(dwHostFormat) >> 24);
    *((PBYTE)(pWireFormat)+1) = (BYTE)((DWORD)(dwHostFormat) >> 16);
    *((PBYTE)(pWireFormat)+2) = (BYTE)((DWORD)(dwHostFormat) >>  8);
    *((PBYTE)(pWireFormat)+3) = (BYTE)(dwHostFormat);
}

WORD WireToHostFormat16(PBYTE pWireFormat)
{
    WORD wHostFormat = ((*((PBYTE)(pWireFormat)+0) << 8) +
            (*((PBYTE)(pWireFormat)+1)));
    return wHostFormat;
}

DWORD WireToHostFormat32(PBYTE pWireFormat)
{
    DWORD dwHostFormat = ((*((PBYTE)(pWireFormat)+0) << 24) +
            (*((PBYTE)(pWireFormat)+1) << 16) +
            (*((PBYTE)(pWireFormat)+2) << 8)  +
            (*((PBYTE)(pWireFormat)+3) ));

    return dwHostFormat;
}

