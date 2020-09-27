# Microsoft Visual C++ generated build script - Do not modify

PROJ = RAWREAD
DEBUG = 0
PROGTYPE = 6
CALLER = 
ARGS = 
DLLS = 
D_RCDEFINES = -d_DEBUG
R_RCDEFINES = -dNDEBUG
ORIGIN = MSVC
ORIGIN_VER = 1.00
PROJPATH = D:\XBOX\PRIVATE\TEST\TOOLS\RAWREAD\
USEMFC = 0
CC = cl
CPP = cl
CXX = cl
CCREATEPCHFLAG = 
CPPCREATEPCHFLAG = 
CUSEPCHFLAG = 
CPPUSEPCHFLAG = 
FIRSTC = TEXT.C      
FIRSTCPP =             
RC = rc
CFLAGS_D_DEXE = /nologo /G2 /Zp1 /W3 /Zi /AL /O2 /D "APPTITLE"="\"RawRead\"" /D "DPF_LIBRARY"="\"RAWREAD\"" /D "_DEBUG" /D "MSINTERNAL" /D "_DOS" /FR /Fd"RAWREAD.PDB"
CFLAGS_R_DEXE = /nologo /Gs /G2 /Zp1 /W3 /AL /O2 /Ox /D "APPTITLE"="\"RawRead\"" /D "DPF_LIBRARY"="\"RAWREAD\"" /D "NDEBUG" /D "_DOS" /FR 
LFLAGS_D_DEXE = /NOLOGO /NOI /STACK:5120 /EXEPACK /ONERROR:NOEXE /CO 
LFLAGS_R_DEXE = /NOLOGO /NOI /STACK:5120 /EXEPACK /ONERROR:NOEXE 
LIBS_D_DEXE = oldnames llibce 
LIBS_R_DEXE = oldnames llibce 
RCFLAGS = /nologo
RESFLAGS = /nologo
RUNFLAGS = 
OBJS_EXT = 
LIBS_EXT = 
!if "$(DEBUG)" == "1"
CFLAGS = $(CFLAGS_D_DEXE)
LFLAGS = $(LFLAGS_D_DEXE)
LIBS = $(LIBS_D_DEXE)
MAPFILE = nul
RCDEFINES = $(D_RCDEFINES)
!else
CFLAGS = $(CFLAGS_R_DEXE)
LFLAGS = $(LFLAGS_R_DEXE)
LIBS = $(LIBS_R_DEXE)
MAPFILE = nul
RCDEFINES = $(R_RCDEFINES)
!endif
!if [if exist MSVC.BND del MSVC.BND]
!endif
SBRS = BITMAP.SBR \
		COMPRESS.SBR \
		DISKUTIL.SBR \
		FATUTIL.SBR \
		FILE.SBR \
		IMAGE.SBR \
		MEM.SBR \
		MRCICODE.SBR \
		RAWREAD.SBR \
		TEXT.SBR \
		DEBUG.SBR


BITMAP_DEP = d:\xbox\private\test\tools\rawread\bitmap.h \
	d:\xbox\private\test\tools\rawread\defs.h \
	d:\xbox\private\test\tools\rawread\file.h \
	d:\xbox\private\test\tools\rawread\mem.h


COMPRESS_DEP = d:\xbox\private\test\tools\rawread\compress.h \
	d:\xbox\private\test\tools\rawread\defs.h \
	d:\xbox\private\test\tools\rawread\mrcicode.h


DISKUTIL_DEP = d:\xbox\private\test\tools\rawread\debug.h \
	d:\xbox\private\test\tools\rawread\defs.h \
	d:\xbox\private\test\tools\rawread\diskutil.h \
	d:\xbox\private\test\tools\rawread\mem.h


FATUTIL_DEP = d:\xbox\private\test\tools\rawread\bitmap.h \
	d:\xbox\private\test\tools\rawread\defs.h \
	d:\xbox\private\test\tools\rawread\bootsec.h \
	d:\xbox\private\test\tools\rawread\debug.h \
	d:\xbox\private\test\tools\rawread\diskutil.h \
	d:\xbox\private\test\tools\rawread\fatutil.h \
	d:\xbox\private\test\tools\rawread\image.h \
	d:\xbox\private\test\tools\rawread\mem.h \
	d:\xbox\private\test\tools\rawread\text.h


FILE_DEP = d:\xbox\private\test\tools\rawread\win32\winmisc.h \
	d:\xbox\private\test\tools\rawread\defs.h \
	d:\xbox\private\test\tools\rawread\diskutil.h \
	d:\xbox\private\test\tools\rawread\image.h \
	d:\xbox\private\test\tools\rawread\bitmap.h \
	d:\xbox\private\test\tools\rawread\bootsec.h \
	d:\xbox\private\test\tools\rawread\file.h \
	d:\xbox\private\test\tools\rawread\text.h \
	d:\xbox\private\test\tools\rawread\debug.h


