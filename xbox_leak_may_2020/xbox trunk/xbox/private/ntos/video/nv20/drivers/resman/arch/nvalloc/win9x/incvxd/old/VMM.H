#ifndef _VMM_H_
#define _VMM_H_

//---------------------------------------------------------------------------
//
//  vmm.h
//
//              6/21/94   dschmenk    Updated to use Watcom inline functions
//              4/11/95   dschmenk    Updated to use Win 95 extensions
//
//---------------------------------------------------------------------------

#include <vmmtypes.h>

//----------------------------------------------------------------------------
//
//                        Device ID equates
//
//----------------------------------------------------------------------------

#define Undefined_Device_ID    0x00000
#define VMM_Device_ID          0x00001   // Used for dynalink table
#define Debug_Device_ID        0x00002
#define VPICD_Device_ID        0x00003
#define VDMAD_Device_ID        0x00004
#define VTD_Device_ID          0x00005
#define V86MMGR_Device_ID      0x00006
#define PageSwap_Device_ID     0x00007
#define Parity_Device_ID       0x00008
#define Reboot_Device_ID       0x00009
#define VDD_Device_ID          0x0000A
#define VSD_Device_ID          0x0000B
#define VMD_Device_ID          0x0000C
#define VKD_Device_ID          0x0000D
#define VCD_Device_ID          0x0000E
#define VPD_Device_ID          0x0000F
#define BlockDev_Device_ID     0x00010
#define VMCPD_Device_ID        0x00011
#define EBIOS_Device_ID        0x00012
#define BIOSXlat_Device_ID     0x00013
#define VNETBIOS_Device_ID     0x00014
#define DOSMGR_Device_ID       0x00015
#define WINLOAD_Device_ID      0x00016
#define SHELL_Device_ID        0x00017
#define VMPoll_Device_ID       0x00018
#define VPROD_Device_ID        0x00019
#define DOSNET_Device_ID       0x0001A
#define VFD_Device_ID          0x0001B
#define VDD2_Device_ID         0x0001C   // Secondary display adapter
#define WINDEBUG_Device_ID     0x0001D
#define TSRLoad_Device_ID      0x0001E   // TSR instance utility ID
#define BiosHook_Device_ID     0x0001F   // Bios interrupt hooker VxD
#define Int13_Device_ID        0x00020
#define PageFile_Device_ID     0x00021   // Paging File device
#define SCSI_Device_ID         0x00022   // SCSI device
#define MCA_POS_Device_ID      0x00023   // MCA_POS device
#define SCSIFD_Device_ID       0x00024   // SCSI FastDisk device
#define VPEND_Device_ID        0x00025   // Pen device
#define APM_Device_ID          0x00026   // Power Management device
#define VPOWERD_DEVICE_ID      APM_DEVICE_ID   // We overload APM since we replace it
#define VXDLDR_DEVICE_ID       0x00027   // VxD Loader device
#define NDIS_DEVICE_ID         0x00028   // NDIS wrapper
#define BIOS_EXT_DEVICE_ID     0x00029   // Fix Broken BIOS device
#define VWIN32_DEVICE_ID       0x0002A   // for new WIN32-VxD
#define VCOMM_DEVICE_ID        0x0002B   // New COMM device driver
#define SPOOLER_DEVICE_ID      0x0002C   // Local Spooler
#define WIN32S_DEVICE_ID       0x0002D   // Win32S on Win 3.1 driver
#define DEBUGCMD_DEVICE_ID     0x0002E   // Debug command extensions
#define CONFIGMG_DEVICE_ID     0x00033   // Configuration manager (Plug&Play)
#define DWCFGMG_DEVICE_ID      0x00034   // Configuration manager for win31 and DOS
#define SCSIPORT_DEVICE_ID     0x00035   // Dragon miniport loader/driver
#define VFBACKUP_DEVICE_ID     0x00036   // allows backup apps to work with NEC
#define ENABLE_DEVICE_ID       0x00037   // for access VxD
#define VCOND_DEVICE_ID        0x00038   // Virtual Console Device - check vcond.inc
#define ISAPNP_DEVICE_ID       0x0003C   // ISA P&P Enumerator
#define BIOS_DEVICE_ID         0x0003D   // BIOS P&P Enumerator
#define IFSMgr_Device_ID       0x00040   // Installable File System Manager
#define VCDFSD_DEVICE_ID       0x00041   // Static CDFS ID
#define MRCI2_DEVICE_ID        0x00042   // DrvSpace compression engine
#define PCI_DEVICE_ID          0x00043   // PCI P&P Enumerator
#define PELOADER_DEVICE_ID     0x00044   // PE Image Loader
#define EISA_DEVICE_ID         0x00045   // EISA P&P Enumerator
#define DRAGCLI_DEVICE_ID      0x00046   // Dragon network client
#define DRAGSRV_DEVICE_ID      0x00047   // Dragon network server
#define PERF_DEVICE_ID         0x00048   // Config/stat info
#define ETEN_Device_ID         0x00060   // ETEN DOS (Taiwan) driver
#define CHBIOS_Device_ID       0x00061   // CHBIOS DOS (Korean) driver
#define VMSGD_Device_ID        0x00062   // DBCS Message Mode driver
#define VPPID_Device_ID        0x00063   // PC-98 System Control PPI
#define VIME_Device_ID         0x00064   // Virtual DOS IME
#define VHBIOSD_Device_ID      0x00065   // HBIOS (Korean) for HWin31 driver
#define BASEID_FOR_NAMEBASEDVXD        0xF000 // Name based VxD IDs start here
#define BASEID_FOR_NAMEBASEDVXD_MASK   0x0FFF // Mask to get the real vxd id

//----------------------------------------------------------------------------
//
//                     Initialization order equates
//
//----------------------------------------------------------------------------

#define VMM_Init_Order         0x000000000
#define PERF_INIT_ORDER        0x000900000
#define APM_Init_Order         0x001000000
#define VPOWERD_INIT_ORDER     APM_INIT_ORDER  // We overload APM since we replace it
#define Debug_Init_Order       0x004000000
#define BiosHook_Init_Order    0x006000000
#define VPROD_Init_Order       0x008000000
#define VPICD_Init_Order       0x00C000000
#define VTD_Init_Order         0x014000000
#define VXDLDR_INIT_ORDER      0x016000000
#define ENUMERATOR_INIT_ORDER  0x016800000
#define ISAPNP_INIT_ORDER      ENUMERATOR_INIT_ORDER
#define EISA_INIT_ORDER        ENUMERATOR_INIT_ORDER
#define PCI_INIT_ORDER         ENUMERATOR_INIT_ORDER
#define BIOS_INIT_ORDER        ENUMERATOR_INIT_ORDER+1
#define CONFIGMG_INIT_ORDER    ENUMERATOR_INIT_ORDER+0xFFFF
#define VCDFSD_INIT_ORDER      0x016F00000
#define IOS_INIT_ORDER         0x017000000
#define PageFile_Init_Order    0x018000000
#define PageSwap_Init_Order    0x01C000000
#define Parity_Init_Order      0x020000000
#define Reboot_Init_Order      0x024000000
#define EBIOS_Init_Order       0x026000000
#define VDD_Init_Order         0x028000000
#define VSD_Init_Order         0x02C000000
#define VCD_Init_Order         0x030000000
#define COMMDRVR_INIT_ORDER    (VCD_INIT_ORDER - 1)
#define PRTCL_INIT_ORDER       (COMMDRVR_INIT_ORDER - 2)
#define MODEM_INIT_ORDER       (COMMDRVR_INIT_ORDER - 3)
#define PORT_INIT_ORDER        (COMMDRVR_INIT_ORDER - 4)
#define VMD_Init_Order         0x034000000
#define VKD_Init_Order         0x038000000
#define VPD_Init_Order         0x03C000000
#define BlockDev_Init_Order    0x040000000
#define MCA_POS_Init_Order     0x041000000
#define SCSIFD_Init_Order      0x041400000
#define SCSIMaster_Init_Order  0x041800000
#define Int13_Init_Order       0x042000000
#define VFD_Init_Order         0x044000000
#define VMCPD_Init_Order       0x048000000
#define BIOSXlat_Init_Order    0x050000000
#define VNETBIOS_Init_Order    0x054000000
#define DOSMGR_Init_Order      0x058000000
#define DOSNET_Init_Order      0x05C000000
#define WINLOAD_Init_Order     0x060000000
#define VMPoll_Init_Order      0x064000000
#define Undefined_Init_Order   0x080000000
#define WINDEBUG_Init_Order    0x081000000
#define VDMAD_Init_Order       0x090000000
#define V86MMGR_Init_Order     0x0A0000000
#define IFSMgr_Init_Order      0x10000 + V86MMGR_Init_Order
#define FSD_Init_Order         0x00100 + IFSMgr_Init_Order
#define VFD_INIT_ORDER         0x50000 + IFSMgr_Init_Order
//
// Device ID for device that must touch memory in 1st MB
// at crit init (after V86mmgr)
//
#define Undef_Touch_Mem_Init_Order 0x0A8000000
#define SHELL_Init_Order           0x0B0000000
//
// VxD load result.
//
#define VXD_FAILURE 0
#define VXD_SUCCESS 1

//----------------------------------------------------------------------------
//
//                      System control call messages
//
//----------------------------------------------------------------------------

#define Sys_Critical_Init     0x0000
#define Device_Init           0x0001
#define Init_Complete         0x0002
#define Sys_VM_Init           0x0003
#define Sys_VM_Terminate      0x0004
#define System_Exit           0x0005
#define Sys_Critical_Exit     0x0006
#define Create_VM             0x0007
#define VM_Critical_Init      0x0008
#define VM_Init               0x0009
#define VM_Terminate          0x000A
#define VM_Not_Executable     0x000B
#define Destroy_VM            0x000C
#define VM_Suspend            0x000D
#define VM_Resume             0x000E
#define Set_Device_Focus      0x000F
#define Begin_Message_Mode    0x0010
#define End_Message_Mode      0x0011

#define Reboot_Processor      0x0012
#define Query_Destroy         0x0013
#define Debug_Query           0x0014

#define Begin_PM_App          0x0015

#define BPA_32_Bit            0x0001
#define BPA_32_Bit_Flag       0x0001

#define End_PM_App            0x0016

#define Device_Reboot_Notify  0x0017
#define Crit_Reboot_Notify    0x0018

#define Close_VM_Notify       0x0019

#define PnP_New_DevNode       0x0022

#define CVNF_Crit_Close       0x0001
#define CNVF_Crit_Close_Bit   0x0000

#define Power_Event           0x001A

//----------------------------------------------------------------------------
//
//                          Service Table definition.
//
//----------------------------------------------------------------------------

#define BeginServiceTable(did) enum { did##_begin = (did##_Device_ID << 16) - 1,
#define EndServiceTable()      did##_end };

//----------------------------------------------------------------------------
//
//                          VMM Service Definitions
//
//----------------------------------------------------------------------------

#if 0
BeginServiceTable(VMM)
    Get_VMM_Version,
    Get_Cur_VM_Handle,
    Test_Cur_VM_Handle,
    Get_Sys_VM_Handle,
    Test_Sys_VM_Handle,
    Validate_VM_Handle,
    Get_VMM_Reenter_Count,
    Begin_Reentrant_Execution,
    End_Reentrant_Execution,
    Install_V86_Break_Point,
    Remove_V86_Break_Point,
    Allocate_V86_Call_Back,
    Allocate_PM_Call_Back,
    Call_When_VM_Returns,
    Schedule_Global_Event,
    Schedule_VM_Event,
    Call_Global_Event,
    Call_VM_Event,
    Cancel_Global_Event,
    Cancel_VM_Event,
    Call_Priority_VM_Event,
    Cancel_Priority_VM_Event,
    Get_NMI_Handler_Addr,
    Set_NMI_Handler_Addr,
    Hook_NMI_Event,
    Call_When_VM_Ints_Enabled,
    Enable_VM_Ints,
    Disable_VM_Ints,
    Map_Flat,
    Map_Lin_To_VM_Addr,
    Adjust_Exec_Priority,
    Begin_Critical_Section,
    End_Critical_Section,
    End_Crit_And_Suspend,
    Claim_Critical_Section,
    Release_Critical_Section,
    Call_When_Not_Critical,
    Create_Semaphore,
    Destroy_Semaphore,
    Wait_Semaphore,
    Signal_Semaphore,
    Get_Crit_Section_Status,
    Call_When_Task_Switched,
    Suspend_VM,
    Resume_VM,
    No_Fail_Resume_VM,
    Nuke_VM,
    Crash_Cur_VM,
    Get_Execution_Focus,
    Set_Execution_Focus,
    Get_Time_Slice_Priority,
    Set_Time_Slice_Priority,
    Get_Time_Slice_Granularity,
    Set_Time_Slice_Granularity,
    Get_Time_Slice_Info,
    Adjust_Execution_Time,
    Release_Time_Slice,
    Wake_Up_VM,
    Call_When_Idle,
    Get_Next_VM_Handle,
    Set_Global_Time_Out,
    Set_VM_Time_Out,
    Cancel_Time_Out,
    Get_System_Time,
    Get_VM_Exec_Time,
    Hook_V86_Int_Chain,
    Get_V86_Int_Vector,
    Set_V86_Int_Vector,
    Get_PM_Int_Vector,
    Set_PM_Int_Vector,
    Simulate_Int,
    Simulate_Iret,
    Simulate_Far_Call,
    Simulate_Far_Jmp,
    Simulate_Far_Ret,
    Simulate_Far_Ret_N,
    Build_Int_Stack_Frame,
    Simulate_Push,
    Simulate_Pop,
    _HeapAllocate,
    _HeapReAllocate,
    _HeapFree,
    _HeapGetSize,
    _PageAllocate,
    _PageReAllocate,
    _PageFree,
    _PageLock,
    _PageUnLock,
    _PageGetSizeAddr,
    _PageGetAllocInfo,
    _GetFreePageCount,
    _GetSysPageCount,
    _GetVMPgCount,
    _MapIntoV86,
    _PhysIntoV86,
    _TestGlobalV86Mem,
    _ModifyPageBits,
    _CopyPageTable,
    _LinMapIntoV86,
    _LinPageLock,
    _LinPageUnLock,
    _SetResetV86Pageable,
    _GetV86PageableArray,
    _PageCheckLinRange,
    _PageOutDirtyPages,
    _PageDiscardPages,
    _GetNulPageHandle,
    _GetFirstV86Page,
    _MapPhysToLinear,
    _GetAppFlatDSAlias,
    _SelectorMapFlat,
    _GetDemandPageInfo,
    _GetSetPageOutCount,
    Hook_V86_Page,
    _Assign_Device_V86_Pages,
    _DeAssign_Device_V86_Pages,
    _Get_Device_V86_Pages_Array,
    MMGR_SetNULPageAddr,
    _Allocate_GDT_Selector,
    _Free_GDT_Selector,
    _Allocate_LDT_Selector,
    _Free_LDT_Selector,
    _BuildDescriptorDWORDs,
    _GetDescriptor,
    _SetDescriptor,
    _MMGR_Toggle_HMA,
    Get_Fault_Hook_Addrs,
    Hook_V86_Fault,
    Hook_PM_Fault,
    Hook_VMM_Fault,
    Begin_Nest_V86_Exec,
    Begin_Nest_Exec,
    Exec_Int,
    Resume_Exec,
    End_Nest_Exec,
    Allocate_PM_App_CB_Area,
    Get_Cur_PM_App_CB,
    Set_V86_Exec_Mode,
    Set_PM_Exec_Mode,
    Begin_Use_Locked_PM_Stack,
    End_Use_Locked_PM_Stack,
    Save_Client_State,
    Restore_Client_State,
    Exec_VxD_Int,
    Hook_Device_Service,
    Hook_Device_V86_API,
    Hook_Device_PM_API,
    System_Control,
    Simulate_IO,
    Install_Mult_IO_Handlers,
    Install_IO_Handler,
    Enable_Global_Trapping,
    Enable_Local_Trapping,
    Disable_Global_Trapping,
    Disable_Local_Trapping,
    List_Create,
    List_Destroy,
    List_Allocate,
    List_Attach,
    List_Attach_Tail,
    List_Insert,
    List_Remove,
    List_Deallocate,
    List_Get_First,
    List_Get_Next,
    List_Remove_First,
    _AddInstanceItem,
    _Allocate_Device_CB_Area,
    _Allocate_Global_V86_Data_Area,
    _Allocate_Temp_V86_Data_Area,
    _Free_Temp_V86_Data_Area,
    Get_Profile_Decimal_Int,
    Convert_Decimal_String,
    Get_Profile_Fixed_Point,
    Convert_Fixed_Point_String,
    Get_Profile_Hex_Int,
    Convert_Hex_String,
    Get_Profile_Boolean,
    Convert_Boolean_String,
    Get_Profile_String,
    Get_Next_Profile_String,
    Get_Environment_String,
    Get_Exec_Path,
    Get_Config_Directory,
    OpenFile,
    Get_PSP_Segment,
    GetDOSVectors,
    Get_Machine_Info,
    GetSet_HMA_Info,
    Set_System_Exit_Code,
    Fatal_Error_Handler,
    Fatal_Memory_Error,
    Update_System_Clock,
    Test_Debug_Installed,
    Out_Debug_String,
    Out_Debug_Chr,
    In_Debug_Chr,
    Debug_Convert_Hex_Binary,
    Debug_Convert_Hex_Decimal,
    Debug_Test_Valid_Handle,
    Validate_Client_Ptr,
    Test_Reenter,
    Queue_Debug_String,
    Log_Proc_Call,
    Debug_Test_Cur_VM,
    Get_PM_Int_Type,
    Set_PM_Int_Type,
    Get_Last_Updated_System_Time,
    Get_Last_Updated_VM_Exec_Time,
    Test_DBCS_Lead_Byte,
    _AddFreePhysPage,
    _PageResetHandlePAddr,
    _SetLastV86Page,
    _GetLastV86Page,
    _MapFreePhysReg,
    _UnmapFreePhysReg,
    _XchgFreePhysReg,
    _SetFreePhysRegCalBk,
    Get_Next_Arena,
    Get_Name_Of_Ugly_TSR,
    Get_Debug_Options,
    Set_Physical_HMA_Alias,
    _GetGlblRng0V86IntBase,
    _Add_Global_V86_Data_Area,
    GetSetDetailedVMError,
    Is_Debug_Chr,
    Clear_Mono_Screen,
    Out_Mono_Chr,
    Out_Mono_String,
    Set_Mono_Cur_Pos,
    Get_Mono_Cur_Pos,
    Get_Mono_Chr,
    Locate_Byte_In_ROM,
    Hook_Invalid_Page_Fault,
    Unhook_Invalid_Page_Fault,
    Set_Delete_On_Exit_File,
    Close_VM,
    Enable_Touch_1st_Meg,
    Disable_Touch_1st_Meg,
    Install_Exception_Handler,
    Remove_Exception_Handler,
    Get_Crit_Status_No_Block,
    _GetLastUpdatedThreadExecTime,
    _Trace_Out_Service,
    _Debug_Out_Service,
    _Debug_Flags_Service,
    VMMAddImportModuleName,
    VMM_Add_DDB,
    VMM_Remove_DDB,
    Test_VM_Ints_Enabled,
    _BlockOnID,
    Schedule_Thread_Event,
    Cancel_Thread_Event,
    Set_Thread_Time_Out,
    Set_Async_Time_Out,
    _AllocateThreadDataSlot,
    _FreeThreadDataSlot,
    _CreateMutex,
    _DestroyMutex,
    _GetMutexOwner,
    Call_When_Thread_Switched,
    VMMCreateThread,
    _GetThreadExecTime,
    VMMTerminateThread,
    Get_Cur_Thread_Handle,
    Test_Cur_Thread_Handle,
    Get_Sys_Thread_Handle,
    Test_Sys_Thread_Handle,
    Validate_Thread_Handle,
    Get_Initial_Thread_Handle,
    Test_Initial_Thread_Handle,
    Debug_Test_Valid_Thread_Handle,
    Debug_Test_Cur_Thread,
    VMM_GetSystemInitState,
    Cancel_Call_When_Thread_Switched,
    Get_Next_Thread_Handle,
    Adjust_Thread_Exec_Priority,
    _Deallocate_Device_CB_Area,
    Remove_IO_Handler,
    Remove_Mult_IO_Handlers,
    Unhook_V86_Int_Chain,
    Unhook_V86_Fault,
    Unhook_PM_Fault,
    Unhook_VMM_Fault,
    Unhook_Device_Service,
    _PageReserve,
    _PageCommit,
    _PageDecommit,
    _PagerRegister,
    _PagerQuery,
    _PagerDeregister,
    _ContextCreate,
    _ContextDestroy,
    _PageAttach,
    _PageFlush,
    _SignalID,
    _PageCommitPhys,
    _Register_Win32_Services,
    Cancel_Call_When_Not_Critical,
    Cancel_Call_When_Idle,
    Cancel_Call_When_Task_Switched,
    _Debug_Printf_Service,
    _EnterMutex,
    _LeaveMutex,
    Simulate_VM_IO,
    Signal_Semaphore_No_Switch,
    _ContextSwitch,
    _PageModifyPermissions,
    _PageQuery,
    _EnterMustComplete,
    _LeaveMustComplete,
    _ResumeExecMustComplete,
    _GetThreadTerminationStatus,
    _GetInstanceInfo,
    _ExecIntMustComplete,
    _ExecVxDIntMustComplete,
    Begin_V86_Serialization,
    Unhook_V86_Page,
    VMM_GetVxDLocationList,
    VMM_GetDDBList,
    Unhook_NMI_Event,
    Get_Instanced_V86_Int_Vector,
    Get_Set_Real_DOS_PSP,
    Call_Priority_Thread_Event,
    Get_System_Time_Address,
    Get_Crit_Status_Thread,
    Get_DDB,
    Directed_Sys_Control,
    _RegOpenKey,
    _RegCloseKey,
    _RegCreateKey,
    _RegDeleteKey,
    _RegEnumKey,
    _RegQueryValue,
    _RegSetValue,
    _RegDeleteValue,
    _RegEnumValue,
    _RegQueryValueEx,
    _RegSetValueEx,
    _CallRing3,
    Exec_PM_Int,
    _RegFlushKey,
    _PageCommitContig,
    _GetCurrentContext,
    _LocalizeSprintf,
    _LocalizeStackSprintf,
    Call_Restricted_Event,
    Cancel_Restricted_Event,
    Register_PEF_Provider, VMM_ICODE,
    _GetPhysPageInfo,
    _RegQueryInfoKey,
    MemArb_Reserve_Pages,
    Time_Slice_Sys_VM_Idle,
    Time_Slice_Sleep,
    Boost_With_Decay,
    Set_Inversion_Pri,
    Reset_Inversion_Pri,
    Release_Inversion_Pri,
    Get_Thread_Win32_Pri,
    Set_Thread_Win32_Pri,
    Set_Thread_Static_Boost,
    Set_VM_Static_Boost,
    Release_Inversion_Pri_ID,
    Attach_Thread_To_Group,
    Detach_Thread_From_Group,
    Set_Group_Static_Boost,
    _GetRegistryPath,
    _GetRegistryKey,
    Cleanup_Thread_State,
    _RegRemapPreDefKey,
    End_V86_Serialization,
    _Assert_Range,
    _Sprintf,
    _PageChangePager,
    _RegCreateDynKey,
    _RegQueryMultipleValues,
    Boost_Thread_With_VM,
    Get_Boot_Flags,
    Set_Boot_Flags,
    _lstrcpyn,
    _lstrlen,
    _lmemcpy,
    _GetVxDName,
    Force_Mutexes_Free,
    Restore_Forced_Mutexes,
    _AddReclaimableItem,
    _SetReclaimableItem,
    _EnumReclaimableItem,
    Time_Slice_Wake_Sys_VM,
    VMM_Replace_Global_Environment,
    Begin_Non_Serial_Nest_V86_Exec,
    Get_Nest_Exec_Status,
    Open_Boot_Log,
    Write_Boot_Log,
    Close_Boot_Log,
    EnableDisable_Boot_Log,
