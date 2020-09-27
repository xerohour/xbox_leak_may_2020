# Microsoft Developer Studio Generated NMAKE File, Format Version 4.20
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101
# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

!IF "$(CFG)" == ""
CFG=Imutil32 - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to Imutil32 - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "RawRead - Win32 Release" && "$(CFG)" !=\
 "RawRead - Win32 Debug" && "$(CFG)" != "RawWrite - Win32 Release" && "$(CFG)"\
 != "RawWrite - Win32 Debug" && "$(CFG)" != "Imutil32 - Win32 Release" &&\
 "$(CFG)" != "Imutil32 - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "rawread.mak" CFG="Imutil32 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "RawRead - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "RawRead - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "RawWrite - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "RawWrite - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "Imutil32 - Win32 Release" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Imutil32 - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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
# PROP Target_Last_Scanned "Imutil32 - Win32 Debug"

!IF  "$(CFG)" == "RawRead - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "RawRead\Release"
# PROP Intermediate_Dir "RawRead\Release"
# PROP Target_Dir ""
OUTDIR=.\RawRead\Release
INTDIR=.\RawRead\Release

ALL : "Imutil32 - Win32 Release" "$(OUTDIR)\rawread.exe"

CLEAN : 
	-@erase "$(INTDIR)\bitmap.obj"
	-@erase "$(INTDIR)\bootsec.obj"
	-@erase "$(INTDIR)\compress.obj"
	-@erase "$(INTDIR)\debug.obj"
	-@erase "$(INTDIR)\fatutil.obj"
	-@erase "$(INTDIR)\file.obj"
	-@erase "$(INTDIR)\image.obj"
	-@erase "$(INTDIR)\image.res"
	-@erase "$(INTDIR)\mem.obj"
	-@erase "$(INTDIR)\mrcicode.obj"
	-@erase "$(INTDIR)\rawread.obj"
	-@erase "$(INTDIR)\reg.obj"
	-@erase "$(INTDIR)\text.obj"
	-@erase "$(INTDIR)\winmisc.obj"
	-@erase "$(OUTDIR)\rawread.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D APPTITLE=\"RawRead\" /YX /c
CPP_PROJ=/nologo /ML /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 APPTITLE=\"RawRead\" /Fp"$(INTDIR)/rawread.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\RawRead\Release/
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
# ADD RSC /l 0x409 /d "NDEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/image.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/rawread.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 ..\win32\imutil32\release\imutil32.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=..\win32\imutil32\release\imutil32.lib kernel32.lib user32.lib\
 gdi32.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib /nologo\
 /subsystem:windows /incremental:no /pdb:"$(OUTDIR)/RawRead.pdb" /machine:I386\
 /out:"$(OUTDIR)/RawRead.exe" 
LINK32_OBJS= \
	"$(INTDIR)\bitmap.obj" \
	"$(INTDIR)\bootsec.obj" \
	"$(INTDIR)\compress.obj" \
	"$(INTDIR)\debug.obj" \
	"$(INTDIR)\fatutil.obj" \
	"$(INTDIR)\file.obj" \
	"$(INTDIR)\image.obj" \
	"$(INTDIR)\image.res" \
	"$(INTDIR)\mem.obj" \
	"$(INTDIR)\mrcicode.obj" \
	"$(INTDIR)\rawread.obj" \
	"$(INTDIR)\reg.obj" \
	"$(INTDIR)\text.obj" \
	"$(INTDIR)\winmisc.obj" \
	".\Imutil32\Release\Imutil32.lib"

"$(OUTDIR)\rawread.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "RawRead - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "RawRead_"
# PROP BASE Intermediate_Dir "RawRead_"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "RawRead\Debug"
# PROP Intermediate_Dir "RawRead\Debug"
# PROP Target_Dir ""
OUTDIR=.\RawRead\Debug
INTDIR=.\RawRead\Debug

ALL : "Imutil32 - Win32 Debug" "$(OUTDIR)\rawread.exe"

