# Microsoft Developer Studio Generated NMAKE File, Based on Jpeg.dsp
!IF "$(CFG)" == ""
CFG=jpeg - Win32 Release
!MESSAGE No configuration specified. Defaulting to jpeg - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "jpeg - Win32 Release" && "$(CFG)" != "jpeg - Win32 Debug" &&\
 "$(CFG)" != "jpeg - Win32 Hybrid"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Jpeg.mak" CFG="jpeg - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "jpeg - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "jpeg - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "jpeg - Win32 Hybrid" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "jpeg - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

!IF "$(RECURSE)" == "0" 

ALL : "..\..\..\..\maxsdk\plugin\jpeg.bmi"

!ELSE 

ALL : "..\..\..\..\maxsdk\plugin\jpeg.bmi"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\interfce.obj"
	-@erase "$(INTDIR)\jcapimin.obj"
	-@erase "$(INTDIR)\jcapistd.obj"
	-@erase "$(INTDIR)\jccoefct.obj"
	-@erase "$(INTDIR)\jccolor.obj"
	-@erase "$(INTDIR)\jcdctmgr.obj"
	-@erase "$(INTDIR)\jchuff.obj"
	-@erase "$(INTDIR)\jcinit.obj"
	-@erase "$(INTDIR)\jcmainct.obj"
	-@erase "$(INTDIR)\jcmarker.obj"
	-@erase "$(INTDIR)\jcmaster.obj"
	-@erase "$(INTDIR)\jcomapi.obj"
	-@erase "$(INTDIR)\jcparam.obj"
	-@erase "$(INTDIR)\jcphuff.obj"
	-@erase "$(INTDIR)\jcprepct.obj"
	-@erase "$(INTDIR)\jcsample.obj"
	-@erase "$(INTDIR)\jctrans.obj"
	-@erase "$(INTDIR)\jdapimin.obj"
	-@erase "$(INTDIR)\jdapistd.obj"
	-@erase "$(INTDIR)\jdatadst.obj"
	-@erase "$(INTDIR)\jdatasrc.obj"
	-@erase "$(INTDIR)\jdcoefct.obj"
	-@erase "$(INTDIR)\jdcolor.obj"
	-@erase "$(INTDIR)\jddctmgr.obj"
	-@erase "$(INTDIR)\jdhuff.obj"
	-@erase "$(INTDIR)\jdinput.obj"
	-@erase "$(INTDIR)\jdmainct.obj"
	-@erase "$(INTDIR)\jdmarker.obj"
	-@erase "$(INTDIR)\jdmaster.obj"
	-@erase "$(INTDIR)\jdmerge.obj"
	-@erase "$(INTDIR)\jdphuff.obj"
	-@erase "$(INTDIR)\jdpostct.obj"
	-@erase "$(INTDIR)\jdsample.obj"
	-@erase "$(INTDIR)\jdtrans.obj"
	-@erase "$(INTDIR)\jerror.obj"
	-@erase "$(INTDIR)\jfdctflt.obj"
	-@erase "$(INTDIR)\jfdctfst.obj"
	-@erase "$(INTDIR)\jfdctint.obj"
	-@erase "$(INTDIR)\jidctflt.obj"
	-@erase "$(INTDIR)\jidctfst.obj"
	-@erase "$(INTDIR)\jidctint.obj"
	-@erase "$(INTDIR)\jidctred.obj"
	-@erase "$(INTDIR)\jmemmgr.obj"
	-@erase "$(INTDIR)\jmemnobs.obj"
	-@erase "$(INTDIR)\jpeg.obj"
	-@erase "$(INTDIR)\jpeg.res"
	-@erase "$(INTDIR)\jquant1.obj"
	-@erase "$(INTDIR)\jquant2.obj"
	-@erase "$(INTDIR)\jutils.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(OUTDIR)\jpeg.exp"
	-@erase "$(OUTDIR)\jpeg.lib"
	-@erase "..\..\..\..\maxsdk\plugin\jpeg.bmi"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MD /W3 /O2 /I "..\..\..\Include" /D "NDEBUG" /D "WIN32"\
 /D "_WINDOWS" /Fp"$(INTDIR)\Jpeg.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD\
 /c 
CPP_OBJS=.\Release/
CPP_SBRS=.
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\jpeg.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Jpeg.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib\
 comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo\
 /release /base:"0x2c1e0000" /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)\jpeg.pdb" /machine:I386 /def:".\jpeg.def"\
 /out:"..\..\..\..\maxsdk\plugin\jpeg.bmi" /implib:"$(OUTDIR)\jpeg.lib" 
DEF_FILE= \
	".\jpeg.def"
LINK32_OBJS= \
	"$(INTDIR)\interfce.obj" \
	"$(INTDIR)\jcapimin.obj" \
	"$(INTDIR)\jcapistd.obj" \
	"$(INTDIR)\jccoefct.obj" \
	"$(INTDIR)\jccolor.obj" \
	"$(INTDIR)\jcdctmgr.obj" \
	"$(INTDIR)\jchuff.obj" \
	"$(INTDIR)\jcinit.obj" \
	"$(INTDIR)\jcmainct.obj" \
	"$(INTDIR)\jcmarker.obj" \
	"$(INTDIR)\jcmaster.obj" \
	"$(INTDIR)\jcomapi.obj" \
	"$(INTDIR)\jcparam.obj" \
	"$(INTDIR)\jcphuff.obj" \
	"$(INTDIR)\jcprepct.obj" \
	"$(INTDIR)\jcsample.obj" \
	"$(INTDIR)\jctrans.obj" \
	"$(INTDIR)\jdapimin.obj" \
	"$(INTDIR)\jdapistd.obj" \
	"$(INTDIR)\jdatadst.obj" \
	"$(INTDIR)\jdatasrc.obj" \
	"$(INTDIR)\jdcoefct.obj" \
	"$(INTDIR)\jdcolor.obj" \
	"$(INTDIR)\jddctmgr.obj" \
	"$(INTDIR)\jdhuff.obj" \
	"$(INTDIR)\jdinput.obj" \
	"$(INTDIR)\jdmainct.obj" \
	"$(INTDIR)\jdmarker.obj" \
	"$(INTDIR)\jdmaster.obj" \
	"$(INTDIR)\jdmerge.obj" \
	"$(INTDIR)\jdphuff.obj" \
	"$(INTDIR)\jdpostct.obj" \
	"$(INTDIR)\jdsample.obj" \
	"$(INTDIR)\jdtrans.obj" \
	"$(INTDIR)\jerror.obj" \
	"$(INTDIR)\jfdctflt.obj" \
	"$(INTDIR)\jfdctfst.obj" \
	"$(INTDIR)\jfdctint.obj" \
	"$(INTDIR)\jidctflt.obj" \
	"$(INTDIR)\jidctfst.obj" \
	"$(INTDIR)\jidctint.obj" \
	"$(INTDIR)\jidctred.obj" \
	"$(INTDIR)\jmemmgr.obj" \
	"$(INTDIR)\jmemnobs.obj" \
	"$(INTDIR)\jpeg.obj" \
	"$(INTDIR)\jpeg.res" \
	"$(INTDIR)\jquant1.obj" \
	"$(INTDIR)\jquant2.obj" \
	"$(INTDIR)\jutils.obj" \
	"..\..\..\Lib\BMM.lib" \
	"..\..\..\Lib\core.lib" \
	"..\..\..\Lib\maxutil.lib"

"..\..\..\..\maxsdk\plugin\jpeg.bmi" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "jpeg - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

!IF "$(RECURSE)" == "0" 

ALL : "..\..\..\..\maxsdk\plugin\jpeg.bmi"

!ELSE 

