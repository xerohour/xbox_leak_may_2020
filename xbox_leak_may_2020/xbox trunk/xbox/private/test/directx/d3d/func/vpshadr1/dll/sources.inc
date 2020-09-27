!include ..\..\..\..\sources.inc

TARGETNAME=vpshadr1
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

OBJLIBFILES=\
    $(OBJLIBFILES) \
    $(TEST_LIB_PATH)\l_vpshd1.lib

SOURCES=\
    include.cpp
