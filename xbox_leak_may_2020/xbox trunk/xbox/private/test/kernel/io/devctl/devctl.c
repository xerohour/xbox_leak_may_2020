/*++

Copyright (c) 1997-2000  Microsoft Corporation

Module Name:

    devctl.c

Abstract:

    This program is designed to call as many of the native NT API's as
    possible. The program is written to crash drivers as its primary function.

Author:

    Neill Clift (neillc) 23-Oct-1997

Environment:

    Xbox

Revision History:

    2-May-2000    schanbai

        Ported to Xbox

--*/


#pragma warning( disable : 4201 4057 )

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "nt.h"
#include "ntrtl.h"
#include "nturtl.h"
#include <xtl.h>
#include "time.h"
#include "xtestlib.h"
#include "xlog.h"


#define MAX_DEVICES                 4096
#define SLOP_SENTINAL               0xA5

#define SLOP                        100
#define BIGBUF_SIZE                 0x10000
#define RAND_REP                    1000
#define MAX_IOCTL_TAILOR            100
#define INITIAL_IOCTL_TAILOR_SIZE   200

typedef struct _DEVMAP {
    OBJECT_NAME_INFORMATION *name;
    FILE_NAME_INFORMATION   *filename;
    HANDLE                  handle;
    DEVICE_TYPE             devtype;
    ACCESS_MASK             access;
} DEVMAP, *PDEVMAP;

//
// Define a structure to keep a track of issued IOCTL's. We do this to try and make a
// guess at what IOCTL's/FSCTL's a driver is actualy processing.
//
typedef struct _IOCTLINFO {
   NTSTATUS status;
   ULONG ioctl;
   ULONG count;
} IOCTLINFO, *PIOCTLINFO;

typedef struct _IOCTLREC {
   ULONG total, count;
   IOCTLINFO ioctl[1];
} IOCTLREC, *PIOCTLREC;

DEVMAP      g_devmap[MAX_DEVICES];
UCHAR       *bigbuf;
HANDLE      randthread, alertthread, mainthread;
ULONG       ioctl_min_function=0;
ULONG       ioctl_max_function=200 /*0xFFF*/;
ULONG       ioctl_min_devtype=0;
ULONG       ioctl_max_devtype=200;
ULONG       max_random_calls   = 100000;
ULONG       max_tailured_calls = 10000;
ULONG       progress_counter=0;
ULONG       alerted=0;
HANDLE      DevCtlLogHandle;
ULONG       MAX_RET;
HANDLE      DevCtlHeapHandle;
PCHAR       DevCtlBigBuffer = NULL;

volatile BOOL ExitFromThread;


/*
   Hack to get a 32 bit random value from a 15 bit source
*/
ULONG
rand32(
    void
    )
{
    return(rand() << 17) + rand() + rand();
}


/*
   Allocate a buffer with slop and fill the slop with a know value
*/
PVOID
reallocslop(
    PVOID  p,
    ULONG  len
    )
{
    progress_counter++;
    if ( !p ) {
        p = HeapAlloc( DevCtlHeapHandle, HEAP_ZERO_MEMORY, len + SLOP );
    } else {
        p = HeapReAlloc( DevCtlHeapHandle, 0, p, len + SLOP );
    }
    memset(p, SLOP_SENTINAL, len + SLOP);
    return p;
}


/*
   Check to see if the driver wrote too far by checking the slop values
*/
VOID
testslop(
    PVOID p,
    ULONG len,
    PCHAR what,
    PCHAR subwhat
)
{
    UCHAR *pc;
    ULONG i;

    pc = p;
    pc += len;

    for (i = 0; i < SLOP; i++, pc++) {
        if (*pc != SLOP_SENTINAL) {
            DbgPrint(
                "DEVCTL: Driver wrote beyond end during %s %s for length %d!\n",
                what,
                subwhat,
                len
                );
            break;
        }
    }
}

/*
   Do volume queries of different lengths
*/
VOID
do_query_volume(
    HANDLE                  handle,
    FS_INFORMATION_CLASS    InfoType,
    ULONG                   bufl,
    PCHAR                   what
)
{
    ULONG                   l, ret;
    IO_STATUS_BLOCK         iosb;
    PVOID                   buf;
    NTSTATUS                status;

    ret = 0;
    buf = NULL;
    do {
       l = bufl + 1024;

       do {
           buf = reallocslop(buf, l);
           status = NtQueryVolumeInformationFile(handle, &iosb, buf, l, InfoType);
           testslop(buf, l, "NtQueryVolumeInformationFile", what);

           if (status == STATUS_NOT_IMPLEMENTED ||
               status == STATUS_INVALID_INFO_CLASS ||
               status == STATUS_INVALID_DEVICE_REQUEST ||
               status == STATUS_INVALID_PARAMETER ||
               status == STATUS_ACCESS_DENIED) {

                //break;
           }
       } while (l-- != 0);

       status = NtQueryVolumeInformationFile(
                    handle,
                    &iosb,
                    (PVOID)-1024,
                    0,
                    InfoType
                    );

    } while (++ret < MAX_RET);

    HeapFree( DevCtlHeapHandle, 0, buf );
}

/*
   Do file queries
*/
VOID
do_query_file(
    HANDLE                  handle,
    FILE_INFORMATION_CLASS  InfoType,
    ULONG                   bufl,
    PCHAR                   what
    )
{
    ULONG                   l, ret;
    IO_STATUS_BLOCK         iosb;
    PVOID                   buf;
    NTSTATUS                status;

    ret = 0;
    buf = NULL;
    do {

       l = bufl + 1024;

       do {
           buf = reallocslop(buf, l);
           status = NtQueryInformationFile(handle, &iosb, buf, l, InfoType);
           testslop(buf, l, "NtQueryInformationFile", what);

           if (status == STATUS_NOT_IMPLEMENTED ||
               status == STATUS_INVALID_INFO_CLASS ||
               status == STATUS_INVALID_DEVICE_REQUEST ||
               status == STATUS_INVALID_PARAMETER ||
               status == STATUS_ACCESS_DENIED) {

                //break;
           }

       } while (l-- != 0);

       status = NtQueryInformationFile(
                    handle,
                    &iosb,
                    (PVOID)-1024,
                    0,
                    InfoType
                    );

    } while (++ret < MAX_RET);

    HeapFree( DevCtlHeapHandle, 0, buf );
}


