
#ifndef __TRANSPRT_H__
#define __TRANSPRT_H__

//******************************************************************************
// Preprocessor definitions
//******************************************************************************

#define PORT_BROADCAST                  2201

#define PACKETID_SC_SUCCESS             0xFFFFFFFF
#define PACKETID_SC_FAULT               0xFFFFFFFE
#define PACKETID_SC_DEVICEAFFINITY      0xFFFFFFFD
#define PACKETID_SC_REFFRAMECRC32       0xFFFFFFFC
#define PACKETID_SC_REFFRAME            0xFFFFFFFB
#define PACKETID_CS_CHECKDEVICEAFFINITY 0xFFFFFFFA
#define PACKETID_CS_DIRECT3DCREATE8     0xFFFFFFF9
#define PACKETID_CS_GETREFFRAMECRC32    0xFFFFFFF8
#define PACKETID_CS_GETREFFRAME         0xFFFFFFF7
#define PACKETID_CS_GETKEYSTATE         0xFFFFFFF6

// Number of distinct classes requiring translation during marshalling (i.e. pixel shader handles)
#define TRANSLATION_TYPES               4

#define __ADDRESS                       0   // Object in memory
#define __VSHADER                       1   // Vertex shader handle
#define __PSHADER                       2   // Pixel shader handle
#define __SBLOCK                        3   // State block handle
#define __DATA                          9

#define _ADDRESS                        "0"
#define _VSHADER                        "1"
#define _PSHADER                        "2"
#define _SBLOCK                         "3"
#define _DATA                           "9"

#define GET_TRANSTYPE(t)                (t - '0')

// IN   - DWORD given to be passed in.  DWORD in, nothing out.
// DEL  - DWORD given to be passed in.  DWORD in, nothing out.  Cannot be used with _DATA
// PIN  - LPDWORD given pointing to buffer followed by DWORD containing buffer size.  DWORD offset and buffer in, nothing out.  Can only be used with _DATA
// POUT - LPDWORD given pointing to buffer to be filled followed by DWORD containing buffer size.  DWORD buffer size in.  DWORD offset and buffer out.  Can only be used with _DATA // RET(p) - LPDWORD given to be filled with return value followed by DWORD given with return value size.  Size in, DWORD with offset and buffer out.
// OUT  - LPDWORD given to be filled with a DWORD value on return.  Nothing in, returned DWORD value out.  _OUT cannot be used to transmit a NULL DWORD pointer for invalid parameter checking...if it is desired to do so, use a _POUT to send the NULL pointer across.  // RET(s) - LPDWORD given to be filled with return value.  Nothing in, DWORD out.
// ADD  - LPDWORD given to be filled with a DWORD value on return and containing translation DWORD on entry.  Translation DWORD in, returned DWORD value out.  Cannot be used with _DATA
// BI   - LPDWORD given to be filled with a DWORD value on return and containing a DWORD on entry.  DWORD in, returned DWORD value out.  Can only be used with _DATA.  Cannot be used with a NULL pointer to the bidirectional parameter.

#define _IN(t)       "i" t
#define _DEL(t)      "l" t
#define _PIN(t)      "n" t
#define _POUT(t)     "t" t
#define _OUT(t)      "o" t
#define _ADD(t)      "a" t
#define _BI(t)       "b" t
#define _VOID        ""

#define __IN         'i'
#define __PIN        'n'
#define __OUT        'o'
#define __POUT       't'
#define __ADD        'a'
#define __DEL        'l'
#define __BI         'b'

//******************************************************************************
// Packet Data Types and Structures
//******************************************************************************

typedef DWORD TRANSTYPE;

typedef DWORD PACKETID;

// Method identifiers (supplied as the packet ID)
#define SIG_MACRO(f,s)  MID_##f,
typedef enum _METHODID {
#include "sigs.h"
} METHODID, *PMETHODID;

// Enumeration identifier
typedef struct _ENUMERATION_ID {
    DWORD                       dwID[4];
    _ENUMERATION_ID()           {dwID[0] = 0x357fe46c; dwID[1] = 0x520a54d7; 
                                dwID[2] = 0x221bc940; dwID[3] = 0x9823E1CC;}
} ENUMERATION_ID, *PENUMERATION_ID;

// Enumeration packet
typedef struct _PACKET_ENUM {
    ENUMERATION_ID              enumid;
    IN_ADDR                     s_addrClient;
    UINT                        uPort;
} PACKET_ENUM, *PPACKET_ENUM;

// Packet data header
typedef struct _PACKET {
    DWORD                       dwSize;
    PACKETID                    packetID;
} PACKET, *PPACKET;

