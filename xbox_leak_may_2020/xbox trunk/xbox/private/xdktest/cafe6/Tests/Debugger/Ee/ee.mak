# Microsoft Developer Studio Generated NMAKE File, Format Version 4.10
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) External Target" 0x0106
# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

!IF "$(CFG)" == ""
CFG=mix - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to mix - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "ee - Win32 Release" && "$(CFG)" != "ee - Win32 Debug" &&\
 "$(CFG)" != "int - Win32 Release" && "$(CFG)" != "int - Win32 Debug" &&\
 "$(CFG)" != "misc - Win32 Release" && "$(CFG)" != "misc - Win32 Debug" &&\
 "$(CFG)" != "mix - Win32 Release" && "$(CFG)" != "mix - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "ee.mak" CFG="mix - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ee - Win32 Release" (based on "Win32 (x86) External Target")
!MESSAGE "ee - Win32 Debug" (based on "Win32 (x86) External Target")
!MESSAGE "int - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "int - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "misc - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "misc - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "mix - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "mix - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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
# PROP Target_Last_Scanned "ee - Win32 Debug"

!IF  "$(CFG)" == "ee - Win32 Release"

# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP BASE Cmd_Line "NMAKE /f ee.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "ee.exe"
# PROP BASE Bsc_Name "ee.bsc"
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# PROP Cmd_Line ""
# PROP Rebuild_Opt ""
# PROP Bsc_Name ""
OUTDIR=.\Release
INTDIR=.\Release

ALL : "mix - Win32 Release" "misc - Win32 Release" "int - Win32 Release" 

CLEAN : 
	-@erase 

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

!ELSEIF  "$(CFG)" == "ee - Win32 Debug"

# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP BASE Cmd_Line "NMAKE /f ee.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "ee.exe"
# PROP BASE Bsc_Name "ee.bsc"
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# PROP Cmd_Line ""
# PROP Rebuild_Opt ""
# PROP Bsc_Name ""
OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "mix - Win32 Debug" "misc - Win32 Debug" "int - Win32 Debug" 

CLEAN : 
	-@erase 

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

!ELSEIF  "$(CFG)" == "int - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "int\Release"
# PROP BASE Intermediate_Dir "int\Release"
# PROP BASE Target_Dir "int"
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "int\Release"
# PROP Intermediate_Dir "int\Release"
# PROP Target_Dir "int"
OUTDIR=.\int\Release
INTDIR=.\int\Release

ALL : ".\bin\int.dll"

CLEAN : 
	-@erase "$(INTDIR)\cleanup.obj"
	-@erase "$(INTDIR)\dbgset.obj"
	-@erase "$(INTDIR)\i10cases.obj"
	-@erase "$(INTDIR)\i11cases.obj"
	-@erase "$(INTDIR)\i12cases.obj"
	-@erase "$(INTDIR)\i13cases.obj"
	-@erase "$(INTDIR)\i1cases.obj"
	-@erase "$(INTDIR)\i2cases.obj"
	-@erase "$(INTDIR)\i3cases.obj"
	-@erase "$(INTDIR)\i4cases.obj"
	-@erase "$(INTDIR)\i5cases.obj"
	-@erase "$(INTDIR)\i6cases.obj"
	-@erase "$(INTDIR)\i7cases.obj"
	-@erase "$(INTDIR)\i8cases.obj"
	-@erase "$(INTDIR)\i9cases.obj"
	-@erase "$(INTDIR)\int.pch"
	-@erase "$(INTDIR)\intsub.obj"
	-@erase "$(INTDIR)\stdafx.obj"
	-@erase "$(OUTDIR)\int.exp"
	-@erase "$(OUTDIR)\int.lib"
	-@erase ".\bin\int.dll"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_WINDLL" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/int.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\int\Release/
CPP_SBRS=.\.

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

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/int.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:$(PROCESSOR_ARCHITECTURE)
# ADD LINK32 /nologo /subsystem:windows /dll /machine:$(PROCESSOR_ARCHITECTURE) /out:"bin/int.dll"
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/int.pdb" /machine:$(PROCESSOR_ARCHITECTURE) /out:"bin/int.dll"\
 /implib:"$(OUTDIR)/int.lib" 
LINK32_OBJS= \
	"$(INTDIR)\cleanup.obj" \
	"$(INTDIR)\dbgset.obj" \
	"$(INTDIR)\i10cases.obj" \
	"$(INTDIR)\i11cases.obj" \
	"$(INTDIR)\i12cases.obj" \
	"$(INTDIR)\i13cases.obj" \
	"$(INTDIR)\i1cases.obj" \
	"$(INTDIR)\i2cases.obj" \
	"$(INTDIR)\i3cases.obj" \
	"$(INTDIR)\i4cases.obj" \
	"$(INTDIR)\i5cases.obj" \
	"$(INTDIR)\i6cases.obj" \
	"$(INTDIR)\i7cases.obj" \
	"$(INTDIR)\i8cases.obj" \
	"$(INTDIR)\i9cases.obj" \
	"$(INTDIR)\intsub.obj" \
	"$(INTDIR)\stdafx.obj"

".\bin\int.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "int - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "int\int___Wi"
# PROP BASE Intermediate_Dir "int\int___Wi"
# PROP BASE Target_Dir "int"
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "int\Debug"
# PROP Intermediate_Dir "int\Debug"
# PROP Target_Dir "int"
OUTDIR=.\int\Debug
INTDIR=.\int\Debug

ALL : ".\bin\intd.dll"

CLEAN : 
	-@erase "$(INTDIR)\cleanup.obj"
	-@erase "$(INTDIR)\dbgset.obj"
	-@erase "$(INTDIR)\i10cases.obj"
	-@erase "$(INTDIR)\i11cases.obj"
	-@erase "$(INTDIR)\i12cases.obj"
	-@erase "$(INTDIR)\i13cases.obj"
	-@erase "$(INTDIR)\i1cases.obj"
	-@erase "$(INTDIR)\i2cases.obj"
	-@erase "$(INTDIR)\i3cases.obj"
	-@erase "$(INTDIR)\i4cases.obj"
	-@erase "$(INTDIR)\i5cases.obj"
	-@erase "$(INTDIR)\i6cases.obj"
	-@erase "$(INTDIR)\i7cases.obj"
	-@erase "$(INTDIR)\i8cases.obj"
	-@erase "$(INTDIR)\i9cases.obj"
	-@erase "$(INTDIR)\int.pch"
	-@erase "$(INTDIR)\intsub.obj"
	-@erase "$(INTDIR)\stdafx.obj"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\intd.exp"
	-@erase "$(OUTDIR)\intd.lib"
	-@erase "$(OUTDIR)\intd.pdb"
	-@erase ".\bin\intd.dll"
	-@erase ".\bin\intd.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/int.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\int\Debug/
CPP_SBRS=.\.

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

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/int.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:$(PROCESSOR_ARCHITECTURE)
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:$(PROCESSOR_ARCHITECTURE) /out:"bin/intd.dll"
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/intd.pdb" /debug /machine:$(PROCESSOR_ARCHITECTURE) /out:"bin/intd.dll"\
 /implib:"$(OUTDIR)/intd.lib" 
LINK32_OBJS= \
	"$(INTDIR)\cleanup.obj" \
	"$(INTDIR)\dbgset.obj" \
	"$(INTDIR)\i10cases.obj" \
	"$(INTDIR)\i11cases.obj" \
	"$(INTDIR)\i12cases.obj" \
	"$(INTDIR)\i13cases.obj" \
	"$(INTDIR)\i1cases.obj" \
	"$(INTDIR)\i2cases.obj" \
	"$(INTDIR)\i3cases.obj" \
	"$(INTDIR)\i4cases.obj" \
	"$(INTDIR)\i5cases.obj" \
	"$(INTDIR)\i6cases.obj" \
	"$(INTDIR)\i7cases.obj" \
	"$(INTDIR)\i8cases.obj" \
	"$(INTDIR)\i9cases.obj" \
	"$(INTDIR)\intsub.obj" \
	"$(INTDIR)\stdafx.obj"

".\bin\intd.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "misc - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "misc\Release"
# PROP BASE Intermediate_Dir "misc\Release"
# PROP BASE Target_Dir "misc"
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "misc\Release"
# PROP Intermediate_Dir "misc\Release"
# PROP Target_Dir "misc"
OUTDIR=.\misc\Release
INTDIR=.\misc\Release

ALL : ".\bin\misc.dll"

