 /***************************************************************************\
|*                                                                           *|
|*        Copyright (c) 1993-2000 NVIDIA, Corp.  All rights reserved.        *|
|*                                                                           *|
|*     NOTICE TO USER:   The source code  is copyrighted under  U.S. and     *|
|*     international laws.   NVIDIA, Corp. of Sunnyvale, California owns     *|
|*     the copyright  and as design patents  pending  on the design  and     *|
|*     interface  of the NV chips.   Users and possessors of this source     *|
|*     code are hereby granted  a nonexclusive,  royalty-free  copyright     *|
|*     and  design  patent license  to use this code  in individual  and     *|
|*     commercial software.                                                  *|
|*                                                                           *|
|*     Any use of this source code must include,  in the user documenta-     *|
|*     tion and  internal comments to the code,  notices to the end user     *|
|*     as follows:                                                           *|
|*                                                                           *|
|*     Copyright (c) 1993-2000  NVIDIA, Corp.    NVIDIA  design  patents     *|
|*     pending in the U.S. and foreign countries.                            *|
|*                                                                           *|
|*     NVIDIA, CORP.  MAKES  NO REPRESENTATION ABOUT  THE SUITABILITY OF     *|
|*     THIS SOURCE CODE FOR ANY PURPOSE.  IT IS PROVIDED "AS IS" WITHOUT     *|
|*     EXPRESS OR IMPLIED WARRANTY OF ANY KIND.  NVIDIA, CORP. DISCLAIMS     *|
|*     ALL WARRANTIES  WITH REGARD  TO THIS SOURCE CODE,  INCLUDING  ALL     *|
|*     IMPLIED   WARRANTIES  OF  MERCHANTABILITY  AND   FITNESS   FOR  A     *|
|*     PARTICULAR  PURPOSE.   IN NO EVENT SHALL NVIDIA, CORP.  BE LIABLE     *|
|*     FOR ANY SPECIAL, INDIRECT, INCIDENTAL,  OR CONSEQUENTIAL DAMAGES,     *|
|*     OR ANY DAMAGES  WHATSOEVER  RESULTING  FROM LOSS OF USE,  DATA OR     *|
|*     PROFITS,  WHETHER IN AN ACTION  OF CONTRACT,  NEGLIGENCE OR OTHER     *|
|*     TORTIOUS ACTION, ARISING OUT  OF OR IN CONNECTION WITH THE USE OR     *|
|*     PERFORMANCE OF THIS SOURCE CODE.                                      *|
|*                                                                           *|
 \***************************************************************************/
/******************* Operating System Interface Routines *******************\
*                                                                           *
* Module: OSAPI.C                                                           *
*   This is the OS interface module.  All operating system transactions     *
*   pass through these routines.  No other operating system specific code   *
*   or data should exist in the source.                                     *
*                                                                           *
\***************************************************************************/
#ifdef DEBUG
#define CAT_HELPER(x, y)    x##y
#define CAT(x, y)       CAT_HELPER(x, y)
#define MAKE_HEADER(RetType, DecType, Function, Parameters)
#endif
#include "basedef.h"
#include "vmm.h"        // put this first. we redefine macro SF_BIT in nvrm.h
#include "vmmreg.h"
#include "vpicd.h"
#ifdef DEBUG
#include "configmg.h"
#endif
#include <nv_ref.h>
#include <nvrm.h>
#include <state.h>
#include <class.h>
#include <vblank.h>
#include <dma.h>
#include <fifo.h>
#include <gr.h>
#include <dac.h>
#include <devinit.h>
#include <mc.h>
#include <os.h>
#include <edid.h>
#include <dac.h>
#include "nvrmwin.h"
#include "nvhw.h"
#include "vdd.h"
#include "vnvrmd.h"
#include "oswin.h"
#include "vmmtypes.h"
#include "vmm2.h"       // more vmm services
#include "vwin32.h"
#include "nvcm.h"
#include "nvReg.h"
#include "nvmisc.h"
#include "files.h"
#ifdef DEBUG
#include <vcomm.h>
#include "osdbg.h"
#endif
  
// number of bytes of Instance Memory to save 
#define INST_COPY   (pDev->Pram.HalInfo.TotalInstSize)

//
// prototypes
//
RM_STATUS   EDIDDetect                  (PHWINFO, U032, U032);
VOID CDECL  rmService                   (VOID);
U032 CDECL  rmSetConfig                 (U032*);
RM_STATUS   RmAllocDeviceInstance       (U032*);
RM_STATUS   EDIDRead                    (PHWINFO, U032, U032);
DWORD       OnW32DeviceIoControl        (PDIOCPARAMETERS);
int         osGetNextAvailableInstance  (VOID);

#ifdef DEBUG
U032        vmmTestDebugInstalled       (VOID);
#endif

// wrapper functions in vxdstub
VOID rmUsrAPIEntry(VOID);
VOID rmControlEntry(VOID);

// Prototype of the architecture dispatch function
VOID NvArchRMDispatch(U032 /*function*/, PVOID /*paramPtr*/);

U032   NvInfoSelectors[MAX_INSTANCE] = {0,0,0,0,0,0,0,0};
U032   NvInfoSelectorsSelector;

char strDevNodeRM[128] = NV4_REG_GLOBAL_BASE_PATH "\\" NV4_REG_RESOURCE_MANAGER_SUBKEY;
char strRegistryGlobalParameters[128] = NV4_REG_GLOBAL_BASE_PATH;
char strRmCoreName[] = STR_RM_CORE_NAME;
char strRmCoreVxd[] = NV4_RM_KERNEL_BINARY;

char strMaxRefreshRate[128]               = MAXREFRESHRATE;           
char strMaxOptimalRefreshRate[128]        = MAXOPTIMALREFRESHRATE;    
char strOptimalNotFromEDID[128]           = OPTIMALREFRESHNOTFROMEDID;
char strDMTOverride[128]                  = DMTOVERRIDE;              
char strMonitorTiming[128]                = MONITORTIMING;            
char strCursorCacheOverride[128]          = CURSORCACHE;              
char strDisplayType[128]                  = DISPLAY_TYPE;             
char strDisplay2Type[128]                 = DISPLAY2_TYPE;
char strTVtype[128]                       = TV_TYPE;                  
char strStartUp[128]                      = POWERUPFLAGS;             
char strTVOutType[128]                    = TV_OUT;                   
char strNewModeSet[128]                   = NEWSTYLEMODESET;          
char strFpMode[128]                       = FLATPANELMODE;            


typedef struct MiniVDD_ParameterStruct {
  DWORD  callrm_device_handle;       
  DWORD  callrm_modechange_callback; 
  DWORD  callrm_irq_bus;             
  DWORD  callrm_fbphys;              
  DWORD  callrm_nvphys;              
  DWORD  callrm_registry_path;       
} MiniVDD_ParameterStruct;

//---------------------------------------------------------------------------
//
//  Global variables used thoughout the OS specific module.
//
//---------------------------------------------------------------------------

int global_clientID;

//---------------------------------------------------------------------------
//
//  Global variables used by the OS specific calls.
//
//---------------------------------------------------------------------------
U032        rmInit              = 0;
U032        osPhase             = 0;
U032        *inst_sv;
BOOL        InstanceMemSaved    = FALSE;
BOOL        FirstDynamicInit    = TRUE;

//
// These globals are used by the mediaport callback mechanism; used by Canopus
//
CallBackDef mpCallback, mpImageCallback;
DWORD mpInstalledCallback = FALSE;
DWORD mpImageInstalledCallback = FALSE;
DWORD rmmpInCallback = 0;

#define MVDD_COPY_PTES_FOR_LINEAR_RANGE 0x87650002
U032        p_agp_dummy_page;

U032        ResourceManagerGlobalToken=0;
extern U032        nvInfoSelector;
extern U032        clientInfoSelector;
extern U032        rmInService;
extern U032        osInService;
extern U032        rmInSafeService;
extern U032        rmInCallback;

U032 deviceInstanceSelector;
U032 the16bitProcessDSselector;
U032 the16bitProcessDS;

U032        rmSemaphore;
U032        osSemaphore;

PCALLBACK   osCallbackTable;
PCALLBACK   osCallbackList;
PCALLBACK   osCallbackFreeList;

DISPLAYINFO osDisplayInfo;

// Global semaphore for multi-monitor implementation.
U032        mmSemaphore;
// Used by the MiniVDD to monitor mode changes
U032        mode_changed_after_save = 0;
// Signal cursor re-enable after ACPI resume
U032        resume;

#ifdef DEBUG
extern char * DotN_help;
extern void DotN_cmd_asm();
#endif
extern void RegisterDisplayDriver(PCS  x);

extern WIN9XHWINFO  win9xHwInfo[MAX_INSTANCE];

#ifdef DEBUG
extern int dbgObject_inuse;
extern DBGOBJECT DbgObject[];
extern DBGPORT DbgPort;
#endif

//---------------------------------------------------------------------------
//
//  Resource Manager VxD control function.
//
//---------------------------------------------------------------------------

