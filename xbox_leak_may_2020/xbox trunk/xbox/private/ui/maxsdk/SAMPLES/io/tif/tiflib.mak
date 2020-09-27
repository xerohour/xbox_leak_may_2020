# Microsoft Developer Studio Generated NMAKE File, Based on Tiflib.dsp
!IF "$(CFG)" == ""
CFG=tiflib - Win32 Release
!MESSAGE No configuration specified. Defaulting to tiflib - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "tiflib - Win32 Release" && "$(CFG)" != "tiflib - Win32 Debug" && "$(CFG)" != "tiflib - Win32 Hybrid"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Tiflib.mak" CFG="tiflib - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "tiflib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "tiflib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "tiflib - Win32 Hybrid" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "tiflib - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\Tiflib.lib"


CLEAN :
	-@erase "$(INTDIR)\GeoTIFF_Read.obj"
	-@erase "$(INTDIR)\TIF_CFAX.OBJ"
	-@erase "$(INTDIR)\TIF_CLSE.OBJ"
	-@erase "$(INTDIR)\TIF_COMP.OBJ"
	-@erase "$(INTDIR)\TIF_CRLE.OBJ"
	-@erase "$(INTDIR)\TIF_DIR.OBJ"
	-@erase "$(INTDIR)\TIF_DUMP.OBJ"
	-@erase "$(INTDIR)\TIF_FLSH.OBJ"
	-@erase "$(INTDIR)\TIF_LZW.OBJ"
	-@erase "$(INTDIR)\tif_misc.obj"
	-@erase "$(INTDIR)\TIF_NEXT.OBJ"
	-@erase "$(INTDIR)\TIF_OPEN.OBJ"
	-@erase "$(INTDIR)\TIF_PACK.OBJ"
	-@erase "$(INTDIR)\TIF_PIC.OBJ"
	-@erase "$(INTDIR)\TIF_PRNT.OBJ"
	-@erase "$(INTDIR)\TIF_READ.OBJ"
	-@erase "$(INTDIR)\TIF_SGI.OBJ"
	-@erase "$(INTDIR)\TIF_SWAB.OBJ"
	-@erase "$(INTDIR)\TIF_THDR.OBJ"
	-@erase "$(OUTDIR)\Tiflib.lib"
	-@erase ".\tiflib.idb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MD /W3 /O2 /I "..\..\..\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "WIN32_LEAN_AND_MEAN" /Fo"$(INTDIR)\\" /Fd"tiflib.pdb" /FD /c 

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

RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Tiflib.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\Tiflib.lib" 
LIB32_OBJS= \
	"$(INTDIR)\GeoTIFF_Read.obj" \
	"$(INTDIR)\TIF_CFAX.OBJ" \
	"$(INTDIR)\TIF_CLSE.OBJ" \
	"$(INTDIR)\TIF_COMP.OBJ" \
	"$(INTDIR)\TIF_CRLE.OBJ" \
	"$(INTDIR)\TIF_DIR.OBJ" \
	"$(INTDIR)\TIF_DUMP.OBJ" \
	"$(INTDIR)\TIF_FLSH.OBJ" \
	"$(INTDIR)\TIF_LZW.OBJ" \
	"$(INTDIR)\tif_misc.obj" \
	"$(INTDIR)\TIF_NEXT.OBJ" \
	"$(INTDIR)\TIF_OPEN.OBJ" \
	"$(INTDIR)\TIF_PACK.OBJ" \
	"$(INTDIR)\TIF_PIC.OBJ" \
	"$(INTDIR)\TIF_PRNT.OBJ" \
	"$(INTDIR)\TIF_READ.OBJ" \
	"$(INTDIR)\TIF_SGI.OBJ" \
	"$(INTDIR)\TIF_SWAB.OBJ" \
	"$(INTDIR)\TIF_THDR.OBJ"

"$(OUTDIR)\Tiflib.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "tiflib - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\Tiflib.lib"


