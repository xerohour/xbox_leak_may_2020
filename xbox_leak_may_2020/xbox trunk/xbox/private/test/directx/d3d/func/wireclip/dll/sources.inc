!include ..\..\..\..\sources.inc

TARGETNAME=Wirefram
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

OBJLIBFILES=\
    $(OBJLIBFILES) \
    $(TEST_LIB_PATH)\l_wclip.lib

SOURCES=\
    include.cpp