RM_STATUS rmControl
(
    U032 Msg,
    U032 hVM
//    U032 flags
)
{
    RM_STATUS status;
    U032      i,event;
    CLIENT_STRUCT     crs;
    PHWINFO   pDev = NULL;  // Initialize the pDev to Null.
    PWIN9XHWINFO pOsHwInfo = NULL;

    status = RM_OK;
    //
    // WARNING: Remember that there are no devices initialized during early sections of POST (DEVICE_INIT),
    // so don't attempt to access any pDev structures or fields.  They haven't been built yet.
    //
    switch (Msg)
    {
    //
    // IOCTL Control Interface.
    //
    case W32_DEVICEIOCONTROL:
        {
            PDIOCPARAMETERS ptemp;
            _asm  mov ptemp, esi
            if (ptemp->dwIoControlCode == NVRM_IOCTL_NV_ARCH)
            {
                //
                // Architectural IOCTL.
                //
                U032                  hVM = 0;
                NV_IOCTL_ARCH_STRUCT *ptr = (NV_IOCTL_ARCH_STRUCT *)ptemp->lpvInBuffer;
              
                if (ptr->nvarchFunction <= NVRM_ARCH_MAX_IOCTL)
                    NvArchRMDispatch(ptr->nvarchFunction, (PVOID)ptr->nvarchParameters);
                    //rmArchDispatchTable[ptr->nvarchFunction](hVM, (U032)vmmGetCurrentContext(), (PVOID)ptr->nvarchParameters);
            }
            else
            {
                //
                // Diag stuff.
                //
                OnW32DeviceIoControl(ptemp);
            }
            status = RM_OK;
        }
        break;
        
    case SYS_DYNAMIC_DEVICE_INIT:    
       // We get this message when the primary or the Secondary mini VDD loads nv4rm.vxd using
       // VXDLDR_LoadDevice.
       // I have noticed that we get this message only the first time. Just in case we get called 
       // more than once, we have a global to tell us not to initialize ourselves more than once.
       if (FirstDynamicInit)
       {
#ifdef DEBUG
          DbgObject[dbgObject_inuse].pConfig(NULL);          
          DbgObject[dbgObject_inuse].pInit(&DbgPort);          
          DbgObject[dbgObject_inuse].pOpen(NULL);          
#endif            
          // This is the first dynamic init we have ever got. Initialize global data structures.
          if (!status) status = initSemaphore();
          if (!status) status = initCallbackFreeList();
          if (!status) status = initClientInfo();
          
          if (!status)
          {
              rmInCallback = 0;
              rmInService  = 0;
              osInService  = 0;
              rmInit = DEVICE_INIT; // show we've init'd, thus can be uninit'd
              FirstDynamicInit = FALSE;
          }
       }
       break;
       
    //
    // We now init as a Plug and Play device, but for the time being we'll terminate on these system messages
    // We may want to move the termination code into the Plug and Play handler later.
    case INIT_COMPLETE:
#ifdef DEBUG
        dbgEnableDevice2(&DbgPort);
#endif
        DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: Init_Complete\n\r");
        osPhase = INIT_COMPLETE;
        break;
    case SYS_VM_INIT:
        DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: Sys_VM_Init\n\r");
        osPhase = SYS_VM_INIT;

        // VBE SetMode
        // We'll do a mode set here because the VDD wants to see one, possibly so it will have a chance to save CRTC
        // registers. If we don't do this, the VDD may set a mode 3 later, at a bad time (when a high res display is
        // running) in response to some VGA-type activity (this happens when Winbench 97 starts and tries to find out
        // what type of display is being used).
        crs.CWRS.Client_AX = 0x4F02;
        crs.CWRS.Client_BX = 0x4003;
        VBE(&crs);
    
#ifdef DEBUG
        if(vmmTestDebugInstalled())
        {
          
          // Hook the .dot commands for the winice debugger.
          _asm {
            mov   ax,0070h
            mov   bl,'T'            
            mov   esi, offset DotN_cmd_asm
            mov   edi, offset DotN_help
            int   41h
          }
          DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: Soft-Ice/w Extension Installed\n\r");
        }
#endif  
     
        break;
    case POWER_EVENT:   // Win 3.1 style power event (display driver will not handle)
        _asm mov event,esi

        {
            int max_device,DeviceInstance;
            U032 Head = 0;  // TO DO: for all heads
            
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM: Control: PowerEvent=", event);

            max_device = osGetNextAvailableInstance();
            for(DeviceInstance=0;DeviceInstance<max_device;DeviceInstance++){
                // Make sure the memory window for the device is enabled.
                pDev = NvDBPtr_Table[DeviceInstance];

                // Ignore old-style event if PM supported.
                if(pDev->Power.Flags & DAC_PM_SUPPORTED)
                    break;
            
                     pOsHwInfo = (PWIN9XHWINFO) pDev->pOsHwInfo;
                osEnsureDeviceEnabled(pDev);
                REG_WR08(NV_PRMCIO_CRX__COLOR, 0x571F & 0xFF); 
                REG_WR08(NV_PRMCIO_CRX__COLOR+1, (0x571F >> 8) & 0xFF);
                ////OEMEnableExtensions();
                switch (event)
                {
                    case 1:     // suspend
                        if (pDev->Power.State == MC_POWER_LEVEL_0)
                        {
                            // turn off the monitor
                                       if(pDev->Power.Flags & DAC_POWER_PENDING)
                                       {
                                             //Reset global power_pending flag
                                             pDev->Power.Flags &= ~DAC_POWER_PENDING;

                                             // Bug 29711: if you hibernate on CRT, unplug CRT, boot back up
                                             // on LCD, the LCD blooms. When we get to this function, we do
                                             // a modeset on the CRT because the pVidLutCurDacs are setup for
                                             // head A = CRT and head B = Nothing.  If we reset the chip in 
                                             // mcPowerState, we turn off the flat panel clocks.  When we call 
                                             // Wake Up in this function, the LCD turns on, but the clocks
                                             // do not. This causes blooming.  
                                             // So to fix this, if we've reset the chip, and we've boot to  
                                             // an LCD, don't power on the panel!  The Reset flag is turned
                                             // off at the end of dacSetFlatPanelMode in dacfp.c.
                                             if(pDev->Dac.DevicesEnabled & DAC_PANEL_CLK_RESET)
                                             {
                                                     if(pDev->Dac.BootDevicesBitMap & DAC_DD_MASK_IN_MAP)
                                                     {
                                                             DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: Set Monitor Power called after chip reset without panel clocks setup!\n");
                                                             break;
                                                     }
                                             }

                                             for(Head = 0; Head < MAX_CRTCS; Head++)
                                             {
                                                     if ((GETDISPLAYTYPE(pDev, Head) != DISPLAY_TYPE_NONE))
                                                     {
                                                             // Enable display
                                                             dacEnableDac(pDev, Head);

                                                             // also make sure the cursor image is updated
                                                             pDev->Dac.CrtcInfo[Head].UpdateFlags &= (~UPDATE_HWINFO_DAC_CURSOR_ENABLE);
                                                             pDev->Dac.CrtcInfo[Head].UpdateFlags |= (UPDATE_HWINFO_DAC_CURSOR_IMAGE_NEW |
                                                                                                                                       UPDATE_HWINFO_DAC_CURSOR_DISABLE);
                                                     }
                                             }

                                             //Toshiba specefic call (DPMS, hotkeys)
                                             //Backlight enabled inside dacEnableDac for other mobiles
                                             if (pDev->Power.MobileOperation == 2)
                                             { 
                                                     U032    EAX, EBX, ECX, EDX;
                                                     EAX = 0x4F10;
                                                     EBX = 0x0001; //wake up
                                                     EDX = 0x0000;
                                                     ECX = 0x0000;
                                                     status = osCallVideoBIOS(pDev, &EAX, &EBX, &ECX, &EDX, NULL);
                                             }
                                       }
                            // enter power level
                            mcPowerState(pDev, MC_POWER_LEVEL_4, 0);
                        }                
                        break;
                    // suspend-resume or critical-resume
                    case 2:
                    case 3:
                        if (pDev->Power.State == MC_POWER_LEVEL_4)
                        {
                            // leave power level
                            mcPowerState(pDev, MC_POWER_LEVEL_0, 0);
                            //
                            //  Windows 98 will only trigger a modeset if you are at the
                            //  desktop .. so if you're at the login screen we're SOL
                            //  going to force a modeset.
                            //
                            if (pDev->Power.MobileOperation) 
                            {
                                pDev->Vga.Enabled = TRUE;   // We actually are already in HiRes
                                rmEnableHiRes(pDev, TRUE);                          
                            }
                            // turn monitor back on

                           // Bug 29711: if you hibernate on CRT, unplug CRT, boot back up
                                       // on LCD, the LCD blooms. When we get to this function, we do
                                       // a modeset on the CRT because the pVidLutCurDacs are setup for
                                       // head A = CRT and head B = Nothing.  If we reset the chip in 
                                       // mcPowerState, we turn off the flat panel clocks.  When we call 
                                       // Wake Up in this function, the LCD turns on, but the clocks
                                       // do not. This causes blooming.  
                                       // So to fix this, if we've reset the chip, and we've boot to  
                                       // an LCD, don't power on the panel!  The Reset flag is turned
                                       // off at the end of dacSetFlatPanelMode in dacfp.c.
                                       if(pDev->Dac.DevicesEnabled & DAC_PANEL_CLK_RESET)
                                       {
                                             if(pDev->Dac.BootDevicesBitMap & DAC_DD_MASK_IN_MAP)
                                             {
                                                     DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: Set Monitor Power called after chip reset without panel clocks setup!\n");
                                                     break;
                                             }
                                       }

                                       for(Head = 0; Head < MAX_CRTCS; Head++)
                                       {
                                             if ((GETDISPLAYTYPE(pDev, Head) != DISPLAY_TYPE_NONE))
                                             {
                                                   // Enable display
                                                   dacEnableDac(pDev, Head);

                                                   // also make sure the cursor image is updated
                                                   pDev->Dac.CrtcInfo[Head].UpdateFlags &= (~UPDATE_HWINFO_DAC_CURSOR_ENABLE);
                                                   pDev->Dac.CrtcInfo[Head].UpdateFlags |= (UPDATE_HWINFO_DAC_CURSOR_IMAGE_NEW |
                                                                                                                               UPDATE_HWINFO_DAC_CURSOR_DISABLE);
                                             }
                                       }

                                       //Toshiba specefic call (DPMS, hotkeys)
                                       //Backlight enabled inside dacEnableDac for other mobiles
                                       if (pDev->Power.MobileOperation == 2)
                                       { 
                                             U032    EAX, EBX, ECX, EDX;
                                             EAX = 0x4F10;
                                             EBX = 0x0001; //wake up
                                             EDX = 0x0000;
                                             ECX = 0x0000;
                                             status = osCallVideoBIOS(pDev, &EAX, &EBX, &ECX, &EDX, NULL);
                                       }
                        }
                        break;
                }
                REG_WR08(NV_PRMCIO_CRX__COLOR, 0x991F & 0xFF); 
                REG_WR08(NV_PRMCIO_CRX__COLOR+1, (0x991F >> 8) & 0xFF);
                ////OEMDisableExtensions();
            }
        }
        // return status = RM_OK, this will ensure carry clear (must do)
        break;
     case CRIT_REBOOT_NOTIFY2:
        // Disable interrupts
        pDev = NvDBPtr_Table[0];
        pOsHwInfo = (PWIN9XHWINFO) pDev->pOsHwInfo;
        REG_WR32(NV_PMC_INTR_EN_0, 0);
        //
        // There is a bug in the current mobile BIOS where it does not explicitly program
        // the FP clock routing register.  It relies on the fact that it resets to 0, which
        // means the FP should be using HeadA.  Since the OS uses HeadB, a restart of the
        // system (warm) results in a bloom screen.  Until the BIOS fixes this, force
        // it back to HeadA as we leave the OS.
        //
        // REMOVE REMOVE REMOVE 
        //
        if (pDev->Power.MobileOperation)
        {
            // Check the BIOS rev
            if ((pDev->Chip.BiosRevision & 0xFF00FFFF) < 0x03000020)
            {
                // Not a multi-head BIOS
                dacDisableDac(pDev, 1);             // clear HeadB's device in case the BIOS doesn't
                REG_WR32(NV_PRAMDAC_SEL_CLK, 0);    // force the DFP back to HeadA
            }
        }
        else
           // turn on scaling mode for Windows reboot screen--BIOS won't reprogram FP regs
            DAC_FLD_WR_DRF_DEF(_PRAMDAC, _FP_TG_CONTROL, _MODE, _SCALE, 0); // desktop BIOS uses head A

        break;
     case KERNEL32_SHUTDOWN:
     case SYS_VM_TERMINATE:
        DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: Sys_VM_Terminate\n\r");
        if (osPhase == SYS_VM_TERMINATE)
        {
            DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: Sys_VM_Terminate again!\n\r");
            //DBG_BREAKPOINT();
        }
        osPhase = SYS_VM_TERMINATE;

        //
        // Terminate all active devices
        //        
        for (i=0;i<MAX_INSTANCE;i++)
        {
          // Check if this device exists.
          if (NvDBPtr_Table[i] != NULL)
          {
            pDev = NvDBPtr_Table[i];
            pOsHwInfo = (PWIN9XHWINFO) pDev->pOsHwInfo;
          
            osEnsureDeviceEnabled(pDev);
            // Make sure the device is enabled for this. During a shutdown I have found Win98 disabling the memory
            // map of the device.
            // How about if Win98 decides to unmap us. Huh?
            // Just unhook the interrupt and avoid the stateNv(STATE_DESTROY) in that case.
            {
              U032 we_are_mapped;
              U032 pci_address;
              
              // Check if the device has become unmapped. If so stop servicing.
              // Read the 1st base register.
              pci_address = pDev->Mapping.nvBusDeviceFunc;
              _asm mov  eax, pci_address  
              _asm or   eax, 0x80000010 
              _asm mov  dx, 0xcf8
              _asm out  dx, eax
              _asm mov  dx, 0xcfc
              _asm in   eax,dx
              _asm mov  we_are_mapped,eax
              
              DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM: Disable and Unhook interrupt on device ", i);
              if(we_are_mapped)
              {
                  if (rmInit == DEVICE_INIT)  // only if we were init'd should we have anything to do
                  {
                      //
                      // Nothing to callback to.
                      //
                      pOsHwInfo->dllIpcOffset = 0;
                      pOsHwInfo->dllIpcSelector    = 0;
                      pOsHwInfo->dllNotifyOffset   = 0;
                      pOsHwInfo->dllNotifySelector = 0;
                      osCallbackList    = NULL;
                  }
                
                  stateNv(pDev, STATE_DESTROY);
                  if (pDev->Mapping.hpicIRQ)
                     VPICD_Force_Default_Behavior(pDev->Mapping.hpicIRQ);
                  pDev->Mapping.hpicIRQ = NULL;
                  pDev->DeviceDisabled = TRUE;

                  // Turn off other devices for shutdown
                  if (pDev->Power.MobileOperation)
                  {
                  U032 Broadcast;
                  U032 VGAHead;
                  U032 i;
                  U032 CR3B;
                  U008 Lock0;
                  U008 Lock1;
               
                     Lock0 = UnlockCRTC(pDev, 0x0);
                     Lock1 = UnlockCRTC(pDev, 0x1);
                     CRTC_RD(0x3C, Broadcast, 0); 
                     Broadcast &= 0x02;
//                     if (!Broadcast)
                     {
                        // Determine which head is the Primary device
                        for (i = 0, VGAHead = 0; i < pDev->Dac.HalInfo.NumCrtcs; i++)
                        {
                           if (pDev->Dac.CrtcInfo[i].PrimaryDevice == TRUE)
                           {
                              VGAHead = i;
                              break;
                           }
                        }
                     CRTC_RD(0x3B, CR3B, VGAHead^0x01);
                     CRTC_WR(0x3B, CR3B & 0x0F, VGAHead^0x01);
                     } 
                     RestoreLock(pDev, 0x0, Lock0);
                     RestoreLock(pDev, 0x1, Lock1);
                  }
              }
              else
              {
                  if (pDev->Mapping.hpicIRQ)
                     VPICD_Force_Default_Behavior(pDev->Mapping.hpicIRQ);
                  pDev->Mapping.hpicIRQ = NULL;
                  pDev->DeviceDisabled = TRUE;
              } 
            }
            
            //
            // Clear the device structure
            //
            NvDBPtr_Table[i] = NULL;
          } // This device exists.
        } // for all devices
               
        break;
    case SYSTEM_EXIT:
        DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: System_Exit\n\r");
        // nothing to do here.
        break;
    case SYS_CRITICAL_EXIT:
        DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: Sys_Critical_Exit\n\r");
#ifdef DEBUG
        DbgObject[dbgObject_inuse].pClose(NULL);          
#endif            
        osPhase = SYS_CRITICAL_EXIT;
        rmInit = 0;
        break;
    default:
        // DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM: Unhandled rmControl msg: ", Msg);
        break;
    }
    return (status);
}

//---------------------------------------------------------------------------
//
//  Function to determine if Windows is terminating.
//
//---------------------------------------------------------------------------
BOOL osTerminating (VOID)
{
   return((osPhase == SYS_CRITICAL_EXIT) ||
      (osPhase == SYSTEM_EXIT) ||
      (osPhase == SYS_VM_TERMINATE));
}

//---------------------------------------------------------------------------
//
//  Resource Manager service functions.
//  These are called from the miniVDD
//
//---------------------------------------------------------------------------