CLEAN : 
	-@erase "$(INTDIR)\bitmap.obj"
	-@erase "$(INTDIR)\bootsec.obj"
	-@erase "$(INTDIR)\compress.obj"
	-@erase "$(INTDIR)\debug.obj"
	-@erase "$(INTDIR)\fatutil.obj"
	-@erase "$(INTDIR)\file.obj"
	-@erase "$(INTDIR)\image.obj"
	-@erase "$(INTDIR)\image.res"
	-@erase "$(INTDIR)\mem.obj"
	-@erase "$(INTDIR)\mrcicode.obj"
	-@erase "$(INTDIR)\rawread.obj"
	-@erase "$(INTDIR)\reg.obj"
	-@erase "$(INTDIR)\text.obj"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(INTDIR)\winmisc.obj"
	-@erase "$(OUTDIR)\rawread.exe"
	-@erase "$(OUTDIR)\rawread.map"
	-@erase "$(OUTDIR)\rawread.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D APPTITLE=\"RawRead\" /YX /c
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D APPTITLE=\"RawRead\" /Fp"$(INTDIR)/rawread.pch" /YX /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\RawRead\Debug/
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
# ADD RSC /l 0x409 /d "_DEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/image.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/rawread.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 ..\win32\imutil32\debug\imutil32.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib /nologo /subsystem:windows /incremental:no /map /debug /machine:I386
LINK32_FLAGS=..\win32\imutil32\debug\imutil32.lib kernel32.lib user32.lib\
 gdi32.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib /nologo\
 /subsystem:windows /incremental:no /pdb:"$(OUTDIR)/RawRead.pdb"\
 /map:"$(INTDIR)/RawRead.map" /debug /machine:I386 /out:"$(OUTDIR)/RawRead.exe" 
LINK32_OBJS= \
	"$(INTDIR)\bitmap.obj" \
	"$(INTDIR)\bootsec.obj" \
	"$(INTDIR)\compress.obj" \
	"$(INTDIR)\debug.obj" \
	"$(INTDIR)\fatutil.obj" \
	"$(INTDIR)\file.obj" \
	"$(INTDIR)\image.obj" \
	"$(INTDIR)\image.res" \
	"$(INTDIR)\mem.obj" \
	"$(INTDIR)\mrcicode.obj" \
	"$(INTDIR)\rawread.obj" \
	"$(INTDIR)\reg.obj" \
	"$(INTDIR)\text.obj" \
	"$(INTDIR)\winmisc.obj" \
	".\Imutil32\Debug\Imutil32.lib"

"$(OUTDIR)\rawread.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "RawWrite - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "RawWrite\Release"
# PROP BASE Intermediate_Dir "RawWrite\Release"
# PROP BASE Target_Dir "RawWrite"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "RawWrite\Release"
# PROP Intermediate_Dir "RawWrite\Release"
# PROP Target_Dir "RawWrite"
OUTDIR=.\RawWrite\Release
INTDIR=.\RawWrite\Release

ALL : "Imutil32 - Win32 Release" "$(OUTDIR)\RawWrite.exe"

CLEAN : 
	-@erase "$(INTDIR)\bitmap.obj"
	-@erase "$(INTDIR)\bootsec.obj"
	-@erase "$(INTDIR)\compress.obj"
	-@erase "$(INTDIR)\debug.obj"
	-@erase "$(INTDIR)\fatutil.obj"
	-@erase "$(INTDIR)\file.obj"
	-@erase "$(INTDIR)\image.obj"
	-@erase "$(INTDIR)\image.res"
	-@erase "$(INTDIR)\mem.obj"
	-@erase "$(INTDIR)\mrcicode.obj"
	-@erase "$(INTDIR)\rawwrite.obj"
	-@erase "$(INTDIR)\reg.obj"
	-@erase "$(INTDIR)\text.obj"
	-@erase "$(INTDIR)\winmisc.obj"
	-@erase "$(OUTDIR)\RawWrite.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D APPTITLE=\"RawWrite\" /YX /c
CPP_PROJ=/nologo /ML /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 APPTITLE=\"RawWrite\" /Fp"$(INTDIR)/RawWrite.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\RawWrite\Release/
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
# ADD RSC /l 0x409 /d "NDEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/image.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/RawWrite.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 ..\win32\imutil32\release\imutil32.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=..\win32\imutil32\release\imutil32.lib kernel32.lib user32.lib\
 gdi32.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib /nologo\
 /subsystem:windows /incremental:no /pdb:"$(OUTDIR)/RawWrite.pdb" /machine:I386\
 /out:"$(OUTDIR)/RawWrite.exe" 
