# Microsoft Visual C++ Generated NMAKE File, Format Version 2.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

!IF "$(CFG)" == ""
CFG=Win32 Debug
!MESSAGE No configuration specified.  Defaulting to Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "Win32 Release" && "$(CFG)" != "Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "spyxx2.mak" CFG="Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

################################################################################
# Begin Project
# PROP Target_Last_Scanned "Win32 Debug"
MTL=MkTypLib.exe
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "WinRel"
# PROP BASE Intermediate_Dir "WinRel"
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "WinRel"
# PROP Intermediate_Dir "WinRel"
OUTDIR=.\WinRel
INTDIR=.\WinRel

ALL : $(OUTDIR)/spyxx2.exe $(OUTDIR)/spyxx2.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE CPP /nologo /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FR /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /D "_AFXDLL" /D "_MBCS" /D "DISABLE_WIN95_MESSAGES" /FR /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_X86_" /D "_AFXDLL" /D "_MBCS" /D "DISABLE_WIN95_MESSAGES" /FR$(INTDIR)/\
 /Fp$(OUTDIR)/"spyxx2.pch" /Yu"stdafx.h" /Fo$(INTDIR)/ /c 
CPP_OBJS=.\WinRel/
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"spyxx.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"spyxx2.bsc" 
BSC32_SBRS= \
	$(INTDIR)/mstream.sbr \
	$(INTDIR)/animsgs.sbr \
	$(INTDIR)/wndptabs.sbr \
	$(INTDIR)/msglog.sbr \
	$(INTDIR)/lbmsgs.sbr \
	$(INTDIR)/kbdmsgs.sbr \
	$(INTDIR)/msgptabs.sbr \
	$(INTDIR)/msgdoc2.sbr \
	$(INTDIR)/mainfrm.sbr \
	$(INTDIR)/blmsgs.sbr \
	$(INTDIR)/stbmsgs.sbr \
	$(INTDIR)/thdtreed.sbr \
	$(INTDIR)/spytreev.sbr \
	$(INTDIR)/prcnode.sbr \
	$(INTDIR)/outfdlg.sbr \
	$(INTDIR)/propinsp.sbr \
	$(INTDIR)/tbmsgs.sbr \
	$(INTDIR)/msgfdlg.sbr \
	$(INTDIR)/trkmsgs.sbr \
	$(INTDIR)/lvmsgs.sbr \
	$(INTDIR)/spyxxpch.sbr \
	$(INTDIR)/winfdlg.sbr \
	$(INTDIR)/ncmsgs.sbr \
	$(INTDIR)/findtool.sbr \
	$(INTDIR)/mdimsgs.sbr \
	$(INTDIR)/winmsgs.sbr \
	$(INTDIR)/imemsgs.sbr \
	$(INTDIR)/tvmsgs.sbr \
	$(INTDIR)/spytreed.sbr \
	$(INTDIR)/spytreec.sbr \
	$(INTDIR)/procdb.sbr \
	$(INTDIR)/msghook.sbr \
	$(INTDIR)/filtrdlg.sbr \
	$(INTDIR)/imgwell.sbr \
	$(INTDIR)/helpdirs.sbr \
	$(INTDIR)/wndtreed.sbr \
	$(INTDIR)/prcptabs.sbr \
	$(INTDIR)/hotlinkc.sbr \
	$(INTDIR)/btnmsgs.sbr \
	$(INTDIR)/property.sbr \
	$(INTDIR)/srchdlgs.sbr \
	$(INTDIR)/editmsgs.sbr \
	$(INTDIR)/hdrmsgs.sbr \
	$(INTDIR)/mousmsgs.sbr \
	$(INTDIR)/wndnode.sbr \
	$(INTDIR)/statmsgs.sbr \
	$(INTDIR)/prfdb.sbr \
	$(INTDIR)/dlgmsgs.sbr \
	$(INTDIR)/spyxx.sbr \
	$(INTDIR)/proptab.sbr \
	$(INTDIR)/progmsgs.sbr \
	$(INTDIR)/prftxtdb.sbr \
	$(INTDIR)/msgdoc.sbr \
	$(INTDIR)/treectl.sbr \
	$(INTDIR)/cbmsgs.sbr \
	$(INTDIR)/findwdlg.sbr \
	$(INTDIR)/ttmsgs.sbr \
	$(INTDIR)/hkmsgs.sbr \
	$(INTDIR)/aboutdlg.sbr \
	$(INTDIR)/thdptabs.sbr \
	$(INTDIR)/fontdlg.sbr \
	$(INTDIR)/ddemsgs.sbr \
	$(INTDIR)/msgview.sbr \
	$(INTDIR)/thdnode.sbr \
	$(INTDIR)/help.sbr \
	$(INTDIR)/updnmsgs.sbr \
	$(INTDIR)/prctreed.sbr \
	$(INTDIR)/msgcrack.sbr \
	$(INTDIR)/tabmsgs.sbr \
	$(INTDIR)/spyxxmdi.sbr \
	$(INTDIR)/sbmsgs.sbr \
	$(INTDIR)/clipmsgs.sbr \
	$(INTDIR)/tabdlg.sbr

$(OUTDIR)/spyxx2.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows /machine:$(PROCESSOR_ARCHITECTURE)
# ADD LINK32 WinRel/spyhkxx30.lib /NOLOGO /SUBSYSTEM:windows /machine:$(PROCESSOR_ARCHITECTURE)
# SUBTRACT LINK32 /PDB:none
LINK32_FLAGS=WinRel/spyhkxx30.lib /NOLOGO /SUBSYSTEM:windows /INCREMENTAL:no\
 /PDB:$(OUTDIR)/"spyxx2.pdb" /machine:$(PROCESSOR_ARCHITECTURE) /OUT:$(OUTDIR)/"spyxx2.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/mstream.obj \
	$(INTDIR)/animsgs.obj \
	$(INTDIR)/wndptabs.obj \
	$(INTDIR)/msglog.obj \
	$(INTDIR)/lbmsgs.obj \
	$(INTDIR)/kbdmsgs.obj \
	$(INTDIR)/msgptabs.obj \
	$(INTDIR)/msgdoc2.obj \
	$(INTDIR)/mainfrm.obj \
	$(INTDIR)/blmsgs.obj \
	$(INTDIR)/stbmsgs.obj \
	$(INTDIR)/thdtreed.obj \
	$(INTDIR)/spytreev.obj \
	$(INTDIR)/prcnode.obj \
	$(INTDIR)/outfdlg.obj \
	$(INTDIR)/propinsp.obj \
	$(INTDIR)/tbmsgs.obj \
	$(INTDIR)/msgfdlg.obj \
	$(INTDIR)/trkmsgs.obj \
	$(INTDIR)/lvmsgs.obj \
	$(INTDIR)/spyxxpch.obj \
	$(INTDIR)/winfdlg.obj \
	$(INTDIR)/ncmsgs.obj \
	$(INTDIR)/findtool.obj \
	$(INTDIR)/mdimsgs.obj \
	$(INTDIR)/winmsgs.obj \
	$(INTDIR)/imemsgs.obj \
	$(INTDIR)/tvmsgs.obj \
	$(INTDIR)/spytreed.obj \
	$(INTDIR)/spytreec.obj \
	$(INTDIR)/procdb.obj \
	$(INTDIR)/msghook.obj \
	$(INTDIR)/filtrdlg.obj \
	$(INTDIR)/imgwell.obj \
	$(INTDIR)/helpdirs.obj \
	$(INTDIR)/wndtreed.obj \
	$(INTDIR)/prcptabs.obj \
	$(INTDIR)/hotlinkc.obj \
	$(INTDIR)/btnmsgs.obj \
	$(INTDIR)/property.obj \
	$(INTDIR)/srchdlgs.obj \
	$(INTDIR)/editmsgs.obj \
	$(INTDIR)/hdrmsgs.obj \
	$(INTDIR)/mousmsgs.obj \
	$(INTDIR)/wndnode.obj \
	$(INTDIR)/statmsgs.obj \
	$(INTDIR)/prfdb.obj \
	$(INTDIR)/dlgmsgs.obj \
	$(INTDIR)/spyxx.obj \
	$(INTDIR)/proptab.obj \
	$(INTDIR)/progmsgs.obj \
	$(INTDIR)/prftxtdb.obj \
	$(INTDIR)/msgdoc.obj \
	$(INTDIR)/treectl.obj \
	$(INTDIR)/cbmsgs.obj \
	$(INTDIR)/findwdlg.obj \
	$(INTDIR)/ttmsgs.obj \
	$(INTDIR)/hkmsgs.obj \
	$(INTDIR)/aboutdlg.obj \
	$(INTDIR)/thdptabs.obj \
	$(INTDIR)/fontdlg.obj \
	$(INTDIR)/ddemsgs.obj \
	$(INTDIR)/msgview.obj \
	$(INTDIR)/thdnode.obj \
	$(INTDIR)/help.obj \
	$(INTDIR)/updnmsgs.obj \
	$(INTDIR)/prctreed.obj \
	$(INTDIR)/msgcrack.obj \
	$(INTDIR)/tabmsgs.obj \
	$(INTDIR)/spyxxmdi.obj \
	$(INTDIR)/sbmsgs.obj \
	$(INTDIR)/clipmsgs.obj \
	$(INTDIR)/spyxx.res \
	$(INTDIR)/tabdlg.obj

