!include ..\..\..\..\sources.inc

TARGETNAME=m_basic
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

USER_C_FLAGS=$(USER_C_FLAGS) /Zm200

SOURCES=\
    dxlogo.bmp.cpp