/*
   Do file sets
*/
VOID
do_set_file(
    HANDLE                  handle,
    FILE_INFORMATION_CLASS  InfoType,
    ULONG                   bufl
    )
{
    ULONG                   l, i, ret;
    IO_STATUS_BLOCK         iosb;
    PVOID                   buf;
    NTSTATUS                status;

    ret = 0;
    buf = NULL;
    do {

       l = bufl + 1024;
       buf = HeapAlloc( DevCtlHeapHandle, HEAP_ZERO_MEMORY, l );
       if ( buf ) {
           do {
               for (i = 0; i < l; i++) {
                  ((PCHAR)buf)[i] = (CHAR) rand ();
               }
               progress_counter++;
               status = NtSetInformationFile(
                            handle,
                            &iosb,
                            buf,
                            l,
                            InfoType
                            );

               if (status == STATUS_NOT_IMPLEMENTED ||
                   status == STATUS_INVALID_INFO_CLASS ||
                   status == STATUS_INVALID_DEVICE_REQUEST ||
                   status == STATUS_INVALID_PARAMETER ||
                   status == STATUS_ACCESS_DENIED) {

                    //break;
               }

           } while (l-- != 0);

           status = NtSetInformationFile(
                        handle,
                        &iosb,
                        (PVOID)-1024,
                        0,
                        InfoType
                        );

           HeapFree( DevCtlHeapHandle, 0, buf );
       }
    } while (++ret < MAX_RET);
}

NTSTATUS
query_object(
    HANDLE  handle,
    PDEVMAP devmap
    )
{
    OBJECT_NAME_INFORMATION *on = NULL;
    FILE_NAME_INFORMATION   *fn = NULL;
    ULONG                   sfn, son;
    FILE_FS_DEVICE_INFORMATION devinfo;
    NTSTATUS                status;
    static IO_STATUS_BLOCK  iosb;

    sfn = sizeof (*fn) + 1024;
    son = sizeof (*on) + 1024;
    fn = reallocslop(NULL, sfn);
    on = reallocslop(NULL, son);

    if (devmap) {
       devmap->filename = fn;
       devmap->name = on;
    }

    if (fn == NULL || on == NULL) {
        KdPrint(( "DEVCTL: Memory allocation failure in query_object!\n" ));
        return STATUS_NO_MEMORY;
    }

    status = NtQueryInformationFile(
                handle,
                &iosb,
                fn,
                sfn,
                FileNameInformation
                );

    testslop(fn, sfn, "NtQueryInformationFile", "FileNameInformation");

    if (NT_SUCCESS(status)) {
        status = iosb.Status;
    }

    if (!NT_SUCCESS(status)) {
        fn->FileNameLength = 0;
    }

    if (!devmap) {
       HeapFree( DevCtlHeapHandle, 0, fn );
       HeapFree( DevCtlHeapHandle, 0, on );
    }

    status = NtQueryVolumeInformationFile(handle,
                                          &iosb,
                                          &devinfo,
                                          sizeof (devinfo),
                                          FileFsDeviceInformation);

    if (NT_SUCCESS(status)) {
        status = iosb.Status;
    }

    if (!NT_SUCCESS(status)) {
        if (devmap) {
           devmap->devtype = 0;
        }
    } else {
        if (devmap) {
           devmap->devtype = devinfo.DeviceType;
        }
//        KdPrint(( "Got the device number for a device!" ));
    }

    //
    // Do loads of different queries with different buffer lengths.
    //
    do_query_file(handle, FileBasicInformation, sizeof(FILE_BASIC_INFORMATION), "FileBasicInformation");
    do_query_file(handle, FileStandardInformation, sizeof(FILE_STANDARD_INFORMATION), "FileStandardInformation");
    do_query_file(handle, FileInternalInformation, sizeof(FILE_INTERNAL_INFORMATION), "FileInternalInformation");
    do_query_file(handle, FileEaInformation, sizeof(FILE_EA_INFORMATION), "FileEaInformation");
    do_query_file(handle, FileAccessInformation, sizeof(FILE_ACCESS_INFORMATION), "FileAccessInformation");
    do_query_file(handle, FileNameInformation, sizeof(FILE_NAME_INFORMATION) + 1024, "FileNameInformation");
    do_query_file(handle, FileModeInformation, sizeof(FILE_MODE_INFORMATION), "FileModeInformation");
    do_query_file(handle, FileAlignmentInformation, sizeof(FILE_ALIGNMENT_INFORMATION), "FileAlignmentInformation");
    do_query_file(handle, FileAllInformation, sizeof(FILE_ALL_INFORMATION), "FileAllInformation");
    do_query_file(handle, FileStreamInformation, sizeof(FILE_STREAM_INFORMATION), "FileStreamInformation");
    do_query_file(handle, FilePipeInformation, sizeof(FILE_PIPE_INFORMATION), "FilePipeInformation");
    do_query_file(handle, FilePipeLocalInformation, sizeof (FILE_PIPE_LOCAL_INFORMATION), "FilePipeLocalInformation");
    do_query_file(handle, FilePipeRemoteInformation, sizeof(FILE_PIPE_REMOTE_INFORMATION), "FilePipeRemoteInformation");
    do_query_file(handle, FileCompressionInformation, sizeof(FILE_COMPRESSION_INFORMATION), "FileCompressionInformation");
    do_query_file(handle, FileNetworkOpenInformation, sizeof(FILE_NETWORK_OPEN_INFORMATION), "FileNetworkOpenInformation");
    do_query_file(handle, FileAttributeTagInformation, sizeof(FILE_ATTRIBUTE_TAG_INFORMATION), "FileAttributeTagInformation");

    do_set_file(handle, FileBasicInformation, sizeof(FILE_BASIC_INFORMATION));
    do_set_file(handle, FileLinkInformation, sizeof(FILE_LINK_INFORMATION));
    do_set_file(handle, FileDispositionInformation, sizeof(FILE_DISPOSITION_INFORMATION));
    do_set_file(handle, FilePositionInformation, sizeof(FILE_POSITION_INFORMATION));
    do_set_file(handle, FileAllocationInformation, sizeof(FILE_ALLOCATION_INFORMATION));
    do_set_file(handle, FileEndOfFileInformation, sizeof(FILE_END_OF_FILE_INFORMATION));
    do_set_file(handle, FileMoveClusterInformation, sizeof(FILE_MOVE_CLUSTER_INFORMATION));

    do_query_volume(handle, FileFsVolumeInformation, sizeof(FILE_FS_VOLUME_INFORMATION) + 1024, "FileFsVolumeInformation");
    do_query_volume(handle, FileFsSizeInformation, sizeof(FILE_FS_SIZE_INFORMATION), "FileFsSizeInformation" );
    do_query_volume(handle, FileFsDeviceInformation, sizeof(FILE_FS_DEVICE_INFORMATION) + 1024, "FileFsDeviceInformation");
    do_query_volume(handle, FileFsAttributeInformation, sizeof(FILE_FS_ATTRIBUTE_INFORMATION), "FileFsAttributeInformation");
    do_query_volume(handle, FileFsControlInformation, sizeof(FILE_FS_CONTROL_INFORMATION), "FileFsControlInformation");
    do_query_volume(handle, FileFsFullSizeInformation, sizeof(FILE_FS_SIZE_INFORMATION), "FileFsFullSizeInformation");
    do_query_volume(handle, FileFsObjectIdInformation, sizeof(FILE_FS_OBJECTID_INFORMATION) + 1024, "FileFsObjectIdInformation");

    return status;
}

