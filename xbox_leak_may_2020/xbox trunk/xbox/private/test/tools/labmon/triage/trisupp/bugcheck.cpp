#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "failure.h"
#include "utils.h"
#include "owner.h"
#include "getutils.h"
#include "stacks.h"
#include "isdoutils.h"
#include "bugcheck.h"

extern Cfailure *Failure;

// stolen from ntstatus.h
#define STATUS_INSUFFICIENT_RESOURCES    0xC000009AL     // ntsubauth
#define STATUS_DEVICE_DATA_ERROR         0xC000009CL
#define STATUS_DISK_OPERATION_FAILED     0xC000016AL

#define THE7B_STRING "Please see instructions located at http://winweb/setup/debug/bugcheck7b.htm\n"
#define THE77RES_STRING "STATUS_INSUFFICIENT_RESOURCES:\nPlease do a !vm, a !poolused 3, a !poolused 4 and send the output\nto the owner of the leaking pooltags or to xstress if you need\nadditional assistance.\n"
#define THE77DATA_STRING "STATUS_DEVICE_DATA_ERROR OR STATUS_DISK_OPERATION_FAILED:\nPlease run chkdsk /r on the machine and reboot.\n"
#define THEED_STRING "If your machines Boot Volume is NTFS please assign this to ntfsdev.  If it is FAT please assign it to Danlo.\n"

BOOL Handle1E();
BOOL HandleD7(CHAR *Followup, DWORD FollowupSize);
BOOL HandleC4(CHAR *Followup, DWORD FollowupSize);
BOOL HandleC0000034(ULARGE_INTEGER p1, ULARGE_INTEGER p3);
BOOL BugCheckAnalyzer(DWORD BugCode, ULARGE_INTEGER p1, ULARGE_INTEGER p2, ULARGE_INTEGER p3, ULARGE_INTEGER p4);

