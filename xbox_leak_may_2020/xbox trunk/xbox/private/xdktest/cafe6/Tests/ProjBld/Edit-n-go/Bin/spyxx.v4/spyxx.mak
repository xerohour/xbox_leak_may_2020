# Microsoft Visual C++ Generated NMAKE File, Format Version 3.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

!IF "$(CFG)" == ""
CFG=Win32 (80x86) Debug
!MESSAGE No configuration specified.  Defaulting to Win32 (80x86) Debug.
!ENDIF 

!IF "$(CFG)" != "Win32 (80x86) Release" && "$(CFG)" != "Win32 (80x86) Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "spyxx.mak" CFG="Win32 (80x86) Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Win32 (80x86) Release" (based on "Win32 (x86) Application")
!MESSAGE "Win32 (80x86) Debug" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 
################################################################################
# Begin Project
# PROP Target_Last_Scanned "Win32 (80x86) Debug"
MTL=mktyplib.exe
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Win32 (80x86) Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir WinRel
# PROP BASE Intermediate_Dir WinRel
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir WinRel
# PROP Intermediate_Dir WinRel
OUTDIR=.\WinRel
INTDIR=.\WinRel

ALL : "$(OUTDIR)/spyxx.exe" "$(OUTDIR)/spyxx.bsc"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

F90=fl32.exe
# ADD BASE F90 /I "WinRel/"
# ADD F90 /I "WinRel/"
F90_OBJS=.\WinRel/
F90_PROJ=/I "WinDebug/" /Fo"WinDebug/" 

.for{$(F90_OBJS)}.obj:
   $(F90) $(F90_PROJ) $<  

.f{$(F90_OBJS)}.obj:
   $(F90) $(F90_PROJ) $<  

.f90{$(F90_OBJS)}.obj:
   $(F90) $(F90_PROJ) $<  

# ADD BASE CPP /nologo /MD /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /c
# ADD CPP /nologo /MD /W3 /Gi- /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_X86_" /D "DISABLE_WIN95_MESSAGES" /FR /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /Gi- /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_X86_" /D "DISABLE_WIN95_MESSAGES" /FR"$(INTDIR)/"\
 /Fp"$(INTDIR)/spyxx.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\WinRel/
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/spyxx.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/spyxx.bsc" 
BSC32_SBRS= \
	"$(INTDIR)/thdptabs.sbr" \
	"$(INTDIR)/mstream.sbr" \
	"$(INTDIR)/kbdmsgs.sbr" \
	"$(INTDIR)/prctreed.sbr" \
	"$(INTDIR)/msgdoc2.sbr" \
	"$(INTDIR)/mainfrm.sbr" \
	"$(INTDIR)/sbmsgs.sbr" \
	"$(INTDIR)/msgcrack.sbr" \
	"$(INTDIR)/thdnode.sbr" \
	"$(INTDIR)/prcnode.sbr" \
	"$(INTDIR)/outfdlg.sbr" \
	"$(INTDIR)/msgdoc.sbr" \
	"$(INTDIR)/msgfdlg.sbr" \
	"$(INTDIR)/cbmsgs.sbr" \
	"$(INTDIR)/winfdlg.sbr" \
	"$(INTDIR)/help.sbr" \
	"$(INTDIR)/filtrdlg.sbr" \
	"$(INTDIR)/msglog.sbr" \
	"$(INTDIR)/wndptabs.sbr" \
	"$(INTDIR)/msgptabs.sbr" \
	"$(INTDIR)/mdimsgs.sbr" \
	"$(INTDIR)/winmsgs.sbr" \
	"$(INTDIR)/editmsgs.sbr" \
	"$(INTDIR)/msghook.sbr" \
	"$(INTDIR)/thdtreed.sbr" \
	"$(INTDIR)/spytreev.sbr" \
	"$(INTDIR)/propinsp.sbr" \
	"$(INTDIR)/mousmsgs.sbr" \
	"$(INTDIR)/findtool.sbr" \
	"$(INTDIR)/procdb.sbr" \
	"$(INTDIR)/spytreed.sbr" \
	"$(INTDIR)/wndnode.sbr" \
	"$(INTDIR)/spytreec.sbr" \
	"$(INTDIR)/lbmsgs.sbr" \
	"$(INTDIR)/proptab.sbr" \
	"$(INTDIR)/treectl.sbr" \
	"$(INTDIR)/tabdlg.sbr" \
	"$(INTDIR)/helpdirs.sbr" \
	"$(INTDIR)/wndtreed.sbr" \
	"$(INTDIR)/prcptabs.sbr" \
	"$(INTDIR)/hotlinkc.sbr" \
	"$(INTDIR)/property.sbr" \
	"$(INTDIR)/fontdlg.sbr" \
	"$(INTDIR)/msgview.sbr" \
	"$(INTDIR)/imgwell.sbr" \
	"$(INTDIR)/clipmsgs.sbr" \
	"$(INTDIR)/ncmsgs.sbr" \
	"$(INTDIR)/prftxtdb.sbr" \
	"$(INTDIR)/findwdlg.sbr" \
	"$(INTDIR)/prfdb.sbr" \
	"$(INTDIR)/aboutdlg.sbr" \
	"$(INTDIR)/srchdlgs.sbr" \
	"$(INTDIR)/spyxxpch.sbr" \
	"$(INTDIR)/btnmsgs.sbr" \
	"$(INTDIR)/imemsgs.sbr" \
	"$(INTDIR)/ddemsgs.sbr" \
	"$(INTDIR)/statmsgs.sbr" \
	"$(INTDIR)/dlgmsgs.sbr" \
	"$(INTDIR)/spyxx.sbr" \
	"$(INTDIR)/animsgs.sbr" \
	"$(INTDIR)/updnmsgs.sbr" \
	"$(INTDIR)/trkmsgs.sbr" \
	"$(INTDIR)/tbmsgs.sbr" \
	"$(INTDIR)/lvmsgs.sbr" \
	"$(INTDIR)/stbmsgs.sbr" \
	"$(INTDIR)/hdrmsgs.sbr" \
	"$(INTDIR)/tvmsgs.sbr" \
	"$(INTDIR)/progmsgs.sbr" \
	"$(INTDIR)/ttmsgs.sbr" \
	"$(INTDIR)/tabmsgs.sbr" \
	"$(INTDIR)/hkmsgs.sbr" \
	"$(INTDIR)/spyxxmdi.sbr"

"$(OUTDIR)/spyxx.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:$(PROCESSOR_ARCHITECTURE)
# ADD LINK32 mfc40.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib WinRel/spyhk30.lib /nologo /subsystem:windows /machine:$(PROCESSOR_ARCHITECTURE) /nodefaultlib
# SUBTRACT LINK32 /incremental:yes
LINK32_FLAGS=mfc40.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib\
 shell32.lib WinRel/spyhk30.lib /nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/spyxx.pdb" /machine:$(PROCESSOR_ARCHITECTURE) /nodefaultlib\
 /out:"$(OUTDIR)/spyxx.exe" 
