TARGETNAME=AATest_$(NAME)
TARGETPATH=obj
TARGETTYPE=PROGRAM
UMTYPE=xboxapp
LINK_DSSTDFX=1
USE_STL=1
XE_FLAGS=/TESTID:0xA7049955 /INITFLAGS:3 /TESTNAME:$(TARGETNAME)
USE_LIBCMT=1
SYNCHRONIZE_DRAIN=1


INCLUDES =         \
                   $(BASEDIR)\private\inc;                    \
                   $(BASEDIR)\private\test\inc;               \
                   $(BASEDIR)\private\test\multimedia\inc;    \
                   $(SDK_INC_PATH);                           

TESTLIBS=$(BASEDIR)\private\test\lib\$(TARGET_DIRECTORY)

TARGETLIBS= \
           $(SDK_LIB_PATH)\dsound$(D).lib \
           $(SDK_LIB_PATH)\dmusic$(D).lib \
           $(TESTLIBS)\medialoader.lib    \
           $(LIBRARY_PATH)\xnet$(D).lib \
           $(TESTLIBS)\xtestlib.lib \


C_DEFINES=$(C_DEFINES) -DAPPNAME=\"$(NAME)\"

NTTARGETFILE0 = $(O)\CREATEINSTANCE.CPP obj\i386\FILELIST.TXT
