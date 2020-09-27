#
# The contents of this file was moved from windows\makefile0
#

COPYRIGHT_STRING = Copyright (c) 1990-2001  Microsoft Corporation

!IFNDEF NO_STRICT
STRICT=1
!ENDIF

TARGETSRCS0= \
             $(SDK_INC_PATH)\dxfile.h        \
             $(SDK_INC_PATH)\rmxfguid.h      \
             $(SDK_INC_PATH)\rmxftmpl.h      \
             $(SDK_INC_PATH)\winbase.h       \
             $(SDK_INC_PATH)\windef.h        \
             $(SDK_INC_PATH)\windows.h       \
             $(SDK_INC_PATH)\wingdi.h        \
             $(SDK_INC_PATH)\winioctl.h      \
             $(SDK_INC_PATH)\winnls.h        \
             $(SDK_INC_PATH)\winnt.h         \
             $(SDK_INC_PATH)\winsockx.h      \
             $(SDK_INC_PATH)\winuser.h       \
             $(SDK_INC_PATH)\winver.h        \
             $(SDK_INC_PATH)\xbox.h          \
             $(SDK_INC_PATH)\xkbd.h          \
             $(SDK_INC_PATH)\xmv.h           \
             $(SDK_INC_PATH)\xtl.h

TARGETSRCS1= \
             $(PRIVATE_INC_PATH)\winbasep.h  \
             $(PRIVATE_INC_PATH)\windowsp.h  \
             $(PRIVATE_INC_PATH)\wingdip.h   \
             $(PRIVATE_INC_PATH)\winnlsp.h   \
             $(PRIVATE_INC_PATH)\winuserp.h  \
             $(PRIVATE_INC_PATH)\xbdm.h      \
             $(PRIVATE_INC_PATH)\xbdmp.h     \
             $(PRIVATE_INC_PATH)\xboxdbg.h   \
             $(PRIVATE_INC_PATH)\xboxp.h     \
             $(PRIVATE_INC_PATH)\winsockp.h  \
             $(PRIVATE_INC_PATH)\wsockntp.h

TARGETSRCS2= \
             $(XDK_INC_PATH)\dxfile.h        \
             $(XDK_INC_PATH)\rmxfguid.h      \
             $(XDK_INC_PATH)\rmxftmpl.h      \
             $(XDK_INC_PATH)\winbase.h       \
             $(XDK_INC_PATH)\windef.h        \
             $(XDK_INC_PATH)\winnt.h         \
             $(XDK_INC_PATH)\winsockx.h      \
             $(XDK_INC_PATH)\xbox.h          \
             $(XDK_INC_PATH)\xbdm.h          \
             $(XDK_INC_PATH)\xboxdbg.h       \
             $(XDK_INC_PATH)\xkbd.h          \
             $(XDK_INC_PATH)\xmv.h           \
             $(XDK_INC_PATH)\xtl.h

INTERMEDIATES0= \
             winbase.x                       \
             winbasep.x                      \
             windows.x                       \
             windowsp.x                      \
             wingdi.x                        \
             wingdip.x                       \
             winnls.x                        \
             winsockx.x                      \
             winuser.x                       \
             winuserp.x                      \
             xbdm.x                          \
             xbdmp.x                         \
             xboxdbg.x

WINNT_DEPENDS = $(SDK_INC_PATH)\ntdef.h \
                $(SDK_INC_PATH)\ntstatus.h \
                $(SDK_INC_PATH)\ntkeapi.h \
                $(SDK_INC_PATH)\nti386.h \
                $(SDK_INC_PATH)\ntxcapi.h \
                $(SDK_INC_PATH)\ntseapi.h \
                $(SDK_INC_PATH)\ntpsapi.h \
                $(SDK_INC_PATH)\ntexapi.h \
                $(SDK_INC_PATH)\ntmmapi.h \
                $(SDK_INC_PATH)\ntioapi.h \
                $(SDK_INC_PATH)\ntobapi.h \
                $(SDK_INC_PATH)\ntpoapi.h \
                $(SDK_INC_PATH)\ntimage.h \
                $(SDK_INC_PATH)\ntrtl.h  \
                $(SDK_INC_PATH)\nturtl.h  \
                $(SDK_INC_PATH)\ntldr.h  \
                $(SDK_INC_PATH)\ntdbg.h \
                $(SDK_INC_PATH)\ntregapi.h \
                $(SDK_INC_PATH)\ntconfig.h \

WINIOCTL_DEPENDS = $(SDK_INC_PATH)\devioctl.h \
                   $(SDK_INC_PATH)\ntddstor.h \
                   $(SDK_INC_PATH)\ntdddisk.h \
                   $(SDK_INC_PATH)\ntioapi.h


