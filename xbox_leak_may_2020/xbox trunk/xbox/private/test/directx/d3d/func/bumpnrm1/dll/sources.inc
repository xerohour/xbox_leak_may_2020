!include ..\..\..\..\sources.inc

TARGETNAME=bumpnrm1
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

OBJLIBFILES=\
    $(OBJLIBFILES) \
    $(TEST_LIB_PATH)\l_bumpn1.lib

SOURCES=\
    include.cpp
