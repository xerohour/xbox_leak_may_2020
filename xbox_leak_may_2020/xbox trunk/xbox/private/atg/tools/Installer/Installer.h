//-----------------------------------------------------------------------------
// File: Installer.h
//
// Desc: 
//
// Hist: 04.18.01 - New for CliffG
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "scsi.h"
#include "ntddscsi.h"

typedef struct _LSA_STRING {
    USHORT Length;
    USHORT MaximumLength;
    PCHAR Buffer;
} LSA_STRING, *PLSA_STRING;
 
typedef LSA_STRING STRING, *PSTRING ;

typedef struct _CSTRING {
    USHORT Length;
    USHORT MaximumLength;
    CONST char *Buffer;
} CSTRING;
typedef CSTRING *PCSTRING;
#define ANSI_NULL ((CHAR)0)     // winnt

typedef STRING CANSI_STRING;
typedef PSTRING PCANSI_STRING;


typedef STRING OBJECT_STRING;
typedef CSTRING COBJECT_STRING;
typedef PSTRING POBJECT_STRING;
typedef PCSTRING PCOBJECT_STRING;
typedef CHAR OCHAR;
typedef CHAR* POCHAR;
typedef PSTR POSTR;
typedef PCSTR PCOSTR;

typedef struct _OBJECT_ATTRIBUTES {
    HANDLE RootDirectory;
    POBJECT_STRING ObjectName;
    ULONG Attributes;
} OBJECT_ATTRIBUTES;
typedef OBJECT_ATTRIBUTES *POBJECT_ATTRIBUTES;

// 
// NTSTATUS
//
 
typedef LONG NTSTATUS;

typedef NTSTATUS *PNTSTATUS;
 
//
//  Status values are 32 bit values layed out as follows:
//
//   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
//   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
//  +---+-+-------------------------+-------------------------------+
//  |Sev|C|       Facility          |               Code            |
//  +---+-+-------------------------+-------------------------------+
//
//  where
//
//      Sev - is the severity code
//
//          00 - Success
//          01 - Informational
//          10 - Warning
//          11 - Error
//
//      C - is the Customer code flag
//
//      Facility - is the facility code
//
//      Code - is the facility's status code
//
 
//
// Generic test for success on any status value (non-negative numbers
// indicate success).
//
 
#define NT_SUCCESS(Status) ((NTSTATUS)(Status) >= 0)
 
// begin_ntddk begin_wdm begin_nthal
//
// Define the base asynchronous I/O argument types
//
 
typedef struct _IO_STATUS_BLOCK {
    union {
        NTSTATUS Status;
        PVOID Pointer;
    };
 
    ULONG_PTR Information;
} IO_STATUS_BLOCK, *PIO_STATUS_BLOCK;

typedef char OCHAR;
#define __OTEXT(quote) quote
#define OTEXT(quote) __OTEXT(quote)

extern "C"
{
NTSYSCALLAPI
NTSTATUS
NTAPI
IoCreateSymbolicLink(
    IN POBJECT_STRING SymbolicLinkName,
    IN POBJECT_STRING DeviceName
    );
}

#define CONSTANT_OBJECT_STRING(s)   { sizeof( s ) - sizeof( OCHAR ), sizeof( s ), s }

#define OBJ_CASE_INSENSITIVE    0x00000040L

#define InitializeObjectAttributes( p, n, a, r, s ) { \
    (p)->RootDirectory = r;                             \
    (p)->Attributes = a;                                \
    (p)->ObjectName = n;                                \
    }

extern "C"
{
NTSYSCALLAPI
NTSTATUS
NTAPI
NtOpenFile(
    OUT PHANDLE FileHandle,
    IN ACCESS_MASK DesiredAccess, // SYNCHRONIZE | FILE_READ_DATA
    IN POBJECT_ATTRIBUTES ObjectAttributes,
    OUT PIO_STATUS_BLOCK IoStatusBlock,
    IN ULONG ShareAccess, // 0
    IN ULONG OpenOptions // FILE_SYNCHRONOUS_IO_ALERT | FILE_NO_INTERMEDIATE_BUFFERING
    );
}

#define FILE_SYNCHRONOUS_IO_ALERT               0x00000010