CHAR *BugcodeNames[255]={
    "",							//00
        "APC_INDEX_MISMATCH",
        "DEVICE_QUEUE_NOT_BUSY",
        "INVALID_AFFINITY_SET",
        "INVALID_DATA_ACCESS_TRAP",
        "INVALID_PROCESS_ATTACH_ATTEMPT",
        "INVALID_PROCESS_DETACH_ATTEMPT",
        "INVALID_SOFTWARE_INTERRUPT",
        "IRQL_NOT_DISPATCH_LEVEL",
        "IRQL_NOT_GREATER_OR_EQUAL",
        "IRQL_NOT_LESS_OR_EQUAL",
        "NO_EXCEPTION_HANDLING_SUPPORT",
        "MAXIMUM_WAIT_OBJECTS_EXCEEDED",
        "MUTEX_LEVEL_NUMBER_VIOLATION",
        "NO_USER_MODE_CONTEXT",
        "SPIN_LOCK_ALREADY_OWNED",
        "SPIN_LOCK_NOT_OWNED",			//10
        "THREAD_NOT_MUTEX_OWNER",
        "TRAP_CAUSE_UNKNOWN",
        "EMPTY_THREAD_REAPER_LIST",
        "CREATE_DELETE_LOCK_NOT_LOCKED",
        "LAST_CHANCE_CALLED_FROM_KMODE",
        "CID_HANDLE_CREATION",
        "CID_HANDLE_DELETION",
        "REFERENCE_BY_POINTER",
        "BAD_POOL_HEADER",
        "MEMORY_MANAGEMENT",
        "PFN_SHARE_COUNT",
        "PFN_REFERENCE_COUNT",
        "NO_SPIN_LOCK_AVAILABLE",
        "KMODE_EXCEPTION_NOT_HANDLED",
        "SHARED_RESOURCE_CONV_ERROR",
        "KERNEL_APC_PENDING_DURING_EXIT",  //20
        "QUOTA_UNDERFLOW",
        "FILE_SYSTEM",
        "FAT_FILE_SYSTEM",
        "NTFS_FILE_SYSTEM",
        "NPFS_FILE_SYSTEM",
        "CDFS_FILE_SYSTEM",
        "RDR_FILE_SYSTEM",
        "CORRUPT_ACCESS_TOKEN",
        "SECURITY_SYSTEM",
        "INCONSISTENT_IRP",
        "PANIC_STACK_SWITCH",
        "PORT_DRIVER_INTERNAL",			//2c
        "SCSI_DISK_DRIVER_INTERNAL",	//2d
        "DATA_BUS_ERROR",				//2e
        "INSTRUCTION_BUS_ERROR",
        "SET_OF_INVALID_CONTEXT",
        "PHASE0_INITIALIZATION_FAILED",
        "PHASE1_INITIALIZATION_FAILED",
        "UNEXPECTED_INITIALIZATION_CALL",
        "CACHE_MANAGER",
        "NO_MORE_IRP_STACK_LOCATIONS",
        "DEVICE_REFERENCE_COUNT_NOT_ZERO",
        "FLOPPY_INTERNAL_ERROR",
        "SERIAL_DRIVER_INTERNAL",
        "SYSTEM_EXIT_OWNED_MUTEX",
        "SYSTEM_UNWIND_PREVIOUS_USER",
        "SYSTEM_SERVICE_EXCEPTION",
        "INTERRUPT_UNWIND_ATTEMPTED",
        "INTERRUPT_EXCEPTION_NOT_HANDLED",
        "MULTIPROCESSOR_CONFIGURATION_NOT_SUPPORTED",
        "NO_MORE_SYSTEM_PTES",
        "TARGET_MDL_TOO_SMALL",
        "MUST_SUCCEED_POOL_EMPTY",
        "ATDISK_DRIVER_INTERNAL",
        "NO_SUCH_PARTITION",
        "MULTIPLE_IRP_COMPLETE_REQUESTS",
        "INSUFFICIENT_SYSTEM_MAP_REGS",
        "DEREF_UNKNOWN_LOGON_SESSION",
        "REF_UNKNOWN_LOGON_SESSION",
        "CANCEL_STATE_IN_COMPLETED_IRP",
        "PAGE_FAULT_WITH_INTERRUPTS_OFF",
        "IRQL_GT_ZERO_AT_SYSTEM_SERVICE",
        "STREAMS_INTERNAL_ERROR",
        "FATAL_UNHANDLED_HARD_ERROR",
        "NO_PAGES_AVAILABLE",
        "PFN_LIST_CORRUPT",
        "NDIS_INTERNAL_ERROR",
        "PAGE_FAULT_IN_NONPAGED_AREA",
        "REGISTRY_ERROR",
        "MAILSLOT_FILE_SYSTEM",
        "NO_BOOT_DEVICE",
        "LM_SERVER_INTERNAL_ERROR",
        "DATA_COHERENCY_EXCEPTION",
        "INSTRUCTION_COHERENCY_EXCEPTION",
        "XNS_INTERNAL_ERROR",
        "FTDISK_INTERNAL_ERROR",
        "PINBALL_FILE_SYSTEM",
        "CRITICAL_SERVICE_FAILED",
        "SET_ENV_VAR_FAILED",
        "HAL_INITIALIZATION_FAILED",
        "HEAP_INITIALIZATION_FAILED",
        "OBJECT_INITIALIZATION_FAILED",
        "SECURITY_INITIALIZATION_FAILED",
        "PROCESS_INITIALIZATION_FAILED",
        "HAL1_INITIALIZATION_FAILED",
        "OBJECT1_INITIALIZATION_FAILED",
        "SECURITY1_INITIALIZATION_FAILED",
        "SYMBOLIC_INITIALIZATION_FAILED",
        "MEMORY1_INITIALIZATION_FAILED",
        "CACHE_INITIALIZATION_FAILED",
        "CONFIG_INITIALIZATION_FAILED",
        "FILE_INITIALIZATION_FAILED",
        "IO1_INITIALIZATION_FAILED",
        "LPC_INITIALIZATION_FAILED",
        "PROCESS1_INITIALIZATION_FAILED",
        "REFMON_INITIALIZATION_FAILED",
        "SESSION1_INITIALIZATION_FAILED",
        "SESSION2_INITIALIZATION_FAILED",
        "SESSION3_INITIALIZATION_FAILED",
        "SESSION4_INITIALIZATION_FAILED",
        "SESSION5_INITIALIZATION_FAILED",
        "ASSIGN_DRIVE_LETTERS_FAILED",
        "CONFIG_LIST_FAILED",
        "BAD_SYSTEM_CONFIG_INFO",
        "CANNOT_WRITE_CONFIGURATION",
        "PROCESS_HAS_LOCKED_PAGES",
        "KERNEL_STACK_INPAGE_ERROR",
        "PHASE0_EXCEPTION",
        "MISMATCHED_HAL",
        "KERNEL_DATA_INPAGE_ERROR",	
        "INACCESSIBLE_BOOT_DEVICE",
        "BUGCODE_PSS_MESSAGE",
        "INSTALL_MORE_MEMORY",
        "WINDOWS_NT_BANNER",
        "UNEXPECTED_KERNEL_MODE_TRAP",
        "NMI_HARDWARE_FAILURE",
        "SPIN_LOCK_INIT_FAILURE",
        "DFS_FILE_SYSTEM", //82
        "OFS_FILE_SYSTEM",
        "RECOM_DRIVER",
        "SETUP_FAILURE", //85
        "AUDIT_FAILURE",
        "WINDOWS_NT_CSD_STRING",
        "WINDOWS_NT_INFO_STRING",
        "WINDOWS_NT_MP_STRING",
        "THREAD_TERMINATE_HELD_MUTEX",
        "MBR_CHECKSUM_MISMATCH", //8b
        "BUGCODE_PSS_CRASH_INIT",
        "BUGCODE_PSS_CRASH_PROGRESS",
        "BUGCODE_PSS_CRASH_DONE",
        "PP0_INITIALIZATION_FAILED",  //8f
        "PP1_INITIALIZATION_FAILED",  //90
        "WIN32K_INIT_OR_RIT_FAILURE",
        "UP_DRIVER_ON_MP_SYSTEM",	//92
        "INVALID_KERNEL_HANDLE",
        "KERNEL_STACK_LOCKED_AT_EXIT",  //94
        "PNP_INTERNAL_ERROR",
        "INVALID_WORK_QUEUE_ITEM",		//96
        "BOUND_IMAGE_UNSUPPORTED",
        "END_OF_NT_EVALUATION_PERIOD",
        "INVALID_REGION_OR_SEGMENT",
        "SYSTEM_LICENSE_VIOLATION",
        "UDFS_FILE_SYSTEM",
        "MACHINE_CHECK_EXCEPTION",	//9c
        "WINDOWS_NT_INFO_STRING_PLURAL",
        "WINDOWS_NT_RC_STRING",
        "DRIVER_POWER_STATE_FAILURE", //9f
        "INTERNAL_POWER_ERROR",
        "PCI_BUS_DRIVER_INTERNAL",			//a1
        "MEMORY_IMAGE_CORRUPT",
        "ACPI_DRIVER_INTERNAL",
        "CNSS_FILE_SYSTEM_FILTER",
        "ACPI_BIOS_ERROR",
        "FP_EMULATION_ERROR",			//a6
        "BAD_EXHANDLE",
        "BOOTING_IN_SAFEMODE_MINIMAL",	//a8
        "BOOTING_IN_SAFEMODE_NETWORK", //a9
        "BOOTING_IN_SAFEMODE_DSREPAIR",	//aa
        "SESSION_HAS_VALID_POOL_ON_EXIT",
        "HAL_MEMORY_ALLOCATION",	//ac
        "BUGCODE_PSS_MESSAGE_A",	//ad
        "BUGCODE_PSS_MESSAGE_1E",	//ae
        "BUGCODE_PSS_MESSAGE_23",	//af
        "BUGCODE_PSS_MESSAGE_2E",	//b0
        "BUGCODE_PSS_MESSAGE_3F",	//b1
        "BUGCODE_PSS_MESSAGE_7B",	//b2
        "BUGCODE_PSS_MESSAGE_7F",	//b3
        "VIDEO_DRIVER_INIT_FAILURE",	//b4
        "BOOTLOG_LOADED",	//b5
        "BOOTLOG_NOT_LOADED",	//b6
        "BOOTLOG_ENABLED",	//b7
        "ATTEMPTED_SWITCH_FROM_DPC",	//b8
        "CHIPSET_DETECTED_ERROR",
        "SESSION_HAS_VALID_VIEWS_ON_EXIT",
        "NETWORK_BOOT_INITIALIZATION_FAILED",
        "NETWORK_BOOT_DUPLICATE_ADDRESS",
        "INVALID_HIBERNATED_STATE",
        "ATTEMPTED_WRITE_TO_READONLY_MEMORY",	//be
        "MUTEX_ALREADY_OWNED",
        "PCI_CONFIG_SPACE_ACCESS_FAILURE",
        "SPECIAL_POOL_DETECTED_MEMORY_CORRUPTION",	//c1
        "BAD_POOL_CALLER",		//c2
        "BUGCODE_PSS_MESSAGE_SIGNATURE",	//c3
        "DRIVER_VERIFIER_DETECTED_VIOLATION",	//c4
        "DRIVER_CORRUPTED_EXPOOL",	//c5
        "DRIVER_CAUGHT_MODIFYING_FREED_POOL",	//c6
        "TIMER_OR_DPC_INVALID",	//c7
        "IRQL_UNEXPECTED_VALUE",	//c8
        "DRIVER_VERIFIER_IOMANAGER_VIOLATION",	//c9
        "PNP_DETECTED_FATAL_ERROR",	//ca
        "DRIVER_LEFT_LOCKED_PAGES_IN_PROCESS",	//cb
        "PAGE_FAULT_IN_FREED_SPECIAL_POOL",	//cc
        "PAGE_FAULT_BEYOND_END_OF_ALLOCATION",	//cd
        "DRIVER_UNLOADED_WITHOUT_CANCELLING_PENDING_OPERATIONS",	//ce
        "TERMINAL_SERVER_DRIVER_MADE_INCORRECT_MEMORY_REFERENCE",	//cf
        "DRIVER_CORRUPTED_MMPOOL",	//d0
        "DRIVER_IRQL_NOT_LESS_OR_EQUAL",	//d1
        "BUGCODE_ID_DRIVER",		//d2
        "DRIVER_PORTION_MUST_BE_NONPAGED",	//d3
        "SYSTEM_SCAN_AT_RAISED_IRQL_CAUGHT_IMPROPER_DRIVER_UNLOAD",	//d4
        "DRIVER_PAGE_FAULT_IN_FREED_SPECIAL_POOL",	//d5
        "DRIVER_PAGE_FAULT_BEYOND_END_OF_ALLOCATION",  //d6
        "DRIVER_UNMAPPING_INVALID_VIEW",  //d7
        "DRIVER_USED_EXCESSIVE_PTES",//d8
        "LOCKED_PAGES_TRACKER_CORRUPTION",//d9
        "SYSTEM_PTE_MISUSE",//da
        "DRIVER_CORRUPTED_SYSPTES",//db
        "DRIVER_INVALID_STACK_ACCESS",//dc
        "BUGCODE_PSS_MESSAGE_A5",//dd
        "POOL_CORRUPTION_IN_FILE_AREA",//de
        "IMPERSONATING_WORKER_THREAD",//df
        "ACPI_BIOS_FATAL_ERROR",//e0
        "WORKER_THREAD_RETURNED_AT_BAD_IRQL",//e1
        "MANUALLY_INITIATED_CRASH",//e2
        "RESOURCE_NOT_OWNED",//e3
        "WORKER_INVALID",//e4
        "POWER_FAILURE_SIMULATE",//e5
        "DRIVER_VERIFIER_DMA_VIOLATION",//e6
        "INVALID_FLOATING_POINT_STATE",//e7
        "INVALID_CANCEL_OF_FILE_OPEN",//e8
        "ACTIVE_EX_WORKER_THREAD_TERMINATION", // e9
        "THREAD_STUCK_IN_DEVICE_DRIVER", //ea
        "",
        "",
        "UNMOUNTABLE_BOOT_VOLUME"
};


