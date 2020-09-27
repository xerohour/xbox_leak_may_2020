!include ..\..\..\..\sources.inc

TARGETNAME=persistd
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

OBJLIBFILES=\
    $(OBJLIBFILES) \
    $(TEST_LIB_PATH)\l_prsist.lib

SOURCES=\
    include.cpp
