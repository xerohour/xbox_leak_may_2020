#
# Include this from your sources file when building a Win32 app
#

!include sources.inc

UMTYPE = windows

TARGETPATH=$(BASEDIR)\public\wsdk\lib

INCLUDES = \
    $(BASEDIR)\public\wsdk\inc;\
    $(BASEDIR)\public\wsdk\inc\mfc42;\
    $(BASEDIR)\public\sdk\inc;\
    $(INCLUDES)
