!include ..\..\..\..\sources.inc

TARGETNAME=m_alpal1
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

USER_C_FLAGS=$(USER_C_FLAGS) /Zm180

SOURCES=\
    texture8.bmp.cpp \
    backgrnd.bmp.cpp \
