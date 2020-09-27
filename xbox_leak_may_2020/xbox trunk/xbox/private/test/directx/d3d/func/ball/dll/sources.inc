!include ..\..\..\..\sources.inc

TARGETNAME=Ball
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

OBJLIBFILES=\
    $(OBJLIBFILES) \
    $(TEST_LIB_PATH)\l_ball.lib

SOURCES=\
    include.cpp