TARGET_MOVED0 = \
                $(SDK_INC_PATH)\accctrl.h       \
                $(SDK_INC_PATH)\dde.rh          \
                $(SDK_INC_PATH)\ddeml.h         \
                $(SDK_INC_PATH)\ddraw.h         \
                $(SDK_INC_PATH)\ddrawex.h       \
                $(SDK_INC_PATH)\digitalv.h      \
                $(SDK_INC_PATH)\dxtdbg.h        \
                $(SDK_INC_PATH)\ime.h           \
                $(SDK_INC_PATH)\imm.h           \
                $(SDK_INC_PATH)\indicml.h       \
                $(SDK_INC_PATH)\lzexpand.h      \
                $(SDK_INC_PATH)\nspapi.h        \
                $(SDK_INC_PATH)\pbt.h           \
                $(SDK_INC_PATH)\userenv.h       \
                $(SDK_INC_PATH)\wincon.h        \
                $(SDK_INC_PATH)\wincrypt.h      \
                $(SDK_INC_PATH)\winnls32.h      \
                $(SDK_INC_PATH)\winnt.rh        \
                $(SDK_INC_PATH)\winreg.h        \
                $(SDK_INC_PATH)\winspool.h      \
                $(SDK_INC_PATH)\winsvc.h

TARGET_MOVED1 = \
                $(OAK_INC_PATH)\dderror.h       \
                $(OAK_INC_PATH)\immdev.h        \
                $(OAK_INC_PATH)\winsecp.h       \
                $(PRIVATE_INC_PATH)\conapi.h    \
                $(PRIVATE_INC_PATH)\ddmc.h      \
                $(PRIVATE_INC_PATH)\ddmcp.h     \
                $(PRIVATE_INC_PATH)\ddemlp.h    \
                $(PRIVATE_INC_PATH)\ddrawp.h    \
                $(PRIVATE_INC_PATH)\dvpp.h      \
                $(PRIVATE_INC_PATH)\imep.h      \
                $(PRIVATE_INC_PATH)\immp.h      \
                $(PRIVATE_INC_PATH)\indicmlp.h  \
                $(PRIVATE_INC_PATH)\userenvp.h  \
                $(PRIVATE_INC_PATH)\profmapp.h  \
                $(PRIVATE_INC_PATH)\wbasek.h    \
                $(PRIVATE_INC_PATH)\winconp.h   \
                $(PRIVATE_INC_PATH)\winregp.h   \
                $(PRIVATE_INC_PATH)\winsprlp.h  \
                $(PRIVATE_INC_PATH)\xtlp.h

TARGET_MOVED2 = \
                accctrl.x                       \
                accctrlp.x                      \
                wincon.x                        \
                winconp.x                       \
                winnls32.x                      \
                winnls3p.x                      \
                winreg.x                        \
                winregp.x                       \
                ddeml.x                         \
                ime.x                           \
                imep.x                          \
                imm.x                           \
                immdev.x                        \
                immp.x                          \
                userenv.x                       \
                userenvp.x                      \
                profmap.x                       \
                profmapp.x                      \
                qos.w                           \
                qospol.w                        \
                qossp.w                         \
                wbasenoxbox.x                   \
                winnetp.x                       \
                winnetwk.x                      \
                winspool.x                      \
                xtl.x                           \
                xtlp.x

TARGETSRCS= $(TARGETSRCS0) $(TARGETSRCS1) $(TARGETSRCS2)

wininc: $(TARGETSRCS)

clean: cleansrc cleanmoved

# split up the deletes so we don't get nmake "line too long" errors
cleansrc:
    -del /q /f $(TARGETSRCS0) 2>nul
    -del /q /f $(TARGETSRCS1) 2>nul
    -del /q /f $(TARGETSRCS2) 2>nul
    -del /q /f $(INTERMEDIATES0) 2>nul
    -del /q /f $(INTERMEDIATES1) 2>nul

# delete the files that were used to be generated

cleanmoved:
    -del /q /f $(TARGET_MOVED0) 2>nul
    -del /q /f $(TARGET_MOVED1) 2>nul
    -del /q /f $(TARGET_MOVED2) 2>nul

#
# mmddk.h, mmddkp.h
#

$(SDK_INC_PATH)\mmddk.h: mmddk.h
    hsplit -lt2 Internal -e -o $@ nul mmddk.h

$(PRIVATE_INC_PATH)\mmddkp.h: mmddk.h
    copy mmddk.h $@


#
# mmsystem.h, mmsystem.x, mmsysp.h, mmsysp.x
#

mmsystem.x: mmsystem.w
    hsplit -e -s -o mmsystem.y nul -lt2 inc -bt2 begin_inc end_inc -ltb hinc -btb begin_hinc end_hinc mmsystem.w
    hsplit -e -n -o mmsystem.x mmsysp.x mmsystem.y
    del mmsystem.y

