!include ..\..\..\..\sources.inc

TARGETNAME=MultiLit
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

OBJLIBFILES=\
    $(OBJLIBFILES) \
    $(TEST_LIB_PATH)\l_mullit.lib

SOURCES=\
    include.cpp