$(OUTDIR)/spyxx2.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "WinDebug"
# PROP BASE Intermediate_Dir "WinDebug"
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "WinDebug"
# PROP Intermediate_Dir "WinDebug"
OUTDIR=.\WinDebug
INTDIR=.\WinDebug

ALL : $(OUTDIR)/spyxx.exe $(OUTDIR)/spyxx2.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE CPP /nologo /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /c
# ADD CPP /nologo /MD /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_X86_" /D "DISABLE_WIN95_MESSAGES" /FR /Yu"stdafx.h" /Bt /c
CPP_PROJ=/nologo /MD /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /D "_X86_" /D "DISABLE_WIN95_MESSAGES" /FR$(INTDIR)/\
 /Fp$(OUTDIR)/"spyxx2.pch" /Yu"stdafx.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"spyxx2.pdb"\
 /Bt  /c 
CPP_OBJS=.\WinDebug/
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"spyxx.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"spyxx2.bsc" 
BSC32_SBRS= \
	$(INTDIR)/mstream.sbr \
	$(INTDIR)/animsgs.sbr \
	$(INTDIR)/wndptabs.sbr \
	$(INTDIR)/msglog.sbr \
	$(INTDIR)/lbmsgs.sbr \
	$(INTDIR)/kbdmsgs.sbr \
	$(INTDIR)/msgptabs.sbr \
	$(INTDIR)/msgdoc2.sbr \
	$(INTDIR)/mainfrm.sbr \
	$(INTDIR)/blmsgs.sbr \
	$(INTDIR)/stbmsgs.sbr \
	$(INTDIR)/thdtreed.sbr \
	$(INTDIR)/spytreev.sbr \
	$(INTDIR)/prcnode.sbr \
	$(INTDIR)/outfdlg.sbr \
	$(INTDIR)/propinsp.sbr \
	$(INTDIR)/tbmsgs.sbr \
	$(INTDIR)/msgfdlg.sbr \
	$(INTDIR)/trkmsgs.sbr \
	$(INTDIR)/lvmsgs.sbr \
	$(INTDIR)/spyxxpch.sbr \
	$(INTDIR)/winfdlg.sbr \
	$(INTDIR)/ncmsgs.sbr \
	$(INTDIR)/findtool.sbr \
	$(INTDIR)/mdimsgs.sbr \
	$(INTDIR)/winmsgs.sbr \
	$(INTDIR)/imemsgs.sbr \
	$(INTDIR)/tvmsgs.sbr \
	$(INTDIR)/spytreed.sbr \
	$(INTDIR)/spytreec.sbr \
	$(INTDIR)/procdb.sbr \
	$(INTDIR)/msghook.sbr \
	$(INTDIR)/filtrdlg.sbr \
	$(INTDIR)/imgwell.sbr \
	$(INTDIR)/helpdirs.sbr \
	$(INTDIR)/wndtreed.sbr \
	$(INTDIR)/prcptabs.sbr \
	$(INTDIR)/hotlinkc.sbr \
	$(INTDIR)/btnmsgs.sbr \
	$(INTDIR)/property.sbr \
	$(INTDIR)/srchdlgs.sbr \
	$(INTDIR)/editmsgs.sbr \
	$(INTDIR)/hdrmsgs.sbr \
	$(INTDIR)/mousmsgs.sbr \
	$(INTDIR)/wndnode.sbr \
	$(INTDIR)/statmsgs.sbr \
	$(INTDIR)/prfdb.sbr \
	$(INTDIR)/dlgmsgs.sbr \
	$(INTDIR)/spyxx.sbr \
	$(INTDIR)/proptab.sbr \
	$(INTDIR)/progmsgs.sbr \
	$(INTDIR)/prftxtdb.sbr \
	$(INTDIR)/msgdoc.sbr \
	$(INTDIR)/treectl.sbr \
	$(INTDIR)/cbmsgs.sbr \
	$(INTDIR)/findwdlg.sbr \
	$(INTDIR)/ttmsgs.sbr \
	$(INTDIR)/hkmsgs.sbr \
	$(INTDIR)/aboutdlg.sbr \
	$(INTDIR)/thdptabs.sbr \
	$(INTDIR)/fontdlg.sbr \
	$(INTDIR)/ddemsgs.sbr \
	$(INTDIR)/msgview.sbr \
	$(INTDIR)/thdnode.sbr \
	$(INTDIR)/help.sbr \
	$(INTDIR)/updnmsgs.sbr \
	$(INTDIR)/prctreed.sbr \
	$(INTDIR)/msgcrack.sbr \
	$(INTDIR)/tabmsgs.sbr \
	$(INTDIR)/spyxxmdi.sbr \
	$(INTDIR)/sbmsgs.sbr \
	$(INTDIR)/clipmsgs.sbr \
	$(INTDIR)/tabdlg.sbr

$(OUTDIR)/spyxx2.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows /DEBUG /machine:$(PROCESSOR_ARCHITECTURE)
# ADD LINK32 WinDebug/spyxxhk30.lib /NOLOGO /SUBSYSTEM:windows /DEBUG /machine:$(PROCESSOR_ARCHITECTURE) /OUT:"WinDebug/spyxx.exe"
# SUBTRACT LINK32 /PDB:none
LINK32_FLAGS=WinDebug/spyxxhk30.lib /NOLOGO /SUBSYSTEM:windows /INCREMENTAL:yes\
 /PDB:$(OUTDIR)/"spyxx2.pdb" /DEBUG /machine:$(PROCESSOR_ARCHITECTURE) /OUT:"WinDebug/spyxx.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/mstream.obj \
	$(INTDIR)/animsgs.obj \
	$(INTDIR)/wndptabs.obj \
	$(INTDIR)/msglog.obj \
	$(INTDIR)/lbmsgs.obj \
	$(INTDIR)/kbdmsgs.obj \
	$(INTDIR)/msgptabs.obj \
	$(INTDIR)/msgdoc2.obj \
	$(INTDIR)/mainfrm.obj \
	$(INTDIR)/blmsgs.obj \
	$(INTDIR)/stbmsgs.obj \
	$(INTDIR)/thdtreed.obj \
	$(INTDIR)/spytreev.obj \
	$(INTDIR)/prcnode.obj \
	$(INTDIR)/outfdlg.obj \
	$(INTDIR)/propinsp.obj \
	$(INTDIR)/tbmsgs.obj \
	$(INTDIR)/msgfdlg.obj \
	$(INTDIR)/trkmsgs.obj \
	$(INTDIR)/lvmsgs.obj \
	$(INTDIR)/spyxxpch.obj \
	$(INTDIR)/winfdlg.obj \
	$(INTDIR)/ncmsgs.obj \
	$(INTDIR)/findtool.obj \
	$(INTDIR)/mdimsgs.obj \
	$(INTDIR)/winmsgs.obj \
	$(INTDIR)/imemsgs.obj \
	$(INTDIR)/tvmsgs.obj \
	$(INTDIR)/spytreed.obj \
	$(INTDIR)/spytreec.obj \
	$(INTDIR)/procdb.obj \
	$(INTDIR)/msghook.obj \
	$(INTDIR)/filtrdlg.obj \
	$(INTDIR)/imgwell.obj \
	$(INTDIR)/helpdirs.obj \
	$(INTDIR)/wndtreed.obj \
	$(INTDIR)/prcptabs.obj \
	$(INTDIR)/hotlinkc.obj \
	$(INTDIR)/btnmsgs.obj \
	$(INTDIR)/property.obj \
	$(INTDIR)/srchdlgs.obj \
	$(INTDIR)/editmsgs.obj \
	$(INTDIR)/hdrmsgs.obj \
	$(INTDIR)/mousmsgs.obj \
	$(INTDIR)/wndnode.obj \
	$(INTDIR)/statmsgs.obj \
	$(INTDIR)/prfdb.obj \
	$(INTDIR)/dlgmsgs.obj \
	$(INTDIR)/spyxx.obj \
	$(INTDIR)/proptab.obj \
	$(INTDIR)/progmsgs.obj \
	$(INTDIR)/prftxtdb.obj \
	$(INTDIR)/msgdoc.obj \
	$(INTDIR)/treectl.obj \
	$(INTDIR)/cbmsgs.obj \
	$(INTDIR)/findwdlg.obj \
	$(INTDIR)/ttmsgs.obj \
	$(INTDIR)/hkmsgs.obj \
	$(INTDIR)/aboutdlg.obj \
	$(INTDIR)/thdptabs.obj \
	$(INTDIR)/fontdlg.obj \
	$(INTDIR)/ddemsgs.obj \
	$(INTDIR)/msgview.obj \
	$(INTDIR)/thdnode.obj \
	$(INTDIR)/help.obj \
	$(INTDIR)/updnmsgs.obj \
	$(INTDIR)/prctreed.obj \
	$(INTDIR)/msgcrack.obj \
	$(INTDIR)/tabmsgs.obj \
	$(INTDIR)/spyxxmdi.obj \
	$(INTDIR)/sbmsgs.obj \
	$(INTDIR)/clipmsgs.obj \
	$(INTDIR)/spyxx.res \
	$(INTDIR)/tabdlg.obj

