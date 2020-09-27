!include ..\..\..\sources.inc

TARGETNAME=d3dbase
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

!if defined(USE_VC7)
C_DEFINES=$(C_DEFINES) -Zm200
!endif

SOURCES=\
    conio.cpp \
    display.cpp \
    enum.cpp \
    fontmap.cpp \
    input.cpp \
    light.cpp \
    mesh.cpp \
    misc.cpp \
    noise.cpp \
    object.cpp \
    palette.cpp \
    rsrcdata.cpp \
    scene.cpp \
    texture.cpp \
    util.cpp \
    vibuffer.cpp \
    vpshader.cpp \
    consolbk.bmp.cpp