DEF_FILE=
LINK32_OBJS= \
	"$(INTDIR)/thdptabs.obj" \
	"$(INTDIR)/mstream.obj" \
	"$(INTDIR)/kbdmsgs.obj" \
	"$(INTDIR)/prctreed.obj" \
	"$(INTDIR)/msgdoc2.obj" \
	"$(INTDIR)/mainfrm.obj" \
	"$(INTDIR)/sbmsgs.obj" \
	"$(INTDIR)/msgcrack.obj" \
	"$(INTDIR)/thdnode.obj" \
	"$(INTDIR)/prcnode.obj" \
	"$(INTDIR)/outfdlg.obj" \
	"$(INTDIR)/msgdoc.obj" \
	"$(INTDIR)/msgfdlg.obj" \
	"$(INTDIR)/cbmsgs.obj" \
	"$(INTDIR)/winfdlg.obj" \
	"$(INTDIR)/help.obj" \
	"$(INTDIR)/filtrdlg.obj" \
	"$(INTDIR)/msglog.obj" \
	"$(INTDIR)/wndptabs.obj" \
	"$(INTDIR)/msgptabs.obj" \
	"$(INTDIR)/mdimsgs.obj" \
	"$(INTDIR)/winmsgs.obj" \
	"$(INTDIR)/editmsgs.obj" \
	"$(INTDIR)/msghook.obj" \
	"$(INTDIR)/thdtreed.obj" \
	"$(INTDIR)/spytreev.obj" \
	"$(INTDIR)/propinsp.obj" \
	"$(INTDIR)/mousmsgs.obj" \
	"$(INTDIR)/findtool.obj" \
	"$(INTDIR)/procdb.obj" \
	"$(INTDIR)/spytreed.obj" \
	"$(INTDIR)/wndnode.obj" \
	"$(INTDIR)/spytreec.obj" \
	"$(INTDIR)/lbmsgs.obj" \
	"$(INTDIR)/proptab.obj" \
	"$(INTDIR)/treectl.obj" \
	"$(INTDIR)/tabdlg.obj" \
	"$(INTDIR)/helpdirs.obj" \
	"$(INTDIR)/wndtreed.obj" \
	"$(INTDIR)/prcptabs.obj" \
	"$(INTDIR)/hotlinkc.obj" \
	"$(INTDIR)/property.obj" \
	"$(INTDIR)/fontdlg.obj" \
	"$(INTDIR)/msgview.obj" \
	"$(INTDIR)/imgwell.obj" \
	"$(INTDIR)/clipmsgs.obj" \
	"$(INTDIR)/ncmsgs.obj" \
	"$(INTDIR)/prftxtdb.obj" \
	"$(INTDIR)/findwdlg.obj" \
	"$(INTDIR)/prfdb.obj" \
	"$(INTDIR)/aboutdlg.obj" \
	"$(INTDIR)/srchdlgs.obj" \
	"$(INTDIR)/spyxxpch.obj" \
	"$(INTDIR)/btnmsgs.obj" \
	"$(INTDIR)/imemsgs.obj" \
	"$(INTDIR)/ddemsgs.obj" \
	"$(INTDIR)/statmsgs.obj" \
	"$(INTDIR)/dlgmsgs.obj" \
	"$(INTDIR)/spyxx.res" \
	"$(INTDIR)/spyxx.obj" \
	"$(INTDIR)/animsgs.obj" \
	"$(INTDIR)/updnmsgs.obj" \
	"$(INTDIR)/trkmsgs.obj" \
	"$(INTDIR)/tbmsgs.obj" \
	"$(INTDIR)/lvmsgs.obj" \
	"$(INTDIR)/stbmsgs.obj" \
	"$(INTDIR)/hdrmsgs.obj" \
	"$(INTDIR)/tvmsgs.obj" \
	"$(INTDIR)/progmsgs.obj" \
	"$(INTDIR)/ttmsgs.obj" \
	"$(INTDIR)/tabmsgs.obj" \
	"$(INTDIR)/hkmsgs.obj" \
	"$(INTDIR)/spyxxmdi.obj"

"$(OUTDIR)/spyxx.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir WinDebug
# PROP BASE Intermediate_Dir WinDebug
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir WinDebug
# PROP Intermediate_Dir WinDebug
OUTDIR=.\WinDebug
INTDIR=.\WinDebug

ALL : "$(OUTDIR)/spyxx.exe" "$(OUTDIR)/spyxx.bsc"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

F90=fl32.exe
# ADD BASE F90 /I "WinDebug/"
# ADD F90 /I "WinDebug/"
F90_OBJS=.\WinDebug/
F90_PROJ=/I "WinDebug/" /Fo"WinDebug/" 

.for{$(F90_OBJS)}.obj:
   $(F90) $(F90_PROJ) $<  

.f{$(F90_OBJS)}.obj:
   $(F90) $(F90_PROJ) $<  

.f90{$(F90_OBJS)}.obj:
   $(F90) $(F90_PROJ) $<  

# ADD BASE CPP /nologo /MD /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "NDEBUG" /FR /c
# ADD CPP /nologo /MD /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_X86_" /D "DISABLE_WIN95_MESSAGES" /FR /Yu"stdafx.h" /Bt /c
CPP_PROJ=/nologo /MD /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_X86_" /D "DISABLE_WIN95_MESSAGES" /FR"$(INTDIR)/"\
 /Fp"$(INTDIR)/spyxx.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/" /Fd"$(OUTDIR)/spyxx.pdb"\
 /Bt /c 
CPP_OBJS=.\WinDebug/
# ADD BASE MTL /nologo /D "_DEBUG" /D "NDEBUG" /win32 /mips /mips
# ADD MTL /nologo /D "_DEBUG" /D "NDEBUG" /win32 /mips /mips
MTL_PROJ=/nologo /D "_DEBUG" /D "NDEBUG" /win32 /mips /mips 
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL" /d "NDEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/spyxx.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/spyxx.bsc" 
BSC32_SBRS= \
	"$(INTDIR)/thdptabs.sbr" \
	"$(INTDIR)/mstream.sbr" \
	"$(INTDIR)/kbdmsgs.sbr" \
	"$(INTDIR)/prctreed.sbr" \
	"$(INTDIR)/msgdoc2.sbr" \
	"$(INTDIR)/mainfrm.sbr" \
	"$(INTDIR)/sbmsgs.sbr" \
	"$(INTDIR)/msgcrack.sbr" \
	"$(INTDIR)/thdnode.sbr" \
	"$(INTDIR)/prcnode.sbr" \
	"$(INTDIR)/outfdlg.sbr" \
	"$(INTDIR)/msgdoc.sbr" \
	"$(INTDIR)/msgfdlg.sbr" \
	"$(INTDIR)/cbmsgs.sbr" \
	"$(INTDIR)/winfdlg.sbr" \
	"$(INTDIR)/help.sbr" \
	"$(INTDIR)/filtrdlg.sbr" \
	"$(INTDIR)/msglog.sbr" \
	"$(INTDIR)/wndptabs.sbr" \
	"$(INTDIR)/msgptabs.sbr" \
	"$(INTDIR)/mdimsgs.sbr" \
	"$(INTDIR)/winmsgs.sbr" \
	"$(INTDIR)/editmsgs.sbr" \
	"$(INTDIR)/msghook.sbr" \
	"$(INTDIR)/thdtreed.sbr" \
	"$(INTDIR)/spytreev.sbr" \
	"$(INTDIR)/propinsp.sbr" \
	"$(INTDIR)/mousmsgs.sbr" \
	"$(INTDIR)/findtool.sbr" \
	"$(INTDIR)/procdb.sbr" \
	"$(INTDIR)/spytreed.sbr" \
	"$(INTDIR)/wndnode.sbr" \
	"$(INTDIR)/spytreec.sbr" \
	"$(INTDIR)/lbmsgs.sbr" \
	"$(INTDIR)/proptab.sbr" \
	"$(INTDIR)/treectl.sbr" \
	"$(INTDIR)/tabdlg.sbr" \
	"$(INTDIR)/helpdirs.sbr" \
	"$(INTDIR)/wndtreed.sbr" \
	"$(INTDIR)/prcptabs.sbr" \
	"$(INTDIR)/hotlinkc.sbr" \
	"$(INTDIR)/property.sbr" \
	"$(INTDIR)/fontdlg.sbr" \
	"$(INTDIR)/msgview.sbr" \
	"$(INTDIR)/imgwell.sbr" \
	"$(INTDIR)/clipmsgs.sbr" \
	"$(INTDIR)/ncmsgs.sbr" \
	"$(INTDIR)/prftxtdb.sbr" \
	"$(INTDIR)/findwdlg.sbr" \
	"$(INTDIR)/prfdb.sbr" \
	"$(INTDIR)/aboutdlg.sbr" \
	"$(INTDIR)/srchdlgs.sbr" \
	"$(INTDIR)/spyxxpch.sbr" \
	"$(INTDIR)/btnmsgs.sbr" \
	"$(INTDIR)/imemsgs.sbr" \
	"$(INTDIR)/ddemsgs.sbr" \
	"$(INTDIR)/statmsgs.sbr" \
	"$(INTDIR)/dlgmsgs.sbr" \
	"$(INTDIR)/spyxx.sbr" \
	"$(INTDIR)/animsgs.sbr" \
	"$(INTDIR)/updnmsgs.sbr" \
	"$(INTDIR)/trkmsgs.sbr" \
	"$(INTDIR)/tbmsgs.sbr" \
	"$(INTDIR)/lvmsgs.sbr" \
	"$(INTDIR)/stbmsgs.sbr" \
	"$(INTDIR)/hdrmsgs.sbr" \
	"$(INTDIR)/tvmsgs.sbr" \
	"$(INTDIR)/progmsgs.sbr" \
	"$(INTDIR)/ttmsgs.sbr" \
	"$(INTDIR)/tabmsgs.sbr" \
	"$(INTDIR)/hkmsgs.sbr" \
	"$(INTDIR)/spyxxmdi.sbr"