CLEAN : 
	-@erase "$(INTDIR)\aicases.obj"
	-@erase "$(INTDIR)\asncases.obj"
	-@erase "$(INTDIR)\baecases.obj"
	-@erase "$(INTDIR)\bancases.obj"
	-@erase "$(INTDIR)\cleanup.obj"
	-@erase "$(INTDIR)\cmpcases.obj"
	-@erase "$(INTDIR)\cstcases.obj"
	-@erase "$(INTDIR)\dbgset.obj"
	-@erase "$(INTDIR)\divcases.obj"
	-@erase "$(INTDIR)\eqcases.obj"
	-@erase "$(INTDIR)\gecases.obj"
	-@erase "$(INTDIR)\gtcases.obj"
	-@erase "$(INTDIR)\lecases.obj"
	-@erase "$(INTDIR)\lsecases.obj"
	-@erase "$(INTDIR)\lshcases.obj"
	-@erase "$(INTDIR)\ltcases.obj"
	-@erase "$(INTDIR)\mincases.obj"
	-@erase "$(INTDIR)\misc.pch"
	-@erase "$(INTDIR)\miscsub.obj"
	-@erase "$(INTDIR)\neqcases.obj"
	-@erase "$(INTDIR)\notcases.obj"
	-@erase "$(INTDIR)\oelcases.obj"
	-@erase "$(INTDIR)\oeqcases.obj"
	-@erase "$(INTDIR)\orcases.obj"
	-@erase "$(INTDIR)\pdccases.obj"
	-@erase "$(INTDIR)\piccases.obj"
	-@erase "$(INTDIR)\plucases.obj"
	-@erase "$(INTDIR)\qstcases.obj"
	-@erase "$(INTDIR)\remcases.obj"
	-@erase "$(INTDIR)\rmecases.obj"
	-@erase "$(INTDIR)\rsecases.obj"
	-@erase "$(INTDIR)\rshcases.obj"
	-@erase "$(INTDIR)\stdafx.obj"
	-@erase "$(INTDIR)\topcases.obj"
	-@erase "$(INTDIR)\umncases.obj"
	-@erase "$(INTDIR)\xorcases.obj"
	-@erase "$(INTDIR)\xrecases.obj"
	-@erase "$(OUTDIR)\misc.exp"
	-@erase "$(OUTDIR)\misc.lib"
	-@erase ".\bin\misc.dll"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_WINDLL" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/misc.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\misc\Release/
CPP_SBRS=.\.

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

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/misc.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:$(PROCESSOR_ARCHITECTURE)
# ADD LINK32 /nologo /subsystem:windows /dll /machine:$(PROCESSOR_ARCHITECTURE) /out:"bin/misc.dll"
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/misc.pdb" /machine:$(PROCESSOR_ARCHITECTURE) /out:"bin/misc.dll"\
 /implib:"$(OUTDIR)/misc.lib" 
LINK32_OBJS= \
	"$(INTDIR)\aicases.obj" \
	"$(INTDIR)\asncases.obj" \
	"$(INTDIR)\baecases.obj" \
	"$(INTDIR)\bancases.obj" \
	"$(INTDIR)\cleanup.obj" \
	"$(INTDIR)\cmpcases.obj" \
	"$(INTDIR)\cstcases.obj" \
	"$(INTDIR)\dbgset.obj" \
	"$(INTDIR)\divcases.obj" \
	"$(INTDIR)\eqcases.obj" \
	"$(INTDIR)\gecases.obj" \
	"$(INTDIR)\gtcases.obj" \
	"$(INTDIR)\lecases.obj" \
	"$(INTDIR)\lsecases.obj" \
	"$(INTDIR)\lshcases.obj" \
	"$(INTDIR)\ltcases.obj" \
	"$(INTDIR)\mincases.obj" \
	"$(INTDIR)\miscsub.obj" \
	"$(INTDIR)\neqcases.obj" \
	"$(INTDIR)\notcases.obj" \
	"$(INTDIR)\oelcases.obj" \
	"$(INTDIR)\oeqcases.obj" \
	"$(INTDIR)\orcases.obj" \
	"$(INTDIR)\pdccases.obj" \
	"$(INTDIR)\piccases.obj" \
	"$(INTDIR)\plucases.obj" \
	"$(INTDIR)\qstcases.obj" \
	"$(INTDIR)\remcases.obj" \
	"$(INTDIR)\rmecases.obj" \
	"$(INTDIR)\rsecases.obj" \
	"$(INTDIR)\rshcases.obj" \
	"$(INTDIR)\stdafx.obj" \
	"$(INTDIR)\topcases.obj" \
	"$(INTDIR)\umncases.obj" \
	"$(INTDIR)\xorcases.obj" \
	"$(INTDIR)\xrecases.obj"

".\bin\misc.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "misc - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "misc\misc___W"
# PROP BASE Intermediate_Dir "misc\misc___W"
# PROP BASE Target_Dir "misc"
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "misc\Debug"
# PROP Intermediate_Dir "misc\Debug"
# PROP Target_Dir "misc"
OUTDIR=.\misc\Debug
INTDIR=.\misc\Debug

ALL : ".\bin\miscd.dll"

CLEAN : 
	-@erase "$(INTDIR)\aicases.obj"
	-@erase "$(INTDIR)\asncases.obj"
	-@erase "$(INTDIR)\baecases.obj"
	-@erase "$(INTDIR)\bancases.obj"
	-@erase "$(INTDIR)\cleanup.obj"
	-@erase "$(INTDIR)\cmpcases.obj"
	-@erase "$(INTDIR)\cstcases.obj"
	-@erase "$(INTDIR)\dbgset.obj"
	-@erase "$(INTDIR)\divcases.obj"
	-@erase "$(INTDIR)\eqcases.obj"
	-@erase "$(INTDIR)\gecases.obj"
	-@erase "$(INTDIR)\gtcases.obj"
	-@erase "$(INTDIR)\lecases.obj"
	-@erase "$(INTDIR)\lsecases.obj"
	-@erase "$(INTDIR)\lshcases.obj"
	-@erase "$(INTDIR)\ltcases.obj"
	-@erase "$(INTDIR)\mincases.obj"
	-@erase "$(INTDIR)\misc.pch"
	-@erase "$(INTDIR)\miscsub.obj"
	-@erase "$(INTDIR)\neqcases.obj"
	-@erase "$(INTDIR)\notcases.obj"
	-@erase "$(INTDIR)\oelcases.obj"
	-@erase "$(INTDIR)\oeqcases.obj"
	-@erase "$(INTDIR)\orcases.obj"
	-@erase "$(INTDIR)\pdccases.obj"
	-@erase "$(INTDIR)\piccases.obj"
	-@erase "$(INTDIR)\plucases.obj"
	-@erase "$(INTDIR)\qstcases.obj"
	-@erase "$(INTDIR)\remcases.obj"
	-@erase "$(INTDIR)\rmecases.obj"
	-@erase "$(INTDIR)\rsecases.obj"
	-@erase "$(INTDIR)\rshcases.obj"
	-@erase "$(INTDIR)\stdafx.obj"
	-@erase "$(INTDIR)\topcases.obj"
	-@erase "$(INTDIR)\umncases.obj"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(INTDIR)\xorcases.obj"
	-@erase "$(INTDIR)\xrecases.obj"
	-@erase "$(OUTDIR)\miscd.exp"
	-@erase "$(OUTDIR)\miscd.lib"
	-@erase "$(OUTDIR)\miscd.pdb"
	-@erase ".\bin\miscd.dll"
	-@erase ".\bin\miscd.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/misc.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\misc\Debug/
CPP_SBRS=.\.

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

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/misc.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:$(PROCESSOR_ARCHITECTURE)
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:$(PROCESSOR_ARCHITECTURE) /out:"bin/miscd.dll"
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/miscd.pdb" /debug /machine:$(PROCESSOR_ARCHITECTURE) /out:"bin/miscd.dll"\
 /implib:"$(OUTDIR)/miscd.lib" 
