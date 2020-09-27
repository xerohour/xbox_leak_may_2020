

#if !defined(AMC_MS_EMU_API__H)
#define AMC_MS_EMU_API__H


#include <WINDOWS.H>
#include "AmcMsEmuApiError.h"
#define _NTSCSI_USER_MODE_
//#include <SCSI.H>   // Get CDB decleared

// LOG:PL 10/18/01
// 1. Added TimeStamp member variable.

typedef struct _ATAPI_TRANSACTION {
    union {
        CDB Cdb;
        UCHAR CommandData[12];
    } CommandBlock;
    ULARGE_INTEGER TimeStamp;
    PUCHAR Data;
    ULONG DataSize;
    PUCHAR Response;
    ULONG ResponseSize;
    ULARGE_INTEGER ResponseDelay;
    HANDLE ResponseEvent;
} ATAPI_TRANSACTION, *PATAPI_TRANSACTION;

#define HANDLER_RETURN_DATA 1
#define HANDLER_RETURN_ERROR 2

// LOG:PL 10/29/01
// 1. Added IDE_IDENTITY_DATA 

typedef union _IDE_IDENTITY_DATA {
    USHORT IdentityData[256];
    struct {
        USHORT Config;             // word 0
        USHORT Reserved1;          // word 1
        USHORT UniqueConfig;       // word 2
        USHORT Reserved3[7];       // word 3-9
        UCHAR  Serial[20];         // word 10-19
        USHORT Reserved20[3];      // word 20-22
        UCHAR  FirmwareRev[8];     // word 23-26
        UCHAR  ModelNumber[40];    // word 27-46
        USHORT Reserved47[2];      // word 47-48
        USHORT Capabilities;       // word 49
        USHORT Reserved50;         // word 50
        USHORT Obsolete[2];        // word 51-52
        USHORT Reserved53;         // word 53
        USHORT Reserved54[9];      // word 54-62
        USHORT MultiwordDma;       // word 63
        USHORT AdvancedPio;        // word 64
        USHORT CycleTimeDma;       // word 65
        USHORT CycleTimeMultiword; // word 66
        USHORT CycleTimePioNoIordy;// word 67
        USHORT CycleTimePioIordy;  // word 68
        USHORT Reserved69[2];      // word 69-70
        USHORT PacketTime;         // word 71
        USHORT ServiceTime;        // word 72
        USHORT Reserved73[2];      // word 73-74
        USHORT QueueDepth;         // word 75
        USHORT Reserved76[4];      // word 76-79
        USHORT MajorVersion;       // word 80
        USHORT MinorVersion;       // word 81
        USHORT CmdSetsSupported[3];// word 82-84
        USHORT CmdSetsEnabled[3];  // word 85-87
        USHORT UltraDma;           // word 88
        USHORT Reserved89[4];      // word 89-92
        USHORT ResetResult;        // word 93
        USHORT Reserved94[32];     // word 94-125
        USHORT ByteCount0Behavior; // word 126
        USHORT MsnSupport;         // word 127
        USHORT Security;           // word 128
        USHORT Vendor[31];         // word 129-159
        USHORT Reserved160[16];    // word 160-175
        USHORT Reserved176[79];    // word 176-254
        USHORT Integirty;          // word 255
    } PacketDeviceIdentity;
} IDE_IDENTITY_DATA, *PIDE_IDENTITY_DATA;

// LOG:PL 10/30/01
// 1. Added OnEjectButton()
// 2. Renamed ATA_HANDLERS to EMU_HANDLERS

typedef struct _EMU_HANDLERS {
    void  (__stdcall *OnEjectButton)(void);
    void  (__stdcall *OnBusReset)(void);
    ULONG (__stdcall *OnDeviceReset)(void);
    ULONG (__stdcall *OnIdentifyDevice)(PIDE_IDENTIFY_DATA);
    ULONG (__stdcall *OnATAPIPacket)(PATAPI_TRANSACTION);
} EMU_HANDLERS, *PEMU_HANDLERS;


// LOG:PL 10/31/01
// Added new type for states within the libaray

typedef struct _EMU_STATES {
    UCHAR TrayState;
    BOOL  ActivityState;
    BOOL  DmaEnabled;
    UCHAR DmaMode;
} EMU_STATES, *PEMU_STATES;

// LOG:PL 10/24/01
// Added SetDeviceActivityState()

// LOG:PL 10/31/01
// Added GetDeviceState()

HRESULT __stdcall StartEmulationConversation(PEMU_HANDLERS HandlingFunctions);
HRESULT __stdcall SetDeviceTrayState(UCHAR TrayState);
HRESULT __stdcall SetDeviceActivityState(BOOL ActivityState);
HRESULT __stdcall GetDeviceStates(PEMU_STATES DeviceStates);
HRESULT __stdcall EndEmulationConversation(void);

#endif // !AMC_MS_EMU_API__H
// ---------------------- End Of File ---------------------
// --------------------------------------------------------
