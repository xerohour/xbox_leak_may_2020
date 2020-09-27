
TARGETPATH=obj
TARGETTYPE=PROGRAM
UMTYPE=xboxapp
PROJNAME=DukeProd

BROWSER_INFO=1

C_DEFINES=$(C_DEFINES)

XE_FLAGS   = $(XE_FLAGS) /TESTNAME:$(PROJNAME)  /INITFLAGS:0x00000000

XNOHARDISK=1

USE_LIBCMT=1


INCLUDES=\
	$(SDKDIR); \
	$(BASEDIR)\private\test\inc;\
    $(BASEDIR)\private\ntos\inc;\
	$(PRIVATE_INC_PATH);\
	$(BASEDIR)\private\test\inc\cpx;\
	$(SDK_INC_PATH);\
	$(BASEDIR)\private\test\common\include;\
    $(INCLUDES)

SOURCES=\
    dukeprod2.cpp\
	TestControls.cpp \
	base.cpp

LINKLIBS=\
	$(XDK_LIB_PATH)\d3d8$(D).lib                \
    $(XDK_LIB_PATH)\d3dx8$(D).lib				\
    $(XDK_LIB_PATH)\dsound$(D).lib              \
    $(XDK_LIB_PATH)\xgraphics$(D).lib			\
    $(SDK_LIB_PATH)\xfont$(D).lib               \
    $(BASEDIR)\private\test\lib\*\cpxlib.lib    

