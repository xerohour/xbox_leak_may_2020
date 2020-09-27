!include ..\..\..\..\sources.inc

TARGETNAME=wcmp
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

OBJLIBFILES=\
    $(OBJLIBFILES) \
    $(TEST_LIB_PATH)\l_wcmp.lib

SOURCES=\
    include.cpp