// Packet used to request device affinity from the server
typedef struct _PACKET_CS_CHECKDEVICEAFFINITY : public PACKET {
    LPVOID                      pvDirect3D;
    _PACKET_CS_CHECKDEVICEAFFINITY() {dwSize = sizeof(PACKET_CS_CHECKDEVICEAFFINITY);
                                packetID = PACKETID_CS_CHECKDEVICEAFFINITY;}
} PACKET_CS_CHECKDEVICEAFFINITY, *PPACKET_CS_CHECKDEVICEAFFINITY;

// Packet used to initiate a call to Direct3DCreate8
typedef struct _PACKET_CS_DIRECT3DCREATE8 : public PACKET {
    LPVOID                      pvDirect3D;
    _PACKET_CS_DIRECT3DCREATE8() {dwSize = sizeof(PACKET_CS_DIRECT3DCREATE8);
                                packetID = PACKETID_CS_DIRECT3DCREATE8;}
} PACKET_CS_DIRECT3DCREATE8, *PPACKET_CS_DIRECT3DCREATE8;

// Packet used to initiate a call to one of the methods of Direct3D
typedef struct _PACKET_CS_CALLMETHOD : public PACKET {
    LPVOID                      pvObject;
    DWORD                       pdwArguments[1];
} PACKET_CS_CALLMETHOD, *PPACKET_CS_CALLMETHOD;

// Packet used to request a CRC of the frame buffer
typedef struct _PACKET_CS_GETREFFRAMECRC32 : public PACKET {
    LPVOID                      pvDevice;
    _PACKET_CS_GETREFFRAMECRC32() {dwSize = sizeof(PACKET_CS_GETREFFRAMECRC32);
                                packetID = PACKETID_CS_GETREFFRAMECRC32;}
} PACKET_CS_GETREFFRAMECRC32, *PPACKET_CS_GETREFFRAMECRC32;

// Packet used to request the contents of the frame buffer
typedef struct _PACKET_CS_GETREFFRAME : public PACKET {
    LPVOID                      pvDevice;
    _PACKET_CS_GETREFFRAME()    {dwSize = sizeof(PACKET_CS_GETREFFRAME);
                                packetID = PACKETID_CS_GETREFFRAME;}
} PACKET_CS_GETREFFRAME, *PPACKET_CS_GETREFFRAME;

// Packet used to return the results from a call to one of the methods of Direct3D
typedef struct _PACKET_SC_METHODRESULT : public PACKET {
    DWORD                       pdwResult[1];
} PACKET_SC_METHODRESULT, *PPACKET_SC_METHODRESULT;

// Generic packet used to indicate a successful operation
typedef struct _PACKET_SC_SUCCESS : public PACKET {
    _PACKET_SC_SUCCESS()        {dwSize = sizeof(PACKET_SC_SUCCESS);
                                packetID = PACKETID_SC_SUCCESS;}
} PACKET_SC_SUCCESS, *PPACKET_SC_SUCCESS;

// Generic packet used to indicate an unspecified fault
typedef struct _PACKET_SC_FAULT : public PACKET {
    _PACKET_SC_FAULT()          {dwSize = sizeof(PACKET_SC_FAULT);
                                packetID = PACKETID_SC_FAULT;}
} PACKET_SC_FAULT, *PPACKET_SC_FAULT;

// Packet used to inidicate whether or not the video hardware on the server
// will produce rasterized images identical to that of the NV2A
typedef struct _PACKET_SC_DEVICEAFFINITY : public PACKET {
    BOOL                        bEquivalent;
    _PACKET_SC_DEVICEAFFINITY() {dwSize = sizeof(PACKET_SC_DEVICEAFFINITY);
                                packetID = PACKETID_SC_DEVICEAFFINITY;}
} PACKET_SC_DEVICEAFFINITY, *PPACKET_SC_DEVICEAFFINITY;

// Packet used to return the CRC of the frame buffer
typedef struct _PACKET_SC_REFFRAMECRC32 : public PACKET {
    DWORD                       dwRefCRC;
    _PACKET_SC_REFFRAMECRC32()  {dwSize = sizeof(PACKET_SC_REFFRAMECRC32);
                                packetID = PACKETID_SC_REFFRAMECRC32;}
} PACKET_SC_REFFRAMECRC32, *PPACKET_SC_REFFRAMECRC32;

// Packet used to return the contents of the frame buffer
typedef struct _PACKET_SC_REFFRAME : public PACKET {
    BYTE                        pBits[1];
    _PACKET_SC_REFFRAME()       {dwSize = sizeof(PACKET_SC_REFFRAME);
                                packetID = PACKETID_SC_REFFRAME;}
} PACKET_SC_REFFRAME, *PPACKET_SC_REFFRAME;

#endif // __TRANSPRT_H__
