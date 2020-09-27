!include ..\..\..\..\sources.inc

TARGETNAME=m_tunnel
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

USER_C_FLAGS=$(USER_C_FLAGS) /Zm180

SOURCES=\
    checker.bmp.cpp \
    errorbox.bmp.cpp
