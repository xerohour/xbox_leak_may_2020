!include ..\..\..\..\sources.inc

TARGETNAME=gammarmp
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

OBJLIBFILES=\
    $(OBJLIBFILES) \
    $(TEST_LIB_PATH)\l_gammar.lib

SOURCES=\
    include.cpp
