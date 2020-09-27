# Microsoft Developer Studio Generated NMAKE File, Based on Libpng.dsp
!IF "$(CFG)" == ""
CFG=libpng - Win32 Hybrid
!MESSAGE No configuration specified. Defaulting to libpng - Win32 Hybrid.
!ENDIF 

!IF "$(CFG)" != "libpng - Win32 Release" && "$(CFG)" != "libpng - Win32 Debug"\
 && "$(CFG)" != "libpng - Win32 Hybrid"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Libpng.mak" CFG="libpng - Win32 Hybrid"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libpng - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "libpng - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "libpng - Win32 Hybrid" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe

!IF  "$(CFG)" == "libpng - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

!IF "$(RECURSE)" == "0" 

ALL : ".\libpng.lib"

!ELSE 

ALL : ".\libpng.lib"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\png.obj"
	-@erase "$(INTDIR)\pngerror.obj"
	-@erase "$(INTDIR)\pngget.obj"
	-@erase "$(INTDIR)\pngmem.obj"
	-@erase "$(INTDIR)\pngpread.obj"
	-@erase "$(INTDIR)\pngread.obj"
	-@erase "$(INTDIR)\pngrio.obj"
	-@erase "$(INTDIR)\pngrtran.obj"
	-@erase "$(INTDIR)\pngrutil.obj"
	-@erase "$(INTDIR)\pngset.obj"
	-@erase "$(INTDIR)\pngtrans.obj"
	-@erase "$(INTDIR)\pngwio.obj"
	-@erase "$(INTDIR)\pngwrite.obj"
	-@erase "$(INTDIR)\pngwtran.obj"
	-@erase "$(INTDIR)\pngwutil.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase ".\libpng.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MD /W3 /O2 /I "..\..\..\..\..\include" /I "..\..\..\..\include" /D "NDEBUG" /D\
 "WIN32" /D "_WINDOWS" /D "WIN32_LEAN_AND_MEAN" /Fp"$(INTDIR)\Libpng.pch" /YX\
 /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Release/
CPP_SBRS=.
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Libpng.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:".\libpng.lib" 
LIB32_OBJS= \
	"$(INTDIR)\png.obj" \
	"$(INTDIR)\pngerror.obj" \
	"$(INTDIR)\pngget.obj" \
	"$(INTDIR)\pngmem.obj" \
	"$(INTDIR)\pngpread.obj" \
	"$(INTDIR)\pngread.obj" \
	"$(INTDIR)\pngrio.obj" \
	"$(INTDIR)\pngrtran.obj" \
	"$(INTDIR)\pngrutil.obj" \
	"$(INTDIR)\pngset.obj" \
	"$(INTDIR)\pngtrans.obj" \
	"$(INTDIR)\pngwio.obj" \
	"$(INTDIR)\pngwrite.obj" \
	"$(INTDIR)\pngwtran.obj" \
	"$(INTDIR)\pngwutil.obj" \
	"..\..\..\..\lib\zlibdll.lib"

".\libpng.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "libpng - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

!IF "$(RECURSE)" == "0" 

ALL : ".\libpng.lib"

!ELSE 

ALL : ".\libpng.lib"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\png.obj"
	-@erase "$(INTDIR)\pngerror.obj"
	-@erase "$(INTDIR)\pngget.obj"
	-@erase "$(INTDIR)\pngmem.obj"
	-@erase "$(INTDIR)\pngpread.obj"
	-@erase "$(INTDIR)\pngread.obj"
	-@erase "$(INTDIR)\pngrio.obj"
	-@erase "$(INTDIR)\pngrtran.obj"
	-@erase "$(INTDIR)\pngrutil.obj"
	-@erase "$(INTDIR)\pngset.obj"
	-@erase "$(INTDIR)\pngtrans.obj"
	-@erase "$(INTDIR)\pngwio.obj"
	-@erase "$(INTDIR)\pngwrite.obj"
	-@erase "$(INTDIR)\pngwtran.obj"
	-@erase "$(INTDIR)\pngwutil.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase ".\libpng.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MDd /W3 /Z7 /Od /I "..\..\..\..\..\include" /I "..\..\..\..\include" /D "_DEBUG"\
 /D "WIN32" /D "_WINDOWS" /D "WIN32_LEAN_AND_MEAN" /Fp"$(INTDIR)\Libpng.pch" /YX\
 /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Libpng.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:".\libpng.lib" 
