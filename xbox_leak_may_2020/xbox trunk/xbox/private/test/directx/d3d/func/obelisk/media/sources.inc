!include ..\..\..\..\sources.inc

TARGETNAME=m_obelsk
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

USER_C_FLAGS=$(USER_C_FLAGS) /Zm200

SOURCES=\
    ground.bmp.cpp \
    clouds.bmp.cpp \
    obelisk.bmp.cpp