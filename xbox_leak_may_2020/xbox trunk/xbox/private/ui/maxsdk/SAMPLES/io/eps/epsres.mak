# Microsoft Developer Studio Generated NMAKE File, Based on Epsres.dsp
!IF "$(CFG)" == ""
CFG=epsres - Win32 Release
!MESSAGE No configuration specified. Defaulting to epsres - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "epsres - Win32 Release" && "$(CFG)" != "epsres - Win32 Debug"\
 && "$(CFG)" != "epsres - Win32 Hybrid"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Epsres.mak" CFG="epsres - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "epsres - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "epsres - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "epsres - Win32 Hybrid" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "epsres - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

!IF "$(RECURSE)" == "0" 

ALL : "..\..\..\..\maxsdk\plugin\epsres.dll"

!ELSE 

ALL : "..\..\..\..\maxsdk\plugin\epsres.dll"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\EPSRES.res"
	-@erase "$(OUTDIR)\epsres.exp"
	-@erase "$(OUTDIR)\epsres.lib"
	-@erase "..\..\..\..\maxsdk\plugin\epsres.dll"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MD /W3 /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "WIN32_LEAN_AND_MEAN" /Fp"$(INTDIR)\Epsres.pch" /YX /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Release/
CPP_SBRS=.
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Epsres.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /base:"0x10320000" /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)\epsres.pdb" /machine:I386 /nodefaultlib:"libc"\
 /out:"..\..\..\..\maxsdk\plugin\epsres.dll" /implib:"$(OUTDIR)\epsres.lib"\
 /release /NOENTRY 
LINK32_OBJS= \
	"$(INTDIR)\EPSRES.res"

"..\..\..\..\maxsdk\plugin\epsres.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "epsres - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

!IF "$(RECURSE)" == "0" 

ALL : "..\..\..\..\maxsdk\plugin\epsres.dll"

!ELSE 

ALL : "..\..\..\..\maxsdk\plugin\epsres.dll"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\EPSRES.res"
	-@erase "$(OUTDIR)\epsres.exp"
	-@erase "$(OUTDIR)\epsres.lib"
	-@erase "$(OUTDIR)\epsres.pdb"
	-@erase "..\..\..\..\maxsdk\plugin\epsres.dll"
	-@erase "..\..\..\..\maxsdk\plugin\epsres.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MDd /W3 /Od /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "WIN32_LEAN_AND_MEAN" /Fp"$(INTDIR)\Epsres.pch" /YX /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Epsres.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /base:"0x10320000" /subsystem:windows /dll\
 /incremental:yes /pdb:"$(OUTDIR)\epsres.pdb" /debug /machine:I386\
 /nodefaultlib:"libc" /out:"..\..\..\..\maxsdk\plugin\epsres.dll"\
 /implib:"$(OUTDIR)\epsres.lib" /NOENTRY 
LINK32_OBJS= \
	"$(INTDIR)\EPSRES.res"

"..\..\..\..\maxsdk\plugin\epsres.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "epsres - Win32 Hybrid"

OUTDIR=.\Hybrid
INTDIR=.\Hybrid

!IF "$(RECURSE)" == "0" 

ALL : "..\..\..\..\maxsdk\plugin\epsres.dll"

!ELSE 

ALL : "..\..\..\..\maxsdk\plugin\epsres.dll"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\EPSRES.res"
	-@erase "$(OUTDIR)\epsres.exp"
	-@erase "$(OUTDIR)\epsres.lib"
	-@erase "$(OUTDIR)\epsres.pdb"
	-@erase "..\..\..\..\maxsdk\plugin\epsres.dll"
	-@erase "..\..\..\..\maxsdk\plugin\epsres.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MD /W3 /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "WIN32_LEAN_AND_MEAN" /Fp"$(INTDIR)\Epsres.pch" /YX /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Hybrid/
CPP_SBRS=.
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Epsres.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /base:"0x10320000" /subsystem:windows /dll\
 /incremental:yes /pdb:"$(OUTDIR)\epsres.pdb" /debug /machine:I386\
 /nodefaultlib:"libc" /out:"..\..\..\..\maxsdk\plugin\epsres.dll"\
 /implib:"$(OUTDIR)\epsres.lib" /NOENTRY 
LINK32_OBJS= \
	"$(INTDIR)\EPSRES.res"

"..\..\..\..\maxsdk\plugin\epsres.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\EPSRES.res" /d "NDEBUG" 

!IF "$(CFG)" == "epsres - Win32 Release" || "$(CFG)" == "epsres - Win32 Debug"\
 || "$(CFG)" == "epsres - Win32 Hybrid"
SOURCE=.\EPSRES.RC
DEP_RSC_EPSRE=\
	".\landscap.BMP"\
	".\landscap.ICO"\
	".\portrait.BMP"\
	".\PORTRAIT.ICO"\
	

"$(INTDIR)\EPSRES.res" : $(SOURCE) $(DEP_RSC_EPSRE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

