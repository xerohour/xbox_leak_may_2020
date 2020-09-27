TARGETPATH=obj
TARGETTYPE=PROGRAM
TARGETNAME=AddOnlineUser
USE_LIBCMT=1
XE_FLAGS=$(XE_FLAGS) /TESTNAME:$(TARGETNAME) /TESTID:1235

MSC_WARNING_LEVEL=/W3 /WX /Ob1

!if defined(GENASM)
MSC_WARNING_LEVEL=$(MSC_WARNING_LEVEL) /Fa
!endif

!if $(FREEBUILD) == 0
MSC_OPTIMIZATION=/Odi /Ob1
!endif

C_DEFINES=$(C_DEFINES) -DNT -DXONLINE_BUILD_LIB$(LIBTAG)

LINKER_FLAGS=$(LINKER_FLAGS) /map /IGNORE:4099

INCLUDES=$(INCLUDES); \
    $(BASEDIR)\public\sdk\inc;\
    $(BASEDIR)\private\inc\crypto;\
    $(BASEDIR)\private\ntos\inc;\
    $(BASEDIR)\private\ntos\fatx;\
    $(BASEDIR)\private\ntos\xapi\inc;\
    $(BASEDIR)\private\ntos\xapi\k32;\
    $(BASEDIR)\private\ntos\net;\
    $(BASEDIR)\private\online;

SOURCES=\
    AddOnlineUser.cpp \


