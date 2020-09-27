/*++

Copyright (C) 1999 Microsoft Corporation

Module Name:

    Reporting.cpp

Abstract:

    

Author:

    Josh Poley (jpoley) 1-1-1999

Revision History:

--*/

#include "StdAfx.h"
#include "Reporting.h"
#include "../HttpClient/HttpClient.h"

// NOTE: due to limitations in HTTP POST, these strings cannot contain 
//       commas (,) ampersands (&) percents (%) or equal signs (=)
//       also, plus symbols (+) will be turned into spaces
static const ErrorDescription HttpErrors[] = 
    {
    { HTTP_STATUS_CONTINUE,             "HTTP 100", "HTTP 100 CONTINUE - Ok to continue with request."},
    { HTTP_STATUS_SWITCH_PROTOCOLS,     "HTTP 101", "HTTP 101 SWITCH PROTOCOLS - The server has switched protocols in upgrade header."},
    { HTTP_STATUS_OK,                   "HTTP 200", "HTTP 200 OK - The request completed successfully."},
    { HTTP_STATUS_CREATED,              "HTTP 201", "HTTP 201 CREATED - The request has been fulfilled and resulted in the creation of a new resource."},
    { HTTP_STATUS_ACCEPTED,             "HTTP 202", "HTTP 202 ACCEPTED - The request has been accepted for processing but the processing has not been completed."},
    { HTTP_STATUS_PARTIAL,              "HTTP 203", "HTTP 203 PARTIAL - The returned meta information in the entity-header is not the definitive set available from the origin server."},
    { HTTP_STATUS_NO_CONTENT,           "HTTP 204", "HTTP 204 NO CONTENT - The server has fulfilled the request but there is no new information to send back."},
    { HTTP_STATUS_RESET_CONTENT,        "HTTP 205", "HTTP 205 RESET CONTENT - The request has been completed and the client program should reset the document view that caused the request to be sent to allow the user to easily initiate another input action."},
    { HTTP_STATUS_PARTIAL_CONTENT,      "HTTP 206", "HTTP 206 PARTIAL CONTENT - The server has fulfilled the partial GET request for the resource."},
    { HTTP_STATUS_AMBIGUOUS,            "HTTP 300", "HTTP 300 AMBIGUOUS - The server couldn't decide what to return."},
    { HTTP_STATUS_MOVED,                "HTTP 301", "HTTP 301 MOVED - The requested resource has been assigned to a new permanent URI and any future references to this resource should be done using one of the returned URIs."},
    { HTTP_STATUS_REDIRECT,             "HTTP 302", "HTTP 302 REDIRECT - The requested resource resides temporarily under a different URI."},
    { HTTP_STATUS_REDIRECT_METHOD,      "HTTP 303", "HTTP 303 REDIRECT METHOD - The response to the request can be found under a different URI and should be retrieved using a GET method on that resource."},
    { HTTP_STATUS_NOT_MODIFIED,         "HTTP 304", "HTTP 304 NOT MODIFIED - The requested resource has not been modified."},
    { HTTP_STATUS_USE_PROXY,            "HTTP 305", "HTTP 305 USE PROXY - The requested resource must be accessed through the proxy given by the location field."},
    { HTTP_STATUS_REDIRECT_KEEP_VERB,   "HTTP 307", "HTTP 307 REDIRECT KEEP VERB - The redirected request keeps the same verb. HTTP/1.1 behavior."},
    { HTTP_STATUS_BAD_REQUEST,          "HTTP 400", "HTTP 400 BAD REQUEST- The request could not be processed by the server due to invalid syntax."},
    { HTTP_STATUS_DENIED,               "HTTP 401", "HTTP 401 DENIED -The requested resource requires user authentication."},
    { HTTP_STATUS_PAYMENT_REQ,          "HTTP 402", "HTTP 402 PAYMENT REQ - Not currently implemented in the HTTP protocol."},
    { HTTP_STATUS_FORBIDDEN,            "HTTP 403", "HTTP 403 FORBIDDEN - The server understood the request but is refusing to fulfill it."},
    { HTTP_STATUS_NOT_FOUND,            "HTTP 404", "HTTP 404 NOT FOUND - The server has not found anything matching the requested URI."},
    { HTTP_STATUS_BAD_METHOD,           "HTTP 405", "HTTP 405 BAD METHOD - The method used is not allowed."},
    { HTTP_STATUS_NONE_ACCEPTABLE,      "HTTP 406", "HTTP 406 NONE ACCEPTABLE - No responses acceptable to the client were found."},
    { HTTP_STATUS_PROXY_AUTH_REQ,       "HTTP 407", "HTTP 407 PROXY AUTH REQ - Proxy authentication required."},
    { HTTP_STATUS_REQUEST_TIMEOUT,      "HTTP 408", "HTTP 408 REQUEST TIMEOUT - The server timed out waiting for the request."},
    { HTTP_STATUS_CONFLICT,             "HTTP 409", "HTTP 409 CONFLICT - The request could not be completed due to a conflict with the current state of the resource. The user should resubmit with more information."},
    { HTTP_STATUS_GONE,                 "HTTP 410", "HTTP 410 GONE - The requested resource is no longer available at the server and no forwarding address is known."},
    { HTTP_STATUS_LENGTH_REQUIRED,      "HTTP 411", "HTTP 411 LENGTH REQUIRED - The server refuses to accept the request without a defined content length."},
    { HTTP_STATUS_PRECOND_FAILED,       "HTTP 412", "HTTP 412 PRECONDITION FAILED -The precondition given in one or more of the request header fields evaluted to false when it was tested on the server."},
    { HTTP_STATUS_REQUEST_TOO_LARGE,    "HTTP 413", "HTTP 413 REQUEST TOO LARGE - The server is refusing to process a request because the request entity is larger than the server is willing or able to process."},
    { HTTP_STATUS_URI_TOO_LONG,         "HTTP 414", "HTTP 414 URI TOO LONG - The server is refusing to service the request because the request URI is longer than the server is willing to interpret."},
    { HTTP_STATUS_UNSUPPORTED_MEDIA,    "HTTP 415", "HTTP 415 UNSUPPORTED MEDIA - The server is refusing to service the request because the entity of the request is in a format not supported by the requested resource for the requested method."},
    { HTTP_STATUS_SERVER_ERROR,         "HTTP 500", "HTTP 500 SERVER ERROR - The server encountered an unexpected condition that prevented it from fulfilling the request."},
    { HTTP_STATUS_NOT_SUPPORTED,        "HTTP 501", "HTTP 501 NOT SUPPORTED - The server does not support the functionality required to fulfill the request."},
    { HTTP_STATUS_BAD_GATEWAY,          "HTTP 502", "HTTP 502 BAD GATEWAY - The server while acting as a gateway or proxy received an invalid response from the upstream server it accessed in attempting to fulfill the request."},
    { HTTP_STATUS_SERVICE_UNAVAIL,      "HTTP 503", "HTTP 503 SERVICE UNAVAILABLE - The service is temporarily overloaded."},
    { HTTP_STATUS_GATEWAY_TIMEOUT,      "HTTP 504", "HTTP 504 GATEWAY TIMEOUT - The request was timed out waiting for a gateway."},
    { HTTP_STATUS_VERSION_NOT_SUP,      "HTTP 505", "HTTP 505 VERSION NOT SUPPORTED - The server does not support or refuses to support the HTTP protocol version that was used in the request message."},
    { 0, NULL, NULL}
    };

