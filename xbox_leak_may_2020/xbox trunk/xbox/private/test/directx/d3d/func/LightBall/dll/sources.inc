!include ..\..\..\..\sources.inc

TARGETNAME=LiteBall
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

OBJLIBFILES=\
    $(OBJLIBFILES) \
    $(TEST_LIB_PATH)\l_ltball.lib

SOURCES=\
    include.cpp
