!include ..\..\..\..\sources.inc

TARGETNAME=perspect
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

OBJLIBFILES=\
    $(OBJLIBFILES) \
    $(TEST_LIB_PATH)\l_perspc.lib

SOURCES=\
    include.cpp