BOOL HandleBugCheck()
    {
    BOOL bRet = TRUE; 
    INT numLines = 0, i = 0;
    CHAR *LineHeads[MAXLINES] = {NULL, NULL};
    ULARGE_INTEGER Addr = {0}, BCData[5] = {0};
    CHAR *buffer;
    CHAR szTempBuf[512];
    CHAR szCommand[64];
    CHAR szStringBuf[512];
    CHAR Followup[128];
    DWORD FollowupSize = sizeof(Followup);
    
    if ((Failure->Architecture()) == MACH_X86)
        strcpy(szCommand, "dd ");
    else
        strcpy(szCommand, "dq ");
    
    if (stristr(Failure->Description(), "wdbugcheck"))
        strcat(szCommand, "watchdog!g_WdBugCheckData l5\n");
    else
        strcat(szCommand, "nt!KiBugCheckData l5\n");
    
    bRet = Failure->SendCommand(szCommand, &buffer);
    if (!bRet)
        return bRet;
    
    numLines = GetLinesFromString(buffer, LineHeads);
    
    for (i=0;i<numLines;i++) 
        {
        if (IsHex(LineHeads[i])) 
            break;
        }
    if (i >= numLines)
        {
        free(buffer);
        return FALSE;
        }
    
   	if ((Failure->Architecture()) == MACH_X86)
        {
        if (5 == sscanf(LineHeads[i], "%x  %x %x %x %x", &Addr.LowPart, &(BCData[0].LowPart),
            &(BCData[1].LowPart), &(BCData[2].LowPart), &(BCData[3].LowPart)))
            {
            if (i < numLines - 1) 
                {
                if (2 != sscanf(LineHeads[i+1], "%x  %x", &Addr.LowPart, &(BCData[4].LowPart)))
                    {
                    free(buffer);
                    return FALSE;
                    }
                }
            }
        }
    else if ((Failure->Architecture()) == MACH_IA64)
        {
        if (6 == sscanf(LineHeads[i], "%x`%x  %x`%x %x`%x", &Addr.HighPart, &Addr.LowPart, &(BCData[0].HighPart),
            &(BCData[0].LowPart), &(BCData[1].HighPart), &(BCData[1].LowPart)))
            {
            if (i < numLines-1) 
                {
                if (6 == sscanf(LineHeads[i+1], "%x`%x  %x`%x %x`%x", &Addr.HighPart, &Addr.LowPart, 
                    &(BCData[2].HighPart), &(BCData[2].LowPart),
                    &(BCData[3].HighPart), &(BCData[3].LowPart)))
                    {
                    if (i < numLines-2) 
                        {
                        if (4 != sscanf(LineHeads[i+2], "%x`%x  %x`%x", &Addr.HighPart, &Addr.LowPart, &(BCData[4].HighPart),
                            &(BCData[4].LowPart)))
                            {
                            free(buffer);
                            return FALSE;
                            }
                        }
                    }
                }
            }
        }
    
    free(buffer);
    BugCheckAnalyzer(BCData[0].LowPart, 
        BCData[1], BCData[2],
        BCData[3], BCData[4]);
    
    switch(BCData[0].LowPart)
        {
        case 0x1e:
            Handle1E();
            break;
            
        case 0x4d:
            if (BCData[3].LowPart == 0)
                {
                bRet = GetSpecial("bugcheck!4d3p0", Followup, FollowupSize);
                if (bRet)
                    Failure->SetFollowup(Followup);
                
                return bRet;
                }
            break;
            
        case 0xd7:
            bRet = HandleD7(Followup, FollowupSize);
            if (bRet)
                Failure->SetFollowup(Followup);
            
            return bRet;
            break;
            
        case 0xea:
            Failure->SendBangThreadStack(BCData[1].LowPart, BCData[1].HighPart);
            break;
            
        case 0xed:
            Failure->AppendDescription(THE77RES_STRING);
            Failure->SetFollowup("Machine Owner");
            return TRUE;
            break;
            
        case 0xc4:
            if (BCData[1].LowPart == 0x60)
                {
                bRet = HandleC4(Followup, FollowupSize);
                if (bRet)
                    Failure->SetFollowup(Followup);
                
                return bRet;
                }
            break;
            
        case 0x77:
            switch(BCData[1].LowPart)
                {
                case STATUS_INSUFFICIENT_RESOURCES:
                    {
                    Failure->AppendDescription(THE77RES_STRING);
                    Failure->SetFollowup("Machine Owner");
                    return TRUE;
                    break;
                    }
                case STATUS_DEVICE_DATA_ERROR:
                case STATUS_DISK_OPERATION_FAILED:
                    {
                    Failure->AppendDescription(THE77DATA_STRING);
                    Failure->SetFollowup("Machine Owner");
                    return TRUE;
                    break;
                    }
                default:
                    {
                    break;
                    }
                }
            break;
            
                case 0x7a:
                    switch(BCData[2].LowPart)
                        {
                        case STATUS_INSUFFICIENT_RESOURCES:
                            {
                            Failure->AppendDescription(THE77RES_STRING);
                            Failure->SetFollowup("Machine Owner");
                            return TRUE;
                            break;
                            }
                        case STATUS_DEVICE_DATA_ERROR:
                        case STATUS_DISK_OPERATION_FAILED:
                            {
                            Failure->AppendDescription(THE77DATA_STRING);
                            Failure->SetFollowup("Machine Owner");
                            return TRUE;
                            break;
                            }
                        default:
                            {
                            break;
                            }
                        }
                    break;
                    
                        case 0x7b:
                            Failure->AppendDescription(THE7B_STRING);
                            Failure->SetFollowup("Machine Owner");
                            return TRUE;
                            break;
                            
                        case 0x9c:
                            if ((Failure->Architecture()) == MACH_X86)
                                {
                                Failure->SendCommand("!mca\n", &buffer);
                                Failure->AppendMisc(buffer);
                                free(buffer);
                                if (!(strstr(Failure->Misc(), "Intel")))
                                    return TRUE;
                                }
                            break;
                            
                        case 0xc000026c:
                            if (GetStringAtAddress(BCData[1], szStringBuf, sizeof(szStringBuf), FALSE))
                                {
                                sprintf(szTempBuf, 
                                    "The driver %s is bad.  Please replace this driver or contact its owner.\n", 
                                    szStringBuf);
                                }
                            else
                                sprintf(szTempBuf, 
                                "You have a bad driver at address %x. Type 'dc %x' and replace the driver specified or contact its owner.\n", 
                                BCData[1].LowPart, BCData[1].LowPart);
                            
                            Failure->AppendDescription(szTempBuf);
                            Failure->SetFollowup("Machine Owner");
                            return TRUE;
                            break;
                            
                        case 0xc0000034:
                            return HandleC0000034(BCData[1], BCData[3]);
                            
                            break;
                            
                            
                        default:
                            break;
    }
    
    bRet = GetSpecial(Failure->Description(), Followup, FollowupSize);
    if (bRet)
        Failure->SetFollowup(Followup);
    
    return bRet;
}