$(SDK_INC_PATH)\mmsystem.h: mmsystem.x
    wcshdr < mmsystem.x > $@

$(XDK_INC_PATH)\mmsystem.h : $(SDK_INC_PATH)\mmsystem.h
    copy $** $@

$(PRIVATE_INC_PATH)\mmsysp.h:  mmsysp.x
    wcshdr <  mmsysp.x > $@

#
# winsockx.h, winsockx.w
#

$(SDK_INC_PATH)\winsockx.h $(PRIVATE_INC_PATH)\winsockp.h : winsockx.w
    hsplit -ts noxbox -ts ntp -ta xboxp -o nul $(PRIVATE_INC_PATH)\winsockp.h winsockx.w
    hsplit -ts noxbox -ts xboxp -ta ntp -o $(SDK_INC_PATH)\winsockx.h $(PRIVATE_INC_PATH)\wsockntp.h winsockx.w

$(XDK_INC_PATH)\winsockx.h : $(SDK_INC_PATH)\winsockx.h
    copy $** $@

#
# winbase.h, winbasep.h, winbase.x, winbasep.x
#

winbase.x winbasep.x $(PRIVATE_INC_PATH)\wbasek.h : winbase.w
    hsplit -ts noxbox -ta xboxonly -o winbase.x winbasep.x winbase.w

$(SDK_INC_PATH)\winbase.h : winbase.x
    wcshdr < winbase.x > $@

$(XDK_INC_PATH)\winbase.h : $(SDK_INC_PATH)\winbase.h
    copy $** $@

$(PRIVATE_INC_PATH)\winbasep.h : winbasep.x
    wcshdr < winbasep.x > $@

#
# windef.h
#

$(SDK_INC_PATH)\windef.h: windef.w
    hsplit -e -o $@ nul windef.w

$(XDK_INC_PATH)\windef.h : $(SDK_INC_PATH)\windef.h
    copy $** $@

#
# windows.h, windowsp.h, windows.x, windowsp.x
#

windows.x windowsp.x: windows.w
    hsplit -e -o windows.x windowsp.x windows.w

$(SDK_INC_PATH)\windows.h : windows.x
    wcshdr < windows.x > $@

$(PRIVATE_INC_PATH)\windowsp.h : windowsp.x
    wcshdr < windowsp.x > $@

#
# xtl.h
#

$(SDK_INC_PATH)\xtl.h : xtl.w $(PRIVATE_INC_PATH)\xboxverp.h makefile.inc
    type << > $@
/*++ BUILD Version: ????     Increment this if a change has global effects

$(COPYRIGHT_STRING)

Module Name:

    xtl.h

Abstract:

    Master include file for Xbox applications.

--*/

#ifndef _XTL_
#define _XTL_

#ifndef WINVER
#define WINVER 0x0500
#endif

#ifndef _INC_XTL
#define _INC_XTL

<<
    hextract -o $@ -lt xtl $(PRIVATE_INC_PATH)\xboxverp.h
    hextract -o $@ -bt begin_xtl end_xtl xtl.w

$(XDK_INC_PATH)\xtl.h : $(SDK_INC_PATH)\xtl.h
    copy $** $@

#
# wingdi.h, wingdip.h, wingdip.x, wingdi.x
#

wingdi.x wingdip.x: wingdi.w
    hsplit -e -o wingdi.x wingdip.x wingdi.w

$(PRIVATE_INC_PATH)\wingdip.h: wingdip.x
    wcshdr < wingdip.x > $@

$(SDK_INC_PATH)\wingdi.h: wingdi.x
    wcshdr < wingdi.x > $@

#
# winnls.h, winnlsp.h, winnls.x
#

winnls.x $(PRIVATE_INC_PATH)\winnlsp.h: winnls.w
    hsplit -e -o winnls.x $(PRIVATE_INC_PATH)\winnlsp.h winnls.w

$(SDK_INC_PATH)\winnls.h: winnls.x
    wcshdr < winnls.x > $@

#
# winuser.h, winuserp.h, winuserp.x
#

winuser.x winuserp.x : winuser.w
    hsplit -e -o winuser.x winuserp.x -i winuser.w

$(SDK_INC_PATH)\winuser.h: winuser.x
    wcshdr < winuser.x > $@

$(PRIVATE_INC_PATH)\winuserp.h: winuserp.x
    wcshdr < winuserp.x > $@

#
# winver.h
#

$(SDK_INC_PATH)\winver.h : winver.x
    wcshdr < winver.x > $@


#
# dxfile.h
#

$(SDK_INC_PATH)\dxfile.h: dxfile.h
        copy dxfile.h $@

$(XDK_INC_PATH)\dxfile.h : $(SDK_INC_PATH)\dxfile.h
    copy $** $@

