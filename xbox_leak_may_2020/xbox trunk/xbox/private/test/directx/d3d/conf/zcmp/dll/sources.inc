!include ..\..\..\..\sources.inc

TARGETNAME=zcmp
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

OBJLIBFILES=\
    $(OBJLIBFILES) \
    $(TEST_LIB_PATH)\l_zcmp.lib

SOURCES=\
    include.cpp
