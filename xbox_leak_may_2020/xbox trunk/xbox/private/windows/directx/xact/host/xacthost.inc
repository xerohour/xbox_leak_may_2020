#
# Common sources file for all XACT Win32 components
#

DXBASEDIR = $(BASEDIR)\private\windows\directx
DSBASEDIR = $(DXBASEDIR)\dsound
XACTBASEDIR = $(DXBASEDIR)\xact
XACTHOSTBASEDIR = $(XACTBASEDIR)\host
XACTHOSTOBJDIR = $(XACTHOSTBASEDIR)\obj

TARGETTYPE = LIBRARY
TARGETPATH = $(XACTHOSTOBJDIR)
UMTYPE = windows

NTDBGFILES = 1
USE_MAPSYM = 1

USER_C_FLAGS = $(USER_C_FLAGS) /FAcs

!if !$(FREEBUILD)
MSC_OPTIMIZATION = $(MSC_OPTIMIZATION) /Od
!endif

INCLUDES =\
    $(BASEDIR)\public\wsdk\inc;\
    $(BASEDIR)\public\wsdk\inc\mfc42;\
    $(BASEDIR)\public\sdk\inc;\
    $(DSBASEDIR)\common;\
    $(XACTBASEDIR)\common\wavbndlr;