"$(OUTDIR)/spyxx.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:MIPS
# ADD LINK32 mfc40d.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib WinDebug/spyhk30.lib mfc40.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib mfc40d.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib WinDebug/spyhk30.lib /nologo /subsystem:windows /debug /machine:$(PROCESSOR_ARCHITECTURE)
# SUBTRACT LINK32 /pdb:none /incremental:no
LINK32_FLAGS=mfc40d.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib\
 shell32.lib WinDebug/spyhk30.lib mfc40.lib user32.lib gdi32.lib comdlg32.lib\
 advapi32.lib shell32.lib mfc40d.lib user32.lib gdi32.lib comdlg32.lib\
 advapi32.lib shell32.lib WinDebug/spyhk30.lib /nologo /subsystem:windows\
 /incremental:yes /pdb:"$(OUTDIR)/spyxx.pdb" /debug /machine:$(PROCESSOR_ARCHITECTURE)\
 /out:"$(OUTDIR)/spyxx.exe" 
DEF_FILE=
LINK32_OBJS= \
	"$(INTDIR)/thdptabs.obj" \
	"$(INTDIR)/mstream.obj" \
	"$(INTDIR)/kbdmsgs.obj" \
	"$(INTDIR)/prctreed.obj" \
	"$(INTDIR)/msgdoc2.obj" \
	"$(INTDIR)/mainfrm.obj" \
	"$(INTDIR)/sbmsgs.obj" \
	"$(INTDIR)/msgcrack.obj" \
	"$(INTDIR)/thdnode.obj" \
	"$(INTDIR)/prcnode.obj" \
	"$(INTDIR)/outfdlg.obj" \
	"$(INTDIR)/msgdoc.obj" \
	"$(INTDIR)/msgfdlg.obj" \
	"$(INTDIR)/cbmsgs.obj" \
	"$(INTDIR)/winfdlg.obj" \
	"$(INTDIR)/help.obj" \
	"$(INTDIR)/filtrdlg.obj" \
	"$(INTDIR)/msglog.obj" \
	"$(INTDIR)/wndptabs.obj" \
	"$(INTDIR)/msgptabs.obj" \
	"$(INTDIR)/mdimsgs.obj" \
	"$(INTDIR)/winmsgs.obj" \
	"$(INTDIR)/editmsgs.obj" \
	"$(INTDIR)/msghook.obj" \
	"$(INTDIR)/thdtreed.obj" \
	"$(INTDIR)/spytreev.obj" \
	"$(INTDIR)/propinsp.obj" \
	"$(INTDIR)/mousmsgs.obj" \
	"$(INTDIR)/findtool.obj" \
	"$(INTDIR)/procdb.obj" \
	"$(INTDIR)/spytreed.obj" \
	"$(INTDIR)/wndnode.obj" \
	"$(INTDIR)/spytreec.obj" \
	"$(INTDIR)/lbmsgs.obj" \
	"$(INTDIR)/proptab.obj" \
	"$(INTDIR)/treectl.obj" \
	"$(INTDIR)/tabdlg.obj" \
	"$(INTDIR)/helpdirs.obj" \
	"$(INTDIR)/wndtreed.obj" \
	"$(INTDIR)/prcptabs.obj" \
	"$(INTDIR)/hotlinkc.obj" \
	"$(INTDIR)/property.obj" \
	"$(INTDIR)/fontdlg.obj" \
	"$(INTDIR)/msgview.obj" \
	"$(INTDIR)/imgwell.obj" \
	"$(INTDIR)/clipmsgs.obj" \
	"$(INTDIR)/ncmsgs.obj" \
	"$(INTDIR)/prftxtdb.obj" \
	"$(INTDIR)/findwdlg.obj" \
	"$(INTDIR)/prfdb.obj" \
	"$(INTDIR)/aboutdlg.obj" \
	"$(INTDIR)/srchdlgs.obj" \
	"$(INTDIR)/spyxxpch.obj" \
	"$(INTDIR)/btnmsgs.obj" \
	"$(INTDIR)/imemsgs.obj" \
	"$(INTDIR)/ddemsgs.obj" \
	"$(INTDIR)/statmsgs.obj" \
	"$(INTDIR)/dlgmsgs.obj" \
	"$(INTDIR)/spyxx.res" \
	"$(INTDIR)/spyxx.obj" \
	"$(INTDIR)/animsgs.obj" \
	"$(INTDIR)/updnmsgs.obj" \
	"$(INTDIR)/trkmsgs.obj" \
	"$(INTDIR)/tbmsgs.obj" \
	"$(INTDIR)/lvmsgs.obj" \
	"$(INTDIR)/stbmsgs.obj" \
	"$(INTDIR)/hdrmsgs.obj" \
	"$(INTDIR)/tvmsgs.obj" \
	"$(INTDIR)/progmsgs.obj" \
	"$(INTDIR)/ttmsgs.obj" \
	"$(INTDIR)/tabmsgs.obj" \
	"$(INTDIR)/hkmsgs.obj" \
	"$(INTDIR)/spyxxmdi.obj"

"$(OUTDIR)/spyxx.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
# Begin Target

# Name "Win32 (80x86) Debug"
# Name "Win32 (80x86) Release"

!IF  "$(CFG)" == "Win32 (80x86) Debug"

!ELSEIF  "$(CFG)" == "Win32 (80x86) Release"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\thdptabs.cpp

!IF  "$(CFG)" == "Win32 (80x86) Release"

DEP_CPP_THDPT=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	".\hotlinkc.h"\
	".\proptab.h"\
	".\thdptabs.h"\
	

"$(INTDIR)/thdptabs.obj" : $(SOURCE) $(DEP_CPP_THDPT) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

DEP_CPP_THDPT=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	".\hotlinkc.h"\
	".\proptab.h"\
	".\thdptabs.h"\
	

"$(INTDIR)/thdptabs.obj" : $(SOURCE) $(DEP_CPP_THDPT) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\mstream.cpp

!IF  "$(CFG)" == "Win32 (80x86) Release"

DEP_CPP_MSTRE=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	".\msglog.h"\
	".\msgview.h"\
	

"$(INTDIR)/mstream.obj" : $(SOURCE) $(DEP_CPP_MSTRE) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

DEP_CPP_MSTRE=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	".\msglog.h"\
	".\msgview.h"\
	

"$(INTDIR)/mstream.obj" : $(SOURCE) $(DEP_CPP_MSTRE) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\kbdmsgs.cpp

!IF  "$(CFG)" == "Win32 (80x86) Release"

DEP_CPP_KBDMS=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	

"$(INTDIR)/kbdmsgs.obj" : $(SOURCE) $(DEP_CPP_KBDMS) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

DEP_CPP_KBDMS=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	

"$(INTDIR)/kbdmsgs.obj" : $(SOURCE) $(DEP_CPP_KBDMS) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\prctreed.cpp

!IF  "$(CFG)" == "Win32 (80x86) Release"

DEP_CPP_PRCTR=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	".\spytreed.h"\
	".\prctreed.h"\
	".\treectl.h"\
	".\imgwell.h"\
	".\spytreec.h"\
	".\prcnode.h"\
	".\thdnode.h"\
	".\findtool.h"\
	".\spytreev.h"\
	".\srchdlgs.h"\
	".\mainfrm.h"\
	

"$(INTDIR)/prctreed.obj" : $(SOURCE) $(DEP_CPP_PRCTR) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

