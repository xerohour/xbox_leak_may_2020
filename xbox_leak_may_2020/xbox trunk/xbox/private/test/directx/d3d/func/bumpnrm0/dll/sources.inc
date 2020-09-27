!include ..\..\..\..\sources.inc

TARGETNAME=bumpnrm0
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

OBJLIBFILES=\
    $(OBJLIBFILES) \
    $(TEST_LIB_PATH)\l_bumpn0.lib

SOURCES=\
    include.cpp
