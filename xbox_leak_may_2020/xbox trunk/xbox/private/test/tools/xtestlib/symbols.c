#include "precomp.h"

#include <ntstatus.dbg>
#include <winerror.dbg>



LPSTR
WINAPI
NtStatusSymbolicName(
    NTSTATUS  Status
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Converts status code to symbolic name

Arguments:

  Status - NT status code

Return Value:

  Pointer to symbolic name of specified status code

------------------------------------------------------------------------------*/
{
    int i = 0;

    while (ntstatusSymbolicNames[i].SymbolicName) {
        if (ntstatusSymbolicNames[i].MessageId == Status) {
            return ntstatusSymbolicNames[i].SymbolicName;
        }
        ++i;
    }

    return "No Symbolic Name";
}



LPSTR
WINAPI
WinErrorSymbolicName(
    DWORD  LastError
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Converts error code to symbolic name

Arguments:

  LastError - Error code returned by GetLastError()

Return Value:

  Pointer to symbolic name of specified error code

------------------------------------------------------------------------------*/
{
    int i = 0;

    while (winerrorSymbolicNames[i].SymbolicName) {
        if (winerrorSymbolicNames[i].MessageId == LastError) {
            return winerrorSymbolicNames[i].SymbolicName;
        }
        ++i;
    }

    return "No Symbolic Name";
}
