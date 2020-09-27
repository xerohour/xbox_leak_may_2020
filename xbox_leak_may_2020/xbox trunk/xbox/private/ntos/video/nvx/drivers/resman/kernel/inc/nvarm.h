#if !defined _NVARMH_
#define _NVARMH_

// define to make sure nvrm.h does not include all headers
#if !defined RM_HEADERS_MINIMAL
#define RM_HEADERS_MINIMAL
#endif

#include <nvrm.h>

#define MAX_AUDIO_INSTANCE          16

typedef struct _def_hw_audio_common_ *PHWINFO_COMMON_FIELDS;

typedef union _audioRef
{
    struct
    {
        U032    type    : 16;   // device type
        U032    revID   : 8;    // device instance
        U032    client  : 8;    // unique client ID
    } field;
    
    U032 uValue;
} AUDIO_REF, *PAUDIO_REF;


typedef struct _def_hw_audio_common_
{
    // this structure uniquely identifies a piece of hardware
    // successive calls to allocating the devref just increment
    // the instance counter
    
    U032            uDevType;           // type of device
    U008            uRevisionID;        // revision ID of the device
    U008            uInstanceCounter;   // num of clients using the device
    U032            pDeviceBase;        // PCI mem base
    
    union
    {
        // for NT and WDM
        struct
        {
            VOID    *pInterrupt;
            VOID    *pDeviceObject;     // PDO.. not FDO
        } OsNT;
        
        // for Win9X VxD
        struct
        {
            U032    irqNum;
            U032    irqHandle;
        }Os9X;
    }field;
    
    VOID    *pHalContext;
    
} HWINFO_COMMON_FIELDS;

#endif