 // LanguageNames=(English=9:BRERROR1)
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
// Define the facility codes
//


//
// Define the severity codes
//


//
// MessageId: MROUTER_E_INVALID_OUTPUT_ID
//
// MessageText:
//
//  The VIF ID is invalid.
//
#define MROUTER_E_INVALID_OUTPUT_ID      ((HRESULT)0xC0040001L)

//
// MessageId: MROUTER_E_INVALID_RESV_ID
//
// MessageText:
//
//  The bandwidth reservation ID is invalid.
//
#define MROUTER_E_INVALID_RESV_ID        ((HRESULT)0xC0040002L)

//
// MessageId: MROUTER_E_INVALID_ROUTE
//
// MessageText:
//
//  The specified route does not match any entry in the route table.
//
#define MROUTER_E_INVALID_ROUTE          ((HRESULT)0xC0040003L)

//
// MessageId: MROUTER_E_INVALID_STATE
//
// MessageText:
//
//  The specified VIF state code is not valid.
//
#define MROUTER_E_INVALID_STATE          ((HRESULT)0xC0040004L)

//
// MessageId: MROUTER_E_ROUTE_EXISTS
//
// MessageText:
//
//  The specified route already exists.
//
#define MROUTER_E_ROUTE_EXISTS           ((HRESULT)0xC0040005L)

//
// MessageId: MROUTER_E_INSUFFICIENT_BANDWIDTH
//
// MessageText:
//
//  The bandwidth reservation cannot be created or altered because
//  the maximum bandwidth of the target virtual interface would be exceeded.
//
#define MROUTER_E_INSUFFICIENT_BANDWIDTH ((HRESULT)0xC0040006L)

//
// MessageId: MROUTER_E_OUTPUT_MAX_REACHED
//
// MessageText:
//
//  The service has reached its maximum number of virtual interfaces.
//  No more virtual interfaces can be installed.
//
#define MROUTER_E_OUTPUT_MAX_REACHED     ((HRESULT)0xC0040007L)

//
// MessageId: MROUTER_E_INTERNAL_ERROR
//
// MessageText:
//
//  The service encountered an internal error.
//  This error should not occur during normal usage.
//  Please check the event log on the server computer.
//
#define MROUTER_E_INTERNAL_ERROR         ((HRESULT)0xC0040008L)

//
// MessageId: MROUTER_E_OUTPUT_VALUE_ERROR
//
// MessageText:
//
//  The output driver returned invalid configuration data.
//  Make sure that the VIF driver DLL is compatible with
//  this version of the multicast router.
//
#define MROUTER_E_OUTPUT_VALUE_ERROR     ((HRESULT)0xC0040009L)

//
// MessageId: MROUTER_E_INVALID_OUTPUT_VALUE
//
// MessageText:
//
//  The configuration value is not a valid datatype.
//
#define MROUTER_E_INVALID_OUTPUT_VALUE   ((HRESULT)0xC004000AL)

//
// MessageId: MROUTER_E_OUTPUT_VALUE_REJECTED
//
// MessageText:
//
//  The output driver rejected the configuration data.
//  Make sure the index and data type of the configuration data is valid.
//  Please check the event log on the server computer.
//
#define MROUTER_E_OUTPUT_VALUE_REJECTED  ((HRESULT)0xC004000BL)

//
// MessageId: MROUTER_E_OUTPUT_VALUE_NOT_SUPPORTED
//
// MessageText:
//
//  The output driver does not support dynamic configuration.
//
#define MROUTER_E_OUTPUT_VALUE_NOT_SUPPORTED ((HRESULT)0xC004000CL)

//
// MessageId: MROUTER_E_OUTPUT_SET_STATE_ERROR
//
// MessageText:
//
//  The output driver refused to change its state.
//  Please check the event log on the server computer.
//
#define MROUTER_E_OUTPUT_SET_STATE_ERROR ((HRESULT)0xC004000DL)

//
// MessageId: MROUTER_E_OUTPUT_DLL_LOAD_FAILED
//
// MessageText:
//
//  The output driver DLL could not be loaded.
//  Please check to make sure that the driver DLL path is correct
//  and that the DLL is compatible with this version of the multicast router.
//  Please check the event log on the server computer.
//
#define MROUTER_E_OUTPUT_DLL_LOAD_FAILED ((HRESULT)0xC004000EL)

//
// MessageId: MROUTER_E_OUTPUT_DLL_INCOMPATIBLE
//
// MessageText:
//
//  The VIF driver DLL is not compatible with this version of the multicast router.
//
#define MROUTER_E_OUTPUT_DLL_INCOMPATIBLE ((HRESULT)0xC004000FL)

//
// MessageId: MROUTER_E_REGISTRY_ERROR
//
// MessageText:
//
//  A registry operation could not be completed.
//  Please check the event log on the server computer.
//
#define MROUTER_E_REGISTRY_ERROR         ((HRESULT)0xC0040010L)

//
// MessageId: MROUTER_E_WINSOCK_ERROR
//
// MessageText:
//
//  An error occurred in the WinSock subsystem.
//  Please check the event log on the server computer.
//
#define MROUTER_E_WINSOCK_ERROR          ((HRESULT)0xC0040011L)

//
// MessageId: MROUTER_E_OUTPUT_NOT_LOADED
//
// MessageText:
//
//  The operation could not be completed because the output driver is not loaded.
//  Please load the output driver first.
//
#define MROUTER_E_OUTPUT_NOT_LOADED      ((HRESULT)0xC0040012L)

//
// MessageId: MROUTER_E_OUTPUT_STARTUP_FAILED
//
// MessageText:
//
//  The virtual interface could not be started.
//  Please check the event log on the server computer.
//
#define MROUTER_E_OUTPUT_STARTUP_FAILED  ((HRESULT)0xC0040013L)

//
// MessageId: MROUTER_E_OUTPUT_SHUTDOWN_PENDING
//
// MessageText:
//
//  The operation could not be completed because the virtual interface is in the process of shutting down.
//
#define MROUTER_E_OUTPUT_SHUTDOWN_PENDING ((HRESULT)0xC0040014L)

//
// MessageId: MROUTER_E_VIF_CANNOT_DELETE
//
// MessageText:
//
//  The virtual interface cannot be deleted because it is a permanent virtual interface.
//
#define MROUTER_E_VIF_CANNOT_DELETE      ((HRESULT)0xC0040015L)

//
// MessageId: MROUTER_E_VIF_DUPLICATE_DISPLAY_NAME
//
// MessageText:
//
//  A virtual interface with the specified display name already exists.
//
#define MROUTER_E_VIF_DUPLICATE_DISPLAY_NAME ((HRESULT)0xC0040016L)

//
// MessageId: MROUTER_E_INCONSISTENT_VIF_ID
//
// MessageText:
//
//  The virtual interface and bandwidth reservation specified are inconsistent.
//  The destination virtual interface of the bandwidth reservation does not
//  match the specified virtual interface.
//
#define MROUTER_E_INCONSISTENT_VIF_ID    ((HRESULT)0xC0040017L)