DEP_CPP_PRCTR=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	".\spytreed.h"\
	".\prctreed.h"\
	".\treectl.h"\
	".\imgwell.h"\
	".\spytreec.h"\
	".\prcnode.h"\
	".\thdnode.h"\
	".\findtool.h"\
	".\spytreev.h"\
	".\srchdlgs.h"\
	".\mainfrm.h"\
	

"$(INTDIR)/prctreed.obj" : $(SOURCE) $(DEP_CPP_PRCTR) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\msgdoc2.cpp

!IF  "$(CFG)" == "Win32 (80x86) Release"

DEP_CPP_MSGDO=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	".\msglog.h"\
	".\msgview.h"\
	".\msgdoc.h"\
	

"$(INTDIR)/msgdoc2.obj" : $(SOURCE) $(DEP_CPP_MSGDO) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

DEP_CPP_MSGDO=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	".\msglog.h"\
	".\msgview.h"\
	".\msgdoc.h"\
	

"$(INTDIR)/msgdoc2.obj" : $(SOURCE) $(DEP_CPP_MSGDO) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\mainfrm.cpp

!IF  "$(CFG)" == "Win32 (80x86) Release"

DEP_CPP_MAINF=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	".\mainfrm.h"\
	

"$(INTDIR)/mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

DEP_CPP_MAINF=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	".\mainfrm.h"\
	

"$(INTDIR)/mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\sbmsgs.cpp

!IF  "$(CFG)" == "Win32 (80x86) Release"

DEP_CPP_SBMSG=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	

"$(INTDIR)/sbmsgs.obj" : $(SOURCE) $(DEP_CPP_SBMSG) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

DEP_CPP_SBMSG=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	

"$(INTDIR)/sbmsgs.obj" : $(SOURCE) $(DEP_CPP_SBMSG) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\msgcrack.cpp

!IF  "$(CFG)" == "Win32 (80x86) Release"

DEP_CPP_MSGCR=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	

"$(INTDIR)/msgcrack.obj" : $(SOURCE) $(DEP_CPP_MSGCR) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

DEP_CPP_MSGCR=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	

"$(INTDIR)/msgcrack.obj" : $(SOURCE) $(DEP_CPP_MSGCR) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\thdnode.cpp

!IF  "$(CFG)" == "Win32 (80x86) Release"

DEP_CPP_THDNO=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	".\treectl.h"\
	".\imgwell.h"\
	".\spytreec.h"\
	".\thdnode.h"\
	".\wndnode.h"\
	

"$(INTDIR)/thdnode.obj" : $(SOURCE) $(DEP_CPP_THDNO) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

DEP_CPP_THDNO=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	".\treectl.h"\
	".\imgwell.h"\
	".\spytreec.h"\
	".\thdnode.h"\
	".\wndnode.h"\
	

"$(INTDIR)/thdnode.obj" : $(SOURCE) $(DEP_CPP_THDNO) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\prcnode.cpp

!IF  "$(CFG)" == "Win32 (80x86) Release"

DEP_CPP_PRCNO=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	".\treectl.h"\
	".\imgwell.h"\
	".\spytreec.h"\
	".\prcnode.h"\
	".\thdnode.h"\
	

"$(INTDIR)/prcnode.obj" : $(SOURCE) $(DEP_CPP_PRCNO) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

DEP_CPP_PRCNO=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	".\treectl.h"\
	".\imgwell.h"\
	".\spytreec.h"\
	".\prcnode.h"\
	".\thdnode.h"\
	

"$(INTDIR)/prcnode.obj" : $(SOURCE) $(DEP_CPP_PRCNO) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\outfdlg.cpp

!IF  "$(CFG)" == "Win32 (80x86) Release"

DEP_CPP_OUTFD=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	".\msglog.h"\
	".\msgview.h"\
	".\findtool.h"\
	".\msgdoc.h"\
	".\filtrdlg.h"\
	

"$(INTDIR)/outfdlg.obj" : $(SOURCE) $(DEP_CPP_OUTFD) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

DEP_CPP_OUTFD=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	".\msglog.h"\
	".\msgview.h"\
	".\findtool.h"\
	".\msgdoc.h"\
	".\filtrdlg.h"\
	

"$(INTDIR)/outfdlg.obj" : $(SOURCE) $(DEP_CPP_OUTFD) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\msgdoc.cpp

!IF  "$(CFG)" == "Win32 (80x86) Release"

DEP_CPP_MSGDOC=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	".\msglog.h"\
	".\msgview.h"\
	".\findtool.h"\
	".\srchdlgs.h"\
	".\mainfrm.h"\
	".\msgdoc.h"\
	".\filtrdlg.h"\
	

"$(INTDIR)/msgdoc.obj" : $(SOURCE) $(DEP_CPP_MSGDOC) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

DEP_CPP_MSGDOC=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	".\msglog.h"\
	".\msgview.h"\
	".\findtool.h"\
	".\srchdlgs.h"\
	".\mainfrm.h"\
	".\msgdoc.h"\
	".\filtrdlg.h"\
	

"$(INTDIR)/msgdoc.obj" : $(SOURCE) $(DEP_CPP_MSGDOC) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\msgfdlg.cpp

!IF  "$(CFG)" == "Win32 (80x86) Release"

DEP_CPP_MSGFD=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	".\msglog.h"\
	".\msgview.h"\
	".\findtool.h"\
	".\msgdoc.h"\
	".\filtrdlg.h"\
	

"$(INTDIR)/msgfdlg.obj" : $(SOURCE) $(DEP_CPP_MSGFD) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

DEP_CPP_MSGFD=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	".\msglog.h"\
	".\msgview.h"\
	".\findtool.h"\
	".\msgdoc.h"\
	".\filtrdlg.h"\
	

"$(INTDIR)/msgfdlg.obj" : $(SOURCE) $(DEP_CPP_MSGFD) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\cbmsgs.cpp

!IF  "$(CFG)" == "Win32 (80x86) Release"

DEP_CPP_CBMSG=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	

"$(INTDIR)/cbmsgs.obj" : $(SOURCE) $(DEP_CPP_CBMSG) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

DEP_CPP_CBMSG=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	

"$(INTDIR)/cbmsgs.obj" : $(SOURCE) $(DEP_CPP_CBMSG) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\winfdlg.cpp

!IF  "$(CFG)" == "Win32 (80x86) Release"

DEP_CPP_WINFD=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	".\msglog.h"\
	".\msgview.h"\
	".\findtool.h"\
	".\msgdoc.h"\
	".\filtrdlg.h"\
	

"$(INTDIR)/winfdlg.obj" : $(SOURCE) $(DEP_CPP_WINFD) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

DEP_CPP_WINFD=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	".\msglog.h"\
	".\msgview.h"\
	".\findtool.h"\
	".\msgdoc.h"\
	".\filtrdlg.h"\
	

"$(INTDIR)/winfdlg.obj" : $(SOURCE) $(DEP_CPP_WINFD) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\help.cpp

!IF  "$(CFG)" == "Win32 (80x86) Release"

DEP_CPP_HELP_=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	".\helpdirs.h"\
	

"$(INTDIR)/help.obj" : $(SOURCE) $(DEP_CPP_HELP_) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

DEP_CPP_HELP_=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	".\helpdirs.h"\
	

"$(INTDIR)/help.obj" : $(SOURCE) $(DEP_CPP_HELP_) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\filtrdlg.cpp

!IF  "$(CFG)" == "Win32 (80x86) Release"

DEP_CPP_FILTR=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	".\msglog.h"\
	".\msgview.h"\
	".\findtool.h"\
	".\msgdoc.h"\
	".\filtrdlg.h"\
	

"$(INTDIR)/filtrdlg.obj" : $(SOURCE) $(DEP_CPP_FILTR) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

DEP_CPP_FILTR=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	".\msglog.h"\
	".\msgview.h"\
	".\findtool.h"\
	".\msgdoc.h"\
	".\filtrdlg.h"\
	

"$(INTDIR)/filtrdlg.obj" : $(SOURCE) $(DEP_CPP_FILTR) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\msglog.cpp

!IF  "$(CFG)" == "Win32 (80x86) Release"

DEP_CPP_MSGLO=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	".\msglog.h"\
	".\msgview.h"\
	".\msgdoc.h"\
	