LIB32_OBJS= \
	"$(INTDIR)\png.obj" \
	"$(INTDIR)\pngerror.obj" \
	"$(INTDIR)\pngget.obj" \
	"$(INTDIR)\pngmem.obj" \
	"$(INTDIR)\pngpread.obj" \
	"$(INTDIR)\pngread.obj" \
	"$(INTDIR)\pngrio.obj" \
	"$(INTDIR)\pngrtran.obj" \
	"$(INTDIR)\pngrutil.obj" \
	"$(INTDIR)\pngset.obj" \
	"$(INTDIR)\pngtrans.obj" \
	"$(INTDIR)\pngwio.obj" \
	"$(INTDIR)\pngwrite.obj" \
	"$(INTDIR)\pngwtran.obj" \
	"$(INTDIR)\pngwutil.obj" \
	"..\..\..\..\lib\zlibdll.lib"

".\libpng.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "libpng - Win32 Hybrid"

OUTDIR=.\Hybrid
INTDIR=.\Hybrid

!IF "$(RECURSE)" == "0" 

ALL : ".\libpng.lib"

!ELSE 

ALL : ".\libpng.lib"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\png.obj"
	-@erase "$(INTDIR)\pngerror.obj"
	-@erase "$(INTDIR)\pngget.obj"
	-@erase "$(INTDIR)\pngmem.obj"
	-@erase "$(INTDIR)\pngpread.obj"
	-@erase "$(INTDIR)\pngread.obj"
	-@erase "$(INTDIR)\pngrio.obj"
	-@erase "$(INTDIR)\pngrtran.obj"
	-@erase "$(INTDIR)\pngrutil.obj"
	-@erase "$(INTDIR)\pngset.obj"
	-@erase "$(INTDIR)\pngtrans.obj"
	-@erase "$(INTDIR)\pngwio.obj"
	-@erase "$(INTDIR)\pngwrite.obj"
	-@erase "$(INTDIR)\pngwtran.obj"
	-@erase "$(INTDIR)\pngwutil.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase ".\libpng.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MD /W3 /Z7 /Od /I "..\..\..\..\..\include" /I "..\..\..\..\include" /D "_DEBUG" /D\
 "WIN32" /D "_WINDOWS" /D "WIN32_LEAN_AND_MEAN" /Fp"$(INTDIR)\Libpng.pch" /YX\
 /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Hybrid/
CPP_SBRS=.
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Libpng.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:".\libpng.lib" 
LIB32_OBJS= \
	"$(INTDIR)\png.obj" \
	"$(INTDIR)\pngerror.obj" \
	"$(INTDIR)\pngget.obj" \
	"$(INTDIR)\pngmem.obj" \
	"$(INTDIR)\pngpread.obj" \
	"$(INTDIR)\pngread.obj" \
	"$(INTDIR)\pngrio.obj" \
	"$(INTDIR)\pngrtran.obj" \
	"$(INTDIR)\pngrutil.obj" \
	"$(INTDIR)\pngset.obj" \
	"$(INTDIR)\pngtrans.obj" \
	"$(INTDIR)\pngwio.obj" \
	"$(INTDIR)\pngwrite.obj" \
	"$(INTDIR)\pngwtran.obj" \
	"$(INTDIR)\pngwutil.obj" \
	"..\..\..\..\lib\zlibdll.lib"

".\libpng.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
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


!IF "$(CFG)" == "libpng - Win32 Release" || "$(CFG)" == "libpng - Win32 Debug"\
 || "$(CFG)" == "libpng - Win32 Hybrid"
SOURCE=.\png.cpp

!IF  "$(CFG)" == "libpng - Win32 Release"

DEP_CPP_PNG_C=\
	"..\..\..\..\include\zlibdll.h"\
	".\png.h"\
	".\pngconf.h"\
	".\zconf.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_PNG_C=\
	".\alloc.h"\
	

"$(INTDIR)\png.obj" : $(SOURCE) $(DEP_CPP_PNG_C) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "libpng - Win32 Debug"

DEP_CPP_PNG_C=\
	"..\..\..\..\include\zlibdll.h"\
	".\png.h"\
	".\pngconf.h"\
	".\zconf.h"\
	

