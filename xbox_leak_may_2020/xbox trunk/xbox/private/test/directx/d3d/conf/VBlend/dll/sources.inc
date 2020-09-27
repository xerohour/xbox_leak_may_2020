!include ..\..\..\..\sources.inc

TARGETNAME=VBlend
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

OBJLIBFILES=\
    $(OBJLIBFILES) \
    $(TEST_LIB_PATH)\l_vblend.lib

SOURCES=\
    include.cpp