BOOL HandleC0000034(ULARGE_INTEGER p1, ULARGE_INTEGER p3)
    {
    CHAR *szStringBuf = NULL;
    CHAR Followup[64];
    CHAR *temp = NULL;
    BOOL Assigned = FALSE;
    
    if (!(szStringBuf = (CHAR *)malloc(512)))
        return FALSE;
    
    Failure->AppendMisc("The following Drivers image was not found:\n");
    if (GetStringAtAddress(p3, szStringBuf, 512, FALSE))
        {
        Failure->AppendMisc(szStringBuf);
        Failure->AppendMisc("\n");
        
        temp = strrchr(szStringBuf, '.');
        if (NULL != temp)
            temp[0] = '\0';
        
        temp = strrchr(szStringBuf, '\\');
        if (NULL != temp)
            {
            temp++;
            if (GetOwner(temp, Followup, sizeof(Followup)))
                {
                if (!stristr(Followup, "xstress"))
                    {
                    Assigned = TRUE;
                    Failure->SetFollowup(Followup);
                    }
                }
            }
        
        }
    
    if (GetStringAtAddress(p1, szStringBuf, 512, FALSE))
        {
        Failure->AppendMisc("Load Attempted by:\n");
        Failure->AppendMisc(szStringBuf);
        Failure->AppendMisc("\n");
        
        if (!Assigned)
            {
            temp = strrchr(szStringBuf, '.');
            if (NULL != temp)
                temp[0] = '\0';
            
            temp = strrchr(szStringBuf, '\\');
            if (NULL != temp)
                {
                temp++;
                if (GetOwner(temp, Followup, sizeof(Followup)))
                    Failure->SetFollowup(Followup);
                else
                    {
                    free(szStringBuf);
                    return FALSE;
                    }
                }
            }
        }
    
    free(szStringBuf);
    return TRUE;
    }

