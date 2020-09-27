!include ..\..\..\..\sources.inc

TARGETNAME=alphabnt
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

OBJLIBFILES=\
    $(OBJLIBFILES) \
    $(TEST_LIB_PATH)\l_albnt.lib

SOURCES=\
    include.cpp
