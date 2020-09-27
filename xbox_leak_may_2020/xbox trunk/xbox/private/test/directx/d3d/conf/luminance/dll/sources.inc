!include ..\..\..\..\sources.inc

TARGETNAME=luminanc
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

OBJLIBFILES=\
    $(OBJLIBFILES) \
    $(TEST_LIB_PATH)\l_lumin.lib

SOURCES=\
    include.cpp