/*
   Do the fast queries on the open path
*/
NTSTATUS
try_fast_query_delete_etc(
    POBJECT_ATTRIBUTES poa
    )
{
    PVOID fi = NULL;
    NTSTATUS status;
    ULONG ret;

    status = STATUS_SUCCESS;

    ret = 0;
    do {
        fi = reallocslop(fi, sizeof (FILE_NETWORK_OPEN_INFORMATION));
        status = NtQueryFullAttributesFile(poa, fi);
        testslop(fi, sizeof (FILE_NETWORK_OPEN_INFORMATION), "NtQueryFullAttributesFile", "");
    } while (++ret < MAX_RET);

    ret = 0;
    do {
        status = NtDeleteFile(poa);
    } while (++ret < MAX_RET);

    HeapFree( DevCtlHeapHandle, 0, fi );
    return status;
}

/*
   Do a whole bunch of random things
*/
NTSTATUS misc_functions(
    HANDLE handle,
    ULONG sync
    )
{
    IO_STATUS_BLOCK iosb;
    NTSTATUS status = STATUS_SUCCESS;
    PVOID buf;
    ULONG bufl;
    LONG i;
    LARGE_INTEGER bo;
    ULONG ret;

    buf = HeapAlloc( DevCtlHeapHandle, HEAP_ZERO_MEMORY, bufl = 1024 );
    if (buf == 0) {
       KdPrint(( "DEVCTL: Failed to allocate buffer!\n" ));
       return STATUS_NO_MEMORY;
    }

    i = bufl;

    if (!sync) {
         ret = 0;
         do {
            progress_counter++;
            bo.QuadPart = 0;
            status = NtReadFile (handle, NULL, NULL, NULL, &iosb, buf, i,
                                 &bo, NULL);
            if (NT_SUCCESS (status))
               status = iosb.Status;
            progress_counter++;
            bo.QuadPart = 0x7FFFFFFFFFFFFFFF - i + 1;
            status = NtReadFile (handle, NULL, NULL, NULL, &iosb, buf, i,
                                 &bo, NULL);
            if (NT_SUCCESS (status))
               status = iosb.Status;
         } while (++ret < MAX_RET);

          ret = 0;
          do {
             progress_counter++;
             bo.QuadPart = 0;
             NtWriteFile (handle, NULL, NULL, NULL, &iosb, buf, i, &bo, NULL);

             //
             // Wrap to negative call
             //
             progress_counter++;
             bo.QuadPart = 0x7FFFFFFFFFFFFFFF - i + 1;
             NtWriteFile (handle, NULL, NULL, NULL, &iosb, buf, i,
                                   &bo, NULL);
             //
             // Do an append call.
             //
             progress_counter++;
             bo.QuadPart = -1;
             NtWriteFile (handle, NULL, NULL, NULL, &iosb, buf, i, &bo, NULL);
          } while (++ret < MAX_RET);
    }

       ret = 0;
       do {
          progress_counter++;
          NtFlushBuffersFile (handle, &iosb);
       } while (++ret < MAX_RET);

       {
       ULONG_PTR first = 1, j, datalen, l;
       CHAR bufn[1024];
       PFILE_NAMES_INFORMATION tfni;

       ret = 0;
       do {
          progress_counter++;
          status = NtQueryDirectoryFile (handle, NULL, NULL, NULL, &iosb, buf, i,
                                         FileNamesInformation, FALSE, NULL, TRUE);
          if ( NT_SUCCESS(status) && first && status != STATUS_PENDING ) {
             first = 0;
             datalen = iosb.Information;
             for (j = 0; j < datalen; j += tfni->NextEntryOffset) {
                tfni = (PFILE_NAMES_INFORMATION)((PCHAR)buf + j);
                memset (bufn, 0, sizeof (bufn));
                l = tfni->FileNameLength / sizeof (WCHAR);
                if (l >= sizeof (bufn) / sizeof (bufn[0]))
                   l = sizeof (bufn) / sizeof (bufn[0]) - 1;
                strncpy (bufn, tfni->FileName, l);
                KdPrint(( "DEVCTL: -> %s\n", bufn ));
                if (tfni->NextEntryOffset == 0)
                   break;
             }
          }
       } while (++ret < MAX_RET);
        }

       ret = 0;
       do {
          progress_counter++;
          NtQueryDirectoryFile (handle, NULL, NULL, NULL, &iosb, buf, i,
                                         FileDirectoryInformation, FALSE, NULL, TRUE);
       } while (++ret < MAX_RET);

       ret = 0;
       do {
          progress_counter++;
          NtQueryDirectoryFile (handle, NULL, NULL, NULL, &iosb, buf, i,
                                         FileFullDirectoryInformation, FALSE, NULL, TRUE);
       } while (++ret < MAX_RET);

       ret = 0;
       do {
          progress_counter++;
          NtQueryDirectoryFile (handle, NULL, NULL, NULL, &iosb, buf, i,
                                         FileBothDirectoryInformation, FALSE, NULL, TRUE);
       } while (++ret < MAX_RET);

       ret = 0;
       do {
          progress_counter++;
          NtQueryDirectoryFile (handle, NULL, NULL, NULL, &iosb, buf, i,
                                         FileObjectIdInformation, FALSE, NULL, TRUE);
       } while (++ret < MAX_RET);

       ret = 0;
       do {
          progress_counter++;
          NtQueryDirectoryFile (handle, NULL, NULL, NULL, &iosb, buf, i,
                                         FileQuotaInformation, FALSE, NULL, TRUE);
       } while (++ret < MAX_RET);

       ret = 0;
       do {
          progress_counter++;
          NtQueryDirectoryFile (handle, NULL, NULL, NULL, &iosb, buf, i,
                                         FileReparsePointInformation, FALSE, NULL, TRUE);
       } while (++ret < MAX_RET);

       ret = 0;

    HeapFree( DevCtlHeapHandle, 0, buf );
    return status;
}

