//=============================================================================
// Includes
//=============================================================================
#include <windows.h>
#include <winerror.h>
#ifdef _XBOX
#include "convhelp.h"
#endif

#ifndef NO_ERRORS_DPLAY
#include <dplay.h>
#endif // NO_ERRORS_DPLAY

#ifndef NO_ERRORS_DPLAY8
#include <dplay8.h>
#endif // NO_ERRORS_DPLAY8

#ifndef NO_ERRORS_DVOICE
#include <dvoice.h>
#endif // NO_ERRORS_DVOICE

#ifndef NO_ERRORS_DSOUND
#include <mmreg.h> // NT BUILD complains about WAVEFORMATEX without it
#include <dsound.h>
#endif // NO_ERRORS_DSOUND

#ifndef NO_ERRORS_DDRAW
#include <ddraw.h>
#endif // NO_ERRORS_DDRAW

#ifndef _XBOX
#ifndef NO_ERRORS_WINSOCK
#include <winsock.h>
#endif // NO_ERRORS_WINSOCK
#else // ! XBOX
#ifndef NO_ERRORS_WINSOCK
#include <winsockx.h>
#endif // NO_ERRORS_WINSOCK
#endif // XBOX

#include <raserror.h>
#ifndef _XBOX  // no TAPI
#include <tapi.h>
#endif

#ifndef NO_ERRORS_TNCONTRL
// We can't actually do this because there's some wacky #including with that.  We'll
// just copy the code we need.
//#include "..\tncontrl\tncontrl.h"

#define TNERR_CONNECTIONDROPPED		0x86661001
#define TNERR_LOSTTESTER			0x86661002

//#define TNSR_INSYNC					S_OK
#define TNSR_USERCANCEL				0x86661101
#define TNSR_LOSTTESTER				0x86661103

//#define TNWR_GOTEVENT				S_OK
#define TNWR_USERCANCEL				0x86661111
#define TNWR_TIMEOUT				0x86661112
#define TNWR_LOSTTESTER				0x86661113

//#define TNCWR_CONNECTED				S_OK
#define TNCWR_USERCANCEL			0x86661121
#define TNCWR_TIMEOUT				0x86661122
#define TNCWR_LOSTTESTER			0x86661123

#endif // NO_ERRORS_TNCONTRL

#include "main.h"
#include "errors.h"
#include "sprintf.h"
#ifdef _XBOX
#include "debugprint.h"
#endif // XBOX





