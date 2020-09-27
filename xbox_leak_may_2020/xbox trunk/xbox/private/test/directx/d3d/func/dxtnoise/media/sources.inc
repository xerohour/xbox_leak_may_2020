!include ..\..\..\..\sources.inc

TARGETNAME=m_dnoise
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

USER_C_FLAGS=$(USER_C_FLAGS) /Zm180

SOURCES=\
    gradient.bmp.cpp