!include ..\..\..\..\sources.inc

TARGETNAME=mapping
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

OBJLIBFILES=\
    $(OBJLIBFILES) \
    $(TEST_LIB_PATH)\l_maping.lib

SOURCES=\
    include.cpp