LINK32_OBJS= \
	"$(INTDIR)\bitmap.obj" \
	"$(INTDIR)\bootsec.obj" \
	"$(INTDIR)\compress.obj" \
	"$(INTDIR)\debug.obj" \
	"$(INTDIR)\fatutil.obj" \
	"$(INTDIR)\file.obj" \
	"$(INTDIR)\image.obj" \
	"$(INTDIR)\image.res" \
	"$(INTDIR)\mem.obj" \
	"$(INTDIR)\mrcicode.obj" \
	"$(INTDIR)\rawwrite.obj" \
	"$(INTDIR)\reg.obj" \
	"$(INTDIR)\text.obj" \
	"$(INTDIR)\winmisc.obj" \
	".\Imutil32\Release\Imutil32.lib"

"$(OUTDIR)\RawWrite.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "RawWrite - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "RawWrite\Debug"
# PROP BASE Intermediate_Dir "RawWrite\Debug"
# PROP BASE Target_Dir "RawWrite"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "RawWrite\Debug"
# PROP Intermediate_Dir "RawWrite\Debug"
# PROP Target_Dir "RawWrite"
OUTDIR=.\RawWrite\Debug
INTDIR=.\RawWrite\Debug

ALL : "Imutil32 - Win32 Debug" "$(OUTDIR)\RawWrite.exe"

CLEAN : 
	-@erase "$(INTDIR)\bitmap.obj"
	-@erase "$(INTDIR)\bootsec.obj"
	-@erase "$(INTDIR)\compress.obj"
	-@erase "$(INTDIR)\debug.obj"
	-@erase "$(INTDIR)\fatutil.obj"
	-@erase "$(INTDIR)\file.obj"
	-@erase "$(INTDIR)\image.obj"
	-@erase "$(INTDIR)\image.res"
	-@erase "$(INTDIR)\mem.obj"
	-@erase "$(INTDIR)\mrcicode.obj"
	-@erase "$(INTDIR)\rawwrite.obj"
	-@erase "$(INTDIR)\reg.obj"
	-@erase "$(INTDIR)\text.obj"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(INTDIR)\winmisc.obj"
	-@erase "$(OUTDIR)\RawWrite.exe"
	-@erase "$(OUTDIR)\RawWrite.ilk"
	-@erase "$(OUTDIR)\RawWrite.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D APPTITLE=\"RawWrite\" /YX /c
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D APPTITLE=\"RawWrite\" /Fp"$(INTDIR)/RawWrite.pch" /YX /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\RawWrite\Debug/
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
# ADD RSC /l 0x409 /d "_DEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/image.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/RawWrite.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 ..\win32\imutil32\debug\imutil32.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib /nologo /subsystem:windows /debug /machine:I386
LINK32_FLAGS=..\win32\imutil32\debug\imutil32.lib kernel32.lib user32.lib\
 gdi32.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib /nologo\
 /subsystem:windows /incremental:yes /pdb:"$(OUTDIR)/RawWrite.pdb" /debug\
 /machine:I386 /out:"$(OUTDIR)/RawWrite.exe" 
LINK32_OBJS= \
	"$(INTDIR)\bitmap.obj" \
	"$(INTDIR)\bootsec.obj" \
	"$(INTDIR)\compress.obj" \
	"$(INTDIR)\debug.obj" \
	"$(INTDIR)\fatutil.obj" \
	"$(INTDIR)\file.obj" \
	"$(INTDIR)\image.obj" \
	"$(INTDIR)\image.res" \
	"$(INTDIR)\mem.obj" \
	"$(INTDIR)\mrcicode.obj" \
	"$(INTDIR)\rawwrite.obj" \
	"$(INTDIR)\reg.obj" \
	"$(INTDIR)\text.obj" \
	"$(INTDIR)\winmisc.obj" \
	".\Imutil32\Debug\Imutil32.lib"

"$(OUTDIR)\RawWrite.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Imutil32 - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Imutil32\Imutil32"
# PROP BASE Intermediate_Dir "Imutil32\Imutil32"
# PROP BASE Target_Dir "Imutil32"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Imutil32\Release"
# PROP Intermediate_Dir "Imutil32\Release"
# PROP Target_Dir "Imutil32"
OUTDIR=.\Imutil32\Release
INTDIR=.\Imutil32\Release

