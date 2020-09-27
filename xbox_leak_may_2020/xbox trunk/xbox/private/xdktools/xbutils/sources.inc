TARGETPATH=obj
TARGETTYPE=PROGRAM
UMTYPE=console
UMENTRY=main

INCLUDES=..\xbfile

LINKLIBS=$(LINKLIBS) \
    $(SDK_LIB_PATH)\xboxdbg.lib \
    ..\xbfile\obj\*\xbfile.lib \
    $(WSDK_LIB_PATH)\kernel32.lib \
    $(WSDK_LIB_PATH)\advapi32.lib \
    $(WSDK_LIB_PATH)\user32.lib

USE_MSVCRT=1

MSC_WARNING_FLAGS=/W3 /WX