ALL : "..\..\..\..\maxsdk\plugin\jpeg.bmi"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\interfce.obj"
	-@erase "$(INTDIR)\jcapimin.obj"
	-@erase "$(INTDIR)\jcapistd.obj"
	-@erase "$(INTDIR)\jccoefct.obj"
	-@erase "$(INTDIR)\jccolor.obj"
	-@erase "$(INTDIR)\jcdctmgr.obj"
	-@erase "$(INTDIR)\jchuff.obj"
	-@erase "$(INTDIR)\jcinit.obj"
	-@erase "$(INTDIR)\jcmainct.obj"
	-@erase "$(INTDIR)\jcmarker.obj"
	-@erase "$(INTDIR)\jcmaster.obj"
	-@erase "$(INTDIR)\jcomapi.obj"
	-@erase "$(INTDIR)\jcparam.obj"
	-@erase "$(INTDIR)\jcphuff.obj"
	-@erase "$(INTDIR)\jcprepct.obj"
	-@erase "$(INTDIR)\jcsample.obj"
	-@erase "$(INTDIR)\jctrans.obj"
	-@erase "$(INTDIR)\jdapimin.obj"
	-@erase "$(INTDIR)\jdapistd.obj"
	-@erase "$(INTDIR)\jdatadst.obj"
	-@erase "$(INTDIR)\jdatasrc.obj"
	-@erase "$(INTDIR)\jdcoefct.obj"
	-@erase "$(INTDIR)\jdcolor.obj"
	-@erase "$(INTDIR)\jddctmgr.obj"
	-@erase "$(INTDIR)\jdhuff.obj"
	-@erase "$(INTDIR)\jdinput.obj"
	-@erase "$(INTDIR)\jdmainct.obj"
	-@erase "$(INTDIR)\jdmarker.obj"
	-@erase "$(INTDIR)\jdmaster.obj"
	-@erase "$(INTDIR)\jdmerge.obj"
	-@erase "$(INTDIR)\jdphuff.obj"
	-@erase "$(INTDIR)\jdpostct.obj"
	-@erase "$(INTDIR)\jdsample.obj"
	-@erase "$(INTDIR)\jdtrans.obj"
	-@erase "$(INTDIR)\jerror.obj"
	-@erase "$(INTDIR)\jfdctflt.obj"
	-@erase "$(INTDIR)\jfdctfst.obj"
	-@erase "$(INTDIR)\jfdctint.obj"
	-@erase "$(INTDIR)\jidctflt.obj"
	-@erase "$(INTDIR)\jidctfst.obj"
	-@erase "$(INTDIR)\jidctint.obj"
	-@erase "$(INTDIR)\jidctred.obj"
	-@erase "$(INTDIR)\jmemmgr.obj"
	-@erase "$(INTDIR)\jmemnobs.obj"
	-@erase "$(INTDIR)\jpeg.obj"
	-@erase "$(INTDIR)\jpeg.res"
	-@erase "$(INTDIR)\jquant1.obj"
	-@erase "$(INTDIR)\jquant2.obj"
	-@erase "$(INTDIR)\jutils.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\vc50.pdb"
	-@erase "$(OUTDIR)\jpeg.exp"
	-@erase "$(OUTDIR)\jpeg.lib"
	-@erase "$(OUTDIR)\jpeg.pdb"
	-@erase "..\..\..\..\maxsdk\plugin\jpeg.bmi"
	-@erase "..\..\..\..\maxsdk\plugin\jpeg.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MDd /W3 /Gm /Zi /Od /I "..\..\..\Include" /D "_DEBUG" /D\
 "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\Jpeg.pch" /YX /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\jpeg.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Jpeg.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib\
 comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo\
 /base:"0x2c1e0000" /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)\jpeg.pdb" /debug /machine:I386 /def:".\jpeg.def"\
 /out:"..\..\..\..\maxsdk\plugin\jpeg.bmi" /implib:"$(OUTDIR)\jpeg.lib" 
DEF_FILE= \
	".\jpeg.def"
LINK32_OBJS= \
	"$(INTDIR)\interfce.obj" \
	"$(INTDIR)\jcapimin.obj" \
	"$(INTDIR)\jcapistd.obj" \
	"$(INTDIR)\jccoefct.obj" \
	"$(INTDIR)\jccolor.obj" \
	"$(INTDIR)\jcdctmgr.obj" \
	"$(INTDIR)\jchuff.obj" \
	"$(INTDIR)\jcinit.obj" \
	"$(INTDIR)\jcmainct.obj" \
	"$(INTDIR)\jcmarker.obj" \
	"$(INTDIR)\jcmaster.obj" \
	"$(INTDIR)\jcomapi.obj" \
	"$(INTDIR)\jcparam.obj" \
	"$(INTDIR)\jcphuff.obj" \
	"$(INTDIR)\jcprepct.obj" \
	"$(INTDIR)\jcsample.obj" \
	"$(INTDIR)\jctrans.obj" \
	"$(INTDIR)\jdapimin.obj" \
	"$(INTDIR)\jdapistd.obj" \
	"$(INTDIR)\jdatadst.obj" \
	"$(INTDIR)\jdatasrc.obj" \
	"$(INTDIR)\jdcoefct.obj" \
	"$(INTDIR)\jdcolor.obj" \
	"$(INTDIR)\jddctmgr.obj" \
	"$(INTDIR)\jdhuff.obj" \
	"$(INTDIR)\jdinput.obj" \
	"$(INTDIR)\jdmainct.obj" \
	"$(INTDIR)\jdmarker.obj" \
	"$(INTDIR)\jdmaster.obj" \
	"$(INTDIR)\jdmerge.obj" \
	"$(INTDIR)\jdphuff.obj" \
	"$(INTDIR)\jdpostct.obj" \
	"$(INTDIR)\jdsample.obj" \
	"$(INTDIR)\jdtrans.obj" \
	"$(INTDIR)\jerror.obj" \
	"$(INTDIR)\jfdctflt.obj" \
	"$(INTDIR)\jfdctfst.obj" \
	"$(INTDIR)\jfdctint.obj" \
	"$(INTDIR)\jidctflt.obj" \
	"$(INTDIR)\jidctfst.obj" \
	"$(INTDIR)\jidctint.obj" \
	"$(INTDIR)\jidctred.obj" \
	"$(INTDIR)\jmemmgr.obj" \
	"$(INTDIR)\jmemnobs.obj" \
	"$(INTDIR)\jpeg.obj" \
	"$(INTDIR)\jpeg.res" \
	"$(INTDIR)\jquant1.obj" \
	"$(INTDIR)\jquant2.obj" \
	"$(INTDIR)\jutils.obj" \
	"..\..\..\Lib\BMM.lib" \
	"..\..\..\Lib\core.lib" \
	"..\..\..\Lib\maxutil.lib"

"..\..\..\..\maxsdk\plugin\jpeg.bmi" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "jpeg - Win32 Hybrid"

OUTDIR=.\Hybrid
INTDIR=.\Hybrid

!IF "$(RECURSE)" == "0" 

ALL : "..\..\..\..\maxsdk\plugin\jpeg.bmi"

!ELSE 

ALL : "..\..\..\..\maxsdk\plugin\jpeg.bmi"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\interfce.obj"
	-@erase "$(INTDIR)\jcapimin.obj"
	-@erase "$(INTDIR)\jcapistd.obj"
	-@erase "$(INTDIR)\jccoefct.obj"
	-@erase "$(INTDIR)\jccolor.obj"
	-@erase "$(INTDIR)\jcdctmgr.obj"
	-@erase "$(INTDIR)\jchuff.obj"
	-@erase "$(INTDIR)\jcinit.obj"
	-@erase "$(INTDIR)\jcmainct.obj"
	-@erase "$(INTDIR)\jcmarker.obj"
	-@erase "$(INTDIR)\jcmaster.obj"
	-@erase "$(INTDIR)\jcomapi.obj"
	-@erase "$(INTDIR)\jcparam.obj"
	-@erase "$(INTDIR)\jcphuff.obj"
	-@erase "$(INTDIR)\jcprepct.obj"
	-@erase "$(INTDIR)\jcsample.obj"
	-@erase "$(INTDIR)\jctrans.obj"
	-@erase "$(INTDIR)\jdapimin.obj"
	-@erase "$(INTDIR)\jdapistd.obj"
	-@erase "$(INTDIR)\jdatadst.obj"
	-@erase "$(INTDIR)\jdatasrc.obj"
	-@erase "$(INTDIR)\jdcoefct.obj"
	-@erase "$(INTDIR)\jdcolor.obj"
	-@erase "$(INTDIR)\jddctmgr.obj"
	-@erase "$(INTDIR)\jdhuff.obj"
	-@erase "$(INTDIR)\jdinput.obj"
	-@erase "$(INTDIR)\jdmainct.obj"
	-@erase "$(INTDIR)\jdmarker.obj"
	-@erase "$(INTDIR)\jdmaster.obj"
	-@erase "$(INTDIR)\jdmerge.obj"
	-@erase "$(INTDIR)\jdphuff.obj"
	-@erase "$(INTDIR)\jdpostct.obj"
	-@erase "$(INTDIR)\jdsample.obj"
	-@erase "$(INTDIR)\jdtrans.obj"
	-@erase "$(INTDIR)\jerror.obj"
	-@erase "$(INTDIR)\jfdctflt.obj"
	-@erase "$(INTDIR)\jfdctfst.obj"
	-@erase "$(INTDIR)\jfdctint.obj"
	-@erase "$(INTDIR)\jidctflt.obj"
	-@erase "$(INTDIR)\jidctfst.obj"
	-@erase "$(INTDIR)\jidctint.obj"
	-@erase "$(INTDIR)\jidctred.obj"
	-@erase "$(INTDIR)\jmemmgr.obj"
	-@erase "$(INTDIR)\jmemnobs.obj"
	-@erase "$(INTDIR)\jpeg.obj"
	-@erase "$(INTDIR)\jpeg.res"
	-@erase "$(INTDIR)\jquant1.obj"
	-@erase "$(INTDIR)\jquant2.obj"
	-@erase "$(INTDIR)\jutils.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\vc50.pdb"
	-@erase "$(OUTDIR)\jpeg.exp"
	-@erase "$(OUTDIR)\jpeg.lib"
	-@erase "$(OUTDIR)\jpeg.pdb"
	-@erase "..\..\..\..\maxsdk\plugin\jpeg.bmi"
	-@erase "..\..\..\..\maxsdk\plugin\jpeg.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MD /W3 /Gm /Zi /Od /I "..\..\..\Include" /D "_DEBUG" /D\
 "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\Jpeg.pch" /YX /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Hybrid/
