# Microsoft Visual C++ generated build script - Do not modify

PROJ = IMUTIL16
DEBUG = 1
PROGTYPE = 1
CALLER = 
ARGS = 
DLLS = 
D_RCDEFINES = -d_DEBUG
R_RCDEFINES = -dNDEBUG
ORIGIN = MSVC
ORIGIN_VER = 1.00
PROJPATH = D:\TESTS\RAWREAD\WIN32\IMUTIL16\
USEMFC = 0
CC = cl
CPP = cl
CXX = cl
CCREATEPCHFLAG = 
CPPCREATEPCHFLAG = 
CUSEPCHFLAG = 
CPPUSEPCHFLAG = 
FIRSTC = MEM.C       
FIRSTCPP =             
RC = rc
CFLAGS_D_WDLL = /nologo /G2 /Zp1 /W3 /Zi /ALw /Od /D "APPTITLE"="\"IMUTIL16\"" /D "_DEBUG" /D "WIN16" /FR /GD /Fd"IMUTIL.PDB"
CFLAGS_R_WDLL = /nologo /Zp1 /W3 /ALw /O2 /D "APPTITLE"="\"IMUTIL16\"" /D "NDEBUG" /D "WIN16" /FR /GD 
LFLAGS_D_WDLL = /NOLOGO /NOD /NOE /PACKC:61440 /ALIGN:16 /ONERROR:NOEXE /CO /MAP /LINE /MAP:FULL ..\thunk\16\thunk.obj
LFLAGS_R_WDLL = /NOLOGO /NOD /NOE /PACKC:61440 /ALIGN:16 /ONERROR:NOEXE /MAP /LINE /MAP:FULL
LIBS_D_WDLL = oldnames libw ldllcew 
LIBS_R_WDLL = oldnames libw ldllcew ..\..\thunk\16\thunk.obj commdlg.lib olecli.lib olesvr.lib shell.lib 
RCFLAGS = 
RESFLAGS = -40
RUNFLAGS = 
DEFFILE = IMUTIL16.DEF
OBJS_EXT = 
LIBS_EXT = 
!if "$(DEBUG)" == "1"
CFLAGS = $(CFLAGS_D_WDLL)
LFLAGS = $(LFLAGS_D_WDLL)
LIBS = $(LIBS_D_WDLL)
MAPFILE = nul
RCDEFINES = $(D_RCDEFINES)
!else
CFLAGS = $(CFLAGS_R_WDLL)
LFLAGS = $(LFLAGS_R_WDLL)
LIBS = $(LIBS_R_WDLL)
MAPFILE = nul
RCDEFINES = $(R_RCDEFINES)
!endif
!if [if exist MSVC.BND del MSVC.BND]
!endif
SBRS = MEM.SBR \
		DLLMAIN.SBR \
		DEBUG.SBR \
		DISKUTIL.SBR


MEM_DEP = \defs.h \
	\mem.h


DLLMAIN_DEP = \win32\debug.h


DEBUG_DEP = \win32\debug.h


DISKUTIL_DEP = \win32\debug.h \
	\defs.h \
	\diskutil.h \
	\mem.h


all:	$(PROJ).DLL $(PROJ).BSC

MEM.OBJ:	..\..\MEM.C $(MEM_DEP)
	$(CC) $(CFLAGS) $(CCREATEPCHFLAG) /c ..\..\MEM.C

DLLMAIN.OBJ:	DLLMAIN.C $(DLLMAIN_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c DLLMAIN.C

DEBUG.OBJ:	..\DEBUG.C $(DEBUG_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c ..\DEBUG.C

DISKUTIL.OBJ:	..\..\DISKUTIL.C $(DISKUTIL_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c ..\..\DISKUTIL.C


$(PROJ).DLL::	MEM.OBJ DLLMAIN.OBJ DEBUG.OBJ DISKUTIL.OBJ $(OBJS_EXT) $(DEFFILE)
	echo >NUL @<<$(PROJ).CRF
MEM.OBJ +
DLLMAIN.OBJ +
DEBUG.OBJ +
DISKUTIL.OBJ +
$(OBJS_EXT)
$(PROJ).DLL
$(MAPFILE)
d:\msvc15\lib\+
$(LIBS)
$(DEFFILE);
<<
	link $(LFLAGS) @$(PROJ).CRF
	$(RC) $(RESFLAGS) $@
	implib /nowep $(PROJ).LIB $(PROJ).DLL


run: $(PROJ).DLL
	$(PROJ) $(RUNFLAGS)


$(PROJ).BSC: $(SBRS)
	bscmake @<<
/o$@ $(SBRS)
<<
