!include ..\..\..\..\sources.inc

TARGETNAME=chessbrd
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

OBJLIBFILES=\
    $(OBJLIBFILES) \
    $(TEST_LIB_PATH)\l_chess.lib

SOURCES=\
    include.cpp