EndServiceTable()
#endif // 0
#define Get_VMM_Version                     0x00010000
#define Get_Cur_VM_Handle                   0x00010001
#define Test_Cur_VM_Handle                  0x00010002
#define Get_Sys_VM_Handle                   0x00010003
#define Test_Sys_VM_Handle                  0x00010004
#define Validate_VM_Handle                  0x00010005
#define Get_VMM_Reenter_Count               0x00010006
#define Begin_Reentrant_Execution           0x00010007
#define End_Reentrant_Execution             0x00010008
#define Install_V86_Break_Point             0x00010009
#define Remove_V86_Break_Point              0x0001000a
#define Allocate_V86_Call_Back              0x0001000b
#define Allocate_PM_Call_Back               0x0001000c
#define Call_When_VM_Returns                0x0001000d
#define Schedule_Global_Event               0x0001000e
#define Schedule_VM_Event                   0x0001000f
#define Call_Global_Event                   0x00010010
#define Call_VM_Event                       0x00010011
#define Cancel_Global_Event                 0x00010012
#define Cancel_VM_Event                     0x00010013
#define Call_Priority_VM_Event              0x00010014
#define Cancel_Priority_VM_Event            0x00010015
#define Get_NMI_Handler_Addr                0x00010016
#define Set_NMI_Handler_Addr                0x00010017
#define Hook_NMI_Event                      0x00010018
#define Call_When_VM_Ints_Enabled           0x00010019
#define Enable_VM_Ints                      0x0001001a
#define Disable_VM_Ints                     0x0001001b
#define Map_Flat                            0x0001001c
#define Map_Lin_To_VM_Addr                  0x0001001d
#define Adjust_Exec_Priority                0x0001001e
#define Begin_Critical_Section              0x0001001f
#define End_Critical_Section                0x00010020
#define End_Crit_And_Suspend                0x00010021
#define Claim_Critical_Section              0x00010022
#define Release_Critical_Section            0x00010023
#define Call_When_Not_Critical              0x00010024
#define Create_Semaphore                    0x00010025
#define Destroy_Semaphore                   0x00010026
#define Wait_Semaphore                      0x00010027
#define Signal_Semaphore                    0x00010028
#define Get_Crit_Section_Status             0x00010029
#define Call_When_Task_Switched             0x0001002a
#define Suspend_VM                          0x0001002b
#define Resume_VM                           0x0001002c
#define No_Fail_Resume_VM                   0x0001002d
#define Nuke_VM                             0x0001002e
#define Crash_Cur_VM                        0x0001002f
#define Get_Execution_Focus                 0x00010030
#define Set_Execution_Focus                 0x00010031
#define Get_Time_Slice_Priority             0x00010032
#define Set_Time_Slice_Priority             0x00010033
#define Get_Time_Slice_Granularity          0x00010034
#define Set_Time_Slice_Granularity          0x00010035
#define Get_Time_Slice_Info                 0x00010036
#define Adjust_Execution_Time               0x00010037
#define Release_Time_Slice                  0x00010038
#define Wake_Up_VM                          0x00010039
#define Call_When_Idle                      0x0001003a
#define Get_Next_VM_Handle                  0x0001003b
#define Set_Global_Time_Out                 0x0001003c
#define Set_VM_Time_Out                     0x0001003d
#define Cancel_Time_Out                     0x0001003e
#define Get_System_Time                     0x0001003f
#define Get_VM_Exec_Time                    0x00010040
#define Hook_V86_Int_Chain                  0x00010041
#define Get_V86_Int_Vector                  0x00010042
#define Set_V86_Int_Vector                  0x00010043
#define Get_PM_Int_Vector                   0x00010044
#define Set_PM_Int_Vector                   0x00010045
#define Simulate_Int                        0x00010046
#define Simulate_Iret                       0x00010047
#define Simulate_Far_Call                   0x00010048
#define Simulate_Far_Jmp                    0x00010049
#define Simulate_Far_Ret                    0x0001004a
#define Simulate_Far_Ret_N                  0x0001004b
#define Build_Int_Stack_Frame               0x0001004c
#define Simulate_Push                       0x0001004d
#define Simulate_Pop                        0x0001004e
#define _HeapAllocate                       0x0001004f
#define _HeapReAllocate                     0x00010050
#define _HeapFree                           0x00010051
#define _HeapGetSize                        0x00010052
#define _PageAllocate                       0x00010053
#define _PageReAllocate                     0x00010054
#define _PageFree                           0x00010055
#define _PageLock                           0x00010056
#define _PageUnLock                         0x00010057
#define _PageGetSizeAddr                    0x00010058
#define _PageGetAllocInfo                   0x00010059
#define _GetFreePageCount                   0x0001005a
#define _GetSysPageCount                    0x0001005b
#define _GetVMPgCount                       0x0001005c
#define _MapIntoV86                         0x0001005d
#define _PhysIntoV86                        0x0001005e
#define _TestGlobalV86Mem                   0x0001005f
#define _ModifyPageBits                     0x00010060
#define _CopyPageTable                      0x00010061
#define _LinMapIntoV86                      0x00010062
#define _LinPageLock                        0x00010063
#define _LinPageUnLock                      0x00010064
#define _SetResetV86Pageable                0x00010065
#define _GetV86PageableArray                0x00010066
#define _PageCheckLinRange                  0x00010067
#define _PageOutDirtyPages                  0x00010068
#define _PageDiscardPages                   0x00010069
#define _GetNulPageHandle                   0x0001006a
#define _GetFirstV86Page                    0x0001006b
#define _MapPhysToLinear                    0x0001006c
#define _GetAppFlatDSAlias                  0x0001006d
#define _SelectorMapFlat                    0x0001006e
#define _GetDemandPageInfo                  0x0001006f
#define _GetSetPageOutCount                 0x00010070
#define Hook_V86_Page                       0x00010071
#define _Assign_Device_V86_Pages            0x00010072
#define _DeAssign_Device_V86_Pages          0x00010073
#define _Get_Device_V86_Pages_Array         0x00010074
#define MMGR_SetNULPageAddr                 0x00010075
#define _Allocate_GDT_Selector              0x00010076
#define _Free_GDT_Selector                  0x00010077
#define _Allocate_LDT_Selector              0x00010078
#define _Free_LDT_Selector                  0x00010079
#define _BuildDescriptorDWORDs              0x0001007a
#define _GetDescriptor                      0x0001007b
#define _SetDescriptor                      0x0001007c
#define _MMGR_Toggle_HMA                    0x0001007d
#define Get_Fault_Hook_Addrs                0x0001007e
#define Hook_V86_Fault                      0x0001007f
#define Hook_PM_Fault                       0x00010080
#define Hook_VMM_Fault                      0x00010081
#define Begin_Nest_V86_Exec                 0x00010082
#define Begin_Nest_Exec                     0x00010083
#define Exec_Int                            0x00010084
#define Resume_Exec                         0x00010085
#define End_Nest_Exec                       0x00010086
#define Allocate_PM_App_CB_Area             0x00010087
#define Get_Cur_PM_App_CB                   0x00010088
#define Set_V86_Exec_Mode                   0x00010089
#define Set_PM_Exec_Mode                    0x0001008a
#define Begin_Use_Locked_PM_Stack           0x0001008b
#define End_Use_Locked_PM_Stack             0x0001008c
#define Save_Client_State                   0x0001008d
#define Restore_Client_State                0x0001008e
#define Exec_VxD_Int                        0x0001008f
#define Hook_Device_Service                 0x00010090
#define Hook_Device_V86_API                 0x00010091
#define Hook_Device_PM_API                  0x00010092
#define System_Control                      0x00010093
#define Simulate_IO                         0x00010094
#define Install_Mult_IO_Handlers            0x00010095
#define Install_IO_Handler                  0x00010096
#define Enable_Global_Trapping              0x00010097
#define Enable_Local_Trapping               0x00010098
#define Disable_Global_Trapping             0x00010099
#define Disable_Local_Trapping              0x0001009a
#define List_Create                         0x0001009b
#define List_Destroy                        0x0001009c
#define List_Allocate                       0x0001009d
#define List_Attach                         0x0001009e
#define List_Attach_Tail                    0x0001009f
#define List_Insert                         0x000100a0
#define List_Remove                         0x000100a1
#define List_Deallocate                     0x000100a2
#define List_Get_First                      0x000100a3
#define List_Get_Next                       0x000100a4
#define List_Remove_First                   0x000100a5
#define _AddInstanceItem                    0x000100a6
#define _Allocate_Device_CB_Area            0x000100a7
#define _Allocate_Global_V86_Data_Area      0x000100a8
#define _Allocate_Temp_V86_Data_Area        0x000100a9
#define _Free_Temp_V86_Data_Area            0x000100aa
#define Get_Profile_Decimal_Int             0x000100ab
#define Convert_Decimal_String              0x000100ac
#define Get_Profile_Fixed_Point             0x000100ad
#define Convert_Fixed_Point_String          0x000100ae
#define Get_Profile_Hex_Int                 0x000100af
#define Convert_Hex_String                  0x000100b0
#define Get_Profile_Boolean                 0x000100b1
#define Convert_Boolean_String              0x000100b2
#define Get_Profile_String                  0x000100b3
#define Get_Next_Profile_String             0x000100b4
#define Get_Environment_String              0x000100b5
#define Get_Exec_Path                       0x000100b6
#define Get_Config_Directory                0x000100b7
#define OpenFile                            0x000100b8
#define Get_PSP_Segment                     0x000100b9
#define GetDOSVectors                       0x000100ba
#define Get_Machine_Info                    0x000100bb
#define GetSet_HMA_Info                     0x000100bc
#define Set_System_Exit_Code                0x000100bd
#define Fatal_Error_Handler                 0x000100be
#define Fatal_Memory_Error                  0x000100bf
#define Update_System_Clock                 0x000100c0
#define Test_Debug_Installed                0x000100c1
#define Out_Debug_String                    0x000100c2
#define Out_Debug_Chr                       0x000100c3
#define In_Debug_Chr                        0x000100c4
#define Debug_Convert_Hex_Binary            0x000100c5
#define Debug_Convert_Hex_Decimal           0x000100c6
#define Debug_Test_Valid_Handle             0x000100c7
#define Validate_Client_Ptr                 0x000100c8
#define Test_Reenter                        0x000100c9
#define Queue_Debug_String                  0x000100ca
#define Log_Proc_Call                       0x000100cb
#define Debug_Test_Cur_VM                   0x000100cc
#define Get_PM_Int_Type                     0x000100cd
#define Set_PM_Int_Type                     0x000100ce
#define Get_Last_Updated_System_Time        0x000100cf
#define Get_Last_Updated_VM_Exec_Time       0x000100d0
#define Test_DBCS_Lead_Byte                 0x000100d1
#define _AddFreePhysPage                    0x000100d2
#define _PageResetHandlePAddr               0x000100d3
#define _SetLastV86Page                     0x000100d4
#define _GetLastV86Page                     0x000100d5
#define _MapFreePhysReg                     0x000100d6
#define _UnmapFreePhysReg                   0x000100d7
#define _XchgFreePhysReg                    0x000100d8
#define _SetFreePhysRegCalBk                0x000100d9
#define Get_Next_Arena                      0x000100da
#define Get_Name_Of_Ugly_TSR                0x000100db
#define Get_Debug_Options                   0x000100dc
#define Set_Physical_HMA_Alias              0x000100dd
#define _GetGlblRng0V86IntBase              0x000100de
#define _Add_Global_V86_Data_Area           0x000100df
#define GetSetDetailedVMError               0x000100e0
#define Is_Debug_Chr                        0x000100e1
#define Clear_Mono_Screen                   0x000100e2
#define Out_Mono_Chr                        0x000100e3
#define Out_Mono_String                     0x000100e4
#define Set_Mono_Cur_Pos                    0x000100e5
#define Get_Mono_Cur_Pos                    0x000100e6
#define Get_Mono_Chr                        0x000100e7
#define Locate_Byte_In_ROM                  0x000100e8
#define Hook_Invalid_Page_Fault             0x000100e9
#define Unhook_Invalid_Page_Fault           0x000100ea
#define Set_Delete_On_Exit_File             0x000100eb
#define Close_VM                            0x000100ec
#define Enable_Touch_1st_Meg                0x000100ed
#define Disable_Touch_1st_Meg               0x000100ee
#define Install_Exception_Handler           0x000100ef
#define Remove_Exception_Handler            0x000100f0
#define Get_Crit_Status_No_Block            0x000100f1
#define _GetLastUpdatedThreadExecTime       0x000100f2
#define _Trace_Out_Service                  0x000100f3
#define _Debug_Out_Service                  0x000100f4
#define _Debug_Flags_Service                0x000100f5
#define VMMAddImportModuleName              0x000100f6
#define VMM_Add_DDB                         0x000100f7
#define VMM_Remove_DDB                      0x000100f8
#define Test_VM_Ints_Enabled                0x000100f9
#define _BlockOnID                          0x000100fa
#define Schedule_Thread_Event               0x000100fb
#define Cancel_Thread_Event                 0x000100fc
#define Set_Thread_Time_Out                 0x000100fd
#define Set_Async_Time_Out                  0x000100fe
#define _AllocateThreadDataSlot             0x000100ff
#define _FreeThreadDataSlot                 0x00010100
#define _CreateMutex                        0x00010101
#define _DestroyMutex                       0x00010102
#define _GetMutexOwner                      0x00010103
#define Call_When_Thread_Switched           0x00010104
#define VMMCreateThread                     0x00010105
#define _GetThreadExecTime                  0x00010106
#define VMMTerminateThread                  0x00010107
#define Get_Cur_Thread_Handle               0x00010108
#define Test_Cur_Thread_Handle              0x00010109
#define Get_Sys_Thread_Handle               0x0001010a
#define Test_Sys_Thread_Handle              0x0001010b
#define Validate_Thread_Handle              0x0001010c
#define Get_Initial_Thread_Handle           0x0001010d
#define Test_Initial_Thread_Handle          0x0001010e
#define Debug_Test_Valid_Thread_Handle      0x0001010f
#define Debug_Test_Cur_Thread               0x00010110
#define VMM_GetSystemInitState              0x00010111
#define Cancel_Call_When_Thread_Switched    0x00010112
#define Get_Next_Thread_Handle              0x00010113
#define Adjust_Thread_Exec_Priority         0x00010114
#define _Deallocate_Device_CB_Area          0x00010115
#define Remove_IO_Handler                   0x00010116
#define Remove_Mult_IO_Handlers             0x00010117
#define Unhook_V86_Int_Chain                0x00010118
#define Unhook_V86_Fault                    0x00010119
#define Unhook_PM_Fault                     0x0001011a
#define Unhook_VMM_Fault                    0x0001011b
#define Unhook_Device_Service               0x0001011c
#define _PageReserve                        0x0001011d
#define _PageCommit                         0x0001011e
#define _PageDecommit                       0x0001011f
#define _PagerRegister                      0x00010120
#define _PagerQuery                         0x00010121
#define _PagerDeregister                    0x00010122
#define _ContextCreate                      0x00010123
#define _ContextDestroy                     0x00010124
#define _PageAttach                         0x00010125
#define _PageFlush                          0x00010126
#define _SignalID                           0x00010127
#define _PageCommitPhys                     0x00010128
#define _Register_Win32_Services            0x00010129
#define Cancel_Call_When_Not_Critical       0x0001012a
#define Cancel_Call_When_Idle               0x0001012b
#define Cancel_Call_When_Task_Switched      0x0001012c
#define _Debug_Printf_Service               0x0001012d
#define _EnterMutex                         0x0001012e
#define _LeaveMutex                         0x0001012f
#define Simulate_VM_IO                      0x00010130
#define Signal_Semaphore_No_Switch          0x00010131
#define _ContextSwitch                      0x00010132
#define _PageModifyPermissions              0x00010133
#define _PageQuery                          0x00010134
#define _EnterMustComplete                  0x00010135
#define _LeaveMustComplete                  0x00010136
#define _ResumeExecMustComplete             0x00010137
#define _GetThreadTerminationStatus         0x00010138
#define _GetInstanceInfo                    0x00010139
#define _ExecIntMustComplete                0x0001013a
#define _ExecVxDIntMustComplete             0x0001013b
#define Begin_V86_Serialization             0x0001013c
#define Unhook_V86_Page                     0x0001013d
#define VMM_GetVxDLocationList              0x0001013e
#define VMM_GetDDBList                      0x0001013f
#define Unhook_NMI_Event                    0x00010140
#define Get_Instanced_V86_Int_Vector        0x00010141
#define Get_Set_Real_DOS_PSP                0x00010142
#define Call_Priority_Thread_Event          0x00010143
#define Get_System_Time_Address             0x00010144
#define Get_Crit_Status_Thread              0x00010145
#define Get_DDB                             0x00010146
#define Directed_Sys_Control                0x00010147
#define _RegOpenKey                         0x00010148
#define _RegCloseKey                        0x00010149
#define _RegCreateKey                       0x0001014a
#define _RegDeleteKey                       0x0001014b
#define _RegEnumKey                         0x0001014c
#define _RegQueryValue                      0x0001014d
#define _RegSetValue                        0x0001014e
#define _RegDeleteValue                     0x0001014f
#define _RegEnumValue                       0x00010150
#define _RegQueryValueEx                    0x00010151
#define _RegSetValueEx                      0x00010152
#define _CallRing3                          0x00010153
#define Exec_PM_Int                         0x00010154
#define _RegFlushKey                        0x00010155
#define _PageCommitContig                   0x00010156
#define _GetCurrentContext                  0x00010157
#define _LocalizeSprintf                    0x00010158
#define _LocalizeStackSprintf               0x00010159
#define Call_Restricted_Event               0x0001015a
#define Cancel_Restricted_Event             0x0001015b
#define Register_PEF_Provider               0x0001015c
#define _GetPhysPageInfo                    0x0001015d
#define _RegQueryInfoKey                    0x0001015e
#define MemArb_Reserve_Pages                0x0001015f
#define Time_Slice_Sys_VM_Idle              0x00010160
#define Time_Slice_Sleep                    0x00010161
#define Boost_With_Decay                    0x00010162
#define Set_Inversion_Pri                   0x00010163
#define Reset_Inversion_Pri                 0x00010164
#define Release_Inversion_Pri               0x00010165
#define Get_Thread_Win32_Pri                0x00010166
#define Set_Thread_Win32_Pri                0x00010167
#define Set_Thread_Static_Boost             0x00010168
#define Set_VM_Static_Boost                 0x00010169
#define Release_Inversion_Pri_ID            0x0001016a
#define Attach_Thread_To_Group              0x0001016b
#define Detach_Thread_From_Group            0x0001016c
#define Set_Group_Static_Boost              0x0001016d
#define _GetRegistryPath, VMM_ICODE         0x0001016e
#define _GetRegistryKey                     0x0001016f
#define Cleanup_Thread_State                0x00010170
#define _RegRemapPreDefKey                  0x00010171
#define End_V86_Serialization               0x00010172
#define _Assert_Range                       0x00010173
#define _Sprintf                            0x00010174
#define _PageChangePager                    0x00010175
#define _RegCreateDynKey                    0x00010176
#define _RegQueryMultipleValues             0x00010177
#define Boost_Thread_With_VM                0x00010178
#define Get_Boot_Flags                      0x00010179
#define Set_Boot_Flags                      0x0001017a
#define _lstrcpyn                           0x0001017b
#define _lstrlen                            0x0001017c
#define _lmemcpy                            0x0001017d
#define _GetVxDName                         0x0001017e
#define Force_Mutexes_Free                  0x0001017f
#define Restore_Forced_Mutexes              0x00010180
#define _AddReclaimableItem                 0x00010181
#define _SetReclaimableItem                 0x00010182
#define _EnumReclaimableItem                0x00010183
#define Time_Slice_Wake_Sys_VM              0x00010184
#define VMM_Replace_Global_Environment      0x00010185
#define Begin_Non_Serial_Nest_V86_Exec      0x00010186
#define Get_Nest_Exec_Status                0x00010187
#define Open_Boot_Log                       0x00010188
#define Write_Boot_Log                      0x00010189
#define Close_Boot_Log                      0x0001018a
#define EnableDisable_Boot_Log              0x0001018b