"$(INTDIR)/msglog.obj" : $(SOURCE) $(DEP_CPP_MSGLO) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

DEP_CPP_MSGLO=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	".\msglog.h"\
	".\msgview.h"\
	".\msgdoc.h"\
	

"$(INTDIR)/msglog.obj" : $(SOURCE) $(DEP_CPP_MSGLO) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\wndptabs.cpp

!IF  "$(CFG)" == "Win32 (80x86) Release"

DEP_CPP_WNDPT=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	".\hotlinkc.h"\
	".\proptab.h"\
	".\wndptabs.h"\
	

"$(INTDIR)/wndptabs.obj" : $(SOURCE) $(DEP_CPP_WNDPT) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

DEP_CPP_WNDPT=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	".\hotlinkc.h"\
	".\proptab.h"\
	".\wndptabs.h"\
	

"$(INTDIR)/wndptabs.obj" : $(SOURCE) $(DEP_CPP_WNDPT) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\msgptabs.cpp

!IF  "$(CFG)" == "Win32 (80x86) Release"

DEP_CPP_MSGPT=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	".\hotlinkc.h"\
	".\proptab.h"\
	".\msglog.h"\
	".\msgview.h"\
	".\msgdoc.h"\
	".\msgptabs.h"\
	

"$(INTDIR)/msgptabs.obj" : $(SOURCE) $(DEP_CPP_MSGPT) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

DEP_CPP_MSGPT=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	".\hotlinkc.h"\
	".\proptab.h"\
	".\msglog.h"\
	".\msgview.h"\
	".\msgdoc.h"\
	".\msgptabs.h"\
	

"$(INTDIR)/msgptabs.obj" : $(SOURCE) $(DEP_CPP_MSGPT) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\mdimsgs.cpp

!IF  "$(CFG)" == "Win32 (80x86) Release"

DEP_CPP_MDIMS=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	

"$(INTDIR)/mdimsgs.obj" : $(SOURCE) $(DEP_CPP_MDIMS) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

DEP_CPP_MDIMS=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	

"$(INTDIR)/mdimsgs.obj" : $(SOURCE) $(DEP_CPP_MDIMS) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\winmsgs.cpp

!IF  "$(CFG)" == "Win32 (80x86) Release"

DEP_CPP_WINMS=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	

"$(INTDIR)/winmsgs.obj" : $(SOURCE) $(DEP_CPP_WINMS) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

DEP_CPP_WINMS=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	

"$(INTDIR)/winmsgs.obj" : $(SOURCE) $(DEP_CPP_WINMS) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\editmsgs.cpp

!IF  "$(CFG)" == "Win32 (80x86) Release"

DEP_CPP_EDITM=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	

"$(INTDIR)/editmsgs.obj" : $(SOURCE) $(DEP_CPP_EDITM) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

DEP_CPP_EDITM=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	

"$(INTDIR)/editmsgs.obj" : $(SOURCE) $(DEP_CPP_EDITM) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\msghook.cpp

!IF  "$(CFG)" == "Win32 (80x86) Release"

DEP_CPP_MSGHO=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	".\msglog.h"\
	".\msgview.h"\
	".\msgdoc.h"\
	

"$(INTDIR)/msghook.obj" : $(SOURCE) $(DEP_CPP_MSGHO) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

DEP_CPP_MSGHO=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	".\msglog.h"\
	".\msgview.h"\
	".\msgdoc.h"\
	

"$(INTDIR)/msghook.obj" : $(SOURCE) $(DEP_CPP_MSGHO) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\thdtreed.cpp

!IF  "$(CFG)" == "Win32 (80x86) Release"

DEP_CPP_THDTR=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	".\spytreed.h"\
	".\treectl.h"\
	".\imgwell.h"\
	".\spytreec.h"\
	".\thdnode.h"\
	".\findtool.h"\
	".\spytreev.h"\
	".\srchdlgs.h"\
	".\thdtreed.h"\
	

"$(INTDIR)/thdtreed.obj" : $(SOURCE) $(DEP_CPP_THDTR) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

DEP_CPP_THDTR=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	".\spytreed.h"\
	".\treectl.h"\
	".\imgwell.h"\
	".\spytreec.h"\
	".\thdnode.h"\
	".\findtool.h"\
	".\spytreev.h"\
	".\srchdlgs.h"\
	".\thdtreed.h"\
	

"$(INTDIR)/thdtreed.obj" : $(SOURCE) $(DEP_CPP_THDTR) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\spytreev.cpp

!IF  "$(CFG)" == "Win32 (80x86) Release"

DEP_CPP_SPYTR=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	".\spytreed.h"\
	".\treectl.h"\
	".\imgwell.h"\
	".\spytreec.h"\
	".\spytreev.h"\
	".\fontdlg.h"\
	

"$(INTDIR)/spytreev.obj" : $(SOURCE) $(DEP_CPP_SPYTR) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

DEP_CPP_SPYTR=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	".\spytreed.h"\
	".\treectl.h"\
	".\imgwell.h"\
	".\spytreec.h"\
	".\spytreev.h"\
	".\fontdlg.h"\
	

"$(INTDIR)/spytreev.obj" : $(SOURCE) $(DEP_CPP_SPYTR) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\propinsp.cpp

!IF  "$(CFG)" == "Win32 (80x86) Release"

DEP_CPP_PROPI=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	".\hotlinkc.h"\
	".\proptab.h"\
	".\thdptabs.h"\
	".\msglog.h"\
	".\msgview.h"\
	".\msgdoc.h"\
	".\wndptabs.h"\
	".\msgptabs.h"\
	".\prcptabs.h"\
	

"$(INTDIR)/propinsp.obj" : $(SOURCE) $(DEP_CPP_PROPI) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

DEP_CPP_PROPI=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	".\hotlinkc.h"\
	".\proptab.h"\
	".\thdptabs.h"\
	".\msglog.h"\
	".\msgview.h"\
	".\msgdoc.h"\
	".\wndptabs.h"\
	".\msgptabs.h"\
	".\prcptabs.h"\
	

"$(INTDIR)/propinsp.obj" : $(SOURCE) $(DEP_CPP_PROPI) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\mousmsgs.cpp

!IF  "$(CFG)" == "Win32 (80x86) Release"

DEP_CPP_MOUSM=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	".\msglog.h"\
	".\msgview.h"\
	".\msgdoc.h"\
	

"$(INTDIR)/mousmsgs.obj" : $(SOURCE) $(DEP_CPP_MOUSM) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

DEP_CPP_MOUSM=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	".\msglog.h"\
	".\msgview.h"\
	".\msgdoc.h"\
	

"$(INTDIR)/mousmsgs.obj" : $(SOURCE) $(DEP_CPP_MOUSM) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\findtool.cpp

!IF  "$(CFG)" == "Win32 (80x86) Release"

DEP_CPP_FINDT=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	".\msglog.h"\
	".\msgview.h"\
	".\findtool.h"\
	".\srchdlgs.h"\
	".\msgdoc.h"\
	".\filtrdlg.h"\
	".\findwdlg.h"\
	

"$(INTDIR)/findtool.obj" : $(SOURCE) $(DEP_CPP_FINDT) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

DEP_CPP_FINDT=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	".\msglog.h"\
	".\msgview.h"\
	".\findtool.h"\
	".\srchdlgs.h"\
	".\msgdoc.h"\
	".\filtrdlg.h"\
	".\findwdlg.h"\
	

"$(INTDIR)/findtool.obj" : $(SOURCE) $(DEP_CPP_FINDT) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\procdb.cpp

!IF  "$(CFG)" == "Win32 (80x86) Release"

DEP_CPP_PROCD=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	".\prftilib.h"\
	

"$(INTDIR)/procdb.obj" : $(SOURCE) $(DEP_CPP_PROCD) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

DEP_CPP_PROCD=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	".\prftilib.h"\
	

"$(INTDIR)/procdb.obj" : $(SOURCE) $(DEP_CPP_PROCD) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\spytreed.cpp

!IF  "$(CFG)" == "Win32 (80x86) Release"

DEP_CPP_SPYTRE=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	".\spytreed.h"\
	".\treectl.h"\
	".\imgwell.h"\
	".\spytreec.h"\
	".\spytreev.h"\
	

