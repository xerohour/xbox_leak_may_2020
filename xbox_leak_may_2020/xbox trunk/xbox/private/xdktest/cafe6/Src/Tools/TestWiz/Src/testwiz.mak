# Microsoft Developer Studio Generated NMAKE File, Based on testwiz.bld
!IF "$(CFG)" == ""
CFG=testwiz - Win32 Release
!MESSAGE No configuration specified.  Defaulting to testwiz - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "testwiz - Win32 Release" && "$(CFG)" !=\
 "testwiz - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "testwiz.bld" CFG="testwiz - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "testwiz - Win32 Release" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "testwiz - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

RSC=rc.exe
F90=fl32.exe
CPP=cl.exe
MTL=mktyplib.exe

!IF  "$(CFG)" == "testwiz - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "$(OUTDIR)\testwiz.awx" "..\..\..\x86\bin\testwiz.awx"

CLEAN : 
	-@erase "$(INTDIR)\apitabdl.obj"
	-@erase "$(INTDIR)\Chooser.obj"
	-@erase "$(INTDIR)\Data.obj"
	-@erase "$(INTDIR)\Dummymen.obj"
	-@erase "$(INTDIR)\Owdlist.obj"
	-@erase "$(INTDIR)\page1dlg.obj"
	-@erase "$(INTDIR)\Scriptpr.obj"
	-@erase "$(INTDIR)\Scriptr.obj"
	-@erase "$(INTDIR)\Stdafx.obj"
	-@erase "$(INTDIR)\Tabcusto.obj"
	-@erase "$(INTDIR)\Testawx.obj"
	-@erase "$(INTDIR)\Testwiz.obj"
	-@erase "$(INTDIR)\testwiz.pch"
	-@erase "$(INTDIR)\testwiz.res"
	-@erase "$(INTDIR)\Treelist.obj"
	-@erase "$(OUTDIR)\testwiz.awx"
	-@erase "$(OUTDIR)\testwiz.exp"
	-@erase "$(OUTDIR)\testwiz.lib"
	-@erase "..\..\..\x86\bin\testwiz.awx"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

F90_PROJ=/Ox /I "Release/" /c /nologo /MD /Fo"Release/" 
F90_OBJS=.\Release/
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/testwiz.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/"\
 /c 
CPP_OBJS=.\Release/
CPP_SBRS=.\.
MTL_PROJ=/nologo /D "NDEBUG" /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/testwiz.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)/testwiz.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/testwiz.pdb" /machine:$(PROCESSOR_ARCHITECTURE) /out:"$(OUTDIR)/testwiz.awx"\
 /implib:"$(OUTDIR)/testwiz.lib" 
LINK32_OBJS= \
	"$(INTDIR)\apitabdl.obj" \
	"$(INTDIR)\Chooser.obj" \
	"$(INTDIR)\Data.obj" \
	"$(INTDIR)\Dummymen.obj" \
	"$(INTDIR)\Owdlist.obj" \
	"$(INTDIR)\page1dlg.obj" \
	"$(INTDIR)\Scriptpr.obj" \
	"$(INTDIR)\Scriptr.obj" \
	"$(INTDIR)\Stdafx.obj" \
	"$(INTDIR)\Tabcusto.obj" \
	"$(INTDIR)\Testawx.obj" \
	"$(INTDIR)\Testwiz.obj" \
	"$(INTDIR)\testwiz.res" \
	"$(INTDIR)\Treelist.obj"

"$(OUTDIR)\testwiz.awx" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

OutDir=.\Release
InputPath=.\Release\testwiz.awx
SOURCE=$(InputPath)

".\..\..\..\x86\bin\testwiz.awx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   echo f | xcopy /r $(OutDir)\testwiz.awx  .\..\..\..\x86\bin\testwiz.awx

!ELSEIF  "$(CFG)" == "testwiz - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "$(OUTDIR)\testwiz.awx"

CLEAN : 
	-@erase "$(INTDIR)\apitabdl.obj"
	-@erase "$(INTDIR)\Chooser.obj"
	-@erase "$(INTDIR)\Data.obj"
	-@erase "$(INTDIR)\Dummymen.obj"
	-@erase "$(INTDIR)\Owdlist.obj"
	-@erase "$(INTDIR)\page1dlg.obj"
	-@erase "$(INTDIR)\Scriptpr.obj"
	-@erase "$(INTDIR)\Scriptr.obj"
	-@erase "$(INTDIR)\Stdafx.obj"
	-@erase "$(INTDIR)\Tabcusto.obj"
	-@erase "$(INTDIR)\Testawx.obj"
	-@erase "$(INTDIR)\Testwiz.obj"
	-@erase "$(INTDIR)\testwiz.pch"
	-@erase "$(INTDIR)\testwiz.res"
	-@erase "$(INTDIR)\Treelist.obj"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\testwiz.awx"
	-@erase "$(OUTDIR)\testwiz.exp"
	-@erase "$(OUTDIR)\testwiz.ilk"
	-@erase "$(OUTDIR)\testwiz.lib"
	-@erase "$(OUTDIR)\testwiz.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