LINK32_OBJS= \
	"$(INTDIR)\aicases.obj" \
	"$(INTDIR)\asncases.obj" \
	"$(INTDIR)\baecases.obj" \
	"$(INTDIR)\bancases.obj" \
	"$(INTDIR)\cleanup.obj" \
	"$(INTDIR)\cmpcases.obj" \
	"$(INTDIR)\cstcases.obj" \
	"$(INTDIR)\dbgset.obj" \
	"$(INTDIR)\divcases.obj" \
	"$(INTDIR)\eqcases.obj" \
	"$(INTDIR)\gecases.obj" \
	"$(INTDIR)\gtcases.obj" \
	"$(INTDIR)\lecases.obj" \
	"$(INTDIR)\lsecases.obj" \
	"$(INTDIR)\lshcases.obj" \
	"$(INTDIR)\ltcases.obj" \
	"$(INTDIR)\mincases.obj" \
	"$(INTDIR)\miscsub.obj" \
	"$(INTDIR)\neqcases.obj" \
	"$(INTDIR)\notcases.obj" \
	"$(INTDIR)\oelcases.obj" \
	"$(INTDIR)\oeqcases.obj" \
	"$(INTDIR)\orcases.obj" \
	"$(INTDIR)\pdccases.obj" \
	"$(INTDIR)\piccases.obj" \
	"$(INTDIR)\plucases.obj" \
	"$(INTDIR)\qstcases.obj" \
	"$(INTDIR)\remcases.obj" \
	"$(INTDIR)\rmecases.obj" \
	"$(INTDIR)\rsecases.obj" \
	"$(INTDIR)\rshcases.obj" \
	"$(INTDIR)\stdafx.obj" \
	"$(INTDIR)\topcases.obj" \
	"$(INTDIR)\umncases.obj" \
	"$(INTDIR)\xorcases.obj" \
	"$(INTDIR)\xrecases.obj"

".\bin\miscd.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "mix - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "mix\Release"
# PROP BASE Intermediate_Dir "mix\Release"
# PROP BASE Target_Dir "mix"
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "mix\Release"
# PROP Intermediate_Dir "mix\Release"
# PROP Target_Dir "mix"
OUTDIR=.\mix\Release
INTDIR=.\mix\Release

ALL : ".\bin\mix.dll"

CLEAN : 
	-@erase "$(INTDIR)\cleanup.obj"
	-@erase "$(INTDIR)\dbgset.obj"
	-@erase "$(INTDIR)\m10cases.obj"
	-@erase "$(INTDIR)\m11cases.obj"
	-@erase "$(INTDIR)\m12cases.obj"
	-@erase "$(INTDIR)\m13cases.obj"
	-@erase "$(INTDIR)\m1cases.obj"
	-@erase "$(INTDIR)\m2cases.obj"
	-@erase "$(INTDIR)\m3cases.obj"
	-@erase "$(INTDIR)\m4cases.obj"
	-@erase "$(INTDIR)\m5cases.obj"
	-@erase "$(INTDIR)\m6cases.obj"
	-@erase "$(INTDIR)\m7cases.obj"
	-@erase "$(INTDIR)\m8cases.obj"
	-@erase "$(INTDIR)\m9cases.obj"
	-@erase "$(INTDIR)\mix.pch"
	-@erase "$(INTDIR)\mixsub.obj"
	-@erase "$(INTDIR)\stdafx.obj"
	-@erase "$(OUTDIR)\mix.exp"
	-@erase "$(OUTDIR)\mix.lib"
	-@erase ".\bin\mix.dll"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_WINDLL" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/mix.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\mix\Release/
CPP_SBRS=.\.

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

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/mix.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:$(PROCESSOR_ARCHITECTURE)
# ADD LINK32 /nologo /subsystem:windows /dll /machine:$(PROCESSOR_ARCHITECTURE) /out:"bin/mix.dll"
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/mix.pdb" /machine:$(PROCESSOR_ARCHITECTURE) /out:"bin/mix.dll"\
 /implib:"$(OUTDIR)/mix.lib" 
LINK32_OBJS= \
	"$(INTDIR)\cleanup.obj" \
	"$(INTDIR)\dbgset.obj" \
	"$(INTDIR)\m10cases.obj" \
	"$(INTDIR)\m11cases.obj" \
	"$(INTDIR)\m12cases.obj" \
	"$(INTDIR)\m13cases.obj" \
	"$(INTDIR)\m1cases.obj" \
	"$(INTDIR)\m2cases.obj" \
	"$(INTDIR)\m3cases.obj" \
	"$(INTDIR)\m4cases.obj" \
	"$(INTDIR)\m5cases.obj" \
	"$(INTDIR)\m6cases.obj" \
	"$(INTDIR)\m7cases.obj" \
	"$(INTDIR)\m8cases.obj" \
	"$(INTDIR)\m9cases.obj" \
	"$(INTDIR)\mixsub.obj" \
	"$(INTDIR)\stdafx.obj"

".\bin\mix.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "mix - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "mix\mix___Wi"
# PROP BASE Intermediate_Dir "mix\mix___Wi"
# PROP BASE Target_Dir "mix"
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "mix\Debug"
# PROP Intermediate_Dir "mix\Debug"
# PROP Target_Dir "mix"
OUTDIR=.\mix\Debug
INTDIR=.\mix\Debug

ALL : ".\bin\mixd.dll"

CLEAN : 
	-@erase "$(INTDIR)\cleanup.obj"
	-@erase "$(INTDIR)\dbgset.obj"
	-@erase "$(INTDIR)\m10cases.obj"
	-@erase "$(INTDIR)\m11cases.obj"
	-@erase "$(INTDIR)\m12cases.obj"
	-@erase "$(INTDIR)\m13cases.obj"
	-@erase "$(INTDIR)\m1cases.obj"
	-@erase "$(INTDIR)\m2cases.obj"
	-@erase "$(INTDIR)\m3cases.obj"
	-@erase "$(INTDIR)\m4cases.obj"
	-@erase "$(INTDIR)\m5cases.obj"
	-@erase "$(INTDIR)\m6cases.obj"
	-@erase "$(INTDIR)\m7cases.obj"
	-@erase "$(INTDIR)\m8cases.obj"
	-@erase "$(INTDIR)\m9cases.obj"
	-@erase "$(INTDIR)\mix.pch"
	-@erase "$(INTDIR)\mixsub.obj"
	-@erase "$(INTDIR)\stdafx.obj"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\mixd.exp"
	-@erase "$(OUTDIR)\mixd.lib"
	-@erase "$(OUTDIR)\mixd.pdb"
	-@erase ".\bin\mixd.dll"
	-@erase ".\bin\mixd.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/mix.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\mix\Debug/
CPP_SBRS=.\.

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

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/mix.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:$(PROCESSOR_ARCHITECTURE)
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:$(PROCESSOR_ARCHITECTURE) /out:"bin/mixd.dll"
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/mixd.pdb" /debug /machine:$(PROCESSOR_ARCHITECTURE) /out:"bin/mixd.dll"\
 /implib:"$(OUTDIR)/mixd.lib" 
LINK32_OBJS= \
	"$(INTDIR)\cleanup.obj" \
	"$(INTDIR)\dbgset.obj" \
	"$(INTDIR)\m10cases.obj" \
	"$(INTDIR)\m11cases.obj" \
	"$(INTDIR)\m12cases.obj" \
	"$(INTDIR)\m13cases.obj" \
	"$(INTDIR)\m1cases.obj" \
	"$(INTDIR)\m2cases.obj" \
	"$(INTDIR)\m3cases.obj" \
	"$(INTDIR)\m4cases.obj" \
	"$(INTDIR)\m5cases.obj" \
	"$(INTDIR)\m6cases.obj" \
	"$(INTDIR)\m7cases.obj" \
	"$(INTDIR)\m8cases.obj" \
	"$(INTDIR)\m9cases.obj" \
	"$(INTDIR)\mixsub.obj" \
	"$(INTDIR)\stdafx.obj"

".\bin\mixd.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

################################################################################
# Begin Target

# Name "ee - Win32 Release"
# Name "ee - Win32 Debug"

!IF  "$(CFG)" == "ee - Win32 Release"

".\ee.exe" : 
   CD C:\snap\ee
   

!ELSEIF  "$(CFG)" == "ee - Win32 Debug"

".\ee.exe" : 
   CD C:\snap\ee
   

!ENDIF 

################################################################################
# Begin Project Dependency

# Project_Dep_Name "int"

!IF  "$(CFG)" == "ee - Win32 Release"

"int - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\ee.mak" CFG="int - Win32 Release" 

!ELSEIF  "$(CFG)" == "ee - Win32 Debug"

"int - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\ee.mak" CFG="int - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "misc"

!IF  "$(CFG)" == "ee - Win32 Release"

"misc - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\ee.mak" CFG="misc - Win32 Release" 

!ELSEIF  "$(CFG)" == "ee - Win32 Debug"

"misc - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\ee.mak" CFG="misc - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "mix"