"$(INTDIR)/spytreed.obj" : $(SOURCE) $(DEP_CPP_SPYTRE) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

DEP_CPP_SPYTRE=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	".\spytreed.h"\
	".\treectl.h"\
	".\imgwell.h"\
	".\spytreec.h"\
	".\spytreev.h"\
	

"$(INTDIR)/spytreed.obj" : $(SOURCE) $(DEP_CPP_SPYTRE) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\wndnode.cpp

!IF  "$(CFG)" == "Win32 (80x86) Release"

DEP_CPP_WNDNO=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	".\treectl.h"\
	".\imgwell.h"\
	".\spytreec.h"\
	".\wndnode.h"\
	

"$(INTDIR)/wndnode.obj" : $(SOURCE) $(DEP_CPP_WNDNO) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

DEP_CPP_WNDNO=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	".\treectl.h"\
	".\imgwell.h"\
	".\spytreec.h"\
	".\wndnode.h"\
	

"$(INTDIR)/wndnode.obj" : $(SOURCE) $(DEP_CPP_WNDNO) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\spytreec.cpp

!IF  "$(CFG)" == "Win32 (80x86) Release"

DEP_CPP_SPYTREE=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	".\treectl.h"\
	".\imgwell.h"\
	".\spytreec.h"\
	

"$(INTDIR)/spytreec.obj" : $(SOURCE) $(DEP_CPP_SPYTREE) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

DEP_CPP_SPYTREE=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	".\treectl.h"\
	".\imgwell.h"\
	".\spytreec.h"\
	

"$(INTDIR)/spytreec.obj" : $(SOURCE) $(DEP_CPP_SPYTREE) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\lbmsgs.cpp

!IF  "$(CFG)" == "Win32 (80x86) Release"

DEP_CPP_LBMSG=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	

"$(INTDIR)/lbmsgs.obj" : $(SOURCE) $(DEP_CPP_LBMSG) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

DEP_CPP_LBMSG=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	

"$(INTDIR)/lbmsgs.obj" : $(SOURCE) $(DEP_CPP_LBMSG) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\proptab.cpp

!IF  "$(CFG)" == "Win32 (80x86) Release"

DEP_CPP_PROPT=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	".\hotlinkc.h"\
	".\proptab.h"\
	

"$(INTDIR)/proptab.obj" : $(SOURCE) $(DEP_CPP_PROPT) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

DEP_CPP_PROPT=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	".\hotlinkc.h"\
	".\proptab.h"\
	

"$(INTDIR)/proptab.obj" : $(SOURCE) $(DEP_CPP_PROPT) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\treectl.cpp

!IF  "$(CFG)" == "Win32 (80x86) Release"

DEP_CPP_TREEC=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	".\treectl.h"\
	".\imgwell.h"\
	

"$(INTDIR)/treectl.obj" : $(SOURCE) $(DEP_CPP_TREEC) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

DEP_CPP_TREEC=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	".\treectl.h"\
	".\imgwell.h"\
	

"$(INTDIR)/treectl.obj" : $(SOURCE) $(DEP_CPP_TREEC) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\tabdlg.cpp

!IF  "$(CFG)" == "Win32 (80x86) Release"

DEP_CPP_TABDL=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	

"$(INTDIR)/tabdlg.obj" : $(SOURCE) $(DEP_CPP_TABDL) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

DEP_CPP_TABDL=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	

"$(INTDIR)/tabdlg.obj" : $(SOURCE) $(DEP_CPP_TABDL) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\helpdirs.cpp

!IF  "$(CFG)" == "Win32 (80x86) Release"

DEP_CPP_HELPD=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	".\helpdirs.h"\
	

"$(INTDIR)/helpdirs.obj" : $(SOURCE) $(DEP_CPP_HELPD) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

DEP_CPP_HELPD=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	".\helpdirs.h"\
	

"$(INTDIR)/helpdirs.obj" : $(SOURCE) $(DEP_CPP_HELPD) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\wndtreed.cpp

!IF  "$(CFG)" == "Win32 (80x86) Release"

DEP_CPP_WNDTR=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	".\spytreed.h"\
	".\treectl.h"\
	".\imgwell.h"\
	".\spytreec.h"\
	".\findtool.h"\
	".\spytreev.h"\
	".\srchdlgs.h"\
	".\wndnode.h"\
	".\wndtreed.h"\
	

"$(INTDIR)/wndtreed.obj" : $(SOURCE) $(DEP_CPP_WNDTR) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

DEP_CPP_WNDTR=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	".\spytreed.h"\
	".\treectl.h"\
	".\imgwell.h"\
	".\spytreec.h"\
	".\findtool.h"\
	".\spytreev.h"\
	".\srchdlgs.h"\
	".\wndnode.h"\
	".\wndtreed.h"\
	

"$(INTDIR)/wndtreed.obj" : $(SOURCE) $(DEP_CPP_WNDTR) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\prcptabs.cpp

!IF  "$(CFG)" == "Win32 (80x86) Release"

DEP_CPP_PRCPT=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	".\hotlinkc.h"\
	".\proptab.h"\
	".\prcptabs.h"\
	

"$(INTDIR)/prcptabs.obj" : $(SOURCE) $(DEP_CPP_PRCPT) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

DEP_CPP_PRCPT=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	".\hotlinkc.h"\
	".\proptab.h"\
	".\prcptabs.h"\
	

"$(INTDIR)/prcptabs.obj" : $(SOURCE) $(DEP_CPP_PRCPT) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\hotlinkc.cpp

!IF  "$(CFG)" == "Win32 (80x86) Release"

DEP_CPP_HOTLI=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	".\hotlinkc.h"\
	

"$(INTDIR)/hotlinkc.obj" : $(SOURCE) $(DEP_CPP_HOTLI) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

DEP_CPP_HOTLI=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	".\hotlinkc.h"\
	

"$(INTDIR)/hotlinkc.obj" : $(SOURCE) $(DEP_CPP_HOTLI) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\property.cpp

!IF  "$(CFG)" == "Win32 (80x86) Release"

DEP_CPP_PROPE=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	".\msglog.h"\
	".\msgview.h"\
	".\msgdoc.h"\
	

"$(INTDIR)/property.obj" : $(SOURCE) $(DEP_CPP_PROPE) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

DEP_CPP_PROPE=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	".\msglog.h"\
	".\msgview.h"\
	".\msgdoc.h"\
	

"$(INTDIR)/property.obj" : $(SOURCE) $(DEP_CPP_PROPE) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\fontdlg.cpp

!IF  "$(CFG)" == "Win32 (80x86) Release"

DEP_CPP_FONTD=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	".\fontdlg.h"\
	

"$(INTDIR)/fontdlg.obj" : $(SOURCE) $(DEP_CPP_FONTD) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

DEP_CPP_FONTD=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	".\fontdlg.h"\
	

"$(INTDIR)/fontdlg.obj" : $(SOURCE) $(DEP_CPP_FONTD) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\msgview.cpp

!IF  "$(CFG)" == "Win32 (80x86) Release"

DEP_CPP_MSGVI=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	".\msglog.h"\
	".\msgview.h"\
	".\msgdoc.h"\
	".\fontdlg.h"\
	

"$(INTDIR)/msgview.obj" : $(SOURCE) $(DEP_CPP_MSGVI) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

DEP_CPP_MSGVI=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	".\msglog.h"\
	".\msgview.h"\
	".\msgdoc.h"\
	".\fontdlg.h"\
	

"$(INTDIR)/msgview.obj" : $(SOURCE) $(DEP_CPP_MSGVI) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\imgwell.cpp

!IF  "$(CFG)" == "Win32 (80x86) Release"

DEP_CPP_IMGWE=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	".\imgwell.h"\
	

"$(INTDIR)/imgwell.obj" : $(SOURCE) $(DEP_CPP_IMGWE) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

DEP_CPP_IMGWE=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	".\imgwell.h"\
	

"$(INTDIR)/imgwell.obj" : $(SOURCE) $(DEP_CPP_IMGWE) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\clipmsgs.cpp

!IF  "$(CFG)" == "Win32 (80x86) Release"

DEP_CPP_CLIPM=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	

