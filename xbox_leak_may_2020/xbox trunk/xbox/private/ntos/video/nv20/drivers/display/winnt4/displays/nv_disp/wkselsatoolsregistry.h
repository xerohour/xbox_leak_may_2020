#ifndef _WKSELSATOOLSREGISTRY_H
#define _WKSELSATOOLSREGISTRY_H
//*****************************Module*Header******************************
//
// Module Name: wkselsatoolsregistry.h
//
// FNicklisch 09/14/2000: New, derived from registry.h and others...
//
// This part of code was taken from the code bought from ELSA. Parts of 
// it is used to get the workstation tools, application and features up
// and running. It is implemented on a temporary base and will be replaced
// by a NVIDIA propritary interface as soon as possible.
// Don't care about the code not necessariliy used.
//
// Copyright (c) 1999-2000, ELSA AG, Aachen (Germany)
// Copyright (c) 2000 NVidia Corporation. All Rights Reserved.
//

#ifdef USE_WKS_ELSA_TOOLS
// Errorcodes returned by DrvEscape
#define REG_ERROR   -1
#define REG_OK       0
#define REG_DEFAULT  1

// copies of this structures are located in m_registry.h for the miniport side of this
// information. If values are changed, change them in m_registry.h, too

//*******************************************************************************************
// 131098 MSchwarz 
// structures to transfer data throught the EngDeviceIoControl function 
// between driver and miniport for Registry settings
// therefore 4 IOCTL_VIDEO constants are defined:
// IOCTL_VIDEO_REG_ID_TRANSFER_SET  : set value with key decoded with ID
// IOCTL_VIDEO_REG_SZ_TRANSFER_GET  : get value with key decoded with ID
// IOCTL_VIDEO_REG_ID_TRANSFER_SET  : set value with key given as String
// IOCTL_VIDEO_REG_SZ_TRANSFER_GET  : get value with key given as  String

#define MAX_STRING 260

#pragma warning(disable:4200) // nonstandard extension used : zero-sized array in struct/union
typedef struct tagIOCTL_ID_TRANSFER
{
  ULONG dwId;                 // ELSA_ESC ID to set
  ULONG dwDataSize;           // sizeof( Data ) without sizeof( dwSize / dwType / dwId )
  ULONG dwDataType;           // Type of Data ( REG_SZ, REG_BINARY, etc )
  LONG  lReturnCode;          // REG_OK, REG_DEFAULT, (REG_ERROR) ...
  CHAR  ajData[0];            // complete Data to move
} IOCTL_ID_TRANSFER, *PIOCTL_ID_TRANSFER;


typedef struct tagIOCTL_SZ_TRANSFER
{
  CHAR  szId[MAX_STRING];     // the key in Registry to set
  ULONG dwDataSize;           // sizeof( Data ) without sizeof( dwSize / dwType / dwId )
  ULONG dwDataType;           // Type of Data ( REG_SZ, REG_BINARY, etc )
  LONG  lReturnCode;          // REG_OK, REG_DEFAULT, (REG_ERROR) ...
  CHAR  ajData[0];            // complete Data to move
} IOCTL_SZ_TRANSFER, *PIOCTL_SZ_TRANSFER;
#pragma warning(default:4200) // nonstandard extension used : zero-sized array in struct/union

//*******************************************************************************************


// driver internal registry IDs, reserved are for free temporary use!

// ********************************************************
// query/set values identifier: 
//   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1              Bit No
//   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 
//  +-------+-------+-----------------------------------------------+
//  |o o o o|r r r r|                                          code |
//  +-------+-------+-----------------------------------------------+
//
//  o = bitfield defining OS dependencies
//    0       = no specific OS
//    0x1     = Win9x
//    0x2     = Win NT
//    0x4,0x8 = unused
//  r = bitfield reserved for driver internal stuff.
//    This field must be 0 in all IDs-codes called via ExtEscape!

#define INTERNAL_ET_VALUE_DEFAULT        0x01000000 // Use internal default instead of registry value
#define INTERNAL_ET_VALUE_unused0        0x02000000
#define INTERNAL_ET_VALUE_unused1        0x04000000
#define INTERNAL_ET_VALUE_HIDDEN_KEYS    0x08000000 // BIT to test internaly used keys

#define INTERNAL_ET_VALUE_KEY_ID_ONLY_MASK (~(INTERNAL_ET_VALUE_DEFAULT))