CLEAN :
	-@erase "$(INTDIR)\GeoTIFF_Read.obj"
	-@erase "$(INTDIR)\TIF_CFAX.OBJ"
	-@erase "$(INTDIR)\TIF_CLSE.OBJ"
	-@erase "$(INTDIR)\TIF_COMP.OBJ"
	-@erase "$(INTDIR)\TIF_CRLE.OBJ"
	-@erase "$(INTDIR)\TIF_DIR.OBJ"
	-@erase "$(INTDIR)\TIF_DUMP.OBJ"
	-@erase "$(INTDIR)\TIF_FLSH.OBJ"
	-@erase "$(INTDIR)\TIF_LZW.OBJ"
	-@erase "$(INTDIR)\tif_misc.obj"
	-@erase "$(INTDIR)\TIF_NEXT.OBJ"
	-@erase "$(INTDIR)\TIF_OPEN.OBJ"
	-@erase "$(INTDIR)\TIF_PACK.OBJ"
	-@erase "$(INTDIR)\TIF_PIC.OBJ"
	-@erase "$(INTDIR)\TIF_PRNT.OBJ"
	-@erase "$(INTDIR)\TIF_READ.OBJ"
	-@erase "$(INTDIR)\TIF_SGI.OBJ"
	-@erase "$(INTDIR)\TIF_SWAB.OBJ"
	-@erase "$(INTDIR)\TIF_THDR.OBJ"
	-@erase "$(OUTDIR)\Tiflib.lib"
	-@erase ".\tiflib.idb"
	-@erase ".\tiflib.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "WIN32_LEAN_AND_MEAN" /Fo"$(INTDIR)\\" /Fd"tiflib.pdb" /FD /c 

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

RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Tiflib.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\Tiflib.lib" 
LIB32_OBJS= \
	"$(INTDIR)\GeoTIFF_Read.obj" \
	"$(INTDIR)\TIF_CFAX.OBJ" \
	"$(INTDIR)\TIF_CLSE.OBJ" \
	"$(INTDIR)\TIF_COMP.OBJ" \
	"$(INTDIR)\TIF_CRLE.OBJ" \
	"$(INTDIR)\TIF_DIR.OBJ" \
	"$(INTDIR)\TIF_DUMP.OBJ" \
	"$(INTDIR)\TIF_FLSH.OBJ" \
	"$(INTDIR)\TIF_LZW.OBJ" \
	"$(INTDIR)\tif_misc.obj" \
	"$(INTDIR)\TIF_NEXT.OBJ" \
	"$(INTDIR)\TIF_OPEN.OBJ" \
	"$(INTDIR)\TIF_PACK.OBJ" \
	"$(INTDIR)\TIF_PIC.OBJ" \
	"$(INTDIR)\TIF_PRNT.OBJ" \
	"$(INTDIR)\TIF_READ.OBJ" \
	"$(INTDIR)\TIF_SGI.OBJ" \
	"$(INTDIR)\TIF_SWAB.OBJ" \
	"$(INTDIR)\TIF_THDR.OBJ"

"$(OUTDIR)\Tiflib.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "tiflib - Win32 Hybrid"

OUTDIR=.\Hybrid
INTDIR=.\Hybrid
# Begin Custom Macros
OutDir=.\Hybrid
# End Custom Macros

ALL : "$(OUTDIR)\Tiflib.lib"


CLEAN :
	-@erase "$(INTDIR)\GeoTIFF_Read.obj"
	-@erase "$(INTDIR)\TIF_CFAX.OBJ"
	-@erase "$(INTDIR)\TIF_CLSE.OBJ"
	-@erase "$(INTDIR)\TIF_COMP.OBJ"
	-@erase "$(INTDIR)\TIF_CRLE.OBJ"
	-@erase "$(INTDIR)\TIF_DIR.OBJ"
	-@erase "$(INTDIR)\TIF_DUMP.OBJ"
	-@erase "$(INTDIR)\TIF_FLSH.OBJ"
	-@erase "$(INTDIR)\TIF_LZW.OBJ"
	-@erase "$(INTDIR)\tif_misc.obj"
	-@erase "$(INTDIR)\TIF_NEXT.OBJ"
	-@erase "$(INTDIR)\TIF_OPEN.OBJ"
	-@erase "$(INTDIR)\TIF_PACK.OBJ"
	-@erase "$(INTDIR)\TIF_PIC.OBJ"
	-@erase "$(INTDIR)\TIF_PRNT.OBJ"
	-@erase "$(INTDIR)\TIF_READ.OBJ"
	-@erase "$(INTDIR)\TIF_SGI.OBJ"
	-@erase "$(INTDIR)\TIF_SWAB.OBJ"
	-@erase "$(INTDIR)\TIF_THDR.OBJ"
	-@erase "$(OUTDIR)\Tiflib.lib"
	-@erase ".\tiflib.idb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MD /W3 /Z7 /Od /I "..\..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "WIN32_LEAN_AND_MEAN" /Fo"$(INTDIR)\\" /Fd"tiflib.pdb" /FD /c 

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

RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Tiflib.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\Tiflib.lib" 
LIB32_OBJS= \
	"$(INTDIR)\GeoTIFF_Read.obj" \
	"$(INTDIR)\TIF_CFAX.OBJ" \
	"$(INTDIR)\TIF_CLSE.OBJ" \
	"$(INTDIR)\TIF_COMP.OBJ" \
	"$(INTDIR)\TIF_CRLE.OBJ" \
	"$(INTDIR)\TIF_DIR.OBJ" \
	"$(INTDIR)\TIF_DUMP.OBJ" \
	"$(INTDIR)\TIF_FLSH.OBJ" \
	"$(INTDIR)\TIF_LZW.OBJ" \
	"$(INTDIR)\tif_misc.obj" \
	"$(INTDIR)\TIF_NEXT.OBJ" \
	"$(INTDIR)\TIF_OPEN.OBJ" \
	"$(INTDIR)\TIF_PACK.OBJ" \
	"$(INTDIR)\TIF_PIC.OBJ" \
	"$(INTDIR)\TIF_PRNT.OBJ" \
	"$(INTDIR)\TIF_READ.OBJ" \
	"$(INTDIR)\TIF_SGI.OBJ" \
	"$(INTDIR)\TIF_SWAB.OBJ" \
	"$(INTDIR)\TIF_THDR.OBJ"

"$(OUTDIR)\Tiflib.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("Tiflib.dep")
!INCLUDE "Tiflib.dep"
!ELSE 
!MESSAGE Warning: cannot find "Tiflib.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "tiflib - Win32 Release" || "$(CFG)" == "tiflib - Win32 Debug" || "$(CFG)" == "tiflib - Win32 Hybrid"
SOURCE=.\GeoTIFF_Read.cpp

"$(INTDIR)\GeoTIFF_Read.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\TIF_CFAX.C

"$(INTDIR)\TIF_CFAX.OBJ" : $(SOURCE) "$(INTDIR)"


SOURCE=.\TIF_CLSE.C

"$(INTDIR)\TIF_CLSE.OBJ" : $(SOURCE) "$(INTDIR)"


SOURCE=.\TIF_COMP.C

"$(INTDIR)\TIF_COMP.OBJ" : $(SOURCE) "$(INTDIR)"


SOURCE=.\TIF_CRLE.C

"$(INTDIR)\TIF_CRLE.OBJ" : $(SOURCE) "$(INTDIR)"


SOURCE=.\TIF_DIR.C

"$(INTDIR)\TIF_DIR.OBJ" : $(SOURCE) "$(INTDIR)"


SOURCE=.\TIF_DUMP.C

"$(INTDIR)\TIF_DUMP.OBJ" : $(SOURCE) "$(INTDIR)"


SOURCE=.\TIF_FLSH.C

"$(INTDIR)\TIF_FLSH.OBJ" : $(SOURCE) "$(INTDIR)"


SOURCE=.\TIF_LZW.C

"$(INTDIR)\TIF_LZW.OBJ" : $(SOURCE) "$(INTDIR)"


SOURCE=.\tif_misc.c

"$(INTDIR)\tif_misc.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\TIF_NEXT.C

"$(INTDIR)\TIF_NEXT.OBJ" : $(SOURCE) "$(INTDIR)"


SOURCE=.\TIF_OPEN.C

"$(INTDIR)\TIF_OPEN.OBJ" : $(SOURCE) "$(INTDIR)"


SOURCE=.\TIF_PACK.C

"$(INTDIR)\TIF_PACK.OBJ" : $(SOURCE) "$(INTDIR)"


SOURCE=.\TIF_PIC.C

"$(INTDIR)\TIF_PIC.OBJ" : $(SOURCE) "$(INTDIR)"


SOURCE=.\TIF_PRNT.C

"$(INTDIR)\TIF_PRNT.OBJ" : $(SOURCE) "$(INTDIR)"


SOURCE=.\TIF_READ.C

"$(INTDIR)\TIF_READ.OBJ" : $(SOURCE) "$(INTDIR)"


SOURCE=.\TIF_SGI.C

"$(INTDIR)\TIF_SGI.OBJ" : $(SOURCE) "$(INTDIR)"


SOURCE=.\TIF_SWAB.C

"$(INTDIR)\TIF_SWAB.OBJ" : $(SOURCE) "$(INTDIR)"


SOURCE=.\TIF_THDR.C

"$(INTDIR)\TIF_THDR.OBJ" : $(SOURCE) "$(INTDIR)"



!ENDIF 

