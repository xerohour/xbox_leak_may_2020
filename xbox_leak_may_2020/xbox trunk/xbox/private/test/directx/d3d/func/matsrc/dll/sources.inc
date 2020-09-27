!include ..\..\..\..\sources.inc

TARGETNAME=matsrc
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

OBJLIBFILES=\
    $(OBJLIBFILES) \
    $(TEST_LIB_PATH)\l_matsrc.lib

SOURCES=\
    include.cpp
