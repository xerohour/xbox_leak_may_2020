!include ..\..\..\sources.inc

TARGETNAME=d3dlocus
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

!if defined(USE_VC7)
C_DEFINES=$(C_DEFINES) -Zm200
!endif

SOURCES=\
    basetex.cpp \
    client.cpp \
    conio.cpp \
    cubetex.cpp \
    device.cpp \
    direct3d.cpp \
    display.cpp \
    enum.cpp \
    fontmap.cpp \
    indxbuff.cpp \
    input.cpp \
    light.cpp \
    log.cpp \
    mesh.cpp \
    misc.cpp \
    noise.cpp \
    object.cpp \
    pal.cpp \
    palette.cpp \
    resource.cpp \
    rsrcdata.cpp \
    scene.cpp \
    surface.cpp \
    swapchn.cpp \
    tex.cpp \
    texture.cpp \
    util.cpp \
    vertbuff.cpp \
    vibuffer.cpp \
    voltex.cpp \
    volume.cpp \
    vpshader.cpp \
    consolbk.bmp.cpp