//
// Do a load of opens etc relative from the current handle
//
NTSTATUS
do_sub_open_etc(
    HANDLE handle,
    PCHAR s
)
{
    OBJECT_ATTRIBUTES oa;
    OBJECT_STRING name;
    NTSTATUS status;
    IO_STATUS_BLOCK iosb;
    PCHAR as;

    if (strlen (s) > 30) {
       as = "Big...";
    } else {
       as = s;
    }

    RtlInitObjectString (&name, s);
    InitializeObjectAttributes(&oa,
                               &name,
                               OBJ_CASE_INSENSITIVE,
                               handle,
                               NULL);
    status = NtCreateFile(&handle,
                          MAXIMUM_ALLOWED,
                          &oa,
                          &iosb,
                          NULL,
                          0,
                          0,
                          FILE_OPEN,
                          0,
                          NULL,
                          0);

    if (NT_SUCCESS (status)) {
       status = iosb.Status;
    }

    if (NT_SUCCESS (status)) {
       query_object(handle, NULL);
       misc_functions (handle, 0);
       status = NtClose(handle);
       if (!NT_SUCCESS (status)) {
          KdPrint(( "DEVCTL: NtClose failed %s\n", NtStatusSymbolicName(status) ));
       }
    }

    try_fast_query_delete_etc (&oa);
    return status;
}
//
// Try a few opens relative to the device its self.
//
NTSTATUS
try_funny_opens(
    HANDLE handle
)
{
    ULONG ret, i;
    static ULONG bigl;

    if (DevCtlBigBuffer == NULL) {
       DevCtlBigBuffer = HeapAlloc( DevCtlHeapHandle, HEAP_ZERO_MEMORY, bigl = 0x10000 );
       if (DevCtlBigBuffer == NULL) {
          KdPrint(( "DEVCTL: Memory allocation failure in try_funny_opens!\n" ));
          return STATUS_NO_MEMORY;
       }
       bigl /= sizeof (DevCtlBigBuffer[0]);

       for (i = 0; i < bigl; i++)
          DevCtlBigBuffer[i] = 'A';
       DevCtlBigBuffer[bigl - 3] = '\0';
    }
    ret = 0;
    do {
       do_sub_open_etc (handle, "");
       do_sub_open_etc (handle, " ");
       do_sub_open_etc (handle, "\\");
       do_sub_open_etc (handle, "\\\\\\\\\\\\");
       do_sub_open_etc (handle, DevCtlBigBuffer);
       do_sub_open_etc (handle, ":");
       do_sub_open_etc (handle, " :");
       do_sub_open_etc (handle, ": ");
       do_sub_open_etc (handle, ": ");
       do_sub_open_etc (handle, "::");
       do_sub_open_etc (handle, ": :");
       do_sub_open_etc (handle, "::$UNUSED");
       do_sub_open_etc (handle, "::$STANDARD_INFORMATION");
       do_sub_open_etc (handle, "::$ATTRIBUTE_LIST");
       do_sub_open_etc (handle, "::$FILE_NAME");
       do_sub_open_etc (handle, "::$OBJECT_ID");
       do_sub_open_etc (handle, "::$SECURITY_DESCRIPTOR");
       do_sub_open_etc (handle, "::$VOLUME_NAME");
       do_sub_open_etc (handle, "::$VOLUME_INFORMATION");
       do_sub_open_etc (handle, "::$DATA");
       do_sub_open_etc (handle, "::$INDEX_ROOT");
       do_sub_open_etc (handle, "::$INDEX_ALLOCATION");
       do_sub_open_etc (handle, "::$BITMAP");
       do_sub_open_etc (handle, "::$REPARSE_POINT");
       do_sub_open_etc (handle, "::$EA_INFORMATION");
       do_sub_open_etc (handle, "::$PROPERTY_SET");
       do_sub_open_etc (handle, "::$FIRST_USER_DEFINED_ATTRIBUTE");
       do_sub_open_etc (handle, "::$END");
    } while (++ret < MAX_RET);
    return 0;
}

VOID
randomize_buf(
    PVOID buf,
    ULONG bufl
)
{
   ULONG i;
   PUCHAR pc = buf;

   for (i = 0; i < bufl; i++) {
      *pc++ = (UCHAR)(rand() & 0xff);
   }
}

/*

   Thread used to randomize buffers
*/
VOID
WINAPI
randomize(
    PVOID buf
)
{
   ULONG i;
   PUCHAR pc;

   while ( ExitFromThread == FALSE ) {
      pc = buf;
      try {
          for (i = 0; i < BIGBUF_SIZE; i++) {
              *pc++ = (UCHAR)(rand() & 0xff);
          }
      } except (EXCEPTION_EXECUTE_HANDLER) {
      }
      SleepEx (0, TRUE);
   }
}


VOID
WINAPI
AlerterThread(
    HANDLE MainThreadHandle
)

/*++

Routine Description:

    Thread used to keep the main thread moving if it gets stuck in a synchronouse I/O

Arguments:

    MainThreadHandle - thread handle of main thread

Return Value:

    None

--*/

{
    NTSTATUS Status;
    ULONG last = progress_counter;

    while ( ExitFromThread == FALSE ) {

        last = progress_counter;
        SleepEx( 5000, TRUE );
        if ( progress_counter == 0 || progress_counter != last ) {
            continue;
        }

        Status = NtAlertThreadEx ( MainThreadHandle, UserMode );
        if ( !NT_SUCCESS(Status) ) {
            KdPrint(( "DEVCTL: NtAlertThreadEx failed %s\n", NtStatusSymbolicName(Status) ));
        }

        alerted++;
    }
}

