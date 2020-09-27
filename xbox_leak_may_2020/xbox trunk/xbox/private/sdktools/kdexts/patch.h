/*++

Copyright (c) 1998  Microsoft Corporation

Module Name:

    patch.h

Abstract:

    This file contains the assertion scripts for !Patch

Author:

    Adrian J. Oney (adriao) 07-28-1998

Environment:

    User Mode.

Revision History:

--*/

#if 0

#ifndef NO_SPECIAL_IRP

//
// Every patch must belong to a category
//
BEGIN_CATEGORY("WDM Driver Correctness") ;

  //
  // Non-exclusive means the children can be turned on and off individually
  // (like check boxes). In exclusive lists, only one option can be on at
  // once (ie, radio buttons).
  //
  BEGIN_SIBLING_LIST(LISTSTYLE_NON_EXCLUSIVE) ;

  //
  // Collections are a set of one or more patches. The first parameter
  // is a list of the patches to apply.
  //
  DECLARE_COLLECTION(                                                        \
      ("IrpConsumeStacks IrpDeferCompletion IrpEditSRBs"                     \
        " IrpWatchAlloc IrpSeedStack IrpDpcComplete"),                       \
      "Irps",                                                                \
      "Assert IRP usage is correct",                                         \
      NULL                                                                   \
      );

  //
  // An entry describes a patch to code. The first parameter has the
  // following syntax
  //   ([ed | ew | eb] module!symbolic [& | |]= hexvalue;)*
  //
  // The default size is assumed to be dword/ulong. Supported operators
  // are &=, =, and |=
  //
  // The second parameter is the shortcut name (e.g. "!Patch IrpTrack")
  // The third parameter is the menu description displayed when !Patch
  //   is typed with no parameters.
  // The fourth parameter is a terse description displayed when the
  //   user requests more information (e.g. "!Patch IrpTrack?")
  //

  DECLARE_ENTRY(                                                             \
    ("ed nt!IovpTrackingFlags = %x;ed nt!IovpEnforcementLevel = 0;",         \
     ASSERTFLAG_TRACKIRPS | ASSERTFLAG_POLICEIRPS |                          \
     ASSERTFLAG_MONITORMAJORS),                                              \
    "IrpTrack",                                                              \
    "Monitor IRPs for mistakes",                                             \
    "This enables a battery of basic assertions."                            \
    );

    BEGIN_SIBLING_LIST(LISTSTYLE_NON_EXCLUSIVE) ;

    DECLARE_ENTRY(                                                           \
      ("ed nt!IovpTrackingFlags |= %x;", ASSERTFLAG_ROTATE_STATUS),          \
      "IrpRotateStatus",                                                     \
      "Rotate success codes",                                                \
      "Successful IRPs have their status codes changed as they come up the"  \
      " stack to verify drivers return the correct codes and support success"\
      " codes other than STATUS_SUCCESS."                                    \
      );

    DECLARE_ENTRY(                                                           \
      ("ed nt!IovpTrackingFlags |= %x;", ASSERTFLAG_FORCEPENDING),           \
      "IrpPendAll",                                                          \
      "Force IRPs Pending",                                                  \
      "All IRPs (except IRP_MJ_PNP.IRP_MN_REMOVE) return with STATUS_PENDING"\
      " and are marked appropriately. This is used to verify drivers"        \
      " properly handle IRPs that return before being completed."            \
      );

      BEGIN_SIBLING_LIST(LISTSTYLE_NON_EXCLUSIVE) ;

      DECLARE_ENTRY(                                                         \
        ("ed nt!IovpTrackingFlags |= %x;", ASSERTFLAG_DEFERCOMPLETION),      \
        "IrpDeferCompletion",                                                \
        "IRP completion is stalled at each stack",                           \
        "IRP completion is deferred at every stack location using a timer"   \
        " based delay at nt!IopDiagIrpDeferralTime (default 300us per stack" \
        " location."                                                         \
        );

      END_SIBLING_LIST() ;

    DECLARE_ENTRY(                                                           \
      ("ed nt!IovpTrackingFlags |= %x;", ASSERTFLAG_CONSUME_ALWAYS),         \
      "IrpConsumeStacks",                                                    \
      "Consume stack locations (undo IoSkips)",                              \
      "IoSkipCurrentStackLocation calls are unwound (ie, the stack location" \
      " is still consumed) when an IRP is forwarded to another driver in the"\
      " same driver stack. This verifies drivers don't consume too many IRP" \
      " locations."                                                          \
      );

    DECLARE_ENTRY(                                                           \
      ("ed nt!IovpTrackingFlags |= %x;", ASSERTFLAG_SURROGATE),              \
      "IrpSurrogate",                                                        \
      "Writes to IRPs after completion fault",                               \
      "The original IRP sent to IoCallDriver is replaced with a surrogate"   \
      " IRP allocated from the special pool. When that IRP has been"         \
      " completed at the top, the original is completed and the special pool"\
      " surrogate is freed. Writes to the surrogate thus cause a fault."     \
      );

      BEGIN_SIBLING_LIST(LISTSTYLE_NON_EXCLUSIVE) ;

      DECLARE_ENTRY(                                                         \
        ("ed nt!IovpTrackingFlags |= %x;", ASSERTFLAG_SMASH_SRBS),           \
        "IrpEditSRBs",                                                       \
        "Patch SRBs so writes after completion fault.",                      \
        "SCSI SRBs contain a pointer to the original IRP that must be edited"\
        " if a surrogate IRP is to be used. Note that the method for"        \
        " identifying a SRB might erroneously edit an IRP as there is no"    \
        " signature on SRBs. If this option is off no"                       \
        " IRP_MJ_INTERNAL_DEVICE_CONTROL IRPs use surrogates."               \
        );

      END_SIBLING_LIST() ;

    DECLARE_ENTRY(                                                           \
      NULL,                                                                  \
      NO_SHORTCUT,                                                           \
      "Complete IRPs at specific IRQLs.",                                    \
      NULL                                                                   \
      );

      BEGIN_SIBLING_LIST(LISTSTYLE_EXCLUSIVE) ;

      DECLARE_ENTRY(                                                         \
        ("ed nt!IovpTrackingFlags |= %x;", ASSERTFLAG_COMPLETEATDPC),        \
        "IrpDpcComplete",                                                    \
        "Complete all IRPs at IRQL2 (DPC).",                                 \
        "The CPU's IRQL is raised to DPC whenever a completion is going to"  \
        " be called."                                                        \
        );

      DECLARE_ENTRY(                                                         \
        ("ed nt!IovpTrackingFlags |= %x;",                                   \
          ASSERTFLAG_COMPLETEATPASSIVE | ASSERTFLAG_FORCEPENDING),           \
        "IrpPassiveComplete",                                                \
        "Complete all IRPs at IRQL0 (Passive).",                             \
        "IRPs are completed within a passive level work item if legal."      \
        " All IRPs are pended (!patch IrpPendAll) if this action is enabled."\
        );

      END_SIBLING_LIST() ;

    END_SIBLING_LIST() ;

  DECLARE_ENTRY(                                                             \
    ("ed nt!IovpTrackingFlags |= %x;", ASSERTFLAG_SEEDSTACK),                \
    "IrpSeedStack",                                                          \
    "Catches uninitialized variables.",                                      \
    "Prior to calling the IRP dispatch routine, the stack is seeded with"    \
    " 0xFFFFFFFF's. If this value is returned or passed down, an assert"     \
    " occurs."                                                               \
    );

  DECLARE_ENTRY(                                                             \
    ("ed nt!IovpTrackingFlags |= %x;", ASSERTFLAG_MONITOR_ALLOCS),           \
    "IrpWatchAlloc",                                                         \
    "IRPs are allocated from the special pool.",                             \
    "This feature allows IRPs touched after being freed to cause a fault."   \
    " While allocated, information is kept on the allocator of the IRP."     \
    );

  DECLARE_ENTRY(                                                             \
    ("ed nt!IovpHackFlags = 0;"),                                            \
    "IrpNoHacks",                                                            \
    "Disable all hacks for IRP asserting.",                                  \
    "To allow the system to boot and shutdown during IRP assertion"          \
    " several hacks are step around current bugs in the OS. !Patch"          \
    " IrpNoHacks disables these hacks."                                      \
    );

  DECLARE_ENTRY(                                                             \
    ("ed nt!IovpTrackingFlags = 0;"),                                        \
    "IrpNoAsserts",                                                          \
    "Turn off IRP assertions.",                                              \
    "Turns off all assertions enabled by !Patch Irps."                       \
    );

  END_SIBLING_LIST() ;