ALL : "$(OUTDIR)\Imutil32.dll"

CLEAN : 
	-@erase "$(INTDIR)\debug.obj"
	-@erase "$(INTDIR)\dllmain.obj"
	-@erase "$(INTDIR)\imutil32.obj"
	-@erase "$(INTDIR)\imutil32.res"
	-@erase "$(INTDIR)\mem.obj"
	-@erase "$(OUTDIR)\Imutil32.dll"
	-@erase "$(OUTDIR)\Imutil32.exp"
	-@erase "$(OUTDIR)\Imutil32.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D APPTITLE=\"IMUTIL32\" /YX /c
CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 APPTITLE=\"IMUTIL32\" /Fp"$(INTDIR)/Imutil32.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Imutil32\Release/
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
# ADD RSC /l 0x409 /d "NDEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/imutil32.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Imutil32.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib thunk32.lib thunk\32\thunk.obj /nologo /subsystem:windows /dll /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib thunk32.lib thunk\32\thunk.obj /nologo\
 /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)/Imutil32.pdb"\
 /machine:I386 /def:".\imutil32\imutil32.def" /out:"$(OUTDIR)/Imutil32.dll"\
 /implib:"$(OUTDIR)/Imutil32.lib" 
DEF_FILE= \
	".\imutil32\imutil32.def"
LINK32_OBJS= \
	"$(INTDIR)\debug.obj" \
	"$(INTDIR)\dllmain.obj" \
	"$(INTDIR)\imutil32.obj" \
	"$(INTDIR)\imutil32.res" \
	"$(INTDIR)\mem.obj"

"$(OUTDIR)\Imutil32.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Imutil32 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Imutil32\Imutil32"
# PROP BASE Intermediate_Dir "Imutil32\Imutil32"
# PROP BASE Target_Dir "Imutil32"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Imutil32\Debug"
# PROP Intermediate_Dir "Imutil32\Debug"
# PROP Target_Dir "Imutil32"
OUTDIR=.\Imutil32\Debug
INTDIR=.\Imutil32\Debug

ALL : "$(OUTDIR)\Imutil32.dll"

CLEAN : 
	-@erase "$(INTDIR)\debug.obj"
	-@erase "$(INTDIR)\dllmain.obj"
	-@erase "$(INTDIR)\imutil32.obj"
	-@erase "$(INTDIR)\imutil32.res"
	-@erase "$(INTDIR)\mem.obj"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\Imutil32.dll"
	-@erase "$(OUTDIR)\Imutil32.exp"
	-@erase "$(OUTDIR)\Imutil32.ilk"
	-@erase "$(OUTDIR)\Imutil32.lib"
	-@erase "$(OUTDIR)\Imutil32.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D APPTITLE=\"IMUTIL32\" /YX /c
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D APPTITLE=\"IMUTIL32\" /Fp"$(INTDIR)/Imutil32.pch" /YX /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Imutil32\Debug/
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
# ADD RSC /l 0x409 /d "_DEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/imutil32.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Imutil32.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib thunk32.lib thunk\32\thunk.obj /nologo /subsystem:windows /dll /debug /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib thunk32.lib thunk\32\thunk.obj /nologo\
 /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)/Imutil32.pdb" /debug\
 /machine:I386 /def:".\imutil32\imutil32.def" /out:"$(OUTDIR)/Imutil32.dll"\
 /implib:"$(OUTDIR)/Imutil32.lib" 
DEF_FILE= \
	".\imutil32\imutil32.def"
LINK32_OBJS= \
	"$(INTDIR)\debug.obj" \
	"$(INTDIR)\dllmain.obj" \
	"$(INTDIR)\imutil32.obj" \
	"$(INTDIR)\imutil32.res" \
	"$(INTDIR)\mem.obj"

"$(OUTDIR)\Imutil32.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

################################################################################
# Begin Target

# Name "RawRead - Win32 Release"
# Name "RawRead - Win32 Debug"

!IF  "$(CFG)" == "RawRead - Win32 Release"