//----------------------------------------------------------------------------
//
//                    Flags for page allocator calls
//
//----------------------------------------------------------------------------

#define PageZeroInit            0x00000001
#define PageUseAlign            0x00000002
#define PageContig              0x00000004
#define PageFixed               0x00000008
#define PageDEBUGNulFault       0x00000010
#define PageZeroReInit          0x00000020
#define PageNoCopy              0x00000040
#define PageLocked              0x00000080
#define PageLockedIfDP          0x00000100
#define PageSetV86Pageable      0x00000200
#define PageClearV86Pageable    0x00000400
#define PageSetV86IntsLocked    0x00000800
#define PageClearV86IntsLocked  0x00001000
#define PageMarkPageOut         0x00002000
#define PagePDPSetBase          0x00004000
#define PagePDPClearBase        0x00008000
#define PageDiscard             0x00010000
#define PagePDPQueryDirty       0x00020000
//
// New for 3.10
//
#define PageMapFreePhysReg      0x00040000
//
// NOTE: HIGH 8 BITS (bits 24-31) are reserved
//
//----------------------------------------------------------------------------
//
//                         Page table entry bits
//
//----------------------------------------------------------------------------

#define P_SIZE        0x1000  // page size
#define P_PRES        0x01    // page present bit
#define P_PRESBit     0
#define P_WRITE       0x02    // write access bit
#define P_WRITEBit    1
#define P_USER        0x04    // access bit for User mode
#define P_USERBit     2
#define P_ACC         0x20    // page accessed bit
#define P_ACCBit      5
#define P_DIRTY       0x40    // page dirty bit
#define P_DIRTYBit    6
#define P_AVAIL       (P_PRES|P_WRITE|P_USER) // avail to everyone & present

//----------------------------------------------------------------------------
//
// Page types - definition of the OS reserved
//              bits in the page table entry.
//
//----------------------------------------------------------------------------

#define PG_TYPE       0x0E00  // TYPE bits in PTE

//----------------------------------------------------------------------------
//
// Page types for page allocator calls
//
//----------------------------------------------------------------------------

#define PG_VM         0
#define PG_SYS        1
#define PG_RESERVED1  2
#define PG_PRIVATE    3
#define PG_RESERVED2  4
#define PG_RELOCK     5      // PRIVATE to MMGR
#define PG_INSTANCE   6
#define PG_HOOKED     7
#define PG_IGNORE     0x0FFFFFFFF

//----------------------------------------------------------------------------
//
// Types for page table entries
//
//----------------------------------------------------------------------------

#define PgT_VM        (PG_VM << 9)
#define PgT_SYS       (PG_SYS << 9)
#define PgT_RESERVED1 (PG_RESERVED1 << 9)
#define PgT_PRIVATE   (PG_PRIVATE << 9)
#define PgT_RESERVED2 (PG_RESERVED2 << 9)
#define PgT_RELOCK    (PG_RELOCK << 9)
#define PgT_INSTANCE  (PG_INSTANCE << 9)
#define PgT_HOOKED    (PG_HOOKED << 9)

//----------------------------------------------------------------------------
//
// Definitions for the access byte in a descriptor
//
//----------------------------------------------------------------------------

#define D_PRES       0x80     // present in memory
#define D_NOTPRES    0x00     // not present in memory
#define D_DPL0       0x00     // Ring 0
#define D_DPL1       0x20     // Ring 1
#define D_DPL2       0x40     // Ring 2
#define D_DPL3       0x60     // Ring 3
#define D_SEG        0x10     // Segment descriptor
#define D_CTRL       0x00     // Control descriptor
#define D_GRAN_BYTE  0x00     // Segment length is byte granular
#define D_GRAN_PAGE  0x80     // Segment length is page granular
#define D_DEF16      0x00     // Default operation size is 16 bits
#define D_DEF32      0x40     // Default operation size is 32 bits
#define D_CODE       0x08     // code
#define D_DATA       0x00     // data
#define D_RX         0x02     // if code, readable
#define D_X          0x00     // if code, exec only
#define D_ED         0x04     // if expand down
#define D_EU         0x00     // if expand up
#define D_W          0x02     // if data, writable
#define D_R          0x00     // if data, read only
#define D_ACCESSED   0x01     // segment accessed bit
#define RW_Data_Type (D_PRES+D_SEG+D_DATA+D_W)
#define R_Data_Type  (D_PRES+D_SEG+D_DATA+D_R)
#define Code_Type    (D_PRES+D_SEG+D_CODE+D_RX)
#define D_PAGE32     (D_GRAN_PAGE+D_DEF32)
#define SELECTOR_MASK  0xfff8      // selector index
#define SEL_LOW_MASK   0x00f8      // mask for low byte of sel indx
#define TABLE_MASK     0x0004      // table bit
#define RPL_MASK       0x0003      // priviledge bits
#define RPL_CLR        (~0x0003)   // clear ring bits

//----------------------------------------------------------------------------
//
// Flags for List_Create
//
//----------------------------------------------------------------------------

#define LF_Async            0x0001
#define LF_Async_Bit        0
#define LF_Use_Heap         0x0002
#define LF_Use_Heap_Bit     1
#define LF_Alloc_Error      0x0004
#define LF_Alloc_Error_Bit  2

//----------------------------------------------------------------------------
//
// Scheduler Boost Values
//
//----------------------------------------------------------------------------

#define Reserved_Low_Boost      0x00000001
#define Cur_Run_VM_Boost        0x00000004
#define Low_Pri_Device_Boost    0x00000010
#define High_Pri_Device_Boost   0x00001000
#define Critical_Section_Boost  0x00100000
#define Time_Critical_Boost     0x00400000
#define Reserved_High_Boost     0x40000000

//----------------------------------------------------------------------------
//
// Flags for Call_Priority_VM_Event
//
//----------------------------------------------------------------------------

#define PEF_Wait_For_STI       0x0001
#define PEF_Wait_For_STI_Bit   0
#define PEF_Wait_Not_Crit      0x0002
#define PEF_Wait_Not_Crit_Bit  1
#define PEF_Dont_Unboost       0x0004
#define PEF_Dont_Unboost_Bit   2
#define PEF_Always_Sched       0x0008
#define PEF_Always_Sched_Bit   3
#define PEF_Time_Out           0x0010
#define PEF_Time_Out_Bit       4

//----------------------------------------------------------------------------
//
// Flags for Begin_Critical_Section and Wait_Semaphore
//
//----------------------------------------------------------------------------

#define Block_Svc_Ints                0x0001
#define Block_Svc_Ints_Bit            0
#define Block_Svc_If_Ints_Locked      0x0002
#define Block_Svc_If_Ints_Locked_Bit  1
#define Block_Enable_Ints             0x0004
#define Block_Enable_Ints_Bit         2
#define Block_Poll                    0x0008
#define Block_Poll_Bit                3

//----------------------------------------------------------------------------
//
// Data structures
//
//----------------------------------------------------------------------------

typedef struct tagREGS
{
    DWORD  eflags ;
    DWORD  edi ;
    DWORD  esi ;
    DWORD  ebp ;
    DWORD  esp ;
    DWORD  ebx ;
    DWORD  edx ;
    DWORD  ecx ;
    DWORD  eax ;
} REGS, *PREGS ;

#define EFLAGS_CF   0x00000001
#define EFLAGS_PF   0x00000004
#define EFLAGS_AF   0x00000010
#define EFLAGS_ZF   0x00000040
#define EFLAGS_SF   0x00000080

typedef struct tagCRS_32
{
   DWORD  Client_EDI ;
   DWORD  Client_ESI ;
   DWORD  Client_EBP ;
   DWORD  dwReserved_1 ;          // ESP at pushall
   DWORD  Client_EBX ;
   DWORD  Client_EDX ;
   DWORD  Client_ECX ;
   DWORD  Client_EAX ;
   DWORD  Client_Error ;          // DWORD error code
   DWORD  Client_EIP ;
   WORD   Client_CS ;
   WORD   wReserved_2 ;           // (padding)
   DWORD  Client_EFlags ;
   DWORD  Client_ESP ;
   WORD   Client_SS ;
   WORD   wReserved_3 ;           // (padding)
   WORD   Client_ES ;
   WORD   WReserved_4 ;           // (padding)
   WORD   Client_DS ;
   WORD   wReserved_5 ;           // (padding)
   WORD   Client_FS ;
   WORD   wReserved_6 ;           // (padding)
   WORD   Client_GS ;
   WORD   wReserved_7 ;           // (padding)

   DWORD  Client_Alt_EIP ;
   WORD   Client_Alt_CS ;
   WORD   wReserved_8 ;           // (padding)
   DWORD  Client_Alt_EFlags ;
   DWORD  Client_Alt_ESP ;
   WORD   Client_Alt_SS ;
   WORD   wReserved_9 ;           // (padding)
   WORD   Client_Alt_ES ;
   WORD   WReserved_10 ;          // (padding)
   WORD   Client_Alt_DS ;
   WORD   wReserved_11 ;          // (padding)
   WORD   Client_Alt_FS ;
   WORD   wReserved_12 ;          // (padding)
   WORD   Client_Alt_GS ;
   WORD   wReserved_13 ;          // (padding)
} CRS_32, *PCRS_32 ;

typedef struct tagCRS_16
{
   WORD   Client_DI ;
   WORD   wReserved_1 ;           // (padding)
   WORD   Client_SI ;
   WORD   wReserved_2 ;           // (padding)
   WORD   Client_BP ;
   WORD   wReserved_3 ;           // (padding)
   DWORD  dwReserved_4 ;          // ESP at pushall
   WORD   Client_BX ;
   WORD   wReserved_5 ;           // (padding)
   WORD   Client_DX ;
   WORD   wReserved_6 ;           // (padding)
   WORD   Client_CX ;
   WORD   wReserved_7 ;           // (padding)
   WORD   Client_AX ;
   WORD   wReserved_8 ;           // (padding)
   DWORD  dwReserved_9 ;          // DWORD error code
   WORD   Client_IP ;
   WORD   wReserved_10 ;          // (padding)
   WORD   Client_CS ;
   WORD   wReserved_11 ;          // (padding)
   WORD   Client_Flags ;          // flags (loword)
   WORD   wReserved_12 ;          // (padding)
   WORD   Client_SP ;
   WORD   wReserved_13 ;          // (padding)
   WORD   Client_SS ;
   WORD   wReserved_14 ;          // (padding)
   WORD   Client_ES ;
   WORD   wReserved_15 ;          // (padding)
   WORD   Client_DS ;
   WORD   wReserved_16 ;          // (padding)
   WORD   Client_FS ;
   WORD   wReserved_17 ;          // (padding)
   WORD   Client_GS ;
   WORD   wReserved_18 ;          // (padding)

   WORD   Client_Alt_IP ;
   WORD   wReserved_19 ;          // (padding)
   WORD   Client_Alt_CS ;
   WORD   wReserved_20 ;          // (padding)
   WORD   Client_Alt_Flags ;
   WORD   wReserved_21 ;          // (padding)
   WORD   Client_Alt_SP ;
   WORD   wReserved_22 ;          // (padding)
   WORD   Client_Alt_SS ;
   WORD   wReserved_23 ;          // (padding)
   WORD   Client_Alt_ES ;
   WORD   wReserved_24 ;          // (padding)
   WORD   Client_Alt_DS ;
   WORD   wReserved_25 ;          // (padding)
   WORD   Client_Alt_FS ;
   WORD   wReserved_26 ;          // (padding)
   WORD   Client_Alt_GS ;
   WORD   wReserved_27 ;          // (padding)
} CRS_16, *PCRS_16 ;

typedef struct tagCRS_8
{
   WORD   Client_DI ;
   WORD   wReserved_1 ;           // (padding)
   WORD   Client_SI ;
   WORD   wReserved_2 ;           // (padding)
   WORD   Client_BP ;
   WORD   wReserved_3 ;           // (padding)
   DWORD  dwReserved_4 ;          // ESP at pushall
   BYTE   Client_BL ;
   BYTE   Client_BH ;
   WORD   wReserved_5 ;           // (padding)
   BYTE   Client_DL ;
   BYTE   Client_DH ;
   WORD   wReserved_6 ;           // (padding)
   BYTE   Client_CL ;
   BYTE   Client_CH ;
   WORD   wReserved_7 ;           // (padding)
   BYTE   Client_AL ;
   BYTE   Client_AH ;
   WORD   wReserved_8 ;           // (padding)
   DWORD  dwReserved_9 ;          // DWORD error code
   WORD   Client_IP ;
   WORD   wReserved_10 ;          // (padding)
   WORD   Client_CS ;
   WORD   wReserved_11 ;          // (padding)
   WORD   Client_Flags ;          // flags (loword)
   WORD   wReserved_12 ;          // (padding)
   WORD   Client_SP ;
   WORD   wReserved_13 ;          // (padding)
   WORD   Client_SS ;
   WORD   wReserved_14 ;          // (padding)
   WORD   Client_ES ;
   WORD   wReserved_15 ;          // (padding)
   WORD   Client_DS ;
   WORD   wReserved_16 ;          // (padding)
   WORD   Client_FS ;
   WORD   wReserved_17 ;          // (padding)
   WORD   Client_GS ;
   WORD   wReserved_18 ;          // (padding)

   WORD   Client_Alt_IP ;
   WORD   wReserved_19 ;          // (padding)
   WORD   Client_Alt_CS ;
   WORD   wReserved_20 ;          // (padding)
   WORD   Client_Alt_Flags ;
   WORD   wReserved_21 ;          // (padding)
   WORD   Client_Alt_SP ;
   WORD   wReserved_22 ;          // (padding)
   WORD   Client_Alt_SS ;
   WORD   wReserved_23 ;          // (padding)
   WORD   Client_Alt_ES ;
   WORD   wReserved_24 ;          // (padding)
   WORD   Client_Alt_DS ;
   WORD   wReserved_25 ;          // (padding)
   WORD   Client_Alt_FS ;
   WORD   wReserved_26 ;          // (padding)
   WORD   Client_Alt_GS ;
   WORD   wReserved_27 ;          // (padding)
} CRS_8, *PCRS_8 ;

typedef union tagCRS
{
    CRS_32 Regs32;
    CRS_16 Regs16;
    CRS_8  Regs8;
} CRS, *PCRS ;

//----------------------------------------------------------------------------
//
// VM control block structure (VMM)
//
//----------------------------------------------------------------------------

typedef struct tagVMMCB
{
   DWORD  CB_VM_Status ;
   DWORD  CB_High_Linear ;
   DWORD  CB_Client_Pointer ;
   DWORD  CB_VMID ;
} VMMCB, *PVMMCB ;

#define VMCB_ID 0x62634D56      /* VMcb */

// VM status indicates globally interesting VM states

#define VMStat_Exclusive           0x00001 // VM is exclusive mode
#define VMStat_Exclusive_Bit       0x00
#define VMStat_Background          0x00002 // VM runs in background
#define VMStat_Background_Bit      0x01
#define VMStat_Creating            0x00004 // In process of creating
#define VMStat_Creating_Bit        0x02
#define VMStat_Suspended           0x00008 // VM not scheduled
#define VMStat_Suspended_Bit       0x03
#define VMStat_Not_Executeable     0x00010 // VM partially destroyed
#define VMStat_Not_Executeable_Bit 0x04
#define VMStat_PM_Exec             0x00020 // Currently in PM app
#define VMStat_PM_Exec_Bit         0x05
#define VMStat_PM_App              0x00040 // PM app present in VM
#define VMStat_PM_App_Bit          0x06
#define VMStat_PM_Use32            0x00080 // PM app is 32-bit
#define VMStat_PM_Use32_Bit        0x07
#define VMStat_VxD_Exec            0x00100 // Call from VxD
#define VMStat_VxD_Exec_Bit        0x08
#define VMStat_High_Pri_Back       0x00200 // High pri background
#define VMStat_High_Pri_Back_Bit   0x09
#define VMStat_Blocked             0x00400 // Blocked on semaphore
#define VMStat_Blocked_Bit         0x0A
#define VMStat_Awakening           0x00800 // Woke up after blocked
#define VMStat_Awakening_Bit       0x0B
#define VMStat_PageableV86         0x01000 // part of V86 is pageable (PM app)
#define VMStat_PageableV86Bit      0x0C
#define VMStat_V86IntsLocked       0x02000 // Rest of V86 is locked
#define VMStat_V86IntsLockedBit    0x0D    //    regardless of pager type
#define VMStat_TS_Sched            0x04000 // Scheduled by time-slicer
#define VMStat_TS_Sched_Bit        0x0E
#define VMStat_Idle                0x08000 // VM has released time slice
#define VMStat_Idle_Bit            0x0F
#define VMStat_Closing             0x10000 // Close_VM called for VM
#define VMStat_Closing_Bit         0x10

#define VMStat_Use32_Mask          (VMStat_PM_Use32 | VMStat_VxD_Exec)

typedef struct tagTCB
{
    DWORD   TCB_Flags;              // Thread status flags
    DWORD   TCB_Reserved1;          // Used internally by VMM
    DWORD   TCB_Reserved2;          // Used internally by VMM
    DWORD   TCB_Signature;
    DWORD   TCB_ClientPtr;          // Client registers of thread
    DWORD   TCB_VMHandle;           // VM that thread is part of
    WORD    TCB_ThreadId;           // Unique Thread ID
    WORD    TCB_PMLockOrigSS;       // Original SS:ESP before lock stack
    DWORD   TCB_PMLockOrigESP;
    DWORD   TCB_PMLockOrigEIP;      // Original CS:EIP before lock stack
    DWORD   TCB_PMLockStackCount;
    WORD    TCB_PMLockOrigCS;
    WORD    TCB_PMPSPSelector;
    DWORD   TCB_ThreadType;         // dword passed to VMMCreateThread
    WORD    TCB_pad1;               // reusable; for dword align
    BYTE    TCB_pad2;               // reusable; for dword align
    BYTE    TCB_extErrLocus;        // extended error Locus
    WORD    TCB_extErr;             // extended error Code
    BYTE    TCB_extErrAction;       //      "   "   Action
    BYTE    TCB_extErrClass;        //      "   "   Class
    DWORD   TCB_extErrPtr;          //      "   pointer
} TCB, *PTCB;

#define SCHED_OBJ_ID_THREAD         0x42434854    // THCB in ASCII

//
//  Thread status indicates globally interesting thread states.
//  Flags are for information only and must not be modified.
//

#define THFLAG_SUSPENDED_Bit        0x03   // Thread not scheduled
#define THFLAG_SUSPENDED            (1L << THFLAG_SUSPENDED_Bit)
#define THFLAG_NOT_EXECUTEABLE_Bit  0x04   // Thread partially destroyed
#define THFLAG_NOT_EXECUTEABLE      (1L << THFLAG_NOT_EXECUTEABLE_Bit)
#define THFLAG_THREAD_CREATION_Bit  0x08   // Thread in status nascendi
#define THFLAG_THREAD_CREATION      (1L << THFLAG_THREAD_CREATION_Bit)
#define THFLAG_THREAD_BLOCKED_Bit   0x0A   // Blocked on semaphore
#define THFLAG_THREAD_BLOCKED       (1L << THFLAG_THREAD_BLOCKED_Bit)
#define THFLAG_CHARSET_Bits         0x10   // Default character set
#define THFLAG_CHARSET_MASK         (3L << THFLAG_CHARSET_Bits)
#define THFLAG_ANSI                 (0L << THFLAG_CHARSET_Bits)
#define THFLAG_OEM                  (1L << THFLAG_CHARSET_Bits)
#define THFLAG_UNICODE              (2L << THFLAG_CHARSET_Bits)
#define THFLAG_RESERVED             (3L << THFLAG_CHARSET_Bits)
#define THFLAG_EXTENDED_HANDLES_Bit 0x12   // Thread uses extended file handles
#define THFLAG_EXTENDED_HANDLES     (1L << THFLAG_EXTENDED_HANDLES_Bit)
//
// the win32 loader opens win32 exes with this bit set to notify IFS
// so a defragger won't move these files
// the bit is turned off once the open completes.
// file open flags are overloaded which is why this is here
//
#define THFLAG_OPEN_AS_IMMOVABLE_FILE_Bit 0x13   // File thus opened not moved
#define THFLAG_OPEN_AS_IMMOVABLE_FILE     (1L << THFLAG_OPEN_AS_IMMOVABLE_FILE_Bit)
//
// Protected mode application control blocks
//
typedef struct tagPMCB
{
    DWORD PMCB_Flags;
    DWORD PMCB_Parent;
} PMCB, *PPMCB;
//
//  The reference data for fault error codes 1-5 (GSDVME_PRIVINST through
//  GSDVME_INVALFLT) is a pointer to the following fault information structure.
//
typedef struct tagVMFaultInfo
{
    DWORD VMFI_EIP;             // faulting EIP
    WORD  VMFI_CS;              // faulting CS
    WORD  VMFI_Ints;            // interrupts in service, if any
} VMFAULTINFO, *PVMFAULTINFO;

