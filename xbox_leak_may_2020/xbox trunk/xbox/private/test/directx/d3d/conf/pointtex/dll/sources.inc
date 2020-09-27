!include ..\..\..\..\sources.inc

TARGETNAME=PointTex
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

OBJLIBFILES=\
    $(OBJLIBFILES) \
    $(TEST_LIB_PATH)\l_pnttex.lib

SOURCES=\
    include.cpp