static const ErrorDescription SockErrors[] = 
    {
    { WSAEINTR,                "Interrupted function call", "WSAEINTR - A blocking operation was interrupted by a call to WSACancelBlockingCall."},
    { WSAEBADF,                "WSAEBADF", "WSAEBADF"},
    { WSAEACCES,               "Permission denied", "WSAEACCES - An attempt was made to access a socket in a way forbidden by its access permissions."},
    { WSAEFAULT,               "Bad address", "WSAEFAULT - The system detected an invalid pointer address in attempting to use a pointer argument of a call."},
    { WSAEINVAL,               "Invalid argument", "WSAEINVAL - Some invalid argument was supplied."},
    { WSAEMFILE,               "Too many open files", "WSAEMFILE - Too many open sockets. Each implementation may have a maximum number of socket handles available either globally per process or per thread."},
    { WSAEWOULDBLOCK,          "Resource temporarily unavailable", "WSAEWOULDBLOCK - This error is returned from operations on non-blocking sockets that cannot be completed immediately."},
    { WSAEINPROGRESS,          "Operation now in progress", "WSAEINPROGRESS - A blocking operation is currently executing."},
    { WSAEALREADY,             "Operation already in progress", "WSAEALREADY - An operation was attempted on a non-blocking socket that already had an operation in progress."},
    { WSAENOTSOCK,             "Socket operation on non-socket", "WSAENOTSOCK - An operation was attempted on something that is not a socket."},
    { WSAEDESTADDRREQ,         "Destination address required", "WSAEDESTADDRREQ - A required address was omitted from an operation on a socket."},
    { WSAEMSGSIZE,             "Message too long", "WSAEMSGSIZE - A message sent on a datagram socket was larger than the internal message buffer or some other network limit or the buffer used to receive a datagram into was smaller than the datagram itself."},
    { WSAEPROTOTYPE,           "Protocol wrong type for socket", "WSAEPROTOTYPE - A protocol was specified in the socket function call that does not support the semantics of the socket type requested."},
    { WSAENOPROTOOPT,          "Bad protocol option", "WSAENOPROTOOPT - An unknown invalid or unsupported option or level was specified in a getsockopt or setsockopt call."},
    { WSAEPROTONOSUPPORT,      "Protocol not supported", "WSAEPROTONOSUPPORT - The requested protocol has not been configured into the system or no implementation for it exists."},
    { WSAESOCKTNOSUPPORT,      "Socket type not supported", "WSAESOCKTNOSUPPORT - The support for the specified socket type does not exist in this address family."},
    { WSAEOPNOTSUPP,           "Operation not supported", "WSAEOPNOTSUPP - The attempted operation is not supported for the type of object referenced."},
    { WSAEPFNOSUPPORT,         "Protocol family not supported", "WSAEPFNOSUPPORT - The protocol family has not been configured into the system or no implementation for it exists."},
    { WSAEAFNOSUPPORT,         "Address family not supported by protocol family", "WSAEAFNOSUPPORT - An address incompatible with the requested protocol was used. All sockets are created with an associated address family (i.e. AF_INET for Internet Protocols) and a generic protocol type (i.e. SOCK_STREAM)."},
    { WSAEADDRINUSE,           "Address already in use", "WSAEADDRINUSE - Only one usage of each socket address (protocol/IP address/port) is normally permitted."},
    { WSAEADDRNOTAVAIL,        "Cannot assign requested address", "WSAEADDRNOTAVAIL - The requested address is not valid in its context. Normally results from an attempt to bind to an address that is not valid for the local machine or connect/sendto an address or port that is not valid for a remote machine."},
    { WSAENETDOWN,             "Network is down", "WSAENETDOWN - A socket operation encountered a dead network. This could indicate a serious failure of the network system (i.e. the protocol stack that the WinSock DLL runs over) the network interface or the local network itself."},
    { WSAENETUNREACH,          "Network is unreachable", "WSAENETUNREACH - A socket operation was attempted to an unreachable network. This usually means the local software knows no route to reach the remote host."},
    { WSAENETRESET,            "Network dropped connection on reset", "WSAENETRESET - The host you were connected to crashed and rebooted. May also be returned by setsockopt if an attempt is made to set SO_KEEPALIVE on a connection that has already failed."},
    { WSAECONNABORTED,         "Software caused connection abort", "WSAECONNABORTED - An established connection was aborted by the software in your host machine possibly due to a data transmission timeout or protocol error."},
    { WSAECONNRESET,           "Connection reset by peer", "WSAECONNRESET - A existing connection was forcibly closed by the remote host."},
    { WSAENOBUFS,              "No buffer space available", "WSAENOBUFS - An operation on a socket could not be performed because the system lacked sufficient buffer space or because a queue was full."},
    { WSAEISCONN,              "Socket is already connected", "WSAEISCONN - A connect request was made on an already connected socket."},
    { WSAENOTCONN,             "Socket is not connected", "WSAENOTCONN - A request to send or receive data was disallowed because the socket is not connected and (when sending on a datagram socket using sendto) no address was supplied. Any other type of operation might also return this error."},
    { WSAESHUTDOWN,            "Cannot send after socket shutdown", "WSAESHUTDOWN - A request to send or receive data was disallowed because the socket had already been shut down in that direction with a previous shutdown call."},
    { WSAETOOMANYREFS,         "WSAETOOMANYREFS", "WSAETOOMANYREFS"},
    { WSAETIMEDOUT,            "Connection timed out", "WSAETIMEDOUT - A connection attempt failed because the connected party did not properly respond after a period of time or established connection failed because connected host has failed to respond."},
    { WSAECONNREFUSED,         "Connection refused", "WSAECONNREFUSED - No connection could be made because the target machine actively refused it."},
    { WSAELOOP,                "WSAELOOP", "WSAELOOP"},
    { WSAENAMETOOLONG,         "WSAENAMETOOLONG", "WSAENAMETOOLONG"},
    { WSAEHOSTDOWN,            "Host is down", "WSAEHOSTDOWN - A socket operation failed because the destination host was down. A socket operation encountered a dead host. Networking activity on the local host has not been initiated."},
    { WSAEHOSTUNREACH,         "No route to host", "WSAEHOSTUNREACH - A socket operation was attempted to an unreachable host. See WSAENETUNREACH."},
    { WSAENOTEMPTY,            "WSAENOTEMPTY", "WSAENOTEMPTY"},
    { WSAEPROCLIM,             "Too many processes", "WSAEPROCLIM - A Windows Sockets implementation may have a limit on the number of applications that may use it simultaneously."},
    { WSAEUSERS,               "WSAEUSERS", "WSAEUSERS"},
    { WSAEDQUOT,               "WSAEDQUOT", "WSAEDQUOT"},
    { WSAESTALE,               "WSAESTALE", "WSAESTALE"},
    { WSASYSNOTREADY,          "Network subsystem is unavailable", "WSASYSNOTREADY - This error is returned by WSAStartup if the Windows Sockets implementation cannot function at this time because the underlying system it uses to provide network services is currently unavailable."},
    { WSAVERNOTSUPPORTED,      "WINSOCK.DLL version out of range", "WSAVERNOTSUPPORTED - The current Windows Sockets implementation does not support the Windows Sockets specification version requested by the application."},
    { WSANOTINITIALISED,       "Successful WSAStartup not yet performed", "WSANOTINITIALISED - Either the application hasn’t called WSAStartup or WSAStartup failed."},
    { WSAEDISCON,              "Graceful shutdown in progress", "WSAEDISCON - Returned by recv WSARecv to indicate the remote party has initiated a graceful shutdown sequence."},
    { WSAENOMORE,              "WSAENOMORE", "WSAENOMORE"},
    { WSAECANCELLED,           "WSAECANCELLED", "WSAECANCELLED"},
    { WSAEINVALIDPROCTABLE,    "Invalid procedure table from service provider", "WSAEINVALIDPROCTABLE - A service provider returned a bogus proc table to WS2_32.DLL."},
    { WSAEINVALIDPROVIDER,     "Invalid service provider version number", "WSAEINVALIDPROVIDER - A service provider returned a version number other than 2.0."},
    { WSAEPROVIDERFAILEDINIT,  "Unable to initialize a service provider", "WSAEPROVIDERFAILEDINIT - Either a service provider's DLL could not be loaded (LoadLibrary failed) or the provider's WSPStartup/NSPStartup function failed."},
    { WSASYSCALLFAILURE,       "System call failure", "WSASYSCALLFAILURE - Returned when a system call that should never fail does."},
    { WSASERVICE_NOT_FOUND,    "WSASERVICE_NOT_FOUND", "WSASERVICE_NOT_FOUND"},
    { WSATYPE_NOT_FOUND,       "WSATYPE_NOT_FOUND", "WSATYPE_NOT_FOUND"},
    { WSA_E_NO_MORE,           "WSA_E_NO_MORE", "WSA_E_NO_MORE"},
    { WSA_E_CANCELLED,         "WSA_E_CANCELLED", "WSA_E_CANCELLED"},
    { WSAEREFUSED,             "WSAEREFUSED", "WSAEREFUSED"},
    { WSAHOST_NOT_FOUND,       "Host not found", "WSAHOST_NOT_FOUND - No such host is known. The name is not an official hostname or alias or it cannot be found in the database(s) being queried."},
    { WSATRY_AGAIN,            "Non-authoritative host not found", "WSATRY_AGAIN - This is usually a temporary error during hostname resolution and means that the local server did not receive a response from an authoritative server. A retry at some time later may be successful."},
    { WSANO_RECOVERY,          "This is a non-recoverable error", "WSANO_RECOVERY - This indicates some sort of non-recoverable error occurred during a database lookup."},
    { WSANO_DATA,              "Valid name no data record of requested type", "WSANO_DATA - The requested name is valid and was found in the database but it does not have the correct associated data being resolved for."},
    { WSANO_ADDRESS,           "WSANO_ADDRESS", "WSANO_ADDRESS"},
//    { WSA_INVALID_HANDLE,      "Specified event object handle is invalid", "WSA_INVALID_HANDLE - An application attempts to use an event object but the specified handle is not valid."},
//    { WSA_INVALID_PARAMETER,   "One or more parameters are invalid", "WSA_INVALID_PARAMETER - An application used a Windows Sockets function which directly maps to a Win32 function. The Win32 function is indicating a problem with one or more parameters."},
//    { WSA_IO_PENDING,          "Overlapped operations will complete later", "WSA_IO_PENDING - The application has initiated an overlapped operation which cannot be completed immediately."},
//    { WSA_IO_INCOMPLETE,       "Overlapped I/O event object not in signaled state", "WSA_IO_INCOMPLETE - The application has tried to determine the status of an overlapped operation which is not yet completed."},
//    { WSA_NOT_ENOUGH_MEMORY,   "Insufficient memory available", "WSA_NOT_ENOUGH_MEMORY - An application used a Windows Sockets function which directly maps to a Win32 function. The Win32 function is indicating a lack of required memory resources."},
//    { WSA_OPERATION_ABORTED,   "Overlapped operation aborted", "WSA_OPERATION_ABORTED - An overlapped operation was canceled due to the closure of the socket or the execution of the SIO_FLUSH command in WSAIoctl."},
    { SEC_E_INSUFFICIENT_MEMORY,    "SEC_E_INSUFFICIENT_MEMORY", "SEC_E_INSUFFICIENT_MEMORY - Not enough memory is available to complete this request."},
    { SEC_E_INVALID_HANDLE,         "SEC_E_INVALID_HANDLE", "SEC_E_INVALID_HANDLE - The handle specified is invalid."},
    { SEC_E_UNSUPPORTED_FUNCTION,   "SEC_E_UNSUPPORTED_FUNCTION", "SEC_E_UNSUPPORTED_FUNCTION - The function requested is not supported."},
    { SEC_E_TARGET_UNKNOWN,         "SEC_E_TARGET_UNKNOWN", "SEC_E_TARGET_UNKNOWN - The specified target is unknown or unreachable."},
    { SEC_E_INTERNAL_ERROR,         "SEC_E_INTERNAL_ERROR", "SEC_E_INTERNAL_ERROR - The Local Security Authority cannot be contacted or an exception occured."},
    { SEC_E_SECPKG_NOT_FOUND,       "SEC_E_SECPKG_NOT_FOUND", "SEC_E_SECPKG_NOT_FOUND - The requested security package does not exist."},
    { SEC_E_NOT_OWNER,              "SEC_E_NOT_OWNER", "SEC_E_NOT_OWNER - The caller is not the owner of the desired credentials."},
    { SEC_E_CANNOT_INSTALL,         "SEC_E_CANNOT_INSTALL", "SEC_E_CANNOT_INSTALL - The security package failed to initialize and cannot be installed."},
    { SEC_E_INVALID_TOKEN,          "SEC_E_INVALID_TOKEN", "SEC_E_INVALID_TOKEN - The token supplied to the function is invalid."},
    { SEC_E_CANNOT_PACK,            "SEC_E_CANNOT_PACK", "SEC_E_CANNOT_PACK - The security package is not able to marshall the logon buffer so the logon attempt has failed."},
    { SEC_E_QOP_NOT_SUPPORTED,      "SEC_E_QOP_NOT_SUPPORTED", "SEC_E_QOP_NOT_SUPPORTED - The per-message Quality of Protection is not supported by the security package."},
    { SEC_E_NO_IMPERSONATION,       "SEC_E_NO_IMPERSONATION", "SEC_E_NO_IMPERSONATION - The security context does not allow impersonation of the client."},
    { SEC_E_LOGON_DENIED,           "SEC_E_LOGON_DENIED", "SEC_E_LOGON_DENIED - The logon attempt failed."},
    { SEC_E_UNKNOWN_CREDENTIALS,    "SEC_E_UNKNOWN_CREDENTIALS", "SEC_E_UNKNOWN_CREDENTIALS - The credentials supplied to the package were not recognized."},
    { SEC_E_NO_CREDENTIALS,         "SEC_E_NO_CREDENTIALS", "SEC_E_NO_CREDENTIALS - No credentials are available in the security package."},
    { SEC_E_MESSAGE_ALTERED,        "SEC_E_MESSAGE_ALTERED", "SEC_E_MESSAGE_ALTERED - The message or signature supplied for verification has been altered."},
    { SEC_E_OUT_OF_SEQUENCE,        "SEC_E_OUT_OF_SEQUENCE", "SEC_E_OUT_OF_SEQUENCE - The message supplied for verification is out of sequence."},
    { SEC_E_NO_AUTHENTICATING_AUTHORITY,    "SEC_E_NO_AUTHENTICATING_AUTHORITY", "SEC_E_NO_AUTHENTICATING_AUTHORITY - No authority could be contacted for authentication."},
    { SEC_I_CONTINUE_NEEDED,        "SEC_I_CONTINUE_NEEDED", "SEC_I_CONTINUE_NEEDED - The function completed successfully but must be called again to complete the context."},
    { SEC_I_COMPLETE_NEEDED,        "SEC_I_COMPLETE_NEEDED", "SEC_I_COMPLETE_NEEDED - The function completed successfully but CompleteToken must be called."},
    { SEC_I_COMPLETE_AND_CONTINUE,  "SEC_I_COMPLETE_AND_CONTINUE", "SEC_I_COMPLETE_AND_CONTINUE - The function completed successfully but both CompleteToken and this function must be called to complete the context."},
    { SEC_I_LOCAL_LOGON,            "SEC_I_LOCAL_LOGON", "SEC_I_LOCAL_LOGON - The logon was completed but no network authority was available.  The logon was made using locally known information."},
    { SEC_E_BAD_PKGID,              "SEC_E_BAD_PKGID", "SEC_E_BAD_PKGID - The requested security package does not exist."},
    { SEC_E_CONTEXT_EXPIRED,        "SEC_E_CONTEXT_EXPIRED", "SEC_E_CONTEXT_EXPIRED - The context has expired and can no longer be used."},
    { SEC_E_INCOMPLETE_MESSAGE,     "SEC_E_INCOMPLETE_MESSAGE", "SEC_E_INCOMPLETE_MESSAGE - The supplied message is incomplete.  The signature was not verified."},
    { SEC_E_INCOMPLETE_CREDENTIALS, "SEC_E_INCOMPLETE_CREDENTIALS", "SEC_E_INCOMPLETE_CREDENTIALS - The credentials supplied were not complete and could not be verified.  The context could not be initialized."},
    { SEC_E_BUFFER_TOO_SMALL,       "SEC_E_BUFFER_TOO_SMALL", "SEC_E_BUFFER_TOO_SMALL - The buffers supplied to a function was too small."},
    { SEC_I_INCOMPLETE_CREDENTIALS, "SEC_I_INCOMPLETE_CREDENTIALS", "SEC_I_INCOMPLETE_CREDENTIALS - The credentials supplied were not complete and could not be verified. Additional information can be returned from the context."},
    { SEC_I_RENEGOTIATE,            "SEC_I_RENEGOTIATE", "SEC_I_RENEGOTIATE - The context data must be renegotiated with the peer."},
    { SEC_E_WRONG_PRINCIPAL,        "SEC_E_WRONG_PRINCIPAL", "SEC_E_WRONG_PRINCIPAL - The target principal name is incorrect."},
    { SEC_I_NO_LSA_CONTEXT,         "SEC_I_NO_LSA_CONTEXT", "SEC_I_NO_LSA_CONTEXT - There is no LSA mode context associated with this context."},
    { SEC_E_TIME_SKEW,              "SEC_E_TIME_SKEW", "SEC_E_TIME_SKEW - The clocks on the client and server machines are skewed."},
    { SEC_E_UNTRUSTED_ROOT,         "SEC_E_UNTRUSTED_ROOT", "SEC_E_UNTRUSTED_ROOT - The certificate received from the remote computer was issued by an untrusted authority."},
//    { SEC_E_CERTIFICATE_REVOKED,    "SEC_E_CERTIFICATE_REVOKED", "SEC_E_CERTIFICATE_REVOKED - The certificate received from the remote computer has been revoked."},
    { SEC_E_CERT_UNKNOWN,           "SEC_E_CERT_UNKNOWN", "SEC_E_CERT_UNKNOWN - The certificate received from the remote computer is unacceptable."},
    { SEC_E_CERT_EXPIRED,           "SEC_E_CERT_EXPIRED", "SEC_E_CERT_EXPIRED - The certificate received from the remote computer has expired or is not currently valid."},
    { 0, NULL, NULL}
    };