//
// Handles a bugcheck 1E
//
BOOL Handle1E()
    {
    ULARGE_INTEGER exr = {0,0};
    ULARGE_INTEGER cxr = {0,0};
    ULARGE_INTEGER Address = {0,0};
    CHAR *buffer = NULL;
    
    if (!GetParamFromStackBuffer("KeBugCheckEx", EBP, &Address))
        return FALSE;
    
    GetDxLongNumber(Address, 30, &exr);
    GetDxLongNumber(Address, 32, &cxr);
    
    GetExceptionBuffer(exr, cxr);
    
    DoEXR(exr);
    return Failure->SendCXRStack(cxr);
    }

//
// Handles a bugcheck c4
//
BOOL HandleC4(CHAR *Followup, DWORD FollowupSize)
    {
    CHAR BadDriver[129];
    CHAR *inbuff = NULL;
    
    if (!GetViBadDriverString(BadDriver, 129))
        return FALSE;
    
    Failure->AppendMisc("ViBadDriver = ");
    Failure->AppendMisc(BadDriver);
    Failure->AppendMisc("\n");
    
    inbuff = strchr(BadDriver, '.');
    if (inbuff)
        inbuff[0] = '\0';
    
    GetOwner(BadDriver, Followup, FollowupSize);
    return TRUE;
    }