#
# xmv.h
#

$(SDK_INC_PATH)\xmv.h: xmv.h
        copy xmv.h $@

$(XDK_INC_PATH)\xmv.h : $(SDK_INC_PATH)\xmv.h
    copy $** $@

#
# rmxfguid.h
#

$(SDK_INC_PATH)\rmxfguid.h: rmxfguid.h
        copy rmxfguid.h $@

$(XDK_INC_PATH)\rmxfguid.h : $(SDK_INC_PATH)\rmxfguid.h
    copy $** $@

#
# rmxftmpl.h
#

$(SDK_INC_PATH)\rmxftmpl.h: rmxftmpl.h
        copy rmxftmpl.h $@

$(XDK_INC_PATH)\rmxftmpl.h : $(SDK_INC_PATH)\rmxftmpl.h
    copy $** $@


#
# xbox.h, xboxp.h
#

$(SDK_INC_PATH)\xbox.h $(PRIVATE_INC_PATH)\xboxp.h : xbox.x
    hsplit -o $(SDK_INC_PATH)\xbox.h $(PRIVATE_INC_PATH)\xboxp.h xbox.x

$(XDK_INC_PATH)\xbox.h : $(SDK_INC_PATH)\xbox.h
    copy $** $@


#
# xbdm.h, xboxdbg.h
#

xbdm.x xbdmp.x : xbdm.w
    hsplit -ta xbdm -ts xboxdbg -ts priv -o xbdm.x xbdmp.x -i xbdm.w

$(PRIVATE_INC_PATH)\xbdm.h : xbdm.x
    copy $** $@

$(PRIVATE_INC_PATH)\xbdmp.h : xbdmp.x
    copy $** $@

$(XDK_INC_PATH)\XbDm.h : $(PRIVATE_INC_PATH)\xbdm.h
    Copy "$?" "$@"

xboxdbg.x : xbdm.w
    hsplit -ts xbdm -ta xboxdbg -o xboxdbg.x xboxdbgp.x xbdm.w

$(PRIVATE_INC_PATH)\xboxdbg.h : xboxdbg.x
    wcshdr < xboxdbg.x > $@

$(XDK_INC_PATH)\XboxDbg.h : $(PRIVATE_INC_PATH)\XboxDbg.h
    Copy "$?" "$@"

#
# xkbd.h
#

$(SDK_INC_PATH)\xkbd.h : xkbd.x
    copy $** $@   

$(XDK_INC_PATH)\xkbd.h : xkbd.x
    copy $** $@   


########################################################################
# winnt.h
#

$(SDK_INC_PATH)\winnt.h: $(WINNT_DEPENDS) makefile.inc
    type << > $@
/*++ BUILD Version: ????     Increment this if a change has global effects

$(COPYRIGHT_STRING)

Module Name:

    winnt.h

Abstract:

    This module defines the 32-Bit Windows types and constants that are
    defined by NT, but exposed through the Win32 API.

Revision History:

--*/

#ifndef _WINNT_
#define _WINNT_

#ifdef __cplusplus
extern "C" {
#endif

<<
    hextract -f2 -o $@ -lt winnt -bt begin_winnt end_winnt $(WINNT_DEPENDS)
    type << >> $@

#ifdef __cplusplus
}
#endif

#endif /* _WINNT_ */

<<

$(XDK_INC_PATH)\winnt.h : $(SDK_INC_PATH)\winnt.h
    copy $** $@


########################################################################
# winioctl.h
#

$(SDK_INC_PATH)\winioctl.h: $(WINIOCTL_DEPENDS) makefile.inc
    type << > $@
/*++ BUILD Version: ????    // Increment this if a change has global effects

$(COPYRIGHT_STRING)

Module Name:

    winioctl.h

Abstract:

    This module defines the 32-Bit Windows Device I/O control codes.

Revision History:

--*/


//
// Interface GUIDs
//
// need these GUIDs outside conditional includes so that user can
//   #include <winioctl.h> in precompiled header
//   #include <initguid.h> in a single source file
//   #include <winioctl.h> in that source file a second time to instantiate the GUIDs
//
#ifdef DEFINE_GUID
//
// Make sure FAR is defined...
//
#ifndef FAR
#ifdef _WIN32
#define FAR
#else
#define FAR _far
#endif
#endif

<<
    hextract -f -o $@ -lt wioctlguids -bt begin_wioctlguids end_wioctlguids $(WINIOCTL_DEPENDS)
    type << >> $@
#endif // DEFINE_GUID

#ifndef _WINIOCTL_
#define _WINIOCTL_

<<
    hextract -f -o $@ -lt winioctl -bt begin_winioctl end_winioctl $(WINIOCTL_DEPENDS)
    type << >> $@

#endif // _WINIOCTL_

<<
