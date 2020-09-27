!include ..\..\..\..\sources.inc

TARGETNAME=m_mstag1
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

USER_C_FLAGS=$(USER_C_FLAGS) /Zm180

SOURCES=\
    texture1.bmp.cpp \
    texture2.bmp.cpp