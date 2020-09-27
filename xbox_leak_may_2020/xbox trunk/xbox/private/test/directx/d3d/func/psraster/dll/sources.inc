!include ..\..\..\..\sources.inc

TARGETNAME=psraster
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

OBJLIBFILES=\
    $(OBJLIBFILES) \
    $(TEST_LIB_PATH)\l_psrast.lib

SOURCES=\
    include.cpp
