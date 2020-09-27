!include ..\..\..\..\sources.inc

TARGETNAME=m_textr4
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

USER_C_FLAGS=$(USER_C_FLAGS) /Zm180

SOURCES=\
    brikmip0.bmp.cpp \
    brikmip1.bmp.cpp \
    brikmip2.bmp.cpp \
    brikmip3.bmp.cpp \
    brikmip4.bmp.cpp \
    brikmip5.bmp.cpp \
    brikmip6.bmp.cpp \
    brikmip7.bmp.cpp \
    brikmip8.bmp.cpp
