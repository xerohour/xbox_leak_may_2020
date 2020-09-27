!include ..\..\..\..\sources.inc

TARGETNAME=m_mstag4
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

USER_C_FLAGS=$(USER_C_FLAGS) /Zm180

SOURCES=\
    texture1.bmp.cpp \
    texture2.bmp.cpp \
    texture3.bmp.cpp \
    texture4.bmp.cpp \
    texture5.bmp.cpp \
    texture6.bmp.cpp \
    texture7.bmp.cpp \
    texture8.bmp.cpp \
    backgrnd.bmp.cpp
