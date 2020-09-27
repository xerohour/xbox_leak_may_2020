###############################################################################
# This makefile builds the resource only stub dll for mfcapwz (mfcapwz.stb).
# This DLL contains all localized templates as resources.
#
# 12/03/95 (WinslowF)
#
!ifndef RELEASE
RELEASE=0
!endif

!ifndef DEBUG
DEBUG=1
!endif

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!if "$(DEBUG)" != "0"
CFG=Win32 Debug
!else
CFG=Win32 Release
!endif

################################################################################
# Begin Project
# PROP Target_Last_Scanned "Win32 Debug"
RSC=rc.exe
CPP=cl.exe
MTL=mktyplib.exe

!IF  "$(CFG)" == "Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
OUTDIR=.\Release
INTDIR=.\Release

ALL : "$(OUTDIR)\mfcapwz.stb"

CLEAN : 
	-@erase ".\Release\mfcapwz.lib"
	-@erase ".\Release\dllmain.obj"
	-@erase ".\Release\script1.res"
	-@erase ".\Release\mfcapwz.exp"
	-@erase "..\bin\mfcapwz.stb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/mfcapwz.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x411 /d "NDEBUG"
# ADD RSC /l 0x411 /d "NDEBUG"
RSC_PROJ=/l 0x411 /fo"$(INTDIR)/script1.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/mfcapwz.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386 /out:"../bin/mfcapwz.stb"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/mfcapwz.pdb" /machine:I386 /out:"../bin/mfcapwz.stb"\
 /implib:"$(OUTDIR)/mfcapwz.lib" 
LINK32_OBJS= \
	".\Release\dllmain.obj" \
	".\Release\script1.res"

"$(OUTDIR)\mfcapwz.stb" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "$(OUTDIR)\mfcapwzd.stb"

CLEAN : 
	-@erase ".\Debug\vc40.pdb"
	-@erase ".\Debug\vc40.idb"
	-@erase "..\bin\mfcapwzd.stb"
	-@erase ".\Debug\dllmain.obj"
	-@erase ".\Debug\script1.res"
	-@erase "..\bin\mfcapwzd.ilk"
	-@erase ".\Debug\mfcapwzd.lib"
	-@erase ".\Debug\mfcapwzd.exp"
	-@erase ".\Debug\mfcapwzd.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/mfcapwz.pch" /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x411 /d "_DEBUG"
# ADD RSC /l 0x411 /d "_DEBUG"
RSC_PROJ=/l 0x411 /fo"$(INTDIR)/script1.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/mfcapwz.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"../bin/mfcapwzd.stb"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/mfcapwzd.pdb" /debug /machine:I386 /out:"../bin/mfcapwzd.stb"\
 /implib:"$(OUTDIR)/mfcapwzd.lib" 
LINK32_OBJS= \
	".\Debug\dllmain.obj" \
	".\Debug\script1.res"

"$(OUTDIR)\mfcapwzd.stb" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

################################################################################
# Begin Target

# Name "Win32 Release"
# Name "Win32 Debug"

!IF  "$(CFG)" == "Win32 Release"

!ELSEIF  "$(CFG)" == "Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\dllmain.cpp

!IF  "$(CFG)" == "Win32 Release"


"$(INTDIR)\dllmain.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Win32 Debug"


"$(INTDIR)\dllmain.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\res\script1.rc
DEP_RSC_SCRIP=\
	".\res\all.rc"\
	".\res\childfrm.cpp"\
	".\res\childfrm.h"\
	".\res\cntritem.cpp"\
	".\res\cntritem.h"\
	".\res\confirm.inf"\
	".\res\dialog.cpp"\
	".\res\dialog.h"\
	".\res\dlgall.rc"\
	".\res\dlgres.h"\
	".\res\dlgroot.clw"\
	".\res\dlgroot.cnt"\
	".\res\dlgroot.cpp"\
	".\res\dlgroot.h"\
	".\res\dllroot.clw"\
	".\res\dllroot.cpp"\
	".\res\dllroot.h"\
	".\res\doc.cpp"\
	".\res\doc.h"\
	".\res\frame.cpp"\
	".\res\frame.h"\
	".\res\ipframe.cpp"\
	".\res\ipframe.h"\
	".\res\macroot.reg"\
	".\res\makehelp.bat"\
	".\res\newproj.inf"\
	".\res\readme.txt"\
	".\res\recset.cpp"\
	".\res\recset.h"\
	".\res\rfx.txt"\
	".\res\root.clw"\
	".\res\root.cpp"\
	".\res\root.def"\
	".\res\root.h"\
	".\res\root.ico"\
	".\res\root.odl"\
	".\res\root.r"\
	".\res\root.rc2"\
	".\res\root.reg"\
	".\res\root.vcw"\
	".\res\srvritem.cpp"\
	".\res\srvritem.h"\
	".\res\stdafx.cpp"\
	".\res\stdafx.h"\
	".\res\view.cpp"\
	".\res\view.h"\
	".\res\resource.h"\
	".\res\bullet.bmp"\
	".\res\curarw2.bmp"\
	".\res\curarw4.bmp"\
	".\res\curhelp.bmp"\
	".\res\editcopy.bmp"\
	".\res\editcut.bmp"\
	".\res\editpast.bmp"\
	".\res\editundo.bmp"\
	".\res\filenew.bmp"\
	".\res\fileopen.bmp"\
	".\res\fileprnt.bmp"\
	".\res\filesave.bmp"\
	".\res\hlptbar.bmp"\
	".\res\maccmd.bmp"\
	".\res\recfirst.bmp"\
	".\res\reclast.bmp"\
	".\res\recnext.bmp"\
	".\res\recprev.bmp"\
	".\res\recvtbar.bmp"\
	".\res\scmax.bmp"\
	".\res\scmin.bmp"\
	".\res\tba__.bmp"\
	".\res\tba_i.bmp"\
	".\res\tbah_.bmp"\
	".\res\tbahi.bmp"\
	".\res\tbd__.bmp"\
	".\res\tbdh_.bmp"\
	".\res\tbea_.bmp"\
	".\res\tbeah.bmp"\
	".\res\tbed_.bmp"\
	".\res\tbedh.bmp"\
	".\res\tbend.bmp"\
	".\res\tber_.bmp"\
	".\res\tberh.bmp"\
	".\res\tbex_.bmp"\
	".\res\tbnd_.bmp"\
	".\res\tbr__.bmp"\
	".\res\tbr_i.bmp"\
	".\res\tbrh_.bmp"\
	".\res\tbrhi.bmp"\
	".\res\treeview.cpp"\
	".\res\treeview.h"\
	".\res\wndview.cpp"\
	".\res\wndview.h"\
	

!IF  "$(CFG)" == "Win32 Release"


"$(INTDIR)\script1.res" : $(SOURCE) $(DEP_RSC_SCRIP) "$(INTDIR)"
   $(RSC) /l 0x411 /fo"$(INTDIR)/script1.res" /i "res" /d "NDEBUG" $(SOURCE)


!ELSEIF  "$(CFG)" == "Win32 Debug"


"$(INTDIR)\script1.res" : $(SOURCE) $(DEP_RSC_SCRIP) "$(INTDIR)"
   $(RSC) /l 0x411 /fo"$(INTDIR)/script1.res" /i "res" /d "_DEBUG" $(SOURCE)


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
