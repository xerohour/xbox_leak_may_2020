!include ..\..\..\..\sources.inc

TARGETNAME=stencil
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

OBJLIBFILES=\
    $(OBJLIBFILES) \
    $(TEST_LIB_PATH)\l_stncil.lib

SOURCES=\
    include.cpp
