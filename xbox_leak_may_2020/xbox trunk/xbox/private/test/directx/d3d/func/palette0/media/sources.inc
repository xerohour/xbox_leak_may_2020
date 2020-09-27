!include ..\..\..\..\sources.inc

TARGETNAME=m_palet0
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

USER_C_FLAGS=$(USER_C_FLAGS) /Zm180

SOURCES=\
    brick1_5.bmp.cpp \
    brick1_6.bmp.cpp \
    brick2_1.bmp.cpp \
    brick2_2.bmp.cpp \
    brick2_3.bmp.cpp \
    brick2_4.bmp.cpp \
    brick2_5.bmp.cpp \
    brick2_6.bmp.cpp \
    brick3_1.bmp.cpp \
    brick3_2.bmp.cpp \
    brick3_3.bmp.cpp \
    brick3_4.bmp.cpp \
    brick3_5.bmp.cpp \
    brick3_6.bmp.cpp \
    brick0_1.bmp.cpp \
    brick0_2.bmp.cpp \
    brick0_3.bmp.cpp \
    brick0_4.bmp.cpp \
    brick0_5.bmp.cpp \
    brick0_6.bmp.cpp \
    brick1_1.bmp.cpp \
    brick1_2.bmp.cpp \
    brick1_3.bmp.cpp \
    brick1_4.bmp.cpp
