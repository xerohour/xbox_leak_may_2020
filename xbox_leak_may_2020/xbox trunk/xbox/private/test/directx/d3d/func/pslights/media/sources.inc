!include ..\..\..\..\sources.inc

TARGETNAME=m_pslite
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

USER_C_FLAGS=$(USER_C_FLAGS) /Zm180

SOURCES=\
    base.bmp.cpp \
    bump.bmp.cpp \
    light1.vsh.cpp \
    light2.vsh.cpp