void record_ioctl (PIOCTLREC *piorec, ULONG ioctl, NTSTATUS status)
{
   PIOCTLREC iorec = *piorec;
   ULONG i, j;

   if (!iorec) {
      iorec = HeapAlloc( DevCtlHeapHandle, HEAP_ZERO_MEMORY, sizeof (*iorec) + \
                      INITIAL_IOCTL_TAILOR_SIZE * sizeof (iorec->ioctl[0]) );
      if (!iorec) {
         return;
      }
      *piorec = iorec;
      iorec->total = INITIAL_IOCTL_TAILOR_SIZE;
      iorec->count = 0;
   }
   for (i = 0; i < iorec->count; i++) {
      if (iorec->ioctl[i].ioctl == ioctl && iorec->ioctl[i].status == status) {
         return;
      }
      if (iorec->ioctl[i].status == status) {
         if (iorec->ioctl[i].count > MAX_IOCTL_TAILOR) {
            return; // too many seen of this one
         }
         if (++iorec->ioctl[i].count > MAX_IOCTL_TAILOR) {
            for (j = i + 1; j < iorec->count; j++) {
               if (iorec->ioctl[j].status == status) {
                  iorec->ioctl[j] = iorec->ioctl[--iorec->count];
                  j--;
               }
            }
            return;
         }
         break;
      }
   }
   if (iorec->total == iorec->count) {
      iorec = HeapReAlloc( DevCtlHeapHandle, 0, iorec, sizeof (*iorec) + \
                       iorec->total * 2 * sizeof (iorec->ioctl[0]) );
      if (!iorec) {
         return;
      }
      *piorec = iorec;
      iorec->total *= 2;
   }
   i = iorec->count;
   iorec->ioctl[i].status = status;
   iorec->ioctl[i].ioctl = ioctl;
   iorec->ioctl[i].count = 0;
   iorec->count = i + 1;
}

void do_tailored_ioctl (PDEVMAP                 devmap,
                        PIOCTLREC               iorec,
                        ULONG fsctl)
{
   ULONG                   method, ioctl_val;
   PVOID                   inbuf, outbuf;
   ULONG                   inlen, outlen;
   static IO_STATUS_BLOCK  static_iosb;
   ULONG                   ret;
   ULONG                   i, j, k;
   NTSTATUS                status;

   if (!iorec) {
      return;
   }

   for (i = 0; i < iorec->count; i++) {
      if (iorec->ioctl[i].count >= MAX_IOCTL_TAILOR)
         continue;
      ioctl_val = iorec->ioctl[i].ioctl;
      method = ioctl_val&0x3;
      alerted = 0;
      for (j = 0; j < max_tailured_calls; j += RAND_REP) {
         ret = 0;
         do {
            for (k = 0; k < RAND_REP; k++) {
               switch(method) {
                  case METHOD_BUFFERED :

                     inlen = rand()&(BIGBUF_SIZE - 1);
                     outlen = rand()&(BIGBUF_SIZE - 1);

                     inbuf = bigbuf;
                     outbuf = bigbuf;

                     randomize_buf (inbuf, inlen);
                     break;

                  case METHOD_IN_DIRECT :
                  case METHOD_OUT_DIRECT :

                     inlen = rand()&(BIGBUF_SIZE - 1);
                     outlen = rand()&(BIGBUF_SIZE - 1);

                     inbuf = bigbuf;
                     outbuf = &bigbuf[BIGBUF_SIZE - outlen];
                     randomize_buf (inbuf, inlen);
                     randomize_buf (outbuf, outlen);

                     break;

                  case METHOD_NEITHER :
                  default :

                     inlen = rand32();
                     outlen = rand32();

                     inbuf = (PVOID)rand32();
                     outbuf = (PVOID)rand32();

                     break;
               }

               progress_counter++;
               if (!fsctl) {
                  status = NtDeviceIoControlFile(devmap->handle,
                                                 NULL,
                                                 NULL,
                                                 NULL,
                                                 &static_iosb,
                                                 ioctl_val,
                                                 inbuf,
                                                 inlen,
                                                 outbuf,
                                                 outlen);
               } else {
                  status = NtFsControlFile(devmap->handle,
                                           NULL,
                                           NULL,
                                           NULL,
                                           &static_iosb,
                                           ioctl_val,
                                           inbuf,
                                           inlen,
                                           outbuf,
                                           outlen);
               }
            }
            if (alerted > 5) {
               break;
            }
         } while (++ret < MAX_RET);
         if (alerted > 5)
            break;

      }
   }
}