// hidden and internal driver keys, used within the driver only
#define INTERNAL_ET_VALUE_DW_RESERVED0  ((ET_VALUE_OS_WINNT|INTERNAL_ET_VALUE_HIDDEN_KEYS) + 0) // Reserved 0
#define INTERNAL_ET_VALUE_DW_RESERVED1  ((ET_VALUE_OS_WINNT|INTERNAL_ET_VALUE_HIDDEN_KEYS) + 1) // Reserved 1
#define INTERNAL_ET_VALUE_DW_RESERVED2  ((ET_VALUE_OS_WINNT|INTERNAL_ET_VALUE_HIDDEN_KEYS) + 2) // Reserved 2
#define INTERNAL_ET_VALUE_DW_RESERVED3  ((ET_VALUE_OS_WINNT|INTERNAL_ET_VALUE_HIDDEN_KEYS) + 3) // Reserved 3
#define INTERNAL_ET_VALUE_DW_RESERVED4  ((ET_VALUE_OS_WINNT|INTERNAL_ET_VALUE_HIDDEN_KEYS) + 4) // Reserved 4
#define INTERNAL_ET_VALUE_DW_RESERVED5  ((ET_VALUE_OS_WINNT|INTERNAL_ET_VALUE_HIDDEN_KEYS) + 5) // Reserved 5
#define INTERNAL_ET_VALUE_DW_RESERVED6  ((ET_VALUE_OS_WINNT|INTERNAL_ET_VALUE_HIDDEN_KEYS) + 6) // Reserved 6
#define INTERNAL_ET_VALUE_DW_RESERVED7  ((ET_VALUE_OS_WINNT|INTERNAL_ET_VALUE_HIDDEN_KEYS) + 7) // Reserved 7
#define INTERNAL_ET_VALUE_DW_RESERVED8  ((ET_VALUE_OS_WINNT|INTERNAL_ET_VALUE_HIDDEN_KEYS) + 8) // Reserved 8
#define INTERNAL_ET_VALUE_DW_RESERVED9  ((ET_VALUE_OS_WINNT|INTERNAL_ET_VALUE_HIDDEN_KEYS) + 9) // Reserved 9

#define INTERNAL_ET_VALUE_DW_ICDRESERVED0  ((ET_VALUE_OS_WINNT|INTERNAL_ET_VALUE_HIDDEN_KEYS) + 10) // ICD Reserved 0
#define INTERNAL_ET_VALUE_DW_ICDRESERVED1  ((ET_VALUE_OS_WINNT|INTERNAL_ET_VALUE_HIDDEN_KEYS) + 11) // ICD Reserved 1
#define INTERNAL_ET_VALUE_DW_ICDRESERVED2  ((ET_VALUE_OS_WINNT|INTERNAL_ET_VALUE_HIDDEN_KEYS) + 12) // ICD Reserved 2
#define INTERNAL_ET_VALUE_DW_ICDRESERVED3  ((ET_VALUE_OS_WINNT|INTERNAL_ET_VALUE_HIDDEN_KEYS) + 13) // ICD Reserved 3
#define INTERNAL_ET_VALUE_DW_ICDRESERVED4  ((ET_VALUE_OS_WINNT|INTERNAL_ET_VALUE_HIDDEN_KEYS) + 14) // ICD Reserved 4
#define INTERNAL_ET_VALUE_DW_ICDRESERVED5  ((ET_VALUE_OS_WINNT|INTERNAL_ET_VALUE_HIDDEN_KEYS) + 15) // ICD Reserved 5
#define INTERNAL_ET_VALUE_DW_ICDRESERVED6  ((ET_VALUE_OS_WINNT|INTERNAL_ET_VALUE_HIDDEN_KEYS) + 16) // ICD Reserved 6
#define INTERNAL_ET_VALUE_DW_ICDRESERVED7  ((ET_VALUE_OS_WINNT|INTERNAL_ET_VALUE_HIDDEN_KEYS) + 17) // ICD Reserved 7
#define INTERNAL_ET_VALUE_DW_ICDRESERVED8  ((ET_VALUE_OS_WINNT|INTERNAL_ET_VALUE_HIDDEN_KEYS) + 18) // ICD Reserved 8
#define INTERNAL_ET_VALUE_DW_ICDRESERVED9  ((ET_VALUE_OS_WINNT|INTERNAL_ET_VALUE_HIDDEN_KEYS) + 19) // ICD Reserved 9

#define INTERNAL_ET_VALUE_DW_ERAZOR_II   ((ET_VALUE_OS_WINNT|INTERNAL_ET_VALUE_HIDDEN_KEYS) + 20)    // temporary keys used for not implemented features

