#define BUILD_XIPSIGN
#include <windows.h>
#include <xcrypt.h>
#include <xip.h>
#include <stdio.h>
#include <stdlib.h>

int __cdecl main(int argc, char **argv)
{
    XIP_PROTECTION xipp;
    XIP_SIG xsig;
    HANDLE hOut;
    HANDLE hIn = INVALID_HANDLE_VALUE;
    int nRet = 1;
    BYTE rgb[65536];
    DWORD cb;
    int i;
    LPCSTR szOutputName;
    int ich;

    if(argc < 3) {
        fprintf(stderr, "usage: xipsign <output-file> <input.xip> ...\n");
        exit(1);
    }

    argc -= 2;
    argv += 2;

    __try {
        szOutputName = argv[-1];
        hOut = CreateFile(szOutputName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
            0, NULL);
        if(hOut == INVALID_HANDLE_VALUE) {
            fprintf(stderr, "error X0000: cannot open %s\n", szOutputName);
            __leave;
        }

        while(argc--) {
            hIn = CreateFile(*argv++, GENERIC_READ, 0, NULL, OPEN_EXISTING,
                0, NULL);
            if(hIn == INVALID_HANDLE_VALUE) {
                fprintf(stderr, "error X0000: cannot open %s\n", argv[-1]);
                __leave;
            }

            /* Construct the sig header.  Note that we're stupid about the
             * filename and always use the pathname we were given -- we don't
             * strip off any directories */
            memset(&xipp, 0, sizeof xipp);
            for(ich = 0; argv[-1][ich] && ich < (sizeof xipp.Name / sizeof xipp.Name[0] - 1); ++ich)
                xipp.Name[ich] = argv[-1][ich];
            xipp.SignatureCount = (GetFileSize(hIn, NULL) + 65535) / 65536;

            /* Write out this portion of the header */
            if(!WriteFile(hOut, &xipp, sizeof xipp, &cb, NULL)) {
writeerr:
                fprintf(stderr, "error X0000: error writing to %s\n",
                    szOutputName);
                __leave;
            }

            /* Read the data in 64k chunks and write out each digest we get */
            for(i = xipp.SignatureCount; i--; ) {
                if(!ReadFile(hIn, rgb, sizeof rgb, &cb, NULL) ||
                    (cb < sizeof rgb && i != 0))
                {
                    fprintf(stderr, "error X0000: error reading %s\n",
                        argv[-1]);
                    __leave;
                }
                printf("%s chunk %d: %d\n", argv[-1], xipp.SignatureCount - i - 1,
                    cb);
                XCCalcDigest(rgb, cb, xsig.Signature);
                if(!WriteFile(hOut, &xsig, sizeof xsig, &cb, NULL))
                    goto writeerr;
            }
            CloseHandle(hIn);
            hIn = INVALID_HANDLE_VALUE;
        }

        /* Spit out an empty header to mark the end of the list */
        memset(&xipp, 0, sizeof xipp);
        if(!WriteFile(hOut, &xipp, sizeof xipp, &cb, NULL))
            goto writeerr;
        nRet = 0;
    } except(EXCEPTION_EXECUTE_HANDLER) {
        fprintf(stderr, "error X0000: exception %08X occurred\n",
            GetExceptionCode());
    }

    if(hIn != INVALID_HANDLE_VALUE)
        CloseHandle(hIn);

    CloseHandle(hOut);

    if(nRet)
        DeleteFile(szOutputName);

    return nRet;
}
