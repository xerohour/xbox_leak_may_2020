# Microsoft Developer Studio Generated NMAKE File, Based on DDK32.dsp
!IF "$(CFG)" == ""
CFG=DDK32 - Win32 Release
!MESSAGE No configuration specified. Defaulting to DDK32 - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "DDK32 - Win32 Release" && "$(CFG)" != "DDK32 - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "DDK32.mak" CFG="DDK32 - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "DDK32 - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "DDK32 - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "DDK32 - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\DDK32.exe"


CLEAN :
	-@erase "$(INTDIR)\main.obj"
	-@erase "$(OUTDIR)\DDK32.exe"
	-@erase "$(OUTDIR)\DDK32.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /ML /W3 /GX /O2 /I "..\..\INC" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /Fp"$(INTDIR)\DDK32.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\DDK32.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib nvw32ms.lib nvrmapi.lib /nologo /subsystem:console /incremental:yes /pdb:"$(OUTDIR)\DDK32.pdb" /machine:I386 /out:"$(OUTDIR)\DDK32.exe" /libpath:"..\..\nv10\lib" 
LINK32_OBJS= \
	"$(INTDIR)\main.obj" \
	"..\..\nv10\lib\nvrmapi.lib" \
	"..\..\nv10\lib\nvw32ms.lib"

"$(OUTDIR)\DDK32.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "DDK32 - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\DDK32.exe"


CLEAN :
	-@erase "$(INTDIR)\main.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\DDK32.exe"
	-@erase "$(OUTDIR)\DDK32.ilk"
	-@erase "$(OUTDIR)\DDK32.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MLd /W3 /Gm /GX /ZI /Od /I "..\..\INC\\" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /Fp"$(INTDIR)\DDK32.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\DDK32.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib nvw32ms.lib nvrmapi.lib /nologo /subsystem:console /incremental:yes /pdb:"$(OUTDIR)\DDK32.pdb" /debug /machine:I386 /nodefaultlib:"libc.lib" /out:"$(OUTDIR)\DDK32.exe" /libpath:"..\..\nv10\lib\\" 
LINK32_OBJS= \
	"$(INTDIR)\main.obj" \
	"..\..\nv10\lib\nvrmapi.lib" \
	"..\..\nv10\lib\nvw32ms.lib"

"$(OUTDIR)\DDK32.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("DDK32.dep")
!INCLUDE "DDK32.dep"
!ELSE 
!MESSAGE Warning: cannot find "DDK32.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "DDK32 - Win32 Release" || "$(CFG)" == "DDK32 - Win32 Debug"
SOURCE=.\main.c

"$(INTDIR)\main.obj" : $(SOURCE) "$(INTDIR)"



!ENDIF 

