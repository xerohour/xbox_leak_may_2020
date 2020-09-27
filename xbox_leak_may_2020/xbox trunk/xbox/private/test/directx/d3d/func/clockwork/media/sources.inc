!include ..\..\..\..\sources.inc

TARGETNAME=m_clwork
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

USER_C_FLAGS=$(USER_C_FLAGS) /Zm600

SOURCES=\
    backcogs.bmp.cpp \
    cog0.bmp.cpp \
    cog1.bmp.cpp \
    cog2.bmp.cpp \
    cog3.bmp.cpp \
    cog4.bmp.cpp \
    cog5.bmp.cpp \
    cog6.bmp.cpp \
    cog7.bmp.cpp \
    cog8.bmp.cpp \
    cog9.bmp.cpp \
    cog10.bmp.cpp \
    cog11.bmp.cpp \
    cog12.bmp.cpp \
    cog13.bmp.cpp \
    cog14.bmp.cpp \
    cog15.bmp.cpp \
    cog16.bmp.cpp \
    shaft0.bmp.cpp \
    shaft1.bmp.cpp \
    shaft2.bmp.cpp \
    shaft3.bmp.cpp \
    shaft4.bmp.cpp \
    shaft5.bmp.cpp \
    shaft6.bmp.cpp
