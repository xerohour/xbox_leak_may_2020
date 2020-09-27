!include ..\..\..\..\sources.inc

TARGETNAME=m_stncl0
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

USER_C_FLAGS=$(USER_C_FLAGS) /Zm200

SOURCES=\
    texture.bmp.cpp