IMAGE_DEP = d:\xbox\private\test\tools\rawread\bitmap.h \
	d:\xbox\private\test\tools\rawread\defs.h \
	d:\xbox\private\test\tools\rawread\bootsec.h \
	d:\xbox\private\test\tools\rawread\diskutil.h \
	d:\xbox\private\test\tools\rawread\image.h \
	d:\xbox\private\test\tools\rawread\file.h \
	d:\xbox\private\test\tools\rawread\mem.h \
	d:\xbox\private\test\tools\rawread\text.h \
	d:\xbox\private\test\tools\rawread\debug.h \
	d:\xbox\private\test\tools\rawread\win32\winmisc.h


MEM_DEP = d:\xbox\private\test\tools\rawread\defs.h \
	d:\xbox\private\test\tools\rawread\mem.h


MRCICODE_DEP = d:\xbox\private\test\tools\rawread\defs.h \
	d:\xbox\private\test\tools\rawread\mrcicode.h


RAWREAD_DEP = d:\xbox\private\test\tools\rawread\win32\reg.h \
	d:\xbox\private\test\tools\rawread\defs.h \
	d:\xbox\private\test\tools\rawread\image.h \
	d:\xbox\private\test\tools\rawread\bitmap.h \
	d:\xbox\private\test\tools\rawread\bootsec.h \
	d:\xbox\private\test\tools\rawread\diskutil.h \
	d:\xbox\private\test\tools\rawread\win32\winmisc.h \
	d:\xbox\private\test\tools\rawread\compress.h \
	d:\xbox\private\test\tools\rawread\debug.h \
	d:\xbox\private\test\tools\rawread\fatutil.h \
	d:\xbox\private\test\tools\rawread\file.h \
	d:\xbox\private\test\tools\rawread\mem.h \
	d:\xbox\private\test\tools\rawread\text.h


TEXT_DEP = d:\xbox\private\test\tools\rawread\bootsec.h \
	d:\xbox\private\test\tools\rawread\defs.h \
	d:\xbox\private\test\tools\rawread\diskutil.h \
	d:\xbox\private\test\tools\rawread\image.h \
	d:\xbox\private\test\tools\rawread\bitmap.h \
	d:\xbox\private\test\tools\rawread\text.h \
	d:\xbox\private\test\tools\rawread\debug.h


DEBUG_DEP = d:\xbox\private\test\tools\rawread\debug.h \
	d:\xbox\private\test\tools\rawread\text.h \
	d:\xbox\private\test\tools\rawread\defs.h \
	d:\xbox\private\test\tools\rawread\image.h


all:	$(PROJ).EXE $(PROJ).BSC

BITMAP.OBJ:	BITMAP.C $(BITMAP_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c BITMAP.C

COMPRESS.OBJ:	COMPRESS.C $(COMPRESS_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c COMPRESS.C

DISKUTIL.OBJ:	DISKUTIL.C $(DISKUTIL_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c DISKUTIL.C

FATUTIL.OBJ:	FATUTIL.C $(FATUTIL_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c FATUTIL.C

FILE.OBJ:	FILE.C $(FILE_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c FILE.C

IMAGE.OBJ:	IMAGE.C $(IMAGE_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c IMAGE.C

MEM.OBJ:	MEM.C $(MEM_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c MEM.C

MRCICODE.OBJ:	MRCICODE.C $(MRCICODE_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c MRCICODE.C

RAWREAD.OBJ:	RAWREAD.C $(RAWREAD_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c RAWREAD.C

TEXT.OBJ:	TEXT.C $(TEXT_DEP)
	$(CC) $(CFLAGS) $(CCREATEPCHFLAG) /c TEXT.C

DEBUG.OBJ:	DEBUG.C $(DEBUG_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c DEBUG.C

$(PROJ).EXE::	BITMAP.OBJ COMPRESS.OBJ DISKUTIL.OBJ FATUTIL.OBJ FILE.OBJ IMAGE.OBJ \
	MEM.OBJ MRCICODE.OBJ RAWREAD.OBJ TEXT.OBJ DEBUG.OBJ $(OBJS_EXT) $(DEFFILE)
	echo >NUL @<<$(PROJ).CRF
BITMAP.OBJ +
COMPRESS.OBJ +
DISKUTIL.OBJ +
FATUTIL.OBJ +
FILE.OBJ +
IMAGE.OBJ +
MEM.OBJ +
MRCICODE.OBJ +
RAWREAD.OBJ +
TEXT.OBJ +
DEBUG.OBJ +
$(OBJS_EXT)
$(PROJ).EXE
$(MAPFILE)
c:\progra~1\msvc15\lib\+
$(LIBS)
$(DEFFILE);
<<
	link $(LFLAGS) @$(PROJ).CRF

run: $(PROJ).EXE
	$(PROJ) $(RUNFLAGS)


$(PROJ).BSC: $(SBRS)
	bscmake @<<
/o$@ $(SBRS)
<<