//=========================================================================================
// Data
//=========================================================================================
TNERRORITEM g_TNErrorTable [] =
{
	// 0L
	// S_OK
	// DP_OK
	// DV_OK
	// DPN_OK
	// ERROR_SUCCESS
	NO_ERROR,						"S_OK, ERROR_SUCCESS, NO_ERROR", "The operation completed successfully.",
	// 1L
	// S_FALSE
	ERROR_INVALID_FUNCTION,			"S_FALSE, ERROR_INVALID_FUNCTION", "Generic failure.' or 'Incorrect function.",
	// 2L
	ERROR_FILE_NOT_FOUND,			"ERROR_FILE_NOT_FOUND", "The system cannot find the file specified.",
	// 3L
	ERROR_PATH_NOT_FOUND,			"ERROR_PATH_NOT_FOUND", "The system cannot find the path specified.",
	// 4L
	ERROR_TOO_MANY_OPEN_FILES,		"ERROR_TOO_MANY_OPEN_FILES", "The system cannot open the file.",
	// 5L
	ERROR_ACCESS_DENIED,			"ERROR_ACCESS_DENIED", "Access is denied.",
	// 6L
	ERROR_INVALID_HANDLE,			"ERROR_INVALID_HANDLE", "The handle is invalid.",
	// 7L
	ERROR_ARENA_TRASHED,			"ERROR_ARENA_TRASHED", "The storage control blocks were destroyed.",
	// 8L
	ERROR_NOT_ENOUGH_MEMORY,		"ERROR_NOT_ENOUGH_MEMORY", "Not enough storage is available to process this command.",
	// 9L
	ERROR_INVALID_BLOCK,			"ERROR_INVALID_BLOCK", "The storage control block address is invalid.",
	// 10L
	ERROR_BAD_ENVIRONMENT,			"ERROR_BAD_ENVIRONMENT", "The environment is incorrect.",
	// 11L
	ERROR_BAD_FORMAT,				"ERROR_BAD_FORMAT", "An attempt was made to load a program with an incorrect format.",
	// 12L
	ERROR_INVALID_ACCESS,			"ERROR_INVALID_ACCESS", "The access code is invalid.",
	// 13L
	ERROR_INVALID_DATA,				"ERROR_INVALID_DATA", "The data is invalid.",
	// 14L
	ERROR_OUTOFMEMORY,				"ERROR_OUTOFMEMORY", "Not enough storage is available to complete this operation.",
	// 15L
	ERROR_INVALID_DRIVE,			"ERROR_INVALID_DRIVE", "The system cannot find the drive specified.",
	// 16L
	ERROR_CURRENT_DIRECTORY,		"ERROR_CURRENT_DIRECTORY", "The directory cannot be removed.",
	// 17L
	ERROR_NOT_SAME_DEVICE,			"ERROR_NOT_SAME_DEVICE", "The system cannot move the file to a different disk drive.",
	// 18L
	ERROR_NO_MORE_FILES,			"ERROR_NO_MORE_FILES", "There are no more files.",
	// 19L
	ERROR_WRITE_PROTECT,			"ERROR_ACCESS_DENIED", "The media is write protected.",
	// 20L
	ERROR_BAD_UNIT,					"ERROR_BAD_UNIT", "The system cannot find the device specified.",
	// 21L
	ERROR_NOT_READY,				"ERROR_NOT_READY", "The device is not ready.",
	// 22L
	ERROR_BAD_COMMAND,				"ERROR_BAD_COMMAND", "The device does not recognize the command.",
	// 23L
	ERROR_CRC,						"ERROR_CRC", "Data error (cyclic redundancy check).",
	// 24L
	ERROR_BAD_LENGTH,				"ERROR_BAD_LENGTH", "The program issued a command but its length is incorrect.",
	// 25L
	ERROR_SEEK,						"ERROR_SEEK", "The drive cannot locate a specific area or track on the disk.",
	// 26L
	ERROR_NOT_DOS_DISK,				"ERROR_NOT_DOS_DISK", "The specified disk or diskette cannot be accessed.",
	// 27L
	ERROR_SECTOR_NOT_FOUND,			"ERROR_SECTOR_NOT_FOUND", "The drive cannot find the sector requested.",
	// 28L
	ERROR_OUT_OF_PAPER,				"ERROR_OUT_OF_PAPER", "The printer is out of paper.",
	// 29L
	ERROR_WRITE_FAULT,				"ERROR_WRITE_FAULT", "The system cannot write to the specified device.",
	// 30L
	ERROR_READ_FAULT,				"ERROR_READ_FAULT", "The system cannot read from the specified device.",
	// 31L
	ERROR_GEN_FAILURE,				"ERROR_GEN_FAILURE", "A device attached to the system is not functioning.",
	// 32L
	ERROR_SHARING_VIOLATION,		"ERROR_SHARING_VIOLATION", "The process cannot access the file because it is being used by another process.",
	// 33L
	ERROR_LOCK_VIOLATION,			"ERROR_LOCK_VIOLATION", "The process cannot access the file because another process has locked a portion of the file.",
	// 34L
	ERROR_WRONG_DISK,				"ERROR_WRONG_DISK", "The wrong diskette is in the drive.",
	// 36L
	ERROR_SHARING_BUFFER_EXCEEDED,	"ERROR_SHARING_BUFFER_EXCEEDED", "Too many files opened for sharing.",
	// 38L
	ERROR_HANDLE_EOF,				"ERROR_HANDLE_EOF", "Reached the end of the file.",
	// 39L
	ERROR_HANDLE_DISK_FULL,			"ERROR_HANDLE_DISK_FULL", "The disk is full.",
	// 50L
	ERROR_NOT_SUPPORTED,			"ERROR_NOT_SUPPORTED", "The network request is not supported.",
	// 51L
	ERROR_REM_NOT_LIST,				"ERROR_REM_NOT_LIST", "The remote computer is not available.",
	// 52L
	ERROR_DUP_NAME,					"ERROR_DUP_NAME", "A duplicate name exists on the network.",
	// 53L
	ERROR_BAD_NETPATH,				"ERROR_BAD_NETPATH", "The network path was not found.",
	// 54L
	ERROR_NETWORK_BUSY,				"ERROR_NETWORK_BUSY", "The network is busy.",
	// 55L
	ERROR_DEV_NOT_EXIST,			"ERROR_DEV_NOT_EXIST", "The specified network resource or device is no longer available.",
	// 56L
	ERROR_TOO_MANY_CMDS,			"ERROR_TOO_MANY_CMDS", "The network BIOS command limit has been reached.",
	// 57L
	ERROR_ADAP_HDW_ERR,				"ERROR_ADAP_HDW_ERR", "A network adapter hardware error occurred.",
	// 58L
	ERROR_BAD_NET_RESP,				"ERROR_BAD_NET_RESP", "The specified server cannot perform the requested operation.",
	// 59L
	ERROR_UNEXP_NET_ERR,			"ERROR_UNEXP_NET_ERR", "An unexpected network error occurred.",
	// 60L
	ERROR_BAD_REM_ADAP,				"ERROR_BAD_REM_ADAP", "The remote adapter is not compatible.",

	// 72L
	ERROR_REDIR_PAUSED,				"ERROR_REDIR_PAUSED", "The specified printer or disk device has been paused.",
	// 80L
	ERROR_FILE_EXISTS,				"ERROR_FILE_EXISTS", "The file exists.",
	// 82L
	ERROR_CANNOT_MAKE,				"ERROR_CANNOT_MAKE", "The directory or file cannot be created.",
	// 83L
	ERROR_FAIL_I24,					"ERROR_FAIL_I24", "Fail on INT 24.",
	// 84L
	ERROR_OUT_OF_STRUCTURES,		"ERROR_OUT_OF_STRUCTURES", "Storage to process this request is not available.",
	// 85L
	ERROR_ALREADY_ASSIGNED,			"ERROR_ALREADY_ASSIGNED", "The local device name is already in use.",
	// 86L
	ERROR_INVALID_PASSWORD,			"ERROR_INVALID_PASSWORD", "The specified network password is not correct.",
	// 87L
	ERROR_INVALID_PARAMETER,		"ERROR_INVALID_PARAMETER", "The parameter is incorrect.",
	// 88L
	ERROR_NET_WRITE_FAULT,			"ERROR_NET_WRITE_FAULT", "A write fault occurred on the network.",

	//110L
	ERROR_OPEN_FAILED,				"ERROR_OPEN_FAILED", "The system cannot open the device or file specified.",
	//111L
	ERROR_BUFFER_OVERFLOW,			"ERROR_BUFFER_OVERFLOW", "The file name is too long.",
	//112L
	ERROR_DISK_FULL,				"ERROR_DISK_FULL", "There is not enough space on the disk.",
	//113L
	ERROR_NO_MORE_SEARCH_HANDLES,	"ERROR_NO_MORE_SEARCH_HANDLES", "No more internal file identifiers available.",
	//114L
	ERROR_INVALID_TARGET_HANDLE,	"ERROR_INVALID_TARGET_HANDLE", "The target internal file identifier is incorrect.",
	// ... 115 & 116 not specified ...
	//117L
	ERROR_INVALID_CATEGORY,			"ERROR_INVALID_CATEGORY", "The IOCTL call made by the application program is not correct.",
	//118L
	ERROR_INVALID_VERIFY_SWITCH,	"ERROR_INVALID_VERIFY_SWITCH", "The verify-on-write switch parameter value is not correct.",
	//119L
	ERROR_BAD_DRIVER_LEVEL,			"ERROR_BAD_DRIVER_LEVEL", "The system does not support the command requested.",
	//120L
	ERROR_CALL_NOT_IMPLEMENTED,		"ERROR_CALL_NOT_IMPLEMENTED", "This function is not supported on this system.",
	//121L
	ERROR_SEM_TIMEOUT,				"ERROR_SEM_TIMEOUT", "The semaphore timeout period has expired.",
	//122L
	ERROR_INSUFFICIENT_BUFFER,		"ERROR_INSUFFICIENT_BUFFER", "The data area passed to a system call is too small.",
	//123L
	ERROR_INVALID_NAME,				"ERROR_INVALID_NAME", "The filename, directory name, or volume label syntax is incorrect.",
	//124L
	ERROR_INVALID_LEVEL,			"ERROR_INVALID_LEVEL", "The system call level is not correct.",
	//125L
	ERROR_NO_VOLUME_LABEL,			"ERROR_NO_VOLUME_LABEL", "The disk has no volume label.",
	//126L
	ERROR_MOD_NOT_FOUND,			"ERROR_MOD_NOT_FOUND", "The specified module could not be found.",
	//127L
	ERROR_PROC_NOT_FOUND,			"ERROR_PROC_NOT_FOUND", "The specified procedure could not be found.",
	//128L
	ERROR_WAIT_NO_CHILDREN,			"ERROR_WAIT_NO_CHILDREN", "There are no child processes to wait for.",
	//129L
	ERROR_CHILD_NOT_COMPLETE,		"ERROR_CHILD_NOT_COMPLETE", "The application cannot be run in Win32 mode.",
	//130L
	ERROR_DIRECT_ACCESS_HANDLE,		"ERROR_DIRECT_ACCESS_HANDLE", "Attempt to use a file handle to an open disk partition for an operation other than raw disk I/O.",
	//131L
	ERROR_NEGATIVE_SEEK,			"ERROR_NEGATIVE_SEEK", "An attempt was made to move the file pointer before the beginning of the file.",

	//156L
	ERROR_SIGNAL_REFUSED,			"ERROR_SIGNAL_REFUSED", "The recipient process has refused the signal.",
	//157L
	ERROR_DISCARDED,				"ERROR_DISCARDED", "The segment is already discarded and cannot be locked.",
	//158L
	ERROR_NOT_LOCKED,				"ERROR_NOT_LOCKED", "The segment is already unlocked.",
	//159L
	ERROR_BAD_THREADID_ADDR,		"ERROR_BAD_THREADID_ADDR", "The address for the thread ID is not correct.",
	//160L
	ERROR_BAD_ARGUMENTS,			"ERROR_BAD_ARGUMENTS", "The argument string passed to DosExecPgm is not correct.",
	//161L
	ERROR_BAD_PATHNAME,				"ERROR_BAD_PATHNAME", "The specified path is invalid.",
	//162L
	ERROR_SIGNAL_PENDING,			"ERROR_SIGNAL_PENDING", "A signal is already pending.",
	//164L
	ERROR_MAX_THRDS_REACHED,		"ERROR_MAX_THRDS_REACHED", "No more threads can be created in the system.",
	//167L
	ERROR_LOCK_FAILED,				"ERROR_LOCK_FAILED", "Unable to lock a region of a file.",
	//170L
	ERROR_BUSY,						"ERROR_BUSY", "The requested resource is in use.",
	//173L
	ERROR_CANCEL_VIOLATION,			"ERROR_CANCEL_VIOLATION", "A lock request was not outstanding for the supplied cancel region.",
	//174L
	ERROR_ATOMIC_LOCKS_NOT_SUPPORTED, "ERROR_ATOMIC_LOCKS_NOT_SUPPORTED", "The file system does not support atomic changes to the lock type.",
	//180L
	ERROR_INVALID_SEGMENT_NUMBER,	"ERROR_INVALID_SEGMENT_NUMBER", "The system detected a segment number that was not correct.",
	//182L
	ERROR_INVALID_ORDINAL,			"ERROR_INVALID_ORDINAL", "The operating system cannot run that number.",
	//183L
	ERROR_ALREADY_EXISTS,			"ERROR_ALREADY_EXISTS", "Cannot create a file when that file already exists.",

	//232L
	ERROR_NO_DATA,					"ERROR_NO_DATA", "The pipe is being closed.",
	//233L
	ERROR_PIPE_NOT_CONNECTED,		"ERROR_PIPE_NOT_CONNECTED", "No process is on the other end of the pipe.",
	//234L
	ERROR_MORE_DATA,				"ERROR_MORE_DATA", "More data is available.",
	//240L
	ERROR_VC_DISCONNECTED,			"ERROR_VC_DISCONNECTED", "The session was canceled.",
	//254L
	ERROR_EA_LIST_INCONSISTENT,		"ERROR_EA_LIST_INCONSISTENT", "The extended attributes are inconsistent.",
	//258L
	WAIT_TIMEOUT,					"WAIT_TIMEOUT", "The wait operation timed out.",
	//259L
	ERROR_NO_MORE_ITEMS,			"ERROR_NO_MORE_ITEMS", "No more data is available.",
	//266L
	ERROR_CANNOT_COPY,				"ERROR_CANNOT_COPY", "The copy functions cannot be used.",
	//267L
	ERROR_DIRECTORY,				"ERROR_DIRECTORY", "The directory name is invalid.",
	//275L
	ERROR_EAS_DIDNT_FIT,			"ERROR_EAS_DIDNT_FIT", "The extended attributes did not fit in the buffer.",
	//276L
	ERROR_EA_FILE_CORRUPT,			"ERROR_EA_FILE_CORRUPT", "The extended attribute file on the mounted file system is corrupt.",
	//277L
	ERROR_EA_TABLE_FULL,			"ERROR_EA_TABLE_FULL", "The extended attribute table file is full.",
	//278L
	ERROR_INVALID_EA_HANDLE,		"ERROR_INVALID_EA_HANDLE", "The specified extended attribute handle is invalid.",
	//282L
	ERROR_EAS_NOT_SUPPORTED,		"ERROR_EAS_NOT_SUPPORTED", "The mounted file system does not support extended attributes.",
	//288L
	ERROR_NOT_OWNER,				"ERROR_NOT_OWNER", "Attempt to release mutex not owned by caller.",
	//298L
	ERROR_TOO_MANY_POSTS,			"ERROR_TOO_MANY_POSTS", "Too many posts were made to a semaphore.",
	//299L
	ERROR_PARTIAL_COPY,				"ERROR_PARTIAL_COPY", "Only part of a ReadProcessMemory or WriteProcessMemory request was completed.",
	//300L
	ERROR_OPLOCK_NOT_GRANTED,		"ERROR_OPLOCK_NOT_GRANTED", "The oplock request is denied.",
	//301L
	ERROR_INVALID_OPLOCK_PROTOCOL,	"ERROR_INVALID_OPLOCK_PROTOCOL", "An invalid oplock acknowledgment was received by the system.",
	//317L
	ERROR_MR_MID_NOT_FOUND,			"ERROR_MR_MID_NOT_FOUND", "The system cannot find message text for the message number in the message file.",
	//487L
	ERROR_INVALID_ADDRESS,			"ERROR_INVALID_ADDRESS", "Attempt to access invalid address.",
	//534L
	ERROR_ARITHMETIC_OVERFLOW,		"ERROR_ARITHMETIC_OVERFLOW", "Arithmetic result exceeded 32 bits.",
	//535L
	ERROR_PIPE_CONNECTED,			"ERROR_PIPE_CONNECTED", "There is a process on other end of the pipe.",
	//536L
	ERROR_PIPE_LISTENING,			"ERROR_PIPE_LISTENING", "Waiting for a process to open the other end of the pipe.",

	// Begin RAS errors
	//600L
	PENDING,						"PENDING", "An operation is pending.",
	//601L
	ERROR_INVALID_PORT_HANDLE,		"ERROR_INVALID_PORT_HANDLE", "The port handle is invalid.",
	//602L
	ERROR_PORT_ALREADY_OPEN,		"ERROR_PORT_ALREADY_OPEN", "The port is already open.",
	//603L
	ERROR_BUFFER_TOO_SMALL,			"ERROR_BUFFER_TOO_SMALL", "Caller's buffer is too small.",
	//604L
	ERROR_WRONG_INFO_SPECIFIED,		"ERROR_WRONG_INFO_SPECIFIED", "Wrong information specified.",
	//605L
	ERROR_CANNOT_SET_PORT_INFO,		"ERROR_CANNOT_SET_PORT_INFO", "Cannot set port information.",
	//606L
	ERROR_PORT_NOT_CONNECTED,		"ERROR_PORT_NOT_CONNECTED", "The port is not connected.",
	//607L
	ERROR_EVENT_INVALID,			"ERROR_EVENT_INVALID", "The event is invalid.",
	//608L
	ERROR_DEVICE_DOES_NOT_EXIST,	"ERROR_DEVICE_DOES_NOT_EXIST", "The device does not exist.",
	//609L
	ERROR_DEVICETYPE_DOES_NOT_EXIST, "ERROR_DEVICETYPE_DOES_NOT_EXIST", "The device type does not exist.",
	//610L
	ERROR_BUFFER_INVALID,			"ERROR_BUFFER_INVALID", "The buffer is invalid.",
	//611L
	ERROR_ROUTE_NOT_AVAILABLE,		"ERROR_ROUTE_NOT_AVAILABLE", "The route is not available.",
	//612L
	ERROR_ROUTE_NOT_ALLOCATED,		"ERROR_ROUTE_NOT_ALLOCATED", "The route is not allocated.",
	//613L
	ERROR_INVALID_COMPRESSION_SPECIFIED, "ERROR_INVALID_COMPRESSION_SPECIFIED", "Invalid compression specified.",
	//614L
	ERROR_OUT_OF_BUFFERS,			"ERROR_OUT_OF_BUFFERS", "Out of buffers.",
	//615L
	ERROR_PORT_NOT_FOUND,			"ERROR_PORT_NOT_FOUND", "The port was not found.",

	//630L
	ERROR_HARDWARE_FAILURE,			"ERROR_HARDWARE_FAILURE", "The port was disconnected due to hardware failure.",
	//631L
	ERROR_USER_DISCONNECTION,		"ERROR_USER_DISCONNECTION", "The port was disconnected by the user.",
	//632L
	ERROR_INVALID_SIZE,				"ERROR_INVALID_SIZE", "The structure size is incorrect.",
	// End RAS errors


	//994L
	ERROR_EA_ACCESS_DENIED,			"ERROR_EA_ACCESS_DENIED", "Access to the extended attribute was denied.",
	//995L
	ERROR_OPERATION_ABORTED,		"ERROR_OPERATION_ABORTED", "The I/O operation has been aborted because of either a thread exit or an application request.",
	//996L
	ERROR_IO_INCOMPLETE,			"ERROR_IO_INCOMPLETE", "Overlapped I/O event is not in a signaled state.",
	//997L
	ERROR_IO_PENDING,				"ERROR_IO_PENDING", "Overlapped I/O operation is in progress.",
	//998L
	ERROR_NOACCESS,					"ERROR_NOACCESS", "Invalid access to memory location.",
	//999L
	ERROR_SWAPERROR,				"ERROR_SWAPERROR", "Error performing inpage operation.",
	//1001L
	ERROR_STACK_OVERFLOW,			"ERROR_STACK_OVERFLOW", "Recursion too deep; the stack overflowed.",
	//1002L
	ERROR_INVALID_MESSAGE,			"ERROR_INVALID_MESSAGE", "The window cannot act on the sent message.",
	//1003L
	ERROR_CAN_NOT_COMPLETE,			"ERROR_CAN_NOT_COMPLETE", "Cannot complete this function.",
	//1004L
	ERROR_INVALID_FLAGS,			"ERROR_INVALID_FLAGS", "Invalid flags.",
	//1005L
	ERROR_UNRECOGNIZED_VOLUME,		"ERROR_UNRECOGNIZED_VOLUME", "The volume does not contain a recognized file system.",
	//1006L
	ERROR_FILE_INVALID,				"ERROR_FILE_INVALID", "The volume for a file has been externally altered so that the opened file is no longer valid.",
	//1007L
	ERROR_FULLSCREEN_MODE,			"ERROR_FULLSCREEN_MODE", "The requested operation cannot be performed in full-screen mode.",
	//1008L
	ERROR_NO_TOKEN,					"ERROR_NO_TOKEN", "An attempt was made to reference a token that does not exist.",
	//1009L
	ERROR_BADDB,					"ERROR_BADDB", "The configuration registry database is corrupt.",
	//1010L
	ERROR_BADKEY,					"ERROR_BADKEY", "The configuration registry key is invalid.",
	//1011L
	ERROR_CANTOPEN,					"ERROR_CANTOPEN", "The configuration registry key could not be opened.",
	//1012L
	ERROR_CANTREAD,					"ERROR_CANTREAD", "The configuration registry key could not be read.",
	//1013L
	ERROR_CANTWRITE,				"ERROR_CANTWRITE", "The configuration registry key could not be written.",
	//1014L
	ERROR_REGISTRY_RECOVERED,		"ERROR_REGISTRY_RECOVERED", "One of the files in the registry database had to be recovered by use of a log or alternate copy. The recovery was successful.",

	//1109L
	ERROR_UNABLE_TO_UNLOAD_MEDIA,	"ERROR_UNABLE_TO_UNLOAD_MEDIA", "Unable to unload the media.",
	//1110L
	ERROR_MEDIA_CHANGED,			"ERROR_MEDIA_CHANGED", "The media in the drive may have changed.",
	//1111L
	ERROR_BUS_RESET,				"ERROR_BUS_RESET", "The I/O bus was reset.",
	//1112L
	ERROR_NO_MEDIA_IN_DRIVE,		"ERROR_NO_MEDIA_IN_DRIVE", "No media in drive.",
	//1113L
	ERROR_NO_UNICODE_TRANSLATION,	"ERROR_NO_UNICODE_TRANSLATION", "No mapping for the Unicode character exists in the target multi-byte code page.",
	//1114L
	ERROR_DLL_INIT_FAILED,			"ERROR_DLL_INIT_FAILED", "A dynamic link library (DLL) initialization routine failed.",
	//1115L
	ERROR_SHUTDOWN_IN_PROGRESS,		"ERROR_SHUTDOWN_IN_PROGRESS", "A system shutdown is in progress.",
	//1116L
	ERROR_NO_SHUTDOWN_IN_PROGRESS,	"ERROR_NO_SHUTDOWN_IN_PROGRESS", "Unable to abort the system shutdown because no shutdown was in progress.",
	//1117L
	ERROR_IO_DEVICE,				"ERROR_IO_DEVICE", "The request could not be performed because of an I/O device error.",
	//1118L
	ERROR_SERIAL_NO_DEVICE,			"ERROR_SERIAL_NO_DEVICE", "No serial device was successfully initialized. The serial driver will unload.",
	//1119L
	ERROR_IRQ_BUSY,					"ERROR_IRQ_BUSY", "Unable to open a device that was sharing an interrupt request (IRQ) with other devices. At least one other device that uses that IRQ was already opened.",
	//1120L
	ERROR_MORE_WRITES,				"ERROR_MORE_WRITES", "A serial I/O operation was completed by another write to the serial port (the IOCTL_SERIAL_XOFF_COUNTER reached zero).",

	//1156L
	ERROR_DDE_FAIL,					"ERROR_DDE_FAIL", "An error occurred in sending the command to the application.",
	//1157L
	ERROR_DLL_NOT_FOUND,			"ERROR_DLL_NOT_FOUND", "One of the library files needed to run this application cannot be found.",
	//1158L
	ERROR_NO_MORE_USER_HANDLES,		"ERROR_NO_MORE_USER_HANDLES", "The current process has used all of its system allowance of handles for Window Manager objects.",
	//1159L
	ERROR_MESSAGE_SYNC_ONLY,		"ERROR_MESSAGE_SYNC_ONLY", "The message can be used only with synchronous operations.",
	//1160L
	ERROR_SOURCE_ELEMENT_EMPTY,		"ERROR_SOURCE_ELEMENT_EMPTY", "The indicated source element has no media.",
	//1161L
	ERROR_DESTINATION_ELEMENT_FULL,	"ERROR_DESTINATION_ELEMENT_FULL", "The indicated destination element already contains media.",
	//1162L
	ERROR_ILLEGAL_ELEMENT_ADDRESS,	"ERROR_ILLEGAL_ELEMENT_ADDRESS", "The indicated element does not exist.",
	//1163L
	ERROR_MAGAZINE_NOT_PRESENT,		"ERROR_MAGAZINE_NOT_PRESENT", "The indicated element is part of a magazine that is not present.",
	//1164L
	ERROR_DEVICE_REINITIALIZATION_NEEDED, "ERROR_DEVICE_REINITIALIZATION_NEEDED", "The indicated device requires reinitialization due to hardware errors.",
	//1165L
	ERROR_DEVICE_REQUIRES_CLEANING,	"ERROR_DEVICE_REQUIRES_CLEANING", "The device has indicated that cleaning is required before further operations are attempted.",
	//1166L
	ERROR_DEVICE_DOOR_OPEN,			"ERROR_DEVICE_DOOR_OPEN", "The device has indicated that its door is open.",
	//1167L
	ERROR_DEVICE_NOT_CONNECTED,		"ERROR_DEVICE_NOT_CONNECTED", "The device is not connected.",
	//1168L
	ERROR_NOT_FOUND,				"ERROR_NOT_FOUND", "Element not found.",
	//1169L
	ERROR_NO_MATCH,					"ERROR_NO_MATCH", "There was no match for the specified key in the index.",
	//1170L
	ERROR_SET_NOT_FOUND,			"ERROR_SET_NOT_FOUND", "The property set specified does not exist on the object.",
	//1171L
	ERROR_POINT_NOT_FOUND,			"ERROR_POINT_NOT_FOUND", "The point passed to GetMouseMovePoints is not in the buffer.",

	//1400L
	ERROR_INVALID_WINDOW_HANDLE,	"ERROR_INVALID_WINDOW_HANDLE", "Invalid window handle.",
	//1401L
	ERROR_INVALID_MENU_HANDLE,		"ERROR_INVALID_MENU_HANDLE", "Invalid menu handle.",
	//1402L
	ERROR_INVALID_CURSOR_HANDLE,	"ERROR_INVALID_CURSOR_HANDLE", "Invalid cursor handle.",
	//1403L
	ERROR_INVALID_ACCEL_HANDLE,		"ERROR_INVALID_ACCEL_HANDLE", "Invalid accelerator table handle.",
	//1404L
	ERROR_INVALID_HOOK_HANDLE,		"ERROR_INVALID_HOOK_HANDLE", "Invalid hook handle.",
	//1405L
	ERROR_INVALID_DWP_HANDLE,		"ERROR_INVALID_DWP_HANDLE", "Invalid handle to a multiple-window position structure.",
	//1406L
	ERROR_TLW_WITH_WSCHILD,			"ERROR_TLW_WITH_WSCHILD", "Cannot create a top-level child window.",
	//1407L
	ERROR_CANNOT_FIND_WND_CLASS,	"ERROR_CANNOT_FIND_WND_CLASS", "Cannot find window class.",
	//1408L
	ERROR_WINDOW_OF_OTHER_THREAD,	"ERROR_WINDOW_OF_OTHER_THREAD", "Invalid window; it belongs to other thread.",

	//1417L
	ERROR_NO_WILDCARD_CHARACTERS,	"ERROR_NO_WILDCARD_CHARACTERS", "No wildcards were found.",
	//1418L
	ERROR_CLIPBOARD_NOT_OPEN,		"ERROR_CLIPBOARD_NOT_OPEN", "Thread does not have a clipboard open.",
	//1419L
	ERROR_HOTKEY_NOT_REGISTERED,	"ERROR_HOTKEY_NOT_REGISTERED", "Hot key is not registered.",
	//1420L
	ERROR_WINDOW_NOT_DIALOG,		"ERROR_WINDOW_NOT_DIALOG", "The window is not a valid dialog window.",
	//1421L
	ERROR_CONTROL_ID_NOT_FOUND,		"ERROR_CONTROL_ID_NOT_FOUND", "Control ID not found.",
	//1422L
	ERROR_INVALID_COMBOBOX_MESSAGE,	"ERROR_INVALID_COMBOBOX_MESSAGE", "Invalid message for a combo box because it does not have an edit control.",

	//1454L
	ERROR_PAGEFILE_QUOTA,			"ERROR_PAGEFILE_QUOTA", "Insufficient quota to complete the requested service.",
	//1455L
	ERROR_COMMITMENT_LIMIT,			"ERROR_COMMITMENT_LIMIT", "The paging file is too small for this operation to complete.",
	//1456L
	ERROR_MENU_ITEM_NOT_FOUND,		"ERROR_MENU_ITEM_NOT_FOUND", "A menu item was not found.",
	//1457L
	ERROR_INVALID_KEYBOARD_HANDLE,	"ERROR_INVALID_KEYBOARD_HANDLE", "Invalid keyboard layout handle.",
	//1458L
	ERROR_HOOK_TYPE_NOT_ALLOWED,	"ERROR_HOOK_TYPE_NOT_ALLOWED", "Hook type not allowed.",

	//1812L
	ERROR_RESOURCE_DATA_NOT_FOUND,	"ERROR_RESOURCE_DATA_NOT_FOUND", "The specified image file did not contain a resource section.",
	//1813L
	ERROR_RESOURCE_TYPE_NOT_FOUND,	"ERROR_RESOURCE_TYPE_NOT_FOUND", "The specified resource type cannot be found in the image file.",
	//1814L
	ERROR_RESOURCE_NAME_NOT_FOUND,	"ERROR_RESOURCE_NAME_NOT_FOUND", "The specified resource name cannot be found in the image file.",
	//1815L
	ERROR_RESOURCE_LANG_NOT_FOUND,	"ERROR_RESOURCE_LANG_NOT_FOUND", "The specified resource language ID cannot be found in the image file.",

	// Begin WMI errors
	//4200L
	ERROR_WMI_GUID_NOT_FOUND,		"ERROR_WMI_GUID_NOT_FOUND", "The GUID passed was not recognized as valid by a WMI data provider.",
	//4201L
	ERROR_WMI_INSTANCE_NOT_FOUND,	"ERROR_WMI_INSTANCE_NOT_FOUND", "The instance name passed was not recognized as valid by a WMI data provider.",
	//4202L
	ERROR_WMI_ITEMID_NOT_FOUND,		"ERROR_WMI_ITEMID_NOT_FOUND", "The data item ID passed was not recognized as valid by a WMI data provider.",
	//4203L
	ERROR_WMI_TRY_AGAIN,			"ERROR_WMI_TRY_AGAIN", "The WMI request could not be completed and should be retried.",
	//4204L
	ERROR_WMI_DP_NOT_FOUND,			"ERROR_WMI_DP_NOT_FOUND", "The WMI data provider could not be located.",
	//4205L
	ERROR_WMI_UNRESOLVED_INSTANCE_REF, "ERROR_WMI_UNRESOLVED_INSTANCE_REF", "The WMI data provider references an instance set that has not been registered.",

	// End WMI errors


#ifndef NO_ERRORS_WINSOCK
	// Begin WinSock errors
	//10004L
	WSAEINTR,						"WSAEINTR", "Interrupted function call.",
	//10009L
	WSAEBADF,						"WSAEBADF", "The file handle supplied is invalid.",
	//10013L
	WSAEACCES,						"WSAEACCES", "An attempt was made to access a socket in a way forbidden by its access permissions.",
	//10014L
	WSAEFAULT,						"WSAEFAULT", "The system detected an invalid pointer address in attempting to use a pointer argument in a call.",
	//10022L
	WSAEINVAL,						"WSAEINVAL", "An invalid argument was supplied.",
	//10024L
	WSAEMFILE,						"WSAEMFILE", "Too many open sockets.",

	//10035L
	WSAEWOULDBLOCK,					"WSAEWOULDBLOCK", "A non-blocking socket operation could not be completed immediately.",
	//10036L
	WSAEINPROGRESS,					"WSAEINPROGRESS", "A blocking operation is currently executing.",
	//10037L
	WSAEALREADY,					"WSAEALREADY", "An operation was attempted on a non-blocking socket that already had an operation in progress.",
	//10038L
	WSAENOTSOCK,					"WSAENOTSOCK", "Socket operation on a non-socket.",
	//10039L
	WSAEDESTADDRREQ,				"WSAEDESTADDRREQ", "A required address was omitted from an operation on a socket.",
	//10040L
	WSAEMSGSIZE,					"WSAEMSGSIZE", "Message too long.",
	//10041L
	WSAEPROTOTYPE,					"WSAEPROTOTYPE", "A protocol was specified in the socket function call that does not support the semantics of the socket type requested.",
	//10042L
	WSAENOPROTOOPT,					"WSAENOPROTOOPT", "An unknown, invalid, or unsupported option or level was specified in a getsockopt or setsockopt call.",
	//10043L
	WSAEPROTONOSUPPORT,				"WSAEPROTONOSUPPORT", "Invalid flags.",
	//10044L
	WSAESOCKTNOSUPPORT,				"WSAESOCKTNOSUPPORT", "The support for the specified socket type does not exist in this address family.",
	//10045L
	WSAEOPNOTSUPP,					"WSAEOPNOTSUPP", "Operation not supported.",
	//10046L
	WSAEPFNOSUPPORT,				"WSAEPFNOSUPPORT", "The protocol family has not been configured into the system or no implementation for it exists.",
	//10047L
	WSAEAFNOSUPPORT,				"WSAEAFNOSUPPORT", "Address family not supported by protocol family.",
	//10048L
	WSAEADDRINUSE,					"WSAEADDRINUSE", "Address already in use.",
	//10049L
	WSAEADDRNOTAVAIL,				"WSAEADDRNOTAVAIL", "Cannot assign requested address.",
	//10050L
	WSAENETDOWN,					"WSAENETDOWN", "Network is down.",
	//10051L
	WSAENETUNREACH,					"WSAENETUNREACH", "Network is unreachable.",
	//10052L
	WSAENETRESET,					"WSAENETRESET", "Network dropped connection on reset.",
	//10053L
	WSAECONNABORTED,				"WSAECONNABORTED", "Software caused connection abort.",
	//10054L
	WSAECONNRESET,					"WSAECONNRESET", "Connection reset by peer.",
	//10055L
	WSAENOBUFS,						"WSAENOBUFS", "No buffer space available.",
	//10056L
	WSAEISCONN,						"WSAEISCONN", "Socket is already connected.",
	//10057L
	WSAENOTCONN,					"WSAENOTCONN", "Socket is not connected.",
	//10058L
	WSAESHUTDOWN,					"WSAESHUTDOWN", "Cannot send after socket shutdown.",
	//10059L
	WSAETOOMANYREFS,				"WSAETOOMANYREFS", "Too many references to some kernel object.",
	//10060L
	WSAETIMEDOUT,					"WSAETIMEDOUT", "Connection timed out.",
	//10061L
	WSAECONNREFUSED,				"WSAECONNREFUSED", "Connection refused.",
	//10062L
	WSAELOOP,						"WSAELOOP", "Cannot translate name.",
	//10063L
	WSAENAMETOOLONG,				"WSAENAMETOOLONG", "Name component or name was too long.",
	//10064L
	WSAEHOSTDOWN,					"WSAEHOSTDOWN", "A socket operation failed because the destination host was down.",
	//10065L
	WSAEHOSTUNREACH,				"WSAEHOSTUNREACH", "A socket operation was attempted to an unreachable host.",
	//10066L
	WSAENOTEMPTY,					"WSAENOTEMPTY", "Cannot remove a directory that is not empty.",
	//10067L
	WSAEPROCLIM,					"WSAEPROCLIM", "A Windows Sockets implementation may have a limit on the number of applications that may use it simultaneously.",
	//10068L
	WSAEUSERS,						"WSAEUSERS", "Ran out of quota.",
	//10069L
	WSAEDQUOT,						"WSAEDQUOT", "Ran out of disk quota.",
	//10070L
	WSAESTALE,						"WSAESTALE", "File handle reference is no longer available.",
	//10071L
	WSAEREMOTE,						"WSAEREMOTE", "Item is not available locally.",

	//10091L
	WSASYSNOTREADY,					"WSASYSNOTREADY", "WSAStartup cannot function at this time because the underlying system it uses to provide network services is currently unavailable.",
	//10092L
	WSAVERNOTSUPPORTED,				"WSAVERNOTSUPPORTED", "The Windows Sockets version requested is not supported.",
	//10093L
	WSANOTINITIALISED,				"WSANOTINITIALISED", "Either the application has not called WSAStartup, or WSAStartup failed.",

	//10101L
	WSAEDISCON,						"WSAEDISCON", NULL,

	//10112L
	WSAEREFUSED,					"WSAEREFUSED", "A database query failed because it was actively refused.",

	//11001L
	WSAHOST_NOT_FOUND,				"WSAHOST_NOT_FOUND", "No such host is known.",
	//11002L
	WSATRY_AGAIN,					"WSATRY_AGAIN", "This is usually a temporary error during hostname resolution and means that the local server did not receive a response from an authoritative server.",
	//11003L
	WSANO_RECOVERY,					"WSANO_RECOVERY", "A non-recoverable error occurred during a database lookup.",
	//11004L
	WSANO_DATA,						"WSANO_DATA", "The requested name is valid and was found in the database, but it does not have the correct associated data being resolved for.",

	//11015L
	WSA_QOS_GENERIC_ERROR,			"WSA_QOS_GENERIC_ERROR", "General QOS error.",
	// End WinSock errors
#endif // ! NO_ERRORS_WINSOCK


#ifndef _XBOX // no TAPI supported
	// Begin TAPI errors
	//0x80000001L
	LINEERR_ALLOCATED,				"LINEERR_ALLOCATED", NULL,
	//0x80000002L
	LINEERR_BADDEVICEID,			"LINEERR_BADDEVICEID", NULL,
	//0x80000003L
	LINEERR_BEARERMODEUNAVAIL,		"LINEERR_BEARERMODEUNAVAIL", NULL,
	//0x80000005L
	LINEERR_CALLUNAVAIL,			"LINEERR_CALLUNAVAIL", NULL,
	//0x80000006L
	LINEERR_COMPLETIONOVERRUN,		"LINEERR_COMPLETIONOVERRUN", NULL,
	//0x80000007L
	LINEERR_CONFERENCEFULL,			"LINEERR_CONFERENCEFULL", NULL,
	//0x80000008L
	LINEERR_DIALBILLING,			"LINEERR_DIALBILLING", NULL,
	//0x80000009L
	LINEERR_DIALDIALTONE,			"LINEERR_DIALDIALTONE", NULL,
	//0x8000000BL
	LINEERR_DIALQUIET,				"LINEERR_DIALQUIET", NULL,
	//0x8000000CL
	LINEERR_INCOMPATIBLEAPIVERSION, "LINEERR_INCOMPATIBLEAPIVERSION", NULL,
	//0x8000000DL
	LINEERR_INCOMPATIBLEEXTVERSION, "LINEERR_INCOMPATIBLEEXTVERSION", NULL,
	//0x80000048L
	LINEERR_OPERATIONFAILED,		"LINEERR_OPERATIONFAILED", NULL,
	//0x80000049L
	LINEERR_OPERATIONUNAVAIL,		"LINEERR_OPERATIONUNAVAIL", NULL,
	// End TAPI errors
#endif // ! XBOX

	//0x8000000AL
	// LINEERR_DIALPROMPT
	// DPERR_PENDING
	// DVERR_PENDING
	// DPNERR_PENDING
	E_PENDING,						"E_PENDING, LINEERR_DIALPROMPT", "Not an error, this return indicates that an asynchronous command has reached the point where it is successfully queued.",

	//0x80010100L
	RPC_E_SYS_CALL_FAILED,			"RPC_E_SYS_CALL_FAILED", "System call failed.",
	//0x80010101L
	RPC_E_OUT_OF_RESOURCES,			"RPC_E_OUT_OF_RESOURCES", "Could not allocate some required resource (memory, events, ...).",
	//0x80010102L
	RPC_E_ATTEMPTED_MULTITHREAD,	"RPC_E_ATTEMPTED_MULTITHREAD", "Attempted to make calls on more than one thread in single threaded mode.",
	//0x80010103L
	RPC_E_NOT_REGISTERED,			"RPC_E_NOT_REGISTERED", "The requested interface is not registered on the server object.",
	//0x80010104L
	RPC_E_FAULT,					"RPC_E_FAULT", "RPC could not call the server or could not return the results of calling the server.",
	//0x80010105L
	RPC_E_SERVERFAULT,				"RPC_E_SERVERFAULT", "The server threw an exception.",
	//0x80010106L
	RPC_E_CHANGED_MODE,				"RPC_E_CHANGED_MODE", "Cannot change thread mode after it is set.",
	//0x80010107L
	RPC_E_INVALIDMETHOD,			"RPC_E_INVALIDMETHOD", "The method called does not exist on the server.",
	//0x80010108L
	RPC_E_DISCONNECTED,				"RPC_E_DISCONNECTED", "The object invoked has disconnected from its clients.",
	//0x80010109L
	RPC_E_RETRY,					"RPC_E_RETRY", "The object invoked chose not to process the call now.  Try again later.",
	//0x8001010AL
	RPC_E_SERVERCALL_RETRYLATER,	"RPC_E_SERVERCALL_RETRYLATER", "The message filter indicated that the application is busy.",
	//0x8001010BL
	RPC_E_SERVERCALL_REJECTED,		"RPC_E_SERVERCALL_REJECTED", "The message filter rejected the call.",
	//0x8001010CL
	RPC_E_INVALID_CALLDATA,			"RPC_E_INVALID_CALLDATA", "A call control interfaces was called with invalid data.",
	//0x8001010DL
	RPC_E_CANTCALLOUT_ININPUTSYNCCALL, "RPC_E_CANTCALLOUT_ININPUTSYNCCALL", "An outgoing call cannot be made since the application is dispatching an input-synchronous call.",


	//0x80004001L
	// DPERR_UNSUPPORTED,
	// DVERR_UNSUPPORTED,
	// DPNERR_UNSUPPORTED,
	E_NOTIMPL,						"E_NOTIMPL, DPERR_UNSUPPORTED", "The function or feature is not available in this implementation.",
	//0x80004002L
	// DPERR_NOINTERFACE,
	// DVERR_NOINTERFACE,
	// DPNERR_NOINTERFACE,
	E_NOINTERFACE,					"E_NOINTERFACE", "No such interface supported.",

	//0x80004003L
	// DVERR_INVALIDPOINTER
	E_POINTER,						"E_POINTER", "Invalid pointer.",

	//0x80004005L
	// DPERR_GENERIC
	// DPNERR_GENERIC
	E_FAIL,							"E_FAIL, DPERR_GENERIC", "An undefined error condition occurred.",


	//0x80040110L
	CLASS_E_NOAGGREGATION,			"CLASS_E_NOAGGREGATION", "Class does not support aggregation (or class object is remote).",
	//0x80040111L
	CLASS_E_CLASSNOTAVAILABLE,		"CLASS_E_CLASSNOTAVAILABLE", "ClassFactory cannot supply requested class.",
	//0x80040112L
	CLASS_E_NOTLICENSED,			"CLASS_E_NOTLICENSED", "Class is not licensed for use.",

	//0x80040154L
	REGDB_E_CLASSNOTREG,			"REGDB_E_CLASSNOTREG", "Class not registered.",
	//0x80040155L
	REGDB_E_IIDNOTREG,				"REGDB_E_IIDNOTREG", "Interface not registered.",

	//0x80070005L
	E_ACCESSDENIED,					"E_ACCESSDENIED", NULL,
	//0x80070006L
	E_HANDLE,						"E_HANDLE", "Invalid handle.",

	//0x8007000EL
	// DPERR_NOMEMORY
	// DPERR_OUTOFMEMORY
	// DVERR_OUTOFMEMORY
	// DPNERR_OUTOFMEMORY
	E_OUTOFMEMORY,					"E_OUTOFMEMORY", "There is insufficient memory to perform the requested operation.",

	//0x80070057L
	// E_INVALIDARG
	// DPERR_INVALIDPARAM
	// DPERR_INVALIDPARAMS
	// DVERR_INVALIDPARAM
	// DPNERR_INVALIDPARAM
	E_INVALIDARG,					"E_INVALIDARG", "One or more arguments is invalid.",

	//0x8007007EL
	// ERROR_MOD_NOT_FOUND
	(0x80070000 + ERROR_MOD_NOT_FOUND), "ERROR_MOD_NOT_FOUND", "The specified module could not be found.",

	//0x800703E6L
	0x800703E6,						"0x800703E6?", "Seen when CoCreating an object not \"ThreadingModel=Both\" after CoInitializeEx.",


#ifndef NO_ERRORS_SECURITY
	//0x80090001L
	NTE_BAD_UID,					"NTE_BAD_UID", "Bad UID.",
	//0x80090002L
	NTE_BAD_HASH,					"NTE_BAD_HASH", "Bad hash.",
	//0x80090003L
	NTE_BAD_KEY,					"NTE_BAD_KEY", "Bad key.",
	//0x80090004L
	NTE_BAD_LEN,					"NTE_BAD_LEN", "Bad length.",
	//0x80090005L
	NTE_BAD_DATA,					"NTE_BAD_DATA", "Bad data.",
	//0x80090006L
	NTE_BAD_SIGNATURE,				"NTE_BAD_SIGNATURE", "Invalid signature.",
	//0x80090007L
	NTE_BAD_VER,					"NTE_BAD_VER", "Bad version of provider.",
	//0x80090008L
	NTE_BAD_ALGID,					"NTE_BAD_ALGID", "Invalid algorithm specified.",
	//0x80090009L
	NTE_BAD_FLAGS,					"NTE_BAD_FLAGS", "Invalid flags specified.",
	//0x8009000AL
	NTE_BAD_TYPE,					"NTE_BAD_TYPE", "Invalid type specified.",
	//0x8009000BL
	NTE_BAD_KEY_STATE,				"NTE_BAD_KEY_STATE", "Key not valid for use in specified state.",
	//0x8009000CL
	NTE_BAD_HASH_STATE,				"NTE_BAD_HASH_STATE", "Hash not valid for use in specified state.",
	//0x8009000DL
	NTE_NO_KEY,						"NTE_NO_KEY", "Key does not exist.",
	//0x8009000EL
	NTE_NO_MEMORY,					"NTE_NO_MEMORY", "Insufficient memory available for the operation.",
	//0x8009000FL
	NTE_EXISTS,						"NTE_EXISTS", "Object already exists.",
	//0x80090010L
	NTE_PERM,						"NTE_PERM", "Access denied.",
	//0x80090011L
	NTE_NOT_FOUND,					"NTE_NOT_FOUND", "Object was not found.",
	//0x80090012L
	NTE_DOUBLE_ENCRYPT,				"NTE_DOUBLE_ENCRYPT", "Data already encrypted.",
	//0x80090013L
	NTE_BAD_PROVIDER,				"NTE_BAD_PROVIDER", "Invalid provider specified.",
	//0x80090014L
	NTE_BAD_PROV_TYPE,				"NTE_BAD_PROV_TYPE", "Invalid provider type specified.",
	//0x80090015L
	NTE_BAD_PUBLIC_KEY,				"NTE_BAD_PUBLIC_KEY", "Provider's public key is invalid.",
	//0x80090016L
	NTE_BAD_KEYSET,					"NTE_BAD_KEYSET", "Keyset does not exist.",
	//0x80090017L
	NTE_PROV_TYPE_NOT_DEF,			"NTE_PROV_TYPE_NOT_DEF", "Provider type not defined.",
	//0x80090018L
	NTE_PROV_TYPE_ENTRY_BAD,		"NTE_PROV_TYPE_ENTRY_BAD", "Provider type as registered is invalid.",

	//0x8009030CL
	SEC_E_LOGON_DENIED,				"SEC_E_LOGON_DENIED", "The logon attempt failed.",
	//0x8009030DL
	SEC_E_UNKNOWN_CREDENTIALS,		"SEC_E_UNKNOWN_CREDENTIALS", "The credentials supplied to the package were not recognized.",
	//0x8009030EL
	SEC_E_NO_CREDENTIALS,			"SEC_E_NO_CREDENTIALS", "No credentials are available in the security package.",
	//0x8009030FL
	SEC_E_MESSAGE_ALTERED,			"SEC_E_MESSAGE_ALTERED", "The message supplied for verification has been altered.",
	//0x80090310L
	SEC_E_OUT_OF_SEQUENCE,			"SEC_E_OUT_OF_SEQUENCE", "The message supplied for verification is out of sequence.",
	//0x80090311L
	SEC_E_NO_AUTHENTICATING_AUTHORITY, "SEC_E_NO_AUTHENTICATING_AUTHORITY", "No authority could be contacted for authentication.",
	//0x80090312L
	SEC_I_CONTINUE_NEEDED,			"SEC_I_CONTINUE_NEEDED", "The function completed successfully, but must be called again to complete the context.",
	//0x80090313L
	SEC_I_COMPLETE_NEEDED,			"SEC_I_COMPLETE_NEEDED", "The function completed successfully, but CompleteToken must be called.",
#endif // ! NO_ERRORS_SECURITY

	E_UNEXPECTED,					"E_UNEXPECTED", NULL,
	CO_E_NOTINITIALIZED,			"CO_E_NOTINITIALIZED", NULL,
	CLASS_E_NOAGGREGATION,			"CLASS_E_NOAGGREGATION", NULL,
	ERROR_NOT_LOCKED,				"ERROR_NOT_LOCKED", NULL,
	ERROR_DISK_FULL,				"ERROR_DISK_FULL", NULL,
	ERROR_ACCESS_DENIED,			"ERROR_ACCESS_DENIED", NULL,
	ERROR_FILE_EXISTS,				"ERROR_FILE_EXISTS", NULL,
	ERROR_INVALID_NAME,				"ERROR_INVALID_NAME", "The filename, directory name, or volume label syntax is incorrect.",
	ERROR_UNEXP_NET_ERR,			"ERROR_UNEXP_NET_ERR", "An unexpected network error occurred.",


#ifndef NO_ERRORS_DPLAY
	// Begin DPlay errors
	DPERR_ACCESSDENIED,				"DPERR_ACCESSDENIED", "The session is full, or an incorrect password was supplied.",
	DPERR_ACTIVEPLAYERS,			"DPERR_ACTIVEPLAYERS", "The requested operation cannot be performed because there are existing active players.",
	DPERR_ALREADYINITIALIZED,		"DPERR_ALREADYINITIALIZED", "This object is already initialized.",
	DPERR_APPNOTSTARTED,			"DPERR_APPNOTSTARTED", "The application has not been started yet.",
	DPERR_BUFFERTOOLARGE,			"DPERR_BUFFERTOOLARGE", "The data buffer is too large to store.",
	DPERR_BUFFERTOOSMALL,			"DPERR_BUFFERTOOSMALL", "The supplied buffer is not large enough to contain the requested data.",
	DPERR_BUSY,						"DPERR_BUSY", "A message cannot be sent because the transmission medium is busy.",
	DPERR_CANCELFAILED,				"DPERR_CANCELFAILED", "The message could not be canceled, possibly because it is a group message that has already been to sent to one or more members of the group.",
	DPERR_CANTADDPLAYER,			"DPERR_CANTADDPLAYER", "The player cannot be added to the session.",
	DPERR_CANTCREATEGROUP,			"DPERR_CANTCREATEGROUP", "A new group cannot be created.",
	DPERR_CANTCREATEPLAYER,			"DPERR_CANTCREATEPLAYER", "A new player cannot be created.",
	DPERR_CANTCREATEPROCESS,		"DPERR_CANTCREATEPROCESS", "Cannot start the application.",
	DPERR_CANTCREATESESSION,		"DPERR_CANTCREATESESSION", "A new session cannot be created.",
	DPERR_CANTLOADCAPI,				"DPERR_CANTLOADCAPI", "No credentials were supplied and the CryptoAPI package (CAPI) to use for cryptography services cannot be loaded.",
	DPERR_CANTLOADSECURITYPACKAGE,	"DPERR_CANTLOADSECURITYPACKAGE", "The software security package cannot be loaded.",
	DPERR_CANTLOADSSPI,				"DPERR_CANTLOADSSPI", "No credentials were supplied, and the Security Support Provider Interface (SSPI) that will prompt for credentials cannot be loaded.",
	DPERR_CAPSNOTAVAILABLEYET,		"DPERR_CAPSNOTAVAILABLEYET", "The capabilities of the DirectPlay object have not been determined yet. This error will occur if the DirectPlay object is implemented on a connectivity solution that requires polling to determine available bandwidth and latency.",
	DPERR_CONNECTING,				"DPERR_CONNECTING", "The method is in the process of connecting to the network. The application should keep using the method until it returns DP_OK, indicating successful completion, or until it returns a different error.",
	DPERR_CONNECTIONLOST,			"DPERR_CONNECTIONLOST", "The service provider connection was reset while data was being sent.",
	DPERR_ENCRYPTIONFAILED,			"DPERR_ENCRYPTIONFAILED", "The requested information could not be digitally encrypted. Encryption is used for message privacy. This error is only relevant in a secure session.",
	DPERR_EXCEPTION,				"DPERR_EXCEPTION", "An exception occurred when processing the request.",
	DPERR_INVALIDFLAGS,				"DPERR_INVALIDFLAGS", "The flags passed to this method are invalid.",
	DPERR_INVALIDGROUP,				"DPERR_INVALIDGROUP", "The group ID is not recognized as a valid group ID for this game session.",
	DPERR_INVALIDINTERFACE,			"DPERR_INVALIDINTERFACE", "The interface parameter is invalid.",
	DPERR_INVALIDOBJECT,			"DPERR_INVALIDOBJECT", "The DirectPlay object pointer is invalid.",
	DPERR_INVALIDPASSWORD,			"DPERR_INVALIDPASSWORD", "An invalid password was supplied when attempting to join a session that requires a password'",
	DPERR_INVALIDPLAYER,			"DPERR_INVALIDPLAYER", "The player ID is not recognized as a valid player ID for this game session.",
	DPERR_INVALIDPRIORITY,			"DPERR_INVALIDPRIORITY", "The specified priority is not within the range of allowed priorities, which is inclusively 0-65535.",
	DPERR_LOGONDENIED,				"DPERR_LOGONDENIED", "The session could not be opened because credentials are required, and either no credentials were supplied, or the credentials were invalid.",
	DPERR_NOCAPS,					"DPERR_NOCAPS", "The communication link that DirectPlay is attempting to use is not capable of this function.",
	DPERR_NOCONNECTION,				"DPERR_NOCONNECTION", "No communication link was established.",
	DPERR_NOMESSAGES,				"DPERR_NOMESSAGES", "There are no messages in the receive queue.",
	DPERR_NONAMESERVERFOUND,		"DPERR_NONAMESERVERFOUND", "No name server (host) could be found or created. A host must exist to create a player.",
	DPERR_NONEWPLAYERS,				"DPERR_NONEWPLAYERS", "The session is not accepting any new players.",
	DPERR_NOPLAYERS,				"DPERR_NOPLAYERS", "There are no active players in the session.",
	DPERR_NOSESSIONS,				"DPERR_NOSESSIONS", "There are no existing sessions for this game.",
	DPERR_NOTLOBBIED,				"DPERR_NOTLOBBIED", "Returned by the IDirectPlayLobby3::Connect method if the application was not started by using the IDirectPlayLobby3::RunApplication method, or if there is no DPLCONNECTION structure currently initialized for this DirectPlayLobby object.",
	DPERR_NOTLOGGEDIN,				"DPERR_NOTLOGGEDIN", "An action cannot be performed because a player or client application is not logged in. Returned by the Send method when the client application tries to send a secure message without being logged in.",
	DPERR_PLAYERLOST,				"DPERR_PLAYERLOST", "A player has lost the connection to the session.",
	DPERR_SENDTOOBIG,				"DPERR_SENDTOOBIG", "The message being sent by the IDirectPlay4::Send method is too large.",
	DPERR_SESSIONLOST,				"DPERR_SESSIONLOST", "The connection to the session has been lost.",
	DPERR_SIGNFAILED,				"DPERR_SIGNFAILED", "The requested information could not be digitally signed. Digital signatures are used to establish the authenticity of messages.",
	DPERR_TIMEOUT,					"DPERR_TIMEOUT", "The operation could not be completed in the specified time.",
	DPERR_UNAVAILABLE,				"DPERR_UNAVAILABLE", "The requested function is not available at this time.",
	DPERR_UNINITIALIZED,			"DPERR_UNINITIALIZED", "The requested object has not been initialized.",
	DPERR_UNKNOWNAPPLICATION,		"DPERR_UNKNOWNAPPLICATION", "An unknown application was specified.",
	DPERR_UNKNOWNMESSAGE,			"DPERR_UNKNOWNMESSAGE", "The message ID isn't valid. Returned from IDirectPlay4::CancelMessage if the ID of the message to be cancelled is invalid.",
	DPERR_USERCANCEL,				"DPERR_USERCANCEL", "The user canceled the connection process during a call to the Open method or the user clicked Cancel in one of the DirectPlay service provider dialog boxes during a call to EnumSessions.",

	// BUGBUG what is this error?
	DPERR_CANNOTCREATESERVER,		"DPERR_CANNOTCREATESERVER", NULL,
	// End DPlay errors
#endif // ! NO_ERRORS_DPLAY

#ifndef NO_ERRORS_DPLAY8
	// Begin DirectPlay8 errors

	//0x00158000 + 5L
	DPNSUCCESS_EQUAL,				"DPNSUCCESS_EQUAL", NULL,
	//0x00158000 + 10L
	DPNSUCCESS_NOTEQUAL,			"DPNSUCCESS_NOTEQUAL", NULL,

	//0x80158030
	DPNERR_ABORTED,					"DPNERR_ABORTED", NULL,
	//0x80158040
	DPNERR_ADDRESSING,				"DPNERR_ADDRESSING", NULL,
	//0x80158050
	DPNERR_ALREADYCLOSING,			"DPNERR_ALREADYCLOSING", NULL,
	//0x80158060
	DPNERR_ALREADYCONNECTED,		"DPNERR_ALREADYCONNECTED", NULL,
	//0x80158070
	DPNERR_ALREADYDISCONNECTING,	"DPNERR_ALREADYDISCONNECTING", NULL,
	//0x80158080
	DPNERR_ALREADYINITIALIZED,		"DPNERR_ALREADYINITIALIZED", NULL,
	//0x80158090
	DPNERR_BUFFERTOOSMALL,			"DPNERR_BUFFERTOOSMALL", NULL,
	//0x80158100
	DPNERR_CANNOTCANCEL,			"DPNERR_CANNOTCANCEL", NULL,
	//0x80158110
	DPNERR_CANTCREATEGROUP,			"DPNERR_CANTCREATEGROUP", NULL,
	//0x80158120
	DPNERR_CANTCREATEPLAYER,		"DPNERR_CANTCREATEPLAYER", NULL,
	//0x80158130
	DPNERR_CANTLAUNCHAPPLICATION,	"DPNERR_CANTLAUNCHAPPLICATION", NULL,
	//0x80158140
	DPNERR_CONNECTING,				"DPNERR_CONNECTING", NULL,
	//0x80158150
	DPNERR_CONNECTIONLOST,			"DPNERR_CONNECTIONLOST", NULL,
	//0x80158160
	DPNERR_CONVERSION,				"DPNERR_CONVERSION", NULL,
	//0x80158170
	DPNERR_DOESNOTEXIST,			"DPNERR_DOESNOTEXIST", NULL,
	//0x80158180
	DPNERR_DUPLICATECOMMAND,		"DPNERR_DUPLICATECOMMAND", NULL,
	//0x80158190
	DPNERR_ENDPOINTNOTRECEIVING,	"DPNERR_ENDPOINTNOTRECEIVING", NULL,
	//0x80158200
	DPNERR_EXCEPTION,				"DPNERR_EXCEPTION", NULL,
	//0x80158210
	DPNERR_GROUPNOTEMPTY,			"DPNERR_GROUPNOTEMPTY", NULL,
	//0x80158220
	DPNERR_HOSTREJECTEDCONNECTION,	"DPNERR_HOSTREJECTEDCONNECTION", NULL,
	//0x80158230
	DPNERR_INCOMPLETEADDRESS,		"DPNERR_INCOMPLETEADDRESS", NULL,
	//0x80158240
	DPNERR_INVALIDADDRESSFORMAT,	"DPNERR_INVALIDADDRESSFORMAT", NULL,
	//0x80158250
	DPNERR_INVALIDAPPLICATION,		"DPNERR_INVALIDAPPLICATION", NULL,
	//0x80158260
	DPNERR_INVALIDCOMMAND,			"DPNERR_INVALIDCOMMAND", NULL,
	//0x80158270
	DPNERR_INVALIDENDPOINT,			"DPNERR_INVALIDENDPOINT", NULL,
	//0x80158280
	DPNERR_INVALIDFLAGS,			"DPNERR_INVALIDFLAGS", NULL,
	//0x80158290
	DPNERR_INVALIDGROUP,			"DPNERR_INVALIDGROUP", NULL,
	//0x80158300
	DPNERR_INVALIDHANDLE,			"DPNERR_INVALIDHANDLE", NULL,
	//0x80158310
	DPNERR_INVALIDINSTANCE,			"DPNERR_INVALIDINSTANCE", NULL,
	//0x80158320
	DPNERR_INVALIDINTERFACE,		"DPNERR_INVALIDINTERFACE", NULL,
#ifndef _XBOX
	//0x80158330
	DPNERR_INVALIDLOCALADDRESS,		"DPNERR_INVALIDLOCALADDRESS", NULL,
#endif
	//0x80158340
	DPNERR_INVALIDOBJECT,			"DPNERR_INVALIDOBJECT", NULL,
	//0x80158350
	DPNERR_INVALIDPASSWORD,			"DPNERR_INVALIDPASSWORD", NULL,
	//0x80158360
	DPNERR_INVALIDPLAYER,			"DPNERR_INVALIDPLAYER", NULL,
	//0x80158370
	DPNERR_INVALIDPRIORITY,			"DPNERR_INVALIDPRIORITY", NULL,
#ifndef _XBOX
	//0x80158380
	DPNERR_INVALIDREMOTEADDRESS,	"DPNERR_INVALIDREMOTEADDRESS", NULL,
#endif
	//0x80158390
	DPNERR_INVALIDSTRING,			"DPNERR_INVALIDSTRING", NULL,
	//0x80158400
	DPNERR_INVALIDURL,				"DPNERR_INVALIDURL", NULL,
	//0x80158410
	DPNERR_INVALIDVERSION,			"DPNERR_INVALIDVERSION", NULL,
	//0x80158420
	DPNERR_NOCAPS,					"DPNERR_NOCAPS", NULL,
	//0x80158430
	DPNERR_NOCONNECTION,			"DPNERR_NOCONNECTION", NULL,
	//0x80158440
	DPNERR_NOHOSTPLAYER,			"DPNERR_NOHOSTPLAYER", NULL,
	//0x80158450
	DPNERR_NOMOREADDRESSCOMPONENTS,	"DPNERR_NOMOREADDRESSCOMPONENTS", NULL,
	//0x80158460
	DPNERR_NORESPONSE,				"DPNERR_NORESPONSE", NULL,
	//0x80158470
	DPNERR_NOTALLOWED,				"DPNERR_NOTALLOWED", NULL,
	//0x80158480
	DPNERR_NOTHOST,					"DPNERR_NOTHOST", NULL,
	//0x80158490
	DPNERR_NOTREADY,				"DPNERR_NOTREADY", NULL,
	//0x80158500
	DPNERR_PLAYERLOST,				"DPNERR_PLAYERLOST", NULL,
	//0x80158510
	DPNERR_SENDTOOLARGE,			"DPNERR_SENDTOOLARGE", NULL,
	//0x80158520
	DPNERR_SESSIONFULL,				"DPNERR_SESSIONFULL", NULL,
	//0x80158530
	DPNERR_TABLEFULL,				"DPNERR_TABLEFULL", NULL,
	//0x80158540
	DPNERR_TIMEDOUT,				"DPNERR_TIMEDOUT", NULL,
	//0x80158550
	DPNERR_UNINITIALIZED,			"DPNERR_UNINITIALIZED", NULL,
	//0x80158560
	DPNERR_USERCANCEL,				"DPNERR_USERCANCEL", NULL,
	// End DNet errors
#endif // NO_ERRORS_DPLAY8

#ifndef NO_ERRORS_DVOICE
	// Begin DPlayVoice errors
	//0x00150005
	DV_FULLDUPLEX,					"DV_FULLDUPLEX", "Your soundcard is capable of full duplex operation.",
	//0x0015000A
	DV_HALFDUPLEX,					"DV_HALFDUPLEX", "Your soundcard can only run in half duplex mode.",
	//0x0015000F
	DV_EXIT,						"DV_EXIT", NULL,
	//0x8015001E
	DVERR_BUFFERTOOSMALL,			"DVERR_BUFFERTOOSMALL", "The supplied buffer is not large enough to contain the requested data.",
	//0x8015005A
	DVERR_EXCEPTION,				"DVERR_EXCEPTION", "An exception occurred when processing the request.",
	//0x80150078
	DVERR_INVALIDFLAGS,				"DVERR_INVALIDFLAGS", "The flags passed to this method are invalid.",
	//0x80150082
	DVERR_INVALIDOBJECT,			"DVERR_INVALIDOBJECT", "The DirectPlayVoice pointer is invalid.",
	//0x80150087
	DVERR_INVALIDPLAYER,			"DVERR_INVALIDPLAYER", "The player ID is not recognized as a valid ID for this voice session.",
	//0x80150091
	DVERR_INVALIDGROUP,				"DVERR_INVALIDGROUP", "The group ID is not recognized as a valid group ID for the transport session.",
	//0x80150096
	DVERR_INVALIDHANDLE,			"DVERR_INVALIDHANDLE", "The specified handle is not valid.",
	//0x8015012C
	DVERR_SESSIONLOST,				"DVERR_SESSIONLOST", "The transport has lost the connection to the session.",
	//0x8015012E
	DVERR_NOVOICESESSION,			"DVERR_NOVOICESESSION", NULL,
	//0x80150168
	DVERR_CONNECTIONLOST,			"DVERR_CONNECTIONLOST", NULL,
	//0x80150169
	DVERR_NOTINITIALIZED,			"DVERR_NOTINITIALIZED", "Initialize() must be called before using this method.",
	//0x8015016A
	DVERR_CONNECTED,				"DVERR_CONNECTED", "The DirectPlayVoice object is connected.",
	//0x8015016B
	DVERR_NOTCONNECTED,				"DVERR_NOTCONNECTED", "The DirectPlayVoice object is not connected.",
	//0x8015016E
	DVERR_CONNECTABORTING,			"DVERR_CONNECTABORTING", "The connection is being disconnected.",
	//0x8015016F
	DVERR_NOTALLOWED,				"DVERR_NOTALLOWED", "The object does not have the permission to perform this operation.",
	//0x80150170
	DVERR_INVALIDTARGET,			"DVERR_INVALIDTARGET", "The specified target is not a valid player ID or group ID for this voice session.",
	//0x80150171
	DVERR_TRANSPORTNOTHOST,			"DVERR_TRANSPORTNOTHOST", "You're trying to host a voice session on a player which is not a host, but the transport is valid, initialized, connected and a local player is present.",
	//0x80150172
	DVERR_COMPRESSIONNOTSUPPORTED,	"DVERR_COMPRESSIONNOTSUPPORTED", "The specified compression type is not supported on the local machine.",
	//0x80150173
	DVERR_ALREADYPENDING,			"DVERR_ALREADYPENDING", "An ASYNC call of this type is already pending.",
	//0x80150174
	DVERR_SOUNDINITFAILURE,			"DVERR_SOUNDINITFAILURE", "A failure was encountered initializing your soundcard.",
	//0x80150175
	DVERR_TIMEOUT,					"DVERR_TIMEOUT", "The operation could not be performed in the specified time.",
	//0x80150176
	DVERR_CONNECTABORTED,			"DVERR_CONNECTABORTED", "The connection was aborted.",
	//0x80150177
	DVERR_NO3DSOUND,				"DVERR_NO3DSOUND", "The local machine does not support 3D sound.",
	//0x80150178
	DVERR_ALREADYBUFFERED,			"DVERR_ALREADYBUFFERED", "There is already a UserBuffer for the specified ID.",
	//0x80150179
	DVERR_NOTBUFFERED,				"DVERR_NOTBUFFERED", "There is no UserBuffer for the specified ID.",
	//0x8015017A
	DVERR_HOSTING,					"DVERR_HOSTING", "The object is the host of the session.",
	//0x8015017B
	DVERR_NOTHOSTING,				"DVERR_NOTHOSTING", "The object is not the host of the session.",
	//0x8015017C
	DVERR_INVALIDDEVICE,			"DVERR_INVALIDDEVICE", "The specified device is not valid.",
	//0x8015017D
	DVERR_RECORDSYSTEMERROR,		"DVERR_RECORDSYSTEMERROR", "An error in the recording system has occurred.",
	//0x8015017E
	DVERR_PLAYBACKSYSTEMERROR,		"DVERR_PLAYBACKSYSTEMERROR", "An error in the playback system has occurred.",
	//0x8015017F
	DVERR_SENDERROR,				"DVERR_SENDERROR", "An error occurred while sending data.",
	//0x80150180
	DVERR_USERCANCEL,				"DVERR_USERCANCEL", NULL,
	//0x80150183
	DVERR_RUNSETUP,					"DVERR_RUNSETUP", NULL,
	//0x80150184
	DVERR_INCOMPATIBLEVERSION,		"DVERR_INCOMPATIBLEVERSION", NULL,
	//0x80150185
	DVERR_PREVIOUSCRASH,			"DVERR_PREVIOUSCRASH", NULL,
	//0x80150187
	DVERR_INITIALIZED,				"DVERR_INITIALIZED", NULL,
	//0x80150188
	DVERR_NOTRANSPORT,				"DVERR_NOTRANSPORT", "Invalid pointer passed to Initialize, not a valid transport, doesn't support transport interface (bad dplay version).",
	//0x80150189
	DVERR_NOCALLBACK,				"DVERR_NOCALLBACK", NULL,
	//0x8015019A
	DVERR_TRANSPORTNOTINIT,			"DVERR_TRANSPORTNOTINIT", "Initialize() hasn't been called on your transport object (but it is a valid interface).",
	//0x8015019B
	DVERR_TRANSPORTNOSESSION,		"DVERR_TRANSPORTNOSESSION", "Transport object has been initialized but no session is running on it.",
	//0x8015019C
	DVERR_TRANSPORTNOPLAYER,		"DVERR_TRANSPORTNOPLAYER", "Transport object is initialized, connected / hosting a session, but no local player has been created.",
	//0x8015019D
	DVERR_USERBACK,					"DVERR_USERBACK", NULL,
	//0x8015019E
	DVERR_NORECVOLAVAILABLE,		"DVERR_NORECVOLAVAILABLE", NULL,
	//0x8015019F
	DVERR_INVALIDBUFFER,			"DVERR_INVALIDBUFFER", NULL,
	// End DPlayVoice errors
#endif // NO_ERRORS_DVOICE


#ifndef NO_ERRORS_DSOUND
	// Begin DSound errors
#ifndef _XBOX // DSERR_ALLOCATED not supported
	DSERR_ALLOCATED,				"DSERR_ALLOCATED", "The call failed because resources (such as a priority level) were already being used by another caller.",
#endif // ! XBOX
	DSERR_CONTROLUNAVAIL,			"DSERR_CONTROLUNAVAIL", "The control (vol, pan, etc.) requested by the caller is not available.",
	DSERR_INVALIDPARAM,				"DSERR_INVALIDPARAM", "An invalid parameter was passed to the returning function.",
	DSERR_INVALIDCALL,				"DSERR_INVALIDCALL", "This call is not valid for the current state of this object.",

	DSERR_NOAGGREGATION,			"DSERR_NOAGGREGATION", NULL,
	DSERR_NODRIVER,					"DSERR_NODRIVER", NULL,
	DSERR_OUTOFMEMORY,				"DSERR_OUTOFMEMORY", NULL,
#endif // NO_ERRORS_DSOUND
	// End DSound errors

#ifndef NO_ERRORS_DDRAW
	// Begin DDraw errors
	DDERR_ALREADYINITIALIZED,		"DDERR_ALREADYINITIALIZED", "This object is already initialized.",
	DDERR_CANNOTATTACHSURFACE,		"DDERR_CANNOTATTACHSURFACE", "This surface can not be attached to the requested surface.",
	DDERR_CANNOTDETACHSURFACE,		"DDERR_CANNOTDETACHSURFACE", "This surface can not be detached from the requested surface.",
	DDERR_CURRENTLYNOTAVAIL,		"DDERR_CURRENTLYNOTAVAIL", "Support is currently not available.",
	DDERR_EXCEPTION,				"DDERR_EXCEPTION", "An exception was encountered while performing the requested operation.",
	DDERR_HEIGHTALIGN,				"DDERR_HEIGHTALIGN", "Height of rectangle provided is not a multiple of reqd alignment.",
	DDERR_INCOMPATIBLEPRIMARY,		"DDERR_INCOMPATIBLEPRIMARY", "Unable to match primary surface creation request with existing primary surface.",
	DDERR_INVALIDCAPS,				"DDERR_INVALIDCAPS", "One or more of the caps bits passed to the callback are incorrect.",
	DDERR_INVALIDCLIPLIST,			"DDERR_INVALIDCLIPLIST", "DirectDraw does not support provided Cliplist.",
	DDERR_INVALIDMODE,				"DDERR_INVALIDMODE", "DirectDraw does not support the requested mode.",
	DDERR_INVALIDOBJECT,			"DDERR_INVALIDOBJECT", "DirectDraw received a pointer that was an invalid DIRECTDRAW object.",
	DDERR_INVALIDPIXELFORMAT,		"DDERR_INVALIDPIXELFORMAT", "One or more of the parameters passed to the callback function are incorrect.",
	DDERR_INVALIDRECT,				"DDERR_INVALIDRECT", "Rectangle provided was invalid.",
	DDERR_LOCKEDSURFACES,			"DDERR_LOCKEDSURFACES", "Operation could not be carried out because one or more surfaces are locked.",
	DDERR_NO3D,						"DDERR_NO3D", "There is no 3D present.",
	DDERR_NOALPHAHW,				"DDERR_NOALPHAHW", "Operation could not be carried out because there is no alpha accleration hardware present or available.",
	DDERR_NOSTEREOHARDWARE,			"DDERR_NOSTEREOHARDWARE", "Operation could not be carried out because there is no stereo hardware present or available.",
	DDERR_NOSURFACELEFT,			"DDERR_NOSURFACELEFT", "Operation could not be carried out because there is no hardware present which supports stereo surfaces.",
	DDERR_NOCLIPLIST,				"DDERR_NOCLIPLIST", "No clip list available.",
	DDERR_NOCOLORCONVHW,			"DDERR_NOCOLORCONVHW", "Operation could not be carried out because there is no color conversion hardware present or available.",
	DDERR_NOCOOPERATIVELEVELSET,	"DDERR_NOCOOPERATIVELEVELSET", "Create function called without DirectDraw object method SetCooperativeLevel being called.",
	DDERR_NOCOLORKEY,				"DDERR_NOCOLORKEY", "Surface doesn't currently have a color key.",
	DDERR_NOCOLORKEYHW,				"DDERR_NOCOLORKEYHW", "Operation could not be carried out because there is no hardware support of the dest color key.",
	DDERR_NODIRECTDRAWSUPPORT,		"DDERR_NODIRECTDRAWSUPPORT", "No DirectDraw support possible with current display driver.",
	DDERR_NOEXCLUSIVEMODE,			"DDERR_NOEXCLUSIVEMODE", "Operation requires the application to have exclusive mode but the application does not have exclusive mode.",
	DDERR_NOFLIPHW,					"DDERR_NOFLIPHW", "Flipping visible surfaces is not supported.",
	DDERR_NOGDI,					"DDERR_NOGDI", "There is no GDI present.",
	DDERR_NOMIRRORHW,				"DDERR_NOMIRRORHW", "Operation could not be carried out because there is no hardware present or available.",
	DDERR_NOTFOUND,					"DDERR_NOTFOUND", "Requested item was not found.",
	DDERR_NOOVERLAYHW,				"DDERR_NOOVERLAYHW", "Operation could not be carried out because there is no overlay hardware present or available.",
	DDERR_OVERLAPPINGRECTS,			"DDERR_OVERLAPPINGRECTS", "Operation could not be carried out because the source and destination rectangles are on the same surface and overlap each other.",
	DDERR_NORASTEROPHW,				"DDERR_NORASTEROPHW", "Operation could not be carried out because there is no appropriate raster op hardware present or available.",
	DDERR_NOROTATIONHW,				"DDERR_NOROTATIONHW", "Operation could not be carried out because there is no rotation hardware present or available.",
	DDERR_NOSTRETCHHW,				"DDERR_NOSTRETCHHW", "Operation could not be carried out because there is no hardware support for stretching.",
	DDERR_NOT4BITCOLOR,				"DDERR_NOT4BITCOLOR", "DirectDrawSurface is not in 4 bit color palette and the requested operation requires 4 bit color palette.",
	DDERR_NOT4BITCOLORINDEX,		"DDERR_NOT4BITCOLORINDEX", "DirectDrawSurface is not in 4 bit color index palette and the requested operation requires 4 bit color index palette.",
	DDERR_NOT8BITCOLOR,				"DDERR_NOT8BITCOLOR", "DirectDraw Surface is not in 8 bit color mode and the requested operation requires 8 bit color.",
	DDERR_NOTEXTUREHW,				"DDERR_NOTEXTUREHW", "Operation could not be carried out because there is no texture mapping hardware present or available.",
	DDERR_NOVSYNCHW,				"DDERR_NOVSYNCHW", "Operation could not be carried out because there is no hardware support for vertical blank synchronized operations.",
	DDERR_NOZBUFFERHW,				"DDERR_NOZBUFFERHW", "Operation could not be carried out because there is no hardware support for zbuffer blting.",
	DDERR_NOZOVERLAYHW,				"DDERR_NOZOVERLAYHW", "Overlay surfaces could not be z layered based on their BltOrder because the hardware does not support z layering of overlays.",
	DDERR_OUTOFCAPS,				"DDERR_OUTOFCAPS", "The hardware needed for the requested operation has already been allocated.",
	DDERR_OUTOFVIDEOMEMORY,			"DDERR_OUTOFVIDEOMEMORY", "DirectDraw does not have enough memory to perform the operation.",
	DDERR_OVERLAYCANTCLIP,			"DDERR_OVERLAYCANTCLIP", "Hardware does not support clipped overlays.",
	DDERR_OVERLAYCOLORKEYONLYONEACTIVE, "DDERR_OVERLAYCOLORKEYONLYONEACTIVE", "Can only have ony color key active at one time for overlays.",
	DDERR_PALETTEBUSY,				"DDERR_PALETTEBUSY", "Access to this palette is being refused because the palette is already locked by another thread.",
	DDERR_COLORKEYNOTSET,			"DDERR_COLORKEYNOTSET", "No src color key specified for this operation.",
	DDERR_SURFACEALREADYATTACHED,	"DDERR_SURFACEALREADYATTACHED", "This surface is already attached to the surface it is being attached to.",
	DDERR_SURFACEALREADYDEPENDENT,	"DDERR_SURFACEALREADYDEPENDENT", "This surface is already a dependency of the surface it is being made a dependency of.",
	DDERR_SURFACEBUSY,				"DDERR_SURFACEBUSY", "Access to this surface is being refused because the surface is already locked by another thread.",
	DDERR_CANTLOCKSURFACE,			"DDERR_CANTLOCKSURFACE", "Access to this surface is being refused because no driver exists which can supply a pointer to the surface.",
	DDERR_SURFACEISOBSCURED,		"DDERR_SURFACEISOBSCURED", "Access to Surface refused because Surface is obscured.",
	DDERR_SURFACELOST,				"DDERR_SURFACELOST", "Access to this surface is being refused because the surface is gone.",
	DDERR_SURFACENOTATTACHED,		"DDERR_SURFACENOTATTACHED", "The requested surface is not attached.",
	DDERR_TOOBIGHEIGHT,				"DDERR_TOOBIGHEIGHT", "Height requested by DirectDraw is too large.",
	DDERR_TOOBIGSIZE,				"DDERR_TOOBIGSIZE", "Size requested by DirectDraw is too large --  The individual height and width are OK.",
	DDERR_TOOBIGWIDTH,				"DDERR_TOOBIGWIDTH", "Width requested by DirectDraw is too large.",
	DDERR_UNSUPPORTEDFORMAT,		"DDERR_UNSUPPORTEDFORMAT", "Pixel format requested is unsupported by DirectDraw.",
	DDERR_UNSUPPORTEDMASK,			"DDERR_UNSUPPORTEDMASK", "Bitmask in the pixel format requested is unsupported by DirectDraw.",
	DDERR_INVALIDSTREAM,			"DDERR_INVALIDSTREAM", "The specified stream contains invalid data.",
	DDERR_VERTICALBLANKINPROGRESS,	"DDERR_VERTICALBLANKINPROGRESS", "Vertical blank is in progress.",
	DDERR_WASSTILLDRAWING,			"DDERR_WASSTILLDRAWING", "Informs DirectDraw that the previous Blt which is transfering information to or from this Surface is incomplete.",
	DDERR_DDSCAPSCOMPLEXREQUIRED,	"DDERR_DDSCAPSCOMPLEXREQUIRED", "The specified surface type requires specification of the COMPLEX flag.",
	DDERR_XALIGN,					"DDERR_XALIGN", "Rectangle provided was not horizontally aligned on reqd. boundary.",
	DDERR_INVALIDDIRECTDRAWGUID,	"DDERR_INVALIDDIRECTDRAWGUID", "The GUID passed to DirectDrawCreate is not a valid DirectDraw driver identifier.",
	DDERR_DIRECTDRAWALREADYCREATED,	"DDERR_DIRECTDRAWALREADYCREATED", "A DirectDraw object representing this driver has already been created for this process.",
	DDERR_NODIRECTDRAWHW,			"DDERR_NODIRECTDRAWHW", "A hardware only DirectDraw object creation was attempted but the driver did not support any hardware.",
	DDERR_PRIMARYSURFACEALREADYEXISTS, "DDERR_PRIMARYSURFACEALREADYEXISTS", "This process already has created a primary surface.",
	DDERR_NOEMULATION,				"DDERR_NOEMULATION", "Software emulation not available.",
	DDERR_REGIONTOOSMALL,			"DDERR_REGIONTOOSMALL", "Region passed to Clipper::GetClipList is too small.",
	DDERR_CLIPPERISUSINGHWND,		"DDERR_CLIPPERISUSINGHWND", "An attempt was made to set a clip list for a clipper objec that is already monitoring an hwnd.",
	DDERR_NOCLIPPERATTACHED,		"DDERR_NOCLIPPERATTACHED", "No clipper object attached to surface object.",
	DDERR_NOHWND,					"DDERR_NOHWND", "Clipper notification requires an HWND or no HWND has previously been set as the CooperativeLevel HWND.",
	DDERR_HWNDSUBCLASSED,			"DDERR_HWNDSUBCLASSED", "HWND used by DirectDraw CooperativeLevel has been subclassed, this prevents DirectDraw from restoring state.",
	DDERR_HWNDALREADYSET,			"DDERR_HWNDALREADYSET", "The CooperativeLevel HWND has already been set.  It can not be reset while the process has surfaces or palettes created.",
	DDERR_NOPALETTEATTACHED,		"DDERR_NOPALETTEATTACHED", "No palette object attached to this surface.",
	DDERR_NOPALETTEHW,				"DDERR_NOPALETTEHW", "No hardware support for 16 or 256 color palettes.",
	DDERR_BLTFASTCANTCLIP,			"DDERR_BLTFASTCANTCLIP", "If a clipper object is attached to the source surface passed into a BltFast call.",
	DDERR_NOBLTHW,					"DDERR_NOBLTHW", "No blter.",

	// More...

#endif // NO_ERRORS_DDRAW
	// End DDraw errors

#ifndef NO_ERRORS_TNCONTRL
	// Begin TNCONTRL errors
	//0x86661001
	TNERR_CONNECTIONDROPPED,		"TNERR_CONNECTIONDROPPED", "The machine you're trying to send to disconnected.",
	//0x86661002
	TNERR_LOSTTESTER,				"TNERR_LOSTTESTER", "A tester completed the test or crashed.",

	//0x86661101
	TNSR_USERCANCEL,				"TNSR_USERCANCEL", "User cancelled a sync operation.",
	//0x86661103
	TNSR_LOSTTESTER,				"TNSR_LOSTTESTER", "A tester was lost during the sync operation.",

	//0x86661111
	TNWR_USERCANCEL,				"TNWR_USERCANCEL", "User cancelled a WaitForEventOrCancel operation.",
	//0x86661112
	TNWR_TIMEOUT,					"TNWR_TIMEOUT", "A WaitForEventOrCancel operation timed out.",
	//0x86661113
	TNWR_LOSTTESTER,				"TNWR_LOSTTESTER", "A tester was lost during the WaitForEventOrCancel operation.",

	//0x86661121
	TNCWR_USERCANCEL,				"TNCWR_USERCANCEL", "User cancelled a WaitForLeechConnection operation.",
	//0x86661122
	TNCWR_TIMEOUT,					"TNCWR_TIMEOUT", "A WaitForLeechConnection operation timed out.",
	//0x86661123
	TNCWR_LOSTTESTER,				"TNCWR_LOSTTESTER", "A tester was lost during the WaitForLeechConnection operation.",
	// End TNCONTRL errors
#endif // NO_ERRORS_TNCONTRL


	// Begin exceptions
	EXCEPTION_ACCESS_VIOLATION,		"EXCEPTION_ACCESS_VIOLATION", NULL,
	EXCEPTION_DATATYPE_MISALIGNMENT, "EXCEPTION_DATATYPE_MISALIGNMENT", NULL,
	EXCEPTION_BREAKPOINT,			"EXCEPTION_BREAKPOINT", NULL,
	EXCEPTION_SINGLE_STEP,			"EXCEPTION_SINGLE_STEP", NULL,
	EXCEPTION_ARRAY_BOUNDS_EXCEEDED, "EXCEPTION_ARRAY_BOUNDS_EXCEEDED", NULL,
	EXCEPTION_FLT_DENORMAL_OPERAND,	"EXCEPTION_FLT_DENORMAL_OPERAND", NULL,
	EXCEPTION_FLT_DIVIDE_BY_ZERO,	"EXCEPTION_FLT_DIVIDE_BY_ZERO", NULL,
	EXCEPTION_FLT_INEXACT_RESULT,	"EXCEPTION_FLT_INEXACT_RESULT", NULL,
	EXCEPTION_FLT_INVALID_OPERATION, "EXCEPTION_FLT_INVALID_OPERATION", NULL,
	EXCEPTION_FLT_OVERFLOW,			"EXCEPTION_FLT_OVERFLOW", NULL,
	EXCEPTION_FLT_STACK_CHECK,		"EXCEPTION_FLT_STACK_CHECK", NULL,
	EXCEPTION_FLT_UNDERFLOW,		"EXCEPTION_FLT_UNDERFLOW", NULL,
	EXCEPTION_INT_DIVIDE_BY_ZERO,	"EXCEPTION_INT_DIVIDE_BY_ZERO", NULL,
	EXCEPTION_INT_OVERFLOW,			"EXCEPTION_INT_OVERFLOW", NULL,
	EXCEPTION_PRIV_INSTRUCTION,		"EXCEPTION_PRIV_INSTRUCTION", NULL,
	EXCEPTION_IN_PAGE_ERROR,		"EXCEPTION_IN_PAGE_ERROR", NULL,
	EXCEPTION_ILLEGAL_INSTRUCTION,	"EXCEPTION_ILLEGAL_INSTRUCTION", NULL,
	EXCEPTION_NONCONTINUABLE_EXCEPTION, "EXCEPTION_NONCONTINUABLE_EXCEPTION", NULL,
	EXCEPTION_STACK_OVERFLOW,		"EXCEPTION_STACK_OVERFLOW", NULL,
	EXCEPTION_INVALID_DISPOSITION,	"EXCEPTION_INVALID_DISPOSITION", NULL,
	EXCEPTION_GUARD_PAGE,			"EXCEPTION_GUARD_PAGE", NULL,
	EXCEPTION_INVALID_HANDLE,		"EXCEPTION_INVALID_HANDLE", NULL,
	// End exceptions

	WAIT_IO_COMPLETION,				"WAIT_IO_COMPLETION, STATUS_USER_APC", NULL,
	WAIT_ABANDONED_0,				"WAIT_ABANDONED_0", NULL,
	WAIT_FAILED,					"WAIT_FAILED", NULL
};





