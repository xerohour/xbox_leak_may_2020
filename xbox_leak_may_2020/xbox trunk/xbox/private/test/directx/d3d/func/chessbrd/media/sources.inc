!include ..\..\..\..\sources.inc

TARGETNAME=m_chess
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

USER_C_FLAGS=$(USER_C_FLAGS) /Zm600

SOURCES=\
    chess.bmp.cpp \
    edge.bmp.cpp \
    pine.bmp.cpp \
    pine2.bmp.cpp \
    lpawn.xdg.cpp \
    lrook.xdg.cpp \
    lbishop.xdg.cpp \
    sknight.xdg.cpp \
    lqueen.xdg.cpp \
    lking.xdg.cpp
