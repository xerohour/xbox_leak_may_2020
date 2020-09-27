!include ..\..\..\..\sources.inc

TARGETNAME=m_vbatch
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

USER_C_FLAGS=$(USER_C_FLAGS) /Zm180

SOURCES=\
    tex0.bmp.cpp \
    tex1.bmp.cpp \
    tex2.bmp.cpp \
    tex3.bmp.cpp \
    tex4.bmp.cpp \
    tex5.bmp.cpp \
    tex6.bmp.cpp \
    tex7.bmp.cpp
