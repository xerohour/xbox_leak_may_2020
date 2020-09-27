!include ..\..\..\..\sources.inc

TARGETNAME=mipflter
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

OBJLIBFILES=\
    $(OBJLIBFILES) \
    $(TEST_LIB_PATH)\l_mipfil.lib

SOURCES=\
    include.cpp