!IF  "$(CFG)" == "ee - Win32 Release"

"mix - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\ee.mak" CFG="mix - Win32 Release" 

!ELSEIF  "$(CFG)" == "ee - Win32 Debug"

"mix - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\ee.mak" CFG="mix - Win32 Debug" 

!ENDIF 

# End Project Dependency
# End Target
################################################################################
# Begin Target

# Name "int - Win32 Release"
# Name "int - Win32 Debug"

!IF  "$(CFG)" == "int - Win32 Release"

!ELSEIF  "$(CFG)" == "int - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\int\stdafx.cpp
DEP_CPP_STDAF=\
	".\int\stdafx.h"\
	
NODEP_CPP_STDAF=\
	".\int\subsuite.h"\
	".\int\support.h"\
	".\int\test.h"\
	

!IF  "$(CFG)" == "int - Win32 Release"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_WINDLL" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/int.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\int.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "int - Win32 Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/int.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\int.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\int\i11cases.cpp
DEP_CPP_I11CA=\
	".\int\i11cases.h"\
	".\int\intsub.h"\
	".\int\stdafx.h"\
	".\mix\..\..\core\dbgset.h"\
	
NODEP_CPP_I11CA=\
	".\int\subsuite.h"\
	".\int\support.h"\
	".\int\test.h"\
	

"$(INTDIR)\i11cases.obj" : $(SOURCE) $(DEP_CPP_I11CA) "$(INTDIR)"\
 "$(INTDIR)\int.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\int\i12cases.cpp
DEP_CPP_I12CA=\
	".\int\i12cases.h"\
	".\int\intsub.h"\
	".\int\stdafx.h"\
	".\mix\..\..\core\dbgset.h"\
	
NODEP_CPP_I12CA=\
	".\int\subsuite.h"\
	".\int\support.h"\
	".\int\test.h"\
	

"$(INTDIR)\i12cases.obj" : $(SOURCE) $(DEP_CPP_I12CA) "$(INTDIR)"\
 "$(INTDIR)\int.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\int\i13cases.cpp
DEP_CPP_I13CA=\
	".\int\i13cases.h"\
	".\int\intsub.h"\
	".\int\stdafx.h"\
	".\mix\..\..\core\dbgset.h"\
	
NODEP_CPP_I13CA=\
	".\int\subsuite.h"\
	".\int\support.h"\
	".\int\test.h"\
	

"$(INTDIR)\i13cases.obj" : $(SOURCE) $(DEP_CPP_I13CA) "$(INTDIR)"\
 "$(INTDIR)\int.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\int\i1cases.cpp
DEP_CPP_I1CAS=\
	".\int\i1cases.h"\
	".\int\intsub.h"\
	".\int\stdafx.h"\
	".\mix\..\..\core\dbgset.h"\
	
NODEP_CPP_I1CAS=\
	".\int\subsuite.h"\
	".\int\support.h"\
	".\int\test.h"\
	

"$(INTDIR)\i1cases.obj" : $(SOURCE) $(DEP_CPP_I1CAS) "$(INTDIR)"\
 "$(INTDIR)\int.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\int\i2cases.cpp
DEP_CPP_I2CAS=\
	".\int\i2cases.h"\
	".\int\intsub.h"\
	".\int\stdafx.h"\
	".\mix\..\..\core\dbgset.h"\
	
NODEP_CPP_I2CAS=\
	".\int\subsuite.h"\
	".\int\support.h"\
	".\int\test.h"\
	

"$(INTDIR)\i2cases.obj" : $(SOURCE) $(DEP_CPP_I2CAS) "$(INTDIR)"\
 "$(INTDIR)\int.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\int\i3cases.cpp
DEP_CPP_I3CAS=\
	".\int\i3cases.h"\
	".\int\intsub.h"\
	".\int\stdafx.h"\
	".\mix\..\..\core\dbgset.h"\
	
NODEP_CPP_I3CAS=\
	".\int\subsuite.h"\
	".\int\support.h"\
	".\int\test.h"\
	

"$(INTDIR)\i3cases.obj" : $(SOURCE) $(DEP_CPP_I3CAS) "$(INTDIR)"\
 "$(INTDIR)\int.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\int\i4cases.cpp
DEP_CPP_I4CAS=\
	".\int\i4cases.h"\
	".\int\intsub.h"\
	".\int\stdafx.h"\
	".\mix\..\..\core\dbgset.h"\
	
NODEP_CPP_I4CAS=\
	".\int\subsuite.h"\
	".\int\support.h"\
	".\int\test.h"\
	

"$(INTDIR)\i4cases.obj" : $(SOURCE) $(DEP_CPP_I4CAS) "$(INTDIR)"\
 "$(INTDIR)\int.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\int\i5cases.cpp
DEP_CPP_I5CAS=\
	".\int\i5cases.h"\
	".\int\intsub.h"\
	".\int\stdafx.h"\
	".\mix\..\..\core\dbgset.h"\
	
NODEP_CPP_I5CAS=\
	".\int\subsuite.h"\
	".\int\support.h"\
	".\int\test.h"\
	

"$(INTDIR)\i5cases.obj" : $(SOURCE) $(DEP_CPP_I5CAS) "$(INTDIR)"\
 "$(INTDIR)\int.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\int\i6cases.cpp
DEP_CPP_I6CAS=\
	".\int\i6cases.h"\
	".\int\intsub.h"\
	".\int\stdafx.h"\
	".\mix\..\..\core\dbgset.h"\
	
NODEP_CPP_I6CAS=\
	".\int\subsuite.h"\
	".\int\support.h"\
	".\int\test.h"\
	

"$(INTDIR)\i6cases.obj" : $(SOURCE) $(DEP_CPP_I6CAS) "$(INTDIR)"\
 "$(INTDIR)\int.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\int\i7cases.cpp
DEP_CPP_I7CAS=\
	".\int\i7cases.h"\
	".\int\intsub.h"\
	".\int\stdafx.h"\
	".\mix\..\..\core\dbgset.h"\
	
NODEP_CPP_I7CAS=\
	".\int\subsuite.h"\
	".\int\support.h"\
	".\int\test.h"\
	

"$(INTDIR)\i7cases.obj" : $(SOURCE) $(DEP_CPP_I7CAS) "$(INTDIR)"\
 "$(INTDIR)\int.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\int\i8cases.cpp
DEP_CPP_I8CAS=\
	".\int\i8cases.h"\
	".\int\intsub.h"\
	".\int\stdafx.h"\
	".\mix\..\..\core\dbgset.h"\
	
NODEP_CPP_I8CAS=\
	".\int\subsuite.h"\
	".\int\support.h"\
	".\int\test.h"\
	

"$(INTDIR)\i8cases.obj" : $(SOURCE) $(DEP_CPP_I8CAS) "$(INTDIR)"\
 "$(INTDIR)\int.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\int\i9cases.cpp
DEP_CPP_I9CAS=\
	".\int\i9cases.h"\
	".\int\intsub.h"\
	".\int\stdafx.h"\
	".\mix\..\..\core\dbgset.h"\
	
NODEP_CPP_I9CAS=\
	".\int\subsuite.h"\
	".\int\support.h"\
	".\int\test.h"\
	

"$(INTDIR)\i9cases.obj" : $(SOURCE) $(DEP_CPP_I9CAS) "$(INTDIR)"\
 "$(INTDIR)\int.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\int\intsub.cpp
DEP_CPP_INTSU=\
	".\int\i10cases.h"\
	".\int\i11cases.h"\
	".\int\i12cases.h"\
	".\int\i13cases.h"\
	".\int\i1cases.h"\
	".\int\i2cases.h"\
	".\int\i3cases.h"\
	".\int\i4cases.h"\
	".\int\i5cases.h"\
	".\int\i6cases.h"\
	".\int\i7cases.h"\
	".\int\i8cases.h"\
	".\int\i9cases.h"\
	".\int\intsub.h"\
	".\int\stdafx.h"\
	".\mix\..\..\core\cleanup.h"\
	".\mix\..\..\core\dbgset.h"\
	
NODEP_CPP_INTSU=\
	".\int\subsuite.h"\
	".\int\support.h"\
	".\int\test.h"\
	

"$(INTDIR)\intsub.obj" : $(SOURCE) $(DEP_CPP_INTSU) "$(INTDIR)"\
 "$(INTDIR)\int.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\int\i10cases.cpp
DEP_CPP_I10CA=\
	".\int\i10cases.h"\
	".\int\intsub.h"\
	".\int\stdafx.h"\
	".\mix\..\..\core\dbgset.h"\
	
