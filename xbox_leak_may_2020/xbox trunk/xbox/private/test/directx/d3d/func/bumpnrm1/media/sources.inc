!include ..\..\..\..\sources.inc

TARGETNAME=m_bumpn1
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

USER_C_FLAGS=$(USER_C_FLAGS) /Zm180

SOURCES=\
    earth.bmp.cpp \
    earthbump.bmp.cpp