//  response to PreHiResToVga. We are going to full screen DOS.
U032 CDECL rmEnableVga
(
    PHWINFO pDev,
    BOOL PrimaryDevice
)
{
    U032 page;
    U032 size;
    U032 i, VGAHead;
    U032 status;
    U032 *fb_sv;
    U008 data8;
    
    // Indicate that we are going to a DOS Box.
    pDev->Vga.DOSBox = TRUE;
    
    rmInService++;
    stateNv(pDev, STATE_UNLOAD);
    
//  clear the screen: if we're shutting down after an re-install, we don't want to leave a mess on the screen 
//  between now and when Windows puts up the "Windows is restarting" message.
    page = pDev->Framebuffer.HalInfo.Start[pDev->Framebuffer.Current] / 4;
    size = (pDev->Dac.HalInfo.Depth / 8) * pDev->Framebuffer.HalInfo.HorizDisplayWidth * pDev->Framebuffer.HalInfo.VertDisplayWidth / 4;
    for (i = 0; i < size; i++)
    {
        fbAddr->Reg032[page + i] = 0;
    }
    pDev->Vga.Enabled = TRUE;
    stateNv(pDev, STATE_LOAD);
    
    // In VGA mode, set flag for BIOS to program TV encoder
    REG_WR08(NV_PRMCIO_CRX__COLOR, 0x571F & 0xFF); 
    REG_WR08(NV_PRMCIO_CRX__COLOR+1, (0x571F >> 8) & 0xFF);
    CRTC_RD(NV_CIO_CRE_SCRATCH1__INDEX, data8, 0);
    CRTC_WR(NV_CIO_CRE_SCRATCH1__INDEX, data8 & ~0x02, 0);  // clear bit 1 (OK to program TV encoder)
    REG_WR08(NV_PRMCIO_CRX__COLOR, 0x991F & 0xFF); 
    REG_WR08(NV_PRMCIO_CRX__COLOR+1, (0x991F >> 8) & 0xFF);
    
    if (PrimaryDevice && pDev->Vga.Enabled)
    {
            // Copy instance memory to save area.
            // The first 64k is BIOS image (looking backwards from the top). If the BIOS does
            // not get POSTed after standby (BIOS POST can be turned off in BIOS SETUP), 
            // we need to have a copy to use to re-init the device (we use the BIOS init tables),
            // so we'll copy the BIOS image into our save buffer.
            status = osAllocMem((VOID **)&inst_sv, INST_COPY);
            fb_sv = inst_sv;
            if (!status)
            {
                for(i=0; i < INST_COPY/4; i++)
                {
                    *fb_sv++ = REG_RD32(NV_PRAMIN_DATA032(i)); // from the top
                                                               // this actually decrements from the top
                }            
                InstanceMemSaved = TRUE;    // let rmEnableHires restore it
            }
            nvHalFbControl(pDev, FB_CONTROL_INSTWR_DISABLE);
    }
    else
    {
        // For the secondary displays, disable the interrupts.
        REG_WR32(NV_PMC_INTR_EN_0, 0);
        pDev->DeviceDisabled = 1;
    }

//    pDev->Dac.DeviceProperties.InferAuxDev = TRUE;
    pDev->Dac.DeviceProperties.LastSwitchEvent = SWITCH_FROM_FS_DOS;

    DisableMobileHotkeyHandling(pDev);

    // Determine which head is the Primary device
    for (i = 0, VGAHead = 0; i < pDev->Dac.HalInfo.NumCrtcs; i++)
    {
        if (pDev->Dac.CrtcInfo[i].PrimaryDevice == TRUE)
        {
            VGAHead = i;
            break;
        }
    }

    pDev->Dac.CrtcInfo[0].MonitorPowerSetMode = FALSE;
    pDev->Dac.CrtcInfo[1].MonitorPowerSetMode = FALSE;

    // Check the BIOS type: multihead or single head
    // Currently mobile is multihead and desktop is single head.
    // Sometimes Cr44 gets switched to "3" without Head2RegOwner getting update,
    // and the update of the register is skipped. We can't let this happen, so set to UNKNOWN.
    pDev->Dac.HalInfo.Head2RegOwner = HEAD2_OWNER_UNKNOWN; 
    if (!pDev->Power.MobileOperation)
    {
        // Not a multi-head BIOS
        if (pDev->Dac.CrtcInfo[0].pVidLutCurDac != NULL)
        {
            // if going to DOS FS, the BIOS has already set mode. Doing this will kill the TV encoder.
//            dacDisableDac(pDev, 1);    // blank display on the other head
            EnableHead(pDev, 0);       // but leave pointing to head 0
        }
    }
    else
    {
        // Is a multi-head BIOS
        if (pDev->Dac.CrtcInfo[VGAHead].pVidLutCurDac != NULL)
        {
            U008 lockA, lockB, Cr3B[2];
            
            // Read both Cr3B registers
            lockA = UnlockCRTC(pDev, 0);
            lockB = UnlockCRTC(pDev, 1);
            CRTC_RD(0x3B, Cr3B[0], 0);
            CRTC_RD(0x3B, Cr3B[1], 1);

            RestoreLock(pDev, 0, lockA);
            RestoreLock(pDev, 1, lockB);
            
            // check to see if we need to disable second head
            if (!pDev->Dac.BroadcastSupport)  // BIOS/chip revision does not support broadcast mode
            {
                U032 SecondaryHead = VGAHead^1;

                // only turn on TV flag if we intend to disable TV in dacDisableDac().
                if(((PVIDEO_LUT_CURSOR_DAC_OBJECT)pDev->Dac.CrtcInfo[SecondaryHead].pVidLutCurDac)->DisplayType == DISPLAY_TYPE_TV)
                {
                    pDev->Dac.DevicesEnabled |= (DAC_TV_ENABLED); // force disabling of TV in case we already
                                                                  // disabled, but BIOS reenabled during detection.
                }
                
                dacDisableDac(pDev, VGAHead^1);    // blank display on the other head
                EnableHead(pDev, VGAHead);         // should be unnecessary, but just in case

                // Clear out the secondary head nibble
                lockA = UnlockCRTC(pDev, SecondaryHead);
                CRTC_WR(0x3B, (Cr3B[SecondaryHead]&0x0F), SecondaryHead);
                RestoreLock(pDev, SecondaryHead, lockA);
            }
            else
            {
                // We do have a Broadcast aware VBIOS/Chip.  
                // We need to make sure to set Broadcast mode here if not set.
                // Do both heads have active displays?
                if((Cr3B[0]&0xF0) && (Cr3B[1]&0xF0))
                {
                    // 
                    // Both displays are active, so enable broadcast mode
                    lockA = UnlockCRTC(pDev, 0);
                    CRTC_WR(NV_CIO_CRE_H2OWNS, 0x4, 0);
                    RestoreLock(pDev, 0, lockA);
                    pDev->Dac.HalInfo.Head2RegOwner = HEAD2_OWNER_UNKNOWN;
                }
  
            }
            
            
            // Save off this information here for rmEnableHires
            if(pDev->Dac.DeviceProperties.Spanning)
            {
                pDev->Dac.NonBroadcastModeStoredCr3B[0] = Cr3B[0]&0xF0;
                pDev->Dac.NonBroadcastModeStoredCr3B[1] = Cr3B[1]&0xF0;
            }
        }
    }

    rmInService--;
    return (RM_OK);
} // end of rmEnableVga


//  Called from mini-VDD PreVgaToHiRes. We are returning to Windows from full screen DOS.
//  Or called from NVRM_API_CONFIG_VGA. The display driver is asking us to go to hires.
//  Note: we should probably only call this from the mini-VDD, meaning the display driver should do the VDD call instead
//  of calling us directly.
U032 CDECL rmEnableHiRes
(
    PHWINFO pDev,
    BOOL PrimaryDevice
)
{
    PWIN9XHWINFO pOsHwInfo = (PWIN9XHWINFO) pDev->pOsHwInfo;
    U032 page;
    U032 size;
    U032 i;
    U032 data;
    U032 *fb_sv;
    U032 new_config = 0;
    PVIDEO_LUT_CURSOR_DAC_OBJECT pVidLutCurDac;
    U008 lockA, lockB, Cr3B[2];


    if (!pDev->Vga.Enabled)
    {
        //
        // We're already in graphics mode or VGA is Windows display,
        // so do nothing.
        //
        return (RM_OK);
    }

    // Clear up CR44 if Broadcast mode was set.
    // The cached state, Head2RegOwner, can be incorrect if
    // display switch to Broadcast mode happens in VBIOS/rmEnableVGA().
    if (REG_RD_DRF(_PBUS, _DEBUG_1, _DISP_MIRROR))
    {
        // Determine which head is the Primary device
        for (i = 0; i < pDev->Dac.HalInfo.NumCrtcs; i++)
        {
            if (pDev->Dac.CrtcInfo[i].PrimaryDevice == TRUE)
            {
                break;
            }
        }
        
        pDev->Dac.HalInfo.Head2RegOwner = HEAD2_OWNER_UNKNOWN;
        EnableHead(pDev, i);
    }

    // restore the displays for both Cr3B registers
    lockA = UnlockCRTC(pDev, 0);
    lockB = UnlockCRTC(pDev, 1);
    CRTC_RD(0x3B, Cr3B[0], 0);
    CRTC_RD(0x3B, Cr3B[1], 1);
    
    // Restore Cr3B information if we were in extended mode
    if(pDev->Dac.DeviceProperties.Spanning)
    {
        CRTC_WR(0x3B, (pDev->Dac.NonBroadcastModeStoredCr3B[0] | (Cr3B[0]&0x0F)), 0);
        CRTC_WR(0x3B, (pDev->Dac.NonBroadcastModeStoredCr3B[1] | (Cr3B[1]&0x0F)), 1);
    }

    RestoreLock(pDev, 0, lockA);
    RestoreLock(pDev, 1, lockB);

    // Update DeviceEnabled Flags if we're switching back from DOS.
    // If we're switching back from a power save state, the 
    // DevicesEnabled will be updated in mcPowerState().
    if ((pDev->Power.MobileOperation) && 
        (pDev->Dac.DeviceProperties.LastSwitchEvent == SWITCH_FROM_FS_DOS))
    {
        //Set startup display options based on BIOS CR3B settings.
        pDev->Dac.DevicesEnabled &= ~(DAC_ALL_DEVICES_ENABLED);
        if (Cr3B[0] & 0x20) 
        {
            // CRT tracking is not used yet.
            //pDev->Dac.DevicesEnabled |= DAC_CRT_ENABLED;
        }
        else if (Cr3B[0] & 0x40) 
        {
            pDev->Dac.DevicesEnabled |= DAC_TV_ENABLED;
        } 
        
        if (Cr3B[1] & 0x10) 
        {
            pDev->Dac.DevicesEnabled |= DAC_PANEL_ENABLED;
        }
    }

//    dacDetectDevices(pDev, FALSE, pDev->Dac.DeviceProperties.OverrideSBIOSDevs);
    dacDetectDevices(pDev, FALSE, TRUE);

    if (PrimaryDevice)
    {
        //
        // Reenable access to instance memory
        //
        nvHalFbControl(pDev, FB_CONTROL_INSTWR_ENABLE);

        if (InstanceMemSaved)
        {
            // Copy from save area to framebuffer instance memory
            // The first 64k is BIOS image (looking backwards from the top). If the BIOS does
            // not get POSTed after standby (BIOS POST can be turned off in BIOS SETUP), 
            // we need to have a copy to use to re-init the device (we use the BIOS init tables),
            // so we'll copy the BIOS image into our save buffer.
            fb_sv = inst_sv;
            for(i=0; i < INST_COPY/4; i++)
            {
                 data = *fb_sv++;
                 REG_WR32(NV_PRAMIN_DATA032(i), data);
            }            
            osFreeMem((VOID *)inst_sv);
            InstanceMemSaved = FALSE;
        }
    }
    else
    {
        // For secondary devices, enable interrupts.
        REG_WR32(NV_PMC_INTR_EN_0, pDev->Chip.IntrEn0);
        pDev->DeviceDisabled = 0;
    }
    
    //
    // Save/Reload state info.
    //
    rmInService++;
    
    // mcPowerState will call stateNv during ACPI resume. Don't do it twice.
    // (rmUsrApi will also set back to Vga.Enabled, even tho mcPowerState it clears it to load Hires state; 
    // this is so Win2k can avoid making a second call to load state.)
    if (resume == 0) 
    {
        stateNv(pDev, STATE_UNLOAD);
        pDev->Vga.Enabled = FALSE;
        
        // clear display memory (in case we lost power in standby)
        page = pDev->Framebuffer.HalInfo.Start[pDev->Framebuffer.Current] / 4;
        size = (pDev->Dac.HalInfo.Depth / 8) * pDev->Framebuffer.HalInfo.HorizDisplayWidth * pDev->Framebuffer.HalInfo.VertDisplayWidth / 4;
        for (i = 0; i < size; i++)
        {
            fbAddr->Reg032[page + i] = 0;
        }

        stateNv(pDev, STATE_LOAD);
    }

    //Check global powerup pending flag. Win9x display driver will do the modeset
    //call right after this call so we can bypass extraneous modeset call here.
//!!! Commented this out because there was no display on LCD after monitor poweroff from clone
//!!!    if(!(pDev->Power.Flags & DAC_POWER_PENDING))
    {
        //
        // Check if the we need to do the modesets here or not. It's possible the
        // monitor power routines have already done it. Also, if we're setting the
        // panel call the dacSetModeStart/dacSetModeEnd which control the backlight.
        //
        pVidLutCurDac = (PVIDEO_LUT_CURSOR_DAC_OBJECT)pDev->Dac.CrtcInfo[0].pVidLutCurDac;
        if (pVidLutCurDac && (pDev->Dac.CrtcInfo[0].MonitorPowerSetMode == FALSE))
        {
            switch (pVidLutCurDac->DisplayType)
            {
                case DISPLAY_TYPE_MONITOR:
                    new_config |= 0x00000001; break;
                case DISPLAY_TYPE_TV:
                    new_config |= 0x00000100; break;
                case DISPLAY_TYPE_FLAT_PANEL:
                    new_config |= 0x00010000; break;
            }
        }
        pVidLutCurDac = (PVIDEO_LUT_CURSOR_DAC_OBJECT)pDev->Dac.CrtcInfo[1].pVidLutCurDac;
        if (pVidLutCurDac && (pDev->Dac.CrtcInfo[1].MonitorPowerSetMode == FALSE))
        {
            switch (pVidLutCurDac->DisplayType)
            {
                case DISPLAY_TYPE_MONITOR:
                    new_config |= 0x00000001; break;
                case DISPLAY_TYPE_TV:
                    new_config |= 0x00000100; break;
                case DISPLAY_TYPE_FLAT_PANEL:
                    new_config |= 0x00010000; break;
            }
        }

        // If a flat panel is involved, start the modeset
        if (new_config & 0x00010000)
            dacSetModeStart(pDev, (new_config | NV_CFGEX_DISPLAY_CHANGE_BRACKETS_MODESET));

        // Do modesets on active heads (if SET_MONITOR_POWER hasn't already)
        if (pDev->Dac.CrtcInfo[0].MonitorPowerSetMode == FALSE)
        {
            pVidLutCurDac = (PVIDEO_LUT_CURSOR_DAC_OBJECT)pDev->Dac.CrtcInfo[0].pVidLutCurDac;
            if (pVidLutCurDac && (pVidLutCurDac->DisplayType != DISPLAY_TYPE_NONE))
            {
                dacSetModeFromObjectInfo(pDev, pVidLutCurDac);
                pDev->Dac.CrtcInfo[0].MonitorPowerSetMode = TRUE;
            }
        }
        if (pDev->Dac.CrtcInfo[1].MonitorPowerSetMode == FALSE)
        {
            pVidLutCurDac = (PVIDEO_LUT_CURSOR_DAC_OBJECT)pDev->Dac.CrtcInfo[1].pVidLutCurDac;
            if (pVidLutCurDac && (pVidLutCurDac->DisplayType != DISPLAY_TYPE_NONE))
            {
                dacSetModeFromObjectInfo(pDev, pVidLutCurDac);
                pDev->Dac.CrtcInfo[1].MonitorPowerSetMode = TRUE;
            }
        }

        // If a flat panel was involved, end the modeset
        if (new_config & 0x00010000)
            dacSetModeEnd(pDev, (new_config | NV_CFGEX_DISPLAY_CHANGE_BRACKETS_MODESET));
    }

    EnableMobileHotkeyHandling(pDev);

    rmInService--;

    // If we resumed from power save, the cursor is disabled. Normally we expect Windows to re-enable.
    if (resume)
    {
        pDev->Dac.CrtcInfo[0].CursorType = DAC_CURSOR_TWO_COLOR_XOR;   // Enable cursor, because Windows won't.
        resume = 0;
    }                        

    pDev->Vga.Enabled = FALSE;
    pDev->Vga.DOSBox = FALSE;
    // Now Vga.Enable = FALSE. If we are resuming from power management, it would have been TRUE, and we might have processed
    // an IRQ, in which case the IRQ process would have returned with interrupts disabled, so we should now reenable.
    REG_WR32(NV_PMC_INTR_EN_0, pDev->Chip.IntrEn0);

    // BIOS uses legacy readback, RM uses standard readback
    if (pDev->Dac.EncoderType == NV_ENCODER_BROOKTREE_871) 
    {
        dacTV871Readback(pDev, 0, 1);  // set non-legacy readback mode
    }

    return (RM_OK);
} // end of rmEnableHiRes