static const ErrorDescription SurgeErrors[] = 
    {
    { SURGE_NOCOOKIE,         "No Cookie",              "SURGE_NOCOOKIE - No cookie or not enough cookies were found in the HTTP Response."},
    { SURGE_UX_LOGON,         "Unexpected Logon",       "SURGE_UX_LOGON - A user was logged on when a failure was expected."},
    { SURGE_UX_LOGOFF,        "Unexpected Logoff",      "SURGE_UX_LOGOFF - A user was logged off when a failure was expected."},
    { SURGE_UX_NAV,           "Unexpected Auth",        "SURGE_UX_NAV - A user was authenticated when a failure was expected."},
    { SURGE_UX_PROFILE,       "Unexpected Profile",     "SURGE_UX_PROFILE - A user's profile was modified when a failure was expected."},
    { SURGE_UX_PASSWORD,      "Unexpected Password",    "SURGE_UX_PASSWORD - A user's password was modified when a failure was expected."},
    { SURGE_UX_REGISTER,      "Unexpected Register",    "SURGE_UX_REGISTER - A user was registered when a failure was expected."},
    { SURGE_UX_UNREGISTER,    "Unexpected Unregister",  "SURGE_UX_UNREGISTER - A user was unregisterd when a failure was expected."},
    { 0, NULL, NULL}
    };


