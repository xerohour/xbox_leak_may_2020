!include ..\..\..\..\sources.inc

TARGETNAME=m_htide
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

USER_C_FLAGS=$(USER_C_FLAGS) /Zm180

SOURCES=\
    beach.bmp.cpp \
    stars.bmp.cpp \
    rockwall.bmp.cpp \
    redbrick.bmp.cpp
