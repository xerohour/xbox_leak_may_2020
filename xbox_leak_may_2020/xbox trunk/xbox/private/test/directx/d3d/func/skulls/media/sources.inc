!include ..\..\..\..\sources.inc

TARGETNAME=m_skulls
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

USER_C_FLAGS=$(USER_C_FLAGS) /Zm250

SOURCES=\
    skull.xdg.cpp