/*++

Routine Description:

    ErrorNo
    returns a pointer to the static error description string
    
Arguments:

    int errorClass  -
    long error      -

Return Value:

    char *

Notes:

    pointer to error description string or NULL

--*/
char* ErrorNo(int errorClass, long error)
    {
    if(errorClass == ERRCLASS_SOCK)
        {
        for(unsigned i=0; SockErrors[i].dwError != 0; i++)
            {
            if(SockErrors[i].dwError == error) return SockErrors[i].description;
            }
        }
    else if(errorClass == ERRCLASS_HTTP)
        {
        for(unsigned i=0; HttpErrors[i].dwError != 0; i++)
            {
            if(HttpErrors[i].dwError == error) return HttpErrors[i].description;
            }
        }
    else if(errorClass == ERRCLASS_SURGE)
        {
        for(unsigned i=0; SurgeErrors[i].dwError != 0; i++)
            {
            if(SurgeErrors[i].dwError == error) return SurgeErrors[i].description;
            }
        }

    return NULL;
    }

/*++

Routine Description:

    AddWindowText
    Inserts a string at the end of an edit box or rich edit

Arguments:

    HWND window - handle to the edit box or rich edit to send the text to
    char *text - the string to insert

Return Value:

    NONE

Notes:

--*/
void AddWindowText(HWND window, char *text)
    {
    #define MAX_EDITBOX_LINES 500

    if(!window || !text) return;

    // check to see if the box is full
    if(SendMessage(window, EM_GETLINECOUNT, 0, 0) >= MAX_EDITBOX_LINES)
        {
        long len = SendMessage(window, EM_LINELENGTH, (WPARAM)1, 0) + 2;
        SendMessage(window, EM_SETSEL, 0, len);
        SendMessage(window, EM_REPLACESEL, FALSE, (LPARAM)"");
        }

    // move the caret to the end
    SendMessage(window, EM_SETSEL, (((unsigned long)~0)/2-1), (((unsigned long)~0)/2-1));

    // insert the text
    SendMessage(window, EM_REPLACESEL, FALSE, (LPARAM)text);
    }