$(OUTDIR)/spyxx.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

################################################################################
# Begin Group "Source Files"

################################################################################
# Begin Source File

SOURCE=.\mstream.cpp
DEP_MSTRE=\
	.\stdafx.h\
	.\msglog.h\
	.\msgview.h\
	.\tlhelp32.h\
	.\resource.h\
	.\spyxxhk.h\
	.\ime32.h\
	.\mstream.h\
	.\msgcrack.h\
	.\tabdlg.h\
	.\prfdb.h\
	.\procdb.h\
	.\property.h\
	.\propinsp.h\
	.\spyxx.h

$(INTDIR)/mstream.obj :  $(SOURCE)  $(DEP_MSTRE) $(INTDIR)\
 $(INTDIR)/spyxxpch.obj

# End Source File
################################################################################
# Begin Source File

SOURCE=.\animsgs.cpp
DEP_ANIMS=\
	.\stdafx.h\
	.\tlhelp32.h\
	.\resource.h\
	.\spyxxhk.h\
	.\ime32.h\
	.\mstream.h\
	.\msgcrack.h\
	.\tabdlg.h\
	.\prfdb.h\
	.\procdb.h\
	.\property.h\
	.\propinsp.h\
	.\spyxx.h

$(INTDIR)/animsgs.obj :  $(SOURCE)  $(DEP_ANIMS) $(INTDIR)\
 $(INTDIR)/spyxxpch.obj

# End Source File
################################################################################
# Begin Source File

SOURCE=.\wndptabs.cpp
DEP_WNDPT=\
	.\stdafx.h\
	.\hotlinkc.h\
	.\proptab.h\
	.\wndptabs.h\
	.\tlhelp32.h\
	.\resource.h\
	.\spyxxhk.h\
	.\ime32.h\
	.\mstream.h\
	.\msgcrack.h\
	.\tabdlg.h\
	.\prfdb.h\
	.\procdb.h\
	.\property.h\
	.\propinsp.h\
	.\spyxx.h

$(INTDIR)/wndptabs.obj :  $(SOURCE)  $(DEP_WNDPT) $(INTDIR)\
 $(INTDIR)/spyxxpch.obj

# End Source File
################################################################################
# Begin Source File

SOURCE=.\msglog.cpp
DEP_MSGLO=\
	.\stdafx.h\
	.\msglog.h\
	.\msgview.h\
	.\msgdoc.h\
	.\tlhelp32.h\
	.\resource.h\
	.\spyxxhk.h\
	.\ime32.h\
	.\mstream.h\
	.\msgcrack.h\
	.\tabdlg.h\
	.\prfdb.h\
	.\procdb.h\
	.\property.h\
	.\propinsp.h\
	.\spyxx.h

$(INTDIR)/msglog.obj :  $(SOURCE)  $(DEP_MSGLO) $(INTDIR)\
 $(INTDIR)/spyxxpch.obj

# End Source File
################################################################################
# Begin Source File

SOURCE=.\lbmsgs.cpp
DEP_LBMSG=\
	.\stdafx.h\
	.\tlhelp32.h\
	.\resource.h\
	.\spyxxhk.h\
	.\ime32.h\
	.\mstream.h\
	.\msgcrack.h\
	.\tabdlg.h\
	.\prfdb.h\
	.\procdb.h\
	.\property.h\
	.\propinsp.h\
	.\spyxx.h

$(INTDIR)/lbmsgs.obj :  $(SOURCE)  $(DEP_LBMSG) $(INTDIR)\
 $(INTDIR)/spyxxpch.obj

# End Source File
################################################################################
# Begin Source File

SOURCE=.\kbdmsgs.cpp
DEP_KBDMS=\
	.\stdafx.h\
	.\tlhelp32.h\
	.\resource.h\
	.\spyxxhk.h\
	.\ime32.h\
	.\mstream.h\
	.\msgcrack.h\
	.\tabdlg.h\
	.\prfdb.h\
	.\procdb.h\
	.\property.h\
	.\propinsp.h\
	.\spyxx.h

$(INTDIR)/kbdmsgs.obj :  $(SOURCE)  $(DEP_KBDMS) $(INTDIR)\
 $(INTDIR)/spyxxpch.obj

# End Source File
################################################################################
# Begin Source File

SOURCE=.\msgptabs.cpp
DEP_MSGPT=\
	.\stdafx.h\
	.\msglog.h\
	.\msgview.h\
	.\msgdoc.h\
	.\hotlinkc.h\
	.\proptab.h\
	.\msgptabs.h\
	.\tlhelp32.h\
	.\resource.h\
	.\spyxxhk.h\
	.\ime32.h\
	.\mstream.h\
	.\msgcrack.h\
	.\tabdlg.h\
	.\prfdb.h\
	.\procdb.h\
	.\property.h\
	.\propinsp.h\
	.\spyxx.h

$(INTDIR)/msgptabs.obj :  $(SOURCE)  $(DEP_MSGPT) $(INTDIR)\
 $(INTDIR)/spyxxpch.obj

# End Source File
################################################################################
# Begin Source File

SOURCE=.\msgdoc2.cpp
DEP_MSGDO=\
	.\stdafx.h\
	.\msglog.h\
	.\msgview.h\
	.\msgdoc.h\
	.\tlhelp32.h\
	.\resource.h\
	.\spyxxhk.h\
	.\ime32.h\
	.\mstream.h\
	.\msgcrack.h\
	.\tabdlg.h\
	.\prfdb.h\
	.\procdb.h\
	.\property.h\
	.\propinsp.h\
	.\spyxx.h

$(INTDIR)/msgdoc2.obj :  $(SOURCE)  $(DEP_MSGDO) $(INTDIR)\
 $(INTDIR)/spyxxpch.obj

# End Source File
################################################################################
# Begin Source File

SOURCE=.\mainfrm.cpp
DEP_MAINF=\
	.\stdafx.h\
	.\mainfrm.h\
	.\tlhelp32.h\
	.\resource.h\
	.\spyxxhk.h\
	.\ime32.h\
	.\mstream.h\
	.\msgcrack.h\
	.\tabdlg.h\
	.\prfdb.h\
	.\procdb.h\
	.\property.h\
	.\propinsp.h\
	.\spyxx.h

$(INTDIR)/mainfrm.obj :  $(SOURCE)  $(DEP_MAINF) $(INTDIR)\
 $(INTDIR)/spyxxpch.obj

# End Source File
################################################################################
# Begin Source File

SOURCE=.\blmsgs.cpp
DEP_BLMSG=\
	.\stdafx.h\
	.\tlhelp32.h\
	.\resource.h\
	.\spyxxhk.h\
	.\ime32.h\
	.\mstream.h\
	.\msgcrack.h\
	.\tabdlg.h\
	.\prfdb.h\
	.\procdb.h\
	.\property.h\
	.\propinsp.h\
	.\spyxx.h

$(INTDIR)/blmsgs.obj :  $(SOURCE)  $(DEP_BLMSG) $(INTDIR)\
 $(INTDIR)/spyxxpch.obj

# End Source File
################################################################################
# Begin Source File

SOURCE=.\stbmsgs.cpp
DEP_STBMS=\
	.\stdafx.h\
	.\tlhelp32.h\
	.\resource.h\
	.\spyxxhk.h\
	.\ime32.h\
	.\mstream.h\
	.\msgcrack.h\
	.\tabdlg.h\
	.\prfdb.h\
	.\procdb.h\
	.\property.h\
	.\propinsp.h\
	.\spyxx.h

$(INTDIR)/stbmsgs.obj :  $(SOURCE)  $(DEP_STBMS) $(INTDIR)\
 $(INTDIR)/spyxxpch.obj

# End Source File
################################################################################
# Begin Source File

SOURCE=.\thdtreed.cpp
DEP_THDTR=\
	.\stdafx.h\
	.\spytreed.h\
	.\thdtreed.h\
	.\treectl.h\
	.\spytreec.h\
	.\thdnode.h\
	.\findtool.h\
	.\spytreev.h\
	.\srchdlgs.h\
	.\tlhelp32.h\
	.\resource.h\
	.\spyxxhk.h\
	.\ime32.h\
	.\mstream.h\
	.\msgcrack.h\
	.\tabdlg.h\
	.\prfdb.h\
	.\procdb.h\
	.\property.h\
	.\propinsp.h\
	.\spyxx.h\
	.\imgwell.h

$(INTDIR)/thdtreed.obj :  $(SOURCE)  $(DEP_THDTR) $(INTDIR)\
 $(INTDIR)/spyxxpch.obj

# End Source File
################################################################################
# Begin Source File