NTSTATUS
do_ioctl(
    PDEVMAP                 devmap
)
{
    ULONG                   function, method=0, access, i, j, ioctl_val=0;
    static IO_STATUS_BLOCK  static_iosb;
    PVOID                   inbuf, outbuf;
    ULONG                   inlen, outlen;
    ULONG                   tmp;
    NTSTATUS                status;
    ULONG                   devtype;
    ULONG                   ret;
    BOOL                    hit_leak;
    PIOCTLREC               iorec = NULL, fsrec = NULL;

    //
    // do I/O calls with no buffer
    //
    for (function = ioctl_min_function; function <= ioctl_max_function; function++) {
      for (devtype = ioctl_min_devtype; devtype <= ioctl_max_devtype; devtype++) {
          ret = 0;
          do {
             hit_leak = FALSE;
             for (access = FILE_ANY_ACCESS;
                  access <= (devmap->access&(FILE_READ_ACCESS|FILE_WRITE_ACCESS));
                  access++) {
                 for (method = 0; method < 4; method++) {
                     ioctl_val = CTL_CODE(devtype, function, method, access);

                     progress_counter++;
                    status = NtDeviceIoControlFile(devmap->handle,
                                                   NULL, NULL, NULL,
                                                   &static_iosb,
                                                   ioctl_val,
                                                   (PVOID)bigbuf,
                                                   0,
                                                   (PVOID)bigbuf,
                                                   0);
                    record_ioctl (&iorec, ioctl_val, status);

                    status = NtFsControlFile(devmap->handle, NULL, NULL, NULL,
                                             &static_iosb,
                                             ioctl_val,
                                             (PVOID)bigbuf,
                                             0,
                                             (PVOID)bigbuf,
                                             0);
                    record_ioctl (&fsrec, ioctl_val, status);

                    Sleep (0);
                 }
             }
         } while (++ret < MAX_RET);
      }
    }


   status = NtResumeThread (randthread, &tmp);
   if (!NT_SUCCESS (status)) {
      KdPrint(( "DEVCTL: NtResumeThread failed %s\n", NtStatusSymbolicName(status) ));
   }
   for (i = 0; i < max_random_calls; i += RAND_REP) {
       if (ioctl_min_function >= ioctl_max_function)
          function = ioctl_min_function;
       else
          function = ioctl_min_function +
                     (rand() % (ioctl_max_function - ioctl_min_function));
       ret = 0;
       do {
          for (j = 0; j < RAND_REP; j++) {
             if (ioctl_min_devtype >= ioctl_max_devtype)
                devtype = ioctl_min_devtype;
             else
                devtype = ioctl_min_devtype +
                           (rand() % (ioctl_max_devtype - ioctl_min_devtype));
             method = rand() & 0x3;

             access = rand() & devmap->access&(FILE_READ_ACCESS|FILE_WRITE_ACCESS);

             ioctl_val = CTL_CODE(devtype,
                                  function,
                                  method,
                                  access);

             switch(method) {
                 case METHOD_BUFFERED :

                     inlen = rand()&(BIGBUF_SIZE - 1);
                     outlen = rand()&(BIGBUF_SIZE - 1);

                     inbuf = bigbuf;
                     outbuf = bigbuf;

                     break;

                 case METHOD_IN_DIRECT :
                 case METHOD_OUT_DIRECT :

                     inlen = rand()&(BIGBUF_SIZE - 1);
                     outlen = rand()&(BIGBUF_SIZE - 1);

                     inbuf = bigbuf;
                     outbuf = &bigbuf[BIGBUF_SIZE - outlen];
//                       KdPrint(( "%p %d %p %d", inbuf, inlen, outbuf, outlen ));

                     break;

                 case METHOD_NEITHER :
                 default :

                     inlen = rand32();
                     outlen = rand32();

                     inbuf = (PVOID)rand32();
                     outbuf = (PVOID)rand32();

                     break;
             }

             progress_counter++;
                NtDeviceIoControlFile(devmap->handle,
                                      NULL,
                                      NULL,
                                      NULL,
                                      &static_iosb,
                                      ioctl_val,
                                      inbuf,
                                      inlen,
                                      outbuf,
                                      outlen);
                record_ioctl (&iorec, ioctl_val, status);

                NtFsControlFile(devmap->handle,
                                NULL,
                                NULL,
                                NULL,
                                &static_iosb,
                                ioctl_val,
                                inbuf,
                                inlen,
                                outbuf,
                                outlen);
                record_ioctl (&fsrec, ioctl_val, status);
         }

      } while (++ret < MAX_RET);
   }

   status = NtSuspendThread (randthread, &tmp);
   if (!NT_SUCCESS (status)) {
      KdPrint(( "DEVCTL: NtSuspendThread failed %s\n", NtStatusSymbolicName(status) ));
   }

   do_tailored_ioctl (devmap, iorec, 0);
   do_tailored_ioctl (devmap, fsrec, 1);

    if (iorec) {
       HeapFree( DevCtlHeapHandle, 0, iorec );
    }
    if (fsrec) {
       HeapFree( DevCtlHeapHandle, 0, fsrec );
    }
    return 0;
}

/*
   Open device with various options
*/
NTSTATUS
open_device(
    HANDLE                  parent,
    POBJECT_STRING          name,
    PDEVMAP                 devmap,
    BOOL                    synch,        // Do a synchronous open
    BOOL                    direct        // Do a direct open
)
{
    NTSTATUS                  status;
    IO_STATUS_BLOCK           iosb;
    ULONG                     l, lw;
    CHAR                      dn[1024];
    OBJECT_STRING             dnu;
    ULONG                     options;
    OBJECT_ATTRIBUTES         oa;
    ACCESS_MASK               am;
    ULONG                     share=0;

    l = name->Length;
    if (l >= sizeof (dn))
       l = sizeof (dn) - 1;
    lw = l / sizeof (dn[0]);
    strncpy (dn, name->Buffer, lw);
    dn[lw] = '\0';
    RtlInitObjectString (&dnu, dn);
    dn[min(dnu.Length, sizeof (dn) - sizeof (dn[0]))/sizeof(dn[0])] = 0;

    InitializeObjectAttributes(&oa,
                               name,
                               OBJ_CASE_INSENSITIVE,
                               parent,
                               NULL);

    devmap->handle = NULL;

    if (direct) {
        options = 0;
        am = SYNCHRONIZE|FILE_READ_ATTRIBUTES|READ_CONTROL|WRITE_OWNER|WRITE_DAC;
    } else if (synch) {
        options = FILE_SYNCHRONOUS_IO_ALERT;
        am = MAXIMUM_ALLOWED|SYNCHRONIZE;
    } else {
        options = 0;
        am = MAXIMUM_ALLOWED;
    }

    do {
        status = NtCreateFile(
                    &devmap->handle,
                    am,
                    &oa,
                    &iosb,
                    NULL,
                    0,
                    share,
                    FILE_OPEN,
                    options,
                    NULL,
                    0
                    );

        if (status == STATUS_SHARING_VIOLATION) {
            if (share&FILE_SHARE_READ) {
                share &= ~FILE_SHARE_READ;
                if (share&FILE_SHARE_WRITE) {
                   share &= ~FILE_SHARE_WRITE;
                   if (share&FILE_SHARE_DELETE) {
                      break;
                   } else {
                      share |= FILE_SHARE_DELETE;
                   }
                } else {
                   share |= FILE_SHARE_WRITE;
                }
             } else {
                share |= FILE_SHARE_READ;
             }
          } else if (status == STATUS_ACCESS_DENIED) {
             if (am&MAXIMUM_ALLOWED) {
                am &= ~MAXIMUM_ALLOWED;
                am |= FILE_READ_DATA|FILE_WRITE_DATA|SYNCHRONIZE|FILE_READ_ATTRIBUTES|
                      READ_CONTROL|FILE_APPEND_DATA;
             } else if (am&FILE_WRITE_DATA) {
                am &= ~FILE_WRITE_DATA;
             } else if (am&FILE_APPEND_DATA) {
                am &= ~FILE_APPEND_DATA;
             } else if (am&FILE_READ_DATA) {
                am &= ~FILE_READ_DATA;
             } else if (am&WRITE_OWNER) {
                am &= ~WRITE_OWNER;
             } else if (am&WRITE_DAC) {
                am &= ~WRITE_DAC;
             } else if (am&WRITE_DAC) {
                am &= ~WRITE_DAC;
             } else if (am&READ_CONTROL) {
                am &= ~READ_CONTROL;
             } else if (am&FILE_READ_ATTRIBUTES) {
                am &= ~FILE_READ_ATTRIBUTES;
             } else {
                break;
             }
          }
    } while (status == STATUS_SHARING_VIOLATION || status == STATUS_ACCESS_DENIED);

    if (NT_SUCCESS(status)) {
        query_object(devmap->handle, devmap);
        misc_functions (devmap->handle, synch);
    } else {
        if (status != STATUS_INVALID_DEVICE_REQUEST &&
            status != STATUS_ACCESS_DENIED) {
            KdPrint(( "DEVCTL: NtCreateFile failed %s\n", NtStatusSymbolicName(status) ));
        }
    }

    if ( (direct && synch) || (!synch && !direct) ) {
        // Only do this twice
        try_fast_query_delete_etc (&oa);
    }

    return status;
}


