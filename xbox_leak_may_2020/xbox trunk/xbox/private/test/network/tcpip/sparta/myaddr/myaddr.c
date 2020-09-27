#include <windows.h>
#include <winsock2.h>

u_long
GetMyAddr(
)
{
    // hostname is the host name
    char            *hostname = NULL;
    // hostaddr is the host address
    u_long          hostaddr = 0;
    // hostnamelen is the length of the host name buffer
    int             hostnamelen = 0x10;
    // hostent is a pointer to the returned host information
    struct hostent  *hostent = NULL;
    // iResultCode is the result code from gethostname()
    int             iResultCode = 0;
    // iErrorCode is the last error code
    int             iErrorCode = ERROR_SUCCESS;

    do {
        if (NULL == hostname) {
            // Allocate the memory for the standard host name
            hostname = (char *) LocalAlloc(LPTR, hostnamelen);
        }
        else {
            // Increase the memory for the standard host name and reallocate
            hostnamelen += 0x10;
            hostname = (char *) LocalReAlloc(hostname, hostnamelen, LMEM_ZEROINIT);
        }

        if (NULL == hostname) {
            // Get the last error code
            iErrorCode = GetLastError();

            goto FunctionFailed0;
        }

        iResultCode = gethostname(hostname, hostnamelen);
    } while ((SOCKET_ERROR == iResultCode) && (WSAEFAULT == WSAGetLastError()));

    if (SOCKET_ERROR == iResultCode) {
        // Get the last error code
        iErrorCode = WSAGetLastError();

        goto FunctionFailed0;
    }

    // Get the host information for the local machine
    hostent = gethostbyname(hostname);
    if (NULL == hostent) {
        // Get the last error code
        iErrorCode = WSAGetLastError();

        goto FunctionFailed0;
    }

    // Get the host address for the local machine
    CopyMemory(&hostaddr, hostent->h_addr, sizeof(hostaddr));

FunctionFailed0:
    // Free the standard host name
    LocalFree(hostname);

    if (ERROR_SUCCESS != iErrorCode) {
        // Set the last error code
        SetLastError(iErrorCode);
    }

    return hostaddr;
}