CPP_SBRS=.
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\jpeg.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Jpeg.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib\
 comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo\
 /base:"0x2c1e0000" /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)\jpeg.pdb" /debug /machine:I386 /def:".\jpeg.def"\
 /out:"..\..\..\..\maxsdk\plugin\jpeg.bmi" /implib:"$(OUTDIR)\jpeg.lib" 
DEF_FILE= \
	".\jpeg.def"
LINK32_OBJS= \
	"$(INTDIR)\interfce.obj" \
	"$(INTDIR)\jcapimin.obj" \
	"$(INTDIR)\jcapistd.obj" \
	"$(INTDIR)\jccoefct.obj" \
	"$(INTDIR)\jccolor.obj" \
	"$(INTDIR)\jcdctmgr.obj" \
	"$(INTDIR)\jchuff.obj" \
	"$(INTDIR)\jcinit.obj" \
	"$(INTDIR)\jcmainct.obj" \
	"$(INTDIR)\jcmarker.obj" \
	"$(INTDIR)\jcmaster.obj" \
	"$(INTDIR)\jcomapi.obj" \
	"$(INTDIR)\jcparam.obj" \
	"$(INTDIR)\jcphuff.obj" \
	"$(INTDIR)\jcprepct.obj" \
	"$(INTDIR)\jcsample.obj" \
	"$(INTDIR)\jctrans.obj" \
	"$(INTDIR)\jdapimin.obj" \
	"$(INTDIR)\jdapistd.obj" \
	"$(INTDIR)\jdatadst.obj" \
	"$(INTDIR)\jdatasrc.obj" \
	"$(INTDIR)\jdcoefct.obj" \
	"$(INTDIR)\jdcolor.obj" \
	"$(INTDIR)\jddctmgr.obj" \
	"$(INTDIR)\jdhuff.obj" \
	"$(INTDIR)\jdinput.obj" \
	"$(INTDIR)\jdmainct.obj" \
	"$(INTDIR)\jdmarker.obj" \
	"$(INTDIR)\jdmaster.obj" \
	"$(INTDIR)\jdmerge.obj" \
	"$(INTDIR)\jdphuff.obj" \
	"$(INTDIR)\jdpostct.obj" \
	"$(INTDIR)\jdsample.obj" \
	"$(INTDIR)\jdtrans.obj" \
	"$(INTDIR)\jerror.obj" \
	"$(INTDIR)\jfdctflt.obj" \
	"$(INTDIR)\jfdctfst.obj" \
	"$(INTDIR)\jfdctint.obj" \
	"$(INTDIR)\jidctflt.obj" \
	"$(INTDIR)\jidctfst.obj" \
	"$(INTDIR)\jidctint.obj" \
	"$(INTDIR)\jidctred.obj" \
	"$(INTDIR)\jmemmgr.obj" \
	"$(INTDIR)\jmemnobs.obj" \
	"$(INTDIR)\jpeg.obj" \
	"$(INTDIR)\jpeg.res" \
	"$(INTDIR)\jquant1.obj" \
	"$(INTDIR)\jquant2.obj" \
	"$(INTDIR)\jutils.obj" \
	"..\..\..\Lib\BMM.lib" \
	"..\..\..\Lib\core.lib" \
	"..\..\..\Lib\maxutil.lib"

"..\..\..\..\maxsdk\plugin\jpeg.bmi" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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


!IF "$(CFG)" == "jpeg - Win32 Release" || "$(CFG)" == "jpeg - Win32 Debug" ||\
 "$(CFG)" == "jpeg - Win32 Hybrid"
SOURCE=.\interfce.c

!IF  "$(CFG)" == "jpeg - Win32 Release"

DEP_CPP_INTER=\
	".\interfce.h"\
	".\jconfig.h"\
	".\jerror.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	

"$(INTDIR)\interfce.obj" : $(SOURCE) $(DEP_CPP_INTER) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeg - Win32 Debug"

DEP_CPP_INTER=\
	".\interfce.h"\
	".\jconfig.h"\
	".\jerror.h"\
	".\jmorecfg.h"\
	".\jpeglib.h"\
	

"$(INTDIR)\interfce.obj" : $(SOURCE) $(DEP_CPP_INTER) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeg - Win32 Hybrid"

DEP_CPP_INTER=\
	".\interfce.h"\
	".\jconfig.h"\
	".\jerror.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	

"$(INTDIR)\interfce.obj" : $(SOURCE) $(DEP_CPP_INTER) "$(INTDIR)"


!ENDIF 

SOURCE=.\jcapimin.c

!IF  "$(CFG)" == "jpeg - Win32 Release"