//----------------------------------------------------------------------------
//
// Device descriptor block structure
//
//----------------------------------------------------------------------------

#define DDK_Version 0x400

typedef struct tagVxD_Desc_Block
{
   DWORD  DDB_Next ;                // VMM reserved field
   WORD   DDB_SDK_Version  ;        // VMM reserved field
   WORD   DDB_Req_Device_Number ;   // Required device number
   BYTE   DDB_Dev_Major_Version ;   // Major device number
   BYTE   DDB_Dev_Minor_Version ;   // Minor device number
   WORD   DDB_Flags ;               // Flags for init calls complete
   BYTE   DDB_Name[ 8 ] ;           // Device name
   DWORD  DDB_Init_Order ;          // Initialization Order
   DWORD  DDB_Control_Proc ;        // Offset of control procedure
   DWORD  DDB_V86_API_Proc ;        // Offset of API procedure
   DWORD  DDB_PM_API_Proc ;         // Offset of API procedure
   DWORD  DDB_V86_API_CSIP ;        // CS:IP of API entry point
   DWORD  DDB_PM_API_CSIP ;         // CS:IP of API entry point
   DWORD  DDB_Reference_Data ;      // Reference data from real mode
   DWORD  DDB_Service_Table_Ptr ;   // Pointer to service table
   DWORD  DDB_Service_Table_Size ;  // Number of services
   DWORD  DDB_Win32_Service_Table;  // INIT <0> Pointer to Win32 services
   DWORD  DDB_Prev;                 // INIT <'Prev'> Ptr to prev 4.0 DDB
   DWORD  DDB_Reserved0;            // INIT <0> Reserved
   DWORD  DDB_Reserved1;            // INIT <'Rsv1'> Reserved
   DWORD  DDB_Reserved2;            // INIT <'Rsv2'> Reserved
   DWORD  DDB_Reserved3;            // INIT <'Rsv3'> Reserved
} DDB ;
//
// Flag values for DDB_Flags
//
#define DDB_SYS_CRIT_INIT_DONE_Bit  0
#define DDB_SYS_CRIT_INIT_DONE      (1 << DDB_SYS_CRIT_INIT_DONE_Bit)
#define DDB_DEVICE_INIT_DONE_Bit    1
#define DDB_DEVICE_INIT_DONE        (1 << DDB_DEVICE_INIT_DONE_Bit)
#define DDB_HAS_WIN32_SVCS_Bit      14
#define DDB_HAS_WIN32_SVCS          (1 << DDB_HAS_WIN32_SVCS_Bit)
#define DDB_DYNAMIC_VXD_Bit         15
#define DDB_DYNAMIC_VXD             (1 << DDB_DYNAMIC_VXD_Bit)
#define DDB_DEVICE_DYNALINKED_Bit   13
#define DDB_DEVICE_DYNALINKED       (1 << DDB_DEVICE_DYNALINKED_Bit)

//----------------------------------------------------------------------------
//
// Data structures for vmmInstallMultIOHandlers
//
//----------------------------------------------------------------------------

typedef UINT (CDECL* VXDIOPROC)( DWORD, UINT, UINT, PCRS_32, UINT ) ;

typedef struct tagVXDIOENTRY
{
   WORD       VxD_IO_Port ;
   VXDIOPROC  VxD_IO_Proc ;
} VXDIOENTRY, *PVXDIOENTRY ;

typedef struct tagVXDIOTABLE
{
   WORD        IOT_Ports ;
   VXDIOENTRY  IOT_Entries[] ;
} VXDIOTABLE, *PVXDIOTABLE ;
//
// The following are the definitions for the "type of I/O" parameter passed
// to a I/O trap routine.
//
#define BYTE_INPUT      0x000
#define BYTE_OUTPUT     0x004
#define WORD_INPUT      0x008
#define WORD_OUTPUT     0x00C
#define DWORD_INPUT     0x010
#define DWORD_OUTPUT    0x014
#define OUTPUT_BIT      2
#define OUTPUT          (1 << OUTPUT_BIT)
#define WORD_IO_BIT     3
#define WORD_IO         (1 << WORD_IO_BIT)
#define DWORD_IO_BIT    4
#define DWORD_IO        (1 << DWORD_IO_BIT)
#define STRING_IO_BIT   5
#define STRING_IO       (1 << STRING_IO_BIT)
#define REP_IO_BIT      6
#define REP_IO          (1 << REP_IO_BIT)
#define ADDR_32_IO_BIT  7
#define ADDR_32_IO      (1 << ADDR_32_IO_BIT)
#define REVERSE_IO_BIT  8
#define REVERSE_IO      (1 << REVERSE_IO_BIT)
#define IO_SEG_MASK     0x0FFFF0000     // Use this to get segment
#define IO_SEG_SHIFT    0x10            // Must shift right this many

//----------------------------------------------------------------------------
//
//  The following equates are for flags sent to the real mode
//  initialization portion of a device driver:
//
//----------------------------------------------------------------------------

#define DUPLICATE_DEVICE_ID_BIT     0   // loaded
#define DUPLICATE_DEVICE_ID         (1 << DUPLICATE_DEVICE_ID_BIT)
#define DUPLICATE_FROM_INT2F_BIT    1   // loaded from INT 2F list
#define DUPLICATE_FROM_INT2F        (1 << DUPLICATE_FROM_INT2F_BIT)
#define LOADING_FROM_INT2F_BIT      2   // in the INT 2F device list
#define LOADING_FROM_INT2F          (1 << LOADING_FROM_INT2F_BIT)

//----------------------------------------------------------------------------
//
//  The following equates are used to indicate the result of the real mode
//  initialization portion of a device driver:
//
//----------------------------------------------------------------------------

#define DEVICE_LOAD_OK          0   // load protected mode portion
#define ABORT_DEVICE_LOAD       1   // don't load protected mode portion
#define ABORT_WIN386_LOAD       2   // fatal-error: abort load of Win386
#define NO_FAIL_MESSAGE_BIT     15  // set bit to suppress error message
#define NO_FAIL_MESSAGE         (1 << NO_FAIL_MESSAGE_BIT)

//----------------------------------------------------------------------------
//
//  The following equates define the loader services available to the real-mode
//  initialization portion of a device driver:
//
//----------------------------------------------------------------------------

#define LDRSRV_GET_PROFILE_STRING       0   // search SYSTEM.INI for string
#define LDRSRV_GET_NEXT_PROFILE_STRING  1   // search for next string
#define LDRSRV_RESERVED                 2   // RESERVED
#define LDRSRV_GET_PROFILE_BOOLEAN      3   // search SYSTEM.INI for boolean
#define LDRSRV_GET_PROFILE_DECIMAL_INT  4   // search SYSTEM.INI for integer
#define LDRSRV_GET_PROFILE_HEX_INT      5   // search SYSTEM.INI for hex int
#define LDRSRV_COPY_EXTENDED_MEMORY     6   // allocate/init extended memory
#define LDRSRV_GET_MEMORY_INFO          7   // get info about machine memory

//----------------------------------------------------------------------------
//  Registry services for Real mode init time
//
//  The parameters for these are as defined in Windows.h for the
//  corresponding Win Reg API and should be on Stack. These are
//  C Callable except that the function no has to be in AX
//
//----------------------------------------------------------------------------

#define LDRSRV_RegOpenKey           0x100
#define LDRSRV_RegCreateKey         0x101
#define LDRSRV_RegCloseKey          0x102
#define LDRSRV_RegDeleteKey         0x103
#define LDRSRV_RegSetValue          0x104
#define LDRSRV_RegQueryValue        0x105
#define LDRSRV_RegEnumKey           0x106
#define LDRSRV_RegDeleteValue       0x107
#define LDRSRV_RegEnumValue         0x108
#define LDRSRV_RegQueryValueEx      0x109
#define LDRSRV_RegSetValueEx        0x10A
#define LDRSRV_RegFlushKey          0x10B
//
// For the Copy_Extended_Memory service, the following types of memory can be
// requested:
//
#define LDRSRV_COPY_INIT            1   // memory discarded after init
#define LDRSRV_COPY_LOCKED          2   // locked memory
#define LDRSRV_COPY_PAGEABLE        3   // pageable memory

//----------------------------------------------------------------------------
//
//   Object types supported by the vxd loader
//
//  Notes : Low bit of all CODE type objects should be set (VXDLDR uses this)
//           Also Init type objects should be added to the second part of the
//           list (which starts with ICODE_OBJ).
//
//----------------------------------------------------------------------------

#define RCODE_OBJ           -1
#define LCODE_OBJ           0x01
#define LDATA_OBJ           0x02
#define PCODE_OBJ           0x03
#define PDATA_OBJ           0x04
#define SCODE_OBJ           0x05
#define SDATA_OBJ           0x06
#define CODE16_OBJ          0x07
#define LMSG_OBJ            0x08
#define PMSG_OBJ            0x09
#define DBOC_OBJ            0x0B
#define DBOD_OBJ            0x0C
#define ICODE_OBJ           0x11
#define IDATA_OBJ           0x12
#define ICODE16_OBJ         0x13
#define IMSG_OBJ            0x14
typedef struct tagObjectLocation
{
    DWORD OL_LinearAddr ;
    DWORD OL_Size ;
    BYTE  OL_ObjType ;
} OBJECTLOC, *POBJECTLOC;
#define MAXOBJECTS  30
//
// Device_Location structure
//
typedef struct tagDevice_Location_List {
    DWORD      DLL_DDB ;
    BYTE       DLL_NumObjects ;
    POBJECTLOC DLL_ObjLocation[1];
} DEVLOCLIST, *PDEVLOCLIST;

//----------------------------------------------------------------------------
//
// Data structure for vmmAddInstanceItem
//
//----------------------------------------------------------------------------

typedef struct tagINSTDATASTRUC
{
   DWORD   dwInstLinkF ;      // reserved set to NULL
   DWORD   dwInstLinkB ;      // reserved set to NULL
   DWORD   dwInstLinAddr ;    // linear address of start of block
   DWORD   dwInstSize ;       // size of block in bytes
   DWORD   dwInstType ;       // type of block
} INSTDATASTRUC, *PINSTDATASTRUC ;

// values for dwInstType

#define INDOS_Field   0x0100
#define ALWAYS_Field  0x0200

//----------------------------------------------------------------------------
//
// Data structure for page fault
//
//----------------------------------------------------------------------------

typedef struct tagIPFDATA
{

   DWORD  IPF_LinAddr ;       // CR2 address of fault
   DWORD  IPF_MapPageNum ;    // Possible converted page # of fault
   DWORD  IPF_PTEEntry ;      // Contents of PTE that faulted
   DWORD  IPF_FaultingVM ;    // May not = Current VM (IPF_V86PgH set)
   DWORD  IPF_Flags ;         // Flags
} IPFDATA, *PIPFDATA ;
//
// Page fault flags.
//
#define IPF_PAGE_NOT_PRESENT            0x00000008
#define IPF_PAGE_DIR_NOT_PRESENT        0x00000001
#define IPF_PAGE_SWAP_ERROR             0x00000010
#define IPF_PROTECTED_MODE              0x00000080
#define IPF_REENTRANT_FAULT             0x00000020
#define IPF_V86_MODE                    0x00000100
#define IPF_V86_PAGE_NOT_PRESENT        0x00000002
#define IPF_V86_HIGH_PAGE_NOT_PRESENT   0x00000004
#define IPF_VIRTUAL_DEVICE_FAULT        0x00000040
//
// Flags for other page allocator calls
//
//  NOTE: HIGH 8 BITS (bits 24-31) are reserved
//
#define PAGEZEROINIT            0x00000001
#define PAGEUSEALIGN            0x00000002
#define PAGECONTIG              0x00000004
#define PAGEFIXED               0x00000008
#define PAGEDEBUGNULFAULT       0x00000010
#define PAGEZEROREINIT          0x00000020
#define PAGENOCOPY              0x00000040
#define PAGELOCKED              0x00000080
#define PAGELOCKEDIFDP          0x00000100
#define PAGESETV86PAGEABLE      0x00000200
#define PAGECLEARV86PAGEABLE    0x00000400
#define PAGESETV86INTSLOCKED    0x00000800
#define PAGECLEARV86INTSLOCKED  0x00001000
#define PAGEMARKPAGEOUT         0x00002000
#define PAGEPDPSETBASE          0x00004000
#define PAGEPDPCLEARBASE        0x00008000
#define PAGEDISCARD             0x00010000
#define PAGEPDPQUERYDIRTY       0x00020000
#define PAGEMAPFREEPHYSREG      0x00040000
#define PAGENOMOVE              0x10000000
#define PAGEMAPGLOBAL           0x40000000
#define PAGEMARKDIRTY           0x80000000
//
// Flags for _PhysIntoV86, _MapIntoV86, and _LinMapIntoV86
//
#define MAPV86_IGNOREWRAP       0x00000001
//
// Flag equates for _BuildDescriptorDWORDs
//
#define BDDEXPLICITDPL          0x00000001
//
// Flag equates for _Allocate_LDT_Selector
//
#define ALDTSPECSEL             0x00000001
//
// Flag equates for _MMGR_Toggle_HMA
//
#define MMGRHMAPHYSICAL         0x00000001
#define MMGRHMAENABLE           0x00000002
#define MMGRHMADISABLE          0x00000004
#define MMGRHMAQUERY            0x00000008
//
// Flags used by List_Create
//
#define LF_ASYNC_Bit            0
#define LF_ASYNC                (1 << LF_ASYNC_Bit)
#define LF_USE_HEAP_BIT         1
#define LF_USE_HEAP             (1 << LF_USE_HEAP_Bit)
#define LF_ALLOC_ERROR_BIT      2
#define LF_ALLOC_ERROR          (1 << LF_ALLOC_ERROR_Bit)
//
// Swappable lists must use the heap.
//
#define LF_SWAP                 (LF_USE_HEAP + (1 << 3))
//
// Flag bits for _Allocate_Global_V86_Data_Area
//
#define GVDAWordAlign           0x00000001
#define GVDADWordAlign          0x00000002
#define GVDAParaAlign           0x00000004
#define GVDAPageAlign           0x00000008
#define GVDAInstance            0x00000100
#define GVDAZeroInit            0x00000200
#define GVDAReclaim             0x00000400
#define GVDAInquire             0x00000800
#define GVDAHighSysCritOK       0x00001000
#define GVDAOptInstance         0x00002000
#define GVDAForceLow            0x00004000
//
// Flag bits for _Allocate_Temp_V86_Data_Area
//
#define TVDANeedTilInitComplete 0x00000001
//
// Flags for heap allocator calls
//
#define HeapZeroInit    0x0001
#define HeapZeroReInit  0x0002
#define HeapNoCopy      0x0004
#define HeapLockedIfDP  0x0100
#define HeapSwap        0x0200
#define HeapInit        0x0400
#define HeapClean       0x0800
//
// OpenFile, if called after init, must point EDI to a buffer of at least
// this size.
//
#define VMM_OPENFILE_BUF_SIZE   260
//
// GetMachineInfo flag bits.
//
#define GMIF_80486_Bit          0x10
#define GMIF_80486              (1 << GMIF_80486_Bit)
#define GMIF_PCXT_BIT           0x11
#define GMIF_PCXT               (1 << GMIF_PCXT_Bit)
#define GMIF_MCA_BIT            0x12
#define GMIF_MCA                (1 << GMIF_MCA_Bit)
#define GMIF_EISA_BIT           0x13
#define GMIF_EISA               (1 << GMIF_EISA_Bit)
#define GMIF_CPUID_BIT          0x14
#define GMIF_CPUID              (1 << GMIF_CPUID_Bit)
//
// Bits for the ECX return of Get_Next_Arena
//
#define GNA_HIDOSLINKED         0x0002 // High DOS arenas linked when WIN386 started
#define GNA_ISHIGHDOS           0x0004 // High DOS arenas do exist
//
// Error code values for the GetSetDetailedVMError service. PLEASE NOTE
// that all of these error code values need to have bits set in the high
// word. This is to prevent collisions with other VMDOSAPP standard errors.
// Also, the low word must be non-zero.
//
// First set of errors (high word = 0001) are intended to be used
// when a VM is CRASHED (VNE_Crashed or VNE_Nuked bit set on
// VM_Not_Executeable).
//
// PLEASE NOTE that each of these errors (high word == 0001) actually
// has two forms:
//
// 0001xxxxh
// 8001xxxxh
//
// The device which sets the error initially always sets the error with
// the high bit CLEAR. The system will then optionally set the high bit
// depending on the result of the attempt to "nicely" crash the VM. This
// bit allows the system to tell the user whether the crash is likely or
// unlikely to destabalize the system.
//
#define GSDVME_PRIVINST         0x00010001  // Privledged instruction
#define GSDVME_INVALINST        0x00010002  // Invalid instruction
#define GSDVME_INVALPGFLT       0x00010003  // Invalid page fault
#define GSDVME_INVALGPFLT       0x00010004  // Invalid GP fault
#define GSDVME_INVALFLT         0x00010005  // Unspecified invalid fault
#define GSDVME_USERNUKE         0x00010006  // User requested NUKE of VM
#define GSDVME_DEVNUKE          0x00010007  // Device specific problem
#define GSDVME_DEVNUKEHDWR      0x00010008  // Device specific problem:
                                            // invalid hardware fiddling
                                            // by VM (invalid I/O)
#define GSDVME_NUKENOMSG        0x00010009  // Supress standard messages:
                                            // SHELL_Message used for
                                            // custom msg.
#define GSDVME_OKNUKEMASK       0x80000000  // "Nice nuke" bit
//
// Second set of errors (high word = 0002) are intended to be used
// when a VM start up is failed (VNE_CreateFail, VNE_CrInitFail, or
// VNE_InitFail bit set on VM_Not_Executeable).
//
#define GSDVME_INSMEMV86        0x00020001  // base V86 mem    - V86MMGR
#define GSDVME_INSV86SPACE      0x00020002  // Kb Req too large - V86MMGR
#define GSDVME_INSMEMXMS        0x00020003  // XMS Kb Req      - V86MMGR
#define GSDVME_INSMEMEMS        0x00020004  // EMS Kb Req      - V86MMGR
#define GSDVME_INSMEMV86HI      0x00020005  // Hi DOS V86 mem   - DOSMGR,V86MMGR
#define GSDVME_INSMEMVID        0x00020006  // Base Video mem   - VDD
#define GSDVME_INSMEMVM         0x00020007  // Base VM mem     - VMM
                                            // CB, Inst Buffer
#define GSDVME_INSMEMDEV        0x00020008  // Couldn't alloc base VM
                                            // memory for device.
#define GSDVME_CRTNOMSG         0x00020009  // Supress standard messages:
                                            // SHELL_Message used for
                                            // custom msg.
