
//
//  coversys.h
//
//  Must include either <winioctl.h> (for user-mode app) or <ntddk.h>
//  (kernel-mode driver) to get the CTL_CODE macro definition.
//

#ifndef _COVERSYS_H_
#define _COVERSYS_H_

#ifndef CTL_CODE
#error "Must include either winioctl.h or ntddk.h before including coversys.h"
#endif

#define IOCTL_COVERAGE_ENUM_BINARIES                    CTL_CODE( FILE_DEVICE_UNKNOWN, 0x87A, METHOD_NEITHER, FILE_READ_ACCESS )
#define IOCTL_COVERAGE_POLL_LOGGING                     CTL_CODE( FILE_DEVICE_UNKNOWN, 0x87B, METHOD_NEITHER, FILE_READ_ACCESS | FILE_WRITE_ACCESS )
#define IOCTL_COVERAGE_FLUSH_LOGGING                    CTL_CODE( FILE_DEVICE_UNKNOWN, 0x87C, METHOD_NEITHER, FILE_WRITE_ACCESS )
#define IOCTL_COVERAGE_REGISTER_NO_IMPORT_BINARY        CTL_CODE( FILE_DEVICE_UNKNOWN, 0x87D, METHOD_NEITHER, FILE_WRITE_ACCESS )
#define IOCTL_COVERAGE_REGISTER_ALL_NO_IMPORT_BINARIES  CTL_CODE( FILE_DEVICE_UNKNOWN, 0x87E, METHOD_NEITHER, FILE_WRITE_ACCESS )

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  IOCTL_COVERAGE_ENUM_BINARIES                                              //
//                                                                            //
//      IN  pInBuffer   -- not used                                           //
//      IN  cbInBuffer  -- not used                                           //
//      OUT pOutBuffer  -- buffer to return list of binaries                  //
//      IN  cbOutBuffer -- size of pOutBuffer in bytes                        //
//      OUT cbOutActual -- actual size of data returned in pOutBuffer.        //
//                                                                            //
//      This returns the list of registered logging binaries as a NULL-       //
//      terminated sequence of NULL-terminated strings and MD5s               //
//      For example:                                                          //
//      name1<0>MD5_1<0>name2<0>MD5_2<0>name3<0>MD5_3<0><0>.                  //
//      This list of binaries can be used                                     //
//      in subsequent calls to IOCTL_COVERAGE_POLL_LOGGING and                //
//      IOCTL_COVERAGE_FLUSH_LOGGING.  Binary names are always returned       //
//      in lowercase.                                                         //
//                                                                            //
//  IOCTL_COVERAGE_POLL_LOGGING                                               //
//                                                                            //
//      IN  pInBuffer   -- (LPCSTR) pszBinaryName (must be lowercase)         //
//      OUT cbInBuffer  -- (PDWORD) returned checksum of original binary      //
//      OUT pOutBuffer  -- buffer to return logging data                      //
//      IN  cbOutBuffer -- size of pOutBuffer in bytes                        //
//      OUT cbOutActual -- actual size of data returned in pOutBuffer.        //
//                                                                            //
//      This reads the current logging information for a given binary into    //
//      a user supplied buffer and resets the logging buffer to zero, so      //
//      subsequent reads will return only new logging information.  Note      //
//      that logging is not disabled during the read, so no hits are lost.    //
//                                                                            //
//  IOCTL_COVERAGE_FLUSH_LOGGING                                              //
//                                                                            //
//      IN  pInBuffer   -- (LPCSTR) pszBinaryName OPTIONAL (lowercase)        //
//      IN  cbInBuffer  -- not used.                                          //
//      OUT pOutBuffer  -- not used.                                          //
//      IN  cbOutBuffer -- not used.                                          //
//      OUT cbOutActual -- not used.                                          //
//                                                                            //
//      This flushes the logging buffer for the given binary, or for all      //
//      registered binaries if pInBuffer (pszBinaryName) is NULL.  It is      //
//      not necessary to do a flush after a poll, but it might be useful      //
//      to issue a flush before starting a new testing scenario.              //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#if !defined(_NTDDK_) || defined(COVERMON)

