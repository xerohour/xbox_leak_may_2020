!include ..\..\..\..\sources.inc

TARGETNAME=reflect0
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

OBJLIBFILES=\
    $(OBJLIBFILES) \
    $(TEST_LIB_PATH)\l_rflct0.lib

SOURCES=\
    include.cpp