//
// Flags for Close_VM service.
//
#define CVF_CONTINUE_EXEC_Bit           0
#define CVF_CONTINUE_EXEC               (1 << CVF_CONTINUE_EXEC_Bit)
//
// Flag equates for _CreateMutex
//
#define MUTEX_MUST_COMPLETE             1
#define MUTEX_NO_CLEANUP_THREAD_STATE   2
//
// Flag equates for _GetThreadTerminationStatus
//
#define THREAD_TERM_STATUS_CRASH_PEND   1
#define THREAD_TERM_STATUS_NUKE_PEND    2
#define THREAD_TERM_STATUS_SUSPEND_PEND 4
//
// Return values for _GetInstanceInfo
//
#define INSTINFO_NONE                   0       // no data instanced in range
#define INSTINFO_SOME                   1       // some data instanced in range
#define INSTINFO_ALL                    2       // all data instanced in range
//
// Get/Set real DOS PSP.
//
#define GSRDP_Set                       0x0001
#ifndef REG_SZ                          // define only if not there already
#define REG_SZ                          0x0001
#define REG_BINARY                      0x0003
#endif
#ifndef HKEY_LOCAL_MACHINE              // define only if not there already
#define HKEY_CLASSES_ROOT               0x80000000
#define HKEY_CURRENT_USER               0x80000001
#define HKEY_LOCAL_MACHINE              0x80000002
#define HKEY_USERS                      0x80000003
#define HKEY_PERFORMANCE_DATA           0x80000004
#define HKEY_CURRENT_CONFIG             0x80000005
#define HKEY_DYN_DATA                   0x80000006
#endif
//
// Return values for _GetPhysPageInfo
//
#define PHYSINFO_NONE   0       // no pages in the specified range exist
#define PHYSINFO_SOME   1       // some pages in the specified range exist
#define PHYSINFO_ALL    2       // all pages in the specified range exist
//
// TYPE definitions for _GetRegistryKey
//
#define REGTYPE_ENUM                0
#define REGTYPE_CLASS               1
#define REGTYPE_VXD                 2
//
// Flag definitions for _GetRegistryKey
//
#define REGKEY_OPEN                 0
#define REGKEY_CREATE_IFNOTEXIST    1
//
// Flag definitions for _Assert_Range
#define ASSERT_RANGE_NULL_BAD       0x00000000
#define ASSERT_RANGE_NULL_OK        0x00000001
#define ASSERT_RANGE_NO_DEBUG       0x80000000
#define ASSERT_RANGE_BITS           0x80000001
//
// Flag definitions for Get_Boot_Flags
//
#define BOOT_CLEAN                  0x00000001
#define BOOT_DOSCLEAN               0x00000002
#define BOOT_NETCLEAN               0x00000004
#define BOOT_INTERACTIVE            0x00000008
//
// Flag definitions for _Add/_Set/_EnumReclaimableItem
//
#define RS_RECLAIM                  0x00000001
#define RS_RESTORE                  0x00000002
#define RS_DOSARENA                 0x00000004
//
// Structure definition for _EnumReclaimableItem
//
typedef struct tagRECLAIMSTRUC
{
    DWORD   RS_Linear;                  // low (< 1meg) address of item
    DWORD   RS_Bytes;                   // size of item in bytes
    DWORD   RS_CallBack;                // callback, if any (zero if none)
    DWORD   RS_RefData;                 // reference data for callback, if any
    DWORD   RS_HookTable;               // real-mode hook table (zero if none)
    DWORD   RS_Flags;                   // 0 or more of the RS_* equates
} RECLAIMSTRUC, *PRECLAIMSTRUC;
//
// Structures for Force_Mutexes_Free/Restore_Forced_Mutexes
//
typedef struct tagFRMTX
{
    struct tagFRMTX *frmtx_pfrmtxNext;
    DWORD frmtx_hmutex;
    DWORD frmtx_cEnterCount;
    DWORD frmtx_pthcbOwner;
    DWORD frmtx_htimeout;
} FRMTX, *PFRMTX;
typedef struct tagVMMFRINFO
{
    FRMTX vmmfrinfo_frmtxDOS;
    FRMTX vmmfrinfo_frmtxV86;
    FRMTX vmmfrinfo_frmtxOther;
} VMMFRINFO, *PVMMFRINFO;
//
//  Data structure for _GetDemandPageInfo
//
struct tagDemandInfoStruc
{
    DWORD DILin_Total_Count;    // # pages in linear address space
    DWORD DIPhys_Count;         // Count of phys pages
    DWORD DIFree_Count;         // Count of free phys pages
    DWORD DIUnlock_Count;       // Count of unlocked Phys Pages
    DWORD DILinear_Base_Addr;   // Base of pageable address space
    DWORD DILin_Total_Free;     // Total Count of free linear pages
    //
    //  The following 5 fields are all running totals, kept from the time
    //  the system was started
    //
    DWORD DIPage_Faults;        // total page faults
    DWORD DIPage_Ins;           // calls to pagers to page in a page
    DWORD DIPage_Outs;          // calls to pagers to page out a pag
    DWORD DIPage_Discards;      // pages discarded w/o calling pager
    DWORD DIInstance_Faults;    // instance page faults
    DWORD DIPagingFileMax;      // maximum # of pages that could be in paging file
    DWORD DIPagingFileInUse;    // # of pages of paging file currently in use
    DWORD DICommit_Count;       // Total committed memory, in pages
    DWORD DIReserved[2];        // Reserved for expansion
};
//
//  Data structure for _AddInstanceItem
//
struct tagInstDataStruc
{
    DWORD InstLinkF;        // INIT <0> RESERVED
    DWORD InstLinkB;        // INIT <0> RESERVED
    DWORD InstLinAddr;      // Linear address of start of block
    DWORD InstSize;         // Size of block in bytes
    DWORD InstType;         // Type of block
};
//
//  Values for InstType
//
#define INDOS_FIELD     0x100   // Bit indicating INDOS switch requirements
#define ALWAYS_FIELD    0x200   // Bit indicating ALWAYS switch requirements
#define OPTIONAL_FIELD  0x400   // Bit indicating optional instancing requirements
//
// Data structure for Hook_Invalid_Page_Fault handlers.
//
// This is the structure of the "invalid page fault information"
// which is pointed to by EDI when Invalid page fault hookers
// are called.
//
// Page faults can occur on a VM which is not current by touching the VM at
// its high linear address.  In this case, IPF_FaultingVM may not be the
// current VM, it will be set to the VM whos high linear address was touched.
//
struct tagIPF_Data
{
    DWORD IPF_LinAddr;      // CR2 address of fault
    DWORD IPF_MapPageNum;   // Possible converted page # of fault
    DWORD IPF_PTEEntry;     // Contents of PTE that faulted
    DWORD IPF_FaultingVM;   // May not = Current VM (IPF_V86PgH set)
    DWORD IPF_Flags;        // Flags
};
//
//
// Install_Exception_Handler data structure
//
//
struct tagException_Handler_Struc
{
    DWORD EH_Reserved;
    DWORD EH_Start_EIP;
    DWORD EH_End_EIP;
    DWORD EH_Handler;
};
//
// Flags passed in new memory manager functions
//
// PageReserve arena values
//
#define PR_PRIVATE      0x80000400  // anywhere in private arena
#define PR_SHARED       0x80060000  // anywhere in shared arena
#define PR_SYSTEM       0x80080000  // anywhere in system arena
//
// PageReserve flags
//
#define PR_FIXED        0x00000008  // don't move during PageReAllocate
#define PR_4MEG         0x00000001  // allocate on 4mb boundary
#define PR_STATIC       0x00000010  // see PageReserve documentation
//
// PageCommit default pager handle values
//
#define PD_ZEROINIT     0x00000001  // swappable zero-initialized pages
#define PD_NOINIT       0x00000002  // swappable uninitialized pages
#define PD_FIXEDZERO    0x00000003  // fixed zero-initialized pages
#define PD_FIXED        0x00000004  // fixed uninitialized pages
//
// PageCommit flags
//
#define PC_FIXED        0x00000008  // pages are permanently locked
#define PC_LOCKED       0x00000080  // pages are made present and locked
#define PC_LOCKEDIFDP   0x00000100  // pages are locked if swap via DOS
#define PC_WRITEABLE    0x00020000  // make the pages writeable
#define PC_USER         0x00040000  // make the pages ring 3 accessible
#define PC_INCR         0x40000000  // increment "pagerdata" each page
#define PC_PRESENT      0x80000000  // make pages initially present
#define PC_STATIC       0x20000000  // allow commit in PR_STATIC object
#define PC_DIRTY        0x08000000  // make pages initially dirty
//
// PageCommitContig additional flags
//
#define PCC_ZEROINIT    0x00000001  // zero-initialize new pages
#define PCC_NOLIN       0x10000000  // don't map to any linear address
//
// Structure and flags for PageQuery
//
#ifndef _WINNT_
typedef struct _MEMORY_BASIC_INFORMATION
{
    DWORD mbi_BaseAddress;
    DWORD mbi_AllocationBase;
    DWORD mbi_AllocationProtect;
    DWORD mbi_RegionSize;
    DWORD mbi_State;
    DWORD mbi_Protect;
    DWORD mbi_Type;
} MEMORY_BASIC_INFORMATION, *PMEMORY_BASIC_INFORMATION;
#define PAGE_NOACCESS           0x00000001
#define PAGE_READONLY           0x00000002
#define PAGE_READWRITE          0x00000004
#define MEM_COMMIT              0x00001000
#define MEM_RESERVE             0x00002000
#define MEM_FREE                0x00010000
#define MEM_PRIVATE             0x00020000
#endif

//----------------------------------------------------------------------------
//
//                            Pager Descriptor
//
// A PD describes a set of routines to call to bring a page into
// the system or to get it out.  Each committed page in the system
// has an associated PD, a handle to which is stored in the page's
// VP.
//
// For any field that is 0, the pager will not be notified
// when that action takes place.
//
// For the purpose of pagers, a page can be in one of the two states
// describing its current contents:
//
//     clean - page has not been written to since its last page out
//     dirty - page has been written to since its last page out
//
// A page also is in one of two persistent states:
//
//     virgin - page has never been written to since it was committed
//     tainted - page has been written to since it was committed
//
// Note that a tainted page may be either dirty or clean, but a
// virgin page is by definition clean.
//
// Examples of PDs:
//
//     For 32-bit EXE code or read-only data:
//
//       pd_virginin = routine to load page from an exe file
//       pd_taintedin = 0
//       pd_cleanout = 0
//       pd_dirtyout = 0
//       pd_virginfree = 0
//       pd_taintedfree = 0
//       pd_dirty = 0
//       pd_type = PD_PAGERONLY
//
//     For 32-bit EXE writeable data:
//
//       pd_virginin = routine to load page from an exe file
//       pd_taintedin = routine to load page from swap file
//       pd_cleanout = 0
//       pd_dirtyout = routine to write a page out to the swap file
//       pd_virginfree = 0
//       pd_taintedfree = routine to free page from the swap file
//       pd_dirty = routine to free page from the swap file
//       pd_type = PD_SWAPPER
//
//     For zero-initialized swappable data:
//
//       pd_virginin = routine to zero-fill a page
//       pd_taintedin = routine to load page from swap file
//       pd_cleanout = 0
//       pd_dirtyout = routine to write a page out to the swap file
//       pd_virginfree = 0
//       pd_taintedfree = routine to free page from the swap file
//       pd_dirty = routine to free page from the swap file
//       pd_type = PD_SWAPPER
//
//----------------------------------------------------------------------------

typedef DWORD _cdecl FUNPAGE(DWORD * ppagerdata, VOID * ppage, DWORD faultpage);
typedef FUNPAGE * PFUNPAGE;
typedef struct tagPD
{
    //
    // The following four fields are entry points in the pager which
    // we call to page in or page out a page.  The following parameters
    // are passed to the pager during these calls:
    //
    // ppagerdata - pointer to the pager-specific dword of data
    //          stored with the virtual page.  The pager is
    //          free to modify the contents of this dword
    //          DURING the page in or out, but not afterwards.
    //
    // ppage - pointer to page going in or out (a ring 0 alias
    //     to the physical page).  The pager should use this
    //     address to access the contents of the page.
    //
    // faultpage - faulting linear page number for page-ins, -1 for
    //         page-outs.  This address should not be accessed
    //         by the pager.  It is provided for information
    //         only.  Note that a single page can be mapped at
    //         more than one linear address because of the
    //         MapIntoV86 and LinMapIntoV86 services.
    //
    // The pager should return non-0 if the page was successfully
    // paged, or 0 if it failed.
    //
    PFUNPAGE pd_virginin;   // in - while page has never been written to
    PFUNPAGE pd_taintedin;  // in - page written to at least once
    PFUNPAGE pd_cleanout;   // out - page not written to since last out
    PFUNPAGE pd_dirtyout;   // out - page was written to since last out
    //
    // The pd_*free routines are used to inform the pager when the last
    // reference to a virtual page controlled by the pager is
    // decommitted.  A common use of this notification is to
    // free space in a backing file, or write the page contents
    // into the backing file.
    //
    // These calls take the same parameters as the page-out and -in
    // functions, but no return value is recognized.  The "ppage"
    // and "faultpage" parameters will always be 0.
    //
    PFUNPAGE pd_virginfree;  // decommit of never-written-to page
    PFUNPAGE pd_taintedfree; // decommit of page written to at least once
    //
    // The pd_dirty routine is used to inform the pager when the
    // memory manager detects that a page has been written to.  The memory
    // manager does not detect the write at the instant it occurs, so
    // the pager should not depend upon prompt notification.  A common
    // use of this notification might be to invalidate cached data.
    // If the page was dirtied in more than one memory context,
    // the pager's pd_dirty routine will be called once for each
    // context.
    //
    // These calls take the same parameters as the page-out and -in
    // functions except that the "ppage" parameter isn't valid and
    // no return value is recognized.
    //
    PFUNPAGE pd_dirty;
    //
    // The pd_type field gives the sytem information about the
    // overcommit characteristics of pages controlled by this pager.
    // The following are allowable values for the field:
    //
    // PD_SWAPPER - under some conditions, pages of this type
    //      may be paged out into the swap file
    // PD_PAGERONLY - pages controlled by this pager will never
    //      be paged out to the swap file
    //
    // In addition, the following value may be or'ed in to the pd_type field:
    //
    // PD_NESTEXEC - must be specified if either the pd_cleanout or pd_dirtyout
    //      functions perform nested excecution or block using the
    //      BLOCK_SVC_INTS flag.  To be safe, this flag should always be
    //      specified if the pager does any sort of file i/o to anything
    //      other than the default paging file.
    //
    DWORD pd_type;
} PD, * PPD;
//
// values for pd_type
//
#define PD_SWAPPER          0   // pages need direct accounting in swap file
#define PD_PAGERONLY        1   // pages will never be swapped
#define PD_NESTEXEC         2   // page out funtion uses nested execution
//
// The size of a page of memory
//
#define PAGESHIFT           12
#define PAGESIZE            (1 << PAGESHIFT)
#define PAGEMASK            (PAGESIZE - 1)
#define PAGE(p)             ((DWORD)(p) >> PAGESHIFT)
#define NPAGES(cb)          (((DWORD)(cb) + PAGEMASK) >> PAGESHIFT)
//
// Address space (arena) boundaries
//
#define MAXSYSTEMLADDR      0xFFBFFFFF    // 4 GB - 4MB
#define MINSYSTEMLADDR      0xC0000000    // 3 GB
#define MAXSHAREDLADDR      0xBFFFFFFF
#define MINSHAREDLADDR      0x80000000    // 2 GB
#define MAXPRIVATELADDR     0x7FFFFFF
#define MINPRIVATELADDR     0x00400000    // 4 MB
#define MAXDOSLADDR         0x003FFFFF
#define MINDOSLADDR         0x00000000
#define MAXSYSTEMPAGE       (MAXSYSTEMLADDR  >> PAGESHIFT)
#define MINSYSTEMPAGE       (MINSYSTEMLADDR  >> PAGESHIFT)
#define MAXSHAREDPAGE       (MAXSHAREDLADDR  >> PAGESHIFT)
#define MINSHAREDPAGE       (MINSHAREDLADDR  >> PAGESHIFT)
#define MAXPRIVATEPAGE      (MAXPRIVATELADDR >> PAGESHIFT)
#define MINPRIVATEPAGE      (MINPRIVATELADDR >> PAGESHIFT)
#define MAXDOSPAGE          (MAXDOSLADDR     >> PAGESHIFT)
#define MINDOSPAGE          (MINDOSLADDR     >> PAGESHIFT)
#define CBPRIVATE           (1 + MAXPRIVATELADDR - MINPRIVATELADDR)
#define CBSHARED            (1 + MAXSHAREDLADDR - MINSHAREDLADDR)
#define CBSYSTEM            (1 + MAXSYSTEMLADDR - MINSYSTEMLADDR)
#define CBDOS               (1 + MAXDOSLADDR - MINDOSLADDR)
#define CPGPRIVATE          (1 + MAXPRIVATEPAGE - MINPRIVATEPAGE)
#define CPGSHARED           (1 + MAXSHAREDPAGE - MINSHAREDPAGE)
#define CPGSYSTEM           (1 + MAXSYSTEMPAGE - MINSYSTEMPAGE)
#define CPGDOS              (1 + MAXDOSPAGE - MINDOSPAGE)
//
// Largest object that could theoretically be allocated
//
#define CBMAXALLOC          (max(CBSHARED,max(CBPRIVATE, CBSYSTEM)))
#define CPGMAXALLOC         (max(CPGSHARED,max(CPGPRIVATE, CPGSYSTEM)))

//----------------------------------------------------------------------------
//
//  EQUATES FOR SYSTEM_CONTROL CALLS
//
//----------------------------------------------------------------------------