VOID RmEnableInterrupts(
    PHWINFO pDev
)
{
    //DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVGFX: Enabling NV interrupts...\n");
    REG_WR32(NV_PMC_INTR_EN_0, pDev->Chip.IntrEn0);

} // end of RmEnableInterrupts();

//---------------------------------------------------------------------------
//  rmConfigStart
//  Response to Plug and Play handler's ConfigStart message.
//  Perform physical to linear address translation and init the resource manager
//  This function takes globals nvPhys and fbPhys and translates addresses to nvAddr and fbAddr,
//  and translates nvIRQ to pDev->Mapping.Intline.
//
//---------------------------------------------------------------------------
U032 CDECL rmConfigStart(PHWINFO pDev)
{
    PWIN9XHWINFO pOsHwInfo = (PWIN9XHWINFO) pDev->pOsHwInfo;
    RM_STATUS status;

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: ConfigStart\n\r");
    
    //
    // Remap address of NV (16meg) ?
    //
    if (pDev->Mapping.nvPhys != pDev->Mapping.PhysAddr)
    {
        (U032)pDev->Mapping.PhysAddr = pDev->Mapping.nvPhys;
//  the mapping will take place in nvState(state_init)
//        nvAddr = vmmMapPhysToLinear(pDev->Mapping.PhysAddr, 0x01000000, 0);
    }
        
    //
    // Remap address of NV framebuffer (16meg)
    //
    if (pDev->Mapping.fbPhys != pDev->Mapping.PhysFbAddr)
    {
        pDev->Mapping.PhysFbAddr = pDev->Mapping.fbPhys;
//  the mapping will take place in nvState(state_init)
//        fbAddr = vmmMapPhysToLinear(pDev->Mapping.PhysFbAddr, 0x01000000, 0);
    }        
    //
    // Rehook IRQ?
    //
    if (pDev->Mapping.nvIRQ != pDev->Mapping.IntLine)
    {
        pDev->Mapping.IntLine = pDev->Mapping.nvIRQ;
    }
    
    status = RM_OK;
    
    //
    // Grab state of the display first.
    // 
    osDisplayInfo.HdrSize = sizeof(DISPLAYINFO);
    vddGetDisplayInfo(pOsHwInfo->osDeviceHandle, &osDisplayInfo, sizeof(DISPLAYINFO));
#ifdef RM_STATS
    //
    // Init all per device data structures.
    //
    pDev->Statistics.MemAlloced = 0;
    pDev->Statistics.MemLocked  = 0;
#endif    
    status = stateNv(pDev, STATE_INIT);
    //
    // For Windows, come up as VGA until display drivers
    // disables it thus enabling high-res.
    //
    pDev->Vga.Enabled = TRUE;
    if (!status) status = stateNv(pDev, STATE_LOAD);
    
    return (status);
    
}

//---------------------------------------------------------------------------
//  rmConfigStop 
//  Currently used when we get an ACPI D3 message.
//---------------------------------------------------------------------------
U032 CDECL rmConfigStop(PHWINFO pDev)
{
    PWIN9XHWINFO pOsHwInfo = (PWIN9XHWINFO) pDev->pOsHwInfo;

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: ConfigStop\n\r");
    //
    // Nothing to callback to.
    //
    pOsHwInfo->dllIpcOffset      = 0;
    pOsHwInfo->dllIpcSelector    = 0;
    pOsHwInfo->dllNotifyOffset   = 0;
    pOsHwInfo->dllNotifySelector = 0;
    osCallbackList    = NULL;
    
    //
    // Are we still mapped? If secondary display, probably not.
    // Fifo interrupts are still enabled. This is bad, but not too bad, as long as the chip doesn't interrupt.
    //
    if (REG_RD32(NV_PMC_BOOT_0) != -1)
    {
        stateNv(pDev, STATE_UNLOAD);  
        stateNv(pDev, STATE_DESTROY);
    }
    // unhook interrupt
    if (pDev->Mapping.hpicIRQ)
      VPICD_Force_Default_Behavior(pDev->Mapping.hpicIRQ);
    pDev->Mapping.hpicIRQ = NULL;

    // Free per device RM memory if any.
    return (RM_OK);
}

//---------------------------------------------------------------------------
//
//  Resource Manager API for VxDs and user mode clients. (e.g. display driver)
//
//---------------------------------------------------------------------------

//
// System level API for other VxDs.
//
RM_STATUS CDECL rmSysAPI
(
    U032 hReg,
    U032 Msg,
    V032 Param1,
    V032 Param2
)
{
    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: System Service to Resource Manager\n\r");
    return (RM_OK);
}

int osGetNextAvailableInstance
(
  VOID
)
{
  int i;

  for(i=0;i<MAX_INSTANCE;i++)
    if(NvDBPtr_Table[i] == NULL)
      return (i);
  // If there are no NULL entries available ... return -1
  return (-1);
}

//
// User mode client API.
//
// pcs->CRS.Client_EAX will have a pointer to the RM_STRUCT containing the function and device.
RM_STATUS __fastcall rmUsrAPI
(
    U032 hVM,
    PCS  pcs
)
{
    PWIN9XHWINFO pOsHwInfo = NULL;

    U032          i;
    DESCRIPTOR    desc;
    U032          LinAddr;
    U032          PageBase;
    U032          Page;
    U032          PageCount;
    U032          PTE;
    static U032   grCanvasID;
    static U032   grClipCount;
    static U008   NopMask;
    PDMAPUSH_ADDRESS_INFO pDmaPushInfo=NULL;
    U032          status;
    U032          data;
    U032          data32;
    U032          *fb_sv;
//#ifdef BX_FIX
    CLIENT_STRUCT crs;          // another client_reg struct for internal use
//#endif
    MiniVDD_ParameterStruct *pMiniVDD_parameters;
    PHWINFO pDev;
    PRMUSRAPI_STRUCT rm_tempp;

    U032 Head = 0;  // need to pass this as a parameter?
    static U016   BxDev, Bx50, BxA8;   // BX bus & dev #, AGP enables
#ifdef DEBUG
    extern NvU32 dbgPowerSysState;
#endif


    extern U032           DmaPushOperation(U032, PDMAPUSH_ADDRESS_INFO);
    extern U032           DmaPushLock(U032, PDMAPUSH_ADDRESS_INFO);

    
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM: User Service to Resource Manager: VM handle = ", hVM);

    rm_tempp = (PRMUSRAPI_STRUCT) pcs->CRS.Client_EAX;
    pDev = NvDBPtr_Table[rm_tempp->device];
    pcs->CRS.Client_EAX = rm_tempp->function;

    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,     "                                        Service   = ", rm_tempp->function);
    if((rm_tempp->function == NVRM_API_ARCHITECTURE_32)||(rm_tempp->function==NVRM_API_ARCHITECTURE_16)){
        DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO,       "                                        Device not valid for NVARCH FUNCTION\n");
    }else{
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "                                        Device    = ", rm_tempp->device);
    }

    //This is really happens every time when RM started (NVRM_API_INIT).
    //I don't have any idea why our driver even worked before this fix.
    if( pDev != 0 )
    {
        pOsHwInfo = (PWIN9XHWINFO) pDev->pOsHwInfo;

        // It appears that Windows likes to disable our memory decode at random.
        // So ... we have to re-enable in this case.
        // Check if this device exists.
        if(pcs->CRS.Client_EAX != NVRM_API_POWER_UP)
            osEnsureDeviceEnabled(pDev);
    }

    //
    // Handle requested service.
    //
    switch (pcs->CRS.Client_EAX)
    {
        //*************************************************
        // Architecture "IOCTL" calls.
        //*************************************************
        case NVRM_API_ARCHITECTURE_32:
        case NVRM_API_ARCHITECTURE_16:
            {
                DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "                                        ioctl    = ", pcs->CRS.Client_EDX);
              if (pcs->CRS.Client_EDX <= NVRM_ARCH_MAX_IOCTL)
                NvArchRMDispatch(pcs->CRS.Client_EDX, (PVOID)pcs->CRS.Client_EBX);
            }  
            break;
        //*************************************************
        // These first functions are called by the mini-VDD
        //*************************************************
        //
        // Map NV addresses. 
        //
        case NVRM_API_INIT:

        {
          PHWINFO pDev;
          int     dev_instance;
          U032    physAddr;          
          // This macro will produce a divide by 0 error at compile time if the sizeof HWINFO
          // struct is not a multiple of 4. However, it does not help if some sub struct of
          // HWINFO is not aligned properly.
          ASSERT_NVINFO_SIZE;

          // See if the device has already been init'd (i.e. the secondary miniVDD is loaded for the 2nd head of a single device.
          pMiniVDD_parameters = (MiniVDD_ParameterStruct *)pcs->CRS.Client_ESI;
          physAddr = pMiniVDD_parameters->callrm_nvphys;
          for (i=0; i < MAX_INSTANCE; i++) // Check all entries in the table of devices
          {
              if (NvDBPtr_Table[i] == 0)
                  break;
              else
              {
                if (physAddr == NvDBPtr_Table[i]->Mapping.PhysAddr) // Have we already init'd this device?
                {
                    DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: API_INIT for secondary head\n");
                    return (RM_OK);     // We don't need to do anything
                }
                else
                    DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: API_INIT for secondary device\n");
              }
          }
          // This is a new device
          if (! (RmAllocDeviceInstance(&dev_instance)==RM_OK) )
          {
              DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: *** Cannot initialize device database\n");
          }else{

            pDev = NvDBPtr_Table[dev_instance];

            // setup the ptr to the OS Hw info
            pDev->pOsHwInfo = (VOID_PTR) &win9xHwInfo[dev_instance];
            pOsHwInfo = (PWIN9XHWINFO) pDev->pOsHwInfo;

            initStack_frame(pDev);
              
            // Delete the registry override for startup, so Control Panel will know we loaded.
            osDeleteRegistryValue(pDev->Registry.DBstrDevNodeDisplay, strStartUp);
            // Same goes for TV format
            osDeleteRegistryValue(pDev->Registry.DBstrDevNodeDisplay, strTVtype); 

            pMiniVDD_parameters = (MiniVDD_ParameterStruct *)pcs->CRS.Client_ESI;

            pDev->Mapping.nvPhys = pMiniVDD_parameters->callrm_nvphys;
            pDev->Mapping.fbPhys = (pMiniVDD_parameters->callrm_fbphys & 0xFFFF0000);
            pDev->Mapping.nvIRQ = pMiniVDD_parameters->callrm_irq_bus & 0xFF;
            pDev->Mapping.fbLength =(pMiniVDD_parameters->callrm_fbphys & 0xFFFF) << 16;
            pDev->Mapping.nvBusDeviceFunc = pMiniVDD_parameters->callrm_irq_bus & 0xFFFFFF00;

            //
            // This is actually a macro into a device's pDev
            //
            pOsHwInfo->osScheduled = 0;
            
            // This is only necessary for the primary display device. The secondary minivdd will step on this otherwise.
            if(pDev == NvDBPtr_Table[0])
               pOsHwInfo->miniVDDPostModeChange = (VOID *)pMiniVDD_parameters->callrm_modechange_callback;   // save callback address
   
            {
              int  char_count,string_size;
              char stringt[128]=NV4_REG_RESOURCE_MANAGER_SUBKEY;
              char stringt1[128]=RESOURCE_MANAGER_SUBKEY_ADDITION;
              
              osStringCopy(pDev->Registry.DBstrDevNodeDisplayNumber, (char *)pMiniVDD_parameters->callrm_registry_path);
              //osStringCopy(pDev->Registry.DBstrDevNodeDisplay, (char *)pMiniVDD_parameters->callrm_registry_path);
              //osStringCopy(pDev->Registry.DBstrRegistryDeviceParameters, (char *)pMiniVDD_parameters->callrm_registry_path); 
              //string_ptr = (U008 *) pcs->CRS.Client_EBX;
              //for(char_count=0;char_count<128;char_count++){
              //  pDev->Registry.DBstrDevNodeDisplayNumber[char_count] = (U008) *(string_ptr+char_count);
              //  pDev->Registry.DBstrDevNodeDisplay[char_count] = (U008) *(string_ptr+char_count);
              //}

              string_size = osStringLength(pDev->Registry.DBstrDevNodeDisplayNumber);
              pDev->Registry.DBstrDevNodeDisplayNumber[string_size] = '\\';
              string_size++;
              for(char_count=string_size;char_count < (128-string_size);char_count++){
                pDev->Registry.DBstrDevNodeDisplayNumber[char_count]=stringt1[char_count-string_size];
              }
              // Append the Resource Manager subkey string.("System")
              string_size = osStringLength(pDev->Registry.DBstrDevNodeDisplayNumber);
              pDev->Registry.DBstrDevNodeDisplayNumber[string_size] = '\\';
              string_size++;
              for(char_count=string_size;char_count < (128-string_size);char_count++){
                pDev->Registry.DBstrDevNodeDisplayNumber[char_count]=stringt[char_count-string_size];
              }
              osStringCopy(pDev->Registry.DBstrDevNodeDisplay,pDev->Registry.DBstrDevNodeDisplayNumber);
            }

            // Initialize these parameters.
            pDev->Dac.MobileEDID[0] = 1;    //Indicate that the buffer initialization hasn't happened yet.
            pDev->Dac.DevicesEnabled = 0;

            pDev->Dac.CrtcInfo[0].CursorHeight=32;
            pDev->Dac.CrtcInfo[0].CursorWidth=32;
            // This is called by the mini-VDD, so we must put the return values in the client-reg-struct so they'll be popped
            // back into the registers
            pcs->CRS.Client_EAX = rmConfigStart(pDev);                 // function return value
            pcs->CRS.Client_EBX = (U032) nvAddr; // return the address of NV in ebx
            pcs->CRS.Client_ECX = dev_instance + 1; // Return a 1 based handle to this device for the miniVDD.
            pcs->CRS.Client_EDX = (U032) fbAddr;    // return fb address
            pcs->CRS.Client_ESI = (U032) pDev->Dac.DFPPortID; // return DFP port ID
            pcs->CRS.Client_EDI = (U032) pDev->Video.HalInfo.CustomizationCode; // return customer mask
            
            pDev->fb_save = 0;    // JohnH
            //pDev->DBbiosAddr = 0; // JohnH
            pDev->DBdisplayModeHook = NULL;   

            /*KYH
            dmaPushPutOffset        = 0;
            dmaPushGetOffset        = 0;
            dmaPushGetOffsetRead    = 0;
            dmaPushJumpOffset       = 0;
            dmaPushJumpOffsetSet    = 0;
            KYH*/

            pDev->Dac.flickertoggle = 0;
            pDev->DBclass63VBlankList[0] = NULL;
            pDev->DBclass63VBlankList[1] = NULL;
            pDev->Dac.I2cIniFlag = FALSE;

            {
                DESCRIPTOR  desc;
                U032        pAddress;

                // alloc a locked page to be reloaded when the GART has been cleared
                vmmPageAllocate(0x1, PG_SYS, 0, 0, 0, 0, 0, 0, &desc.Desc64);
                pAddress = (U032)desc.Desc32[0];

                if (!vmmLinPageLock(pAddress >> RM_PAGE_SHIFT, 1, 0))
                {
                    DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM:Unable to lock detection page.\n\r");
                    //return (RM_ERR_DMA_MEM_NOT_LOCKED);
                    return (RM_OK);     // not fatal
                }
                p_agp_dummy_page = pAddress;
            }
    
            pDev->MediaPort.ImageBufferNumber = 0;    // used by canopus mediaport callback mechanism

          }//else
            // Abort Hard ....
          
        }
        break;

        // This supplies the miniVDD callback funtion with the number of heads on this device
        case NVRM_API_NUM_HEADS:
        {
            char tempString[256];
            U032 size,i;

            // Switch to the last pDev first
            for(i=0; i<MAX_INSTANCE; i++)
            {
              if(NvDBPtr_Table[i]==0)
                break;
              pDev = NvDBPtr_Table[i];
            }

            //Return DeviceInstance in EAX
            pcs->CRS.Client_EAX = i;

            //
            // Ugly but necessary.  We need to strip off the SYSTEM portion of our registry key
            // to look in the correct location for this DualView config setting.  Please revisit
            // this code and explicitly create a separate string for this registry location
            //
            osStringCopy(tempString, pDev->Registry.DBstrDevNodeDisplay);
            size = osStringLength(tempString);
            i = 2;
            while (tempString[size-i] != '\\') 
                   i++;
            tempString[size-i] = 0x00;  // truncate back to NVIDIA

            //
            // Only enable multiple CRTC's when the hardware supports more than one and the system
            // is configured to deal with it.
            //
            if (pDev->Dac.HalInfo.NumCrtcs > 1)
                pcs->CRS.Client_EBX = pDev->Dac.HalInfo.NumCrtcs;    // and get number of heads from HAL info
            else
                pcs->CRS.Client_EBX = 1;
        }
        break;

        // Remove the device specified in EBX
        case NVRM_API_EXIT:
        {
            BYTE  *x;

            pDev = NvDBPtr_Table[pcs->CRS.Client_EBX];
            rmConfigStop(pDev); 
            if (pOsHwInfo->osDeviceName != 0)
            {
                _HeapFree(pOsHwInfo->osDeviceName, 0);
                pOsHwInfo->osDeviceName = 0;
            }
            if (pDev->EDIDBuffer != 0)
            {
                osFreeMem((VOID *)pDev->EDIDBuffer);
                pDev->EDIDBuffer = 0;
            }

            x = (BYTE *) pDev;
            _HeapFree(x, 0);
            pDev = NULL;
            break;    
        }
            
        //
        //  Enable VGA
        //
        case NVRM_API_ENABLEVGA:
            // Set the context to the primary device.(This will always be the case).
            pDev = NvDBPtr_Table[0];
            rmEnableVga(pDev,TRUE /* is primary device */);
            pcs->CRS.Client_EAX = RM_OK;            // function return value
            break;

        //
        //  Disable VGA
        //
        case NVRM_API_ENABLEHIRES:
            // Set the context to the primary device.(This will always be the case).
            pDev = NvDBPtr_Table[0];
            rmEnableHiRes(pDev, TRUE /* is primary device */);
            pcs->CRS.Client_EAX = RM_OK;            // function return value
            
            break;
        
        //
        //  MiniVDD notifier that ACPI is enabled
        //
        case NVRM_API_ACPI_ENABLED:
            // Set DAC_PM_SUPPORTED bit to prevent mix with old style management.
            pDev->Power.Flags |= DAC_PM_SUPPORTED;

            break;

        //
        //  Map ACPI states into NV power states
        //
        
        //  ACPI D0 state
        case NVRM_API_POWER_UP:
            DBG_PRINT_STRING(DEBUGLEVEL_WARNINGS, "NVRM: ACPI D0 state\n");
