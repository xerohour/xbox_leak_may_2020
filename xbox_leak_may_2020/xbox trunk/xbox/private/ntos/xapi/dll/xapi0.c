#include "dllp.h"
#include <xcrypt.h>
#include "dm.h"

extern ULONG XapiTlsSize;

void _cinit(void);
void _rtinit(void);

int __cdecl main(int argc, ...);

DWORD WINAPI mainXapiStartup(
    LPVOID lpvParameter
    )
/*++

Routine Description:

    Starting point for the app's first thread

Arguments:

    lpvParameter - Unused.

Return Value:

    Returns the exit code of the app, it turns into the exit code of the thread.

--*/
{
    XapiInitProcess();

    {
        /* Tell the debugger where some of our data is*/
        PDMXAPI pdmxapi;
        pdmxapi = DmGetCurrentXapi();
        if(pdmxapi) {
            extern __declspec(thread) DWORD XapiLastErrorCode;
            pdmxapi->LastErrorTlsOff = (ULONG_PTR)&XapiLastErrorCode -
                (ULONG_PTR)KeGetCurrentThread()->TlsData;
            pdmxapi->XapiStarted = TRUE;
        }
    }

    _rtinit();
    _cinit();

    main(
           0, // argc
           NULL, // argv
           NULL // envp
         );

    RIP("Main thread should not return.");

    XapiBootToDash(XLD_LAUNCH_DASHBOARD_ERROR, XLD_ERROR_INVALID_XBE, 0);

    return STATUS_SUCCESS;
}

VOID __cdecl mainCRTStartup(
    VOID
    )
/*++

Routine Description:

    Entrypoint for xapi.dll, initializes api + crt components, launches the
	app

Arguments:

    None.

Return Value:

    None.

--*/
{
    HANDLE hThread;
    PUCHAR EndOfHeaders;
    PXBEIMAGE_CERTIFICATE Certificate;
    ULONG MaxSizeOfCertificate;

    /* Verify that the XBE certificate is wholly contained within the signed
     * XBE headers (the kernel only validated the first
     * XBEIMAGE_CERTIFICATE_BASE_SIZEOF bytes.  If it's too big, we truncate
     * the SizeOfCertificate */
    EndOfHeaders = (PUCHAR)XeImageHeader() + XeImageHeader()->SizeOfHeaders;
    Certificate = XeImageHeader()->Certificate;
    MaxSizeOfCertificate = EndOfHeaders - (PUCHAR)Certificate;
    if(MaxSizeOfCertificate < Certificate->SizeOfCertificate)
        Certificate->SizeOfCertificate = MaxSizeOfCertificate;

    /* Set the TLS index, if we have TLS */
    XapiTlsSize = (_tls_used.EndAddressOfRawData -
        _tls_used.StartAddressOfRawData) + _tls_used.SizeOfZeroFill;
    /* The base of the TLS section must be 16-byte aligned.  We know
     * that fs:StackBase is 16-byte aligned, ensuring that TlsSize is
     * 16-byte aligned is sufficient */
    XapiTlsSize = (XapiTlsSize + 15) & ~15;
    /* Add four bytes for the index pointer */
    XapiTlsSize += 4;
    *(PULONG)(_tls_used.AddressOfIndex) = (int)XapiTlsSize / -4;

    // Create the app's primary thread
    hThread = CreateThread(
                            NULL,
                            0,
                            mainXapiStartup,
                            NULL,
                            0,
                            NULL
                          );

    if (NULL == hThread)
    {
        XapiBootToDash(XLD_LAUNCH_DASHBOARD_ERROR, XLD_ERROR_INVALID_XBE, 0);
    }

    CloseHandle(hThread);
}
