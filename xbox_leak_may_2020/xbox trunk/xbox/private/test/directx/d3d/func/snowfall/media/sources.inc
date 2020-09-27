!include ..\..\..\..\sources.inc

TARGETNAME=m_snow
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

USER_C_FLAGS=$(USER_C_FLAGS) /Zm180

SOURCES=\
    backgrnd.bmp.cpp \
    snow.bmp.cpp
