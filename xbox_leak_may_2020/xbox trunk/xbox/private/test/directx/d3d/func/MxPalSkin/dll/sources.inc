!include ..\..\..\..\sources.inc

TARGETNAME=MPalSkin
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

OBJLIBFILES=\
    $(OBJLIBFILES) \
    $(TEST_LIB_PATH)\l_mxpskn.lib

SOURCES=\
    include.cpp
