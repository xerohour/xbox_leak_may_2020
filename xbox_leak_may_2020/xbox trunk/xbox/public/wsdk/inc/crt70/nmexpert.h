//=============================================================================
//  Microsoft (R) Network Monitor (tm). 
//  Copyright (C) 1997-1999. All rights reserved.
//
//  MODULE: NMExpert.h
//
//  Structures and definitions needed in Experts
//=============================================================================

#ifndef _NMEXPERT_H_
#define _NMEXPERT_H_

// we say zero length arrays are just fine...
#pragma warning(disable:4200)

// all of the stuff that follows is not C++ specific
#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

#define EXPERTSTRINGLENGTH  MAX_PATH
#define EXPERTGROUPNAMELENGTH 25

// HEXPERTKEY tracks running experts. It is only used by experts for 
// self reference. It refers to a RUNNINGEXPERT (an internal only structure)..
typedef LPVOID HEXPERTKEY;
typedef HEXPERTKEY * PHEXPERTKEY;

// HEXPERT tracks loaded experts. It refers to an EXPERTENUMINFO.
typedef LPVOID HEXPERT;
typedef HEXPERT * PHEXPERT;

// HRUNNINGEXPERT tracks a currently running expert.
// It refers to a RUNNINGEXPERT (an internal only structure).
typedef LPVOID HRUNNINGEXPERT;
typedef HRUNNINGEXPERT * PHRUNNINGEXPERT;

// HGROUP tracks a group. It refers to a GROUPLIST (an internal only structure).
typedef LPVOID HGROUP;
typedef HGROUP * PHGROUP;

// forward ref
typedef struct _EXPERTENUMINFO * PEXPERTENUMINFO;
typedef struct _EXPERTCONFIG   * PEXPERTCONFIG;
typedef struct _EXPERTSTARTUPINFO * PEXPERTSTARTUPINFO;

// Definitions needed to call experts
#define EXPERTENTRY_REGISTER      "Register"
#define EXPERTENTRY_CONFIGURE     "Configure"
#define EXPERTENTRY_RUN           "Run"
typedef BOOL (WINAPI * PEXPERTREGISTERPROC)( PEXPERTENUMINFO );
typedef BOOL (WINAPI * PEXPERTCONFIGPROC)  ( HEXPERTKEY, PEXPERTCONFIG*, PEXPERTSTARTUPINFO, DWORD, HWND );
typedef BOOL (WINAPI * PEXPERTRUNPROC)     ( HEXPERTKEY, PEXPERTCONFIG, PEXPERTSTARTUPINFO, DWORD, HWND);


// EXPERTENUMINFO describes an expert that NetMon has loaded from disk. 
// It does not include any configuration or runtime information.
typedef struct _EXPERTENUMINFO
{
    char      szName[EXPERTSTRINGLENGTH];
    char      szVendor[EXPERTSTRINGLENGTH];
    char      szDescription[EXPERTSTRINGLENGTH];
    DWORD     Version;    
    DWORD     Flags;
    char      szDllName[MAX_PATH];      // private, dont' touch
    HEXPERT   hExpert;                  // private, don't touch
    HINSTANCE hModule;                  // private, don't touch
    PEXPERTREGISTERPROC pRegisterProc;  // private, don't touch
    PEXPERTCONFIGPROC   pConfigProc;    // private, don't touch
    PEXPERTRUNPROC      pRunProc;       // private, don't touch
} EXPERTENUMINFO;
typedef EXPERTENUMINFO * PEXPERTENUMINFO;

#define EXPERT_ENUM_FLAG_CONFIGURABLE                0x0001   // Expert dll contains configure export function 
#define EXPERT_ENUM_FLAG_VIEWER_PRIVATE              0x0002   // Expert dll wants private viewer (default is shared)
#define EXPERT_ENUM_FLAG_NO_VIEWER                   0x0004   // Expert dll does not want an associated viewer
#define EXPERT_ENUM_FLAG_ADD_ME_TO_RMC_IN_SUMMARY    0x0010   // Expert dll wants to be on the RMC menu in the summary pane
#define EXPERT_ENUM_FLAG_ADD_ME_TO_RMC_IN_DETAIL     0x0020   // Expert dll wants to be on the RMC menu in the detail pane

// GROUPINFO
// This is an accociation between a group name and its handle.
typedef struct
{
    char        szGroupName[EXPERTGROUPNAMELENGTH+1];
    HGROUP  hGroup;
} GROUPINFO;
typedef GROUPINFO * PGROUPINFO;