//
// SYS_CRITICAL_INIT is a device init call.  Devices that have a
// critical function that needs initializing before interrupts are
// enabled should do it at Sys_Critical_Init.  Devices which REQUIRE a
// certain range of V86 pages to operate (such as the VDD video memory)
// should claim them at Sys_Critical_Init.  SYS VM Simulate_Int,
// Exec_Int ACTIVITY IS NOT ALLOWED.  Returning carry aborts device
// load only.
//
#define SYS_CRITICAL_INIT       0x0000      // Devices req'd for virt mode
//
// DEVICE_INIT is where most devices do the bulk of their initialization.
// SYS VM Simulate_Int, Exec_Int activity is allowed. Returning carry
// aborts device load only.
//
#define DEVICE_INIT             0x0001      // All other devices init
//
// INIT_COMPLETE is the final phase of device init called just before the
// WIN386 INIT pages are released and the Instance snapshot is taken.
// Devices which wish to search for a region of V86 pages >= A0h to use
// should do it at INIT_COMPLETE.
// SYS VM Simulate_Int, Exec_Int activity is allowed.  Returning carry
// aborts device load only.
//
#define INIT_COMPLETE           0x0002      // All devices initialized
//
// --------------- INITIALIZATION CODE AND DATA DISCARDED ------------------
//
//
// Same as VM_Init, except for SYS VM.
//
#define SYS_VM_INIT             0x0003      // Execute the system VM
//
// Same as VM_Terminate, except for SYS VM (Normal WIN386 exit ONLY, on a crash
// exit this call is not made).  SYS VM Simulate_Int, Exec_Int activity is
// allowed.  This and Sys_VM_Terminate2 are your last chances to access
// and/or lock pageable data.
//
#define SYS_VM_TERMINATE        0x0004      // System VM terminated
//
// System_Exit call is made when WIN386 is exiting either normally or via
// a crash.  INTERRUPTS ARE ENABLED.  Instance snapshot has been restored.
// SYS VM Simulate_Int, Exec_Int ACTIVITY IS NOT ALLOWED.
//
#define SYSTEM_EXIT             0x0005      // Devices prepare to exit
//
// SYS_CRITICAL_EXIT call is made when WIN386 is exiting either normally or via
// a crash.  INTERRUPTS ARE DISABLED.  SYS VM Simulate_Int, Exec_Int ACTIVITY
//  IS NOT ALLOWED.
//
#define SYS_CRITICAL_EXIT       0x0006      // System critical devices reset
//
// Create_VM creates a new VM.  EBX = VM handle of new VM.  Returning
// Carry will fail the Create_VM.
//
#define CREATE_VM               0x0007
//
// Second phase of Create_VM.  EBX = VM handle of new VM.  Returning
// Carry will cause the VM to go Not_Executeable, then be destroyed.
// VM Simulate_Int, Exec_Int activity is NOT allowed.
//
#define VM_CRITICAL_INIT        0x0008
//
// Third phase of Create_VM.  EBX = VM handle of new VM.  Returning
// Carry will cause the VM to go Not_Executeable, then be destroyed.
// VM Simulate_Int, Exec_Int activity is allowed.
//
#define VM_INIT                 0x0009
//
// NORMAL (First phase) of Destroy_VM.  EBX = VM Hanlde.  This occurs
// on normal termination of the VM.  Call cannot be failed.  VM
// Simulate_Int, Exec_Int activity is allowed.
//
#define VM_TERMINATE            0x000A      // Still in VM -- About to die
//
// Second phase of Destroy_VM.  EBX = VM Handle, EDX = Flags (see
// below).  Note that in the case of destroying a running VM, this is
// the first call made (VM_Terminate call does not occur).  Call cannot
// be failed.  VM Simulate_Int, Exec_Int activity is NOT allowed.
//
#define VM_NOT_EXECUTEABLE      0x000B      // Most devices die (except VDD)
//
// Final phase of Destroy_VM.  EBX = VM Handle.  Note that considerable
// time can elaps between the VM_Not_Executeable call and this call.
// Call cannot be failed.  VM Simulate_Int, Exec_Int activity is NOT
// allowed.
//
#define DESTROY_VM              0x000C      // VM's control block about to go
//
// Flags for VM_Not_Executeable control call (passed in EDX)
//
#define VNE_CRASHED_Bit         0x00        // VM was crashed
#define VNE_CRASHED             (1 << VNE_CRASHED_Bit)
#define VNE_NUKED_Bit           0x01        // VM was destroyed while active
#define VNE_NUKED               (1 << VNE_NUKED_Bit)
#define VNE_CREATEFAIL_Bit      0x02        // Some device failed Create_VM
#define VNE_CREATEFAIL          (1 << VNE_CREATEFAIL_Bit)
#define VNE_CRINITFAIL_Bit      0x03        // Some device failed VM_Critical_Init
#define VNE_CRINITFAIL          (1 << VNE_CRINITFAIL_Bit)
#define VNE_INITFAIL_Bit        0x04        // Some device failed VM_Init
#define VNE_INITFAIL            (1 << VNE_INITFAIL_Bit)
#define VNE_CLOSED_Bit          0x05
#define VNE_CLOSED              (1 << VNE_CLOSED_Bit)
//
// EBX = VM Handle. Call cannot be failed.
//
#define VM_SUSPEND              0x000D      // VM not runnable until resume
//
// EBX = VM Handle. Returning carry fails and backs out the resume.
//
#define VM_RESUME               0x000E      // VM is leaving suspended state
//
// EBX = VM Handle to set device focus to.  EDX = Device ID if device
// specific setfocus, == 0 if device critical setfocus (all devices).
// THIS CALL CANNOT BE FAILED.
//
// NOTE: In case where EDX == 0, ESI is a FLAG word that indicates
// special functions.  Currently Bit 0 being set indicates that this
// Device critical set focus is also "VM critical".  It means that we
// do not want some other VM to take the focus from this app now.  This
// is primarily used when doing a device critical set focus to Windows
// (the SYS VM) it is interpreted by the SHELL to mean "if an old app
// currently has the Windows activation, set the activation to the
// Windows Shell, not back to the old app".  ALSO in the case where Bit
// 0 is set, EDI = The VM handle of the VM that is "having trouble".
// Set this to 0 if there is no specific VM associated with the
// problem.
//
#define SET_DEVICE_FOCUS        0x000F
//
// EBX = VM Handle going into message mode.  THIS CALL CANNOT BE FAILED.
//
#define BEGIN_MESSAGE_MODE      0x0010
//
// EBX = VM Handle leaving message mode.  THIS CALL CANNOT BE FAILED.
//
#define END_MESSAGE_MODE        0x0011
//
// ----------------------- SPECIAL CONTROL CALLS ---------------------------
//
//
// Request for reboot.  Call cannot be failed.
//
#define REBOOT_PROCESSOR        0x0012
//
// Query_Destroy is an information call made by the SHELL device before
// an attempt is made to initiate a destroy VM sequence on a running VM
// which has not exited normally.  EBX = VM Handle.  Returning carry
// indicates that a device "has a problem" with allowing this.  THE
// DESTROY SEQUENCE CANNOT BE ABORTED HOWEVER, this decision is up to
// the user.  All this does is indicate that there is a "problem" with
// allowing the destroy.  The device which returns carry should call
// the SHELL_Message service to post an informational dialog about the
// reason for the problem.
//
#define QUERY_DESTROY           0x0013
//
// ----------------------- DEBUGGING CONTROL CALL --------------------------
//
// Special call for device specific DEBUG information display and activity.
//
#define DEBUG_QUERY             0x0014
//
// -------- CALLS FOR BEGIN/END OF PROTECTED MODE VM EXECUTION -------------
//
//  About to run a protected mode application.
//  EBX = Current VM handle.
//  EDX = Flags
//  EDI -> Application Control Block
//  Returning with carry set fails the call.
//
#define BEGIN_PM_APP            0x0015
//
// Flags for Begin_PM_App (passed in EDX)
//
#define BPA_32_BIT              0x01
#define BPA_32_BIT_FLAG         1
//
// Protected mode application is terminating.
// EBX = Current VM handle.  THIS CALL CAN NOT FAIL.
// EDI -> Application Control Block
//
#define END_PM_APP              0x0016
//
// Called whenever system is about to be rebooted.  Allows VxDs to clean
// up in preperation for reboot.
//
#define DEVICE_REBOOT_NOTIFY    0x0017
#define CRIT_REBOOT_NOTIFY      0x0018
//
// Called when VM is about to be terminated using the Close_VM service
// EBX = Current VM handle (Handle of VM to close)
// EDX = Flags
//       CVNF_CRIT_CLOSE = 1 if VM is in critical section while closing
//
#define CLOSE_VM_NOTIFY         0x0019
#define CVNF_CRIT_CLOSE_Bit     0
#define CVNF_CRIT_CLOSE         (1 << CVNF_CRIT_CLOSE_Bit)
//
// Power management event notification.
// EBX = 0
// ESI = event notification message
// EDI -> DWORD return value; VxD's modify the DWORD to return info, not EDI
// EDX is reserved
//
#define POWER_EVENT             0x001A
#define SYS_DYNAMIC_DEVICE_INIT 0x001B
#define SYS_DYNAMIC_DEVICE_EXIT 0x001C
//
// Create_THREAD creates a new thread.  EDI = handle of new thread.
// Returning Carry will fail the Create_THREAD. Message is sent in the
// context of the creating thread.
//
#define  CREATE_THREAD          0x001D
//
// Second phase of creating a thread.  EDI = handle of new thread.  Call cannot
// be failed. VM Simulate_Int, Exec_Int activity is not allowed (because
// never allowed in non-initial threads). Message is sent in the context
// of the newly created thread.
//
#define  THREAD_INIT            0x001E
//
// Normal (first) phase of Destroy_THREAD. EDI = handle of thread.
// This occurs on normal termination of the thread.  Call cannot be failed.
// Simulate_Int, Exec_Int activity is allowed.
//
#define  TERMINATE_THREAD       0x001F
//
// Second phase of Destroy_THREAD.  EDI = Handle of thread,
// EDX = flags (see below).  Note that in the case of destroying a
// running thread, this is the first call made (THREAD_Terminate call
// does not occur).  Call cannot be failed.  VM Simulate_Int, Exec_Int
// activity is NOT allowed.
//
#define  THREAD_Not_Executeable 0x0020
//
// Final phase of Destroy_THREAD.  EDI = Thread Handle.  Note that considerable
// time can elapse between the THREAD_Not_Executeable call and this call.
// Call cannot be failed.  VM Simulate_Int, Exec_Int activity is NOT
// allowed.
//
#define  DESTROY_THREAD         0x0021
//
// -------------------- CALLS FOR PLUG&PLAY -------------------------
//
// Configuration manager or a devloader is telling a DLVxD that a new devnode
// has been created. EBX is the handle of the new devnode and EDX is the load
// type (one of the DLVxD_LOAD_* defined in CONFIGMG.H). This is a 'C'
// system control call. Contrarily to the other calls, carry flags must be
// set if any error code other than CR_SUCCESS is to be return.
//
#define PNP_NEW_DEVNODE         0x0022
//
// -------------------- CALLS FOR Win32  -------------------------
//
// vWin32 communicates with Vxds on behalf of Win32 apps thru this mechanism.
// BUGBUG: need more doc here, describing the interface
//
#define W32_DEVICEIOCONTROL     0x0023
//
// sub-functions
//
#define DIOC_GETVERSION         0x0
#define DIOC_OPEN               DIOC_GETVERSION
#define DIOC_CLOSEHANDLE        -1
//
// -------------------- MORE SYSTEM CALLS -------------------------
//
// All these messages are sent immediately following the corresponding
// message of the same name, except that the "2" messages are sent
// in *reverse* init order.
//
#define SYS_VM_TERMINATE2       0x0024
#define SYSTEM_EXIT2            0x0025
#define SYS_CRITICAL_EXIT2      0x0026
#define VM_TERMINATE2           0x0027
#define VM_NOT_EXECUTEABLE2     0x0028
#define DESTROY_VM2             0x0029
#define VM_SUSPEND2             0x002A
#define END_MESSAGE_MODE2       0x002B
#define END_PM_APP2             0x002C
#define DEVICE_REBOOT_NOTIFY2   0x002D
#define CRIT_REBOOT_NOTIFY2     0x002E
#define CLOSE_VM_NOTIFY2        0x002F
//
// VCOMM gets Address of Contention handler from VxDs by sending this
// control message
//
#define GET_CONTENTION_HANDLER  0x0030
#define KERNEL32_INITIALIZED    0x0031
#define KERNEL32_SHUTDOWN       0x0032
#define MAX_SYSTEM_CONTROL      0x0032
//
// Dynamic VxD's can communicate with each other using Directed_Sys_Control
// and a private control message in the following range:
//
#define BEGIN_RESERVED_PRIVATE_SYSTEM_CONTROL   0x70000000
#define END_RESERVED_PRIVATE_SYSTEM_CONTROL     0x7FFFFFFF
//
// Values returned from VMM_GetSystemInitState in EAX.
//
// Comments represent operations performed by VMM; #define's indicate
// what VMM_GetSystemInitState will return if you call it between the
// previous operation and the next.
//
// Future versions of Windows may have additional init states between the
// ones defined here, so you should be careful to use range checks instead
// of test for equality.
//
// Protected mode is entered
//
#define SYSSTATE_PRESYSCRITINIT     0x00000000
//
// SYS_CRITICAL_INIT is broadcast
//
#define SYSSTATE_PREDEVICEINIT      0x10000000
//
// DEVICE_INIT is broadcast
//
#define SYSSTATE_PREINITCOMPLETE    0x20000000
//
// INIT_COMPLETE is broadcast
// VxD initialization complete
//
#define SYSSTATE_VXDINITCOMPLETED   0x40000000
//
// KERNEL32_INITIALIZED is broadcast
//
#define SYSSTATE_KERNEL32INITED     0x50000000
//
// All initialization completed
// System running normally
// System shutdown initiated
// KERNEL32_SHUTDOWN is broadcast
//
#define SYSSTATE_KERNEL32TERMINATED 0xA0000000
//
// System shutdown continues
//
#define SYSSTATE_PRESYSVMTERMINATE  0xB0000000
//
// SYS_VM_TERMINATE is broadcast
//
#define SYSSTATE_PRESYSTEMEXIT      0xE0000000
//
// SYSTEM_EXIT is broadcast
//
#define SYSSTATE_PRESYSTEMEXIT2     0xE4000000
//
// SYSTEM_EXIT2 is broadcast
//
#define SYSSTATE_PRESYSCRITEXIT     0xF0000000
//
// SYS_CRITICAL_EXIT is broadcast
//
#define SYSSTATE_PRESYSCRITEXIT2    0xF4000000
//
// SYS_CRITICAL_EXIT2 is broadcast
//
#define SYSSTATE_POSTSYSCRITEXIT2   0xFFF00000
//
// Return to real mode
// Alternate path: CAD reboot
//
#define SYSSTATE_PREDEVICEREBOOT    0xFFFF0000
//
// DEVICE_REBOOT_NOTIFY is broadcast
//
#define SYSSTATE_PRECRITREBOOT      0xFFFFF000
//
// CRIT_REBOOT_NOTIFY is broadcast
//
#define SYSSTATE_PREREBOOTCPU       0xFFFFFF00
//
// REBOOT_PROCESSOR is broadcast
// Return to real mode
//
//         S C H E D U L E R   B O O S T   V A L U E S
//
#define RESERVED_LOW_BOOST      0x00000001
#define CUR_RUN_VM_BOOST        0x00000004
#define LOW_PRI_DEVICE_BOOST    0x00000010
#define HIGH_PRI_DEVICE_BOOST   0x00001000
#define CRITICAL_SECTION_BOOST  0x00100000
#define TIME_CRITICAL_BOOST     0x00400000
#define RESERVED_HIGH_BOOST     0x40000000
//
//   F L A G S   F O R   C A L L _ P R I O R I T Y _ V M _ E V E N T
//
#define PEF_WAIT_FOR_STI_Bit            0
#define PEF_WAIT_FOR_STI                (1 << PEF_WAIT_FOR_STI_Bit)
#define PEF_WAIT_NOT_CRIT_Bit           1
#define PEF_WAIT_NOT_CRIT               (1 << PEF_WAIT_NOT_CRIT_Bit)
#define PEF_DONT_UNBOOST_Bit            2
#define PEF_DONT_UNBOOST                (1 << PEF_DONT_UNBOOST_Bit)
#define PEF_ALWAYS_SCHED_Bit            3
#define PEF_ALWAYS_SCHED                (1 << PEF_ALWAYS_SCHED_Bit)
#define PEF_TIME_OUT_Bit                4
#define PEF_TIME_OUT                    (1 << PEF_TIME_OUT_Bit)
#define PEF_WAIT_NOT_HW_INT_Bit         5
#define PEF_WAIT_NOT_HW_INT             (1 << PEF_WAIT_NOT_HW_INT_Bit)
#define PEF_WAIT_NOT_NESTED_EXEC_Bit    6
#define PEF_WAIT_NOT_NESTED_EXEC        (1 << PEF_WAIT_NOT_NESTED_EXEC_Bit)
#define PEF_WAIT_IN_PM_Bit              7
#define PEF_WAIT_IN_PM                  (1 << PEF_WAIT_IN_PM_Bit)
#define PEF_THREAD_EVENT_Bit            8
#define PEF_THREAD_EVENT                (1 << PEF_THREAD_EVENT_Bit)
#define PEF_WAIT_FOR_THREAD_STI_Bit     9
#define PEF_WAIT_FOR_THREAD_STI         (1 << PEF_WAIT_FOR_THREAD_STI_Bit)
#define PEF_RING0_EVENT_Bit             10
#define PEF_RING0_EVENT                 (1 << PEF_RING0_EVENT_Bit)
#define PEF_WAIT_CRIT_Bit               11
#define PEF_WAIT_CRIT                   (1 << PEF_WAIT_CRIT_Bit)
#define PEF_WAIT_CRIT_VM_Bit            12
#define PEF_WAIT_CRIT_VM                (1 << PEF_WAIT_CRIT_VM_Bit)
//
// synonyms for event restrictions above
//
#define PEF_WAIT_NOT_TIME_CRIT_Bit          PEF_WAIT_NOT_HW_INT_Bit
#define PEF_WAIT_NOT_TIME_CRIT              PEF_WAIT_NOT_HW_INT
#define PEF_WAIT_NOT_PM_LOCKED_STACK_Bit    PEF_WAIT_NOT_NESTED_EXEC_Bit
#define PEF_WAIT_NOT_PM_LOCKED_STACK        PEF_WAIT_NOT_NESTED_EXEC
//
// FLAGS FOR BEGIN_CRITICAL_SECTION,
// ENTER_MUTEX AND WAIT_SEMAPHORE
//
#define BLOCK_SVC_INTS_Bit              0
#define BLOCK_SVC_INTS                  (1 << BLOCK_SVC_INTS_Bit)
#define BLOCK_SVC_IF_INTS_LOCKED_Bit    1
#define BLOCK_SVC_IF_INTS_LOCKED        (1 << BLOCK_SVC_IF_INTS_LOCKED_Bit)
#define BLOCK_ENABLE_INTS_Bit           2
#define BLOCK_ENABLE_INTS               (1 << BLOCK_ENABLE_INTS_Bit)
#define BLOCK_POLL_Bit                  3
#define BLOCK_POLL                      (1 << BLOCK_POLL_Bit)
#define BLOCK_THREAD_IDLE_Bit           4
#define BLOCK_THREAD_IDLE               (1 << BLOCK_THREAD_IDLE_Bit)
#define BLOCK_FORCE_SVC_INTS_Bit        5
#define BLOCK_FORCE_SVC_INTS            (1 << BLOCK_FORCE_SVC_INTS_Bit)


//----------------------------------------------------------------------------
//
// Function parameter passing/return type defines.
//
//----------------------------------------------------------------------------

#define CARRY       void
#pragma aux VMM_C   "_*" parm caller []         \
                      value no8087 [EAX]        \
                      modify [EAX EBX ECX EDX];
#pragma aux VMM_CTL "*" parm [EAX] [EBX] [EDX]  \
                      modify [EAX EBX ECX EDX];
#pragma aux VMM_CB  "*" parm [EBX] [EDX]  \
                      modify [EAX EBX ECX EDX];
#pragma aux VMM_API "*" parm [EBX] [ECX]        \
                      modify [EAX EBX ECX EDX];

//----------------------------------------------------------------------------
//
// Function prototypes for VMM interface helper functions.
//
//----------------------------------------------------------------------------

#define vmmControlEntry()
CARRY   vmmControlExit(DWORD);
#define vmmCallBackEntry()
CARRY   vmmCallBackExit(DWORD);
VOID    vmmAPIEntry(VOID);
CARRY   vmmAPIExit(DWORD);

//----------------------------------------------------------------------------
//
// VMM interface inline helper functions.
//
//----------------------------------------------------------------------------

#pragma aux vmmControlExit =    \
            "stc"               \
            "or     EAX, EAX"   \
            "jnz    ctlExit"    \
            "clc"               \
            "ctlExit:"          \
            parm [EAX];

#pragma aux vmmCallBackExit =   \
            "stc"               \
            "or     EAX, EAX"   \
            "jnz    cbExit"     \
            "clc"               \
            "cbExit:"           \
            parm [EAX];

#pragma aux vmmAPIEntry =       \
            "mov    ECX, EBP";

#pragma aux vmmAPIExit =        \
            "stc"               \
            "or     EAX, EAX"   \
            "jnz    cbExit"     \
            "clc"               \
            "cbExit:"           \
            parm [EAX];

//----------------------------------------------------------------------------
//
// Function prototypes for VMM API.
//
//----------------------------------------------------------------------------
//
// Win 3.1 services.
//
VOID  vmmAdjustExecPriority(DWORD, DWORD);
VOID  vmmAdjustExecTime(DWORD, DWORD);
DWORD vmmAllocateGDTSelector(DWORD, DWORD, DWORD);
DWORD vmmAllocateLDTSelector(DWORD, DWORD, DWORD, DWORD, DWORD);
VOID  vmmBeginCriticalSection(DWORD);
VOID  vmmBeginNestExecution(VOID);
VOID  vmmBeginNestV86Exec(VOID);
DWORD vmmBeginReentrantExecution(VOID);
VOID  vmmBeginUseLockedPMStack(VOID);
QWORD vmmBuildDescriptorDWORDs(DWORD, DWORD, DWORD, DWORD, DWORD);
VOID  vmmBuildIntStackFrame(DWORD, DWORD);
DWORD vmmCallGlobalEvent(DWORD, DWORD);
DWORD vmmCallPriorityVMEvent(DWORD, DWORD, DWORD, DWORD, DWORD, DWORD);
DWORD vmmCallVMEvent(DWORD, DWORD, DWORD);
DWORD vmmCallWhenIdle(DWORD);
VOID  vmmCallWhenNotCritical(DWORD, DWORD);
VOID  vmmCallWhenVMIntsEnabled(DWORD, DWORD);
VOID  vmmCallWhenVMReturns(DWORD, DWORD, DWORD);
VOID  vmmCancelGlobalEvent(DWORD);
VOID  vmmCancelPriorityVMEvent(DWORD);
VOID  vmmCancelTimeOut(DWORD);
VOID  vmmCancelVMEvent(DWORD, DWORD);
VOID  vmmClaimCriticalSection(DWORD, DWORD);
DWORD vmmCopyPageTable(DWORD, DWORD, DWORD, DWORD);
DWORD vmmCreateSemaphore(DWORD);
VOID  vmmDestroySemaphore(DWORD);
VOID  vmmDisableGlobalTrapping(DWORD);
VOID  vmmDisableLocalTrapping(DWORD, DWORD);
VOID  vmmDisableVMInts(VOID);
VOID  vmmEnableGlobalTrapping(DWORD);
VOID  vmmEnableVMInts(VOID);
DWORD vmmEndCritAndSuspend(VOID);
DWORD vmmEndCriticalSection(VOID);
VOID  vmmEndNestExecution(VOID);
VOID  vmmEndReentrantExecution(DWORD);
VOID  vmmEndUseLockedPMStack(VOID);
VOID  vmmExecInt(DWORD);
VOID  vmmExecVxDInt(DWORD);
VOID  vmmFatalErrorHandler(DWORD, DWORD);
DWORD vmmFreeGDTSelector(DWORD, DWORD);
DWORD vmmFreeLDTSelector(DWORD, DWORD, DWORD);
DWORD vmmGetAppFlatDSAlias(VOID);
PSTR  vmmGetConfigDirectory(VOID);
DWORD vmmGetCritSectionStatus(VOID);
DWORD vmmGetCritStatusNoBlock(VOID);
DWORD vmmGetCurVMHandle();
QWORD vmmGetDescriptor(DWORD, DWORD, DWORD);
PSTR  vmmGetEnvironmentString(PSTR);
PSTR  vmmGetExecPath(VOID);
VOID  vmmGetFaultHookAddrs(DWORD, PDWORD);
QWORD vmmGetFreePageCount(DWORD);
DWORD vmmGetLastUpdatedSystemTime(VOID);
DWORD vmmGetLastUpdatedVMExecTime(VOID);
VOID  vmmGetMachineInfo(PDWORD);
PSTR  vmmGetNextProfileString(PSTR, PSTR);
DWORD vmmGetNulPageHandle(VOID);
DWORD vmmGetProfileBoolean(DWORD, PSTR, PSTR);
DWORD vmmGetProfileDecimalInt(DWORD, PSTR, PSTR);
DWORD vmmGetProfileHexInt(DWORD, PSTR, PSTR);
PSTR  vmmGetProfileString(PSTR, PSTR, PSTR);
DWORD vmmGetPSPSegment(VOID);
DWORD vmmGetSysPageCount(DWORD);
DWORD vmmGetSystemTime(VOID);
DWORD vmmGetSysVMHandle(VOID);
DWORD vmmGetVMExecTime(VOID);
DWORD vmmGetVMMReenterCount(VOID);
WORD  vmmGetVMMVersion(VOID);
QWORD vmmGetVMPgCount(DWORD, DWORD);
BYTE* vmmHeapAllocate(DWORD, DWORD);
DWORD vmmHeapFree(BYTE*, DWORD);
DWORD vmmHeapGetSize(BYTE*, DWORD);
BYTE* vmmHeapReAllocate(BYTE*, DWORD, DWORD);
DWORD vmmHookDevicePMAPI(DWORD, DWORD);
DWORD vmmHookDeviceService(DWORD, DWORD);
DWORD vmmHookDeviceV86API(DWORD, DWORD);
DWORD vmmHookInvalidPageFault(DWORD);
DWORD vmmHookPMFault(DWORD, DWORD);
DWORD vmmHookV86Fault(DWORD, DWORD);
VOID  vmmHookV86IntChain(DWORD, DWORD);
VOID  vmmHookV86Page(DWORD, DWORD);
DWORD vmmHookVMMFault(DWORD, DWORD);
VOID  vmmInstallIOHandler(DWORD, DWORD);
DWORD vmmInstallMultIOHandlers(DWORD*);
DWORD vmmLinMapIntoV86(DWORD, DWORD, DWORD, DWORD, DWORD);
DWORD vmmLinPageLock(DWORD, DWORD, DWORD);
DWORD vmmLinPageUnLock(DWORD, DWORD, DWORD);
DWORD vmmMapFlat(DWORD, DWORD);
DWORD vmmMapFreePhysReg(DWORD, DWORD, DWORD);
DWORD vmmMapIntoV86(DWORD, DWORD, DWORD, DWORD, DWORD, DWORD);
QWORD vmmMapLinToVMAddr(DWORD, DWORD);
DWORD vmmMapPhysToLinear(DWORD, DWORD, DWORD);
DWORD vmmModifyPageBits(DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD);
VOID  vmmNoFailResumeVM(DWORD);
DWORD vmmOpenFile(PSTR, PSTR);
VOID  vmmOutDebugString(PSTR);
VOID  vmmOutDebugStringValue(PSTR, DWORD);
QWORD vmmPageAllocate(DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD);
DWORD vmmPageCheckLinRange(DWORD, DWORD, DWORD);
DWORD vmmPageDiscardPages(DWORD, DWORD, DWORD, DWORD);
DWORD vmmPageFree(DWORD, DWORD);
QWORD vmmPageGetAllocInfo(DWORD);
QWORD vmmPageGetSizeAddr(DWORD, DWORD);
DWORD vmmPageLock(DWORD, DWORD, DWORD, DWORD);
QWORD vmmPageReAllocate(DWORD, DWORD, DWORD);
DWORD vmmPageResetHandlePAddr(DWORD, DWORD, DWORD, DWORD, DWORD);
DWORD vmmPageUnLock(DWORD, DWORD, DWORD, DWORD);
DWORD vmmPhysIntoV86(DWORD, DWORD, DWORD, DWORD, DWORD);
VOID  vmmReleaseCriticalSection(DWORD);
DWORD vmmRemoveExceptionHandler(DWORD);
VOID  vmmRestoreClientState(CRS*);
VOID  vmmResumeExec(VOID);
DWORD vmmResumeVM(DWORD);
VOID  vmmSaveClientState(CRS*);
DWORD vmmScheduleGlobalEvent(DWORD, DWORD);
DWORD vmmScheduleVMEvent(DWORD, DWORD, DWORD);
DWORD vmmSelectorMapFlat(DWORD, DWORD, DWORD);
DWORD vmmSetDescriptor(DWORD, DWORD, DWORD, DWORD, DWORD);
DWORD vmmSetFrePhysRegCalBk(DWORD, DWORD);
DWORD vmmSetGlobalTimeOut(DWORD, DWORD, DWORD);
DWORD vmmSetResetV86Pageable(DWORD, DWORD, DWORD, DWORD);
VOID  vmmSetV86IntVector(DWORD, WORD, DWORD);
DWORD vmmSetVMTimeOut(DWORD, DWORD, DWORD, DWORD);
VOID  vmmSignalSemaphore(DWORD);
VOID  vmmSimulateFarCall(DWORD, DWORD);
VOID  vmmSimulateFarJmp(DWORD, DWORD);
VOID  vmmSimulateFarRet(VOID);
VOID  vmmSimulateFarRetN(DWORD);
VOID  vmmSimulateFarInt(DWORD);
VOID  vmmSimulateIret(VOID);
DWORD vmmSimulatePop(VOID);
VOID  vmmSimulatePush(DWORD);
VOID  vmmTestReenter(VOID);
DWORD vmmTestSysVMHandle(DWORD);
DWORD vmmUnhookInvalidPageFault(DWORD);
DWORD vmmUnhookPMFault(DWORD, DWORD);
DWORD vmmUnhookV86Fault(DWORD, DWORD);
DWORD vmmUnhookVMMFault(DWORD, DWORD);
DWORD vmmUnmapFreePhysReg(DWORD, DWORD, DWORD);
VOID  vmmWaitSemaphore(DWORD, DWORD);
DWORD vmmXchgFreePhysReg(DWORD, DWORD, DWORD);
//
// Win 4.0 services.
//
DWORD vmmPageCommit(DWORD, DWORD, DWORD, DWORD, DWORD);
DWORD vmmPageCommitContig(DWORD, DWORD, DWORD, DWORD, DWORD, DWORD);
DWORD vmmPageCommitPhys(DWORD, DWORD, DWORD, DWORD);
DWORD vmmPageDecommit(DWORD, DWORD, DWORD);
DWORD vmmPageReserve(DWORD, DWORD, DWORD);
DWORD vmmAllocatePMCallBack(DWORD, DWORD);
DWORD vmmAllocateV86CallBack(DWORD, DWORD);
DWORD vmmInstallV86BreakPoint(DWORD, DWORD, DWORD);
DWORD vmmRemoveV86BreakPoint(DWORD);
VOID  vmmCancelThreadEvent(DWORD, DWORD);
DWORD vmmScheduleThreadEvent(DWORD, DWORD, DWORD);
DWORD vmmHookNMIEvent(DWORD);
VOID  vmmUnhookNMIEvent(DWORD);
DWORD vmmCallRestrictedEvent(DWORD, DWORD, DWORD, DWORD, DWORD, DWORD);
DWORD vmmCacelRestrictedEvent(DWORD);
DWORD vmmRegisterPEFProvider(DWORD, DWORD);
DWORD vmmGetNMIHandlerAddr(VOID);
VOID  vmmSetNMIHandlerAddr(DWORD);
DWORD vmmGetSystemInitState(VOID);
DWORD vmmRegisterWin32Services(DWORD, DWORD);
DWORD vmmContextCreate(VOID);
DWORD vmmContextDestroy(DWORD);
DWORD vmmContextSwitch(DWORD);
DWORD vmmGetCurrentContext(VOID);
DWORD vmmPageAttach(DWORD, DWORD, DWORD);
DWORD vmmPageQuery(DWORD, DWORD, DWORD);
DWORD vmmGetRegistryKey(DWORD, DWORD, DWORD, DWORD);
DWORD vmmRegCloseKey(DWORD);
DWORD vmmRegCreateDynKey(DWORD, DWORD, DWORD, DWORD, DWORD, DWORD);
DWORD vmmRegCreateKey(DWORD, DWORD, DWORD);
DWORD vmmRegDeleteKey(DWORD, DWORD);
DWORD vmmRegDeleteValue(DWORD, DWORD);
DWORD vmmRegEnumKey(DWORD, DWORD, DWORD, DWORD);
DWORD vmmRegEnumValue(DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD);
DWORD vmmRegFlushKey(DWORD);
DWORD vmmRegLoadKey(DWORD, DWORD, DWORD);
DWORD vmmRegOpenKey(DWORD, DWORD, DWORD);
DWORD vmmRegQueryInfoKey(DWORD, DWORD, DWORD, DWORD, DWORD, DWORD,
                         DWORD, DWORD, DWORD, DWORD, DWORD, DWORD);