!ELSEIF  "$(CFG)" == "RawRead - Win32 Debug"

!ENDIF 

################################################################################
# Begin Project Dependency

# Project_Dep_Name "Imutil32"

!IF  "$(CFG)" == "RawRead - Win32 Release"

"Imutil32 - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\rawread.mak" CFG="Imutil32 - Win32 Release" 

!ELSEIF  "$(CFG)" == "RawRead - Win32 Debug"

"Imutil32 - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\rawread.mak" CFG="Imutil32 - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Source File

SOURCE=.\winmisc.c
DEP_CPP_WINMI=\
	"..\bitmap.h"\
	"..\bootsec.h"\
	"..\image.h"\
	"..\text.h"\
	".\imutil32\..\..\defs.h"\
	".\imutil32\..\..\diskutil.h"\
	".\imutil32\..\..\mem.h"\
	".\imutil32\..\debug.h"\
	".\winmisc.h"\
	

"$(INTDIR)\winmisc.obj" : $(SOURCE) $(DEP_CPP_WINMI) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\reg.c
DEP_CPP_REG_C=\
	"..\bitmap.h"\
	"..\bootsec.h"\
	"..\image.h"\
	".\imutil32\..\..\defs.h"\
	".\imutil32\..\..\diskutil.h"\
	".\reg.h"\
	".\winmisc.h"\
	

"$(INTDIR)\reg.obj" : $(SOURCE) $(DEP_CPP_REG_C) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\debug.c
DEP_CPP_DEBUG=\
	".\imutil32\..\..\defs.h"\
	".\imutil32\..\debug.h"\
	

"$(INTDIR)\debug.obj" : $(SOURCE) $(DEP_CPP_DEBUG) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=\Tests\RawRead\text.c
DEP_CPP_TEXT_=\
	"..\bitmap.h"\
	"..\bootsec.h"\
	"..\image.h"\
	"..\text.h"\
	".\imutil32\..\..\defs.h"\
	".\imutil32\..\..\diskutil.h"\
	

