!include ..\..\..\..\sources.inc

TARGETNAME=pslights
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

OBJLIBFILES=\
    $(OBJLIBFILES) \
    $(TEST_LIB_PATH)\l_pslite.lib

SOURCES=\
    include.cpp
