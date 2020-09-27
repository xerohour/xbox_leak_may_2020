!include ..\..\..\..\sources.inc

TARGETNAME=palette0
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

OBJLIBFILES=\
    $(OBJLIBFILES) \
    $(TEST_LIB_PATH)\l_palet0.lib

SOURCES=\
    include.cpp