"$(INTDIR)\text.obj" : $(SOURCE) $(DEP_CPP_TEXT_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=\Tests\RawRead\bootsec.c
DEP_CPP_BOOTS=\
	"..\bitmap.h"\
	"..\bootsec.h"\
	"..\image.h"\
	".\imutil32\..\..\defs.h"\
	".\imutil32\..\..\diskutil.h"\
	".\imutil32\..\..\mem.h"\
	

"$(INTDIR)\bootsec.obj" : $(SOURCE) $(DEP_CPP_BOOTS) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=\Tests\RawRead\compress.c
DEP_CPP_COMPR=\
	"..\compress.h"\
	"..\mrcicode.h"\
	".\imutil32\..\..\defs.h"\
	

"$(INTDIR)\compress.obj" : $(SOURCE) $(DEP_CPP_COMPR) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=\Tests\RawRead\fatutil.c
DEP_CPP_FATUT=\
	"..\bitmap.h"\
	"..\bootsec.h"\
	"..\fatutil.h"\
	".\imutil32\..\..\defs.h"\
	".\imutil32\..\..\diskutil.h"\
	".\imutil32\..\..\mem.h"\
	".\imutil32\..\debug.h"\
	

"$(INTDIR)\fatutil.obj" : $(SOURCE) $(DEP_CPP_FATUT) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=\Tests\RawRead\file.c
DEP_CPP_FILE_=\
	"..\bitmap.h"\
	"..\bootsec.h"\
	"..\file.h"\
	"..\image.h"\
	"..\text.h"\
	".\imutil32\..\..\defs.h"\
	".\imutil32\..\..\diskutil.h"\
	".\winmisc.h"\
	

"$(INTDIR)\file.obj" : $(SOURCE) $(DEP_CPP_FILE_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=\Tests\RawRead\image.c
DEP_CPP_IMAGE=\
	"..\bitmap.h"\
	"..\bootsec.h"\
	"..\file.h"\
	"..\image.h"\
	"..\text.h"\
	".\imutil32\..\..\defs.h"\
	".\imutil32\..\..\diskutil.h"\
	".\imutil32\..\..\mem.h"\
	".\winmisc.h"\
	

"$(INTDIR)\image.obj" : $(SOURCE) $(DEP_CPP_IMAGE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=\Tests\RawRead\mem.c
DEP_CPP_MEM_C=\
	".\imutil32\..\..\defs.h"\
	".\imutil32\..\..\mem.h"\
	

"$(INTDIR)\mem.obj" : $(SOURCE) $(DEP_CPP_MEM_C) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=\Tests\RawRead\mrcicode.c
DEP_CPP_MRCIC=\
	"..\mrcicode.h"\
	".\imutil32\..\..\defs.h"\
	

"$(INTDIR)\mrcicode.obj" : $(SOURCE) $(DEP_CPP_MRCIC) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=\Tests\RawRead\rawread.c
DEP_CPP_RAWRE=\
	"..\bitmap.h"\
	"..\bootsec.h"\
	"..\compress.h"\
	"..\fatutil.h"\
	"..\file.h"\
	"..\image.h"\
	"..\text.h"\
	".\imutil32\..\..\defs.h"\
	".\imutil32\..\..\diskutil.h"\
	".\imutil32\..\..\mem.h"\
	".\imutil32\..\debug.h"\
	".\reg.h"\
	".\winmisc.h"\
	

"$(INTDIR)\rawread.obj" : $(SOURCE) $(DEP_CPP_RAWRE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=\Tests\RawRead\bitmap.c
DEP_CPP_BITMA=\
	"..\bitmap.h"\
	"..\file.h"\
	".\imutil32\..\..\defs.h"\
	".\imutil32\..\..\mem.h"\
	

"$(INTDIR)\bitmap.obj" : $(SOURCE) $(DEP_CPP_BITMA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\image.rc

!IF  "$(CFG)" == "RawRead - Win32 Release"

DEP_RSC_IMAGE_=\
	".\image.ico"\
	".\imagedoc.ico"\
	

"$(INTDIR)\image.res" : $(SOURCE) $(DEP_RSC_IMAGE_) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "RawRead - Win32 Debug"

DEP_RSC_IMAGE_=\
	".\image.ico"\
	".\imagedoc.ico"\
	

"$(INTDIR)\image.res" : $(SOURCE) $(DEP_RSC_IMAGE_) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ENDIF 

# End Source File
# End Target
################################################################################
# Begin Target

# Name "RawWrite - Win32 Release"
# Name "RawWrite - Win32 Debug"

!IF  "$(CFG)" == "RawWrite - Win32 Release"

!ELSEIF  "$(CFG)" == "RawWrite - Win32 Debug"

!ENDIF 

################################################################################
# Begin Project Dependency

# Project_Dep_Name "Imutil32"

!IF  "$(CFG)" == "RawWrite - Win32 Release"

"Imutil32 - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\rawread.mak" CFG="Imutil32 - Win32 Release" 

!ELSEIF  "$(CFG)" == "RawWrite - Win32 Debug"

"Imutil32 - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\rawread.mak" CFG="Imutil32 - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Source File

SOURCE=.\winmisc.c
DEP_CPP_WINMI=\
	"..\bitmap.h"\
	"..\bootsec.h"\
	"..\image.h"\
	"..\text.h"\
	".\imutil32\..\..\defs.h"\
	".\imutil32\..\..\diskutil.h"\
	".\imutil32\..\..\mem.h"\
	".\imutil32\..\debug.h"\
	".\winmisc.h"\
	

"$(INTDIR)\winmisc.obj" : $(SOURCE) $(DEP_CPP_WINMI) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\image.rc

!IF  "$(CFG)" == "RawWrite - Win32 Release"

DEP_RSC_IMAGE_=\
	".\image.ico"\
	".\imagedoc.ico"\
	

"$(INTDIR)\image.res" : $(SOURCE) $(DEP_RSC_IMAGE_) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "RawWrite - Win32 Debug"

DEP_RSC_IMAGE_=\
	".\image.ico"\
	".\imagedoc.ico"\
	

"$(INTDIR)\image.res" : $(SOURCE) $(DEP_RSC_IMAGE_) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\reg.c
DEP_CPP_REG_C=\
	"..\bitmap.h"\
	"..\bootsec.h"\
	"..\image.h"\
	".\imutil32\..\..\defs.h"\
	".\imutil32\..\..\diskutil.h"\
	".\reg.h"\
	".\winmisc.h"\
	

"$(INTDIR)\reg.obj" : $(SOURCE) $(DEP_CPP_REG_C) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\debug.c
DEP_CPP_DEBUG=\
	".\imutil32\..\..\defs.h"\
	".\imutil32\..\debug.h"\
	

"$(INTDIR)\debug.obj" : $(SOURCE) $(DEP_CPP_DEBUG) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=\Tests\RawRead\text.c
DEP_CPP_TEXT_=\
	"..\bitmap.h"\
	"..\bootsec.h"\
	"..\image.h"\
	"..\text.h"\
	".\imutil32\..\..\defs.h"\
	".\imutil32\..\..\diskutil.h"\
	

"$(INTDIR)\text.obj" : $(SOURCE) $(DEP_CPP_TEXT_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=\Tests\RawRead\bootsec.c
DEP_CPP_BOOTS=\
	"..\bitmap.h"\
	"..\bootsec.h"\
	"..\image.h"\
	".\imutil32\..\..\defs.h"\
	".\imutil32\..\..\diskutil.h"\
	".\imutil32\..\..\mem.h"\
	

"$(INTDIR)\bootsec.obj" : $(SOURCE) $(DEP_CPP_BOOTS) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=\Tests\RawRead\compress.c
DEP_CPP_COMPR=\
	"..\compress.h"\
	"..\mrcicode.h"\
	".\imutil32\..\..\defs.h"\
	

"$(INTDIR)\compress.obj" : $(SOURCE) $(DEP_CPP_COMPR) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=\Tests\RawRead\fatutil.c
DEP_CPP_FATUT=\
	"..\bitmap.h"\
	"..\bootsec.h"\
	"..\fatutil.h"\
	".\imutil32\..\..\defs.h"\
	".\imutil32\..\..\diskutil.h"\
	".\imutil32\..\..\mem.h"\
	".\imutil32\..\debug.h"\
	

"$(INTDIR)\fatutil.obj" : $(SOURCE) $(DEP_CPP_FATUT) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=\Tests\RawRead\file.c
DEP_CPP_FILE_=\
	"..\bitmap.h"\
	"..\bootsec.h"\
	"..\file.h"\
	"..\image.h"\
	"..\text.h"\
	".\imutil32\..\..\defs.h"\
	".\imutil32\..\..\diskutil.h"\
	".\winmisc.h"\
	

"$(INTDIR)\file.obj" : $(SOURCE) $(DEP_CPP_FILE_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=\Tests\RawRead\image.c
DEP_CPP_IMAGE=\
	"..\bitmap.h"\
	"..\bootsec.h"\
	"..\file.h"\
	"..\image.h"\
	"..\text.h"\
	".\imutil32\..\..\defs.h"\
	".\imutil32\..\..\diskutil.h"\
	".\imutil32\..\..\mem.h"\
	".\winmisc.h"\
	

"$(INTDIR)\image.obj" : $(SOURCE) $(DEP_CPP_IMAGE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=\Tests\RawRead\mem.c
DEP_CPP_MEM_C=\
	".\imutil32\..\..\defs.h"\
	".\imutil32\..\..\mem.h"\
	

"$(INTDIR)\mem.obj" : $(SOURCE) $(DEP_CPP_MEM_C) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=\Tests\RawRead\mrcicode.c
DEP_CPP_MRCIC=\
	"..\mrcicode.h"\
	".\imutil32\..\..\defs.h"\
	

"$(INTDIR)\mrcicode.obj" : $(SOURCE) $(DEP_CPP_MRCIC) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=\Tests\RawRead\rawwrite.c
DEP_CPP_RAWWR=\
	"..\bitmap.h"\
	"..\bootsec.h"\
	"..\compress.h"\
	"..\fatutil.h"\
	"..\file.h"\
	"..\image.h"\
	"..\text.h"\
	".\imutil32\..\..\defs.h"\
	".\imutil32\..\..\diskutil.h"\
	".\imutil32\..\..\mem.h"\
	".\imutil32\..\debug.h"\
	".\reg.h"\
	".\winmisc.h"\
	

"$(INTDIR)\rawwrite.obj" : $(SOURCE) $(DEP_CPP_RAWWR) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=\Tests\RawRead\bitmap.c
DEP_CPP_BITMA=\
	"..\bitmap.h"\
	"..\file.h"\
	".\imutil32\..\..\defs.h"\
	".\imutil32\..\..\mem.h"\
	

"$(INTDIR)\bitmap.obj" : $(SOURCE) $(DEP_CPP_BITMA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
# End Target
################################################################################
# Begin Target

# Name "Imutil32 - Win32 Release"
# Name "Imutil32 - Win32 Debug"

!IF  "$(CFG)" == "Imutil32 - Win32 Release"

!ELSEIF  "$(CFG)" == "Imutil32 - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\imutil32\imutil32.rc

!IF  "$(CFG)" == "Imutil32 - Win32 Release"


"$(INTDIR)\imutil32.res" : $(SOURCE) "$(INTDIR)"
   $(RSC) /l 0x409 /fo"$(INTDIR)/imutil32.res" /i "imutil32" /d "NDEBUG"\
 $(SOURCE)


!ELSEIF  "$(CFG)" == "Imutil32 - Win32 Debug"


"$(INTDIR)\imutil32.res" : $(SOURCE) "$(INTDIR)"
   $(RSC) /l 0x409 /fo"$(INTDIR)/imutil32.res" /i "imutil32" /d "_DEBUG"\
 $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\imutil32\imutil32.c

!IF  "$(CFG)" == "Imutil32 - Win32 Release"

DEP_CPP_IMUTI=\
	".\imutil32\..\..\defs.h"\
	".\imutil32\..\..\diskutil.h"\
	".\imutil32\..\..\mem.h"\
	".\imutil32\..\debug.h"\
	

"$(INTDIR)\imutil32.obj" : $(SOURCE) $(DEP_CPP_IMUTI) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Imutil32 - Win32 Debug"

DEP_CPP_IMUTI=\
	".\imutil32\..\..\defs.h"\
	".\imutil32\..\..\diskutil.h"\
	".\imutil32\..\..\mem.h"\
	".\imutil32\..\debug.h"\
	

"$(INTDIR)\imutil32.obj" : $(SOURCE) $(DEP_CPP_IMUTI) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\imutil32\imutil32.def

!IF  "$(CFG)" == "Imutil32 - Win32 Release"

!ELSEIF  "$(CFG)" == "Imutil32 - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\imutil32\dllmain.c

!IF  "$(CFG)" == "Imutil32 - Win32 Release"

DEP_CPP_DLLMA=\
	".\imutil32\..\..\defs.h"\
	".\imutil32\..\debug.h"\
	

"$(INTDIR)\dllmain.obj" : $(SOURCE) $(DEP_CPP_DLLMA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Imutil32 - Win32 Debug"

DEP_CPP_DLLMA=\
	".\imutil32\..\..\defs.h"\
	".\imutil32\..\debug.h"\
	

"$(INTDIR)\dllmain.obj" : $(SOURCE) $(DEP_CPP_DLLMA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Tests\RawRead\mem.c

!IF  "$(CFG)" == "Imutil32 - Win32 Release"

DEP_CPP_MEM_C=\
	".\imutil32\..\..\defs.h"\
	".\imutil32\..\..\mem.h"\
	

"$(INTDIR)\mem.obj" : $(SOURCE) $(DEP_CPP_MEM_C) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Imutil32 - Win32 Debug"

DEP_CPP_MEM_C=\
	".\imutil32\..\..\defs.h"\
	".\imutil32\..\..\mem.h"\
	

"$(INTDIR)\mem.obj" : $(SOURCE) $(DEP_CPP_MEM_C) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\debug.c

!IF  "$(CFG)" == "Imutil32 - Win32 Release"

DEP_CPP_DEBUG=\
	".\imutil32\..\..\defs.h"\
	".\imutil32\..\debug.h"\
	

"$(INTDIR)\debug.obj" : $(SOURCE) $(DEP_CPP_DEBUG) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Imutil32 - Win32 Debug"

DEP_CPP_DEBUG=\
	".\imutil32\..\..\defs.h"\
	".\imutil32\..\debug.h"\
	

"$(INTDIR)\debug.obj" : $(SOURCE) $(DEP_CPP_DEBUG) "$(INTDIR)"


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