#ifdef DEBUG
            dbgPowerSysState = 2;
#endif
            resume = 1;                                         // Signal resume has occurred, reenable cursor

            // What state are we powering up from?
            switch (pDev->Power.State)
            {
                case MC_POWER_LEVEL_1:
                    mcPowerState(pDev, MC_POWER_LEVEL_0, Head);
                    break;
                case MC_POWER_LEVEL_2:
                    mcPowerState(pDev, MC_POWER_LEVEL_0, Head);
                    break;
                case MC_POWER_LEVEL_3:
                    mcPowerState(pDev, MC_POWER_LEVEL_0, Head);
                    // Hack to fix the system BIOS (440BX only)-- restore AGP access
                    if (BxA8 != 0) // If AGP was enabled before power down, restore it
                    {
                        crs.CWRS.Client_BX  = BxDev;
                        crs.CWRS.Client_CX  = Bx50;       // Aperture Access Global Enable
                        crs.CWRS.Client_CX |= 0x20; // enable MDA
                        crs.CBRS.Client_AH  = 0xB1;       // PCI_FUNCTION_ID
                        crs.CBRS.Client_AL  = 0x0c;       // WRITE_CONFIG_WORD
                        crs.CWRS.Client_DI  = 0x50;       // NBX config reg      
                        PCI(&crs);
                        crs.CWRS.Client_CX  = BxA8;       // AGP, sideband enable; 2x
                        crs.CBRS.Client_AH  = 0xB1;       // PCI_FUNCTION_ID
                        crs.CBRS.Client_AL  = 0x0c;       // WRITE_CONFIG_WORD
                        crs.CWRS.Client_DI  = 0xA8;       // NBX config reg      
                        PCI(&crs);
                    }
                    pOsHwInfo->osScheduled = 0;         // this might have been set before we shut down
                    break;
            }

            //Return MobileOperation to VDD
            pcs->CRS.Client_EAX = pDev->Power.MobileOperation;

            pDev->Vga.Enabled = TRUE;   // set back to VGA (mcPowerState called stateNv(LOAD) for hires)
                                        // so that rmEnableHires will execute addition transition code.
            break;
        //  ACPI D3 state
        case NVRM_API_POWER_DOWN:
            DBG_PRINT_STRING(DEBUGLEVEL_WARNINGS, "NVRM: ACPI D3 state\n");
#ifdef DEBUG
            dbgPowerSysState = 1;
#endif
            if (pDev->Power.MobileOperation)
            {
                if (pDev->Power.MobileOperation == 2)// ||
                    //pDev->Power.MobileOperation == 4) // Toshiba or Compal?
                {
                    mcPowerState(pDev, MC_POWER_LEVEL_3, Head);
                }
                else
                {
                    mcPowerState(pDev, MC_POWER_LEVEL_2, Head);
                }
            }
            else
            {
                mcPowerState(pDev, MC_POWER_LEVEL_3, Head);

                // This gets called for all devices. We don't need to do it multiple times, but it shouldn't matter.
                // This only needs to happen if we have an AGP device.
                Bx50 = 0;
                BxA8 = 0;
                // Hack for 440BX system BIOS bug: save AGP enables on 440BX
                // Find the 440BX Bridge
                crs.CBRS.Client_AH  = 0xB1;           // PCI_FUNCTION_ID
                crs.CBRS.Client_AL  = 0x02;           // FIND_PCI_DEVICE
                crs.CWRS.Client_CX = 0x7190;          // Device ID
                crs.CWRS.Client_DX = 0x8086;          // Vendor ID
                crs.CWRS.Client_SI = 0;               // Index
                PCI(&crs);                              // return BH = bus, BL = dev num
                BxDev = crs.CWRS.Client_BX;
                if (crs.CBRS.Client_AH == 0x00)       // found? if not, nothing to do
                {
                    crs.CBRS.Client_AH  = 0xB1;       // PCI_FUNCTION_ID
                    crs.CBRS.Client_AL  = 0x09;       // READ_CONFIG_WORD
                    crs.CWRS.Client_DI  = 0x50;       // NBX config reg      
                    PCI(&crs);
                    Bx50 = crs.CWRS.Client_CX;
                    crs.CBRS.Client_AH  = 0xB1;       // PCI_FUNCTION_ID
                    crs.CBRS.Client_AL  = 0x09;       // READ_CONFIG_WORD
                    crs.CWRS.Client_DI  = 0xA8;       // NBX config reg      
                    PCI(&crs);
                    BxA8 = crs.CWRS.Client_CX;
            
                }
            }
            break;

        //  ACPI D1 state
        case NVRM_API_POWER_D1:
            DBG_PRINT_STRING(DEBUGLEVEL_WARNINGS, "NVRM: ACPI D1 state\n");
#ifdef DEBUG
            dbgPowerSysState = 1;
#endif
            if (pDev->Power.MobileOperation)
                mcPowerState(pDev, MC_POWER_LEVEL_1, Head);
            else
                mcPowerState(pDev, MC_POWER_LEVEL_1, Head);
            break;            
                                                
        // ACPI Hibernate
        case NVRM_API_HIBERNATE:
            DBG_PRINT_STRING(DEBUGLEVEL_WARNINGS, "NVRM: ACPI Hibernate\n");
#ifdef DEBUG
            dbgPowerSysState = 1;
#endif
            mcPowerState(pDev, MC_POWER_LEVEL_3, Head);
#ifdef DEBUG
            dbgDisableDevice2(&DbgPort);