F90_PROJ=/Zi /I "Debug/" /c /nologo /MD /Fo"Debug/" /Fd"Debug/testwiz.pdb" 
F90_OBJS=.\Debug/
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_PSEUDO_DEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/testwiz.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\.
MTL_PROJ=/nologo /D "_DEBUG" /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/testwiz.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)/testwiz.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/testwiz.pdb" /debug /machine:$(PROCESSOR_ARCHITECTURE) /out:"$(OUTDIR)/testwiz.awx"\
 /implib:"$(OUTDIR)/testwiz.lib" 
LINK32_OBJS= \
	"$(INTDIR)\apitabdl.obj" \
	"$(INTDIR)\Chooser.obj" \
	"$(INTDIR)\Data.obj" \
	"$(INTDIR)\Dummymen.obj" \
	"$(INTDIR)\Owdlist.obj" \
	"$(INTDIR)\page1dlg.obj" \
	"$(INTDIR)\Scriptpr.obj" \
	"$(INTDIR)\Scriptr.obj" \
	"$(INTDIR)\Stdafx.obj" \
	"$(INTDIR)\Tabcusto.obj" \
	"$(INTDIR)\Testawx.obj" \
	"$(INTDIR)\Testwiz.obj" \
	"$(INTDIR)\testwiz.res" \
	"$(INTDIR)\Treelist.obj"

"$(OUTDIR)\testwiz.awx" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.for{$(F90_OBJS)}.obj:
   $(F90) $(F90_PROJ) $<  

.f{$(F90_OBJS)}.obj:
   $(F90) $(F90_PROJ) $<  

.f90{$(F90_OBJS)}.obj:
   $(F90) $(F90_PROJ) $<  

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


!IF "$(CFG)" == "testwiz - Win32 Release" || "$(CFG)" ==\
 "testwiz - Win32 Debug"

!IF  "$(CFG)" == "testwiz - Win32 Release"

!ELSEIF  "$(CFG)" == "testwiz - Win32 Debug"

!ENDIF 

SOURCE=.\testwiz.rc
DEP_RSC_TESTW=\
	".\Res\Bitmap1.bmp"\
	".\Res\Bmp00001.bmp"\
	".\Res\Bmp00002.bmp"\
	".\Res\Bmp00003.bmp"\
	".\Res\Bmp00004.bmp"\
	".\Res\Bmp00005.bmp"\
	".\Res\Cursor1.cur"\
	".\Res\Icon1.ico"\
	".\Res\Icon2.ico"\
	".\res\test5.ico"\
	".\Res\Testwiz.rc2"\
	".\Res\Triangle.bmp"\
	".\template\clwcases.cpp"\
	".\template\clwcases.h"\
	".\template\clwsnif.cpp"\
	".\template\clwsnif.h"\
	".\template\confirm.inf"\
	".\template\debug.stf"\
	".\template\newproj.inf"\
	".\template\release.stf"\
	".\template\stdafx.cpp"\
	".\template\stdafx.h"\
	

"$(INTDIR)\testwiz.res" : $(SOURCE) $(DEP_RSC_TESTW) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\apitabdl.cpp
DEP_CPP_APITA=\
	".\Apitabdl.h"\
	".\Data.h"\
	".\Owdlist.h"\
	".\Scriptr.h"\
	".\Stdafx.h"\
	".\Testawx.h"\
	".\Testwiz.h"\
	".\Treelist.h"\
	{$(INCLUDE)}"\Customaw.h"\
	

"$(INTDIR)\apitabdl.obj" : $(SOURCE) $(DEP_CPP_APITA) "$(INTDIR)"\
 "$(INTDIR)\testwiz.pch"


SOURCE=.\Chooser.cpp
DEP_CPP_CHOOS=\
	".\Chooser.h"\
	".\page1dlg.h"\
	".\Stdafx.h"\
	".\Testawx.h"\
	".\Testwiz.h"\
	{$(INCLUDE)}"\Customaw.h"\
	

"$(INTDIR)\Chooser.obj" : $(SOURCE) $(DEP_CPP_CHOOS) "$(INTDIR)"\
 "$(INTDIR)\testwiz.pch"


SOURCE=.\Data.cpp
DEP_CPP_DATA_=\
	".\Data.h"\
	".\Stdafx.h"\
	".\Testawx.h"\
	".\Testwiz.h"\
	{$(INCLUDE)}"\Customaw.h"\
	

"$(INTDIR)\Data.obj" : $(SOURCE) $(DEP_CPP_DATA_) "$(INTDIR)"\
 "$(INTDIR)\testwiz.pch"


