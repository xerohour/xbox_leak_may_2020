!include ..\..\..\..\sources.inc

TARGETNAME=m_inloop
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

USER_C_FLAGS=$(USER_C_FLAGS) /Zm180

SOURCES=\
    texture0.bmp.cpp \
    texture1.bmp.cpp \
    texture2.bmp.cpp \
    texture3.bmp.cpp
