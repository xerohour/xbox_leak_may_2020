#define WANTVXDWRAPS
#include <basedef.h>
#include <vmm.h>
#include <vxdwraps.h>
#include <configmg.h>
#include <vcomm.h>
#include <nvtypes.h>
#include <osdbg.h>

#ifdef DEBUG

DEVNODE dbgDevNode = 0xFFFFFFFF;
NvU32 dbgPowerState = CM_POWERSTATE_D3;
NvU32 dbgPowerSysState = 0x0;
NvU32 IsConfig98 = FALSE;

char * ppdbgPortStr_COM1[] = {
   DBG_W98_COM1_KEY_STR1,
   DBG_W98_COM1_KEY_STR2,
   DBG_COM1_KEY_STR3,
   DBG_COM1_KEY_STR4,
   };

char * ppdbgPortStr_COM2[] = {
   DBG_W98_COM2_KEY_STR1,
   DBG_W98_COM2_KEY_STR2,
   DBG_COM2_KEY_STR3,
   DBG_COM2_KEY_STR4,
   };

char * ppdbgPortStr_Parallel[] = {
   DBG_W98_PARALLEL_KEY_STR1,
   DBG_PARALLEL_KEY_STR2,
   };

typedef struct cm_array {
   char ** ppStr;
   NvU32   ulSize;
   } CM_ARRAY, * PCM_ARRAY;   

CM_ARRAY CmArray[] = {
   {ppdbgPortStr_COM1, sizeof(ppdbgPortStr_COM1)/sizeof(char **),},
   {ppdbgPortStr_COM2, sizeof(ppdbgPortStr_COM2)/sizeof(char **),},
   {ppdbgPortStr_Parallel, sizeof(ppdbgPortStr_Parallel)/sizeof(char **),},
   };


// Port Names
char * ppdbgPortNameStr[] = {
   DBG_COM1_PORTNAME,
   DBG_COM2_PORTNAME,
   DBG_PARALLEL_PORTNAME,
   };

extern int dbgObject_inuse;
extern DBGOBJECT DbgObject[];
extern DBGPORT DbgPort;

//
// This routine is a callback if the another proc tries to
// our serial port
//
DWORD StealProc(DWORD dwRefData, DWORD dwType)
{
   (VOID*)dwRefData;
   (VOID*)dwType;
   return 0x0;
}

//
// Walk the string of registry serial ports looking for the
// serial port name
//

NvU32 Locate_DevNode(PDEVNODE pdnDevNode, char * pStr[], NvU32 ulSize)
{
   NvU32 i;
   NvU32 ulReturn;

   ulReturn = CR_NO_SUCH_DEVNODE;
   for (i=0; i<ulSize; i++)
      {
      ulReturn = CMxx_Locate_DevNode(pdnDevNode, pStr[i], 0x0);
      if (CR_SUCCESS == ulReturn)
         break;
      }

   return ulReturn;       
}


//
// This routine is used to call CM_Set_DevNode_PowerState
// to activate the port that we want to use.  The bios
// enables the port but then Windows ACPI Policy Manager
// deactivates the port to reduce power 
//
NvU32 dbgEnableDevice(NvU32 nPort)
{
   NvU32 nReturn;

   nReturn = FALSE;

   if (CR_SUCCESS == Locate_DevNode(&dbgDevNode, CmArray[nPort].ppStr, CmArray[nPort].ulSize))
      {
      // Is this Win '98 Version of Configuration Manager
      // VCOMM will attempt to power the port down if 
      // the registry under SW_KEY\Vxd\VCOMM\EnablePowerManagement is nonzero
      if (CONFIG98_VERSION <= CMxx_Get_Version())
         {
         IsConfig98 = TRUE;
         if (CR_SUCCESS == CMxx_Get_DevNode_PowerState(dbgDevNode, &dbgPowerState, 0x0))
            {
            // Does it hurt to do this if EnablePowerManagement is zero???
            if (CR_SUCCESS == CMxx_Set_DevNode_PowerState(dbgDevNode, CM_POWERSTATE_D0, 0x0))
               {
               nReturn = TRUE;
               }
            }
         }
      }
   
   return nReturn;
}