//
//  Define user-mode interface for kernel-mode coverage driver.
//

HANDLE
__inline
KernelCoverageConnectToDriver(
    VOID
    )
    {
    HANDLE hDeviceHandle;

    hDeviceHandle = CreateFile(
                         OTEXT("Coverage:"),
                         GENERIC_READ | GENERIC_WRITE,
                         0,  // exclusive access
                         NULL,
                         OPEN_EXISTING,
                         FILE_FLAG_NO_BUFFERING,
                         NULL
                         );

    if ( hDeviceHandle == INVALID_HANDLE_VALUE )
         hDeviceHandle = NULL;

    return hDeviceHandle;
    }

BOOL
__inline
KernelCoverageEnumerateBinaries(
    IN  HANDLE hKernelCoverageConnectionHandle,
    IN  DWORD  dwSizeOfBuffer,
    OUT LPSTR  pListOfNamesBuffer,      // name1<0>name2<0>name3<0><0>
    OUT PDWORD pdwActualSize
    )
    {
    return DeviceIoControl(
               hKernelCoverageConnectionHandle,
               IOCTL_COVERAGE_ENUM_BINARIES,
               NULL,
               0,
               pListOfNamesBuffer,
               dwSizeOfBuffer,
               pdwActualSize,
               NULL
               );
    }

BOOL
__inline
KernelCoveragePollLogging(
    IN  HANDLE  hKernelCoverageConnectionHandle,
    IN  LPCSTR  pszBinaryName,
    IN  DWORD   dwSizeOfBuffer,
    OUT PVOID   pLoggingBuffer,
    OUT PDWORD  pdwActualSize,
    OUT PDWORD  pdwCheckSumOfBinary
    )
    {
    return DeviceIoControl(
               hKernelCoverageConnectionHandle,
               IOCTL_COVERAGE_POLL_LOGGING,
               (PVOID)pszBinaryName,
               (DWORD)pdwCheckSumOfBinary,
               pLoggingBuffer,
               dwSizeOfBuffer,
               pdwActualSize,
               NULL
               );
    }

BOOL
__inline
KernelCoverageFlushLogging(
    IN HANDLE hKernelCoverageConnectionHandle,
    IN LPCSTR pszBinaryName OPTIONAL
    )
    {
    DWORD dwActualSize;     // not used, but required by DeviceIoControl()

    return DeviceIoControl(
               hKernelCoverageConnectionHandle,
               IOCTL_COVERAGE_FLUSH_LOGGING,
               (PVOID)pszBinaryName,
               0,
               NULL,
               0,
               &dwActualSize,
               NULL
               );
    }

VOID
__inline
KernelCoverageDisconnect(
    IN HANDLE hKernelCoverageConnectionHandle
    )
    {
    CloseHandle( hKernelCoverageConnectionHandle );
    }

BOOL
__inline
KernelRegisterNoImportCoverageBinary(
    IN HANDLE hKernelCoverageConnectionHandle,
    IN HANDLE ImageBase
    )
    {
    DWORD BytesReturned;

    return DeviceIoControl(
               hKernelCoverageConnectionHandle,
               IOCTL_COVERAGE_REGISTER_NO_IMPORT_BINARY,
               (PVOID)ImageBase,
               sizeof(HANDLE),
               NULL,
               0,
               &BytesReturned,
               NULL
               );
    }

BOOL
__inline
KernelRegisterAllNoImportCoverageBinaries(
    IN HANDLE hKernelCoverageConnectionHandle
    )
    {
    DWORD BytesReturned;

    return DeviceIoControl(
               hKernelCoverageConnectionHandle,
               IOCTL_COVERAGE_REGISTER_ALL_NO_IMPORT_BINARIES,
               NULL,
               0,
               NULL,
               0,
               &BytesReturned,
               NULL
               );
    }

#endif /* ! _NTDDK_ */

#endif /* _COVERSYS_H_ */