//
// Define values for pass-through DataIn field.
//

#define SCSI_IOCTL_DATA_OUT          0
#define SCSI_IOCTL_DATA_IN           1
#define SCSI_IOCTL_DATA_UNSPECIFIED  2

//
// Default request sense buffer size
//

#define SENSE_BUFFER_SIZE 18

//
// Sense codes
//

#define SCSI_SENSE_NO_SENSE         0x00
#define SCSI_SENSE_RECOVERED_ERROR  0x01
#define SCSI_SENSE_NOT_READY        0x02
#define SCSI_SENSE_MEDIUM_ERROR     0x03
#define SCSI_SENSE_HARDWARE_ERROR   0x04
#define SCSI_SENSE_ILLEGAL_REQUEST  0x05
#define SCSI_SENSE_UNIT_ATTENTION   0x06
#define SCSI_SENSE_DATA_PROTECT     0x07
#define SCSI_SENSE_BLANK_CHECK      0x08
#define SCSI_SENSE_UNIQUE           0x09
#define SCSI_SENSE_COPY_ABORTED     0x0A
#define SCSI_SENSE_ABORTED_COMMAND  0x0B
#define SCSI_SENSE_EQUAL            0x0C
#define SCSI_SENSE_VOL_OVERFLOW     0x0D
#define SCSI_SENSE_MISCOMPARE       0x0E
#define SCSI_SENSE_RESERVED         0x0F

//
// Macro definition for defining IOCTL and FSCTL function control codes.  Note
// that function codes 0-2047 are reserved for Microsoft Corporation, and
// 2048-4095 are reserved for customers.
//

#define CTL_CODE( DeviceType, Function, Method, Access ) (                 \
    ((DeviceType) << 16) | ((Access) << 14) | ((Function) << 2) | (Method) \
)

#define FILE_DEVICE_CONTROLLER          0x00000004

#define FILE_ANY_ACCESS                 0
#define FILE_SPECIAL_ACCESS    (FILE_ANY_ACCESS)
#define FILE_READ_ACCESS          ( 0x0001 )    // file & pipe
#define FILE_WRITE_ACCESS         ( 0x0002 )    // file & pipe

//
// Define the method codes for how buffers are passed for I/O and FS controls
//

#define METHOD_BUFFERED                 0
#define METHOD_IN_DIRECT                1
#define METHOD_OUT_DIRECT               2
#define METHOD_NEITHER                  3

#define ocslen      lstrlenA
#define ocscpy      lstrcpyA
#define _ocsicmp _stricmp
#define lstrcpynO lstrcpynA

#define ARRAYSIZE(a) (sizeof(a) / sizeof(a[0]))
ULONG g_FilesCopied = 0;

#define APPROX_FILE_COUNT   1350

const POSTR g_RootDirExceptions[] =
{
    OTEXT("default.xbe"),
    OTEXT("RECMEDIA"),
    OTEXT("XDASH")
};



//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------

COBJECT_STRING CDrive =            CONSTANT_OBJECT_STRING( OTEXT("\\??\\C:") );
COBJECT_STRING HdPartition1 =      CONSTANT_OBJECT_STRING( OTEXT("\\Device\\Harddisk0\\partition1") );
COBJECT_STRING VDrive =            CONSTANT_OBJECT_STRING( OTEXT("\\??\\V:") );
COBJECT_STRING DVDDevice =         CONSTANT_OBJECT_STRING( OTEXT("\\Device\\Cdrom0") );

// Threshold for analog buttons
#define XINPUT_ANALOGBUTTONTHRESHOLD 20






//-----------------------------------------------------------------------------
// Name: EjectDVD()
// Desc: 
//-----------------------------------------------------------------------------
VOID EjectDVD();



//-----------------------------------------------------------------------------
// Name: CopyFiles()
// Desc: Copies files from a source path to a destination path. The source path
//       is a filter that can be passed to FindFirstFile() and is currently
//       required to end in "<\\*>".  The destination path should end with a 
//       trailing backslash ('\')
//-----------------------------------------------------------------------------
BOOL CopyFiles( IN PCOSTR pcszSrcPath, IN PCOSTR pcszDestPath,
                IN BOOL fRootDir );