// bugcheck D7 always goes to the line under ntkrnlmp!MmUnmapViewInSessionSpace - per landyw
BOOL HandleD7(CHAR *Followup, DWORD FollowupSize)
    {
    CHAR *LineHeads[MAXLINES] = {NULL, NULL};
    CHAR *sidebuff = NULL;
    DWORD numLines = 0;
    DWORD i=0;
    
    if ((Failure->StackBuffer()) == NULL)
        return FALSE;
    
    if (!strstr(Failure->StackBuffer(), "!MmUnmapViewInSessionSpace"))
        return FALSE;
    
    sidebuff = (CHAR *)malloc(strlen(Failure->StackBuffer()) + 1);
    if (!sidebuff)
        return FALSE;
    
    strcpy(sidebuff, Failure->StackBuffer());
    
    numLines = GetLinesFromString(sidebuff, LineHeads);
    StripStack(LineHeads, &numLines);
    
    for (i=0; i<numLines; i++)
        {
        if (strstr(LineHeads[i], "!MmUnmapViewInSessionSpace"))
            {
            AssignStack(&(LineHeads[i+1]),
                (UINT) numLines, 
                Followup, FollowupSize);
            free(sidebuff);
            return TRUE;
            
            }
        
        }
    free(sidebuff);
    return FALSE;
    }