DWORD vmmRegQueryMultipleValues(DWORD, DWORD, DWORD, DWORD, DWORD);
DWORD vmmRegQueryValue(DWORD, DWORD, DWORD, DWORD);
DWORD vmmRegQueryValueEx(DWORD, DWORD, DWORD, DWORD, DWORD, DWORD);
DWORD vmmRegRemapPreDefKey(DWORD, DWORD);
DWORD vmmRegSaveKey(DWORD, DWORD, DWORD, DWORD, DWORD);
DWORD vmmRegSetValue(DWORD, DWORD, DWORD, DWORD, DWORD);
DWORD vmmRegSetValueEx(DWORD, DWORD, DWORD, DWORD, DWORD, DWORD);
VOID  vmmAdjustThreadExecPriority(DWORD, DWORD);
VOID  vmmCallWhenTaskSwitched(DWORD);
VOID  vmmCallWhenThreadSwitched(DWORD);
DWORD vmmCancelCallWhenTaskSwitched(DWORD);
DWORD vmmCancelCallWhenThreadSwitched(DWORD);
DWORD vmmGetLastUdatedThreadExecTime(DWORD);
DWORD vmmGetThreadExecTime(DWORD);
DWORD vmmGetThreadTerminationStatus(DWORD);
DWORD vmmGetCurThreadHandle(VOID);
DWORD vmmGetInitialThreadHandle(DWORD);
DWORD vmmGetNextThreadHandle(VOID);
DWORD vmmGetSysThreadHandle(VOID);
VOID  vmmGetThreadWin32Pri(DWORD, DWORD *, DWORD *, DWORD *, DWORD *);
DWORD vmmGetVMHandleForThread(DWORD);
DWORD vmmAllocateThreadDataSlot(VOID);
VOID  vmmFreeThreadDataSlot(DWORD);
//
// Debug services.
//
VOID  dbgBreakPoint(VOID);

//----------------------------------------------------------------------------
//
// VMM API inline functions.
//
//----------------------------------------------------------------------------

#define CALL_VXD(v,s)       MK_VDDL(DW v, DW s)
#define MK_VDDL(serv, vd)   \
            "int    20h"    \
            #vd             \
            #serv

#define CALL_VMM(s)         MK_VMDL(DD s)
#define MK_VMDL(vs)         \
            "int    20h"    \
            #vs

#pragma aux vmmAdjustExecPriority =                 \
            CALL_VMM(Adjust_Exec_Priority)          \
            parm [EAX] [EBX];

#pragma aux vmmAdjustExecTime =                     \
            CALL_VMM(Adjust_Execution_Time)         \
            parm [EAX] [EBX];

#pragma aux vmmAllocateGDTSelector =                \
            CALL_VMM(_Allocate_GDT_Selector)        \
            parm caller []                          \
            value [EAX]                             \
            modify [EAX EBX ECX EDX];

#pragma aux vmmAllocateLDTSelector =                \
            CALL_VMM(_Allocate_LDT_Selector)        \
            parm caller []                          \
            value [EAX];

#pragma aux vmmBeginCriticalSection =               \
            CALL_VMM(Begin_Critical_Section)        \
            parm [ECX];

#pragma aux vmmBeginNestExecution =                 \
            CALL_VMM(Begin_Nest_Exec);

#pragma aux vmmBeginNestV86Exec =                   \
            CALL_VMM(Begin_Nest_V86_Exec);

#pragma aux vmmBeginReentrantExecution =            \
            CALL_VMM(Begin_Reentrant_Execution)     \
            value [ECX];

#pragma aux vmmBeginUseLockedPMStack =              \
            CALL_VMM(Begin_Use_Locked_PM_Stack);

#pragma aux vmmBuildDescriptorDWORDs =              \
            CALL_VMM(_BuildDescriptorDWORDs)        \
            parm caller []                          \
            value no8087 [EAX EDX]                  \
            modify [EAX EBX ECX EDX];

#pragma aux vmmBuildIntStackFrame =                 \
            CALL_VMM(Build_Int_Stack_Frame)         \
            parm [ECX] [EDX];

#pragma aux vmmCallGlobalEvent =                    \
            CALL_VMM(Call_Global_Event)             \
            parm [ESI] [EDX]                        \
            value [ESI];

#pragma aux vmmCallPriorityVMEvent =                \
            CALL_VMM(Call_Priority_VM_Event)        \
            parm [EAX] [EBX] [ECX] [EDX] [ESI] [EDI]\
            value [ESI];

#pragma aux vmmCallVMEvent =                        \
            CALL_VMM(Call_VM_Event)                 \
            parm [EBX] [ESI] [EDX]                  \
            value [ESI];

#pragma aux vmmCallWhenIdle =                       \
            CALL_VMM(Call_When_Idle)                \
            "sbb    EAX, EAX"                       \
            parm [ESI]                              \
            value [EAX];

#pragma aux vmmCallWhenNotCritical =                \
            CALL_VMM(Call_When_Not_Critical)        \
            parm [ESI] [EDX];


#pragma aux vmmCallWhenVMIntsEnabled =              \
            CALL_VMM(Call_When_VM_Ints_Enabled)     \
            parm [EDX] [ESI];

#pragma aux vmmCallWhenVMReturns =                  \
            CALL_VMM(Call_When_VM_Returns)          \
            parm [EAX] [EDX] [ESI];

#pragma aux vmmCancelGlobalEvent =                  \
            CALL_VMM(Cancel_Global_Event)           \
            parm [ESI];

#pragma aux vmmCancelPriorityVMEvent =              \
            CALL_VMM(Cancel_Priority_VM_Event)      \
            parm [ESI];

#pragma aux vmmCancelTimeOut =                      \
            CALL_VMM(Cancel_Time_Out)               \
            parm [ESI];

#pragma aux vmmCancelVMEvent =                      \
            CALL_VMM(Cancel_VM_Event)               \
            parm [EBX] [ESI];

#pragma aux vmmClaimCriticalSection =               \
            CALL_VMM(Claim_Critical_Section)        \
            parm [EAX] [ECX];

#pragma aux vmmCopyPageTable =                      \
            CALL_VMM(_CopyPageTable)                \
            parm caller []                          \
            value [EAX]                             \
            modify [EAX EBX ECX EDX];

#pragma aux vmmCreateSemaphore =                    \
            CALL_VMM(Create_Semaphore)              \
            parm [ECX]                              \
            value [EAX];

#pragma aux vmmDestroySemaphore =                   \
            CALL_VMM(Destroy_Semaphore)             \
            parm [EAX];

#pragma aux vmmDisableGlobalTrapping =              \
            CALL_VMM(Disable_Global_Trapping)       \
            parm [EDX];

#pragma aux vmmDisableLocalTrapping =               \
            CALL_VMM(Disable_Local_Trapping)        \
            parm [EBX] [EDX];

#pragma aux vmmDisableVMInts =                      \
            CALL_VMM(Disable_VM_Ints);

#pragma aux vmmEnableGlobalTrapping =               \
            CALL_VMM(Enable_Global_Trapping)        \
            parm [EDX];

#pragma aux vmmEnableLocalTrapping =                \
            CALL_VMM(Enable_Local_Trapping)         \
            parm [EBX] [EDX];

#pragma aux vmmEnableVMInts =                       \
            CALL_VMM(Enable_VM_Ints);

#pragma aux vmmEndCritAndSuspend =                  \
            CALL_VMM(End_Crit_And_Suspend)          \
            "sbb    EAX, EAX"                       \
            value [EAX];

#pragma aux vmmEndCriticalSection =                 \
            CALL_VMM(End_Critical_Section);

#pragma aux vmmEndNestExecution =                   \
            CALL_VMM(End_Nest_Exec);

#pragma aux vmmEndReentrantExecution =              \
            CALL_VMM(End_Reentrant_Execution)       \
            parm [ECX];

#pragma aux vmmEndUseLockedPMStack =                \
            CALL_VMM(End_Use_Locked_PM_Stack);

#pragma aux vmmExecInt =                            \
            CALL_VMM(Exec_Int)                      \
            parm [EAX];

#pragma aux vmmExecVxDInt =                         \
            CALL_VMM(Exec_VxD_Int)                  \
            parm routine [];

#pragma aux vmmFatalErrorHandler =                  \
            CALL_VMM(Fatal_Error_Handler)           \
            parm [ESI] [EAX];

#pragma aux vmmFreeGDTSelector =                    \
            CALL_VMM(_Free_GDT_Selector)            \
            parm caller []                          \
            modify [EAX EBX ECX EDX];

#pragma aux vmmFreeLDTSelector =                    \
            CALL_VMM(_Free_LDT_Selector)            \
            parm caller []                          \
            modify [EAX EBX ECX EDX];

#pragma aux vmmGetAppFlatDSAlias =                  \
            CALL_VMM(_GetAppFlatDSAlias)            \
            parm caller []                          \
            modify [EAX EBX ECX EDX];

#pragma aux vmmGetConfigDirectory =                 \
            CALL_VMM(Get_Config_Directory)          \
            value [EDX];

#pragma aux vmmGetCritSectionStatus =               \
            CALL_VMM(Get_Crit_Section_Status)       \
            value [ECX];

#pragma aux vmmGetCritStatusNoBlock =               \
            CALL_VMM(Get_Crit_Status_No_Block)      \
            value [ECX];

#pragma aux vmmGetCurVMHandle =                     \
            CALL_VMM(Get_Cur_VM_Handle)             \
            value [EBX];

#pragma aux vmmGetDescriptor =                      \
            CALL_VMM(_GetDescriptor)                \
            parm caller []                          \
            value no8087 [EAX EDX]                  \
            modify [EAX EBX ECX EDX];

#pragma aux vmmGetEnvironmentString =               \
            CALL_VMM(Get_Environment_String)        \
            parm [ESI]                              \
            value [EDX];

#pragma aux vmmGetExecPath =                        \
            CALL_VMM(Get_Exec_Path)                 \
            value [EDX];

#pragma aux vmmGetFaultHookAddrs =                  \
            "mov    EAX, [ESP]"                     \
            CALL_VMM(Get_Fault_Hook_Addrs)          \
            "mov    EBX, [ESP + 4]"                 \
            "mov    [EBX], EDI"                     \
            "mov    [EBX + 4], EDX"                 \
            "mov    [EBX + 8], ESI"                 \
            parm caller [];

#pragma aux vmmGetFreePageCount =                   \
            CALL_VMM(_GetFreePageCount)             \
            parm caller []                          \
            value no8087 [EAX EDX]                  \
            modify [EAX EBX ECX EDX];

#pragma aux vmmGetLastUpdatedSystemTime =           \
            CALL_VMM(Get_Last_Updated_System_Time)  \
            value [EAX];

#pragma aux vmmGetLastUpdatedVMExecTime =           \
            CALL_VMM(Get_Last_Updated_VM_Exec_Time) \
            value [EAX];

#pragma aux vmmGetMachineInfo =                     \
            CALL_VMM(Get_Machine_Info)              \
            "mov    EDI, [ESP]"                     \
            "mov    [EDI], EAX"                     \
            "mov    [EDI + 4], EBX"                 \
            "mov    [EDI + 8], ECX"                 \
            "mov    [EDI + 12], EDX"                \
            parm []                                 \
            modify [EAX EBX ECX EDX EDI];

#pragma aux vmmGetNextProfileString =               \
            CALL_VMM(Get_Next_Profile_String)       \
            "jnc    gnpsFound"                      \
            "xor    EDX, EDX"                       \
            "gnpsFound:"                            \
            parm [EDX] [EDI]                        \
            value [EDX];

#pragma aux vmmGetNulPageHandle =                   \
            CALL_VMM(_GetNulPageHandle)             \
            parm caller []                          \
            value [EAX]                             \
            modify [EAX EBX ECX EDX];

#pragma aux vmmGetProfileBoolean =                  \
            CALL_VMM(Get_Profile_Boolean)           \
            parm [EAX] [ESI] [EDI]                  \
            value [EAX];

#pragma aux vmmGetProfileDecimalInt =               \
            CALL_VMM(Get_Profile_Decimal_Int)       \
            parm [EAX] [ESI] [EDI]                  \
            value [EAX];

#pragma aux vmmGetProfileHexInt =                   \
            CALL_VMM(Get_Profile_Hex_Int)           \
            parm [EAX] [ESI] [EDI]                  \
            value [EAX];

#pragma aux vmmGetProfileString =                   \
            CALL_VMM(Get_Profile_String)            \
            parm [EDX] [ESI] [EDI]                  \
            value [EDX];

#pragma aux vmmGetPSPSegment =                      \
            CALL_VMM(Get_PSP_Segment)               \
            value [EAX];

#pragma aux vmmGetSysPageCount =                    \
            CALL_VMM(_GetSysPageCount)              \
            parm caller []                          \
            modify [EAX EBX ECX EDX];

#pragma aux vmmGetSystemTime =                      \
            CALL_VMM(Get_System_Time)               \
            value [EAX];

#pragma aux vmmGetSysVMHandle =                     \
            CALL_VMM(Get_Sys_VM_Handle)             \
            value [EBX];

#pragma aux vmmGetVMExecTime =                      \
            CALL_VMM(Get_VM_Exec_Time)              \
            value [EAX];

#pragma aux vmmGetVMMReenterCount =                 \
            CALL_VMM(Get_VMM_Reenter_Count)         \
            value [ECX];

#pragma aux vmmGetVMMVersion =                      \
            CALL_VMM(Get_VMM_Version)               \
            value [EAX];

#pragma aux vmmGetVMPgCount =                       \
            CALL_VMM(_GetVMPgCount)                 \
            parm caller []                          \
            modify [EAX EBX ECX EDX];

#pragma aux vmmHeapAllocate =                       \
            CALL_VMM(_HeapAllocate)                 \
            parm caller []                          \
            modify [EAX EBX ECX EDX];

#pragma aux vmmHeapFree =                           \
            CALL_VMM(_HeapFree)                     \
            parm caller []                          \
            modify [EAX EBX ECX EDX];

#pragma aux vmmHeapGetSize =                        \
            CALL_VMM(_HeapGetSize)                  \
            parm caller []                          \
            modify [EAX EBX ECX EDX];

#pragma aux vmmHeapReAllocate =                     \
            CALL_VMM(_HeapReAllocate)               \
            parm caller []                          \
            modify [EAX EBX ECX EDX];

#pragma aux vmmHookDevicePMAPI =                    \
            CALL_VMM(Hook_Device_PM_API)            \
            "jnc    hdpaFound"                      \
            "xor    ESI, ESI"                       \
            "hdpaFound:"                            \
            parm [EAX] [ESI]                        \
            value [ESI];

#pragma aux vmmHookDeviceService =                  \
            CALL_VMM(Hook_Device_Service)           \
            "jnc    hdsFound"                       \
            "xor    ESI, ESI"                       \
            "hdsFound:"                             \
            parm [EAX] [ESI]                        \
            value [ESI];

#pragma aux vmmHookDeviceV86API =                   \
            CALL_VMM(Hook_Device_V86_API)           \
            "jnc    hdvaFound"                      \
            "xor    ESI, ESI"                       \
            "hdvaFound:"                            \
            parm [EAX] [ESI]                        \
            value [ESI];

#pragma aux vmmHookPMFault =                        \
            CALL_VMM(Hook_PM_Fault)                 \
            parm [EAX] [ESI]                        \
            value [ESI];

#pragma aux vmmHookV86Fault =                       \
            CALL_VMM(Hook_V86_Fault)                \
            parm [EAX] [ESI]                        \
            value [ESI];

#pragma aux vmmHookV86IntChain =                    \
            CALL_VMM(Hook_V86_Int_Chain)            \
            parm [EAX] [ESI];

#pragma aux vmmHookV86Page =                        \
            CALL_VMM(Hook_V86_Page)                 \
            parm [EAX] [ESI];

#pragma aux vmmHookVMMFault =                       \
            CALL_VMM(Hook_VMM_Fault)                \
            parm [EAX] [ESI]                        \
            value [ESI];

#pragma aux vmmInstallIOHandler =                   \
            CALL_VMM(Install_IO_Handler)            \
            parm [ESI] [EDX];

#pragma aux vmmInstallMultIOHandlers =              \
            CALL_VMM(Install_Mult_IO_Handlers)      \
            parm [EDI]                              \
            value [EDX];

#pragma aux vmmLinMapIntoV86 =                      \
            CALL_VMM(_LinMapIntoV86)                \
            parm caller []                          \
            modify [EAX EBX ECX EDX];

#pragma aux vmmLinPageLock =                        \
            CALL_VMM(_LinPageLock)                  \
            parm caller []                          \
            modify [EAX EBX ECX EDX];

#pragma aux vmmLinPageUnLock =                      \
            CALL_VMM(_LinPageUnLock)                \
            parm caller []                          \
            modify [EAX EBX ECX EDX];