// internal keys
#define INTERNAL_ET_VALUE_DW_CORE_CLOCK          ((ET_VALUE_OS_WINNT|INTERNAL_ET_VALUE_HIDDEN_KEYS) + 21)
#define INTERNAL_ET_VALUE_DW_MEMORY_CLOCK        ((ET_VALUE_OS_WINNT|INTERNAL_ET_VALUE_HIDDEN_KEYS) + 22)
#define INTERNAL_ET_VALUE_DW_MEMORY_TIMING       ((ET_VALUE_OS_WINNT|INTERNAL_ET_VALUE_HIDDEN_KEYS) + 23)
#define INTERNAL_ET_VALUE_DW_HW_STANDARDTIMINGS  ((ET_VALUE_OS_WINNT|INTERNAL_ET_VALUE_HIDDEN_KEYS) + 24)
#define INTERNAL_ET_VALUE_DW_HW_SCRAMBLEMODE     ((ET_VALUE_OS_WINNT|INTERNAL_ET_VALUE_HIDDEN_KEYS) + 25)
#define INTERNAL_ET_VALUE_DW_HW_SCRAMBLESETTINGS ((ET_VALUE_OS_WINNT|INTERNAL_ET_VALUE_HIDDEN_KEYS) + 26)

#define INTERNAL_ET_VALUE_DW_DBG_OPENGLBUFFERNO     ((ET_VALUE_OS_WINNT|INTERNAL_ET_VALUE_HIDDEN_KEYS) + 27)
#define INTERNAL_ET_VALUE_DW_DBG_CHECKHEAPINTEGRITY ((ET_VALUE_OS_WINNT|INTERNAL_ET_VALUE_HIDDEN_KEYS) + 28)

#define INTERNAL_ET_VALUE_DW_HELPERFLAGS              ((ET_VALUE_OS_WINNT|INTERNAL_ET_VALUE_HIDDEN_KEYS) + 29)

// Keys used by the multiboard wrapper to store WINman2 modes (Binary)
#define INTERNAL_ET_VALUE_DW_MB2D_ET_WINMAN2_DATA_GET0 ((ET_VALUE_OS_WINNT|INTERNAL_ET_VALUE_HIDDEN_KEYS) + 30)
#define INTERNAL_ET_VALUE_DW_MB2D_ET_WINMAN2_DATA_GET1 ((ET_VALUE_OS_WINNT|INTERNAL_ET_VALUE_HIDDEN_KEYS) + 31)
#define INTERNAL_ET_VALUE_DW_MB2D_ET_WINMAN2_DATA_GET2 ((ET_VALUE_OS_WINNT|INTERNAL_ET_VALUE_HIDDEN_KEYS) + 32)
#define INTERNAL_ET_VALUE_DW_MB2D_ET_WINMAN2_DATA_GET3 ((ET_VALUE_OS_WINNT|INTERNAL_ET_VALUE_HIDDEN_KEYS) + 33)
#define INTERNAL_ET_VALUE_DW_MB2D_ET_WINMAN2_DATA_GET4 ((ET_VALUE_OS_WINNT|INTERNAL_ET_VALUE_HIDDEN_KEYS) + 34)
#define INTERNAL_ET_VALUE_DW_MB2D_ET_WINMAN2_DATA_GET5 ((ET_VALUE_OS_WINNT|INTERNAL_ET_VALUE_HIDDEN_KEYS) + 35)
#define INTERNAL_ET_VALUE_DW_MB2D_ET_WINMAN2_DATA_GET6 ((ET_VALUE_OS_WINNT|INTERNAL_ET_VALUE_HIDDEN_KEYS) + 36)
#define INTERNAL_ET_VALUE_DW_MB2D_ET_WINMAN2_DATA_GET7 ((ET_VALUE_OS_WINNT|INTERNAL_ET_VALUE_HIDDEN_KEYS) + 37)
#define INTERNAL_ET_VALUE_DW_MB2D_ET_WINMAN2_DATA_GET8 ((ET_VALUE_OS_WINNT|INTERNAL_ET_VALUE_HIDDEN_KEYS) + 38)
#define INTERNAL_ET_VALUE_DW_MB2D_ET_WINMAN2_DATA_GET9 ((ET_VALUE_OS_WINNT|INTERNAL_ET_VALUE_HIDDEN_KEYS) + 39)