BOOL BugCheckAnalyzer(DWORD BugCode, ULARGE_INTEGER p1, ULARGE_INTEGER p2, ULARGE_INTEGER p3, ULARGE_INTEGER p4)
    {
    CHAR buf[256];
    
    if ((BugCode > 0) && (BugCode <= 0xff)) 
        {
        sprintf(buf, "Bugcheck %x - %s ", BugCode, BugcodeNames[BugCode]);
        } 
    else 
        {
        if (BugCode == 0xc000026c)
            sprintf(buf, "Bugcheck %x - STATUS_DRIVER_UNABLE_TO_LOAD", BugCode);
        else
            sprintf(buf, "Bugcheck %x - ", BugCode);
        }
    
    Failure->SetDescription(buf);
    
    if ((Failure->Architecture()) == MACH_X86)
        {
        sprintf(buf, "%08x %08x %08x %08x", p1.LowPart, p2.LowPart, p3.LowPart, p4.LowPart);
        Failure->AppendDescription(buf);    
        }    
    else if ((Failure->Architecture()) == MACH_IA64)
        {
        if ((Failure->Architecture()) == MACH_IA64)
            {
            sprintf(buf, "BC Parameters: %016I64x %016I64x \n               %016I64x %016I64x\n", 
                p1.QuadPart, p2.QuadPart, p3.QuadPart, p4.QuadPart);
            Failure->AppendMisc(buf);
            }        
        }
    
    return TRUE;
    }

