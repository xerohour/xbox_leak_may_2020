!include ..\..\..\..\sources.inc

TARGETNAME=ptsprite
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

OBJLIBFILES=\
    $(OBJLIBFILES) \
    $(TEST_LIB_PATH)\l_psprit.lib

SOURCES=\
    include.cpp
