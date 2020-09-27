!include ..\..\..\..\sources.inc

TARGETNAME=texgen
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

OBJLIBFILES=\
    $(OBJLIBFILES) \
    $(TEST_LIB_PATH)\l_texgen.lib

SOURCES=\
    include.cpp