//==================================================================================
// GetErrorDescription
//----------------------------------------------------------------------------------
//
// Description: Stores a text representation of the error code into the given
//				buffer.  If the pointer to that is NULL, the size is stored in the
//				given DWORD pointer and ERROR_BUFFER_TOO_SMALL is returned.
//				If pdwShortcut is not NULL, the DWORD it points to should be set to
//				ERRORSSHORTCUT_NOTSEARCHED the first time it is called.  It will
//				then be updated so that further calls to this function can skip the
//				lookup step.
//
// Arguments:
//	HRESULT hresult			Result code to print.
//	DWORD_PTR dwFlags		How to print the item.
//	char* pszBuffer			Pointer to buffer, or NULL to retrieve size.
//	DWORD* pdwBufferSize	Pointer to size of buffer, or place to store size
//							required.
//	DWORD* pdwShortcut		Pointer to known index of error code, if any.  
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT GetErrorDescription(LONG_PTR hresult, DWORD_PTR dwFlags, char* pszBuffer,
							DWORD* pdwBufferSize, DWORD* pdwShortcut)
{
	char		szNumber[32];
	BOOL		fFound = FALSE;
	DWORD		dwTemp;


	if (pszBuffer == NULL)
		(*pdwBufferSize) = 1; // NULL termination
	else
		strcpy(pszBuffer, ""); // start with an empty string

	if (dwFlags & ERRORSFLAG_PRINTHEXVALUE)
	{
		wsprintf(szNumber, "0x%08x, ", hresult);
		if (pszBuffer == NULL)
		{
			(*pdwBufferSize) += strlen(szNumber);
		} // end if (there's no buffer)
		else
		{
			strcat(pszBuffer, szNumber);
		} // end else (there is a buffer)
	} // end if (should print hex value)

	if (dwFlags & ERRORSFLAG_PRINTDECIMALVALUE)
	{
		wsprintf(szNumber, "%i, ", hresult);
		if (pszBuffer == NULL)
		{
			(*pdwBufferSize) += strlen(szNumber);
		} // end if (there's no buffer)
		else
		{
			strcat(pszBuffer, szNumber);
		} // end else (there is a buffer)
	} // end if (should print hex value)

	// If there's no shortcut pointer, or it's ERRORSSHORTCUT_NOTSEARCHED,
	// start searching.
	if ((pdwShortcut == NULL) || ((*pdwShortcut) == ERRORSSHORTCUT_NOTSEARCHED))
	{
		for(dwTemp = 0; dwTemp < (sizeof (g_TNErrorTable) / sizeof (TNERRORITEM)); dwTemp++)
		{
			if (g_TNErrorTable[dwTemp].hresult == hresult)
			{
				fFound = TRUE;

				// If the caller was nice enough to let us have a place to store
				// a shortcut, do so.
				if (pdwShortcut != NULL)
					(*pdwShortcut) = dwTemp;

				if (pszBuffer == NULL)
				{
					(*pdwBufferSize) += strlen(g_TNErrorTable[dwTemp].pszName);

					if ((dwFlags & ERRORSFLAG_PRINTDESCRIPTION) && 
						(g_TNErrorTable[dwTemp].pszDescription != NULL))
					{
						(*pdwBufferSize) += 2 + strlen(g_TNErrorTable[dwTemp].pszDescription) + 1;
					} // end if (should print and there is a description)
				} // end if (there's no buffer)
				else
				{
					strcat(pszBuffer, g_TNErrorTable[dwTemp].pszName);

					if ((dwFlags & ERRORSFLAG_PRINTDESCRIPTION) && 
						(g_TNErrorTable[dwTemp].pszDescription != NULL))
					{
						strcat(pszBuffer, " '");
						strcat(pszBuffer, g_TNErrorTable[dwTemp].pszDescription);
						strcat(pszBuffer, "'");
					} // end if (should print and there is a description)
				} // end else (there is a buffer)

				// Get out of the for loop
				break;
			} // end if (we found the entry)
		} // end for (each error entry)
	} // end if (no shortcut pointer or it's NOTSEARCHED)
	else if ((pdwShortcut != NULL) && ((*pdwShortcut) != ERRORSSHORTCUT_NOTFOUND))
	{
		fFound = TRUE;

		if (pszBuffer == NULL)
		{
			(*pdwBufferSize) += strlen(g_TNErrorTable[(*pdwShortcut)].pszName);

			if ((dwFlags & ERRORSFLAG_PRINTDESCRIPTION) && 
				(g_TNErrorTable[(*pdwShortcut)].pszDescription != NULL))
			{
				(*pdwBufferSize) += 2 + strlen(g_TNErrorTable[(*pdwShortcut)].pszDescription) + 1;
			} // end if (should print and there is a description)
		} // end if (there's no buffer)
		else
		{
			strcat(pszBuffer, g_TNErrorTable[(*pdwShortcut)].pszName);

			if ((dwFlags & ERRORSFLAG_PRINTDESCRIPTION) && 
				(g_TNErrorTable[(*pdwShortcut)].pszDescription != NULL))
			{
				strcat(pszBuffer, " '");
				strcat(pszBuffer, g_TNErrorTable[(*pdwShortcut)].pszDescription);
				strcat(pszBuffer, "'");
			} // end if (should print and there is a description)
		} // end else (there is a buffer)
	} // end else if (there was a shortcut pointer and it's not NOTFOUND)

	if (! fFound)
	{
		// If the caller was nice enough to let us have a place to store
		// a shortcut, do so.
		if (pdwShortcut != NULL)
			(*pdwShortcut) = ERRORSSHORTCUT_NOTFOUND; // special value

		/*
		// FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,
		// 				NULL,
		// 				hresult,
		// 				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		// 				pszBuffer,
		//				0,
		//				NULL);
		
		// Process any inserts in lpMsgBuf ?
		*/

		if (pszBuffer == NULL)
		{
			(*pdwBufferSize) += 13;
		} // end if (there's no buffer)
		else
		{
			strcat(pszBuffer, "Unknown_Error");
		} // end else (there is a buffer)
	} // end if (we didn't find the error)

	if (pszBuffer == NULL)
		return (ERROR_BUFFER_TOO_SMALL);

	return (S_OK);
} // GetErrorDescription