"$(INTDIR)/clipmsgs.obj" : $(SOURCE) $(DEP_CPP_CLIPM) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

DEP_CPP_CLIPM=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	

"$(INTDIR)/clipmsgs.obj" : $(SOURCE) $(DEP_CPP_CLIPM) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\ncmsgs.cpp

!IF  "$(CFG)" == "Win32 (80x86) Release"

DEP_CPP_NCMSG=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	

"$(INTDIR)/ncmsgs.obj" : $(SOURCE) $(DEP_CPP_NCMSG) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

DEP_CPP_NCMSG=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	

"$(INTDIR)/ncmsgs.obj" : $(SOURCE) $(DEP_CPP_NCMSG) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\prftxtdb.cpp

!IF  "$(CFG)" == "Win32 (80x86) Release"

DEP_CPP_PRFTX=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	".\prftxtdb.h"\
	

"$(INTDIR)/prftxtdb.obj" : $(SOURCE) $(DEP_CPP_PRFTX) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

DEP_CPP_PRFTX=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	".\prftxtdb.h"\
	

"$(INTDIR)/prftxtdb.obj" : $(SOURCE) $(DEP_CPP_PRFTX) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\findwdlg.cpp

!IF  "$(CFG)" == "Win32 (80x86) Release"

DEP_CPP_FINDW=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	".\findtool.h"\
	".\findwdlg.h"\
	

"$(INTDIR)/findwdlg.obj" : $(SOURCE) $(DEP_CPP_FINDW) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

DEP_CPP_FINDW=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	".\findtool.h"\
	".\findwdlg.h"\
	

"$(INTDIR)/findwdlg.obj" : $(SOURCE) $(DEP_CPP_FINDW) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\prfdb.cpp

!IF  "$(CFG)" == "Win32 (80x86) Release"

DEP_CPP_PRFDB=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	

"$(INTDIR)/prfdb.obj" : $(SOURCE) $(DEP_CPP_PRFDB) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

DEP_CPP_PRFDB=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	

"$(INTDIR)/prfdb.obj" : $(SOURCE) $(DEP_CPP_PRFDB) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\aboutdlg.cpp

!IF  "$(CFG)" == "Win32 (80x86) Release"

DEP_CPP_ABOUT=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	".\aboutdlg.h"\
	".\version.h"\
	

"$(INTDIR)/aboutdlg.obj" : $(SOURCE) $(DEP_CPP_ABOUT) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

DEP_CPP_ABOUT=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	".\aboutdlg.h"\
	".\version.h"\
	

"$(INTDIR)/aboutdlg.obj" : $(SOURCE) $(DEP_CPP_ABOUT) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\srchdlgs.cpp

!IF  "$(CFG)" == "Win32 (80x86) Release"

DEP_CPP_SRCHD=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	".\msglog.h"\
	".\msgview.h"\
	".\spytreed.h"\
	".\prctreed.h"\
	".\treectl.h"\
	".\imgwell.h"\
	".\spytreec.h"\
	".\findtool.h"\
	".\srchdlgs.h"\
	".\msgdoc.h"\
	".\wndnode.h"\
	".\thdtreed.h"\
	".\wndtreed.h"\
	

"$(INTDIR)/srchdlgs.obj" : $(SOURCE) $(DEP_CPP_SRCHD) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

DEP_CPP_SRCHD=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	".\msglog.h"\
	".\msgview.h"\
	".\spytreed.h"\
	".\prctreed.h"\
	".\treectl.h"\
	".\imgwell.h"\
	".\spytreec.h"\
	".\findtool.h"\
	".\srchdlgs.h"\
	".\msgdoc.h"\
	".\wndnode.h"\
	".\thdtreed.h"\
	".\wndtreed.h"\
	

"$(INTDIR)/srchdlgs.obj" : $(SOURCE) $(DEP_CPP_SRCHD) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\spyxxpch.cpp

!IF  "$(CFG)" == "Win32 (80x86) Release"

DEP_CPP_SPYXX=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	
# ADD CPP /Yc"stdafx.h"

"$(INTDIR)/spyxxpch.obj" : $(SOURCE) $(DEP_CPP_SPYXX) "$(INTDIR)"
   $(CPP) /nologo /MD /W3 /Gi- /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_X86_" /D "DISABLE_WIN95_MESSAGES" /FR"$(INTDIR)/"\
 /Fp"$(INTDIR)/spyxx.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/" /c $(SOURCE)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

DEP_CPP_SPYXX=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	
# ADD CPP /Yc"stdafx.h"

"$(INTDIR)/spyxxpch.obj" : $(SOURCE) $(DEP_CPP_SPYXX) "$(INTDIR)"
   $(CPP) /nologo /MD /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_X86_" /D "DISABLE_WIN95_MESSAGES" /FR"$(INTDIR)/"\
 /Fp"$(INTDIR)/spyxx.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/" /Fd"$(OUTDIR)/spyxx.pdb"\
 /Bt /c $(SOURCE)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\btnmsgs.cpp

!IF  "$(CFG)" == "Win32 (80x86) Release"

DEP_CPP_BTNMS=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	

"$(INTDIR)/btnmsgs.obj" : $(SOURCE) $(DEP_CPP_BTNMS) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

DEP_CPP_BTNMS=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	

"$(INTDIR)/btnmsgs.obj" : $(SOURCE) $(DEP_CPP_BTNMS) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\imemsgs.cpp

!IF  "$(CFG)" == "Win32 (80x86) Release"

DEP_CPP_IMEMS=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	

"$(INTDIR)/imemsgs.obj" : $(SOURCE) $(DEP_CPP_IMEMS) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

DEP_CPP_IMEMS=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	

"$(INTDIR)/imemsgs.obj" : $(SOURCE) $(DEP_CPP_IMEMS) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\ddemsgs.cpp

!IF  "$(CFG)" == "Win32 (80x86) Release"

DEP_CPP_DDEMS=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	

"$(INTDIR)/ddemsgs.obj" : $(SOURCE) $(DEP_CPP_DDEMS) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

DEP_CPP_DDEMS=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	

"$(INTDIR)/ddemsgs.obj" : $(SOURCE) $(DEP_CPP_DDEMS) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\statmsgs.cpp

!IF  "$(CFG)" == "Win32 (80x86) Release"

DEP_CPP_STATM=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	

"$(INTDIR)/statmsgs.obj" : $(SOURCE) $(DEP_CPP_STATM) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

DEP_CPP_STATM=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	

"$(INTDIR)/statmsgs.obj" : $(SOURCE) $(DEP_CPP_STATM) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\dlgmsgs.cpp

!IF  "$(CFG)" == "Win32 (80x86) Release"

DEP_CPP_DLGMS=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	

"$(INTDIR)/dlgmsgs.obj" : $(SOURCE) $(DEP_CPP_DLGMS) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

DEP_CPP_DLGMS=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	

"$(INTDIR)/dlgmsgs.obj" : $(SOURCE) $(DEP_CPP_DLGMS) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\spyxx.rc

!IF  "$(CFG)" == "Win32 (80x86) Release"

DEP_RSC_SPYXX_=\
	".\res\spyxx.ico"\
	".\res\message.ico"\
	".\res\prctree.ico"\
	".\res\wndtree.ico"\
	".\res\thdtree.ico"\
	".\res\findtl.ico"\
	".\res\findtl2.ico"\
	".\res\spyxx2.ico"\
	".\res\toolbar.bmp"\
	".\res\expnodes.bmp"\
	".\res\folders.bmp"\
	".\res\treectl.bmp"\
	".\res\scroll.bmp"\
	".\res\scroll_l.bmp"\
	".\res\scroll_r.bmp"\
	".\res\scrollrd.bmp"\
	".\res\scrollld.bmp"\
	".\res\spyxxbig.bmp"\
	".\res\findtl.cur"\
	".\res\jumphand.cur"\
	".\res\spy.rc2"\
	

"$(INTDIR)/spyxx.res" : $(SOURCE) $(DEP_RSC_SPYXX_) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

