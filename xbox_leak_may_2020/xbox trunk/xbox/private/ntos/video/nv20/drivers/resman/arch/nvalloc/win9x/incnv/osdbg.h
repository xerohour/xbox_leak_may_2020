#ifndef _OS_DBG_H_

#define _OS_DBG_H_

// Function Table to use to Output Messages
#define DBG_OBJECT_OS (0)
#define DBG_OBJECT_SERIAL (1)
// All of these below are currently unimplemented
#define DBG_OBJECT_PARALLEL (2)
#define DBG_OBJECT_INFRARED (3)
#define DBG_OBJECT_USB (4)
#define DBG_OBJECT_I2C (5)
#define DBG_OBJECT_LAST DBG_OBJECT_I2C

typedef struct dbgserialinfo {
   NvU32 uPort;
   NvU32 uBaud;
   } DBGSERIALINFO;

typedef struct dbgtbdinfo {
   NvU32 uTBD;
} DBGTBDINFO;

typedef struct dbgport {
   NvU32 uType;
   union {
   DBGSERIALINFO Serial;
   DBGTBDINFO Parallel;
   DBGTBDINFO IIR;
   DBGTBDINFO USB;
   DBGTBDINFO I2C;
   } uTag;
} DBGPORT, * PDBGPORT;

//
// Function Prototypes
//
typedef VOID (* PFNCONFIG)(NvV32 * pDev);
typedef VOID (* PFNINIT)(PDBGPORT pdbgPort);
typedef VOID (* PFNOPEN)(NvV32 * pDev);
typedef VOID (* PFNDISPLAY)(char * pStr);
typedef VOID (* PFNCLOSE)(NvV32 * pDev);

typedef struct dbgobject {
	PFNCONFIG pConfig;
	PFNINIT pInit;
	PFNOPEN pOpen;
	PFNDISPLAY pDisplay;
	PFNCLOSE pClose;
} DBGOBJECT, * PDBGOBJECT;

//
// Simple Macros for IO
//
#define OUTB outpb
#define OUTW outpw
#define INB inpb
#define INW inpw

//
// Indexes fro Name strings
//
#define DBG_COM1_PORT_INDEX (0)
#define DBG_COM2_PORT_INDEX (1)
#define DBG_PARALLEL_PORT_INDEX (2)

//
// Standard PC Plug-n-Play ID's for 16550 and Printer Port
//
// Windows 98
#define DBG_W98_COM1_KEY_STR1 "ACPI\\*PNP0501\\00000001"
#define DBG_W98_COM1_KEY_STR2 "ACPI\\*PNP0500\\00000001"
// Windows 95
#define DBG_COM1_KEY_STR3 "BIOS\\*PNP0501\\02"
#define DBG_COM1_KEY_STR4 "BIOS\\*PNP0500\\00"

// Windows 98
#define DBG_W98_COM2_KEY_STR1 "ACPI\\*PNP0501\\00000002"
#define DBG_W98_COM2_KEY_STR2 "ACPI\\*PNP0500\\00000002"
// Windows 95
#define DBG_COM2_KEY_STR3 "BIOS\\*PNP0501\\03"
#define DBG_COM2_KEY_STR4 "BIOS\\*PNP0500\\01"

#define DBG_W98_PARALLEL_KEY_STR1 "ACPI\\*PNP0400\\0"
#define DBG_PARALLEL_KEY_STR2 "BIOS\\*PNP0400\\0"

#define DBG_COM1_PORTNAME "COM1"
#define DBG_COM2_PORTNAME "COM2"
#define DBG_PARALLEL_PORTNAME "LPT1"

//
// Prototypes
//
// PNP Routines
NvU32 Locate_DevNode(PDEVNODE pdnDevNode, char * pStr[], NvU32 ulSize);
NvU32 dbgEnableDevice(NvU32 nPort);
NvU32 dbgEnableDevice2(PDBGPORT pDbgPort);
NvU32 dbgDisableDevice2(PDBGPORT pDbgPort);
NvU32 dbgDisableDevice(NvU32 nPort);

// Simple Format Routine
VOID dbgFormat(char * pStr, NvU32 value);

//
// Null Routines so we can skipping Null Pointer checking
//
void dbgConfigNull(NvV32 * pDev);
void dbgInitNull(PDBGPORT pDbgPort);
void dbgDisplayNull(char * pStr);
void dbgOpenNull(NvV32 * pDev);
void dbgCloseNull(NvV32 * pDev);

// Simple PIO routines
NvU8 inpb(NvU16 uPort);
NvU16 inpw(NvU16 uPort);
VOID outpb(NvU16 uPort, NvU8 bValue);
VOID outpw(NvU16 uPort, NvU16 wValue);


//
//  This is where I added the Vxd Services that I am using
//  The driver is configured to build with Win95 but I want to use
//  some Win 98 Serivces in the Config Manager so I have added them
//  here
//

#define	CONFIG98_VERSION	(0x040A)
#define  ___CONFIGMG_Get_DevNode_PowerState (0x00330065)
#define  ___CONFIGMG_Set_DevNode_PowerState (0x00330064)

#define CM_POWERSTATE_D0                        0x00000001
#define CM_POWERSTATE_D1			0x00000002
#define CM_POWERSTATE_D2                        0x00000004
#define CM_POWERSTATE_D3                        0x00000008
#define CM_POWERSTATE_BITS                      0x0000000f

