!include ..\..\..\..\sources.inc

TARGETNAME=m_ltball
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

USER_C_FLAGS=$(USER_C_FLAGS) /Zm180

SOURCES=\
    win95.bmp.cpp