BOOL HandleWDMBugCheck()
    {
    ULARGE_INTEGER dwESI = {0,0};
    ULARGE_INTEGER dwStruct = {0,0};
    ULARGE_INTEGER dwStrLoc = {0,0};
    CHAR buffer[128];
    CHAR ownerBuf[1024];
    CHAR *tmp = NULL;
    
    if (!GetESI(&dwESI))
        return FALSE;
    
    if (!GetDxLongNumber(dwESI, 6, &dwStruct))
        return FALSE;
    
    if (!GetDxLongNumber(dwStruct, 2, &dwStrLoc))
        return FALSE;
    
    if (!GetStringAtAddress(dwStrLoc, buffer, sizeof(buffer), TRUE))
        return FALSE;
    
    sprintf(ownerBuf, "Driver %s has caused a WDM assert.\n", buffer);
    Failure->AppendMisc(ownerBuf);
    
    // Get assertion text
    if (!GetDxLongNumber(dwESI, 11, &dwStruct))
        return FALSE;
    
    if (GetStringAtAddress(dwStruct, ownerBuf, sizeof(ownerBuf), FALSE))
        {
        Failure->AppendMisc(ownerBuf);
        Failure->AppendMisc("\n");
        }
    
    if (tmp = strchr(buffer, '.'))
        {
        tmp[0] = '\0';
        }
    
    if (GetOwner(buffer, ownerBuf, sizeof(ownerBuf)) != NULL)
        Failure->SetFollowup(ownerBuf);
    
    return TRUE;
    }