// Keys used by the single board driver to store WINman2 modes (Binary)
#define INTERNAL_ET_VALUE_DW_ET_WINMAN2_DATA_GET0 ((ET_VALUE_OS_WINNT|INTERNAL_ET_VALUE_HIDDEN_KEYS) + 40)
#define INTERNAL_ET_VALUE_DW_ET_WINMAN2_DATA_GET1 ((ET_VALUE_OS_WINNT|INTERNAL_ET_VALUE_HIDDEN_KEYS) + 41)
#define INTERNAL_ET_VALUE_DW_ET_WINMAN2_DATA_GET2 ((ET_VALUE_OS_WINNT|INTERNAL_ET_VALUE_HIDDEN_KEYS) + 42)
#define INTERNAL_ET_VALUE_DW_ET_WINMAN2_DATA_GET3 ((ET_VALUE_OS_WINNT|INTERNAL_ET_VALUE_HIDDEN_KEYS) + 43)
#define INTERNAL_ET_VALUE_DW_ET_WINMAN2_DATA_GET4 ((ET_VALUE_OS_WINNT|INTERNAL_ET_VALUE_HIDDEN_KEYS) + 44)
#define INTERNAL_ET_VALUE_DW_ET_WINMAN2_DATA_GET5 ((ET_VALUE_OS_WINNT|INTERNAL_ET_VALUE_HIDDEN_KEYS) + 45)
#define INTERNAL_ET_VALUE_DW_ET_WINMAN2_DATA_GET6 ((ET_VALUE_OS_WINNT|INTERNAL_ET_VALUE_HIDDEN_KEYS) + 46)
#define INTERNAL_ET_VALUE_DW_ET_WINMAN2_DATA_GET7 ((ET_VALUE_OS_WINNT|INTERNAL_ET_VALUE_HIDDEN_KEYS) + 47)
#define INTERNAL_ET_VALUE_DW_ET_WINMAN2_DATA_GET8 ((ET_VALUE_OS_WINNT|INTERNAL_ET_VALUE_HIDDEN_KEYS) + 48)
#define INTERNAL_ET_VALUE_DW_ET_WINMAN2_DATA_GET9 ((ET_VALUE_OS_WINNT|INTERNAL_ET_VALUE_HIDDEN_KEYS) + 49)

// tetris tiling configuration
#define INTERNAL_ET_VALUE_DW_HW_TETRIS_TILING    ((ET_VALUE_OS_WINNT|INTERNAL_ET_VALUE_HIDDEN_KEYS) + 50)
#define INTERNAL_ET_VALUE_DW_HW_TETRIS_MODE      ((ET_VALUE_OS_WINNT|INTERNAL_ET_VALUE_HIDDEN_KEYS) + 51)
#define INTERNAL_ET_VALUE_DW_HW_TETRIS_SHIFT     ((ET_VALUE_OS_WINNT|INTERNAL_ET_VALUE_HIDDEN_KEYS) + 52)

#define INTERNAL_ET_VALUE_DW_DBG_SURFACENEVERATBOTTOM      ((ET_VALUE_OS_WINNT|INTERNAL_ET_VALUE_HIDDEN_KEYS) + 54)
#define INTERNAL_ET_VALUE_DW_OPENGL_FORCESOFTWARE          ((ET_VALUE_OS_WINNT|INTERNAL_ET_VALUE_HIDDEN_KEYS) + 55)
#define INTERNAL_ET_VALUE_DW_OPENGL_DBGFLAGSOVERRIDE       ((ET_VALUE_OS_WINNT|INTERNAL_ET_VALUE_HIDDEN_KEYS) + 56)
#define INTERNAL_ET_VALUE_DW_OPENGL_DBGMAXLOCALERRORCOUNT  ((ET_VALUE_OS_WINNT|INTERNAL_ET_VALUE_HIDDEN_KEYS) + 57)
#define INTERNAL_ET_VALUE_DW_OPENGL_DBGTRACETOFILE         ((ET_VALUE_OS_WINNT|INTERNAL_ET_VALUE_HIDDEN_KEYS) + 58)
#define INTERNAL_ET_VALUE_DW_OPENGL_USE_DISPLAYDRIVER_BUFFER ((ET_VALUE_OS_WINNT|INTERNAL_ET_VALUE_HIDDEN_KEYS) + 59)// FALSE use DirectDraw-Buffer ; TRUE: use DisplayDriverBuffer via OGLESC
#define INTERNAL_ET_VALUE_DW_HW_FORCE_TIMING               ((ET_VALUE_OS_WINNT|INTERNAL_ET_VALUE_HIDDEN_KEYS) + 60)
#define INTERNAL_ET_VALUE_DW_OPENGL_ACQUIREMUTEXFORKICKOFF ((ET_VALUE_OS_WINNT|INTERNAL_ET_VALUE_HIDDEN_KEYS) + 61)
#define INTERNAL_ET_VALUE_DW_OPENGL_CLIENTCLIPRECTS         ((ET_VALUE_OS_WINNT|INTERNAL_ET_VALUE_HIDDEN_KEYS) + 62)
#define INTERNAL_ET_VALUE_DW_OPENGL_CLIENTRELATIVECLIPRECTS INTERNAL_ET_VALUE_DW_OPENGL_CLIENTCLIPRECTS // old for compatibility only!
#define INTERNAL_ET_VALUE_DW_HW_USE_ELSA_INITCODE           ((ET_VALUE_OS_WINNT|INTERNAL_ET_VALUE_HIDDEN_KEYS) + 63)  
#define INTERNAL_ET_VALUE_DW_HW_SHUTDOWN                    ((ET_VALUE_OS_WINNT|INTERNAL_ET_VALUE_HIDDEN_KEYS) + 64) // FL_SHUTDOWN_GART, ...