NTSTATUS
do_device_opens(
    HANDLE handle,
    POBJECT_STRING name,
    PDEVMAP devmap,
    PULONG  devscount
    )

/*++

Routine Description:

   Do all the various different opens looking for handles

Arguments:

Return Value:

--*/

{
    NTSTATUS Status;

    Status = open_device(
                handle,
                name,
                &devmap[*devscount],
                TRUE,   /* Synchronous           */
                FALSE   /* No direct device open */
                );

    if ( NT_SUCCESS(Status) ) {
        try_funny_opens( devmap[*devscount].handle );
        do_ioctl( &devmap[*devscount] );
        *devscount = *devscount + 1;
        if ( *devscount == MAX_DEVICES ) {
            return Status;
        }
    } else {
        KdPrint((
            "DEVCTL: failed to open device %Z (sync, indirect, %s)\n",
            name,
            NtStatusSymbolicName(Status)
            ));
    }

    Status = open_device(
                handle,
                name,
                &devmap[*devscount],
                FALSE,  /* Synchronous             */
                FALSE   /* No direct device access */
                );

    if ( NT_SUCCESS(Status) ) {
        try_funny_opens( devmap[*devscount].handle );
        do_ioctl( &devmap[*devscount] );
        *devscount = *devscount + 1;
    } else {
        KdPrint((
            "DEVCTL: failed to opened device %Z (async, indirect, %s)\n",
            name,
            NtStatusSymbolicName(Status)
            ));
    }

    Status = open_device(
                handle,
                name,
                &devmap[*devscount],
                TRUE,  /* Synchronous          */
                TRUE   /* Direct device access */
                );

    if ( NT_SUCCESS(Status) ) {
        try_funny_opens( devmap[*devscount].handle );
        do_ioctl( &devmap[*devscount] );
        *devscount = *devscount + 1;
    } else {
        KdPrint((
            "DEVCTL: failed to opened device %Z (sync, direct, %s)\n",
            name,
            NtStatusSymbolicName(Status)
            ));
    }

    return Status;
}


NTSTATUS
recurse(
    HANDLE           handle,
    POBJECT_STRING   name,
    PDEVMAP          devmap,
    PULONG           devscount,
    PCHAR            path
    )

/*++

Routine Description:

    Traverse the object tree looking for devices

Arguments:

Return Value:

--*/

{
    HANDLE nhandle;
    NTSTATUS Status;
    OBJECT_ATTRIBUTES oa;
    OBJECT_DIRECTORY_INFORMATION *od;
    PVOID buffer;
    ULONG bufferlen;
    ULONG retlen, Context=0;
    PCHAR npath = NULL;

    bufferlen = 0x2000;
    buffer = HeapAlloc( DevCtlHeapHandle, HEAP_ZERO_MEMORY, bufferlen );

    if ( !buffer ) {
        return STATUS_NO_MEMORY;
    }

    InitializeObjectAttributes(
        &oa,
        name,
        OBJ_CASE_INSENSITIVE,
        handle,
        NULL
        );

    Status = NtOpenDirectoryObject( &nhandle, &oa );

    if ( NT_SUCCESS(Status) ) {
        do {
            //
            // Under Xbox, NtQueryDirectoryObject returns single entry at a time
            //
            Status = NtQueryDirectoryObject(
                        nhandle,
                        buffer,
                        bufferlen,
                        FALSE,
                        &Context,
                        &retlen
                        );

            if ( Status == STATUS_BUFFER_TOO_SMALL ) {
                PVOID pv = HeapReAlloc(
                                DevCtlHeapHandle,
                                HEAP_ZERO_MEMORY,
                                buffer,
                                bufferlen + 0x1000
                                );
                KdPrint(( "DEVCTL: buffer too small, trying %lu\n", bufferlen + 0x1000 ));
                if ( pv ) {
                    bufferlen += 0x1000;
                    buffer = pv;
                    continue;
                }
            }

            if ( NT_SUCCESS(Status) ) {
                od = (POBJECT_DIRECTORY_INFORMATION)buffer;
                if ( od->Name.Length ) {
                    size_t len = strlen(path);
                    npath = HeapAlloc( DevCtlHeapHandle, 0, len + 1 + od->Name.Length + 1);

                    if (!npath) {
                       KdPrint(( "DEVCTL: memory allocation failed for path buffer! "));
                       Status = STATUS_NO_MEMORY;
                       break;
                    }

                    strcpy (npath, path);
                    if ( len == 0 || (len && path[len-1] != '\\') ) {
                        strcat(npath, "\\");
                    }

                    memcpy( npath+len, od->Name.Buffer, od->Name.Length );
                    *(npath+len+od->Name.Length) = 0;

                    if (od->Type == 'eriD') {

                        recurse(nhandle,
                                &od->Name,
                                devmap,
                                devscount,
                                npath
                                );

                    } else if ( od->Type == 'iveD' ) {
                        Status = do_device_opens (nhandle,
                                                  &od->Name,
                                                  devmap,
                                                  devscount
                                                  );
                    }
                }
            } else if ( !NT_SUCCESS(Status) && Status != STATUS_NO_MORE_ENTRIES ) {
                KdPrint(( "DEVCTL: NtQueryDirectoryObject failed (%s)\n", NtStatusSymbolicName(Status) ));
            }

        } while ( NT_SUCCESS(Status) );

        HeapFree( DevCtlHeapHandle, 0, buffer );
        NtClose(nhandle);
    } else {
        KdPrint((
            "DEVCTL: NtOpenDirectoryObject failed %Z (%s)\n",
            name,
            NtStatusSymbolicName(Status)
            ));
    }

    HeapFree( DevCtlHeapHandle, 0, npath );
    return Status;
}