// EXPERTSTARTUPINFO
// This gives the Expert an indication of where he came from.
typedef struct _EXPERTSTARTUPINFO
{
    DWORD           Flags;
    HCAPTURE        hCapture;
    char            szCaptureFile[MAX_PATH];
    DWORD           dwFrameNumber;
    HPROTOCOL       hProtocol;

    // note: the expert launch code has locked the starting frame
    // so this data will not go out of scope.
    LPPROPERTYINST  lpPropertyInst;

    // if the lpPropertyInst->PropertyInfo->DataQualifier == PROP_QUAL_FLAGS
    // the the following structure is filled in
    struct
    {
        BYTE    BitNumber;
        BOOL    bOn;
    } sBitfield;

} EXPERTSTARTUPINFO;

// EXPERTCONFIG
// This is a generic holder for an Expert's config data.
typedef struct  _EXPERTCONFIG
{
    DWORD   RawConfigLength;
    BYTE    RawConfigData[0];

} EXPERTCONFIG;
typedef EXPERTCONFIG * PEXPERTCONFIG;

// CONFIGUREDEXPERT
// This structure associates a loaded expert with its configuration data.
typedef struct
{
    HEXPERT         hExpert;
    DWORD           StartupFlags;
    PEXPERTCONFIG   pConfig;
} CONFIGUREDEXPERT;
typedef CONFIGUREDEXPERT * PCONFIGUREDEXPERT;

// EXPERTFRAMEDESCRIPTOR - passed back to the expert to fulfil the request for a frame
typedef struct
{
    DWORD                FrameNumber;         // Frame Number.
    HFRAME               hFrame;              // Handle to the frame.
    ULPFRAME             pFrame;              // pointer to frame.
    LPRECOGNIZEDATATABLE lpRecognizeDataTable;// pointer to table of RECOGNIZEDATA structures.
    LPPROPERTYTABLE      lpPropertyTable;     // pointer to property table.
} EXPERTFRAMEDESCRIPTOR;
typedef EXPERTFRAMEDESCRIPTOR * LPEXPERTFRAMEDESCRIPTOR;

// other definitions
#define GET_SPECIFIED_FRAME              0
#define GET_FRAME_NEXT_FORWARD           1
#define GET_FRAME_NEXT_BACKWARD          2

#define FLAGS_DEFER_TO_UI_FILTER       0x1
#define FLAGS_ATTACH_PROPERTIES        0x2

// EXPERTSTATUSENUM
// gives the possible values for the status field in the EXPERTSTATUS structure
typedef enum
{
    EXPERTSTATUS_INACTIVE = 0,
    EXPERTSTATUS_STARTING,
    EXPERTSTATUS_RUNNING,
    EXPERTSTATUS_PROBLEM,
    EXPERTSTATUS_ABORTED,  
    EXPERTSTATUS_DONE,  
} EXPERTSTATUSENUMERATION;

// EXPERTSUBSTATUS bitfield 
// gives the possible values for the substatus field in the EXPERTSTATUS structure
#define  EXPERTSUBSTATUS_ABORTED_USER          0x0001   // User aborted expert dll
#define  EXPERTSUBSTATUS_ABORTED_LOAD_FAIL     0x0002   // Netmon could not load expert dll
#define  EXPERTSUBSTATUS_ABORTED_THREAD_FAIL   0x0004   // Netmon could not start dll thread
#define  EXPERTSUBSTATUS_ABORTED_BAD_ENTRY     0x0008   // Netmon could not find expert dll entry point 

// EXPERTSTATUS
// Indicates the current status of a running expert.
typedef struct
{                                                          
    EXPERTSTATUSENUMERATION   Status;
    DWORD                     SubStatus;
    DWORD                     PercentDone;
    DWORD                     Frame; 
    char                      szStatusText[EXPERTSTRINGLENGTH];
} EXPERTSTATUS;
typedef EXPERTSTATUS * PEXPERTSTATUS;               


// EXPERT STARTUP FLAGS
#define EXPERT_STARTUP_FLAG_USE_STARTUP_DATA_OVER_CONFIG_DATA   0x00000001

#ifdef __cplusplus
}
#endif // __cplusplus

// turn this warning back on
#pragma warning(default:4200)

#endif // _NMEXPERT_H_