"$(INTDIR)\png.obj" : $(SOURCE) $(DEP_CPP_PNG_C) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "libpng - Win32 Hybrid"

DEP_CPP_PNG_C=\
	"..\..\..\..\include\zlibdll.h"\
	".\png.h"\
	".\pngconf.h"\
	".\zconf.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_PNG_C=\
	".\alloc.h"\
	

"$(INTDIR)\png.obj" : $(SOURCE) $(DEP_CPP_PNG_C) "$(INTDIR)"


!ENDIF 

SOURCE=.\pngerror.cpp

!IF  "$(CFG)" == "libpng - Win32 Release"

DEP_CPP_PNGER=\
	"..\..\..\..\include\zlibdll.h"\
	".\png.h"\
	".\pngconf.h"\
	".\zconf.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_PNGER=\
	".\alloc.h"\
	

"$(INTDIR)\pngerror.obj" : $(SOURCE) $(DEP_CPP_PNGER) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "libpng - Win32 Debug"

DEP_CPP_PNGER=\
	"..\..\..\..\include\zlibdll.h"\
	".\png.h"\
	".\pngconf.h"\
	".\zconf.h"\
	

"$(INTDIR)\pngerror.obj" : $(SOURCE) $(DEP_CPP_PNGER) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "libpng - Win32 Hybrid"

DEP_CPP_PNGER=\
	"..\..\..\..\include\zlibdll.h"\
	".\png.h"\
	".\pngconf.h"\
	".\zconf.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_PNGER=\
	".\alloc.h"\
	

"$(INTDIR)\pngerror.obj" : $(SOURCE) $(DEP_CPP_PNGER) "$(INTDIR)"


!ENDIF 

SOURCE=.\pngget.cpp

!IF  "$(CFG)" == "libpng - Win32 Release"

DEP_CPP_PNGGE=\
	"..\..\..\..\include\zlibdll.h"\
	".\png.h"\
	".\pngconf.h"\
	".\zconf.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_PNGGE=\
	".\alloc.h"\
	

