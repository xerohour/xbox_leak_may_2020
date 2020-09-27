

#if !defined(AMC_MS_EMU_API_ERROR__H)
#define AMC_MS_EMU_API_ERROR__H


#include <WINERROR.H>


// Define a few custom error codes following WINERRORS.H:
//
//  Values are 32 bit values layed out as follows:
//
//   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
//   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
//  +---+-+-+-----------------------+-------------------------------+
//  |Sev|C|R|     Facility          |               Code            |
//  +---+-+-+-----------------------+-------------------------------+
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
//      R - is a reserved bit
//
//      Facility - is the facility code
//
//      Code - is the facility's status code
//
//
#define MAKE_EMU_ERROR( code )    _HRESULT_TYPEDEF_( 0xE0000000L | code )
#define MAKE_EMU_WARNING( code )  _HRESULT_TYPEDEF_( 0xA0000000L | code )

#define EMU_NO_ERROR 0

// Use as close as possible to windows error codes. If nothing is close use
// codes between 0x8000 to 0x80ff.

// Warning: unexpected error druning EndEmulationConversation()
#define EMU_WARNING_ERROR_WHILE_ENDING_EMULATION  MAKE_EMU_WARNING(0x8000)

// This libray only suppose to work in Windows 2000
// environment.
#define EMU_ERR_BAD_WIN_VERSION   MAKE_EMU_ERROR(ERROR_OLD_WIN_VERSION)

// Invalid handle possible: 
// 1. driver not installed, 
// 2. another application is using the library
#define EMU_ERR_INVALID_HANDLE    MAKE_EMU_ERROR(ERROR_INVALID_HANDLE)

// Error in getting resources for the library.
#define EMU_ERR_NO_RESOURCES      MAKE_EMU_ERROR(ERROR_NO_SYSTEM_RESOURCES)

// Unexpected error response from driver.
#define EMU_ERR_DRIVER_ERROR      MAKE_EMU_ERROR(0x8001)

// Unexpected error response from hardware.
#define EMU_ERR_HARDWARE          MAKE_EMU_ERROR(0x8002)

// DVD is not responding possible cable disconnect or power to the Xbox is off
#define EMU_ERR_NO_DVD            MAKE_EMU_ERROR(0x8003)

// Unsupported hardware dectected
#define EMU_ERR_OUTDATED_HARDWARE MAKE_EMU_ERROR(0x8004)

// Request an operation when emulation has not started 
#define EMU_ERR_NOT_EMULATING     MAKE_EMU_ERROR(0x8005)

// Invalid parameter
#define EMU_ERR_INVALID_PARAMETER MAKE_EMU_ERROR(ERROR_INVALID_PARAMETER)


#endif // !AMC_MS_EMU_API_ERROR__H
// ---------------------- End Of File ---------------------
// --------------------------------------------------------
