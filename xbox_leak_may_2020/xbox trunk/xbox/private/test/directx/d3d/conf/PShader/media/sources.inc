!include ..\..\..\..\sources.inc

TARGETNAME=m_pshade
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

USER_C_FLAGS=$(USER_C_FLAGS) /Zm600

SOURCES=\
    bem1.bmp.cpp \
    bem2.bmp.cpp \
    bem3.bmp.cpp \
    checker.bmp.cpp \
    checker2.bmp.cpp \
    colorchecker.bmp.cpp \
    dp3.bmp.cpp \
    dxlogo.bmp.cpp \
    sky0.bmp.cpp \
    sky1.bmp.cpp \
    sky2.bmp.cpp \
    sky3.bmp.cpp \
    sky4.bmp.cpp \
    sky5.bmp.cpp