#pragma aux vmmMapFlat =                            \
            "mov    AH, AL"                         \
            "mov    AL, BL"                         \
            CALL_VMM(Map_Flat)                      \
            parm [EAX] [EBX]                        \
            value [EAX];

#pragma aux vmmMapFreePhysReg =                     \
            CALL_VMM(_MapFreePhysReg)               \
            parm caller []                          \
            modify [EAX EBX ECX EDX];

#pragma aux vmmMapIntoV86 =                         \
            CALL_VMM(_MapIntoV86)                   \
            parm caller []                          \
            modify [EAX EBX ECX EDX];

#pragma aux vmmMapLinToVMAddr =                     \
            CALL_VMM(Map_Lin_To_VM_Addr)            \
            parm [EAX ECX]                          \
            value no8087 [ECX EDX];


#pragma aux vmmMapPhysToLinear =                    \
            CALL_VMM(_MapPhysToLinear)              \
            parm caller []                          \
            modify [EAX EBX ECX EDX];

#pragma aux vmmModifyPageBits =                     \
            CALL_VMM(_ModifyPageBits)               \
            parm caller []                          \
            modify [EAX EBX ECX EDX];

#pragma aux vmmNoFailResumeVM =                     \
            CALL_VMM(No_Fail_Resume_VM)             \
            parm [EBX];

#pragma aux vmmOpenFile =                           \
            CALL_VMM(OpenFile)                      \
            "jnc    ofFound"                        \
            "xor    EAX, EAX"                       \
            "ofFound:"                              \
            parm [EDX] [EDI]                        \
            value [EAX];

#pragma aux vmmOutDebugString =                     \
            "pushfd"                                \
            "pushad"                                \
            CALL_VMM(Out_Debug_String)              \
            "popad"                                 \
            "popfd"                                 \
            parm [ESI];

#pragma aux vmmOutDebugStringValue =                \
            "pushfd"                                \
            "pushad"                                \
            CALL_VMM(Out_Debug_String)              \
            "popad"                                 \
            "popfd"                                 \
            parm [ESI] [EAX];

#pragma aux vmmPageAllocate =                       \
            CALL_VMM(_PageAllocate)                 \
            parm caller []                          \
            value no8087 [EAX EDX]                  \
            modify [EAX EBX ECX EDX];

#pragma aux vmmPageCheckLinRange =                  \
            CALL_VMM(_PageCheckLinRange)            \
            parm caller []                          \
            modify [EAX EBX ECX EDX];

#pragma aux vmmPageDiscardPages =                   \
            CALL_VMM(_PageDiscardPages)             \
            parm caller []                          \
            modify [EAX EBX ECX EDX];

#pragma aux vmmPageFree =                           \
            CALL_VMM(_PageFree)                     \
            parm caller []                          \
            modify [EAX EBX ECX EDX];

#pragma aux vmmPageGetAllocInfo =                   \
            CALL_VMM(_PageGetAllocInfo)             \
            parm caller []                          \
            value no8087 [EAX EDX]                  \
            modify [EAX EBX ECX EDX];

#pragma aux vmmPageGetSizeAddr =                    \
            CALL_VMM(_PageGetSizeAddr)              \
            parm caller []                          \
            value no8087 [EAX EDX]                  \
            modify [EAX EBX ECX EDX];

#pragma aux vmmPageLock =                           \
            CALL_VMM(_PageLock)                     \
            parm caller []                          \
            modify [EAX EBX ECX EDX];

#pragma aux vmmPageReAllocate =                     \
            CALL_VMM(_PageReAllocate)               \
            parm caller []                          \
            value no8087 [EAX EDX]                  \
            modify [EAX EBX ECX EDX];

#pragma aux vmmPageResetHandlePAddr =               \
            CALL_VMM(_PageResetHandlePAddr)         \
            parm caller []                          \
            modify [EAX EBX ECX EDX];

#pragma aux vmmPageUnLock =                         \
            CALL_VMM(_PageUnLock)                   \
            parm caller []                          \
            modify [EAX EBX ECX EDX];

#pragma aux vmmPhysIntoV86 =                        \
            CALL_VMM(_PhysIntoV86)                  \
            parm caller []                          \
            modify [EAX EBX ECX EDX];

#pragma aux vmmReleaseCriticalSection =             \
            CALL_VMM(Release_Critical_Section)      \
            parm [ECX];

#pragma aux vmmRemoveExceptionHandler =             \
            CALL_VMM(Remove_Exception_Handler)      \
            "sbb    EAX, EAX"                       \
            parm [ESI]                              \
            value [EAX];

#pragma aux vmmRestoreClientState =                 \
            CALL_VMM(Restore_Client_State)          \
            parm [ESI];

#pragma aux vmmResumeExec =                         \
            CALL_VMM(Resume_Exec);

#pragma aux vmmResumeVM =                           \
            CALL_VMM(Resume_VM)                     \
            "sbb    EAX, EAX"                       \
            parm [EBX]                              \
            value [EAX];

#pragma aux vmmSaveClientState =                    \
            CALL_VMM(Save_Client_State)             \
            parm [EDI];

#pragma aux vmmScheduleGlobalEvent =                \
            CALL_VMM(Schedule_Global_Event)         \
            parm [ESI] [EDX]                        \
            value [ESI];

#pragma aux vmmScheduleVMEvent =                    \
            CALL_VMM(Schedule_VM_Event)             \
            parm [EBX] [ESI] [EDX]                  \
            value [ESI];

#pragma aux vmmSelectorMapFlat =                    \
            CALL_VMM(_SelectorMapFlat)              \
            parm caller []                          \
            modify [EAX EBX ECX EDX];

#pragma aux vmmSetDescriptor =                      \
            CALL_VMM(_SetDescriptor)                \
            parm caller []                          \
            modify [EAX EBX ECX EDX];

#pragma aux vmmSetFreePhysRegCalBk =                \
            CALL_VMM(_SetFreePhysRegCalBk)          \
            parm caller []                          \
            modify [EAX EBX ECX EDX];

#pragma aux vmmSetGlobalTimeOut =                   \
            CALL_VMM(Set_Global_Time_Out)           \
            parm [EAX] [EDX] [ESI]                  \
            value [ESI];

#pragma aux vmmSetResetV86Pageable =                \
            CALL_VMM(_SetResetV86Pageable)          \
            parm caller []                          \
            modify [EAX EBX ECX EDX];

#pragma aux vmmSetV86IntVector =                    \
            CALL_VMM(Set_V86_Int_Vector)            \
            parm [EAX] [CX] [EDX];
            
#pragma aux vmmSetVMTimeOut =                       \
            CALL_VMM(Set_VM_Time_Out)               \
            parm [EAX] [EBX] [EDX] [ESI]            \
            value [ESI];

#pragma aux vmmSignalSemaphore =                    \
            CALL_VMM(Signal_Semaphore)              \
            parm [EAX];

#pragma aux vmmSimulateFarCall =                    \
            CALL_VMM(Simulate_Far_Call)             \
            parm [ECX] [EDX];

#pragma aux vmmSimulateFarJmp =                     \
            CALL_VMM(Simulate_Far_Jmp)              \
            parm [ECX] [EDX];

#pragma aux vmmSimulateFarRet =                     \
            CALL_VMM(Simulate_Far_Ret);

#pragma aux vmmSimulateFarRetN =                    \
            CALL_VMM(Simulate_Far_Ret_N)            \
            parm [EAX];

#pragma aux vmmSimulateFarInt =                     \
            CALL_VMM(Simulate_Int)                  \
            parm [EAX];

#pragma aux vmmSimulateIret =                       \
            CALL_VMM(Simulate_Iret);

#pragma aux vmmSimulatePop =                        \
            CALL_VMM(Simulate_Pop)                  \
            value [EAX];

#pragma aux vmmSimulatePush =                       \
            CALL_VMM(Simulate_Push)                 \
            parm [EAX];

#pragma aux vmmTestReenter =                        \
            CALL_VMM(Test_Reenter);

#pragma aux vmmTestSysVMHandle =                    \
            CALL_VMM(Test_Sys_VM_Handle)            \
            "clc"                                   \
            "jz     tsvhYes"                        \
            "stc"                                   \
            "tsvhYes:"                              \
            "sbb    EAX, EAX"                       \
            parm [EBX]                              \
            value [EAX];

#pragma aux vmmUnhookInvalidPageFault =             \
            CALL_VMM(Unhook_Invalid_Page_Fault)     \
            "sbb    EAX, EAX"                       \
            parm [ESI]                              \
            value [EAX];

#pragma aux vmmUnhookV86IntChain =                  \
            CALL_VMM(Unhook_V86_Int_Chain)           \
            parm [EAX] [ESI];


#pragma aux vmmUnmapFreePhysReg =                   \
            CALL_VMM(_UnmapFreePhysReg)             \
            parm caller []                          \
            value [EAX]                             \
            modify [EAX EBX ECX EDX];

#pragma aux vmmWaitSemaphore =                      \
            CALL_VMM(Wait_Semaphore)                \
            parm [EAX] [ECX];

#pragma aux vmmXchgFreePhysReg =                    \
            CALL_VMM(_XchgFreePhysReg)              \
            parm caller []                          \
            value [EAX]                             \
            modify [EAX EBX ECX EDX];

//----------------------------------------------------------------------------
//
//  END OF 3.10 SERVICE TABLE MUST NOT SHUFFLE SERVICES BEFORE THIS POINT
//  FOR COMPATIBILITY.
//
//----------------------------------------------------------------------------

#pragma aux vmmPageCommit =                         \
            CALL_VMM(_PageCommit)                   \
            parm caller []                          \
            value [EAX]                             \
            modify [EAX EBX ECX EDX];

#pragma aux vmmPageCommitContig =                   \
            CALL_VMM(_PageCommitContig)             \
            parm caller []                          \
            value [EAX]                             \
            modify [EAX EBX ECX EDX];

#pragma aux vmmPageCommitPhys =                     \
            CALL_VMM(_PageCommitPhys)               \
            parm caller []                          \
            value [EAX]                             \
            modify [EAX EBX ECX EDX];

#pragma aux vmmPageDecommit =                       \
            CALL_VMM(_PageDecommit)                 \
            parm caller []                          \
            value [EAX]                             \
            modify [EAX EBX ECX EDX];

#pragma aux vmmPageReserve =                        \
            CALL_VMM(_PageReserve)                  \
            parm caller []                          \
            value [EAX]                             \
            modify [EAX EBX ECX EDX];

#pragma aux vmmAllocatePMCallBack =                 \
            CALL_VMM(Allocate_PM_Call_Back)         \
            value [EAX]                             \
            modify [EAX EBX ECX EDX];

#pragma aux vmmAllocateV86CallBack =                \
            CALL_VMM(Allocate_V86_Call_Back)        \
            value [EAX]                             \
            modify [EAX EBX ECX EDX];

#pragma aux vmmInstallV86BreakPoint =               \
            CALL_VMM(Install_V86_Break_Point)       \
            value [EAX]                             \
            modify [EAX EBX ECX EDX];

#pragma aux vmmRemoveV86BreakPoint =                \
            CALL_VMM(Remove_V86_Break_Point)        \
            value [EAX]                             \
            modify [EAX EBX ECX EDX];

#pragma aux vmmCancelThreadEvent =                  \
            CALL_VMM(Cancel_Thread_Event)           \
            value [EAX]                             \
            modify [EAX EBX ECX EDX];

#pragma aux vmmScheduleThreadEvent =                \
            CALL_VMM(Schedule_Thread_Event)         \
            parm caller []                          \
            value [EAX]                             \
            modify [EAX EBX ECX EDX];

#pragma aux vmmHookNMIEvent =                       \
            CALL_VMM(Hook_NMI_Event)                \
            value [EAX]                             \
            modify [EAX EBX ECX EDX];

#pragma aux vmmUnhookNMIEvent =                     \
            CALL_VMM(Unhook_NMI_Event)              \
            value [EAX]                             \
            modify [EAX EBX ECX EDX];

#pragma aux vmmCallRestrictedEvent =                \
            CALL_VMM(Call_Restricted_Event)         \
            value [EAX]                             \
            modify [EAX EBX ECX EDX];

#pragma aux vmmCancelRestrictedEvent =              \
            CALL_VMM(Cancel_Restricted_Event)       \
            value [EAX]                             \
            modify [EAX EBX ECX EDX];

#pragma aux vmmRegisterPEFProvider =                \
            CALL_VMM(Register_PEF_Provider)         \
            value [EAX]                             \
            modify [EAX EBX ECX EDX];

#pragma aux vmmGetNMIHandlerAddr =                  \
            CALL_VMM(Get_NMI_Handler_Addr)          \
            value [EAX]                             \
            modify [EAX EBX ECX EDX];

#pragma aux vmmSetNMIHandlerAddr =                  \
            CALL_VMM(Set_NMI_Handler_Addr)          \
            value [EAX]                             \
            modify [EAX EBX ECX EDX];

#pragma aux vmmGetSystemInitState =                 \
            CALL_VMM(VMM_GetSystemInitState)        \
            value [EAX]                             \
            modify [EAX EBX ECX EDX];

#pragma aux vmmRegisterWin32Services =              \
            CALL_VMM(_Register_Win32_Services)      \
            value [EAX]                             \
            modify [EAX EBX ECX EDX];

#pragma aux vmmContextCreate =                      \
            CALL_VMM(_ContextCreate)                \
            parm caller []                          \
            value [EAX]                             \
            modify [EAX EBX ECX EDX];

#pragma aux vmmContextDestroy =                     \
            CALL_VMM(_ContextDestroy)               \
            parm caller []                          \
            value [EAX]                             \
            modify [EAX EBX ECX EDX];

#pragma aux vmmContextSwitch =                      \
            CALL_VMM(_ContextSwitch)                \
            parm caller []                          \
            value [EAX]                             \
            modify [EAX EBX ECX EDX];

#pragma aux vmmGetCurrentContext =                  \
            CALL_VMM(_GetCurrentContext)            \
            parm caller []                          \
            value [EAX]                             \
            modify [EAX EBX ECX EDX];

#pragma aux vmmPageAttach =                         \
            CALL_VMM(_PageAttach)                   \
            parm caller []                          \
            value [EAX]                             \
            modify [EAX EBX ECX EDX];

#pragma aux vmmPageQuery =                          \
            CALL_VMM(_PageQuery)                    \
            parm caller []                          \
            value [EAX]                             \
            modify [EAX EBX ECX EDX];

#pragma aux vmmGetRegistryKey =                     \
            CALL_VMM(_GetRegistryKey)               \
            parm caller []                          \
            value [EAX]                             \
            modify [EAX EBX ECX EDX];

#pragma aux vmmRegCloseKey =                        \
            CALL_VMM(_RegCloseKey)                  \
            parm caller []                          \
            value [EAX]                             \
            modify [EAX EBX ECX EDX];

#pragma aux vmmRegCreateDynKey =                    \
            CALL_VMM(_RegCreateDynKey)              \
            parm caller []                          \
            value [EAX]                             \
            modify [EAX EBX ECX EDX];

#pragma aux vmmRegCreateKey =                       \
            CALL_VMM(_RegCreateKey)                 \
            parm caller []                          \
            value [EAX]                             \
            modify [EAX EBX ECX EDX];

#pragma aux vmmRegDeleteKey =                       \
            CALL_VMM(_RegDeleteKey)                 \
            parm caller []                          \
            value [EAX]                             \
            modify [EAX EBX ECX EDX];

#pragma aux vmmRegDeleteValue =                     \
            CALL_VMM(_RegDeleteValue)               \
            parm caller []                          \
            value [EAX]                             \
            modify [EAX EBX ECX EDX];

#pragma aux vmmRegEnumKey =                         \
            CALL_VMM(_RegEnumKey)                   \
            parm caller []                          \
            value [EAX]                             \
            modify [EAX EBX ECX EDX];

#pragma aux vmmRegEnumValue =                       \
            CALL_VMM(_RegEnumValue)                 \
            parm caller []                          \
            value [EAX]                             \
            modify [EAX EBX ECX EDX];

#pragma aux vmmRegFlushKey =                        \
            CALL_VMM(_RegFlushKey)                  \
            parm caller []                          \
            value [EAX]                             \
            modify [EAX EBX ECX EDX];

//#pragma aux vmmRegLoadKey =                         \
//            CALL_VMM(Reg_Load_Key)                  \
//            value [EAX]                             \
//            modify [EAX EBX ECX EDX];

#pragma aux vmmRegOpenKey =                         \
            CALL_VMM(_RegOpenKey)                   \
            parm caller []                          \
            value [EAX]                             \
            modify [EAX EBX ECX EDX];

#pragma aux vmmRegQueryInfoKey =                    \
            CALL_VMM(_RegQueryInfoKey)              \
            parm caller []                          \
            value [EAX]                             \
            modify [EAX EBX ECX EDX];

#pragma aux vmmRegQueryMultipleValues =             \
            CALL_VMM(_RegQueryMultipleValues)       \
            parm caller []                          \
            value [EAX]                             \
            modify [EAX EBX ECX EDX];

#pragma aux vmmRegQueryValue =                      \
            CALL_VMM(_RegQueryValue)                \
            parm caller []                          \
            value [EAX]                             \
            modify [EAX EBX ECX EDX];

#pragma aux vmmRegQueryValueEx =                    \
            CALL_VMM(_RegQueryValueEx)              \
            parm caller []                          \
            value [EAX]                             \
            modify [EAX EBX ECX EDX];

#pragma aux vmmRegRemapPreDefKey =                  \
            CALL_VMM(_RegRemapPreDefKey)            \
            parm caller []                          \
            value [EAX]                             \
            modify [EAX EBX ECX EDX];

//#pragma aux vmmRegSaveKey =                         \
//            CALL_VMM(Reg_Save_Key)                  \
//            value [EAX]                             \
//            modify [EAX EBX ECX EDX];

#pragma aux vmmRegSetValue =                        \
            CALL_VMM(_RegSetValue)                  \
            parm caller []                          \
            value [EAX]                             \
            modify [EAX EBX ECX EDX];

#pragma aux vmmRegSetValueEx =                      \
            CALL_VMM(_RegSetValueEx)                \
            parm caller []                          \
            value [EAX]                             \
            modify [EAX EBX ECX EDX];

#pragma aux vmmAdjustThreadExecPriority =           \
            CALL_VMM(Adjust_Thread_Exec_Priority)   \
            value [EAX]                             \
            modify [EAX EBX ECX EDX];

#pragma aux vmmCallWhenTaskSwitched =               \
            CALL_VMM(Call_When_Task_Switched)       \
            value [EAX]                             \
            modify [EAX EBX ECX EDX];

#pragma aux vmmCallWhenThreadSwitched =             \
            CALL_VMM(Call_When_Thread_Switched)     \
            value [EAX]                             \
            modify [EAX EBX ECX EDX];

#pragma aux vmmCancelCallWhenTaskSwitched =         \
            CALL_VMM(Cancel_Call_When_Task_Switched)\
            value [EAX]                             \
            modify [EAX EBX ECX EDX];

#pragma aux vmmCancelCallWhenThreadSwitched =       \
            CALL_VMM(Cancel_Call_When_Thread_Switched)\
            value [EAX]                             \
            modify [EAX EBX ECX EDX];

#pragma aux vmmGetLastUpdatedThreadExecTime =       \
            CALL_VMM(_GetLastUpdatedThreadExecTime) \
            parm caller []                          \
            value [EAX]                             \
            modify [EAX EBX ECX EDX];

#pragma aux vmmGetThreadExecTime =                  \
            CALL_VMM(_GetThreadExecTime)            \
            parm caller []                          \
            value [EAX]                             \
            modify [EAX EBX ECX EDX];

#pragma aux vmmGetThreadTerminationStatus =         \
            CALL_VMM(_GetThreadTerminationStatus)   \
            parm caller []                          \
            value [EAX]                             \
            modify [EAX EBX ECX EDX];

#pragma aux vmmGetCurThreadHandle =                 \
            CALL_VMM(Get_Cur_Thread_Handle)         \
            value [EAX]                             \
            modify [EAX EBX ECX EDX];

#pragma aux vmmGetInitialThreadHandle =             \
            CALL_VMM(Get_Initial_Thread_Handle)     \
            value [EAX]                             \
            modify [EAX EBX ECX EDX];

#pragma aux vmmGetNextThreadHandle =                \
            CALL_VMM(Get_Next_Thread_Handle)        \
            value [EAX]                             \
            modify [EAX EBX ECX EDX];

#pragma aux vmmGetSysThreadHandle =                 \
            CALL_VMM(Get_Sys_Thread_Handle)         \
            value [EAX]                             \
            modify [EAX EBX ECX EDX];

#pragma aux vmmGetThreadWin32Pri =                  \
            CALL_VMM(Get_Thread_Win32_Pri)          \
            value [EAX]                             \
            modify [EAX EBX ECX EDX];

//#pragma aux vmmGetVMHandleForThread =               \
//            CALL_VMM(Get_VM_Handle_For_Thread)      \
//            value [EAX]                             \
//            modify [EAX EBX ECX EDX];

#pragma aux vmmAllocateThreadDataSlot =             \
            CALL_VMM(_AllocateThreadDataSlot)       \
            parm caller []                          \
            value [EAX]                             \
            modify [EAX EBX ECX EDX];

#pragma aux vmmFreeThreadDataSlot =                 \
            CALL_VMM(_FreeThreadDataSlot)           \
            parm caller []                          \
            value [EAX]                             \
            modify [EAX EBX ECX EDX];

//----------------------------------------------------------------------------
//
//  DEBUG macros.
//
//----------------------------------------------------------------------------

#pragma aux dbgBreakPoint =                         \
            "int 03h";

#endif // _VMM_H_