SOURCE=.\Dummymen.cpp
DEP_CPP_DUMMY=\
	".\Dummymen.h"\
	".\Stdafx.h"\
	".\Testawx.h"\
	{$(INCLUDE)}"\Customaw.h"\
	

"$(INTDIR)\Dummymen.obj" : $(SOURCE) $(DEP_CPP_DUMMY) "$(INTDIR)"\
 "$(INTDIR)\testwiz.pch"


SOURCE=.\Owdlist.cpp
DEP_CPP_OWDLI=\
	".\Owdlist.h"\
	".\Scriptr.h"\
	".\Stdafx.h"\
	".\Testawx.h"\
	{$(INCLUDE)}"\Customaw.h"\
	

"$(INTDIR)\Owdlist.obj" : $(SOURCE) $(DEP_CPP_OWDLI) "$(INTDIR)"\
 "$(INTDIR)\testwiz.pch"


SOURCE=.\page1dlg.cpp
DEP_CPP_PAGE1=\
	".\Apitabdl.h"\
	".\Data.h"\
	".\Owdlist.h"\
	".\page1dlg.h"\
	".\Scriptpr.h"\
	".\Scriptr.h"\
	".\Stdafx.h"\
	".\Tabcusto.h"\
	".\Testawx.h"\
	".\Testwiz.h"\
	".\Treelist.h"\
	{$(INCLUDE)}"\Customaw.h"\
	

"$(INTDIR)\page1dlg.obj" : $(SOURCE) $(DEP_CPP_PAGE1) "$(INTDIR)"\
 "$(INTDIR)\testwiz.pch"


SOURCE=.\Scriptpr.cpp
DEP_CPP_SCRIP=\
	".\Scriptpr.h"\
	".\Stdafx.h"\
	".\Testawx.h"\
	".\Testwiz.h"\
	{$(INCLUDE)}"\Customaw.h"\
	

"$(INTDIR)\Scriptpr.obj" : $(SOURCE) $(DEP_CPP_SCRIP) "$(INTDIR)"\
 "$(INTDIR)\testwiz.pch"


SOURCE=.\Scriptr.cpp
DEP_CPP_SCRIPT=\
	".\Scriptr.h"\
	".\Stdafx.h"\
	".\Testawx.h"\
	{$(INCLUDE)}"\Customaw.h"\
	

"$(INTDIR)\Scriptr.obj" : $(SOURCE) $(DEP_CPP_SCRIPT) "$(INTDIR)"\
 "$(INTDIR)\testwiz.pch"


SOURCE=.\Stdafx.cpp
DEP_CPP_STDAF=\
	".\Stdafx.h"\
	".\Testawx.h"\
	{$(INCLUDE)}"\Customaw.h"\
	

!IF  "$(CFG)" == "testwiz - Win32 Release"


BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/testwiz.pch" /Yc /Fo"$(INTDIR)/" /c\
 $(SOURCE) \
	

"$(INTDIR)\Stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\testwiz.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "testwiz - Win32 Debug"


BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_PSEUDO_DEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/testwiz.pch" /Yc\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\Stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\testwiz.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

SOURCE=.\Tabcusto.cpp
DEP_CPP_TABCU=\
	".\Stdafx.h"\
	".\Tabcusto.h"\
	".\Testawx.h"\
	".\Testwiz.h"\
	{$(INCLUDE)}"\Customaw.h"\
	

"$(INTDIR)\Tabcusto.obj" : $(SOURCE) $(DEP_CPP_TABCU) "$(INTDIR)"\
 "$(INTDIR)\testwiz.pch"


SOURCE=.\Testawx.cpp
DEP_CPP_TESTA=\
	".\Chooser.h"\
	".\Stdafx.h"\
	".\Testawx.h"\
	".\Testwiz.h"\
	{$(INCLUDE)}"\Customaw.h"\
	

"$(INTDIR)\Testawx.obj" : $(SOURCE) $(DEP_CPP_TESTA) "$(INTDIR)"\
 "$(INTDIR)\testwiz.pch"


SOURCE=.\Testwiz.cpp
DEP_CPP_TESTWI=\
	".\Chooser.h"\
	".\Stdafx.h"\
	".\Testawx.h"\
	".\Testwiz.h"\
	{$(INCLUDE)}"\Customaw.h"\
	

"$(INTDIR)\Testwiz.obj" : $(SOURCE) $(DEP_CPP_TESTWI) "$(INTDIR)"\
 "$(INTDIR)\testwiz.pch"


SOURCE=.\Treelist.cpp
DEP_CPP_TREEL=\
	".\Stdafx.h"\
	".\Testawx.h"\
	".\Treelist.h"\
	{$(INCLUDE)}"\Customaw.h"\
	

"$(INTDIR)\Treelist.obj" : $(SOURCE) $(DEP_CPP_TREEL) "$(INTDIR)"\
 "$(INTDIR)\testwiz.pch"



!ENDIF 

