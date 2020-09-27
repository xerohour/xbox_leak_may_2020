!include ..\..\..\..\sources.inc

TARGETNAME=m_sblock
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

USER_C_FLAGS=$(USER_C_FLAGS) /Zm200

SOURCES=\
    poko0.bmp.cpp \
    poko1.bmp.cpp \
    poko2.bmp.cpp \
    poko3.bmp.cpp \
    poko4.bmp.cpp \
    poko5.bmp.cpp \
    poko6.bmp.cpp \
    poko7.bmp.cpp \
    rocks.bmp.cpp \
    checker.bmp.cpp
