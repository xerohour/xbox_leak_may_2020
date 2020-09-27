
#
# The MAJORCOMP and MINORCOMP variables are defined
# so that $(MAJORCOMP)$(MINORCOMP)filename can be used in
# cross compiling to provide unique filenames in a flat namespace.
#

MAJORCOMP=windows
MINORCOMP=media

TARGETNAME=XVOICE$(D)
TARGETPATH=$(BASEDIR)\public\sdk\lib
TARGETTYPE=LIBRARY

#
# The INCLUDES variable specifies any include paths that are specific to
# this source directory.  Separate multiple directory paths with single
# semicolons.  Relative path specifications are okay.
#

INCLUDES= ..\inc;$(BASEDIR)\private\ntos\inc;$(BASEDIR)\public\ddk\inc;\
    ..\..\inc;..\..\dxvutils;$(BASEDIR)\public\sdk\inc;\..\..\..\core;..\..\..\common;..\..\..\protocol;


SOURCES=..\dvcleng.cpp \
        ..\vplayer.cpp \
	..\dvengine.cpp \
	..\mixutils.cpp \
	..\dvclient.cpp \
	..\dvdxtran.cpp \
	..\dvrecsub.cpp \
	..\dvsereng.cpp \
	..\dvserver.cpp \
	..\dvshared.cpp \
	..\trnotify.cpp \
        ..\dvcreate.cpp\
	..\dvsndt.cpp \
	..\vnametbl.cpp\
        ..\frame.cpp\
        ..\inqueue2.cpp\
        ..\va.cpp\
        ..\sndutils.cpp


LINKLIBS = $(SDK_LIB_PATH)\voxcodec$(D).lib

#
# Next specify options for the compiler.
#

C_DEFINES= $(C_DEFINES) /DDIRECTSOUND_VERSION=0x0800

C_DEFINES = $(C_DEFINES) -DKERNEL_MODE 

MSC_OPTIMIZATION=$(MSC_OPTIMIZATION) /FAcs

!if $(FREEBUILD)
C_DEFINES=$(C_DEFINES) -DASSUME_VALID_PARAMETERS
!else
C_DEFINES=$(C_DEFINES) -DDEBUG
MSC_OPTIMIZATION=$(MSC_OPTIMIZATION) /Od
!endif

USER_C_FLAGS=-nologo $(USER_C_FLAGS)

