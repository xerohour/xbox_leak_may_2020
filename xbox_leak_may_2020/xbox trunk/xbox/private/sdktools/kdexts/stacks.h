/*++

Copyright (c) 1998  Microsoft Corporation

Module Name:

    stacks.h

Abstract:

    This file contains the stack walking scripts for !stacks

Author:

    Adrian J. Oney (adriao) 07-28-1998

Environment:

    User Mode.

Revision History:

--*/

//
// This tree describes a tree of functions that will be "drilled" through when
// !stacks is used to give a summary of what each thread is currently doing...
//
BEGIN_TREE() ;
  DECLARE_ENTRY("ntoskrnl!KiSwapThread") ;
  BEGIN_LIST() ;   
    DECLARE_ENTRY("ntoskrnl!KeRemoveQueue") ;
    DECLARE_ENTRY("ntoskrnl!KeWaitForSingleObject") ;
    BEGIN_LIST() ;
      DECLARE_ENTRY("ntoskrnl!NtReplyWaitReceivePort") ;
      BEGIN_LIST() ;
        DECLARE_ENTRY("ntoskrnl!KiSystemService") ;
        BEGIN_LIST() ;
          DECLARE_ENTRY("ntoskrnl!ZwReplyWaitReceivePort") ;
        END_LIST() ;      
      END_LIST() ;      
      DECLARE_ENTRY("ntoskrnl!ExpWaitForResource") ;
      BEGIN_LIST() ;
        DECLARE_ENTRY("ntoskrnl!ExAcquireResourceSharedLite") ;
        DECLARE_ENTRY("ntoskrnl!ExpAcquireResourceExclusiveLite") ;
        BEGIN_LIST() ;
          DECLARE_ENTRY("ntoskrnl!ExAcquireResourceExclusiveLite") ;
        END_LIST() ;
      END_LIST() ;
      DECLARE_ENTRY("ntoskrnl!NtWaitForSingleObject") ;
      BEGIN_LIST() ;
        DECLARE_ENTRY("ntoskrnl!KiSystemService") ;
      END_LIST() ;      
    END_LIST() ;
    DECLARE_ENTRY("ntoskrnl!KeWaitForMultipleObjects") ;
    BEGIN_LIST() ;
      DECLARE_ENTRY("ntoskrnl!NtWaitForMultipleObjects") ;
      BEGIN_LIST() ;
        DECLARE_ENTRY("ntoskrnl!KiSystemService") ;
      END_LIST() ;      
    END_LIST() ;
  END_LIST() ;
  
  DECLARE_ENTRY("ntoskrnl!KiSwapThreadExit") ;
  BEGIN_LIST() ;   
    DECLARE_ENTRY("ntoskrnl!KeRemoveQueue") ;
    DECLARE_ENTRY("ntoskrnl!KeWaitForSingleObject") ;
    BEGIN_LIST() ;
      DECLARE_ENTRY("ntoskrnl!NtReplyWaitReceivePort") ;
      BEGIN_LIST() ;
        DECLARE_ENTRY("ntoskrnl!KiSystemService") ;
        BEGIN_LIST() ;
          DECLARE_ENTRY("ntoskrnl!ZwReplyWaitReceivePort") ;
        END_LIST() ;      
      END_LIST() ;      
      DECLARE_ENTRY("ntoskrnl!ExpWaitForResource") ;
      BEGIN_LIST() ;
        DECLARE_ENTRY("ntoskrnl!ExAcquireResourceSharedLite") ;
        DECLARE_ENTRY("ntoskrnl!ExpAcquireResourceExclusiveLite") ;
        BEGIN_LIST() ;
          DECLARE_ENTRY("ntoskrnl!ExAcquireResourceExclusiveLite") ;
        END_LIST() ;
      END_LIST() ;
      DECLARE_ENTRY("ntoskrnl!NtWaitForSingleObject") ;
      BEGIN_LIST() ;
        DECLARE_ENTRY("ntoskrnl!KiSystemService") ;
      END_LIST() ;      
    END_LIST() ;
    DECLARE_ENTRY("ntoskrnl!KeWaitForMultipleObjects") ;
    BEGIN_LIST() ;
      DECLARE_ENTRY("ntoskrnl!NtWaitForMultipleObjects") ;
      BEGIN_LIST() ;
        DECLARE_ENTRY("ntoskrnl!KiSystemService") ;
      END_LIST() ;      
    END_LIST() ;
  END_LIST() ;
  
  DECLARE_ENTRY("ntoskrnl!KiUnlockDispatcherDatabase") ;  
  BEGIN_LIST() ;   
    DECLARE_ENTRY("ntoskrnl!KeSetEvent") ;
  END_LIST() ;

  DECLARE_ENTRY("ntkrnlmp!KiSwapThread") ;
  BEGIN_LIST() ;   
    DECLARE_ENTRY("ntkrnlmp!KeRemoveQueue") ;
    DECLARE_ENTRY("ntkrnlmp!KeWaitForSingleObject") ;
    BEGIN_LIST() ;
      DECLARE_ENTRY("ntkrnlmp!NtReplyWaitReceivePort") ;
      BEGIN_LIST() ;
        DECLARE_ENTRY("ntkrnlmp!KiSystemService") ;
        BEGIN_LIST() ;
          DECLARE_ENTRY("ntkrnlmp!ZwReplyWaitReceivePort") ;
        END_LIST() ;      
      END_LIST() ;      
      DECLARE_ENTRY("ntkrnlmp!ExpWaitForResource") ;
      BEGIN_LIST() ;
        DECLARE_ENTRY("ntkrnlmp!ExAcquireResourceSharedLite") ;
        DECLARE_ENTRY("ntkrnlmp!ExpAcquireResourceExclusiveLite") ;
        BEGIN_LIST() ;
          DECLARE_ENTRY("ntkrnlmp!ExAcquireResourceExclusiveLite") ;
        END_LIST() ;
      END_LIST() ;
      DECLARE_ENTRY("ntkrnlmp!NtWaitForSingleObject") ;
      BEGIN_LIST() ;
        DECLARE_ENTRY("ntkrnlmp!KiSystemService") ;
      END_LIST() ;      
    END_LIST() ;
    DECLARE_ENTRY("ntkrnlmp!KeWaitForMultipleObjects") ;
    BEGIN_LIST() ;
      DECLARE_ENTRY("ntkrnlmp!NtWaitForMultipleObjects") ;
      BEGIN_LIST() ;
        DECLARE_ENTRY("ntkrnlmp!KiSystemService") ;
      END_LIST() ;      
    END_LIST() ;
  END_LIST() ;
  
  DECLARE_ENTRY("ntkrnlmp!KiSwapThreadExit") ;
  BEGIN_LIST() ;   
    DECLARE_ENTRY("ntkrnlmp!KeRemoveQueue") ;
    DECLARE_ENTRY("ntkrnlmp!KeWaitForSingleObject") ;
    BEGIN_LIST() ;
      DECLARE_ENTRY("ntkrnlmp!NtReplyWaitReceivePort") ;
      BEGIN_LIST() ;
        DECLARE_ENTRY("ntkrnlmp!KiSystemService") ;
        BEGIN_LIST() ;
          DECLARE_ENTRY("ntkrnlmp!ZwReplyWaitReceivePort") ;
        END_LIST() ;      
      END_LIST() ;      
      DECLARE_ENTRY("ntkrnlmp!ExpWaitForResource") ;
      BEGIN_LIST() ;
        DECLARE_ENTRY("ntkrnlmp!ExAcquireResourceSharedLite") ;
        DECLARE_ENTRY("ntkrnlmp!ExpAcquireResourceExclusiveLite") ;
        BEGIN_LIST() ;
          DECLARE_ENTRY("ntkrnlmp!ExAcquireResourceExclusiveLite") ;
        END_LIST() ;
      END_LIST() ;
      DECLARE_ENTRY("ntkrnlmp!NtWaitForSingleObject") ;
      BEGIN_LIST() ;
        DECLARE_ENTRY("ntkrnlmp!KiSystemService") ;
      END_LIST() ;      
    END_LIST() ;
    DECLARE_ENTRY("ntkrnlmp!KeWaitForMultipleObjects") ;
    BEGIN_LIST() ;
      DECLARE_ENTRY("ntkrnlmp!NtWaitForMultipleObjects") ;
      BEGIN_LIST() ;
        DECLARE_ENTRY("ntkrnlmp!KiSystemService") ;
      END_LIST() ;      
    END_LIST() ;
  END_LIST() ;
  
  DECLARE_ENTRY("ntkrnlmp!KiUnlockDispatcherDatabase") ;  
  BEGIN_LIST() ;   
    DECLARE_ENTRY("ntkrnlmp!KeSetEvent") ;
  END_LIST() ;

END_TREE() ;

