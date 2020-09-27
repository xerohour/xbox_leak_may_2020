# Copyright (c) 1998 Microsoft Corporation
NAME  =  dmusic16
EXT   =  dll
ROOT  =  ..\..\..
OBJ1  =  libentry.obj $(NAME).obj debug.obj dmhelp.obj dmthunk.obj device.obj list.obj locks.obj midiout.obj
OBJ2  =  midiin.obj alloc.obj uldiv.obj mmdevldr.obj timerwnd.obj equeue.obj
OBJ3  =  
OBJ4  =  
OBJ5  =  
OBJS  =  $(OBJ1) $(OBJ2) $(OBJ3) $(OBJ4)
LIBS  =  libw mmsystem

GOALS =  $(PBIN)\$(NAME).$(EXT) $(PBIN)\$(NAME).sym $(PBIN)\$(NAME).htm

!if "$(DEBUG)" == "retail"
DEF      = 
CDEBUG   = /Oxwt
L16DEBUG =
RDEBUG   =
ADEBUG   =
!else
!if "$(DEBUG)" == "debug"
DEF      =  -DDEBUG_RETAIL -DISRDEBUG
CDEBUG   =  /Od $(DEF)
L16DEBUG =  /LI
RDEBUG   =  -v $(DEF)
ADEBUG   =  $(DEF)
!else
DEF      =  -DDEBUG -DISRDEBUG
CDEBUG   =  /Od -Zid $(DEF)
L16DEBUG =  /CO/LI
RDEBUG   =  -v $(DEF)
ADEBUG   =  -Zi $(DEF)
!endif
!endif

CFLAGS   =  /DWIN16 /DWINVER=0x0400 /Alnw /GD $(CDEBUG) -Fd$* -Fo$@ -Fc -Fl 
AFLAGS   =  -D?MEDIUM -D?QUIET $(ADEBUG)
L16FLAGS =  /AL:16/ONERROR:NOEXE$(L16DEBUG)
RCFLAGS  =  $(RDEBUG)

WANT_32  =  TRUE
IS_16    =  TRUE
IS_OEM   =  TRUE

!include $(ROOT)\build\project.mk

libentry.obj:   ..\..\$$(@B).asm
        @echo $(@B).asm
        @$(ASM) $(AFLAGS) -DSEGNAME=INIT_TEXT ..\..\$(@B),$@;

#
# Calls are used from MIDI input callback
#
mmdevldr.obj:   ..\..\$$(@B).asm
        @echo $(@B).asm
        @$(ASM) $(AFLAGS) -DSEGNAME=FIX_IN_TEXT ..\..\$(@B),$@;

alloc.obj: ..\..\$$(@B).c
        @$(CL) @<<
$(CFLAGS) -NT _TEXT ..\..\$(@B).c
<<

dmhelp.obj:    ..\..\$$(@B).asm
        @echo $(@B).asm
        mlx -nologo -DIS_16 $(ADEBUG) -W3 -Zd -c -Cx -DMASM6 -Fl -Fo$@ ..\..\$(@B).asm

dmthunk.obj:    $$(@B).asm
        ml -c -DIS_16 $(@B).asm

dmthunk.asm:    ..\..\..\dmusic32\$$(@B).thk
        thunk -o $@ ..\..\..\dmusic32\$(@B).thk

debug.obj: ..\..\$$(@B).c
        @$(CL) @<<
$(CFLAGS) -NT _TEXT ..\..\$(@B).c
<<

device.obj: ..\..\$$(@B).c
        @$(CL) @<<
$(CFLAGS) -NT _TEXT ..\..\$(@B).c
<<

list.obj: ..\..\$$(@B).c
        @$(CL) @<<
$(CFLAGS) -NT _TEXT ..\..\$(@B).c
<<

locks.obj: ..\..\$$(@B).c
        @$(CL) @<<
$(CFLAGS) -NT _TEXT ..\..\$(@B).c
<<

midiout.obj: ..\..\$$(@B).c
        @$(CL) @<<
$(CFLAGS) -NT _TEXT ..\..\$(@B).c
<<

midiin.obj: ..\..\$$(@B).c
        @$(CL) @<<
$(CFLAGS) -NT _TEXT ..\..\$(@B).c
<<

timerwnd.obj: ..\..\$$(@B).c
        @$(CL) @<<
$(CFLAGS) -NT _TEXT ..\..\$(@B).c
<<

equeue.obj: ..\..\$$(@B).c
        @$(CL) @<<
$(CFLAGS) -NT FIX_COMM_TEXT ..\..\$(@B).c
<<

$(NAME).obj: ..\..\$$(@B).c
     @$(CL) @<<
$(CFLAGS) -NT _TEXT ..\..\$(@B).c
<<

uldiv.obj:   ..\..\$$(@B).asm
        @echo $(@B).asm
        @$(ASM) $(AFLAGS) ..\..\$(@B),$@;


$(NAME).res:    ..\..\$$(@B).rc ..\..\$$(@B).rcv \
                        ..\..\preclude.h 
     @$(RC) $(RCFLAGS) -z -fo$@ -I$(PVER) -I..\.. ..\..\$(@B).rc

$(NAME).$(EXT) $(NAME).map:  \
          $(OBJS) ..\..\$$(@B).def $$(@B).res
     @$(LINK16) @<<
$(OBJ1)+
$(OBJ2)+
$(OBJ3)+
$(OBJ4),
$(@B).$(EXT) $(L16FLAGS),
$(@B).map,
$(LIBS),
..\..\$(@B).def
<<
     @$(RC) $(RESFLAGS) $*.res $*.$(EXT)

$(PBIN)\$(NAME).sym: $(NAME).map
    mapsym -o $(PBIN)\$(NAME).sym $(@B).map 

$(PBIN)\$(NAME).htm: $(PBIN)\$(NAME).$(EXT)
    ..\..\..\tools\autodoc\autodoc /r html /f html.fmt /o $(PBIN)\$(NAME).htm ..\..\*.c ..\..\*.asm ..\..\*.h
