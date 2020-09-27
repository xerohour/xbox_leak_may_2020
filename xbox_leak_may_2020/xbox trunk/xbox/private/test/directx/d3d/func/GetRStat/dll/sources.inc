!include ..\..\..\..\sources.inc

TARGETNAME=GetRStat
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

OBJLIBFILES=\
    $(OBJLIBFILES) \
    $(TEST_LIB_PATH)\l_grstat.lib

SOURCES=\
    include.cpp
