MAJORCOMP=d3dx

!ifndef DXGROOT
DXGROOT = $(BASEDIR)\private\windows\directx\dxg
!endif

!ifndef D3DXROOT
D3DXROOT = $(DXGROOT)\d3dx8
!endif

C_DEFINES = $(C_DEFINES) /D__D3DX_INTERNAL__

386_STDCALL = 0

INCLUDES =\
    $(INCLUDES); \
    $(D3DXROOT)\inc; \
    $(D3DXROOT)\core

