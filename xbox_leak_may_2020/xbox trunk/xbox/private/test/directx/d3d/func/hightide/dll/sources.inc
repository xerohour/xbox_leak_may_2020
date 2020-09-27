!include ..\..\..\..\sources.inc

TARGETNAME=hightide
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

OBJLIBFILES=\
    $(OBJLIBFILES) \
    $(TEST_LIB_PATH)\l_htide.lib

SOURCES=\
    include.cpp