#endif
            break;            
                                                
            
        //**************************************************************************
        // The remaining functions are called by "users", such as the display driver
        //**************************************************************************
        // Allocate page aligned DMA buffer.
        //
        case NVRM_API_ALLOC_PAGES:
            vmmGetCritSectionStatus();      // complete delayed releases of critical section
                                            // is this necessary in Win 95?
            vmmPageAllocate(pcs->CRS.Client_ECX, PG_SYS, 0, 0, 0, 0, 0, PAGELOCKED, &desc.Desc64);
            pcs->CRS.Client_EAX = desc.Desc32[0];
            pcs->CRS.Client_EDX = desc.Desc32[1];
            break;
        //
        // Free DMA buffer.
        //
        case NVRM_API_FREE_PAGES:
            vmmGetCritSectionStatus();
            vmmPageGetSizeAddr(pcs->CRS.Client_ECX, 0, &desc.Desc64);
            vmmPageUnLock(pcs->CRS.Client_ECX,
                          desc.Desc32[0],
                          0,
                          0);
            pcs->CRS.Client_EAX = vmmPageFree(pcs->CRS.Client_ECX, 0);
            break;
        //
        // Lock memory pages.
        //
        case NVRM_API_LOCK_PAGES:
            //
            // Map the buffer
            //
            vmmGetCritSectionStatus();
            LinAddr = vmmSelectorMapFlat(vmmGetSysVMHandle(), pcs->CRS.Client_EBX, 0)
                    + pcs->CRS.Client_ECX;
            PageCount = ((LinAddr & PAGEMASK) + pcs->CRS.Client_EDX + PAGESIZE - 2) >> PAGESHIFT;
            PageBase = PAGE(LinAddr);
            pcs->CRS.Client_EAX = TRUE;
            //
            // Scatter/gather lock the pages.
            //
            if (vmmPageCheckLinRange(PageBase, PageCount, 0) != PageCount)
            {
                pcs->CRS.Client_EAX = FALSE;
                break;
            }
            for (Page = 0; Page < PageCount; Page++)
            {
                //
                // Touch page to make sure it is in memory.  Win32s doesn't like
                // to play nicely with the VMMs memory manager.
                //
                NopMask |= *(volatile U008 *)LinAddr;
                LinAddr += PAGESIZE;
                if (!vmmLinPageLock(PageBase + Page, 1, 0))
                {
                    pcs->CRS.Client_EAX = FALSE;
                    break;
                }
                if (!vmmCopyPageTable(PageBase + Page, 1, (U032)&PTE, 0))
                {
                    pcs->CRS.Client_EAX = FALSE;
                    break;
                }
                if (!(PTE & P_PRES))
                {
                    pcs->CRS.Client_EAX = FALSE;
                    break;
                }
            }
            break;
        //
        // Unlock memory pages.
        //
        case NVRM_API_UNLOCK_PAGES:
            //
            // Unlock memory pages.
            //
            vmmGetCritSectionStatus();
            LinAddr   = vmmSelectorMapFlat(vmmGetSysVMHandle(), pcs->CRS.Client_EBX, 0)
                      + pcs->CRS.Client_ECX;
            PageCount = ((LinAddr & PAGEMASK) + pcs->CRS.Client_EDX + PAGESIZE - 2) >> PAGESHIFT;
            PageBase  = PAGE(LinAddr);
            //
            // Make sure the linear address range is valid.
            //
            PageCount = vmmPageCheckLinRange(PageBase, PageCount, 0);
            NopMask   = 0xFF;
            for (Page = 0; Page < PageCount; Page++)
            {
                *(volatile U008 *)LinAddr &= NopMask;
                LinAddr += PAGESIZE;
            }
            pcs->CRS.Client_EAX = vmmLinPageUnLock(PageBase, PageCount, 0) ? TRUE : FALSE;
            break;
        //
        // Map selector to linear address.
        //
        case NVRM_API_MAP_SELECTOR:
            pcs->CRS.Client_EAX = 0;
            if (pcs->CRS.Client_ECX == 0)
                break;
            //
            // Allocate a selector to map this buffer.
            //
            PageCount = ((pcs->CRS.Client_ECX + 0xFFF) & 0xFFFFF000) / 0x1000;
            vmmBuildDescriptorDWORDs(pcs->CRS.Client_EBX,
                                     PageCount,
                                     RW_DATA_TYPE,
                                     D_PAGE32,
                                     0,
                                     &desc.Desc64);
            if (pcs->CWRS.Client_DX == 0)
            {
                pcs->CRS.Client_EAX =
                    vmmAllocateGDTSelector(desc.Desc32[1],
                                           desc.Desc32[0],
                                           0);
            }
            else
            {
                pcs->CRS.Client_EAX =
                    vmmSetDescriptor(pcs->CWRS.Client_DX,
                                     hVM,
                                     desc.Desc32[1],
                                     desc.Desc32[0],
                                     0);
            }
            break;
        //
        // Register Windows Resource Manager callback routines.
        //
        case NVRM_API_IPC_CALLBACK:
            pOsHwInfo->dllIpcOffset   = pcs->CRS.Client_ECX;
            pOsHwInfo->dllIpcSelector = pcs->CWRS.Client_DX;
            break;
        case NVRM_API_NOTIFY_CALLBACK:
            pOsHwInfo->dllNotifyOffset   = pcs->CRS.Client_ECX;
            pOsHwInfo->dllNotifySelector = pcs->CWRS.Client_DX;
            break;
        //
        // Return far pointers to Resource Manager information structures.
        //
        case NVRM_API_MULTIDEVICE_MAP_RM_INFO:
          //
          // Map the DevInfo, channelInfo, & ClientInfo structures.
          //
          pcs->CRS.Client_EAX = RM_ERROR;
          {
            // Use this global to reflect whether this function has been called yet.
            // You must re-allocate sectors for the pDev's because they can be disabled and re-allocated
            // during dynamic enable/disables.
            //if (NvInfoSelectorsSelector == 0)
            {
              int device_count;
              int pDev_count=0;
              // Allocate selectors for each of the Devices.
              device_count = osGetNextAvailableInstance();
              // Set the global context to the first device.
              pDev = NvDBPtr_Table[pDev_count];
              
              // Allocate selectors for each of the device NvInfo structures.
              while(device_count != 0)
              {
                vmmBuildDescriptorDWORDs((DWORD)pDev,
                                         sizeof(HWINFO),
                                         RW_DATA_TYPE,
                                         D_DEF16,
                                         0,
                                         &desc.Desc64);
                NvInfoSelectors[pDev_count] =                         
                    vmmAllocateLDTSelector(hVM,
                                           desc.Desc32[1],
                                           desc.Desc32[0],
                                           1,
                                           0);
                device_count--;    
                pDev_count++;
                pDev = NvDBPtr_Table[pDev_count];
              }                             
            }
            // Allocate selectors for the array of NvInfo pointers.
            vmmBuildDescriptorDWORDs((DWORD)&NvInfoSelectors[0],
                                     (MAX_INSTANCE * sizeof(U032)),
                                     RW_DATA_TYPE,
                                     D_DEF16,
                                     0,
                                     &desc.Desc64);
            NvInfoSelectorsSelector =                         
                vmmAllocateLDTSelector(hVM,
                                       desc.Desc32[1],
                                       desc.Desc32[0],
                                       1,
                                       0);
            
            // Flag that this is done and send back a pointer to the array of selectors. 
            //nvInfoSelector = 0;
            pcs->CRS.Client_EAX = NvInfoSelectorsSelector;
            
            if (clientInfoSelector == 0)
            {
              //
              // Allocate a selector to map this buffer.
              //
              vmmBuildDescriptorDWORDs((DWORD)clientInfo,
                                       sizeof(CLIENTINFO) * NUM_CLIENTS,
                                       RW_DATA_TYPE,
                                       D_DEF16,
                                       0,
                                       &desc.Desc64);
              clientInfoSelector =
                  vmmAllocateLDTSelector(hVM,
                                         desc.Desc32[1],
                                         desc.Desc32[0],
                                         1,
                                         0);
            }
            pcs->CRS.Client_ECX = clientInfoSelector;
            
          }
          
          break;
        
        //
        // Configuration APIs.
        //
        case NVRM_API_GET_VERSION:
            pcs->CRS.Client_EAX = NVRM_VERSION;
            pcs->CRS.Client_EDX = NVRM_VERSION >> 16;
            break;
        case NVRM_API_UPDATE_CONFIGURATION:
        {
            rmInService++;
            if (pDev->Framebuffer.UpdateFlags & UPDATE_HWINFO_BUFFER_PARAMS)
            {
                //
                // Check for invalid resolution setting - use registry.
                //
                //
                //  we want to change the way resolutions are passed to RM:
                //  the display driver or control applet should call nvsys to 
                //  set display width and height and pixel depth
                //  and set flag ResolutionValid. When this function sees 
                //  Resolution Valid, it uses the values in pDev and resets
                //  ResolutionValid. Otherwise, it calls the VDD to get the 
                //  resolution (from the registry).
                //  Until the display driver is changed, always get resolution from the VDD.

                //
                // KJK If the client doesn't know his resolution, go out to the vdd and get it.
                // If he thinks he knows his resolution, just set that.
                //
                if (pDev->Framebuffer.Resolution == -1)
                {
                    //
                    // Get screen resolution and framebuffer configuration from registry.
                    //
                    vddGetDisplayInfo(pOsHwInfo->osDeviceHandle, &osDisplayInfo, sizeof(DISPLAYINFO));
                    pDev->Dac.HalInfo.Depth = (osDisplayInfo.InfoFlags & REGISTRY_BPP_NOT_VALID) ? 8 : osDisplayInfo.Bpp;
                    //
                    // BUG WORKAROUND: If the user has set a specific refresh rate via the Win95 OSR2+ control
                    // panel, the resulting refresh rate will always return MONITOR_INFO_NOT_VALID.  But the
                    // refresh value is correct!  Huh???
                    //
                    // So until we can find the VDD (?) bug, if the refresh value is valid (>60,<250) for our
                    // known refresh settings, let it through.
                    //
                    // Microsoft has admitted to a bug in this area of the VDD which will be fixed
                    // in Memphis (of course).
                    //
                    if ((osDisplayInfo.InfoFlags & MONITOR_INFO_NOT_VALID) &&
                        (osDisplayInfo.InfoFlags & REFRESH_RATE_MAX_ONLY) &&
                        (osDisplayInfo.RefreshRateMax >= 60) &&
                        (osDisplayInfo.RefreshRateMax <= 250))
                            osDisplayInfo.InfoFlags &= ~MONITOR_INFO_NOT_VALID;
                    //
                    // Only go out to the registry for the refresh rate if the DDK has not preset
                    // a different rate manually sometime before this ConfigUpdate().
                    //
                    if (pDev->Framebuffer.UpdateFlags & UPDATE_HWINFO_REFRESH_PARAMS)                
                        pDev->Framebuffer.UpdateFlags &= ~UPDATE_HWINFO_REFRESH_PARAMS;
                    else    
                        if ((osDisplayInfo.InfoFlags & (MONITOR_INFO_NOT_VALID | MONITOR_INFO_DISABLED_BY_USER))
                         || (osDisplayInfo.RefreshRateMax == 0xFFFFFFFF)
                         || (osDisplayInfo.RefreshRateMax == 0))
                        {
                            pDev->Framebuffer.HalInfo.RefreshRate   = 60;
                            pDev->Framebuffer.HSyncPolarity = BUFFER_HSYNC_NEGATIVE;
                            pDev->Framebuffer.VSyncPolarity = BUFFER_HSYNC_NEGATIVE;
                        }
                        else
                        {
                            pDev->Framebuffer.HalInfo.RefreshRate = osDisplayInfo.RefreshRateMax;
                            pDev->Framebuffer.HSyncPolarity = osDisplayInfo.HorzSyncPolarity;
                            pDev->Framebuffer.VSyncPolarity = osDisplayInfo.VertSyncPolarity;
                        }
                        
                    pDev->Framebuffer.HalInfo.HorizDisplayWidth = osDisplayInfo.XRes;
                    pDev->Framebuffer.HalInfo.VertDisplayWidth  = osDisplayInfo.YRes;
                }
                else
                {
                    //
                    // For now, assume that all pDev data is set correctly by the client
                    //
                    //      pDev->Framebuffer.Resolution != -1
                    //      pDev->Framebuffer.HorizDisplayWidth
                    //      pDev->Framebuffer.VertDisplayWidth
                    //      pDev->Framebuffer.Depth
                    //      pDev->Framebuffer.RefreshRate
                    //
                }
                    
                //
                // BUG WORKAROUND: There is another problem with the refresh information
                // returned by the VDD.  If a low-res DDraw mode is selected (ones below 640x400)
                // and there is no refresh information in the registry (since its not a std
                // desktop resolution), the VDD seems to return the rate information that
                // it has for 640x480.  But that is wrong.  Due to line doubling, the max
                // rates for the lower modes is actually determined by the max rate of the
                // doubled mode.
                //
                // For example, the max refresh rate of 512x384 is the same as the max rate
                // for 1024x768.  But that's not the info we're given.  Figures.
                //
                // If we believe the VDD, we may end up setting a refresh rate that is too
                // high for the monitor and will end up losing sync.                    
                //
                // Monitors that appear to show the problem most often actually have invalid
                // EDID information (such as the NANAO T-20), so the blame may actually
                // fall upon the monitor manufacturer.  But ours is not to blame, ours is
                // to fix.
                //
                // Anyway, we need to limit the refresh rate of all modes below 640x400 to
                // a safe/known value.  This restriction can be removed when either
                //
                // 1) the EDID/VDD bug is fixed
                // 2) a board vendor provides explicit support for setting ddraw refresh rates
                //    and we're sure we're given a valid rate.
                //
                // I hate restricting functionality, but until then let's limit all low-res 
                // line-doubled modes to 75Hz.
                //
                if ((pDev->Framebuffer.HalInfo.VertDisplayWidth < 400) 
                    && (pDev->Framebuffer.HalInfo.RefreshRate > 75))
                        pDev->Framebuffer.HalInfo.RefreshRate = 75;    // hardcode to 75Hz
                
                pDev->Framebuffer.HorizFrontPorch = 0;
                pDev->Framebuffer.HorizSyncWidth  = 0;
                pDev->Framebuffer.HorizBackPorch  = 0;
                pDev->Framebuffer.VertFrontPorch  = 0;
                pDev->Framebuffer.VertSyncWidth   = 0;
                pDev->Framebuffer.VertBackPorch   = 0;
                pDev->Framebuffer.CSync           = BUFFER_CSYNC_DISABLED;
                pDev->Dac.HalInfo.VClk            = 0;
                
                //
                // Only enable double buffering if enough memory available.
                //
                pDev->Framebuffer.Count = 1;

                //
                // Before collecting all of the pertinent display timing data and
                // setting the mode, verify that we don't need to limit our
                // timings due to a specific display device
                //
                // This display device check will be called again later, but we
                // need a glimpse of the settings before we generate the
                // first set of timings
                //
                osPreModeSet(pDev, Head);
                //
                // If we are running on a TV, be sure to limit our refresh rate
                // to the adapter default (60Hz).  The clocks are generated by
                // the external TV encoder device, but we must be sure our CRTC
                // timings are not programmed too high for the TV encoder.
                //
                if (GETDISPLAYTYPE(pDev, Head) == DISPLAY_TYPE_TV)
                    pDev->Framebuffer.HalInfo.RefreshRate = 60;
                // Flat panels CRTC timing should always work at 60 Hz. If the FP
                // supports higher refresh rates, the DAC timings will be set higher,
                // and the CRTC running at 60 Hz should still be OK (the blank period is
                // set by the DAC, not the CRTC).
                if (GETDISPLAYTYPE(pDev, Head) == DISPLAY_TYPE_FLAT_PANEL)
                    pDev->Framebuffer.HalInfo.RefreshRate = 60;
                                    
                
                // Setup the timing information for this mode in pDev.
                osSetupForNewStyleModeSet(pDev);

                // set the new mode.
                stateSetMode(pDev);
            }
            //
            // Update any new configuration.
            //
            i = stateNv(pDev, STATE_UPDATE);
            pcs->CRS.Client_EAX = i;
            rmInService--;
            break;
        }
        case NVRM_API_CONFIG_VGA:
        {
            // The Display Driver gets call to do a Disable and then calls us
            // to switch to VGA.  If we are in MC_POWER_LEVEL_2 this is not a 
            // good thing to do
            if (MC_POWER_LEVEL_2 == pDev->Power.State)
            {
               // there is a problem with this code
               // what happens is that there are a couple of mode switches calls
               // as we power down. Mode 3 & Mode 93 --> 13 without memory
               // change.  After these mode switch the screen is corrupted instead of
               // the correct windows bitmap.  The timing of the corruption varies, with it
               // sometimes happening after mode 3 and sometimes mode 13.
               // ok corruption is better then a hang anyday....
#if 1
               U032 bar0_address, pci_address, pci_reg, i;

               bar0_address = pDev->Chip.HalInfo.PciBars[0];
               pci_address = pDev->Mapping.nvBusDeviceFunc;

               // make sure ACPI is off
               _asm mov  dx, 0xcf8
               _asm in   eax, dx
               _asm push eax
               _asm mov  eax, pci_address  
               _asm or   eax, 0x80000064
               _asm out  dx, eax
               _asm mov  dx, 0xcfc
               _asm xor  eax, eax
               _asm out  dx, eax

               // set up pci space
               for (i = 0, pci_reg = 0x80000010; i < pDev->Chip.HalInfo.TotalPciBars; i++, pci_reg += 4)
               {
                  U032 barN_address = pDev->Chip.HalInfo.PciBars[i];

                  _asm mov  dx, 0xcf8
                  _asm mov  eax, pci_address  
                  _asm or   eax, pci_reg
                  _asm out  dx, eax
                  _asm mov  dx, 0xcfc
                  _asm mov  eax, barN_address
                  _asm out  dx, eax 
               }

               // turn us on after we are mapped in
               _asm mov  eax, pci_address  
               _asm mov  dx, 0xcf8
               _asm or   eax, 0x80000004 
               _asm out  dx, eax
               _asm mov  dx, 0xcfc
               _asm in   al,dx

               // Enable the memory decode and BusmasterEnable of the PCIO device.
               _asm or   al,6
               _asm out  dx,al
               _asm pop  eax
               _asm mov  dx, 0xcf8
               _asm out  dx, eax
#else
               break;
#endif
           }

            //
            // It appears we can get a CONFIG_VGA before you've gotten the POWER_EVENT
            // to resume and trying to rmEnableHiRes with the lower power settings hangs
            // the HW, so make sure we've restored the settings first.
            //
            if (pDev->Power.State != MC_POWER_LEVEL_0)
            {
                mcPowerState(pDev, MC_POWER_LEVEL_0, Head);
                pDev->Vga.Enabled = TRUE;
                resume = 1;
            }
            
            if (pcs->CRS.Client_EBX)
                rmEnableVga(pDev, pOsHwInfo->osDeviceHandle == 1);
            else
            {
                if (pDev->fb_save)
                {
                    // copy from save area to framebuffer instance memory
                    // the first 64k is BIOS image (looking backwards from the top)
                    // the BIOS will be POSTed, so it gets copied into instance mem without our help
                    fb_sv = pDev->fb_save;

                    for(i=0; i < INST_COPY/4; i++)
                    {
                         data = *fb_sv++;
                         REG_WR32(NV_PRAMIN_DATA032(i), data);
                    }            
                    osFreeMem((VOID *)pDev->fb_save);
                    pDev->fb_save = 0;
                }
                rmEnableHiRes(pDev, pOsHwInfo->osDeviceHandle == 1);
            }
            break;
        }
        case NVRM_API_MODE_HOOK:
            pDev->DBdisplayModeHook = (U032 (*)(U032))pcs->CRS.Client_EBX;
            break;
        //
        // Return Linear address of hardware.
        //
        case NVRM_API_GET_NV_ADDRESS:
            pcs->CRS.Client_EAX = (U032)nvAddr;
            break;
        //
        // Return Linear address of framebuffer.
        //
        case NVRM_API_GET_FB_ADDRESS:
            pcs->CRS.Client_EAX = (U032)fbAddr;
            break;
        //
        // Return Linear address of AGP aperture.
        //
        case NVRM_API_GET_AGP_ADDRESS:
            pcs->CRS.Client_EAX = (U032)pDev->DBagpAddr;
            break;
        //
        // Return Linear address of Gamma table
        //
        case NVRM_API_GET_GAMMA_ADDRESS:
            pcs->CRS.Client_EAX = (U032)pDev->Dac.Gamma;
            break;
        //
        // Return framebuffer offset of priviliged vdd memory
        //
        case NVRM_API_GET_VDD_PRIV_ADDRESS:
            pcs->CRS.Client_EAX = (U032)pDev->Pram.PrivBase;
            break;
            
        //
        // Mediaport callback mechanism still used by Canopus
        //            
        case NVRM_API_VPE_MISC:
            // decode subfunction
            pcs->CRS.Client_EAX = -1; // so far success

            switch (pcs->CRS.Client_EBX)
            {
            // 16 bit accesses only !!
            case IMAGE_INSTALL_16CALLBACK:
                    mpImageCallback.Selector = (U032) (pcs->CRS.Client_EDX & 0xFFFF);
                    mpImageCallback.Offset   = pcs->CRS.Client_ECX  & 0xFFFF;
                    mpImageInstalledCallback = TRUE; // for time being, debug reasons only
                    break;

            case IMAGE_UNINSTALL_16CALLBACK:
                    mpImageCallback.Selector = 0;
                    mpImageCallback.Offset   = 0;
                    mpImageInstalledCallback = FALSE;
                    break;

            default:
                // undefined subfunction, error alert
                pcs->CRS.Client_EAX = 0;
                break;
            }
            break;
            
        //
        // Don't call system DLL anymore.
        //
        case NVRM_API_UNLOAD_DLL:
            pOsHwInfo->dllIpcOffset      = 0;
            pOsHwInfo->dllIpcSelector    = 0;
            pOsHwInfo->dllNotifyOffset   = 0;
            pOsHwInfo->dllNotifySelector = 0;
            osCallbackList    = NULL;
            break;

        case NVRM_API_SEMAPHORE:
            {
            // Maybe this code should be executed when there are > 1 NV chips in the system,
            // as it could cause a performance hit.
              if (pcs->CRS.Client_ECX == SEMAPHORE_WAIT)
                mmWaitSemaphore();
              if (pcs->CRS.Client_ECX == SEMAPHORE_SIGNAL)
                mmSignalSemaphore();
            }
            break;
            
        case NVRM_API_DRIVER_CONNECT:
            {
              // Check all devices to find out which MiniVDD is calling.
              int temp_counter;
              char * tmpPtr;
              
              for(temp_counter=0;temp_counter<MAX_INSTANCE;temp_counter++){
                pDev = NvDBPtr_Table[temp_counter];

                if(pDev==0)
                {
                  DBG_BREAKPOINT();
                  pcs->CRS.Client_EAX = 0;
                  break;
                }

                pOsHwInfo = (PWIN9XHWINFO) pDev->pOsHwInfo;
                if(pOsHwInfo->osDeviceHandle == pcs->CRS.Client_EBX){
                  pcs->CRS.Client_EAX = temp_counter + 1;            // Return a 1-based device number.
                  temp_counter = MAX_INSTANCE;                       // terminate for loop.
                } // osDeviceHandle matches
                else{
                  if(pOsHwInfo->osDeviceHandle == 0){
                    pOsHwInfo->osDeviceHandle = pcs->CRS.Client_EBX; // Set the os value to nvinfo
                    
                    if (pcs->CRS.Client_ECX != 0) {
                        // Make space for the device name string and copy it.
                        tmpPtr = _HeapAllocate(osStringLength((char *)pcs->CRS.Client_ECX), 0);
                        if (tmpPtr != 0) {
                           osStringCopy(tmpPtr, (char *)pcs->CRS.Client_ECX);
                           pOsHwInfo->osDeviceName = tmpPtr;
                        }
                    }

                    if (!pDev->DBfbInstBitmap)
                    {            
                        status = fbInitializeInstMemBitMap(pDev);
                        if (status)
                        {
                            DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: Cannot allocate InstMemBitMap!");
                            DBG_BREAKPOINT();
                        }
                    }

                    // Now that we have the osDeviceHandle for this device, 
                    // update the mode we are supposed to be in by reading the registry
                    osDisplayInfo.HdrSize = sizeof(DISPLAYINFO);
                    vddGetDisplayInfo(pOsHwInfo->osDeviceHandle, &osDisplayInfo, sizeof(DISPLAYINFO));
                    initFbEx(pDev);
                    
                    pcs->CRS.Client_EAX = temp_counter + 1;              // Return a 1-based device number.
                    temp_counter = MAX_INSTANCE;                         // terminate for loop.
                  } // osDeviceHandle == 0
                  else{
                    if(temp_counter == MAX_INSTANCE-1){
                      DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: Too many devices detected !");
                      DBG_BREAKPOINT();
                    }
                  } // osDeviceHandle is not zero
                } // osDeviceHandle is does not match
              } // loop for all devices
            }  
            break;
            
        case NVRM_API_DRIVER_DISCONNECT:
            {
                // Disconnect the device specified. Note that this is different than API_EXIT. 
                // All we do here is to undo what DRIVER_CONNECT did and throw away the instance memory
                // allocation. There should be no activity on this device. The driver calls this after
                // calling CONFIG_VGA to put the device in VGA mode. Also, this is never called for the
                // primary device.

                // don't do anything if the device is not disabled and in VGA mode.
                if (pDev->Vga.Enabled &&
                    pDev->DeviceDisabled)
                {
                    if(pOsHwInfo->osDeviceHandle != 0)
                    {
                        pOsHwInfo->osDeviceHandle = 0;
                        if (pOsHwInfo->osDeviceName != 0)
                        {
                            _HeapFree(pOsHwInfo->osDeviceName, 0);
                            pOsHwInfo->osDeviceName = NULL;
                        }
                
                        if (pDev->DBfbInstBitmap != 0)
                        {
                            osFreeMem(pDev->DBfbInstBitmap);
                            pDev->DBfbInstBitmap = 0;
                        }
                    } // osDeviceHandle is not 0
                } // device is in VGA and is disabled
            }
            break;
        case NVRM_API_DISPLAY:
            
            switch (pcs->CRS.Client_EBX)
            {
                
                case NVRM_API_DISPLAY_GET_MONITOR_TYPE:
                
                    data32 = GETDISPLAYTYPE(pDev, Head);
                    {
                        switch (data32)
                        {
                            case DISPLAY_TYPE_MONITOR:
                                SETMONITORTYPE(pDev, Head, MONITOR_TYPE_VGA);
                                break;
                            case DISPLAY_TYPE_TV:
                                switch (pDev->Dac.TVStandard)
                                {
                                    case NTSC_M:
                                    case NTSC_J:
                                        SETMONITORTYPE(pDev, Head, MONITOR_TYPE_NTSC);
                                        break;
                                    case PAL_A:
                                    case PAL_M:
                                    case PAL_N:
                                    case PAL_NC:
                                        SETMONITORTYPE(pDev, Head, MONITOR_TYPE_PAL);
                                        break;
                                }
                                break;
                            case DISPLAY_TYPE_FLAT_PANEL:        
                                SETMONITORTYPE(pDev, Head, MONITOR_TYPE_FLAT_PANEL);
                                break;
                            default:
                                // Set display type to the BIOS display type
                                switch (dacGetBiosDisplayType(pDev, Head))
                                {
                                    case DISPLAY_TYPE_MONITOR:
                                        SETMONITORTYPE(pDev, Head, MONITOR_TYPE_VGA);
                                        break;
                                    case DISPLAY_TYPE_TV:
                                        SETMONITORTYPE(pDev, Head, MONITOR_TYPE_NTSC);
                                        break;
                                    case DISPLAY_TYPE_FLAT_PANEL:
                                        SETMONITORTYPE(pDev, Head, MONITOR_TYPE_FLAT_PANEL);
                                        break;
                                }
                        }
                    }
                    pcs->CRS.Client_EAX = (U032)GETMONITORTYPE(pDev, Head);
                    // I'm tacking on this additional info for the mini-VDD. 
                    pcs->CRS.Client_EBX = (U032)pDev->Dac.DisplayChanged;
                    pDev->Dac.DisplayChanged = 0;
                    break;
                    
                case NVRM_API_DISPLAY_GET_DISPLAY_TYPE:
                    pcs->CRS.Client_EAX = GETDISPLAYTYPE(pDev, Head);
                    break;
                
                case NVRM_API_DISPLAY_SET_DISPLAY_TYPE:
                    SETDISPLAYTYPE(pDev, Head, pcs->CRS.Client_EBX);
                    break;
                
                case NVRM_API_DISPLAY_GET_FP_CONNECT_STATUS:
                    pcs->CRS.Client_EAX = dacFlatPanelConnectStatus(pDev, Head);
                    data32 = GETDISPLAYTYPE(pDev, Head);
                    if((data32==DISPLAY_TYPE_FLAT_PANEL)&&(!pcs->CRS.Client_EAX)){
                        if(dacMonitorConnectStatus(pDev, Head))
                            data32 = DISPLAY_TYPE_MONITOR;
                    }
                    SETDISPLAYTYPE(pDev, Head, data32);
                    break;
                
                case NVRM_API_DISPLAY_GET_FP_CONFIG:
                    pcs->CRS.Client_EAX = dacGetFlatPanelConfig(pDev, Head);
                    break;
                
                case NVRM_API_DISPLAY_SET_FP_CONFIG:
                    // KJK
                    // Need to revisit and add search for current dac object using this Head, then pass
                    // that object directly into this call.
                    // KJK 
                    // pcs->CRS.Client_EAX = dacSetFlatPanelMode(pDev, Head, pcs->CRS.Client_ECX, TRUE);
                    break;
                                
                //
                // Handle the incoming display change notifications from the 16bit driver
                //
                case NVRM_API_DISPLAY_CHANGE_CONFIG:
                    pcs->CRS.Client_EAX = dacSetSpanning(pDev, pcs->CRS.Client_ECX);
                    break;
                case NVRM_API_DISPLAY_CHANGE_START:
                    pcs->CRS.Client_EAX = dacSetModeStart(pDev, pcs->CRS.Client_ECX);
                    break;
                
                case NVRM_API_DISPLAY_CHANGE_END:
                    pcs->CRS.Client_EAX = dacSetModeEnd(pDev, pcs->CRS.Client_ECX);
                    break;

            }
                                    
            break;
        case NVRM_API_RM_TOKEN:
          switch(pcs->CRS.Client_EBX){
            case QUERY_TOKEN:
              pcs->CRS.Client_EAX = ResourceManagerGlobalToken;
              break;
            case RETURN_TOKEN:
              if(!ResourceManagerGlobalToken){
                DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: TOKEN ERROR ! TOKEN=0 on RETURN_TOKEN call\n\r");
                DBG_BREAKPOINT();
              }else
                DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: RELEASED THE TOKEN !\n\r");
                ResourceManagerGlobalToken = 0; 
              break;
            case AQUIRE_TOKEN:
              if(!ResourceManagerGlobalToken){
                ResourceManagerGlobalToken = 1; 
                pcs->CRS.Client_EAX = 1;
                DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: AQUIRED THE TOKEN !\n\r");
              }else{
                pcs->CRS.Client_EAX = 0;
                DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: WAITING FOR RM TOKEN ...\n\r");
              }
              break;
            default:
              DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: TOKEN ERROR ! Subfunction UNKOWN !\n\r");
              DBG_BREAKPOINT();
              break;
          }
          break;

        case NVRM_API_GET_DEVICE_TYPE:
            pcs->CRS.Client_EAX = REG_RD32(NV_PBUS_PCI_NV_0);
            break;
        case NVRM_API_REGISTER_DISPLAY_DRIVER:
            RegisterDisplayDriver(pcs);
            break;         

        case NVRM_REGISTRY_SET_STRING:
            {
              int  char_count,string_size;
              char stringt[128]=NV4_REG_RESOURCE_MANAGER_SUBKEY;

              osStringCopy(pDev->Registry.DBstrDevNodeDisplayNumber, (char *)pcs->CRS.Client_EBX);

              // Append the Resource Manager subkey string.("System")
              string_size = osStringLength(pDev->Registry.DBstrDevNodeDisplayNumber);
              pDev->Registry.DBstrDevNodeDisplayNumber[string_size] = '\\';
              string_size++;
              for(char_count=string_size;char_count < (128-string_size);char_count++){
                pDev->Registry.DBstrDevNodeDisplayNumber[char_count]=stringt[char_count-string_size];
              }
              osStringCopy(pDev->Registry.DBstrDevNodeDisplay,pDev->Registry.DBstrDevNodeDisplayNumber);
            }
            break;

        case NVRM_API_DISPLAY_GET_BOOT_PHYS_MAP:
            pcs->CRS.Client_EAX = pDev->Dac.LogicalDeviceMap[pcs->CRS.Client_ECX];
            break;
                
        case NVRM_API_READ_EDID:
            {
                U008 lock;
                U008 failure;
                U032 dispType;
                U032 NVClktemp;

                if (pDev->Power.State != 0)
                {
                    // restore timer before attempting delay
                    REG_WR32(NV_PTIMER_NUMERATOR, pDev->Timer.Numerator);     
                    REG_WR32(NV_PTIMER_DENOMINATOR, pDev->Timer.Denominator);     

                    // turn clocks up
                    REG_WR32(NV_PRAMDAC_PLL_COEFF_SELECT, pDev->Power.coeff_select);
                    if (pDev->Power.MobileOperation)
                    {
                        NVClktemp = pDev->Dac.HalInfo.NVClk;
                        pDev->Dac.HalInfo.NVClk = pDev->Power.PostNVCLK;
                    }

                    if (dacProgramNVClk(pDev) != RM_OK)
                    {
                        REG_WR32(NV_PRAMDAC_NVPLL_COEFF, pDev->Power.nvpll);         // nvclk
                    }

                    if (pDev->Power.MobileOperation)
                    {
                        pDev->Dac.HalInfo.NVClk = NVClktemp;
                    }

                    REG_WR32(NV_PRAMDAC_MPLL_COEFF, pDev->Power.mpll);           // mclk
                }

                // From parameter stack
                Head = pcs->CRS.Client_ECX;

                lock = UnlockCRTC(pDev, Head);

                 // Given a head number only, get the EDID for the attached device
                // This is called just after initDac().
                // We need to figure out the display type before we call EDIDRead().
                // We also need to call EDIDDetect to find out if there really is a display device attached,
                // since EDIDRead() will return RM_OK because it will get the BIOS default EDID.
                dispType = GETDISPLAYTYPE(pDev, Head);
                failure = 0;
                if (dispType == -1)
                {
                    dispType = DISPLAY_TYPE_FLAT_PANEL;
                    if (RM_OK != EDIDDetect(pDev, Head, dispType))
                    {
                        dispType = DISPLAY_TYPE_MONITOR;
                        if (RM_OK != EDIDDetect(pDev, Head, dispType))
                        {
                            failure = 1;
                        }
                    }
                }

                if (failure || RM_OK != EDIDRead(pDev, Head, dispType)) 
                {
                    pcs->CRS.Client_EAX = 0;
                } 
                else 
                {
                    // Copy the pointer to the edid buffer.
                    pcs->CRS.Client_EAX = (U032)pDev->Dac.CrtcInfo[Head].EDID;
                }

                RestoreLock(pDev, Head, lock);
            }
            break;
       
        case NVRM_API_GET_DDC_CAPS:
            // handle the VESA DDC Get Caps function (set bitmap of DDC addresses)
            pcs->CRS.Client_EBX = edidGetDDCCaps(pDev);
            pcs->CRS.Client_EAX = 0x004f;   // success
            break;
                
        case NVRM_API_SET_MONITOR_POWER:
            // In APM (dell data) it is common to get
            // monitor power on after we have entered power level 4
            // this will cause a bloom.  So we ignore power on/off calls
            // after the suspend
            if (pDev->Power.State == MC_POWER_LEVEL_4)
                 break;
            // Handle the ACPI monitor power state
            if (pcs->CRS.Client_ECX == 0)   // only on and off
            {
                DBG_PRINT_STRING(DEBUGLEVEL_WARNINGS, "NVRM: Turn On monitor power\n");

                if(pDev->Power.Flags & DAC_POWER_PENDING)
                {
                    //Reset global power_pending flag
                    pDev->Power.Flags &= ~DAC_POWER_PENDING;

                    // Bug 29711: if you hibernate on CRT, unplug CRT, boot back up
                    // on LCD, the LCD blooms. When we get to this function, we do
                    // a modeset on the CRT because the pVidLutCurDacs are setup for
                    // head A = CRT and head B = Nothing.  If we reset the chip in 
                    // mcPowerState, we turn off the flat panel clocks.  When we call 
                    // Wake Up in this function, the LCD turns on, but the clocks
                    // do not. This causes blooming.  
                    // So to fix this, if we've reset the chip, and we've boot to  
                    // an LCD, don't power on the panel!  The Reset flag is turned
                    // off at the end of dacSetFlatPanelMode in dacfp.c.
                    if(pDev->Dac.DevicesEnabled & DAC_PANEL_CLK_RESET)
                    {
                        if(pDev->Dac.BootDevicesBitMap & DAC_DD_MASK_IN_MAP)
                        {
                            DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: Set Monitor Power called after chip reset without panel clocks setup!\n");
                            break;
                        }
                    }

                    for(Head = 0; Head < MAX_CRTCS; Head++)
                    {
                        if ((GETDISPLAYTYPE(pDev, Head) != DISPLAY_TYPE_NONE))
                        {
                            // Enable display
                            dacEnableDac(pDev, Head);

                            // also make sure the cursor image is updated
                            pDev->Dac.CrtcInfo[Head].UpdateFlags &= (~UPDATE_HWINFO_DAC_CURSOR_ENABLE);
                            pDev->Dac.CrtcInfo[Head].UpdateFlags |= (UPDATE_HWINFO_DAC_CURSOR_IMAGE_NEW |
                                                                  UPDATE_HWINFO_DAC_CURSOR_DISABLE);
                        }
                    }

                    //Toshiba specefic call (DPMS, hotkeys)
                    //Backlight enabled inside dacEnableDac for other mobiles
                    if (pDev->Power.MobileOperation == 2)
                    { 
                        U032    EAX, EBX, ECX, EDX;
                        EAX = 0x4F10;
                        EBX = 0x0001; //wake up
                        EDX = 0x0000;
                        ECX = 0x0000;
                        status = osCallVideoBIOS(pDev, &EAX, &EBX, &ECX, &EDX, NULL);
                    }
                }
            }
            else
            {
                DBG_PRINT_STRING(DEBUGLEVEL_WARNINGS, "NVRM: Turn Off monitor power\n");

                if(!(pDev->Power.Flags & DAC_POWER_PENDING))
                {
                    // Turn off the backlight before disabling the DACs
                    if (pDev->Power.MobileOperation)
                    { 
                        if (pDev->Power.MobileOperation == 2)
                        {
                            //Toshiba specefic call (DPMS, hotkeys)
                            U032    EAX, EBX, ECX, EDX;
                            EAX = 0x4F10;
                            EBX = 0x0401; //sleep
                            EDX = 0x0000;
                            ECX = 0x0000;
                            status = osCallVideoBIOS(pDev, &EAX, &EBX, &ECX, &EDX, NULL);
                        }
                    }

                    for(Head = 0; Head < MAX_CRTCS; Head++)
                    {
                        // if active display on Head?
                        if (GETDISPLAYTYPE(pDev, Head) != DISPLAY_TYPE_NONE)
                        {
                            dacDisableDac(pDev, Head);         // blank display
                        }
                    }

                    //Set global power_pending flag
                    //That means we would never touch neither power nor backlight
                    //on the flat panel until we would pass this case with power ON.
                    pDev->Power.Flags |= DAC_POWER_PENDING;
                }
            }

            break;
        
        case NVRM_API_BLANK_SECONDARY:
            {
                U032    PrimaryHead, i, CurrentDisplayType, CurHRes, CurVRes;
                PVIDEO_LUT_CURSOR_DAC_OBJECT pVidLutCurDac;
                U008 lock, Cr28, Cr33, Cr49;
                
                // Determine which head is the Primary device
                for (i = 0, PrimaryHead = 0; i < pDev->Dac.HalInfo.NumCrtcs; i++)
                {
                    if (pDev->Dac.CrtcInfo[i].PrimaryDevice == TRUE)
                    {
                        PrimaryHead = i;
                        break;
                    }
                }
                if (pDev->Dac.EncoderType == NV_ENCODER_BROOKTREE_871) 
                {

                    dacTV871Readback(pDev, 0, 0);  // set legacy readback mode
                    dacDisableTV(pDev, 0);
                }

                // Blank the secondary device. Also enable the BIOS's primary head and set up the display type
                if (pDev->Dac.CrtcInfo[PrimaryHead].pVidLutCurDac != NULL)
                {
                    if (pDev->Power.MobileOperation)    // mobile BIOS is multihead
                    {
                        // MOBILE
                        U008 Cr3B[2];
                        U032 SecondaryHead = PrimaryHead^1;
                        
                        // Read Head A CRTC regs
                        lock = UnlockCRTC(pDev, 0);
                        CRTC_RD(0x3B, Cr3B[0], 0);   // find out which displays are active
                        RestoreLock(pDev, 0, lock);
                        
                        // Read Head B CRTC regs
                        lock = UnlockCRTC(pDev, 1);
                        CRTC_RD(0x3B, Cr3B[1], 1);   // find out which displays are active
                        RestoreLock(pDev, 1, lock);
                        
                        // check to see if we need to disable second head
                        if (!pDev->Dac.BroadcastSupport)  // BIOS/chip revision does not support broadcast mode
                        {
                            dacDisableDac(pDev, SecondaryHead); // blank display on the other head
                    
                            // remove the displays from the secondary Cr3B registers
                            lock = UnlockCRTC(pDev, SecondaryHead);
                            CRTC_WR(0x3B, (Cr3B[SecondaryHead]&0x0F), SecondaryHead);
                            RestoreLock(pDev, SecondaryHead, lock);
                    
                            // Save off this information here for rmEnableHires
                            if(pDev->Dac.DeviceProperties.Spanning)
                            {
                                pDev->Dac.NonBroadcastModeStoredCr3B[0] = Cr3B[0]&0xF0;
                                pDev->Dac.NonBroadcastModeStoredCr3B[1] = Cr3B[1]&0xF0;
                            }
                        }
                        
                        // CRT and TV use Head A, DFP uses head B
                        EnableHead(pDev, PrimaryHead);  // leave pointing to the BIOS's primary head
                        pVidLutCurDac = (PVIDEO_LUT_CURSOR_DAC_OBJECT) pDev->Dac.CrtcInfo[PrimaryHead].pVidLutCurDac;
                        switch (pVidLutCurDac->DisplayType) 
                        {
                            case DISPLAY_TYPE_FLAT_PANEL:
                                AssocDDC(pDev, 1);  // uses PCRTC, no need to unlock ext CRTC
                                break;
                            case DISPLAY_TYPE_TV:
                            case DISPLAY_TYPE_MONITOR:
                            default:    
                                AssocDDC(pDev, 0);
                                AssocTV(pDev, 0);
                        }
                    }
                    else
                    {
                        // DESKTOP
                        EnableHead(pDev, 0);            // desktop BIOS is single head -- must use head A
                        
                        // Set Cr28 and Cr33 for the current display type
                        pVidLutCurDac = (PVIDEO_LUT_CURSOR_DAC_OBJECT) pDev->Dac.CrtcInfo[PrimaryHead].pVidLutCurDac;
                        
                        lock = UnlockCRTC(pDev, 0);
                        CRTC_RD(NV_CIO_CRE_PIXEL_INDEX, Cr28, 0);      // read pixel format
                        CRTC_RD(NV_CIO_CRE_LCD__INDEX, Cr33, 0);
                        switch (pVidLutCurDac->DisplayType) 
                        {
                            case DISPLAY_TYPE_MONITOR:
                                CRTC_WR(NV_CIO_CRE_PIXEL_INDEX, Cr28 & ~0x80, 0);
                                CRTC_WR(NV_CIO_CRE_LCD__INDEX, Cr33 & ~0x13, 0);
                                break;                                
                            case DISPLAY_TYPE_TV:
                                if (pDev->Dac.EncoderType == NV_ENCODER_BROOKTREE_871) 
                                {
                                    // Set 640x480 encoder resolution. The BIOS uses discrete register programming and will be unable 
                                    // to deal with higher resolutions set by the RM using Autoconfig (there must be registers 
                                    // programmed in Autoconfig which the BIOS does not touch).
                                    CurHRes = pDev->Framebuffer.HalInfo.HorizDisplayWidth;
                                    CurVRes = pDev->Framebuffer.HalInfo.VertDisplayWidth;
                                    pDev->Framebuffer.HalInfo.HorizDisplayWidth = 640;      // set object's res
                                    pDev->Framebuffer.HalInfo.VertDisplayWidth = 480;
                                    dacProgramBrooktreeAuto(pDev, 0);                   // do the Autoconfig modeset
                                    pDev->Framebuffer.HalInfo.HorizDisplayWidth = CurHRes;  // restore desktop res
                                    pDev->Framebuffer.HalInfo.VertDisplayWidth = CurVRes;
                                }
                                
                                // provide a clock before slaving the CRTC
                                CRTC_RD(NV_CIO_CRE_ENGINE_CTRL, Cr49, 0);      
                                CRTC_WR(NV_CIO_CRE_ENGINE_CTRL, Cr49 | 0x14, 0); // Head A owns TV + I2C      
                                        
                                CRTC_WR(NV_CIO_CRE_PIXEL_INDEX, Cr28 | 0x80, 0);
                                CRTC_WR(NV_CIO_CRE_LCD__INDEX, Cr33 & ~0x13, 0);
                                break;                                
                            case DISPLAY_TYPE_FLAT_PANEL:
                                // Head A should own I2C
                                CRTC_RD(NV_CIO_CRE_ENGINE_CTRL, Cr49, 0);      
                                CRTC_WR(NV_CIO_CRE_ENGINE_CTRL, Cr49 | 0x04, 0); // Head A owns I2C      
                                // Put valid timings into PRAMDAC_FP so we have a good clock
                                CurrentDisplayType = GETDISPLAYTYPE(pDev, 0);
                                SETDISPLAYTYPE(pDev, 0, DISPLAY_TYPE_FLAT_PANEL); // make sure display type is correct
                                dacSetFlatPanelMode(pDev, 0, pVidLutCurDac, NV_CFGEX_GET_FLATPANEL_INFO_SCALED, FALSE);
                                SETDISPLAYTYPE(pDev, 0, CurrentDisplayType);      // restore type for RM
                                // slave the CRTC to FP
                                CRTC_WR(NV_CIO_CRE_PIXEL_INDEX, Cr28 | 0x80, 0);
                                CRTC_WR(NV_CIO_CRE_LCD__INDEX, Cr33 | 0x13, 0);
                               
                                break;                                
                        }
                        RestoreLock(pDev, 0, lock);
                    }
                }
            }
            break;

        default:
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_USERERRORS, "NVRM: Unknown user API = ", (DWORD)pcs->CRS.Client_EAX);
            DBG_BREAKPOINT();
            break;
    }

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: User Service to Resource Manager complete\n\r");
    return (RM_OK);
}