//
// Return value is hidden so disable no return value detection warning
//
#pragma warning(disable:4035)
DWORD VXDINLINE __declspec(naked) CMxx_Get_Version(VOID)
{
   _asm  push  ebp
   _asm  mov   ebp, esp
   _asm  push  ecx
   _asm  push  ebx
   _asm  push  esi
   _asm  push  edi
   VMMCall(_CONFIGMG_Get_Version)
   _asm  pop   edi
   _asm  pop   esi
   _asm  pop   ebx
   _asm  pop   ecx
   _asm  mov   esp, ebp
   _asm  pop   ebp
   _asm  ret
}

DWORD VXDINLINE __declspec(naked) CMxx_Locate_DevNode(PDEVNODE pdnDevNode, DEVNODEID pDeviceID, ULONG ulFlags)
{
   VMMJmp(_CONFIGMG_Locate_DevNode);
}

DWORD VXDINLINE __declspec(naked) CMxx_Disable_DevNode (DEVNODE dnDevNode, ULONG ulFlags)
{
   VMMJmp(_CONFIGMG_Disable_DevNode);
}

DWORD VXDINLINE __declspec(naked) CMxx_Enable_DevNode (DEVNODE dnDevNode, ULONG ulFlags)
{
   VMMJmp(_CONFIGMG_Enable_DevNode);
}

DWORD VXDINLINE __declspec(naked) CMxx_Read_Registry_Value (DEVNODE dnDevNode, PFARCHAR pszSubKey, PFARCHAR pszValueName, ULONG ulExpectedType, PFARVOID pBuffer, PFARULONG pulLength, ULONG ulFlags)
{
   VMMJmp(_CONFIGMG_Read_Registry_Value);
}

DWORD VXDINLINE __declspec(naked) CMxx_Write_Registry_Value (DEVNODE dnDevNode, PFARCHAR pszSubKey, PFARCHAR pszValueName, ULONG ulType, PFARVOID pBuffer, ULONG ulLength, ULONG ulFlags)
{
   VMMJmp(_CONFIGMG_Write_Registry_Value);
}

DWORD VXDINLINE __declspec(naked) CMxx_Debug_DevNode (DEVNODE dnDevNode, ULONG ulFlags)
{
   VMMJmp(_CONFIGMG_Debug_DevNode);
}

DWORD VXDINLINE __declspec(naked) CMxx_Set_DevNode_PowerState(DEVNODE dnDevNode, ULONG ulPowerState, ULONG ulFlags)
{
   VMMJmp(_CONFIGMG_Set_DevNode_PowerState);
}

DWORD VXDINLINE __declspec(naked) CMxx_Get_DevNode_PowerState(DEVNODE dnDevNode, PFARULONG pulPowerState, ULONG ulFlags)
{
   VMMJmp(_CONFIGMG_Get_DevNode_PowerState);
}

#define ___ACPI_GetVersion (0x004C0000)

DWORD VXDINLINE __declspec(naked) ACPI_Get_Version(VOID)
{
   _asm  push  ebp
   _asm  mov   ebp, esp
   _asm  push  ecx
   _asm  push  ebx
   _asm  push  esi
   _asm  push  edi
   VxDCall(_ACPI_GetVersion);
   _asm  pop   edi
   _asm  pop   esi
   _asm  pop   ebx
   _asm  pop   ecx
   _asm  mov   esp, ebp
   _asm  pop   ebp
   _asm  ret
}

#define ___ACPI_IdentifyDebuggerCommInfo (0x004C0015)

DWORD VXDINLINE __declspec(naked) ACPI_IdentifyDebuggerCommInfo(DEVNODE dnDevNode, ULONG ulFlags)
{
   VMMJmp(_ACPI_IdentifyDebuggerCommInfo);
}

#define ___VCD_Set_Port_Global (0x000E0001)

DWORD VXDINLINE __declspec(naked) VCD_Set_Port_Global(DWORD nPort)
{
   _asm  push  ebp
   _asm  mov   ebp, esp
   _asm  push  edx
   _asm  push  ecx
   _asm  push  ebx
   _asm  push  esi
   _asm  push  edi
   _asm  mov   eax, nPort
   _asm  xor  edx, edx
   VMMCall(_VCD_Set_Port_Global)
   _asm  pop   edi
   _asm  pop   esi
   _asm  pop   ebx
   _asm  pop   ecx
   _asm  pop   edx
   _asm  mov   esp, ebp
   _asm  pop   ebp
   _asm  ret
}

DWORD VXDINLINE __declspec(naked) VCOMM_OpenComm(char * pStr, DWORD hVM)
{
   VMMJmp(_VCOMM_OpenComm);
}

DWORD VXDINLINE __declspec(naked) VCOMM_CloseComm(DWORD hPort)
{
   VMMJmp(_VCOMM_CloseComm);
}

typedef DWORD (* PFNNOTIFY)(DWORD dwRefData, DWORD dwType);
typedef DWORD (* PFNCONTENT)(DWORD dwType, DWORD dwResource, PFNNOTIFY pFn, DWORD dwRefData, DWORD dwSteal);

PFNCONTENT VXDINLINE __declspec(naked) VCOMM_Get_Contention_Handler(char * pStr)
{
   VMMJmp(_VCOMM_Get_Contention_Handler);
}

DWORD VXDINLINE __declspec(naked) VCOMM_Map_Name_To_Resource(char * pStr)
{
   VMMJmp(_VCOMM_Map_Name_To_Resource);
}

#define ___VCOMM_PowerOnOffComm (___VCOMM_Map_Name_To_Resource+1)

DWORD VXDINLINE __declspec(naked) VCOMM_PowerOnOffComm(DWORD devNode, DWORD PowerState)
{
   VMMJmp(_VCOMM_PowerOnOffComm);
}



//
// Enable detection of no return values
//
#pragma warning(default:4035)

#endif