END_CATEGORY() ;
#endif // NO_SPECIAL_IRP

//
// Every patch must belong to a category
//
BEGIN_CATEGORY("Plug-n-Play Options") ;

  //
  // Non-exclusive means the children can be turned on and off individually
  // (like check boxes). In exclusive lists, only one option can be on at
  // once (ie, radio buttons).
  //
  BEGIN_SIBLING_LIST(LISTSTYLE_NON_EXCLUSIVE) ;

  DECLARE_ENTRY(                                                             \
    ("ed nt!PiDebugLevel = ff"),                                             \
    "PnpQueue",                                                              \
    "Shows Pnp remove information.",                                         \
    "This can be used to see which devices fail various queries."            \
    );

  DECLARE_ENTRY(                                                             \
    ("ed Hal!HalDebug = 3;"),                                                \
    "HalPnpVerbose",                                                         \
    "Turn on Hal Pnp Verbose Output.",                                       \
    "Useful for debugging non-ACPI IRQ Routing problems."                    \
    );

  DECLARE_ENTRY(                                                             \
    ("ed nt!PnpResDebugLevel = 3"),                                          \
    "PnpRes",                                                                \
    "Show resource assignment info.",                                        \
    "Dumps out information during resource allocation (not including"        \
    "arbiters.)"                                                             \
    );

    BEGIN_SIBLING_LIST(LISTSTYLE_EXCLUSIVE) ;

    DECLARE_ENTRY(                                                           \
      ("ed nt!PnpResDebugLevel |= 4;"),                                      \
      "PnpResFull",                                                          \
      "Even more resource info.",                                            \
      "Dumps out detailed information during resource allocation."           \
      );

    END_SIBLING_LIST() ;

  END_SIBLING_LIST() ;

END_CATEGORY() ;

#endif