NODEP_CPP_I10CA=\
	".\int\subsuite.h"\
	".\int\support.h"\
	".\int\test.h"\
	

"$(INTDIR)\i10cases.obj" : $(SOURCE) $(DEP_CPP_I10CA) "$(INTDIR)"\
 "$(INTDIR)\int.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=..\core\cleanup.cpp
DEP_CPP_CLEAN=\
	".\mix\..\..\core\cleanup.h"\
	

!IF  "$(CFG)" == "int - Win32 Release"

# SUBTRACT CPP /YX /Yc /Yu

"$(INTDIR)\cleanup.obj" : $(SOURCE) $(DEP_CPP_CLEAN) "$(INTDIR)"
   $(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_WINDLL" /D "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)/" /c $(SOURCE)


!ELSEIF  "$(CFG)" == "int - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

"$(INTDIR)\cleanup.obj" : $(SOURCE) $(DEP_CPP_CLEAN) "$(INTDIR)"
   $(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c\
 $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\core\dbgset.cpp
DEP_CPP_DBGSE=\
	".\guitarg.h"\
	".\guixprt.h"\
	".\log.h"\
	".\mix\..\..\core\dbgset.h"\
	".\platform.h"\
	".\settings.h"\
	".\target.h"\
	".\targxprt.h"\
	".\toolset.h"\
	".\toolxprt.h"\
	
NODEP_CPP_DBGSE=\
	"..\core\stdafx.h"\
	

"$(INTDIR)\dbgset.obj" : $(SOURCE) $(DEP_CPP_DBGSE) "$(INTDIR)"\
 "$(INTDIR)\int.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
# End Target
################################################################################
# Begin Target

# Name "misc - Win32 Release"
# Name "misc - Win32 Debug"

!IF  "$(CFG)" == "misc - Win32 Release"

!ELSEIF  "$(CFG)" == "misc - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\misc\xrecases.cpp
DEP_CPP_XRECA=\
	".\misc\miscsub.h"\
	".\misc\stdafx.h"\
	".\misc\xrecases.h"\
	".\mix\..\..\core\dbgset.h"\
	
NODEP_CPP_XRECA=\
	".\misc\subsuite.h"\
	".\misc\support.h"\
	".\misc\test.h"\
	

"$(INTDIR)\xrecases.obj" : $(SOURCE) $(DEP_CPP_XRECA) "$(INTDIR)"\
 "$(INTDIR)\misc.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\misc\asncases.cpp
DEP_CPP_ASNCA=\
	".\misc\asncases.h"\
	".\misc\miscsub.h"\
	".\misc\stdafx.h"\
	".\mix\..\..\core\dbgset.h"\
	
NODEP_CPP_ASNCA=\
	".\misc\subsuite.h"\
	".\misc\support.h"\
	".\misc\test.h"\
	

"$(INTDIR)\asncases.obj" : $(SOURCE) $(DEP_CPP_ASNCA) "$(INTDIR)"\
 "$(INTDIR)\misc.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\misc\baecases.cpp
DEP_CPP_BAECA=\
	".\misc\baecases.h"\
	".\misc\miscsub.h"\
	".\misc\stdafx.h"\
	".\mix\..\..\core\dbgset.h"\
	
NODEP_CPP_BAECA=\
	".\misc\subsuite.h"\
	".\misc\support.h"\
	".\misc\test.h"\
	

"$(INTDIR)\baecases.obj" : $(SOURCE) $(DEP_CPP_BAECA) "$(INTDIR)"\
 "$(INTDIR)\misc.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\misc\bancases.cpp
DEP_CPP_BANCA=\
	".\misc\bancases.h"\
	".\misc\miscsub.h"\
	".\misc\stdafx.h"\
	".\mix\..\..\core\dbgset.h"\
	
NODEP_CPP_BANCA=\
	".\misc\subsuite.h"\
	".\misc\support.h"\
	".\misc\test.h"\
	

"$(INTDIR)\bancases.obj" : $(SOURCE) $(DEP_CPP_BANCA) "$(INTDIR)"\
 "$(INTDIR)\misc.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\misc\cmpcases.cpp
DEP_CPP_CMPCA=\
	".\misc\cmpcases.h"\
	".\misc\miscsub.h"\
	".\misc\stdafx.h"\
	".\mix\..\..\core\dbgset.h"\
	
NODEP_CPP_CMPCA=\
	".\misc\subsuite.h"\
	".\misc\support.h"\
	".\misc\test.h"\
	

"$(INTDIR)\cmpcases.obj" : $(SOURCE) $(DEP_CPP_CMPCA) "$(INTDIR)"\
 "$(INTDIR)\misc.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\misc\cstcases.cpp
DEP_CPP_CSTCA=\
	".\misc\cstcases.h"\
	".\misc\miscsub.h"\
	".\misc\stdafx.h"\
	".\mix\..\..\core\dbgset.h"\
	
NODEP_CPP_CSTCA=\
	".\misc\subsuite.h"\
	".\misc\support.h"\
	".\misc\test.h"\
	

"$(INTDIR)\cstcases.obj" : $(SOURCE) $(DEP_CPP_CSTCA) "$(INTDIR)"\
 "$(INTDIR)\misc.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\misc\divcases.cpp
DEP_CPP_DIVCA=\
	".\misc\divcases.h"\
	".\misc\miscsub.h"\
	".\misc\stdafx.h"\
	".\mix\..\..\core\dbgset.h"\
	
NODEP_CPP_DIVCA=\
	".\misc\subsuite.h"\
	".\misc\support.h"\
	".\misc\test.h"\
	

"$(INTDIR)\divcases.obj" : $(SOURCE) $(DEP_CPP_DIVCA) "$(INTDIR)"\
 "$(INTDIR)\misc.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\misc\eqcases.cpp
DEP_CPP_EQCAS=\
	".\misc\eqcases.h"\
	".\misc\miscsub.h"\
	".\misc\stdafx.h"\
	".\mix\..\..\core\dbgset.h"\
	
NODEP_CPP_EQCAS=\
	".\misc\subsuite.h"\
	".\misc\support.h"\
	".\misc\test.h"\
	

"$(INTDIR)\eqcases.obj" : $(SOURCE) $(DEP_CPP_EQCAS) "$(INTDIR)"\
 "$(INTDIR)\misc.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\misc\gecases.cpp
DEP_CPP_GECAS=\
	".\misc\gecases.h"\
	".\misc\miscsub.h"\
	".\misc\stdafx.h"\
	".\mix\..\..\core\dbgset.h"\
	
NODEP_CPP_GECAS=\
	".\misc\subsuite.h"\
	".\misc\support.h"\
	".\misc\test.h"\
	

"$(INTDIR)\gecases.obj" : $(SOURCE) $(DEP_CPP_GECAS) "$(INTDIR)"\
 "$(INTDIR)\misc.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\misc\gtcases.cpp
DEP_CPP_GTCAS=\
	".\misc\gtcases.h"\
	".\misc\miscsub.h"\
	".\misc\stdafx.h"\
	".\mix\..\..\core\dbgset.h"\
	
NODEP_CPP_GTCAS=\
	".\misc\subsuite.h"\
	".\misc\support.h"\
	".\misc\test.h"\
	

"$(INTDIR)\gtcases.obj" : $(SOURCE) $(DEP_CPP_GTCAS) "$(INTDIR)"\
 "$(INTDIR)\misc.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\misc\lecases.cpp
DEP_CPP_LECAS=\
	".\misc\lecases.h"\
	".\misc\miscsub.h"\
	".\misc\stdafx.h"\
	".\mix\..\..\core\dbgset.h"\
	
NODEP_CPP_LECAS=\
	".\misc\subsuite.h"\
	".\misc\support.h"\
	".\misc\test.h"\
	

"$(INTDIR)\lecases.obj" : $(SOURCE) $(DEP_CPP_LECAS) "$(INTDIR)"\
 "$(INTDIR)\misc.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\misc\lsecases.cpp
DEP_CPP_LSECA=\
	".\misc\lsecases.h"\
	".\misc\miscsub.h"\
	".\misc\stdafx.h"\
	".\mix\..\..\core\dbgset.h"\
	
NODEP_CPP_LSECA=\
	".\misc\subsuite.h"\
	".\misc\support.h"\
	".\misc\test.h"\
	

"$(INTDIR)\lsecases.obj" : $(SOURCE) $(DEP_CPP_LSECA) "$(INTDIR)"\
 "$(INTDIR)\misc.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\misc\lshcases.cpp
DEP_CPP_LSHCA=\
	".\misc\lshcases.h"\
	".\misc\miscsub.h"\
	".\misc\stdafx.h"\
	".\mix\..\..\core\dbgset.h"\
	
NODEP_CPP_LSHCA=\
	".\misc\subsuite.h"\
	".\misc\support.h"\
	".\misc\test.h"\
	

"$(INTDIR)\lshcases.obj" : $(SOURCE) $(DEP_CPP_LSHCA) "$(INTDIR)"\
 "$(INTDIR)\misc.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\misc\ltcases.cpp
DEP_CPP_LTCAS=\
	".\misc\ltcases.h"\
	".\misc\miscsub.h"\
	".\misc\stdafx.h"\
	".\mix\..\..\core\dbgset.h"\
	
NODEP_CPP_LTCAS=\
	".\misc\subsuite.h"\
	".\misc\support.h"\
	".\misc\test.h"\
	

"$(INTDIR)\ltcases.obj" : $(SOURCE) $(DEP_CPP_LTCAS) "$(INTDIR)"\
 "$(INTDIR)\misc.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\misc\mincases.cpp
DEP_CPP_MINCA=\
	".\misc\mincases.h"\
	".\misc\miscsub.h"\
	".\misc\stdafx.h"\
	".\mix\..\..\core\dbgset.h"\
	
NODEP_CPP_MINCA=\
	".\misc\subsuite.h"\
	".\misc\support.h"\
	".\misc\test.h"\
	

"$(INTDIR)\mincases.obj" : $(SOURCE) $(DEP_CPP_MINCA) "$(INTDIR)"\
 "$(INTDIR)\misc.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\misc\miscsub.cpp
DEP_CPP_MISCS=\
	".\misc\aicases.h"\
	".\misc\asncases.h"\
	".\misc\baecases.h"\
	".\misc\bancases.h"\
	".\misc\cmpcases.h"\
	".\misc\cstcases.h"\
	".\misc\divcases.h"\
	".\misc\eqcases.h"\
	".\misc\gecases.h"\
	".\misc\gtcases.h"\
	".\misc\lecases.h"\
	".\misc\lsecases.h"\
	".\misc\lshcases.h"\
	".\misc\ltcases.h"\
	".\misc\mincases.h"\
	".\misc\miscsub.h"\
	".\misc\neqcases.h"\
	".\misc\notcases.h"\
	".\misc\oelcases.h"\
	".\misc\oeqcases.h"\
	".\misc\orcases.h"\
	".\misc\pdccases.h"\
	".\misc\piccases.h"\
	".\misc\plucases.h"\
	".\misc\qstcases.h"\
	".\misc\remcases.h"\
	".\misc\rmecases.h"\
	".\misc\rsecases.h"\
	".\misc\rshcases.h"\
	".\misc\stdafx.h"\
	".\misc\topcases.h"\
	".\misc\umncases.h"\
	".\misc\xorcases.h"\
	".\misc\xrecases.h"\
	".\mix\..\..\core\cleanup.h"\
	".\mix\..\..\core\dbgset.h"\
	
NODEP_CPP_MISCS=\
	".\misc\subsuite.h"\
	".\misc\support.h"\
	".\misc\test.h"\
	

"$(INTDIR)\miscsub.obj" : $(SOURCE) $(DEP_CPP_MISCS) "$(INTDIR)"\
 "$(INTDIR)\misc.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\misc\neqcases.cpp
DEP_CPP_NEQCA=\
	".\misc\miscsub.h"\
	".\misc\neqcases.h"\
	".\misc\stdafx.h"\
	".\mix\..\..\core\dbgset.h"\
	
NODEP_CPP_NEQCA=\
	".\misc\subsuite.h"\
	".\misc\support.h"\
	".\misc\test.h"\
	

"$(INTDIR)\neqcases.obj" : $(SOURCE) $(DEP_CPP_NEQCA) "$(INTDIR)"\
 "$(INTDIR)\misc.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\misc\notcases.cpp
DEP_CPP_NOTCA=\
	".\misc\miscsub.h"\
	".\misc\notcases.h"\
	".\misc\stdafx.h"\
	".\mix\..\..\core\dbgset.h"\
	
NODEP_CPP_NOTCA=\
	".\misc\subsuite.h"\
	".\misc\support.h"\
	".\misc\test.h"\
	

"$(INTDIR)\notcases.obj" : $(SOURCE) $(DEP_CPP_NOTCA) "$(INTDIR)"\
 "$(INTDIR)\misc.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\misc\oelcases.cpp
DEP_CPP_OELCA=\
	".\misc\miscsub.h"\
	".\misc\oelcases.h"\
	".\misc\stdafx.h"\
	".\mix\..\..\core\dbgset.h"\
	
NODEP_CPP_OELCA=\
	".\misc\subsuite.h"\
	".\misc\support.h"\
	".\misc\test.h"\
	

"$(INTDIR)\oelcases.obj" : $(SOURCE) $(DEP_CPP_OELCA) "$(INTDIR)"\
 "$(INTDIR)\misc.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\misc\oeqcases.cpp
DEP_CPP_OEQCA=\
	".\misc\miscsub.h"\
	".\misc\oeqcases.h"\
	".\misc\stdafx.h"\
	".\mix\..\..\core\dbgset.h"\
	
NODEP_CPP_OEQCA=\
	".\misc\subsuite.h"\
	".\misc\support.h"\
	".\misc\test.h"\
	

"$(INTDIR)\oeqcases.obj" : $(SOURCE) $(DEP_CPP_OEQCA) "$(INTDIR)"\
 "$(INTDIR)\misc.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\misc\orcases.cpp
DEP_CPP_ORCAS=\
	".\misc\miscsub.h"\
	".\misc\orcases.h"\
	".\misc\stdafx.h"\
	".\mix\..\..\core\dbgset.h"\
	
NODEP_CPP_ORCAS=\
	".\misc\subsuite.h"\
	".\misc\support.h"\
	".\misc\test.h"\
	

"$(INTDIR)\orcases.obj" : $(SOURCE) $(DEP_CPP_ORCAS) "$(INTDIR)"\
 "$(INTDIR)\misc.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\misc\pdccases.cpp
DEP_CPP_PDCCA=\
	".\misc\miscsub.h"\
	".\misc\pdccases.h"\
	".\misc\stdafx.h"\
	".\mix\..\..\core\dbgset.h"\
	
NODEP_CPP_PDCCA=\
	".\misc\subsuite.h"\
	".\misc\support.h"\
	".\misc\test.h"\
	

"$(INTDIR)\pdccases.obj" : $(SOURCE) $(DEP_CPP_PDCCA) "$(INTDIR)"\
 "$(INTDIR)\misc.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\misc\piccases.cpp
DEP_CPP_PICCA=\
	".\misc\miscsub.h"\
	".\misc\piccases.h"\
	".\misc\stdafx.h"\
	".\mix\..\..\core\dbgset.h"\
	
NODEP_CPP_PICCA=\
	".\misc\subsuite.h"\
	".\misc\support.h"\
	".\misc\test.h"\
	

"$(INTDIR)\piccases.obj" : $(SOURCE) $(DEP_CPP_PICCA) "$(INTDIR)"\
 "$(INTDIR)\misc.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\misc\plucases.cpp
DEP_CPP_PLUCA=\
	".\misc\miscsub.h"\
	".\misc\plucases.h"\
	".\misc\stdafx.h"\
	".\mix\..\..\core\dbgset.h"\
	
NODEP_CPP_PLUCA=\
	".\misc\subsuite.h"\
	".\misc\support.h"\
	".\misc\test.h"\
	

"$(INTDIR)\plucases.obj" : $(SOURCE) $(DEP_CPP_PLUCA) "$(INTDIR)"\
 "$(INTDIR)\misc.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\misc\qstcases.cpp
DEP_CPP_QSTCA=\
	".\misc\miscsub.h"\
	".\misc\qstcases.h"\
	".\misc\stdafx.h"\
	".\mix\..\..\core\dbgset.h"\
	
NODEP_CPP_QSTCA=\
	".\misc\subsuite.h"\
	".\misc\support.h"\
	".\misc\test.h"\
	

"$(INTDIR)\qstcases.obj" : $(SOURCE) $(DEP_CPP_QSTCA) "$(INTDIR)"\
 "$(INTDIR)\misc.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\misc\remcases.cpp

!IF  "$(CFG)" == "misc - Win32 Release"

DEP_CPP_REMCA=\
	".\misc\miscsub.h"\
	".\misc\remcases.h"\
	".\misc\stdafx.h"\
	".\mix\..\..\core\dbgset.h"\
	
NODEP_CPP_REMCA=\
	".\misc\subsuite.h"\
	".\misc\support.h"\
	".\misc\test.h"\
	

"$(INTDIR)\remcases.obj" : $(SOURCE) $(DEP_CPP_REMCA) "$(INTDIR)"\
 "$(INTDIR)\misc.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "misc - Win32 Debug"

DEP_CPP_REMCA=\
	".\misc\miscsub.h"\
	".\misc\remcases.h"\
	".\misc\stdafx.h"\
	".\mix\..\..\core\dbgset.h"\
	
NODEP_CPP_REMCA=\
	".\misc\subsuite.h"\
	".\misc\support.h"\
	".\misc\test.h"\
	

"$(INTDIR)\remcases.obj" : $(SOURCE) $(DEP_CPP_REMCA) "$(INTDIR)"\
 "$(INTDIR)\misc.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\misc\rmecases.cpp
DEP_CPP_RMECA=\
	".\misc\miscsub.h"\
	".\misc\rmecases.h"\
	".\misc\stdafx.h"\
	".\mix\..\..\core\dbgset.h"\
	
NODEP_CPP_RMECA=\
	".\misc\subsuite.h"\
	".\misc\support.h"\
	".\misc\test.h"\
	

"$(INTDIR)\rmecases.obj" : $(SOURCE) $(DEP_CPP_RMECA) "$(INTDIR)"\
 "$(INTDIR)\misc.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\misc\rsecases.cpp
DEP_CPP_RSECA=\
	".\misc\miscsub.h"\
	".\misc\rsecases.h"\
	".\misc\stdafx.h"\
	".\mix\..\..\core\dbgset.h"\
	
NODEP_CPP_RSECA=\
	".\misc\subsuite.h"\
	".\misc\support.h"\
	".\misc\test.h"\
	

"$(INTDIR)\rsecases.obj" : $(SOURCE) $(DEP_CPP_RSECA) "$(INTDIR)"\
 "$(INTDIR)\misc.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\misc\rshcases.cpp
DEP_CPP_RSHCA=\
	".\misc\miscsub.h"\
	".\misc\rshcases.h"\
	".\misc\stdafx.h"\
	".\mix\..\..\core\dbgset.h"\
	
NODEP_CPP_RSHCA=\
	".\misc\subsuite.h"\
	".\misc\support.h"\
	".\misc\test.h"\
	

"$(INTDIR)\rshcases.obj" : $(SOURCE) $(DEP_CPP_RSHCA) "$(INTDIR)"\
 "$(INTDIR)\misc.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\misc\stdafx.cpp
DEP_CPP_STDAF=\
	".\misc\stdafx.h"\
	
NODEP_CPP_STDAF=\
	".\misc\subsuite.h"\
	".\misc\support.h"\
	".\misc\test.h"\
	

!IF  "$(CFG)" == "misc - Win32 Release"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_WINDLL" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/misc.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\misc.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "misc - Win32 Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/misc.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\misc.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\misc\topcases.cpp
DEP_CPP_TOPCA=\
	".\misc\miscsub.h"\
	".\misc\stdafx.h"\
	".\misc\topcases.h"\
	".\mix\..\..\core\dbgset.h"\
	
NODEP_CPP_TOPCA=\
	".\misc\subsuite.h"\
	".\misc\support.h"\
	".\misc\test.h"\
	

"$(INTDIR)\topcases.obj" : $(SOURCE) $(DEP_CPP_TOPCA) "$(INTDIR)"\
 "$(INTDIR)\misc.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\misc\umncases.cpp
DEP_CPP_UMNCA=\
	".\misc\miscsub.h"\
	".\misc\stdafx.h"\
	".\misc\umncases.h"\
	".\mix\..\..\core\dbgset.h"\
	
NODEP_CPP_UMNCA=\
	".\misc\subsuite.h"\
	".\misc\support.h"\
	".\misc\test.h"\
	

"$(INTDIR)\umncases.obj" : $(SOURCE) $(DEP_CPP_UMNCA) "$(INTDIR)"\
 "$(INTDIR)\misc.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\misc\xorcases.cpp
DEP_CPP_XORCA=\
	".\misc\miscsub.h"\
	".\misc\stdafx.h"\
	".\misc\xorcases.h"\
	".\mix\..\..\core\dbgset.h"\
	
NODEP_CPP_XORCA=\
	".\misc\subsuite.h"\
	".\misc\support.h"\
	".\misc\test.h"\
	

"$(INTDIR)\xorcases.obj" : $(SOURCE) $(DEP_CPP_XORCA) "$(INTDIR)"\
 "$(INTDIR)\misc.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\misc\aicases.cpp
DEP_CPP_AICAS=\
	".\misc\aicases.h"\
	".\misc\miscsub.h"\
	".\misc\stdafx.h"\
	".\mix\..\..\core\dbgset.h"\
	
NODEP_CPP_AICAS=\
	".\misc\subsuite.h"\
	".\misc\support.h"\
	".\misc\test.h"\
	

"$(INTDIR)\aicases.obj" : $(SOURCE) $(DEP_CPP_AICAS) "$(INTDIR)"\
 "$(INTDIR)\misc.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=..\core\dbgset.cpp
DEP_CPP_DBGSE=\
	".\guitarg.h"\
	".\guixprt.h"\
	".\log.h"\
	".\mix\..\..\core\dbgset.h"\
	".\platform.h"\
	".\settings.h"\
	".\target.h"\
	".\targxprt.h"\
	".\toolset.h"\
	".\toolxprt.h"\
	
NODEP_CPP_DBGSE=\
	"..\core\stdafx.h"\
	

"$(INTDIR)\dbgset.obj" : $(SOURCE) $(DEP_CPP_DBGSE) "$(INTDIR)"\
 "$(INTDIR)\misc.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=..\core\cleanup.cpp
DEP_CPP_CLEAN=\
	".\mix\..\..\core\cleanup.h"\
	

!IF  "$(CFG)" == "misc - Win32 Release"

# SUBTRACT CPP /YX /Yc /Yu

"$(INTDIR)\cleanup.obj" : $(SOURCE) $(DEP_CPP_CLEAN) "$(INTDIR)"
   $(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_WINDLL" /D "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)/" /c $(SOURCE)


!ELSEIF  "$(CFG)" == "misc - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

"$(INTDIR)\cleanup.obj" : $(SOURCE) $(DEP_CPP_CLEAN) "$(INTDIR)"
   $(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c\
 $(SOURCE)


!ENDIF 

# End Source File
# End Target
################################################################################
# Begin Target

# Name "mix - Win32 Release"
# Name "mix - Win32 Debug"

!IF  "$(CFG)" == "mix - Win32 Release"

!ELSEIF  "$(CFG)" == "mix - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\mix\stdafx.cpp
DEP_CPP_STDAF=\
	".\mix\stdafx.h"\
	
NODEP_CPP_STDAF=\
	".\mix\subsuite.h"\
	".\mix\support.h"\
	".\mix\test.h"\
	

!IF  "$(CFG)" == "mix - Win32 Release"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_WINDLL" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/mix.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\mix.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "mix - Win32 Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/mix.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\mix.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\mix\m11cases.cpp
DEP_CPP_M11CA=\
	".\mix\..\..\core\dbgset.h"\
	".\mix\m11cases.h"\
	".\mix\mixsub.h"\
	".\mix\stdafx.h"\
	
NODEP_CPP_M11CA=\
	".\mix\subsuite.h"\
	".\mix\support.h"\
	".\mix\test.h"\
	

"$(INTDIR)\m11cases.obj" : $(SOURCE) $(DEP_CPP_M11CA) "$(INTDIR)"\
 "$(INTDIR)\mix.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\mix\m12cases.cpp
DEP_CPP_M12CA=\
	".\mix\..\..\core\dbgset.h"\
	".\mix\m12cases.h"\
	".\mix\mixsub.h"\
	".\mix\stdafx.h"\
	
NODEP_CPP_M12CA=\
	".\mix\subsuite.h"\
	".\mix\support.h"\
	".\mix\test.h"\
	

"$(INTDIR)\m12cases.obj" : $(SOURCE) $(DEP_CPP_M12CA) "$(INTDIR)"\
 "$(INTDIR)\mix.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\mix\m13cases.cpp
DEP_CPP_M13CA=\
	".\mix\..\..\core\dbgset.h"\
	".\mix\m13cases.h"\
	".\mix\mixsub.h"\
	".\mix\stdafx.h"\
	
NODEP_CPP_M13CA=\
	".\mix\subsuite.h"\
	".\mix\support.h"\
	".\mix\test.h"\
	

"$(INTDIR)\m13cases.obj" : $(SOURCE) $(DEP_CPP_M13CA) "$(INTDIR)"\
 "$(INTDIR)\mix.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\mix\m1cases.cpp
DEP_CPP_M1CAS=\
	".\mix\..\..\core\dbgset.h"\
	".\mix\m1cases.h"\
	".\mix\mixsub.h"\
	".\mix\stdafx.h"\
	
NODEP_CPP_M1CAS=\
	".\mix\subsuite.h"\
	".\mix\support.h"\
	".\mix\test.h"\
	

"$(INTDIR)\m1cases.obj" : $(SOURCE) $(DEP_CPP_M1CAS) "$(INTDIR)"\
 "$(INTDIR)\mix.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\mix\m2cases.cpp
DEP_CPP_M2CAS=\
	".\mix\..\..\core\dbgset.h"\
	".\mix\m2cases.h"\
	".\mix\mixsub.h"\
	".\mix\stdafx.h"\
	
NODEP_CPP_M2CAS=\
	".\mix\subsuite.h"\
	".\mix\support.h"\
	".\mix\test.h"\
	

"$(INTDIR)\m2cases.obj" : $(SOURCE) $(DEP_CPP_M2CAS) "$(INTDIR)"\
 "$(INTDIR)\mix.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\mix\m3cases.cpp
DEP_CPP_M3CAS=\
	".\mix\..\..\core\dbgset.h"\
	".\mix\m3cases.h"\
	".\mix\mixsub.h"\
	".\mix\stdafx.h"\
	
NODEP_CPP_M3CAS=\
	".\mix\subsuite.h"\
	".\mix\support.h"\
	".\mix\test.h"\
	

"$(INTDIR)\m3cases.obj" : $(SOURCE) $(DEP_CPP_M3CAS) "$(INTDIR)"\
 "$(INTDIR)\mix.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\mix\m4cases.cpp
DEP_CPP_M4CAS=\
	".\mix\..\..\core\dbgset.h"\
	".\mix\m4cases.h"\
	".\mix\mixsub.h"\
	".\mix\stdafx.h"\
	
NODEP_CPP_M4CAS=\
	".\mix\subsuite.h"\
	".\mix\support.h"\
	".\mix\test.h"\
	

"$(INTDIR)\m4cases.obj" : $(SOURCE) $(DEP_CPP_M4CAS) "$(INTDIR)"\
 "$(INTDIR)\mix.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\mix\m5cases.cpp
DEP_CPP_M5CAS=\
	".\mix\..\..\core\dbgset.h"\
	".\mix\m5cases.h"\
	".\mix\mixsub.h"\
	".\mix\stdafx.h"\
	
NODEP_CPP_M5CAS=\
	".\mix\subsuite.h"\
	".\mix\support.h"\
	".\mix\test.h"\
	

"$(INTDIR)\m5cases.obj" : $(SOURCE) $(DEP_CPP_M5CAS) "$(INTDIR)"\
 "$(INTDIR)\mix.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\mix\m6cases.cpp
DEP_CPP_M6CAS=\
	".\mix\..\..\core\dbgset.h"\
	".\mix\m6cases.h"\
	".\mix\mixsub.h"\
	".\mix\stdafx.h"\
	
NODEP_CPP_M6CAS=\
	".\mix\subsuite.h"\
	".\mix\support.h"\
	".\mix\test.h"\
	

"$(INTDIR)\m6cases.obj" : $(SOURCE) $(DEP_CPP_M6CAS) "$(INTDIR)"\
 "$(INTDIR)\mix.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\mix\m7cases.cpp
DEP_CPP_M7CAS=\
	".\mix\..\..\core\dbgset.h"\
	".\mix\m7cases.h"\
	".\mix\mixsub.h"\
	".\mix\stdafx.h"\
	
NODEP_CPP_M7CAS=\
	".\mix\subsuite.h"\
	".\mix\support.h"\
	".\mix\test.h"\
	

"$(INTDIR)\m7cases.obj" : $(SOURCE) $(DEP_CPP_M7CAS) "$(INTDIR)"\
 "$(INTDIR)\mix.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\mix\m8cases.cpp
DEP_CPP_M8CAS=\
	".\mix\..\..\core\dbgset.h"\
	".\mix\m8cases.h"\
	".\mix\mixsub.h"\
	".\mix\stdafx.h"\
	
NODEP_CPP_M8CAS=\
	".\mix\subsuite.h"\
	".\mix\support.h"\
	".\mix\test.h"\
	

"$(INTDIR)\m8cases.obj" : $(SOURCE) $(DEP_CPP_M8CAS) "$(INTDIR)"\
 "$(INTDIR)\mix.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\mix\m9cases.cpp
DEP_CPP_M9CAS=\
	".\mix\..\..\core\dbgset.h"\
	".\mix\m9cases.h"\
	".\mix\mixsub.h"\
	".\mix\stdafx.h"\
	
NODEP_CPP_M9CAS=\
	".\mix\subsuite.h"\
	".\mix\support.h"\
	".\mix\test.h"\
	

"$(INTDIR)\m9cases.obj" : $(SOURCE) $(DEP_CPP_M9CAS) "$(INTDIR)"\
 "$(INTDIR)\mix.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\mix\mixsub.cpp
DEP_CPP_MIXSU=\
	".\mix\..\..\core\cleanup.h"\
	".\mix\..\..\core\dbgset.h"\
	".\mix\m10cases.h"\
	".\mix\m11cases.h"\
	".\mix\m12cases.h"\
	".\mix\m13cases.h"\
	".\mix\m1cases.h"\
	".\mix\m2cases.h"\
	".\mix\m3cases.h"\
	".\mix\m4cases.h"\
	".\mix\m5cases.h"\
	".\mix\m6cases.h"\
	".\mix\m7cases.h"\
	".\mix\m8cases.h"\
	".\mix\m9cases.h"\
	".\mix\mixsub.h"\
	".\mix\stdafx.h"\
	
NODEP_CPP_MIXSU=\
	".\mix\subsuite.h"\
	".\mix\support.h"\
	".\mix\test.h"\
	

"$(INTDIR)\mixsub.obj" : $(SOURCE) $(DEP_CPP_MIXSU) "$(INTDIR)"\
 "$(INTDIR)\mix.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\mix\m10cases.cpp
DEP_CPP_M10CA=\
	".\mix\..\..\core\dbgset.h"\
	".\mix\m10cases.h"\
	".\mix\mixsub.h"\
	".\mix\stdafx.h"\
	
NODEP_CPP_M10CA=\
	".\mix\subsuite.h"\
	".\mix\support.h"\
	".\mix\test.h"\
	

"$(INTDIR)\m10cases.obj" : $(SOURCE) $(DEP_CPP_M10CA) "$(INTDIR)"\
 "$(INTDIR)\mix.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=..\core\dbgset.cpp
DEP_CPP_DBGSE=\
	".\guitarg.h"\
	".\guixprt.h"\
	".\log.h"\
	".\mix\..\..\core\dbgset.h"\
	".\platform.h"\
	".\settings.h"\
	".\target.h"\
	".\targxprt.h"\
	".\toolset.h"\
	".\toolxprt.h"\
	
NODEP_CPP_DBGSE=\
	"..\core\stdafx.h"\
	

"$(INTDIR)\dbgset.obj" : $(SOURCE) $(DEP_CPP_DBGSE) "$(INTDIR)"\
 "$(INTDIR)\mix.pch"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=..\core\cleanup.cpp
DEP_CPP_CLEAN=\
	".\mix\..\..\core\cleanup.h"\
	

!IF  "$(CFG)" == "mix - Win32 Release"

# SUBTRACT CPP /YX /Yc /Yu

"$(INTDIR)\cleanup.obj" : $(SOURCE) $(DEP_CPP_CLEAN) "$(INTDIR)"
   $(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_WINDLL" /D "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)/" /c $(SOURCE)


!ELSEIF  "$(CFG)" == "mix - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

"$(INTDIR)\cleanup.obj" : $(SOURCE) $(DEP_CPP_CLEAN) "$(INTDIR)"
   $(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c\
 $(SOURCE)


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