SOURCE=.\spytreev.cpp
DEP_SPYTR=\
	.\stdafx.h\
	.\treectl.h\
	.\spytreec.h\
	.\spytreev.h\
	.\spytreed.h\
	.\fontdlg.h\
	.\tlhelp32.h\
	.\resource.h\
	.\spyxxhk.h\
	.\ime32.h\
	.\mstream.h\
	.\msgcrack.h\
	.\tabdlg.h\
	.\prfdb.h\
	.\procdb.h\
	.\property.h\
	.\propinsp.h\
	.\spyxx.h\
	.\imgwell.h

$(INTDIR)/spytreev.obj :  $(SOURCE)  $(DEP_SPYTR) $(INTDIR)\
 $(INTDIR)/spyxxpch.obj

# End Source File
################################################################################
# Begin Source File

SOURCE=.\prcnode.cpp
DEP_PRCNO=\
	.\stdafx.h\
	.\treectl.h\
	.\spytreec.h\
	.\prcnode.h\
	.\thdnode.h\
	.\tlhelp32.h\
	.\resource.h\
	.\spyxxhk.h\
	.\ime32.h\
	.\mstream.h\
	.\msgcrack.h\
	.\tabdlg.h\
	.\prfdb.h\
	.\procdb.h\
	.\property.h\
	.\propinsp.h\
	.\spyxx.h\
	.\imgwell.h

$(INTDIR)/prcnode.obj :  $(SOURCE)  $(DEP_PRCNO) $(INTDIR)\
 $(INTDIR)/spyxxpch.obj

# End Source File
################################################################################
# Begin Source File

SOURCE=.\outfdlg.cpp
DEP_OUTFD=\
	.\stdafx.h\
	.\msglog.h\
	.\msgview.h\
	.\msgdoc.h\
	.\findtool.h\
	.\filtrdlg.h\
	.\tlhelp32.h\
	.\resource.h\
	.\spyxxhk.h\
	.\ime32.h\
	.\mstream.h\
	.\msgcrack.h\
	.\tabdlg.h\
	.\prfdb.h\
	.\procdb.h\
	.\property.h\
	.\propinsp.h\
	.\spyxx.h

$(INTDIR)/outfdlg.obj :  $(SOURCE)  $(DEP_OUTFD) $(INTDIR)\
 $(INTDIR)/spyxxpch.obj

# End Source File
################################################################################
# Begin Source File

SOURCE=.\propinsp.cpp
DEP_PROPI=\
	.\stdafx.h\
	.\proptab.h\
	.\hotlinkc.h\
	.\wndptabs.h\
	.\prcptabs.h\
	.\thdptabs.h\
	.\msgptabs.h\
	.\msglog.h\
	.\msgview.h\
	.\msgdoc.h\
	.\tlhelp32.h\
	.\resource.h\
	.\spyxxhk.h\
	.\ime32.h\
	.\mstream.h\
	.\msgcrack.h\
	.\tabdlg.h\
	.\prfdb.h\
	.\procdb.h\
	.\property.h\
	.\propinsp.h\
	.\spyxx.h

$(INTDIR)/propinsp.obj :  $(SOURCE)  $(DEP_PROPI) $(INTDIR)\
 $(INTDIR)/spyxxpch.obj

# End Source File
################################################################################
# Begin Source File

SOURCE=.\tbmsgs.cpp
DEP_TBMSG=\
	.\stdafx.h\
	.\tlhelp32.h\
	.\resource.h\
	.\spyxxhk.h\
	.\ime32.h\
	.\mstream.h\
	.\msgcrack.h\
	.\tabdlg.h\
	.\prfdb.h\
	.\procdb.h\
	.\property.h\
	.\propinsp.h\
	.\spyxx.h

$(INTDIR)/tbmsgs.obj :  $(SOURCE)  $(DEP_TBMSG) $(INTDIR)\
 $(INTDIR)/spyxxpch.obj

# End Source File
################################################################################
# Begin Source File

SOURCE=.\msgfdlg.cpp
DEP_MSGFD=\
	.\stdafx.h\
	.\msglog.h\
	.\msgview.h\
	.\msgdoc.h\
	.\findtool.h\
	.\filtrdlg.h\
	.\tlhelp32.h\
	.\resource.h\
	.\spyxxhk.h\
	.\ime32.h\
	.\mstream.h\
	.\msgcrack.h\
	.\tabdlg.h\
	.\prfdb.h\
	.\procdb.h\
	.\property.h\
	.\propinsp.h\
	.\spyxx.h

$(INTDIR)/msgfdlg.obj :  $(SOURCE)  $(DEP_MSGFD) $(INTDIR)\
 $(INTDIR)/spyxxpch.obj

# End Source File
################################################################################
# Begin Source File

SOURCE=.\trkmsgs.cpp
DEP_TRKMS=\
	.\stdafx.h\
	.\tlhelp32.h\
	.\resource.h\
	.\spyxxhk.h\
	.\ime32.h\
	.\mstream.h\
	.\msgcrack.h\
	.\tabdlg.h\
	.\prfdb.h\
	.\procdb.h\
	.\property.h\
	.\propinsp.h\
	.\spyxx.h

$(INTDIR)/trkmsgs.obj :  $(SOURCE)  $(DEP_TRKMS) $(INTDIR)\
 $(INTDIR)/spyxxpch.obj

# End Source File
################################################################################
# Begin Source File

SOURCE=.\lvmsgs.cpp
DEP_LVMSG=\
	.\stdafx.h\
	.\tlhelp32.h\
	.\resource.h\
	.\spyxxhk.h\
	.\ime32.h\
	.\mstream.h\
	.\msgcrack.h\
	.\tabdlg.h\
	.\prfdb.h\
	.\procdb.h\
	.\property.h\
	.\propinsp.h\
	.\spyxx.h

$(INTDIR)/lvmsgs.obj :  $(SOURCE)  $(DEP_LVMSG) $(INTDIR)\
 $(INTDIR)/spyxxpch.obj

# End Source File
################################################################################
# Begin Source File

SOURCE=.\spyxxpch.cpp
DEP_SPYXX=\
	.\stdafx.h\
	.\tlhelp32.h\
	.\resource.h\
	.\spyxxhk.h\
	.\ime32.h\
	.\mstream.h\
	.\msgcrack.h\
	.\tabdlg.h\
	.\prfdb.h\
	.\procdb.h\
	.\property.h\
	.\propinsp.h\
	.\spyxx.h

!IF  "$(CFG)" == "Win32 Release"

# ADD CPP /Yc"stdafx.h"

