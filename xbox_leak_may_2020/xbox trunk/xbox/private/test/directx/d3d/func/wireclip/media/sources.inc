!include ..\..\..\..\sources.inc

TARGETNAME=m_wclip
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

USER_C_FLAGS=$(USER_C_FLAGS) /Zm200

SOURCES=\
    tclptex0.bmp.cpp \
    tclptex1.bmp.cpp \
    tclptex2.bmp.cpp \
    tclptex3.bmp.cpp \
    tclptex4.bmp.cpp \
    tclptex5.bmp.cpp \
    tclptex6.bmp.cpp \
    tclptex7.bmp.cpp