DEP_CPP_JCAPI=\
	".\jconfig.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\jcapimin.obj" : $(SOURCE) $(DEP_CPP_JCAPI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeg - Win32 Debug"

DEP_CPP_JCAPI=\
	".\jconfig.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	

"$(INTDIR)\jcapimin.obj" : $(SOURCE) $(DEP_CPP_JCAPI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeg - Win32 Hybrid"

DEP_CPP_JCAPI=\
	".\jconfig.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\jcapimin.obj" : $(SOURCE) $(DEP_CPP_JCAPI) "$(INTDIR)"


!ENDIF 

SOURCE=.\jcapistd.c

!IF  "$(CFG)" == "jpeg - Win32 Release"

DEP_CPP_JCAPIS=\
	".\jconfig.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\jcapistd.obj" : $(SOURCE) $(DEP_CPP_JCAPIS) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeg - Win32 Debug"

DEP_CPP_JCAPIS=\
	".\jconfig.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	

"$(INTDIR)\jcapistd.obj" : $(SOURCE) $(DEP_CPP_JCAPIS) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeg - Win32 Hybrid"

DEP_CPP_JCAPIS=\
	".\jconfig.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\jcapistd.obj" : $(SOURCE) $(DEP_CPP_JCAPIS) "$(INTDIR)"


!ENDIF 

SOURCE=.\jccoefct.c

!IF  "$(CFG)" == "jpeg - Win32 Release"

DEP_CPP_JCCOE=\
	".\jconfig.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\jccoefct.obj" : $(SOURCE) $(DEP_CPP_JCCOE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeg - Win32 Debug"

DEP_CPP_JCCOE=\
	".\jconfig.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	

"$(INTDIR)\jccoefct.obj" : $(SOURCE) $(DEP_CPP_JCCOE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeg - Win32 Hybrid"

DEP_CPP_JCCOE=\
	".\jconfig.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\jccoefct.obj" : $(SOURCE) $(DEP_CPP_JCCOE) "$(INTDIR)"


!ENDIF 

SOURCE=.\jccolor.c

!IF  "$(CFG)" == "jpeg - Win32 Release"

DEP_CPP_JCCOL=\
	".\jconfig.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\jccolor.obj" : $(SOURCE) $(DEP_CPP_JCCOL) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeg - Win32 Debug"

DEP_CPP_JCCOL=\
	".\jconfig.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	

"$(INTDIR)\jccolor.obj" : $(SOURCE) $(DEP_CPP_JCCOL) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeg - Win32 Hybrid"

DEP_CPP_JCCOL=\
	".\jconfig.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\jccolor.obj" : $(SOURCE) $(DEP_CPP_JCCOL) "$(INTDIR)"


!ENDIF 

SOURCE=.\jcdctmgr.c

!IF  "$(CFG)" == "jpeg - Win32 Release"

DEP_CPP_JCDCT=\
	".\jconfig.h"\
	".\jdct.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\jcdctmgr.obj" : $(SOURCE) $(DEP_CPP_JCDCT) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeg - Win32 Debug"

DEP_CPP_JCDCT=\
	".\jconfig.h"\
	".\jdct.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	

"$(INTDIR)\jcdctmgr.obj" : $(SOURCE) $(DEP_CPP_JCDCT) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeg - Win32 Hybrid"

DEP_CPP_JCDCT=\
	".\jconfig.h"\
	".\jdct.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\jcdctmgr.obj" : $(SOURCE) $(DEP_CPP_JCDCT) "$(INTDIR)"


!ENDIF 

SOURCE=.\jchuff.c

!IF  "$(CFG)" == "jpeg - Win32 Release"

DEP_CPP_JCHUF=\
	".\jchuff.h"\
	".\jconfig.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\jchuff.obj" : $(SOURCE) $(DEP_CPP_JCHUF) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeg - Win32 Debug"

DEP_CPP_JCHUF=\
	".\jchuff.h"\
	".\jconfig.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	

"$(INTDIR)\jchuff.obj" : $(SOURCE) $(DEP_CPP_JCHUF) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeg - Win32 Hybrid"

DEP_CPP_JCHUF=\
	".\jchuff.h"\
	".\jconfig.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\jchuff.obj" : $(SOURCE) $(DEP_CPP_JCHUF) "$(INTDIR)"


!ENDIF 

SOURCE=.\jcinit.c

!IF  "$(CFG)" == "jpeg - Win32 Release"

DEP_CPP_JCINI=\
	".\jconfig.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\jcinit.obj" : $(SOURCE) $(DEP_CPP_JCINI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeg - Win32 Debug"

DEP_CPP_JCINI=\
	".\jconfig.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	

"$(INTDIR)\jcinit.obj" : $(SOURCE) $(DEP_CPP_JCINI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeg - Win32 Hybrid"

DEP_CPP_JCINI=\
	".\jconfig.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\jcinit.obj" : $(SOURCE) $(DEP_CPP_JCINI) "$(INTDIR)"


!ENDIF 

SOURCE=.\jcmainct.c

!IF  "$(CFG)" == "jpeg - Win32 Release"

DEP_CPP_JCMAI=\
	".\jconfig.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\jcmainct.obj" : $(SOURCE) $(DEP_CPP_JCMAI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeg - Win32 Debug"

DEP_CPP_JCMAI=\
	".\jconfig.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	

"$(INTDIR)\jcmainct.obj" : $(SOURCE) $(DEP_CPP_JCMAI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeg - Win32 Hybrid"

DEP_CPP_JCMAI=\
	".\jconfig.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\jcmainct.obj" : $(SOURCE) $(DEP_CPP_JCMAI) "$(INTDIR)"


!ENDIF 

SOURCE=.\jcmarker.c

!IF  "$(CFG)" == "jpeg - Win32 Release"

DEP_CPP_JCMAR=\
	".\jconfig.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\jcmarker.obj" : $(SOURCE) $(DEP_CPP_JCMAR) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeg - Win32 Debug"

DEP_CPP_JCMAR=\
	".\jconfig.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	

"$(INTDIR)\jcmarker.obj" : $(SOURCE) $(DEP_CPP_JCMAR) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeg - Win32 Hybrid"

DEP_CPP_JCMAR=\
	".\jconfig.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\jcmarker.obj" : $(SOURCE) $(DEP_CPP_JCMAR) "$(INTDIR)"


!ENDIF 

SOURCE=.\jcmaster.c

!IF  "$(CFG)" == "jpeg - Win32 Release"

DEP_CPP_JCMAS=\
	".\jconfig.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\jcmaster.obj" : $(SOURCE) $(DEP_CPP_JCMAS) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeg - Win32 Debug"

DEP_CPP_JCMAS=\
	".\jconfig.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	

"$(INTDIR)\jcmaster.obj" : $(SOURCE) $(DEP_CPP_JCMAS) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeg - Win32 Hybrid"

DEP_CPP_JCMAS=\
	".\jconfig.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\jcmaster.obj" : $(SOURCE) $(DEP_CPP_JCMAS) "$(INTDIR)"


!ENDIF 

SOURCE=.\jcomapi.c

!IF  "$(CFG)" == "jpeg - Win32 Release"

DEP_CPP_JCOMA=\
	".\jconfig.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\jcomapi.obj" : $(SOURCE) $(DEP_CPP_JCOMA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeg - Win32 Debug"

DEP_CPP_JCOMA=\
	".\jconfig.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	

"$(INTDIR)\jcomapi.obj" : $(SOURCE) $(DEP_CPP_JCOMA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeg - Win32 Hybrid"

DEP_CPP_JCOMA=\
	".\jconfig.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\jcomapi.obj" : $(SOURCE) $(DEP_CPP_JCOMA) "$(INTDIR)"


!ENDIF 

SOURCE=.\jcparam.c

!IF  "$(CFG)" == "jpeg - Win32 Release"

DEP_CPP_JCPAR=\
	".\jconfig.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\jcparam.obj" : $(SOURCE) $(DEP_CPP_JCPAR) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeg - Win32 Debug"

DEP_CPP_JCPAR=\
	".\jconfig.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	

"$(INTDIR)\jcparam.obj" : $(SOURCE) $(DEP_CPP_JCPAR) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeg - Win32 Hybrid"

DEP_CPP_JCPAR=\
	".\jconfig.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\jcparam.obj" : $(SOURCE) $(DEP_CPP_JCPAR) "$(INTDIR)"


!ENDIF 

SOURCE=.\jcphuff.c

!IF  "$(CFG)" == "jpeg - Win32 Release"

DEP_CPP_JCPHU=\
	".\jchuff.h"\
	".\jconfig.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\jcphuff.obj" : $(SOURCE) $(DEP_CPP_JCPHU) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeg - Win32 Debug"

DEP_CPP_JCPHU=\
	".\jchuff.h"\
	".\jconfig.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	

"$(INTDIR)\jcphuff.obj" : $(SOURCE) $(DEP_CPP_JCPHU) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeg - Win32 Hybrid"

DEP_CPP_JCPHU=\
	".\jchuff.h"\
	".\jconfig.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\jcphuff.obj" : $(SOURCE) $(DEP_CPP_JCPHU) "$(INTDIR)"


!ENDIF 

SOURCE=.\jcprepct.c

!IF  "$(CFG)" == "jpeg - Win32 Release"

DEP_CPP_JCPRE=\
	".\jconfig.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\jcprepct.obj" : $(SOURCE) $(DEP_CPP_JCPRE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeg - Win32 Debug"

DEP_CPP_JCPRE=\
	".\jconfig.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	

"$(INTDIR)\jcprepct.obj" : $(SOURCE) $(DEP_CPP_JCPRE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeg - Win32 Hybrid"

DEP_CPP_JCPRE=\
	".\jconfig.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\jcprepct.obj" : $(SOURCE) $(DEP_CPP_JCPRE) "$(INTDIR)"


!ENDIF 

SOURCE=.\jcsample.c

!IF  "$(CFG)" == "jpeg - Win32 Release"

DEP_CPP_JCSAM=\
	".\jconfig.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\jcsample.obj" : $(SOURCE) $(DEP_CPP_JCSAM) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeg - Win32 Debug"

DEP_CPP_JCSAM=\
	".\jconfig.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	

"$(INTDIR)\jcsample.obj" : $(SOURCE) $(DEP_CPP_JCSAM) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeg - Win32 Hybrid"

DEP_CPP_JCSAM=\
	".\jconfig.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\jcsample.obj" : $(SOURCE) $(DEP_CPP_JCSAM) "$(INTDIR)"


!ENDIF 

SOURCE=.\jctrans.c

!IF  "$(CFG)" == "jpeg - Win32 Release"

DEP_CPP_JCTRA=\
	".\jconfig.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\jctrans.obj" : $(SOURCE) $(DEP_CPP_JCTRA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeg - Win32 Debug"

DEP_CPP_JCTRA=\
	".\jconfig.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	

"$(INTDIR)\jctrans.obj" : $(SOURCE) $(DEP_CPP_JCTRA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeg - Win32 Hybrid"

DEP_CPP_JCTRA=\
	".\jconfig.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\jctrans.obj" : $(SOURCE) $(DEP_CPP_JCTRA) "$(INTDIR)"


!ENDIF 

SOURCE=.\jdapimin.c

!IF  "$(CFG)" == "jpeg - Win32 Release"

DEP_CPP_JDAPI=\
	".\jconfig.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\jdapimin.obj" : $(SOURCE) $(DEP_CPP_JDAPI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeg - Win32 Debug"

DEP_CPP_JDAPI=\
	".\jconfig.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	

"$(INTDIR)\jdapimin.obj" : $(SOURCE) $(DEP_CPP_JDAPI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeg - Win32 Hybrid"

DEP_CPP_JDAPI=\
	".\jconfig.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\jdapimin.obj" : $(SOURCE) $(DEP_CPP_JDAPI) "$(INTDIR)"


!ENDIF 

SOURCE=.\jdapistd.c

!IF  "$(CFG)" == "jpeg - Win32 Release"

DEP_CPP_JDAPIS=\
	".\jconfig.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\jdapistd.obj" : $(SOURCE) $(DEP_CPP_JDAPIS) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeg - Win32 Debug"

DEP_CPP_JDAPIS=\
	".\jconfig.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	

"$(INTDIR)\jdapistd.obj" : $(SOURCE) $(DEP_CPP_JDAPIS) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeg - Win32 Hybrid"

DEP_CPP_JDAPIS=\
	".\jconfig.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\jdapistd.obj" : $(SOURCE) $(DEP_CPP_JDAPIS) "$(INTDIR)"


!ENDIF 

SOURCE=.\jdatadst.c

!IF  "$(CFG)" == "jpeg - Win32 Release"

DEP_CPP_JDATA=\
	".\jconfig.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\jdatadst.obj" : $(SOURCE) $(DEP_CPP_JDATA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeg - Win32 Debug"

DEP_CPP_JDATA=\
	".\jconfig.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpeglib.h"\
	

"$(INTDIR)\jdatadst.obj" : $(SOURCE) $(DEP_CPP_JDATA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeg - Win32 Hybrid"

DEP_CPP_JDATA=\
	".\jconfig.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\jdatadst.obj" : $(SOURCE) $(DEP_CPP_JDATA) "$(INTDIR)"


!ENDIF 

SOURCE=.\jdatasrc.c

!IF  "$(CFG)" == "jpeg - Win32 Release"

DEP_CPP_JDATAS=\
	".\jconfig.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\jdatasrc.obj" : $(SOURCE) $(DEP_CPP_JDATAS) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeg - Win32 Debug"

DEP_CPP_JDATAS=\
	".\jconfig.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpeglib.h"\
	

"$(INTDIR)\jdatasrc.obj" : $(SOURCE) $(DEP_CPP_JDATAS) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeg - Win32 Hybrid"

DEP_CPP_JDATAS=\
	".\jconfig.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\jdatasrc.obj" : $(SOURCE) $(DEP_CPP_JDATAS) "$(INTDIR)"


!ENDIF 

SOURCE=.\jdcoefct.c

!IF  "$(CFG)" == "jpeg - Win32 Release"

DEP_CPP_JDCOE=\
	".\jconfig.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\jdcoefct.obj" : $(SOURCE) $(DEP_CPP_JDCOE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeg - Win32 Debug"

DEP_CPP_JDCOE=\
	".\jconfig.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	

"$(INTDIR)\jdcoefct.obj" : $(SOURCE) $(DEP_CPP_JDCOE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeg - Win32 Hybrid"

DEP_CPP_JDCOE=\
	".\jconfig.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\jdcoefct.obj" : $(SOURCE) $(DEP_CPP_JDCOE) "$(INTDIR)"


!ENDIF 

SOURCE=.\jdcolor.c

!IF  "$(CFG)" == "jpeg - Win32 Release"

DEP_CPP_JDCOL=\
	".\jconfig.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\jdcolor.obj" : $(SOURCE) $(DEP_CPP_JDCOL) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeg - Win32 Debug"

DEP_CPP_JDCOL=\
	".\jconfig.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	

"$(INTDIR)\jdcolor.obj" : $(SOURCE) $(DEP_CPP_JDCOL) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeg - Win32 Hybrid"

DEP_CPP_JDCOL=\
	".\jconfig.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\jdcolor.obj" : $(SOURCE) $(DEP_CPP_JDCOL) "$(INTDIR)"


!ENDIF 

SOURCE=.\jddctmgr.c

!IF  "$(CFG)" == "jpeg - Win32 Release"

DEP_CPP_JDDCT=\
	".\jconfig.h"\
	".\jdct.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\jddctmgr.obj" : $(SOURCE) $(DEP_CPP_JDDCT) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeg - Win32 Debug"

DEP_CPP_JDDCT=\
	".\jconfig.h"\
	".\jdct.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	

"$(INTDIR)\jddctmgr.obj" : $(SOURCE) $(DEP_CPP_JDDCT) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeg - Win32 Hybrid"

DEP_CPP_JDDCT=\
	".\jconfig.h"\
	".\jdct.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\jddctmgr.obj" : $(SOURCE) $(DEP_CPP_JDDCT) "$(INTDIR)"


!ENDIF 

SOURCE=.\jdhuff.c

!IF  "$(CFG)" == "jpeg - Win32 Release"

DEP_CPP_JDHUF=\
	".\jconfig.h"\
	".\jdhuff.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\jdhuff.obj" : $(SOURCE) $(DEP_CPP_JDHUF) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeg - Win32 Debug"

DEP_CPP_JDHUF=\
	".\jconfig.h"\
	".\jdhuff.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	

"$(INTDIR)\jdhuff.obj" : $(SOURCE) $(DEP_CPP_JDHUF) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeg - Win32 Hybrid"

DEP_CPP_JDHUF=\
	".\jconfig.h"\
	".\jdhuff.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\jdhuff.obj" : $(SOURCE) $(DEP_CPP_JDHUF) "$(INTDIR)"


!ENDIF 

SOURCE=.\jdinput.c

!IF  "$(CFG)" == "jpeg - Win32 Release"

DEP_CPP_JDINP=\
	".\jconfig.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\jdinput.obj" : $(SOURCE) $(DEP_CPP_JDINP) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeg - Win32 Debug"

DEP_CPP_JDINP=\
	".\jconfig.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	

"$(INTDIR)\jdinput.obj" : $(SOURCE) $(DEP_CPP_JDINP) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeg - Win32 Hybrid"

DEP_CPP_JDINP=\
	".\jconfig.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\jdinput.obj" : $(SOURCE) $(DEP_CPP_JDINP) "$(INTDIR)"


!ENDIF 

SOURCE=.\jdmainct.c

!IF  "$(CFG)" == "jpeg - Win32 Release"

DEP_CPP_JDMAI=\
	".\jconfig.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\jdmainct.obj" : $(SOURCE) $(DEP_CPP_JDMAI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeg - Win32 Debug"

DEP_CPP_JDMAI=\
	".\jconfig.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	

"$(INTDIR)\jdmainct.obj" : $(SOURCE) $(DEP_CPP_JDMAI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeg - Win32 Hybrid"

DEP_CPP_JDMAI=\
	".\jconfig.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\jdmainct.obj" : $(SOURCE) $(DEP_CPP_JDMAI) "$(INTDIR)"


!ENDIF 

SOURCE=.\jdmarker.c

!IF  "$(CFG)" == "jpeg - Win32 Release"

DEP_CPP_JDMAR=\
	".\jconfig.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\jdmarker.obj" : $(SOURCE) $(DEP_CPP_JDMAR) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeg - Win32 Debug"

DEP_CPP_JDMAR=\
	".\jconfig.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	

"$(INTDIR)\jdmarker.obj" : $(SOURCE) $(DEP_CPP_JDMAR) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeg - Win32 Hybrid"

DEP_CPP_JDMAR=\
	".\jconfig.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\jdmarker.obj" : $(SOURCE) $(DEP_CPP_JDMAR) "$(INTDIR)"


!ENDIF 

SOURCE=.\jdmaster.c

!IF  "$(CFG)" == "jpeg - Win32 Release"

DEP_CPP_JDMAS=\
	".\jconfig.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\jdmaster.obj" : $(SOURCE) $(DEP_CPP_JDMAS) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeg - Win32 Debug"

DEP_CPP_JDMAS=\
	".\jconfig.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	

"$(INTDIR)\jdmaster.obj" : $(SOURCE) $(DEP_CPP_JDMAS) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeg - Win32 Hybrid"

DEP_CPP_JDMAS=\
	".\jconfig.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\jdmaster.obj" : $(SOURCE) $(DEP_CPP_JDMAS) "$(INTDIR)"


!ENDIF 

SOURCE=.\jdmerge.c

!IF  "$(CFG)" == "jpeg - Win32 Release"

DEP_CPP_JDMER=\
	".\jconfig.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\jdmerge.obj" : $(SOURCE) $(DEP_CPP_JDMER) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeg - Win32 Debug"

DEP_CPP_JDMER=\
	".\jconfig.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	

"$(INTDIR)\jdmerge.obj" : $(SOURCE) $(DEP_CPP_JDMER) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeg - Win32 Hybrid"

DEP_CPP_JDMER=\
	".\jconfig.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\jdmerge.obj" : $(SOURCE) $(DEP_CPP_JDMER) "$(INTDIR)"


!ENDIF 

SOURCE=.\jdphuff.c

!IF  "$(CFG)" == "jpeg - Win32 Release"

DEP_CPP_JDPHU=\
	".\jconfig.h"\
	".\jdhuff.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\jdphuff.obj" : $(SOURCE) $(DEP_CPP_JDPHU) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeg - Win32 Debug"

DEP_CPP_JDPHU=\
	".\jconfig.h"\
	".\jdhuff.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	

"$(INTDIR)\jdphuff.obj" : $(SOURCE) $(DEP_CPP_JDPHU) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeg - Win32 Hybrid"

DEP_CPP_JDPHU=\
	".\jconfig.h"\
	".\jdhuff.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\jdphuff.obj" : $(SOURCE) $(DEP_CPP_JDPHU) "$(INTDIR)"


!ENDIF 

SOURCE=.\jdpostct.c

!IF  "$(CFG)" == "jpeg - Win32 Release"

DEP_CPP_JDPOS=\
	".\jconfig.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\jdpostct.obj" : $(SOURCE) $(DEP_CPP_JDPOS) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeg - Win32 Debug"

DEP_CPP_JDPOS=\
	".\jconfig.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	

"$(INTDIR)\jdpostct.obj" : $(SOURCE) $(DEP_CPP_JDPOS) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeg - Win32 Hybrid"

DEP_CPP_JDPOS=\
	".\jconfig.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\jdpostct.obj" : $(SOURCE) $(DEP_CPP_JDPOS) "$(INTDIR)"


!ENDIF 

SOURCE=.\jdsample.c

!IF  "$(CFG)" == "jpeg - Win32 Release"

DEP_CPP_JDSAM=\
	".\jconfig.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\jdsample.obj" : $(SOURCE) $(DEP_CPP_JDSAM) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeg - Win32 Debug"

DEP_CPP_JDSAM=\
	".\jconfig.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	

"$(INTDIR)\jdsample.obj" : $(SOURCE) $(DEP_CPP_JDSAM) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeg - Win32 Hybrid"

DEP_CPP_JDSAM=\
	".\jconfig.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\jdsample.obj" : $(SOURCE) $(DEP_CPP_JDSAM) "$(INTDIR)"


!ENDIF 

SOURCE=.\jdtrans.c

!IF  "$(CFG)" == "jpeg - Win32 Release"

DEP_CPP_JDTRA=\
	".\jconfig.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\jdtrans.obj" : $(SOURCE) $(DEP_CPP_JDTRA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeg - Win32 Debug"

DEP_CPP_JDTRA=\
	".\jconfig.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	

"$(INTDIR)\jdtrans.obj" : $(SOURCE) $(DEP_CPP_JDTRA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeg - Win32 Hybrid"

DEP_CPP_JDTRA=\
	".\jconfig.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\jdtrans.obj" : $(SOURCE) $(DEP_CPP_JDTRA) "$(INTDIR)"


!ENDIF 

SOURCE=.\jerror.c

!IF  "$(CFG)" == "jpeg - Win32 Release"

DEP_CPP_JERRO=\
	".\jconfig.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	".\jversion.h"\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\jerror.obj" : $(SOURCE) $(DEP_CPP_JERRO) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeg - Win32 Debug"

DEP_CPP_JERRO=\
	".\jconfig.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpeglib.h"\
	".\jversion.h"\
	

"$(INTDIR)\jerror.obj" : $(SOURCE) $(DEP_CPP_JERRO) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeg - Win32 Hybrid"

DEP_CPP_JERRO=\
	".\jconfig.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	".\jversion.h"\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\jerror.obj" : $(SOURCE) $(DEP_CPP_JERRO) "$(INTDIR)"


!ENDIF 

SOURCE=.\jfdctflt.c

!IF  "$(CFG)" == "jpeg - Win32 Release"

DEP_CPP_JFDCT=\
	".\jconfig.h"\
	".\jdct.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\jfdctflt.obj" : $(SOURCE) $(DEP_CPP_JFDCT) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeg - Win32 Debug"

DEP_CPP_JFDCT=\
	".\jconfig.h"\
	".\jdct.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	

"$(INTDIR)\jfdctflt.obj" : $(SOURCE) $(DEP_CPP_JFDCT) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeg - Win32 Hybrid"

DEP_CPP_JFDCT=\
	".\jconfig.h"\
	".\jdct.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\jfdctflt.obj" : $(SOURCE) $(DEP_CPP_JFDCT) "$(INTDIR)"


!ENDIF 

SOURCE=.\jfdctfst.c

!IF  "$(CFG)" == "jpeg - Win32 Release"

DEP_CPP_JFDCTF=\
	".\jconfig.h"\
	".\jdct.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\jfdctfst.obj" : $(SOURCE) $(DEP_CPP_JFDCTF) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeg - Win32 Debug"

DEP_CPP_JFDCTF=\
	".\jconfig.h"\
	".\jdct.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	

"$(INTDIR)\jfdctfst.obj" : $(SOURCE) $(DEP_CPP_JFDCTF) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeg - Win32 Hybrid"

DEP_CPP_JFDCTF=\
	".\jconfig.h"\
	".\jdct.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\jfdctfst.obj" : $(SOURCE) $(DEP_CPP_JFDCTF) "$(INTDIR)"


!ENDIF 

SOURCE=.\jfdctint.c

!IF  "$(CFG)" == "jpeg - Win32 Release"

DEP_CPP_JFDCTI=\
	".\jconfig.h"\
	".\jdct.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\jfdctint.obj" : $(SOURCE) $(DEP_CPP_JFDCTI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeg - Win32 Debug"

DEP_CPP_JFDCTI=\
	".\jconfig.h"\
	".\jdct.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	

"$(INTDIR)\jfdctint.obj" : $(SOURCE) $(DEP_CPP_JFDCTI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeg - Win32 Hybrid"

DEP_CPP_JFDCTI=\
	".\jconfig.h"\
	".\jdct.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\jfdctint.obj" : $(SOURCE) $(DEP_CPP_JFDCTI) "$(INTDIR)"


!ENDIF 

SOURCE=.\jidctflt.c

!IF  "$(CFG)" == "jpeg - Win32 Release"

DEP_CPP_JIDCT=\
	".\jconfig.h"\
	".\jdct.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\jidctflt.obj" : $(SOURCE) $(DEP_CPP_JIDCT) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeg - Win32 Debug"

DEP_CPP_JIDCT=\
	".\jconfig.h"\
	".\jdct.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	

"$(INTDIR)\jidctflt.obj" : $(SOURCE) $(DEP_CPP_JIDCT) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeg - Win32 Hybrid"

DEP_CPP_JIDCT=\
	".\jconfig.h"\
	".\jdct.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\jidctflt.obj" : $(SOURCE) $(DEP_CPP_JIDCT) "$(INTDIR)"


!ENDIF 

SOURCE=.\jidctfst.c

!IF  "$(CFG)" == "jpeg - Win32 Release"

DEP_CPP_JIDCTF=\
	".\jconfig.h"\
	".\jdct.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\jidctfst.obj" : $(SOURCE) $(DEP_CPP_JIDCTF) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeg - Win32 Debug"

DEP_CPP_JIDCTF=\
	".\jconfig.h"\
	".\jdct.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	

"$(INTDIR)\jidctfst.obj" : $(SOURCE) $(DEP_CPP_JIDCTF) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeg - Win32 Hybrid"

DEP_CPP_JIDCTF=\
	".\jconfig.h"\
	".\jdct.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\jidctfst.obj" : $(SOURCE) $(DEP_CPP_JIDCTF) "$(INTDIR)"


!ENDIF 

SOURCE=.\jidctint.c

!IF  "$(CFG)" == "jpeg - Win32 Release"

DEP_CPP_JIDCTI=\
	".\jconfig.h"\
	".\jdct.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\jidctint.obj" : $(SOURCE) $(DEP_CPP_JIDCTI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeg - Win32 Debug"

DEP_CPP_JIDCTI=\
	".\jconfig.h"\
	".\jdct.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	

"$(INTDIR)\jidctint.obj" : $(SOURCE) $(DEP_CPP_JIDCTI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeg - Win32 Hybrid"

DEP_CPP_JIDCTI=\
	".\jconfig.h"\
	".\jdct.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\jidctint.obj" : $(SOURCE) $(DEP_CPP_JIDCTI) "$(INTDIR)"


!ENDIF 

SOURCE=.\jidctred.c

!IF  "$(CFG)" == "jpeg - Win32 Release"

DEP_CPP_JIDCTR=\
	".\jconfig.h"\
	".\jdct.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\jidctred.obj" : $(SOURCE) $(DEP_CPP_JIDCTR) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeg - Win32 Debug"

DEP_CPP_JIDCTR=\
	".\jconfig.h"\
	".\jdct.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	

"$(INTDIR)\jidctred.obj" : $(SOURCE) $(DEP_CPP_JIDCTR) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeg - Win32 Hybrid"

DEP_CPP_JIDCTR=\
	".\jconfig.h"\
	".\jdct.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\jidctred.obj" : $(SOURCE) $(DEP_CPP_JIDCTR) "$(INTDIR)"


!ENDIF 

SOURCE=.\jmemmgr.c

!IF  "$(CFG)" == "jpeg - Win32 Release"

DEP_CPP_JMEMM=\
	".\jconfig.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmemsys.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\jmemmgr.obj" : $(SOURCE) $(DEP_CPP_JMEMM) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeg - Win32 Debug"

DEP_CPP_JMEMM=\
	".\jconfig.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmemsys.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	

"$(INTDIR)\jmemmgr.obj" : $(SOURCE) $(DEP_CPP_JMEMM) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeg - Win32 Hybrid"

DEP_CPP_JMEMM=\
	".\jconfig.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmemsys.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\jmemmgr.obj" : $(SOURCE) $(DEP_CPP_JMEMM) "$(INTDIR)"


!ENDIF 

SOURCE=.\jmemnobs.c

!IF  "$(CFG)" == "jpeg - Win32 Release"

DEP_CPP_JMEMN=\
	".\jconfig.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmemsys.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\jmemnobs.obj" : $(SOURCE) $(DEP_CPP_JMEMN) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeg - Win32 Debug"

DEP_CPP_JMEMN=\
	".\jconfig.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmemsys.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	

"$(INTDIR)\jmemnobs.obj" : $(SOURCE) $(DEP_CPP_JMEMN) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeg - Win32 Hybrid"

DEP_CPP_JMEMN=\
	".\jconfig.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmemsys.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\jmemnobs.obj" : $(SOURCE) $(DEP_CPP_JMEMN) "$(INTDIR)"


!ENDIF 

SOURCE=.\jpeg.cpp

!IF  "$(CFG)" == "jpeg - Win32 Release"

DEP_CPP_JPEG_=\
	"..\..\..\include\acolor.h"\
	"..\..\..\include\animtbl.h"\
	"..\..\..\include\appio.h"\
	"..\..\..\include\assert1.h"\
	"..\..\..\include\bitarray.h"\
	"..\..\..\include\bitmap.h"\
	"..\..\..\include\bmmlib.h"\
	"..\..\..\include\box2.h"\
	"..\..\..\include\box3.h"\
	"..\..\..\include\buildver.h"\
	"..\..\..\include\channels.h"\
	"..\..\..\include\cmdmode.h"\
	"..\..\..\include\color.h"\
	"..\..\..\include\control.h"\
	"..\..\..\include\coreexp.h"\
	"..\..\..\include\custcont.h"\
	"..\..\..\include\dbgprint.h"\
	"..\..\..\include\dpoint3.h"\
	"..\..\..\include\euler.h"\
	"..\..\..\include\evuser.h"\
	"..\..\..\include\export.h"\
	"..\..\..\include\gencam.h"\
	"..\..\..\include\genhier.h"\
	"..\..\..\include\genlight.h"\
	"..\..\..\include\genshape.h"\
	"..\..\..\include\geom.h"\
	"..\..\..\include\geomlib.h"\
	"..\..\..\include\gfx.h"\
	"..\..\..\include\gfxlib.h"\
	"..\..\..\include\gutil.h"\
	"..\..\..\include\hitdata.h"\
	"..\..\..\include\hold.h"\
	"..\..\..\include\impapi.h"\
	"..\..\..\include\impexp.h"\
	"..\..\..\include\imtl.h"\
	"..\..\..\include\inode.h"\
	"..\..\..\include\interval.h"\
	"..\..\..\include\ioapi.h"\
	"..\..\..\include\iparamb.h"\
	"..\..\..\include\ipoint2.h"\
	"..\..\..\include\ipoint3.h"\
	"..\..\..\include\linklist.h"\
	"..\..\..\include\lockid.h"\
	"..\..\..\include\log.h"\
	"..\..\..\include\matrix2.h"\
	"..\..\..\include\matrix3.h"\
	"..\..\..\include\max.h"\
	"..\..\..\include\maxapi.h"\
	"..\..\..\include\maxcom.h"\
	"..\..\..\include\maxtess.h"\
	"..\..\..\include\maxtypes.h"\
	"..\..\..\include\mesh.h"\
	"..\..\..\include\meshlib.h"\
	"..\..\..\include\mouseman.h"\
	"..\..\..\include\mtl.h"\
	"..\..\..\include\nametab.h"\
	"..\..\..\include\object.h"\
	"..\..\..\include\objmode.h"\
	"..\..\..\include\palutil.h"\
	"..\..\..\include\patch.h"\
	"..\..\..\include\patchlib.h"\
	"..\..\..\include\patchobj.h"\
	"..\..\..\include\plugapi.h"\
	"..\..\..\include\plugin.h"\
	"..\..\..\include\point2.h"\
	"..\..\..\include\point3.h"\
	"..\..\..\include\point4.h"\
	"..\..\..\include\ptrvec.h"\
	"..\..\..\include\quat.h"\
	"..\..\..\include\ref.h"\
	"..\..\..\include\render.h"\
	"..\..\..\include\rtclick.h"\
	"..\..\..\include\sceneapi.h"\
	"..\..\..\include\snap.h"\
	"..\..\..\include\soundobj.h"\
	"..\..\..\include\stack.h"\
	"..\..\..\include\stack3.h"\
	"..\..\..\include\strbasic.h"\
	"..\..\..\include\strclass.h"\
	"..\..\..\include\tab.h"\
	"..\..\..\include\trig.h"\
	"..\..\..\include\triobj.h"\
	"..\..\..\include\units.h"\
	"..\..\..\include\utilexp.h"\
	"..\..\..\include\utillib.h"\
	"..\..\..\include\vedge.h"\
	"..\..\..\include\winutil.h"\
	".\fmtspec.h"\
	".\interfce.h"\
	".\jpeg.h"\
	

"$(INTDIR)\jpeg.obj" : $(SOURCE) $(DEP_CPP_JPEG_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeg - Win32 Debug"

DEP_CPP_JPEG_=\
	"..\..\..\include\acolor.h"\
	"..\..\..\include\animtbl.h"\
	"..\..\..\include\appio.h"\
	"..\..\..\include\assert1.h"\
	"..\..\..\include\bitarray.h"\
	"..\..\..\include\bitmap.h"\
	"..\..\..\include\bmmlib.h"\
	"..\..\..\include\box2.h"\
	"..\..\..\include\box3.h"\
	"..\..\..\include\buildver.h"\
	"..\..\..\include\channels.h"\
	"..\..\..\include\cmdmode.h"\
	"..\..\..\include\color.h"\
	"..\..\..\include\control.h"\
	"..\..\..\include\coreexp.h"\
	"..\..\..\include\custcont.h"\
	"..\..\..\include\dbgprint.h"\
	"..\..\..\include\dpoint3.h"\
	"..\..\..\include\euler.h"\
	"..\..\..\include\evuser.h"\
	"..\..\..\include\export.h"\
	"..\..\..\include\gencam.h"\
	"..\..\..\include\genhier.h"\
	"..\..\..\include\genlight.h"\
	"..\..\..\include\genshape.h"\
	"..\..\..\include\geom.h"\
	"..\..\..\include\geomlib.h"\
	"..\..\..\include\gfx.h"\
	"..\..\..\include\gfxlib.h"\
	"..\..\..\include\gutil.h"\
	"..\..\..\include\hitdata.h"\
	"..\..\..\include\hold.h"\
	"..\..\..\include\impapi.h"\
	"..\..\..\include\impexp.h"\
	"..\..\..\include\imtl.h"\
	"..\..\..\include\inode.h"\
	"..\..\..\include\interval.h"\
	"..\..\..\include\ioapi.h"\
	"..\..\..\include\iparamb.h"\
	"..\..\..\include\ipoint2.h"\
	"..\..\..\include\ipoint3.h"\
	"..\..\..\include\linklist.h"\
	"..\..\..\include\lockid.h"\
	"..\..\..\include\log.h"\
	"..\..\..\include\matrix2.h"\
	"..\..\..\include\matrix3.h"\
	"..\..\..\include\max.h"\
	"..\..\..\include\maxapi.h"\
	"..\..\..\include\maxcom.h"\
	"..\..\..\include\maxtess.h"\
	"..\..\..\include\maxtypes.h"\
	"..\..\..\include\mesh.h"\
	"..\..\..\include\meshlib.h"\
	"..\..\..\include\mouseman.h"\
	"..\..\..\include\mtl.h"\
	"..\..\..\include\nametab.h"\
	"..\..\..\include\object.h"\
	"..\..\..\include\objmode.h"\
	"..\..\..\include\palutil.h"\
	"..\..\..\include\patch.h"\
	"..\..\..\include\patchlib.h"\
	"..\..\..\include\patchobj.h"\
	"..\..\..\include\plugapi.h"\
	"..\..\..\include\plugin.h"\
	"..\..\..\include\point2.h"\
	"..\..\..\include\point3.h"\
	"..\..\..\include\point4.h"\
	"..\..\..\include\ptrvec.h"\
	"..\..\..\include\quat.h"\
	"..\..\..\include\ref.h"\
	"..\..\..\include\render.h"\
	"..\..\..\include\rtclick.h"\
	"..\..\..\include\sceneapi.h"\
	"..\..\..\include\snap.h"\
	"..\..\..\include\soundobj.h"\
	"..\..\..\include\stack.h"\
	"..\..\..\include\stack3.h"\
	"..\..\..\include\strbasic.h"\
	"..\..\..\include\strclass.h"\
	"..\..\..\include\tab.h"\
	"..\..\..\include\trig.h"\
	"..\..\..\include\triobj.h"\
	"..\..\..\include\units.h"\
	"..\..\..\include\utilexp.h"\
	"..\..\..\include\utillib.h"\
	"..\..\..\include\vedge.h"\
	"..\..\..\include\winutil.h"\
	".\fmtspec.h"\
	".\interfce.h"\
	".\jpeg.h"\
	

"$(INTDIR)\jpeg.obj" : $(SOURCE) $(DEP_CPP_JPEG_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeg - Win32 Hybrid"

DEP_CPP_JPEG_=\
	"..\..\..\include\acolor.h"\
	"..\..\..\include\animtbl.h"\
	"..\..\..\include\appio.h"\
	"..\..\..\include\assert1.h"\
	"..\..\..\include\bitarray.h"\
	"..\..\..\include\bitmap.h"\
	"..\..\..\include\bmmlib.h"\
	"..\..\..\include\box2.h"\
	"..\..\..\include\box3.h"\
	"..\..\..\include\buildver.h"\
	"..\..\..\include\channels.h"\
	"..\..\..\include\cmdmode.h"\
	"..\..\..\include\color.h"\
	"..\..\..\include\control.h"\
	"..\..\..\include\coreexp.h"\
	"..\..\..\include\custcont.h"\
	"..\..\..\include\dbgprint.h"\
	"..\..\..\include\dpoint3.h"\
	"..\..\..\include\euler.h"\
	"..\..\..\include\evuser.h"\
	"..\..\..\include\export.h"\
	"..\..\..\include\gencam.h"\
	"..\..\..\include\genhier.h"\
	"..\..\..\include\genlight.h"\
	"..\..\..\include\genshape.h"\
	"..\..\..\include\geom.h"\
	"..\..\..\include\geomlib.h"\
	"..\..\..\include\gfx.h"\
	"..\..\..\include\gfxlib.h"\
	"..\..\..\include\gutil.h"\
	"..\..\..\include\hitdata.h"\
	"..\..\..\include\hold.h"\
	"..\..\..\include\impapi.h"\
	"..\..\..\include\impexp.h"\
	"..\..\..\include\imtl.h"\
	"..\..\..\include\inode.h"\
	"..\..\..\include\interval.h"\
	"..\..\..\include\ioapi.h"\
	"..\..\..\include\iparamb.h"\
	"..\..\..\include\ipoint2.h"\
	"..\..\..\include\ipoint3.h"\
	"..\..\..\include\linklist.h"\
	"..\..\..\include\lockid.h"\
	"..\..\..\include\log.h"\
	"..\..\..\include\matrix2.h"\
	"..\..\..\include\matrix3.h"\
	"..\..\..\include\max.h"\
	"..\..\..\include\maxapi.h"\
	"..\..\..\include\maxcom.h"\
	"..\..\..\include\maxtess.h"\
	"..\..\..\include\maxtypes.h"\
	"..\..\..\include\mesh.h"\
	"..\..\..\include\meshlib.h"\
	"..\..\..\include\mouseman.h"\
	"..\..\..\include\mtl.h"\
	"..\..\..\include\nametab.h"\
	"..\..\..\include\object.h"\
	"..\..\..\include\objmode.h"\
	"..\..\..\include\palutil.h"\
	"..\..\..\include\patch.h"\
	"..\..\..\include\patchlib.h"\
	"..\..\..\include\patchobj.h"\
	"..\..\..\include\plugapi.h"\
	"..\..\..\include\plugin.h"\
	"..\..\..\include\point2.h"\
	"..\..\..\include\point3.h"\
	"..\..\..\include\point4.h"\
	"..\..\..\include\ptrvec.h"\
	"..\..\..\include\quat.h"\
	"..\..\..\include\ref.h"\
	"..\..\..\include\render.h"\
	"..\..\..\include\rtclick.h"\
	"..\..\..\include\sceneapi.h"\
	"..\..\..\include\snap.h"\
	"..\..\..\include\soundobj.h"\
	"..\..\..\include\stack.h"\
	"..\..\..\include\stack3.h"\
	"..\..\..\include\strbasic.h"\
	"..\..\..\include\strclass.h"\
	"..\..\..\include\tab.h"\
	"..\..\..\include\trig.h"\
	"..\..\..\include\triobj.h"\
	"..\..\..\include\units.h"\
	"..\..\..\include\utilexp.h"\
	"..\..\..\include\utillib.h"\
	"..\..\..\include\vedge.h"\
	"..\..\..\include\winutil.h"\
	".\fmtspec.h"\
	".\interfce.h"\
	".\jpeg.h"\
	

"$(INTDIR)\jpeg.obj" : $(SOURCE) $(DEP_CPP_JPEG_) "$(INTDIR)"


!ENDIF 

SOURCE=.\jpeg.rc

"$(INTDIR)\jpeg.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\jquant1.c

!IF  "$(CFG)" == "jpeg - Win32 Release"

DEP_CPP_JQUAN=\
	".\jconfig.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\jquant1.obj" : $(SOURCE) $(DEP_CPP_JQUAN) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeg - Win32 Debug"

DEP_CPP_JQUAN=\
	".\jconfig.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	

"$(INTDIR)\jquant1.obj" : $(SOURCE) $(DEP_CPP_JQUAN) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeg - Win32 Hybrid"

DEP_CPP_JQUAN=\
	".\jconfig.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\jquant1.obj" : $(SOURCE) $(DEP_CPP_JQUAN) "$(INTDIR)"


!ENDIF 

SOURCE=.\jquant2.c

!IF  "$(CFG)" == "jpeg - Win32 Release"

DEP_CPP_JQUANT=\
	".\jconfig.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\jquant2.obj" : $(SOURCE) $(DEP_CPP_JQUANT) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeg - Win32 Debug"

DEP_CPP_JQUANT=\
	".\jconfig.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	

"$(INTDIR)\jquant2.obj" : $(SOURCE) $(DEP_CPP_JQUANT) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeg - Win32 Hybrid"

DEP_CPP_JQUANT=\
	".\jconfig.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\jquant2.obj" : $(SOURCE) $(DEP_CPP_JQUANT) "$(INTDIR)"


!ENDIF 

SOURCE=.\jutils.c

!IF  "$(CFG)" == "jpeg - Win32 Release"

DEP_CPP_JUTIL=\
	".\jconfig.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\jutils.obj" : $(SOURCE) $(DEP_CPP_JUTIL) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeg - Win32 Debug"

DEP_CPP_JUTIL=\
	".\jconfig.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	

"$(INTDIR)\jutils.obj" : $(SOURCE) $(DEP_CPP_JUTIL) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeg - Win32 Hybrid"

DEP_CPP_JUTIL=\
	".\jconfig.h"\
	".\jerror.h"\
	".\jinclude.h"\
	".\jmorecfg.h"\
	".\jpegint.h"\
	".\jpeglib.h"\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\jutils.obj" : $(SOURCE) $(DEP_CPP_JUTIL) "$(INTDIR)"


!ENDIF 


!ENDIF 