VOID
AlertExitThread(
    HANDLE ThreadHandle
    )
{
    ULONG PreviousSuspendCount;
    NTSTATUS Status;

    do {
        Status = NtResumeThread( ThreadHandle, &PreviousSuspendCount );
    } while ( NT_SUCCESS(Status) && PreviousSuspendCount );

    QueueUserAPC((PAPCFUNC)ExitThread, ThreadHandle, 0 );
    NtAlertThreadEx( ThreadHandle, UserMode );
    SetThreadPriority( ThreadHandle, THREAD_PRIORITY_HIGHEST );

    while ( WaitForSingleObject(ThreadHandle, 5000) == WAIT_TIMEOUT ) {
        KdPrint(( "DEVCTL: timeout while waiting for thread to be terminated\n" ));
#if DBG
        __asm int 3;
#endif
    }

    CloseHandle( ThreadHandle );
}


VOID
WINAPI
DevCtlStartTest(
    HANDLE hLog
    )

/*++

Routine Description:

    This is your test entry point. It returns only when all tests have
    completed. If you create threads, you must wait until all threads have
    completed. You also have to create your own heap using HeapAlloc, do not
    use GlobalAlloc or LocalAlloc.

    Please see "x-box test harness.doc" for more information

Arguments:

    DevCtlLogHandle - Handle used to call logging APIs. Please see "logging.doc"
        for more information regarding logging APIs

Return Value:

    None

--*/

{
    ULONG i, devscount;
    OBJECT_STRING ObjS;
    NTSTATUS Status;

    KdPrint(( "DEVCTL: StartTest\n" ));
    ASSERT( DevCtlLogHandle != INVALID_HANDLE_VALUE );
    DevCtlLogHandle = hLog;

    MAX_RET = GetProfileIntA( "devctl", "maxretry", 500 );

    xSetComponent( DevCtlLogHandle, "Kernel", "Io" );
    xSetFunctionName( DevCtlLogHandle, "stress" );
    xStartVariation( DevCtlLogHandle, "I/O stress" );

    srand( (unsigned)time(NULL) );

    __try {

        bigbuf = NULL;
        randthread = NULL;
        alertthread = NULL;
        mainthread = NULL;
        ExitFromThread = FALSE;

        bigbuf = VirtualAlloc( NULL, BIGBUF_SIZE+SLOP, MEM_COMMIT, PAGE_READWRITE );
        if ( !bigbuf ) {
            KdPrint(( "DEVCTL: unable to allocate memory\n" ));
            __leave;
        }

        randthread = CreateThread( NULL, 0, (LPTHREAD_START_ROUTINE)randomize, bigbuf, 0, NULL );
        if ( !randthread ) {
            __leave;
        }

        Status = NtDuplicateObject( NtCurrentThread(), &mainthread, DUPLICATE_SAME_ACCESS );

        if ( !NT_SUCCESS(Status) ) {
            KdPrint(( "DEVCTL: NtDuplicateObject failed %s\n", NtStatusSymbolicName(Status) ));
            __leave;
        }

        alertthread = CreateThread( NULL, 0, (LPTHREAD_START_ROUTINE)AlerterThread, mainthread, 0, NULL );
        if ( !alertthread ) {
            KdPrint(( "DEVCTL: failed to create alerting thread %s\n", WinErrorSymbolicName(GetLastError()) ));
            __leave;
        }

        Status = NtSuspendThread( randthread, NULL );
        if ( !NT_SUCCESS(Status) ) {
            KdPrint(( "DEVCTL: NtSuspendThread failed %s\n", NtStatusSymbolicName(Status) ));
        }

        devscount = 0;
        RtlInitObjectString( &ObjS, OTEXT("\\") );
        recurse( 0, &ObjS, g_devmap, &devscount, "" );

        for ( i=0; i<devscount; i++ ) {
            HeapFree( DevCtlHeapHandle, 0, g_devmap[i].name );
            g_devmap[i].name = NULL;
            HeapFree( DevCtlHeapHandle, 0, g_devmap[i].filename );
            g_devmap[i].filename = NULL;
            NtClose(g_devmap[i].handle);
        }

        devscount = 0;

        InterlockedExchange( (LPLONG)&ExitFromThread, TRUE );

    } __finally {

        if ( randthread ) {
            AlertExitThread( randthread );
        }

        if ( alertthread ) {
            AlertExitThread( alertthread );
        }

        if ( mainthread ) {
            NtClose( mainthread );
        }

        if ( bigbuf ) {
            VirtualFree( bigbuf, 0, MEM_RELEASE );
        }
    }

    xEndVariation( DevCtlLogHandle );
}


VOID
WINAPI
DevCtlEndTest(
    VOID
    )

/*++

Routine Description:

    This is your test clean up routine. It will be called only ONCE after
    all threads have done. This will allow you to do last chance clean up.
    Do not put any per-thread clean up code here. It will be called only
    once after you have finished your StartTest.

Arguments:

    None

Return Value:

    None

--*/

{
    KdPrint(( "DEVCTL: EndTest\n" ));
}


BOOL
WINAPI
DevCtlDllMain(
    HINSTANCE   hInstance,
    DWORD       fdwReason,
    LPVOID      lpReserved
    )
{
    UNREFERENCED_PARAMETER( hInstance );
    UNREFERENCED_PARAMETER( lpReserved );

    if ( fdwReason == DLL_PROCESS_ATTACH ) {
        DevCtlHeapHandle = HeapCreate( 0, 0, 0 );
    } else if ( fdwReason == DLL_PROCESS_DETACH ) {
        DevCtlBigBuffer = NULL;
        HeapDestroy( DevCtlHeapHandle );
    }

    return DevCtlHeapHandle != NULL;
}


//
// Export function pointers of StartTest and EndTest
//

#pragma data_seg( EXPORT_SECTION_NAME )
DECLARE_EXPORT_DIRECTORY( devctl )
#pragma data_seg()

BEGIN_EXPORT_TABLE( devctl )
    EXPORT_TABLE_ENTRY( "StartTest", DevCtlStartTest )
    EXPORT_TABLE_ENTRY( "EndTest", DevCtlEndTest )
    EXPORT_TABLE_ENTRY( "DllMain", DevCtlDllMain )
END_EXPORT_TABLE( devctl )