$(INTDIR)/spyxxpch.obj :  $(SOURCE)  $(DEP_SPYXX) $(INTDIR)
   $(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_X86_" /D "_AFXDLL" /D "_MBCS" /D "DISABLE_WIN95_MESSAGES" /FR$(INTDIR)/\
 /Fp$(OUTDIR)/"spyxx2.pch" /Yc"stdafx.h" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

# ADD CPP /Yc"stdafx.h"

$(INTDIR)/spyxxpch.obj :  $(SOURCE)  $(DEP_SPYXX) $(INTDIR)
   $(CPP) /nologo /MD /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /D "_X86_" /D "DISABLE_WIN95_MESSAGES" /FR$(INTDIR)/\
 /Fp$(OUTDIR)/"spyxx2.pch" /Yc"stdafx.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"spyxx2.pdb"\
 /Bt  /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\winfdlg.cpp
DEP_WINFD=\
	.\stdafx.h\
	.\msglog.h\
	.\msgview.h\
	.\msgdoc.h\
	.\findtool.h\
	.\filtrdlg.h\
	.\tlhelp32.h\
	.\resource.h\
	.\spyxxhk.h\
	.\ime32.h\
	.\mstream.h\
	.\msgcrack.h\
	.\tabdlg.h\
	.\prfdb.h\
	.\procdb.h\
	.\property.h\
	.\propinsp.h\
	.\spyxx.h

$(INTDIR)/winfdlg.obj :  $(SOURCE)  $(DEP_WINFD) $(INTDIR)\
 $(INTDIR)/spyxxpch.obj

# End Source File
################################################################################
# Begin Source File

SOURCE=.\ncmsgs.cpp
DEP_NCMSG=\
	.\stdafx.h\
	.\tlhelp32.h\
	.\resource.h\
	.\spyxxhk.h\
	.\ime32.h\
	.\mstream.h\
	.\msgcrack.h\
	.\tabdlg.h\
	.\prfdb.h\
	.\procdb.h\
	.\property.h\
	.\propinsp.h\
	.\spyxx.h

$(INTDIR)/ncmsgs.obj :  $(SOURCE)  $(DEP_NCMSG) $(INTDIR)\
 $(INTDIR)/spyxxpch.obj

# End Source File
################################################################################
# Begin Source File

SOURCE=.\findtool.cpp
DEP_FINDT=\
	.\stdafx.h\
	.\findtool.h\
	.\msglog.h\
	.\msgview.h\
	.\msgdoc.h\
	.\filtrdlg.h\
	.\findwdlg.h\
	.\srchdlgs.h\
	.\tlhelp32.h\
	.\resource.h\
	.\spyxxhk.h\
	.\ime32.h\
	.\mstream.h\
	.\msgcrack.h\
	.\tabdlg.h\
	.\prfdb.h\
	.\procdb.h\
	.\property.h\
	.\propinsp.h\
	.\spyxx.h

$(INTDIR)/findtool.obj :  $(SOURCE)  $(DEP_FINDT) $(INTDIR)\
 $(INTDIR)/spyxxpch.obj

# End Source File
################################################################################
# Begin Source File

SOURCE=.\mdimsgs.cpp
DEP_MDIMS=\
	.\stdafx.h\
	.\tlhelp32.h\
	.\resource.h\
	.\spyxxhk.h\
	.\ime32.h\
	.\mstream.h\
	.\msgcrack.h\
	.\tabdlg.h\
	.\prfdb.h\
	.\procdb.h\
	.\property.h\
	.\propinsp.h\
	.\spyxx.h

$(INTDIR)/mdimsgs.obj :  $(SOURCE)  $(DEP_MDIMS) $(INTDIR)\
 $(INTDIR)/spyxxpch.obj

# End Source File
################################################################################
# Begin Source File

SOURCE=.\winmsgs.cpp
DEP_WINMS=\
	.\stdafx.h\
	.\tlhelp32.h\
	.\resource.h\
	.\spyxxhk.h\
	.\ime32.h\
	.\mstream.h\
	.\msgcrack.h\
	.\tabdlg.h\
	.\prfdb.h\
	.\procdb.h\
	.\property.h\
	.\propinsp.h\
	.\spyxx.h

$(INTDIR)/winmsgs.obj :  $(SOURCE)  $(DEP_WINMS) $(INTDIR)\
 $(INTDIR)/spyxxpch.obj

# End Source File
################################################################################
# Begin Source File

SOURCE=.\imemsgs.cpp
DEP_IMEMS=\
	.\stdafx.h\
	.\tlhelp32.h\
	.\resource.h\
	.\spyxxhk.h\
	.\ime32.h\
	.\mstream.h\
	.\msgcrack.h\
	.\tabdlg.h\
	.\prfdb.h\
	.\procdb.h\
	.\property.h\
	.\propinsp.h\
	.\spyxx.h

$(INTDIR)/imemsgs.obj :  $(SOURCE)  $(DEP_IMEMS) $(INTDIR)\
 $(INTDIR)/spyxxpch.obj

# End Source File
################################################################################
# Begin Source File

SOURCE=.\tvmsgs.cpp
DEP_TVMSG=\
	.\stdafx.h\
	.\tlhelp32.h\
	.\resource.h\
	.\spyxxhk.h\
	.\ime32.h\
	.\mstream.h\
	.\msgcrack.h\
	.\tabdlg.h\
	.\prfdb.h\
	.\procdb.h\
	.\property.h\
	.\propinsp.h\
	.\spyxx.h

$(INTDIR)/tvmsgs.obj :  $(SOURCE)  $(DEP_TVMSG) $(INTDIR)\
 $(INTDIR)/spyxxpch.obj

# End Source File
################################################################################
# Begin Source File

SOURCE=.\spytreed.cpp
DEP_SPYTRE=\
	.\stdafx.h\
	.\treectl.h\
	.\spytreed.h\
	.\spytreec.h\
	.\imgwell.h\
	.\spytreev.h\
	.\tlhelp32.h\
	.\resource.h\
	.\spyxxhk.h\
	.\ime32.h\
	.\mstream.h\
	.\msgcrack.h\
	.\tabdlg.h\
	.\prfdb.h\
	.\procdb.h\
	.\property.h\
	.\propinsp.h\
	.\spyxx.h

$(INTDIR)/spytreed.obj :  $(SOURCE)  $(DEP_SPYTRE) $(INTDIR)\
 $(INTDIR)/spyxxpch.obj

# End Source File
################################################################################
# Begin Source File

SOURCE=.\spytreec.cpp
DEP_SPYTREE=\
	.\stdafx.h\
	.\treectl.h\
	.\imgwell.h\
	.\spytreec.h\
	.\tlhelp32.h\
	.\resource.h\
	.\spyxxhk.h\
	.\ime32.h\
	.\mstream.h\
	.\msgcrack.h\
	.\tabdlg.h\
	.\prfdb.h\
	.\procdb.h\
	.\property.h\
	.\propinsp.h\
	.\spyxx.h

$(INTDIR)/spytreec.obj :  $(SOURCE)  $(DEP_SPYTREE) $(INTDIR)\
 $(INTDIR)/spyxxpch.obj

# End Source File
################################################################################
# Begin Source File

SOURCE=.\procdb.cpp
DEP_PROCD=\
	.\stdafx.h\
	.\prftilib.h\
	.\tlhelp32.h\
	.\resource.h\
	.\spyxxhk.h\
	.\ime32.h\
	.\mstream.h\
	.\msgcrack.h\
	.\tabdlg.h\
	.\prfdb.h\
	.\procdb.h\
	.\property.h\
	.\propinsp.h\
	.\spyxx.h

$(INTDIR)/procdb.obj :  $(SOURCE)  $(DEP_PROCD) $(INTDIR)\
 $(INTDIR)/spyxxpch.obj

# End Source File
################################################################################
# Begin Source File

SOURCE=.\msghook.cpp
DEP_MSGHO=\
	.\stdafx.h\
	.\msglog.h\
	.\msgview.h\
	.\msgdoc.h\
	.\tlhelp32.h\
	.\resource.h\
	.\spyxxhk.h\
	.\ime32.h\
	.\mstream.h\
	.\msgcrack.h\
	.\tabdlg.h\
	.\prfdb.h\
	.\procdb.h\
	.\property.h\
	.\propinsp.h\
	.\spyxx.h

$(INTDIR)/msghook.obj :  $(SOURCE)  $(DEP_MSGHO) $(INTDIR)\
 $(INTDIR)/spyxxpch.obj

# End Source File
################################################################################
# Begin Source File

SOURCE=.\filtrdlg.cpp
DEP_FILTR=\
	.\stdafx.h\
	.\msglog.h\
	.\msgview.h\
	.\msgdoc.h\
	.\findtool.h\
	.\filtrdlg.h\
	.\tlhelp32.h\
	.\resource.h\
	.\spyxxhk.h\
	.\ime32.h\
	.\mstream.h\
	.\msgcrack.h\
	.\tabdlg.h\
	.\prfdb.h\
	.\procdb.h\
	.\property.h\
	.\propinsp.h\
	.\spyxx.h

$(INTDIR)/filtrdlg.obj :  $(SOURCE)  $(DEP_FILTR) $(INTDIR)\
 $(INTDIR)/spyxxpch.obj

# End Source File
################################################################################
# Begin Source File

SOURCE=.\imgwell.cpp
DEP_IMGWE=\
	.\stdafx.h\
	.\imgwell.h\
	.\tlhelp32.h\
	.\resource.h\
	.\spyxxhk.h\
	.\ime32.h\
	.\mstream.h\
	.\msgcrack.h\
	.\tabdlg.h\
	.\prfdb.h\
	.\procdb.h\
	.\property.h\
	.\propinsp.h\
	.\spyxx.h

$(INTDIR)/imgwell.obj :  $(SOURCE)  $(DEP_IMGWE) $(INTDIR)\
 $(INTDIR)/spyxxpch.obj

# End Source File
################################################################################
# Begin Source File

SOURCE=.\helpdirs.cpp
DEP_HELPD=\
	.\stdafx.h\
	.\helpdirs.h\
	.\tlhelp32.h\
	.\resource.h\
	.\spyxxhk.h\
	.\ime32.h\
	.\mstream.h\
	.\msgcrack.h\
	.\tabdlg.h\
	.\prfdb.h\
	.\procdb.h\
	.\property.h\
	.\propinsp.h\
	.\spyxx.h

$(INTDIR)/helpdirs.obj :  $(SOURCE)  $(DEP_HELPD) $(INTDIR)\
 $(INTDIR)/spyxxpch.obj

# End Source File
################################################################################
# Begin Source File

SOURCE=.\wndtreed.cpp
DEP_WNDTR=\
	.\stdafx.h\
	.\treectl.h\
	.\imgwell.h\
	.\findtool.h\
	.\spytreed.h\
	.\spytreec.h\
	.\wndnode.h\
	.\wndtreed.h\
	.\spytreev.h\
	.\srchdlgs.h\
	.\tlhelp32.h\
	.\resource.h\
	.\spyxxhk.h\
	.\ime32.h\
	.\mstream.h\
	.\msgcrack.h\
	.\tabdlg.h\
	.\prfdb.h\
	.\procdb.h\
	.\property.h\
	.\propinsp.h\
	.\spyxx.h

$(INTDIR)/wndtreed.obj :  $(SOURCE)  $(DEP_WNDTR) $(INTDIR)\
 $(INTDIR)/spyxxpch.obj

# End Source File
################################################################################
# Begin Source File

SOURCE=.\prcptabs.cpp
DEP_PRCPT=\
	.\stdafx.h\
	.\hotlinkc.h\
	.\proptab.h\
	.\prcptabs.h\
	.\tlhelp32.h\
	.\resource.h\
	.\spyxxhk.h\
	.\ime32.h\
	.\mstream.h\
	.\msgcrack.h\
	.\tabdlg.h\
	.\prfdb.h\
	.\procdb.h\
	.\property.h\
	.\propinsp.h\
	.\spyxx.h

$(INTDIR)/prcptabs.obj :  $(SOURCE)  $(DEP_PRCPT) $(INTDIR)\
 $(INTDIR)/spyxxpch.obj

# End Source File
################################################################################
# Begin Source File

SOURCE=.\hotlinkc.cpp
DEP_HOTLI=\
	.\stdafx.h\
	.\hotlinkc.h\
	.\tlhelp32.h\
	.\resource.h\
	.\spyxxhk.h\
	.\ime32.h\
	.\mstream.h\
	.\msgcrack.h\
	.\tabdlg.h\
	.\prfdb.h\
	.\procdb.h\
	.\property.h\
	.\propinsp.h\
	.\spyxx.h

$(INTDIR)/hotlinkc.obj :  $(SOURCE)  $(DEP_HOTLI) $(INTDIR)\
 $(INTDIR)/spyxxpch.obj

# End Source File
################################################################################
# Begin Source File

SOURCE=.\btnmsgs.cpp
DEP_BTNMS=\
	.\stdafx.h\
	.\tlhelp32.h\
	.\resource.h\
	.\spyxxhk.h\
	.\ime32.h\
	.\mstream.h\
	.\msgcrack.h\
	.\tabdlg.h\
	.\prfdb.h\
	.\procdb.h\
	.\property.h\
	.\propinsp.h\
	.\spyxx.h

$(INTDIR)/btnmsgs.obj :  $(SOURCE)  $(DEP_BTNMS) $(INTDIR)\
 $(INTDIR)/spyxxpch.obj

# End Source File
################################################################################
# Begin Source File

SOURCE=.\property.cpp
DEP_PROPE=\
	.\stdafx.h\
	.\msglog.h\
	.\msgview.h\
	.\msgdoc.h\
	.\tlhelp32.h\
	.\resource.h\
	.\spyxxhk.h\
	.\ime32.h\
	.\mstream.h\
	.\msgcrack.h\
	.\tabdlg.h\
	.\prfdb.h\
	.\procdb.h\
	.\property.h\
	.\propinsp.h\
	.\spyxx.h

$(INTDIR)/property.obj :  $(SOURCE)  $(DEP_PROPE) $(INTDIR)\
 $(INTDIR)/spyxxpch.obj

# End Source File
################################################################################
# Begin Source File

SOURCE=.\srchdlgs.cpp
DEP_SRCHD=\
	.\stdafx.h\
	.\findtool.h\
	.\srchdlgs.h\
	.\spytreed.h\
	.\treectl.h\
	.\spytreec.h\
	.\wndnode.h\
	.\wndtreed.h\
	.\prctreed.h\
	.\thdtreed.h\
	.\msglog.h\
	.\msgview.h\
	.\msgdoc.h\
	.\tlhelp32.h\
	.\resource.h\
	.\spyxxhk.h\
	.\ime32.h\
	.\mstream.h\
	.\msgcrack.h\
	.\tabdlg.h\
	.\prfdb.h\
	.\procdb.h\
	.\property.h\
	.\propinsp.h\
	.\spyxx.h\
	.\imgwell.h

$(INTDIR)/srchdlgs.obj :  $(SOURCE)  $(DEP_SRCHD) $(INTDIR)\
 $(INTDIR)/spyxxpch.obj

# End Source File
################################################################################
# Begin Source File

SOURCE=.\editmsgs.cpp
DEP_EDITM=\
	.\stdafx.h\
	.\tlhelp32.h\
	.\resource.h\
	.\spyxxhk.h\
	.\ime32.h\
	.\mstream.h\
	.\msgcrack.h\
	.\tabdlg.h\
	.\prfdb.h\
	.\procdb.h\
	.\property.h\
	.\propinsp.h\
	.\spyxx.h

$(INTDIR)/editmsgs.obj :  $(SOURCE)  $(DEP_EDITM) $(INTDIR)\
 $(INTDIR)/spyxxpch.obj

# End Source File
################################################################################
# Begin Source File

SOURCE=.\hdrmsgs.cpp
DEP_HDRMS=\
	.\stdafx.h\
	.\tlhelp32.h\
	.\resource.h\
	.\spyxxhk.h\
	.\ime32.h\
	.\mstream.h\
	.\msgcrack.h\
	.\tabdlg.h\
	.\prfdb.h\
	.\procdb.h\
	.\property.h\
	.\propinsp.h\
	.\spyxx.h

$(INTDIR)/hdrmsgs.obj :  $(SOURCE)  $(DEP_HDRMS) $(INTDIR)\
 $(INTDIR)/spyxxpch.obj

# End Source File
################################################################################
# Begin Source File

SOURCE=.\mousmsgs.cpp
DEP_MOUSM=\
	.\stdafx.h\
	.\msglog.h\
	.\msgview.h\
	.\msgdoc.h\
	.\tlhelp32.h\
	.\resource.h\
	.\spyxxhk.h\
	.\ime32.h\
	.\mstream.h\
	.\msgcrack.h\
	.\tabdlg.h\
	.\prfdb.h\
	.\procdb.h\
	.\property.h\
	.\propinsp.h\
	.\spyxx.h

$(INTDIR)/mousmsgs.obj :  $(SOURCE)  $(DEP_MOUSM) $(INTDIR)\
 $(INTDIR)/spyxxpch.obj

# End Source File
################################################################################
# Begin Source File

SOURCE=.\wndnode.cpp
DEP_WNDNO=\
	.\stdafx.h\
	.\treectl.h\
	.\imgwell.h\
	.\spytreec.h\
	.\wndnode.h\
	.\tlhelp32.h\
	.\resource.h\
	.\spyxxhk.h\
	.\ime32.h\
	.\mstream.h\
	.\msgcrack.h\
	.\tabdlg.h\
	.\prfdb.h\
	.\procdb.h\
	.\property.h\
	.\propinsp.h\
	.\spyxx.h

$(INTDIR)/wndnode.obj :  $(SOURCE)  $(DEP_WNDNO) $(INTDIR)\
 $(INTDIR)/spyxxpch.obj

# End Source File
################################################################################
# Begin Source File

SOURCE=.\statmsgs.cpp
DEP_STATM=\
	.\stdafx.h\
	.\tlhelp32.h\
	.\resource.h\
	.\spyxxhk.h\
	.\ime32.h\
	.\mstream.h\
	.\msgcrack.h\
	.\tabdlg.h\
	.\prfdb.h\
	.\procdb.h\
	.\property.h\
	.\propinsp.h\
	.\spyxx.h

$(INTDIR)/statmsgs.obj :  $(SOURCE)  $(DEP_STATM) $(INTDIR)\
 $(INTDIR)/spyxxpch.obj

# End Source File
################################################################################
# Begin Source File

SOURCE=.\prfdb.cpp
DEP_PRFDB=\
	.\stdafx.h\
	.\tlhelp32.h\
	.\resource.h\
	.\spyxxhk.h\
	.\ime32.h\
	.\mstream.h\
	.\msgcrack.h\
	.\tabdlg.h\
	.\prfdb.h\
	.\procdb.h\
	.\property.h\
	.\propinsp.h\
	.\spyxx.h

$(INTDIR)/prfdb.obj :  $(SOURCE)  $(DEP_PRFDB) $(INTDIR) $(INTDIR)/spyxxpch.obj

# End Source File
################################################################################
# Begin Source File

SOURCE=.\dlgmsgs.cpp
DEP_DLGMS=\
	.\stdafx.h\
	.\tlhelp32.h\
	.\resource.h\
	.\spyxxhk.h\
	.\ime32.h\
	.\mstream.h\
	.\msgcrack.h\
	.\tabdlg.h\
	.\prfdb.h\
	.\procdb.h\
	.\property.h\
	.\propinsp.h\
	.\spyxx.h

$(INTDIR)/dlgmsgs.obj :  $(SOURCE)  $(DEP_DLGMS) $(INTDIR)\
 $(INTDIR)/spyxxpch.obj

# End Source File
################################################################################
# Begin Source File

SOURCE=.\spyxx.cpp
DEP_SPYXX_=\
	.\stdafx.h\
	.\msglog.h\
	.\msgview.h\
	.\msgdoc.h\
	.\hotlinkc.h\
	.\treectl.h\
	.\spytreed.h\
	.\spytreec.h\
	.\spytreev.h\
	.\wndnode.h\
	.\wndtreed.h\
	.\prctreed.h\
	.\thdtreed.h\
	.\mainfrm.h\
	.\aboutdlg.h\
	.\findtool.h\
	.\findwdlg.h\
	.\spyxxmdi.h\
	.\tlhelp32.h\
	.\resource.h\
	.\spyxxhk.h\
	.\ime32.h\
	.\mstream.h\
	.\msgcrack.h\
	.\tabdlg.h\
	.\prfdb.h\
	.\procdb.h\
	.\property.h\
	.\propinsp.h\
	.\spyxx.h\
	.\imgwell.h

$(INTDIR)/spyxx.obj :  $(SOURCE)  $(DEP_SPYXX_) $(INTDIR)\
 $(INTDIR)/spyxxpch.obj

# End Source File
################################################################################
# Begin Source File

SOURCE=.\proptab.cpp
DEP_PROPT=\
	.\stdafx.h\
	.\proptab.h\
	.\hotlinkc.h\
	.\tlhelp32.h\
	.\resource.h\
	.\spyxxhk.h\
	.\ime32.h\
	.\mstream.h\
	.\msgcrack.h\
	.\tabdlg.h\
	.\prfdb.h\
	.\procdb.h\
	.\property.h\
	.\propinsp.h\
	.\spyxx.h

$(INTDIR)/proptab.obj :  $(SOURCE)  $(DEP_PROPT) $(INTDIR)\
 $(INTDIR)/spyxxpch.obj

# End Source File
################################################################################
# Begin Source File

SOURCE=.\progmsgs.cpp
DEP_PROGM=\
	.\stdafx.h\
	.\tlhelp32.h\
	.\resource.h\
	.\spyxxhk.h\
	.\ime32.h\
	.\mstream.h\
	.\msgcrack.h\
	.\tabdlg.h\
	.\prfdb.h\
	.\procdb.h\
	.\property.h\
	.\propinsp.h\
	.\spyxx.h

$(INTDIR)/progmsgs.obj :  $(SOURCE)  $(DEP_PROGM) $(INTDIR)\
 $(INTDIR)/spyxxpch.obj

# End Source File
################################################################################
# Begin Source File

SOURCE=.\prftxtdb.cpp
DEP_PRFTX=\
	.\stdafx.h\
	.\prftxtdb.h\
	.\tlhelp32.h\
	.\resource.h\
	.\spyxxhk.h\
	.\ime32.h\
	.\mstream.h\
	.\msgcrack.h\
	.\tabdlg.h\
	.\prfdb.h\
	.\procdb.h\
	.\property.h\
	.\propinsp.h\
	.\spyxx.h

$(INTDIR)/prftxtdb.obj :  $(SOURCE)  $(DEP_PRFTX) $(INTDIR)\
 $(INTDIR)/spyxxpch.obj

# End Source File
################################################################################
# Begin Source File

SOURCE=.\msgdoc.cpp
DEP_MSGDOC=\
	.\stdafx.h\
	.\msglog.h\
	.\msgview.h\
	.\msgdoc.h\
	.\mainfrm.h\
	.\findtool.h\
	.\filtrdlg.h\
	.\srchdlgs.h\
	.\tlhelp32.h\
	.\resource.h\
	.\spyxxhk.h\
	.\ime32.h\
	.\mstream.h\
	.\msgcrack.h\
	.\tabdlg.h\
	.\prfdb.h\
	.\procdb.h\
	.\property.h\
	.\propinsp.h\
	.\spyxx.h

$(INTDIR)/msgdoc.obj :  $(SOURCE)  $(DEP_MSGDOC) $(INTDIR)\
 $(INTDIR)/spyxxpch.obj

# End Source File
################################################################################
# Begin Source File

SOURCE=.\treectl.cpp
DEP_TREEC=\
	.\stdafx.h\
	.\treectl.h\
	.\imgwell.h\
	.\tlhelp32.h\
	.\resource.h\
	.\spyxxhk.h\
	.\ime32.h\
	.\mstream.h\
	.\msgcrack.h\
	.\tabdlg.h\
	.\prfdb.h\
	.\procdb.h\
	.\property.h\
	.\propinsp.h\
	.\spyxx.h

$(INTDIR)/treectl.obj :  $(SOURCE)  $(DEP_TREEC) $(INTDIR)\
 $(INTDIR)/spyxxpch.obj

# End Source File
################################################################################
# Begin Source File

SOURCE=.\cbmsgs.cpp
DEP_CBMSG=\
	.\stdafx.h\
	.\tlhelp32.h\
	.\resource.h\
	.\spyxxhk.h\
	.\ime32.h\
	.\mstream.h\
	.\msgcrack.h\
	.\tabdlg.h\
	.\prfdb.h\
	.\procdb.h\
	.\property.h\
	.\propinsp.h\
	.\spyxx.h

$(INTDIR)/cbmsgs.obj :  $(SOURCE)  $(DEP_CBMSG) $(INTDIR)\
 $(INTDIR)/spyxxpch.obj

# End Source File
################################################################################
# Begin Source File

SOURCE=.\findwdlg.cpp
DEP_FINDW=\
	.\stdafx.h\
	.\findtool.h\
	.\findwdlg.h\
	.\tlhelp32.h\
	.\resource.h\
	.\spyxxhk.h\
	.\ime32.h\
	.\mstream.h\
	.\msgcrack.h\
	.\tabdlg.h\
	.\prfdb.h\
	.\procdb.h\
	.\property.h\
	.\propinsp.h\
	.\spyxx.h

$(INTDIR)/findwdlg.obj :  $(SOURCE)  $(DEP_FINDW) $(INTDIR)\
 $(INTDIR)/spyxxpch.obj

# End Source File
################################################################################
# Begin Source File

SOURCE=.\ttmsgs.cpp
DEP_TTMSG=\
	.\stdafx.h\
	.\tlhelp32.h\
	.\resource.h\
	.\spyxxhk.h\
	.\ime32.h\
	.\mstream.h\
	.\msgcrack.h\
	.\tabdlg.h\
	.\prfdb.h\
	.\procdb.h\
	.\property.h\
	.\propinsp.h\
	.\spyxx.h

$(INTDIR)/ttmsgs.obj :  $(SOURCE)  $(DEP_TTMSG) $(INTDIR)\
 $(INTDIR)/spyxxpch.obj

# End Source File
################################################################################
# Begin Source File

SOURCE=.\hkmsgs.cpp
DEP_HKMSG=\
	.\stdafx.h\
	.\tlhelp32.h\
	.\resource.h\
	.\spyxxhk.h\
	.\ime32.h\
	.\mstream.h\
	.\msgcrack.h\
	.\tabdlg.h\
	.\prfdb.h\
	.\procdb.h\
	.\property.h\
	.\propinsp.h\
	.\spyxx.h

$(INTDIR)/hkmsgs.obj :  $(SOURCE)  $(DEP_HKMSG) $(INTDIR)\
 $(INTDIR)/spyxxpch.obj

# End Source File
################################################################################
# Begin Source File

SOURCE=.\aboutdlg.cpp
DEP_ABOUT=\
	.\stdafx.h\
	.\aboutdlg.h\
	.\version.h\
	.\tlhelp32.h\
	.\resource.h\
	.\spyxxhk.h\
	.\ime32.h\
	.\mstream.h\
	.\msgcrack.h\
	.\tabdlg.h\
	.\prfdb.h\
	.\procdb.h\
	.\property.h\
	.\propinsp.h\
	.\spyxx.h

$(INTDIR)/aboutdlg.obj :  $(SOURCE)  $(DEP_ABOUT) $(INTDIR)\
 $(INTDIR)/spyxxpch.obj

# End Source File
################################################################################
# Begin Source File

SOURCE=.\thdptabs.cpp
DEP_THDPT=\
	.\stdafx.h\
	.\hotlinkc.h\
	.\proptab.h\
	.\thdptabs.h\
	.\tlhelp32.h\
	.\resource.h\
	.\spyxxhk.h\
	.\ime32.h\
	.\mstream.h\
	.\msgcrack.h\
	.\tabdlg.h\
	.\prfdb.h\
	.\procdb.h\
	.\property.h\
	.\propinsp.h\
	.\spyxx.h

$(INTDIR)/thdptabs.obj :  $(SOURCE)  $(DEP_THDPT) $(INTDIR)\
 $(INTDIR)/spyxxpch.obj

# End Source File
################################################################################
# Begin Source File

SOURCE=.\fontdlg.cpp
DEP_FONTD=\
	.\stdafx.h\
	.\fontdlg.h\
	.\tlhelp32.h\
	.\resource.h\
	.\spyxxhk.h\
	.\ime32.h\
	.\mstream.h\
	.\msgcrack.h\
	.\tabdlg.h\
	.\prfdb.h\
	.\procdb.h\
	.\property.h\
	.\propinsp.h\
	.\spyxx.h

$(INTDIR)/fontdlg.obj :  $(SOURCE)  $(DEP_FONTD) $(INTDIR)\
 $(INTDIR)/spyxxpch.obj

# End Source File
################################################################################
# Begin Source File

SOURCE=.\ddemsgs.cpp
DEP_DDEMS=\
	.\stdafx.h\
	.\tlhelp32.h\
	.\resource.h\
	.\spyxxhk.h\
	.\ime32.h\
	.\mstream.h\
	.\msgcrack.h\
	.\tabdlg.h\
	.\prfdb.h\
	.\procdb.h\
	.\property.h\
	.\propinsp.h\
	.\spyxx.h

$(INTDIR)/ddemsgs.obj :  $(SOURCE)  $(DEP_DDEMS) $(INTDIR)\
 $(INTDIR)/spyxxpch.obj

# End Source File
################################################################################
# Begin Source File

SOURCE=.\msgview.cpp
DEP_MSGVI=\
	.\stdafx.h\
	.\msglog.h\
	.\msgview.h\
	.\msgdoc.h\
	.\fontdlg.h\
	.\tlhelp32.h\
	.\resource.h\
	.\spyxxhk.h\
	.\ime32.h\
	.\mstream.h\
	.\msgcrack.h\
	.\tabdlg.h\
	.\prfdb.h\
	.\procdb.h\
	.\property.h\
	.\propinsp.h\
	.\spyxx.h

$(INTDIR)/msgview.obj :  $(SOURCE)  $(DEP_MSGVI) $(INTDIR)\
 $(INTDIR)/spyxxpch.obj

# End Source File
################################################################################
# Begin Source File

SOURCE=.\thdnode.cpp
DEP_THDNO=\
	.\stdafx.h\
	.\treectl.h\
	.\spytreec.h\
	.\thdnode.h\
	.\wndnode.h\
	.\tlhelp32.h\
	.\resource.h\
	.\spyxxhk.h\
	.\ime32.h\
	.\mstream.h\
	.\msgcrack.h\
	.\tabdlg.h\
	.\prfdb.h\
	.\procdb.h\
	.\property.h\
	.\propinsp.h\
	.\spyxx.h\
	.\imgwell.h

$(INTDIR)/thdnode.obj :  $(SOURCE)  $(DEP_THDNO) $(INTDIR)\
 $(INTDIR)/spyxxpch.obj

# End Source File
################################################################################
# Begin Source File

SOURCE=.\help.cpp
DEP_HELP_=\
	.\stdafx.h\
	.\helpdirs.h\
	.\tlhelp32.h\
	.\resource.h\
	.\spyxxhk.h\
	.\ime32.h\
	.\mstream.h\
	.\msgcrack.h\
	.\tabdlg.h\
	.\prfdb.h\
	.\procdb.h\
	.\property.h\
	.\propinsp.h\
	.\spyxx.h

$(INTDIR)/help.obj :  $(SOURCE)  $(DEP_HELP_) $(INTDIR) $(INTDIR)/spyxxpch.obj

# End Source File
################################################################################
# Begin Source File

SOURCE=.\updnmsgs.cpp
DEP_UPDNM=\
	.\stdafx.h\
	.\tlhelp32.h\
	.\resource.h\
	.\spyxxhk.h\
	.\ime32.h\
	.\mstream.h\
	.\msgcrack.h\
	.\tabdlg.h\
	.\prfdb.h\
	.\procdb.h\
	.\property.h\
	.\propinsp.h\
	.\spyxx.h

$(INTDIR)/updnmsgs.obj :  $(SOURCE)  $(DEP_UPDNM) $(INTDIR)\
 $(INTDIR)/spyxxpch.obj

# End Source File
################################################################################
# Begin Source File

SOURCE=.\prctreed.cpp
DEP_PRCTR=\
	.\stdafx.h\
	.\spytreed.h\
	.\prctreed.h\
	.\treectl.h\
	.\imgwell.h\
	.\spytreec.h\
	.\prcnode.h\
	.\thdnode.h\
	.\findtool.h\
	.\spytreev.h\
	.\srchdlgs.h\
	.\mainfrm.h\
	.\tlhelp32.h\
	.\resource.h\
	.\spyxxhk.h\
	.\ime32.h\
	.\mstream.h\
	.\msgcrack.h\
	.\tabdlg.h\
	.\prfdb.h\
	.\procdb.h\
	.\property.h\
	.\propinsp.h\
	.\spyxx.h

$(INTDIR)/prctreed.obj :  $(SOURCE)  $(DEP_PRCTR) $(INTDIR)\
 $(INTDIR)/spyxxpch.obj

# End Source File
################################################################################
# Begin Source File

SOURCE=.\msgcrack.cpp
DEP_MSGCR=\
	.\stdafx.h\
	.\tlhelp32.h\
	.\resource.h\
	.\spyxxhk.h\
	.\ime32.h\
	.\mstream.h\
	.\msgcrack.h\
	.\tabdlg.h\
	.\prfdb.h\
	.\procdb.h\
	.\property.h\
	.\propinsp.h\
	.\spyxx.h

$(INTDIR)/msgcrack.obj :  $(SOURCE)  $(DEP_MSGCR) $(INTDIR)\
 $(INTDIR)/spyxxpch.obj

# End Source File
################################################################################
# Begin Source File

SOURCE=.\tabmsgs.cpp
DEP_TABMS=\
	.\stdafx.h\
	.\tlhelp32.h\
	.\resource.h\
	.\spyxxhk.h\
	.\ime32.h\
	.\mstream.h\
	.\msgcrack.h\
	.\tabdlg.h\
	.\prfdb.h\
	.\procdb.h\
	.\property.h\
	.\propinsp.h\
	.\spyxx.h

$(INTDIR)/tabmsgs.obj :  $(SOURCE)  $(DEP_TABMS) $(INTDIR)\
 $(INTDIR)/spyxxpch.obj

# End Source File
################################################################################
# Begin Source File

SOURCE=.\spyxxmdi.cpp
DEP_SPYXXM=\
	.\stdafx.h\
	.\spyxxmdi.h\
	.\tlhelp32.h\
	.\resource.h\
	.\spyxxhk.h\
	.\ime32.h\
	.\mstream.h\
	.\msgcrack.h\
	.\tabdlg.h\
	.\prfdb.h\
	.\procdb.h\
	.\property.h\
	.\propinsp.h\
	.\spyxx.h

$(INTDIR)/spyxxmdi.obj :  $(SOURCE)  $(DEP_SPYXXM) $(INTDIR)\
 $(INTDIR)/spyxxpch.obj

# End Source File
################################################################################
# Begin Source File

SOURCE=.\sbmsgs.cpp
DEP_SBMSG=\
	.\stdafx.h\
	.\tlhelp32.h\
	.\resource.h\
	.\spyxxhk.h\
	.\ime32.h\
	.\mstream.h\
	.\msgcrack.h\
	.\tabdlg.h\
	.\prfdb.h\
	.\procdb.h\
	.\property.h\
	.\propinsp.h\
	.\spyxx.h

$(INTDIR)/sbmsgs.obj :  $(SOURCE)  $(DEP_SBMSG) $(INTDIR)\
 $(INTDIR)/spyxxpch.obj

# End Source File
################################################################################
# Begin Source File

SOURCE=.\clipmsgs.cpp
DEP_CLIPM=\
	.\stdafx.h\
	.\tlhelp32.h\
	.\resource.h\
	.\spyxxhk.h\
	.\ime32.h\
	.\mstream.h\
	.\msgcrack.h\
	.\tabdlg.h\
	.\prfdb.h\
	.\procdb.h\
	.\property.h\
	.\propinsp.h\
	.\spyxx.h

$(INTDIR)/clipmsgs.obj :  $(SOURCE)  $(DEP_CLIPM) $(INTDIR)\
 $(INTDIR)/spyxxpch.obj

# End Source File
################################################################################
# Begin Source File

SOURCE=.\spyxx.rc
DEP_SPYXX_R=\
	.\res\spyxx.ico\
	.\res\message.ico\
	.\res\prctree.ico\
	.\res\wndtree.ico\
	.\res\thdtree.ico\
	.\res\findtl.ico\
	.\res\findtl2.ico\
	.\res\spyxx2.ico\
	.\res\toolbar.bmp\
	.\res\expnodes.bmp\
	.\res\folders.bmp\
	.\res\treectl.bmp\
	.\res\scroll.bmp\
	.\res\scroll_l.bmp\
	.\res\scroll_r.bmp\
	.\res\scrollrd.bmp\
	.\res\scrollld.bmp\
	.\res\spyxxbig.bmp\
	.\res\findtl.cur\
	.\res\jumphand.cur\
	.\resource.h\
	.\res\spy.rc2

$(INTDIR)/spyxx.res :  $(SOURCE)  $(DEP_SPYXX_R) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\version.h
# End Source File
################################################################################
# Begin Source File

SOURCE=.\tabdlg.cpp
DEP_TABDL=\
	.\stdafx.h\
	.\tlhelp32.h\
	.\resource.h\
	.\spyxxhk.h\
	.\ime32.h\
	.\mstream.h\
	.\msgcrack.h\
	.\tabdlg.h\
	.\prfdb.h\
	.\procdb.h\
	.\property.h\
	.\propinsp.h\
	.\spyxx.h

$(INTDIR)/tabdlg.obj :  $(SOURCE)  $(DEP_TABDL) $(INTDIR)\
 $(INTDIR)/spyxxpch.obj

# End Source File
# End Group
# End Project
################################################################################
