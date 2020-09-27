# Microsoft Visual C++ generated build script - Do not modify

PROJ = RAWINFO
DEBUG = 0
PROGTYPE = 6
CALLER = 
ARGS = 
DLLS = 
D_RCDEFINES = -d_DEBUG
R_RCDEFINES = -dNDEBUG
ORIGIN = MSVC
ORIGIN_VER = 1.00
PROJPATH = D:\TESTS\RAWREAD\
USEMFC = 0
CC = cl
CPP = cl
CXX = cl
CCREATEPCHFLAG = 
CPPCREATEPCHFLAG = 
CUSEPCHFLAG = 
CPPUSEPCHFLAG = 
FIRSTC = BITMAP.C    
FIRSTCPP =             
RC = rc
CFLAGS_D_DEXE = /nologo /G2 /Zp1 /W3 /Zi /AL /O1 /D "_DEBUG" /D "_DOS" /FR /Fd"RAWINFO.PDB"
CFLAGS_R_DEXE = /nologo /Gs /G2 /Zp1 /W3 /AL /O1 /Ox /D "NDEBUG" /D "_DOS" /FR 
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
		FILE.SBR \
		IMAGE.SBR \
		TEXT.SBR \
		RAWINFO.SBR \
		DISKUTIL.SBR


BITMAP_DEP = d:\tests\rawread\bitmap.h \
	d:\tests\rawread\defs.h \
	d:\tests\rawread\file.h \
	d:\tests\rawread\mem.h


FILE_DEP = d:\tests\rawread\win32\winmisc.h \
	d:\tests\rawread\defs.h \
	d:\tests\rawread\diskutil.h \
	d:\tests\rawread\image.h \
	d:\tests\rawread\bitmap.h \
	d:\tests\rawread\bootsec.h \
	d:\tests\rawread\file.h \
	d:\tests\rawread\text.h


IMAGE_DEP = d:\tests\rawread\bitmap.h \
	d:\tests\rawread\defs.h \
	d:\tests\rawread\bootsec.h \
	d:\tests\rawread\diskutil.h \
	d:\tests\rawread\image.h \
	d:\tests\rawread\file.h \
	d:\tests\rawread\mem.h \
	d:\tests\rawread\text.h \
	d:\tests\rawread\win32\winmisc.h


TEXT_DEP = d:\tests\rawread\defs.h \
	d:\tests\rawread\text.h \
	d:\tests\rawread\image.h


RAWINFO_DEP = d:\tests\rawread\bitmap.h \
	d:\tests\rawread\defs.h \
	d:\tests\rawread\file.h \
	d:\tests\rawread\image.h \
	d:\tests\rawread\bootsec.h \
	d:\tests\rawread\diskutil.h \
	d:\tests\rawread\text.h


DISKUTIL_DEP = d:\tests\rawread\win32\debug.h \
	d:\tests\rawread\defs.h \
	d:\tests\rawread\diskutil.h \
	d:\tests\rawread\mem.h


all:	$(PROJ).EXE $(PROJ).BSC

BITMAP.OBJ:	BITMAP.C $(BITMAP_DEP)
	$(CC) $(CFLAGS) $(CCREATEPCHFLAG) /c BITMAP.C

FILE.OBJ:	FILE.C $(FILE_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c FILE.C

IMAGE.OBJ:	IMAGE.C $(IMAGE_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c IMAGE.C

TEXT.OBJ:	TEXT.C $(TEXT_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c TEXT.C

RAWINFO.OBJ:	RAWINFO.C $(RAWINFO_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c RAWINFO.C

DISKUTIL.OBJ:	DISKUTIL.C $(DISKUTIL_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c DISKUTIL.C

$(PROJ).EXE::	BITMAP.OBJ FILE.OBJ IMAGE.OBJ TEXT.OBJ RAWINFO.OBJ DISKUTIL.OBJ $(OBJS_EXT) $(DEFFILE)
	echo >NUL @<<$(PROJ).CRF
BITMAP.OBJ +
FILE.OBJ +
IMAGE.OBJ +
TEXT.OBJ +
RAWINFO.OBJ +
DISKUTIL.OBJ +
$(OBJS_EXT)
$(PROJ).EXE
$(MAPFILE)
d:\msvc15\lib\+
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
