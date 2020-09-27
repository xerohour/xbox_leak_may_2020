# Copyright (c) 1998 Microsoft Corporation
!if [set PATH=;]
!endif
!if [set INCLUDE=;]
!endif
!if [set LIB=;]
!endif
PATH=;
INCLUDE=;
LIB=;

PATH=c:\program files\devstudio\vc\bin;c:\program files\devstudio\sharedide\bin;e:\root\dev\tools\masm61
INCLUDE=c:\program files\devstudio\vc\include;e:\nt\public\sdk\inc
LIB=e:\root\dev\sdk\lib;c:\program files\devstudio\vc\lib
LIBS=user32.lib advapi32.lib kernel32.lib winmm.lib ole32.lib uuid.lib rpcrt4.lib 

CFLAGS=-c -D_DEBUG -Zi -Od -Gz -Fc -MD
#CFLAGS=-G3 -Ox -c

{}.cpp{}.obj:
    cl $(CFLAGS) $(@B).cpp

OBJS=dm32dll.obj enumport.obj dmeport.obj dmthunk.obj debug.obj devioctl.obj dmusic32.res

all: dmusic32.dll dmusic32.sym

dmusic32.dll dmusic32.map: $(OBJS)
    link @<<
-nodefaultlib:libc.lib
-subsystem:windows
-version:4.0
-debug
-pdb:dmusic32.pdb
-out:dmusic32.dll
-dll
-def:dmusic32.def
-map
$(OBJS)
$(LIBS)
<<

dmusic32.sym: dmusic32.map
    mapsym dmusic32.map


dmusic32.res: dmusic32.rc
    rc -l 0x409 /fo .\dmusic32.res dmusic32.rc

dm32dll.obj:    dm32dll.cpp
enumport.obj:   enumport.cpp
dmeport.obj:    dmeport.cpp
debug.obj:      debug.cpp
devioctl.obj:   devioctl.cpp

dmthunk.obj:    dmthunk.asm
    ml -coff -c -DIS_32 dmthunk.asm

dmthunk.asm:    dmthunk.thk
    e:\nt\mstools\thunk dmthunk.thk