"$(INTDIR)\pngget.obj" : $(SOURCE) $(DEP_CPP_PNGGE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "libpng - Win32 Debug"

DEP_CPP_PNGGE=\
	"..\..\..\..\include\zlibdll.h"\
	".\png.h"\
	".\pngconf.h"\
	".\zconf.h"\
	

"$(INTDIR)\pngget.obj" : $(SOURCE) $(DEP_CPP_PNGGE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "libpng - Win32 Hybrid"

DEP_CPP_PNGGE=\
	"..\..\..\..\include\zlibdll.h"\
	".\png.h"\
	".\pngconf.h"\
	".\zconf.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_PNGGE=\
	".\alloc.h"\
	

"$(INTDIR)\pngget.obj" : $(SOURCE) $(DEP_CPP_PNGGE) "$(INTDIR)"


!ENDIF 

SOURCE=.\pngmem.cpp

!IF  "$(CFG)" == "libpng - Win32 Release"

DEP_CPP_PNGME=\
	"..\..\..\..\include\zlibdll.h"\
	".\png.h"\
	".\pngconf.h"\
	".\zconf.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_PNGME=\
	".\alloc.h"\
	

"$(INTDIR)\pngmem.obj" : $(SOURCE) $(DEP_CPP_PNGME) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "libpng - Win32 Debug"

DEP_CPP_PNGME=\
	"..\..\..\..\include\zlibdll.h"\
	".\png.h"\
	".\pngconf.h"\
	".\zconf.h"\
	

"$(INTDIR)\pngmem.obj" : $(SOURCE) $(DEP_CPP_PNGME) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "libpng - Win32 Hybrid"

DEP_CPP_PNGME=\
	"..\..\..\..\include\zlibdll.h"\
	".\png.h"\
	".\pngconf.h"\
	".\zconf.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_PNGME=\
	".\alloc.h"\
	

"$(INTDIR)\pngmem.obj" : $(SOURCE) $(DEP_CPP_PNGME) "$(INTDIR)"


!ENDIF 

SOURCE=.\pngpread.cpp

!IF  "$(CFG)" == "libpng - Win32 Release"

DEP_CPP_PNGPR=\
	"..\..\..\..\include\zlibdll.h"\
	".\png.h"\
	".\pngconf.h"\
	".\zconf.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_PNGPR=\
	".\alloc.h"\
	

"$(INTDIR)\pngpread.obj" : $(SOURCE) $(DEP_CPP_PNGPR) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "libpng - Win32 Debug"

DEP_CPP_PNGPR=\
	"..\..\..\..\include\zlibdll.h"\
	".\png.h"\
	".\pngconf.h"\
	".\zconf.h"\
	

"$(INTDIR)\pngpread.obj" : $(SOURCE) $(DEP_CPP_PNGPR) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "libpng - Win32 Hybrid"

DEP_CPP_PNGPR=\
	"..\..\..\..\include\zlibdll.h"\
	".\png.h"\
	".\pngconf.h"\
	".\zconf.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_PNGPR=\
	".\alloc.h"\
	

"$(INTDIR)\pngpread.obj" : $(SOURCE) $(DEP_CPP_PNGPR) "$(INTDIR)"


!ENDIF 

SOURCE=.\pngread.cpp

!IF  "$(CFG)" == "libpng - Win32 Release"

DEP_CPP_PNGRE=\
	"..\..\..\..\include\zlibdll.h"\
	".\png.h"\
	".\pngconf.h"\
	".\zconf.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_PNGRE=\
	".\alloc.h"\
	

"$(INTDIR)\pngread.obj" : $(SOURCE) $(DEP_CPP_PNGRE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "libpng - Win32 Debug"

DEP_CPP_PNGRE=\
	"..\..\..\..\include\zlibdll.h"\
	".\png.h"\
	".\pngconf.h"\
	".\zconf.h"\
	

"$(INTDIR)\pngread.obj" : $(SOURCE) $(DEP_CPP_PNGRE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "libpng - Win32 Hybrid"

DEP_CPP_PNGRE=\
	"..\..\..\..\include\zlibdll.h"\
	".\png.h"\
	".\pngconf.h"\
	".\zconf.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_PNGRE=\
	".\alloc.h"\
	

"$(INTDIR)\pngread.obj" : $(SOURCE) $(DEP_CPP_PNGRE) "$(INTDIR)"


!ENDIF 

SOURCE=.\pngrio.cpp

!IF  "$(CFG)" == "libpng - Win32 Release"

DEP_CPP_PNGRI=\
	"..\..\..\..\include\zlibdll.h"\
	".\png.h"\
	".\pngconf.h"\
	".\zconf.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_PNGRI=\
	".\alloc.h"\
	

"$(INTDIR)\pngrio.obj" : $(SOURCE) $(DEP_CPP_PNGRI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "libpng - Win32 Debug"

DEP_CPP_PNGRI=\
	"..\..\..\..\include\zlibdll.h"\
	".\png.h"\
	".\pngconf.h"\
	".\zconf.h"\
	

"$(INTDIR)\pngrio.obj" : $(SOURCE) $(DEP_CPP_PNGRI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "libpng - Win32 Hybrid"

DEP_CPP_PNGRI=\
	"..\..\..\..\include\zlibdll.h"\
	".\png.h"\
	".\pngconf.h"\
	".\zconf.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_PNGRI=\
	".\alloc.h"\
	

"$(INTDIR)\pngrio.obj" : $(SOURCE) $(DEP_CPP_PNGRI) "$(INTDIR)"


!ENDIF 

SOURCE=.\pngrtran.cpp

!IF  "$(CFG)" == "libpng - Win32 Release"

DEP_CPP_PNGRT=\
	"..\..\..\..\include\zlibdll.h"\
	".\png.h"\
	".\pngconf.h"\
	".\zconf.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_PNGRT=\
	".\alloc.h"\
	

"$(INTDIR)\pngrtran.obj" : $(SOURCE) $(DEP_CPP_PNGRT) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "libpng - Win32 Debug"

DEP_CPP_PNGRT=\
	"..\..\..\..\include\zlibdll.h"\
	".\png.h"\
	".\pngconf.h"\
	".\zconf.h"\
	

"$(INTDIR)\pngrtran.obj" : $(SOURCE) $(DEP_CPP_PNGRT) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "libpng - Win32 Hybrid"

DEP_CPP_PNGRT=\
	"..\..\..\..\include\zlibdll.h"\
	".\png.h"\
	".\pngconf.h"\
	".\zconf.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_PNGRT=\
	".\alloc.h"\
	

"$(INTDIR)\pngrtran.obj" : $(SOURCE) $(DEP_CPP_PNGRT) "$(INTDIR)"


!ENDIF 

SOURCE=.\pngrutil.cpp

!IF  "$(CFG)" == "libpng - Win32 Release"

DEP_CPP_PNGRU=\
	"..\..\..\..\include\zlibdll.h"\
	".\png.h"\
	".\pngconf.h"\
	".\zconf.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_PNGRU=\
	".\alloc.h"\
	

"$(INTDIR)\pngrutil.obj" : $(SOURCE) $(DEP_CPP_PNGRU) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "libpng - Win32 Debug"

DEP_CPP_PNGRU=\
	"..\..\..\..\include\zlibdll.h"\
	".\png.h"\
	".\pngconf.h"\
	".\zconf.h"\
	

"$(INTDIR)\pngrutil.obj" : $(SOURCE) $(DEP_CPP_PNGRU) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "libpng - Win32 Hybrid"

DEP_CPP_PNGRU=\
	"..\..\..\..\include\zlibdll.h"\
	".\png.h"\
	".\pngconf.h"\
	".\zconf.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_PNGRU=\
	".\alloc.h"\
	

"$(INTDIR)\pngrutil.obj" : $(SOURCE) $(DEP_CPP_PNGRU) "$(INTDIR)"


!ENDIF 

SOURCE=.\pngset.cpp

!IF  "$(CFG)" == "libpng - Win32 Release"

DEP_CPP_PNGSE=\
	"..\..\..\..\include\zlibdll.h"\
	".\png.h"\
	".\pngconf.h"\
	".\zconf.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_PNGSE=\
	".\alloc.h"\
	

"$(INTDIR)\pngset.obj" : $(SOURCE) $(DEP_CPP_PNGSE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "libpng - Win32 Debug"

DEP_CPP_PNGSE=\
	"..\..\..\..\include\zlibdll.h"\
	".\png.h"\
	".\pngconf.h"\
	".\zconf.h"\
	

"$(INTDIR)\pngset.obj" : $(SOURCE) $(DEP_CPP_PNGSE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "libpng - Win32 Hybrid"

DEP_CPP_PNGSE=\
	"..\..\..\..\include\zlibdll.h"\
	".\png.h"\
	".\pngconf.h"\
	".\zconf.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_PNGSE=\
	".\alloc.h"\
	

"$(INTDIR)\pngset.obj" : $(SOURCE) $(DEP_CPP_PNGSE) "$(INTDIR)"


!ENDIF 

SOURCE=.\pngtrans.cpp

!IF  "$(CFG)" == "libpng - Win32 Release"

DEP_CPP_PNGTR=\
	"..\..\..\..\include\zlibdll.h"\
	".\png.h"\
	".\pngconf.h"\
	".\zconf.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_PNGTR=\
	".\alloc.h"\
	

"$(INTDIR)\pngtrans.obj" : $(SOURCE) $(DEP_CPP_PNGTR) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "libpng - Win32 Debug"

DEP_CPP_PNGTR=\
	"..\..\..\..\include\zlibdll.h"\
	".\png.h"\
	".\pngconf.h"\
	".\zconf.h"\
	

"$(INTDIR)\pngtrans.obj" : $(SOURCE) $(DEP_CPP_PNGTR) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "libpng - Win32 Hybrid"

DEP_CPP_PNGTR=\
	"..\..\..\..\include\zlibdll.h"\
	".\png.h"\
	".\pngconf.h"\
	".\zconf.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_PNGTR=\
	".\alloc.h"\
	

"$(INTDIR)\pngtrans.obj" : $(SOURCE) $(DEP_CPP_PNGTR) "$(INTDIR)"


!ENDIF 

SOURCE=.\pngwio.cpp

!IF  "$(CFG)" == "libpng - Win32 Release"

DEP_CPP_PNGWI=\
	"..\..\..\..\include\zlibdll.h"\
	".\png.h"\
	".\pngconf.h"\
	".\zconf.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_PNGWI=\
	".\alloc.h"\
	

"$(INTDIR)\pngwio.obj" : $(SOURCE) $(DEP_CPP_PNGWI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "libpng - Win32 Debug"

DEP_CPP_PNGWI=\
	"..\..\..\..\include\zlibdll.h"\
	".\png.h"\
	".\pngconf.h"\
	".\zconf.h"\
	

"$(INTDIR)\pngwio.obj" : $(SOURCE) $(DEP_CPP_PNGWI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "libpng - Win32 Hybrid"

DEP_CPP_PNGWI=\
	"..\..\..\..\include\zlibdll.h"\
	".\png.h"\
	".\pngconf.h"\
	".\zconf.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_PNGWI=\
	".\alloc.h"\
	

"$(INTDIR)\pngwio.obj" : $(SOURCE) $(DEP_CPP_PNGWI) "$(INTDIR)"


!ENDIF 

SOURCE=.\pngwrite.cpp

!IF  "$(CFG)" == "libpng - Win32 Release"

DEP_CPP_PNGWR=\
	"..\..\..\..\include\zlibdll.h"\
	".\png.h"\
	".\pngconf.h"\
	".\zconf.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_PNGWR=\
	".\alloc.h"\
	

"$(INTDIR)\pngwrite.obj" : $(SOURCE) $(DEP_CPP_PNGWR) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "libpng - Win32 Debug"

DEP_CPP_PNGWR=\
	"..\..\..\..\include\zlibdll.h"\
	".\png.h"\
	".\pngconf.h"\
	".\zconf.h"\
	

"$(INTDIR)\pngwrite.obj" : $(SOURCE) $(DEP_CPP_PNGWR) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "libpng - Win32 Hybrid"

DEP_CPP_PNGWR=\
	"..\..\..\..\include\zlibdll.h"\
	".\png.h"\
	".\pngconf.h"\
	".\zconf.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_PNGWR=\
	".\alloc.h"\
	

"$(INTDIR)\pngwrite.obj" : $(SOURCE) $(DEP_CPP_PNGWR) "$(INTDIR)"


!ENDIF 

SOURCE=.\pngwtran.cpp

!IF  "$(CFG)" == "libpng - Win32 Release"

DEP_CPP_PNGWT=\
	"..\..\..\..\include\zlibdll.h"\
	".\png.h"\
	".\pngconf.h"\
	".\zconf.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_PNGWT=\
	".\alloc.h"\
	

"$(INTDIR)\pngwtran.obj" : $(SOURCE) $(DEP_CPP_PNGWT) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "libpng - Win32 Debug"

DEP_CPP_PNGWT=\
	"..\..\..\..\include\zlibdll.h"\
	".\png.h"\
	".\pngconf.h"\
	".\zconf.h"\
	

"$(INTDIR)\pngwtran.obj" : $(SOURCE) $(DEP_CPP_PNGWT) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "libpng - Win32 Hybrid"

DEP_CPP_PNGWT=\
	"..\..\..\..\include\zlibdll.h"\
	".\png.h"\
	".\pngconf.h"\
	".\zconf.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_PNGWT=\
	".\alloc.h"\
	

"$(INTDIR)\pngwtran.obj" : $(SOURCE) $(DEP_CPP_PNGWT) "$(INTDIR)"


!ENDIF 

SOURCE=.\pngwutil.cpp

!IF  "$(CFG)" == "libpng - Win32 Release"

DEP_CPP_PNGWU=\
	"..\..\..\..\include\zlibdll.h"\
	".\png.h"\
	".\pngconf.h"\
	".\zconf.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_PNGWU=\
	".\alloc.h"\
	

"$(INTDIR)\pngwutil.obj" : $(SOURCE) $(DEP_CPP_PNGWU) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "libpng - Win32 Debug"

DEP_CPP_PNGWU=\
	"..\..\..\..\include\zlibdll.h"\
	".\png.h"\
	".\pngconf.h"\
	".\zconf.h"\
	

"$(INTDIR)\pngwutil.obj" : $(SOURCE) $(DEP_CPP_PNGWU) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "libpng - Win32 Hybrid"

DEP_CPP_PNGWU=\
	"..\..\..\..\include\zlibdll.h"\
	".\png.h"\
	".\pngconf.h"\
	".\zconf.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_PNGWU=\
	".\alloc.h"\
	

"$(INTDIR)\pngwutil.obj" : $(SOURCE) $(DEP_CPP_PNGWU) "$(INTDIR)"


!ENDIF 


!ENDIF 