#undef DEBUG_SECTION
#define DEBUG_SECTION	"DoSprintfErrorBox()"
//==================================================================================
// DoSprintfErrorBox
//----------------------------------------------------------------------------------
//
// Description: Displays an error message box with the given title and text.  The
//				standard sprintf tokens can be specified, see
//				sprintf.h for details.
//
// Arguments:
//	HWND hWndParent				Parent window, or NULL for none.
//	LPTSTR lptszTitle			Title for message box
//	LPTSTR lptszFormatString	String (with optional tokens) to print.
//	DWORD dwNumParms			How many parameters are in the following variable
//								parameter list.
//	...							Variable list of parameters to parse.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
void DoSprintfErrorBox(HWND hWndParent, LPTSTR lptszTitle, LPTSTR lptszFormatString,
					DWORD dwNumParms, ...)
{
	PVOID*		papvParms = NULL;
	va_list		currentparam;
	DWORD		dwTemp = 0;
	TCHAR*		lptszBuffer = NULL;


	if (dwNumParms > 0)
	{
		papvParms = (PVOID*) LocalAlloc(LPTR, (dwNumParms * sizeof (PVOID)));
		if (papvParms == NULL)
			return;

		va_start(currentparam, dwNumParms);

		for(dwTemp = 0; dwTemp < dwNumParms; dwTemp++)
		{
			papvParms[dwTemp] = va_arg(currentparam, PVOID);
		} // end for (each parameter)

		va_end(currentparam);
	} // end if (there are parameters to check)


#ifndef _XBOX // no GDI supported
	TNsprintf_array(&lptszBuffer, lptszFormatString, dwNumParms, papvParms);

	MessageBox(hWndParent, lptszBuffer, lptszTitle, MB_OK | MB_ICONEXCLAMATION);

	TNsprintf_free(&lptszBuffer);
#else // ! XBOX
#pragma TODO(tristanj, "Need to come up with an alternative form for MessageBox's on Xbox")
#endif // XBOX


	//if (papvParms != NULL)
	{
		LocalFree(papvParms);
		papvParms = NULL;
	} // end if (allocated array)
} // DoSprintfErrorBox
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"GetTNErrorTable()"
//==================================================================================
// GetTNErrorTable
//----------------------------------------------------------------------------------
//
// Description: Returns a pointer to the error table and its size.
//
// Arguments:
//	PTNERRORITEM* ppaTNErrorTable	Place to store error table pointer.
//	DWORD* pdwNumEntries			Place to store number of entries in error table.
//
// Returns: None.
//==================================================================================
void GetTNErrorTable(PTNERRORITEM* ppaTNErrorTable, DWORD* pdwNumEntries)
{
	(*ppaTNErrorTable) = g_TNErrorTable;
	(*pdwNumEntries) = sizeof (g_TNErrorTable) / sizeof (TNERRORITEM);
} // GetTNErrorTable
#undef DEBUG_SECTION
#define DEBUG_SECTION	""