DWORD hPort = 0x0;
PFNCONTENT hHandler = 0x0;
DWORD hResource;
NvU32 dbgEnableDevice2(PDBGPORT pDbgPort)
{
#if 1
   DWORD hContend;
#endif
   NvU32 nReturn;
   NvU32 nPort;
   nReturn = FALSE;

   if (DBG_OBJECT_SERIAL == dbgObject_inuse)
      {
      nPort = pDbgPort->uTag.Serial.uPort;
      // Unvirtualize the port
      VCD_Set_Port_Global(nPort+1);
      // Steal the port
      if (0x0 == hPort)
         {
         hPort = VCOMM_OpenComm(ppdbgPortNameStr[nPort], 0xFFFFFFFF);
         if (IE_BADID != hPort)
            {
            if (0x0 == hHandler)
               {
               hHandler = VCOMM_Get_Contention_Handler(ppdbgPortNameStr[nPort]); 
               if (0x0 != hHandler)
                  {
                  hResource = VCOMM_Map_Name_To_Resource(ppdbgPortNameStr[nPort]);
                  if (0x0 != hResource)
                     {
                     hContend = hHandler(ACQUIRE_RESOURCE, hResource, StealProc, 0xAA5555AA, TRUE);
                     if (0x0 != hContend)
                        nReturn = TRUE;
                     }
                  }
               }
            }
         }
      // when the port was virtualized it was reinit
      // let's init the way we like
      //Reinit 1 more time
      DbgObject[dbgObject_inuse].pInit(pDbgPort);          
      }

   return nReturn;
}

NvU32 dbgDisableDevice2(PDBGPORT pDbgPort)
{
   if (DBG_OBJECT_SERIAL == dbgObject_inuse)
      {
            
      // When we did the open an implicit VCOMM_PowerOnOffComm was called
      // here we need to undue this else we cannot hibernate
      VCOMM_PowerOnOffComm(dbgDevNode, CM_POWERSTATE_D3);
      }   

   return 0;
}

//
// This routine is used to return the port to its original state.
//
NvU32 dbgDisableDevice(NvU32 nPort)
{
   
   NvU32 nReturn = FALSE;

   if (0xFFFFFFFF != dbgDevNode)
      {
      if (CR_SUCCESS == CMxx_Set_DevNode_PowerState(dbgDevNode, dbgPowerState, 0x0))
         nReturn = TRUE;
      }

   return nReturn;
}

//
// routine to do a simple hex to ascii convert
//
VOID dbgFormat(char * pStr, NvU32 value)
{
	NvS32 i;
	NvS32 hex_digit;
	NvS32 j;
	char rData[8];

	// simple hex to ascii convert
	for (i=0;value!=0;value>>=4)
		{
		hex_digit=value & 0x0F;
		if (hex_digit < 10)
			rData[i]=hex_digit+'0';
		else
			rData[i]=hex_digit-10+'A';
		i++;
		}

	// reverse string
	for (j=0;j<i;j++)
		pStr[j]=rData[i-j-1];

	// if the number is zero then we need to insert zero digit
	if (0==i)
		pStr[i++]='0';

	// null terminate
	pStr[i]='\0';
}

//
// Null Table Functions
//

//
// Null Config Function
//
void dbgConfigNull(NvV32 * pDev)
{
	return;
}

//
// Null Init Function
//
void dbgInitNull(PDBGPORT pDbgPort)
{
	return;
}

//
// Null Display Function
//
void dbgDisplayNull(char * pStr)
{
	return;
}

//
// Null Open Function
//
void dbgOpenNull(NvV32 * pDev)
{
	return;
}

//
// Null Close Function
//
void dbgCloseNull(NvV32 * pDev)
{
	return;
}


// 
// The following is a simple IO Sections
//
//
// Read a byte from a Word Port
//
// Fix Help find and Figure out how to call internal io routines
NvU8 inpb(NvU16 uPort)
{
	NvU8 bReturn;

	_asm push	edx
	_asm mov	dx, uPort
	_asm in		al, dx
	_asm mov	bReturn, al
	_asm pop	edx

	return bReturn;
}

//
// Read a word from word Port
//
NvU16 inpw(NvU16 uPort)
{
	NvU16 wReturn;

	_asm push	edx
	_asm mov	dx, uPort
	_asm in		ax, dx
	_asm mov	wReturn, ax
	_asm pop	edx

	return wReturn;
}

//
// Write a byte to a word port
//
VOID outpb(NvU16 uPort, NvU8 bValue)
{
	_asm push	edx
	_asm mov	dx, uPort
	_asm mov	al, bValue
	_asm out	dx, al
	_asm pop	edx
}

//
// Write a word to a word port
//
VOID outpw(NvU16 uPort, NvU16 wValue)
{
	_asm push	edx
	_asm mov	dx, uPort
	_asm mov	ax, wValue
	_asm out	dx, ax
	_asm pop	edx
}

#endif
