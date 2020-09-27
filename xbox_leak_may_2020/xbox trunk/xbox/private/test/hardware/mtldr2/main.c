#include "xtl.h"
#include "mfgtest.h"


void __cdecl main()
{
    HANDLE h;
    DWORD size;
    PBYTE buf;
    DWORD read;


    h = CreateFile ("D:\\FOO.XBE", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (h != INVALID_HANDLE_VALUE) {

        size = GetFileSize (h, NULL);
        buf = malloc (size * sizeof (BYTE));

        if (buf) {

            if (ReadFile (h, buf, size, &read, NULL) && read == size) {

                CloseHandle (h);
                if (MfgTestCopyImageToDrive ("Z:\\BAR.XBE", buf, size)) {

                    MfgTestReboot ("Z:\\BAR.XBE");
                }

            }

        }
    }
}