DEP_RSC_SPYXX_=\
	".\res\spyxx.ico"\
	".\res\message.ico"\
	".\res\prctree.ico"\
	".\res\wndtree.ico"\
	".\res\thdtree.ico"\
	".\res\findtl.ico"\
	".\res\findtl2.ico"\
	".\res\spyxx2.ico"\
	".\res\toolbar.bmp"\
	".\res\expnodes.bmp"\
	".\res\folders.bmp"\
	".\res\treectl.bmp"\
	".\res\scroll.bmp"\
	".\res\scroll_l.bmp"\
	".\res\scroll_r.bmp"\
	".\res\scrollrd.bmp"\
	".\res\scrollld.bmp"\
	".\res\spyxxbig.bmp"\
	".\res\findtl.cur"\
	".\res\jumphand.cur"\
	".\res\spy.rc2"\
	

"$(INTDIR)/spyxx.res" : $(SOURCE) $(DEP_RSC_SPYXX_) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\spyxx.cpp

!IF  "$(CFG)" == "Win32 (80x86) Release"

DEP_CPP_SPYXX_C=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	".\hotlinkc.h"\
	".\msglog.h"\
	".\msgview.h"\
	".\spytreed.h"\
	".\prctreed.h"\
	".\treectl.h"\
	".\imgwell.h"\
	".\spytreec.h"\
	".\findtool.h"\
	".\spytreev.h"\
	".\mainfrm.h"\
	".\msgdoc.h"\
	".\wndnode.h"\
	".\thdtreed.h"\
	".\findwdlg.h"\
	".\wndtreed.h"\
	".\aboutdlg.h"\
	".\spyxxmdi.h"\
	

"$(INTDIR)/spyxx.obj" : $(SOURCE) $(DEP_CPP_SPYXX_C) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

DEP_CPP_SPYXX_C=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	".\hotlinkc.h"\
	".\msglog.h"\
	".\msgview.h"\
	".\spytreed.h"\
	".\prctreed.h"\
	".\treectl.h"\
	".\imgwell.h"\
	".\spytreec.h"\
	".\findtool.h"\
	".\spytreev.h"\
	".\mainfrm.h"\
	".\msgdoc.h"\
	".\wndnode.h"\
	".\thdtreed.h"\
	".\findwdlg.h"\
	".\wndtreed.h"\
	".\aboutdlg.h"\
	".\spyxxmdi.h"\
	

"$(INTDIR)/spyxx.obj" : $(SOURCE) $(DEP_CPP_SPYXX_C) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\animsgs.cpp

!IF  "$(CFG)" == "Win32 (80x86) Release"

DEP_CPP_ANIMS=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	

"$(INTDIR)/animsgs.obj" : $(SOURCE) $(DEP_CPP_ANIMS) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

DEP_CPP_ANIMS=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	

"$(INTDIR)/animsgs.obj" : $(SOURCE) $(DEP_CPP_ANIMS) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\updnmsgs.cpp

!IF  "$(CFG)" == "Win32 (80x86) Release"

DEP_CPP_UPDNM=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	

"$(INTDIR)/updnmsgs.obj" : $(SOURCE) $(DEP_CPP_UPDNM) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

DEP_CPP_UPDNM=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	

"$(INTDIR)/updnmsgs.obj" : $(SOURCE) $(DEP_CPP_UPDNM) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\trkmsgs.cpp

!IF  "$(CFG)" == "Win32 (80x86) Release"

DEP_CPP_TRKMS=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	

"$(INTDIR)/trkmsgs.obj" : $(SOURCE) $(DEP_CPP_TRKMS) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

DEP_CPP_TRKMS=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	

"$(INTDIR)/trkmsgs.obj" : $(SOURCE) $(DEP_CPP_TRKMS) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\tbmsgs.cpp

!IF  "$(CFG)" == "Win32 (80x86) Release"

DEP_CPP_TBMSG=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	

"$(INTDIR)/tbmsgs.obj" : $(SOURCE) $(DEP_CPP_TBMSG) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

DEP_CPP_TBMSG=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	

"$(INTDIR)/tbmsgs.obj" : $(SOURCE) $(DEP_CPP_TBMSG) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\lvmsgs.cpp

!IF  "$(CFG)" == "Win32 (80x86) Release"

DEP_CPP_LVMSG=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	

"$(INTDIR)/lvmsgs.obj" : $(SOURCE) $(DEP_CPP_LVMSG) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

DEP_CPP_LVMSG=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	

"$(INTDIR)/lvmsgs.obj" : $(SOURCE) $(DEP_CPP_LVMSG) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\stbmsgs.cpp

!IF  "$(CFG)" == "Win32 (80x86) Release"

DEP_CPP_STBMS=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	

"$(INTDIR)/stbmsgs.obj" : $(SOURCE) $(DEP_CPP_STBMS) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

DEP_CPP_STBMS=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	

"$(INTDIR)/stbmsgs.obj" : $(SOURCE) $(DEP_CPP_STBMS) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\hdrmsgs.cpp

!IF  "$(CFG)" == "Win32 (80x86) Release"

DEP_CPP_HDRMS=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	

"$(INTDIR)/hdrmsgs.obj" : $(SOURCE) $(DEP_CPP_HDRMS) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

DEP_CPP_HDRMS=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	

"$(INTDIR)/hdrmsgs.obj" : $(SOURCE) $(DEP_CPP_HDRMS) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\tvmsgs.cpp

!IF  "$(CFG)" == "Win32 (80x86) Release"

DEP_CPP_TVMSG=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	

"$(INTDIR)/tvmsgs.obj" : $(SOURCE) $(DEP_CPP_TVMSG) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

DEP_CPP_TVMSG=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	

"$(INTDIR)/tvmsgs.obj" : $(SOURCE) $(DEP_CPP_TVMSG) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\progmsgs.cpp

!IF  "$(CFG)" == "Win32 (80x86) Release"

DEP_CPP_PROGM=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	

"$(INTDIR)/progmsgs.obj" : $(SOURCE) $(DEP_CPP_PROGM) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

DEP_CPP_PROGM=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	

"$(INTDIR)/progmsgs.obj" : $(SOURCE) $(DEP_CPP_PROGM) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\ttmsgs.cpp

!IF  "$(CFG)" == "Win32 (80x86) Release"

DEP_CPP_TTMSG=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	

"$(INTDIR)/ttmsgs.obj" : $(SOURCE) $(DEP_CPP_TTMSG) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

DEP_CPP_TTMSG=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	

"$(INTDIR)/ttmsgs.obj" : $(SOURCE) $(DEP_CPP_TTMSG) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\tabmsgs.cpp

!IF  "$(CFG)" == "Win32 (80x86) Release"

DEP_CPP_TABMS=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	

"$(INTDIR)/tabmsgs.obj" : $(SOURCE) $(DEP_CPP_TABMS) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

DEP_CPP_TABMS=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	

"$(INTDIR)/tabmsgs.obj" : $(SOURCE) $(DEP_CPP_TABMS) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\hkmsgs.cpp

!IF  "$(CFG)" == "Win32 (80x86) Release"

DEP_CPP_HKMSG=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	

"$(INTDIR)/hkmsgs.obj" : $(SOURCE) $(DEP_CPP_HKMSG) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

DEP_CPP_HKMSG=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	

"$(INTDIR)/hkmsgs.obj" : $(SOURCE) $(DEP_CPP_HKMSG) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\spyxxmdi.cpp

!IF  "$(CFG)" == "Win32 (80x86) Release"

DEP_CPP_SPYXXM=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	".\spyxxmdi.h"\
	

"$(INTDIR)/spyxxmdi.obj" : $(SOURCE) $(DEP_CPP_SPYXXM) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ELSEIF  "$(CFG)" == "Win32 (80x86) Debug"

DEP_CPP_SPYXXM=\
	".\stdafx.h"\
	".\tlhelp32.h"\
	".\resource.h"\
	".\spyxxhk.h"\
	".\ime32.h"\
	".\mstream.h"\
	".\msgcrack.h"\
	".\tabdlg.h"\
	".\prfdb.h"\
	".\procdb.h"\
	".\property.h"\
	".\propinsp.h"\
	".\spyxx.h"\
	".\spyxxmdi.h"\
	

"$(INTDIR)/spyxxmdi.obj" : $(SOURCE) $(DEP_CPP_SPYXXM) "$(INTDIR)"\
 "$(INTDIR)/spyxxpch.obj"

!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