#define INTERNAL_ET_VALUE_DW_OPENGL_NO_ICD_CALLBACK         ((ET_VALUE_OS_WINNT|INTERNAL_ET_VALUE_HIDDEN_KEYS) + 65) // if set the display driver doesn't call the ICD for FLIP callbacks
#define INTERNAL_ET_VALUE_DW_OPENGL_OVERLAYBPP              ((ET_VALUE_OS_WINNT|INTERNAL_ET_VALUE_HIDDEN_KEYS) + 66) // Bits per pixel used for overlay
#define INTERNAL_ET_VALUE_SZ_OEM_VERSION                    ((ET_VALUE_OS_WINNT|INTERNAL_ET_VALUE_HIDDEN_KEYS) + 67) // OEM build detection (e.g. for DELL drivers)
#define INTERNAL_ET_VALUE_DW_GDI_PUNTFLAGS                  ((ET_VALUE_OS_WINNT|INTERNAL_ET_VALUE_HIDDEN_KEYS) + 68) // flags to punt drv functions FL_PUNT_DRVCOPYBITS,...

#define INTERNAL_ET_VALUE_DW_HW_ONDMACRASH                  ((ET_VALUE_OS_WINNT|INTERNAL_ET_VALUE_HIDDEN_KEYS) + 69) // How to react on a resource manager crash dump 


LONG lEscRegistryValue(
  PPDEV    ppdev,
  ULONG    ulSubEsc, 
  ULONG    cjIn ,
  VOID    *pvIn ,
  ULONG    cjOut,
  VOID    *pvOut);

LONG lRegEtQueryValue(
  IN         HANDLE             hDriver, 
  IN         DWORD              dwSubEsc, 
  IN  struct tagET_QUERY_VALUE *petQueryValue,
  IN         ULONG              cjOut, 
  OUT        VOID              *pvOut);

LONG lRegEtSetValue(
  IN         HANDLE               hDriver, 
  IN         DWORD                dwSubEsc, 
  IN         VOID                *pvIn, 
  IN         ULONG                cjIn, 
  OUT struct tagET_SET_VALUE_RET *pSetValueRet);

LONG lRegEtDefaultValue(
  IN         HANDLE                   hDriver, 
  IN  struct tagET_SET_VALUE_DEFAULT *pDefaultValue, 
  OUT struct tagET_SET_VALUE_RET     *pSetValueRet);

LONG lRegGetIdBinary ( IN HANDLE hDriver, IN DWORD dwID,      OUT BYTE  *pjData,   IN DWORD nByte );
LONG lRegGetIdDWord  ( IN HANDLE hDriver, IN DWORD dwID,      OUT DWORD *pulData );
LONG lRegGetIdSz     ( IN HANDLE hDriver, IN DWORD dwID,      OUT PSTR   pstrData, IN DWORD nByte );
LONG lRegGetIdMultiSz( IN HANDLE hDriver, IN DWORD dwID,      OUT PSTR   pstrData, IN DWORD nByte );

LONG lRegSetIdBinary ( IN HANDLE hDriver, IN DWORD dwID, const IN BYTE  *pjData,   IN DWORD nByte );
LONG lRegSetIdDWord  ( IN HANDLE hDriver, IN DWORD dwID,       IN DWORD  ulData );
LONG lRegSetIdSz     ( IN HANDLE hDriver, IN DWORD dwID, const IN PSTR   pstrData);
LONG lRegSetIdMultiSz( IN HANDLE hDriver, IN DWORD dwID, const IN PSTR   pstrData, IN DWORD nByte );

#endif // USE_WKS_ELSA_TOOLS
#endif // _WKSELSATOOLSREGISTRY_H
