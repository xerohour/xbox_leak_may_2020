########################################################################
# Makefile for ecconsol test application program
#
# Build rules

.SUFFIXES: .c .obj .lib

#       .c = CMerge source file (producing .obj file)
#       .obj = standard PC object file format
#       .lib = standard PC object library format

!CMDSWITCHES +i

CC = cl

CC_FLAGS = -GX -Od -ZI -DWIN32 -D_WINDOWS

OBJDIR = debug
TESTDIR = $(MAKEDIR)

########################################################################
{}.cpp{}.obj:
	$(CC) -c $(CC_FLAGS) $(DEFS) $(TEST) -Fo$@ $<
{}.cxx{}.obj:
	$(CC) -c $(CC_FLAGS) $(DEFS) $(TEST) -Fo$@ $<
{}.c{}.obj:
	$(CC) -c $(CC_FLAGS) $(DEFS) $(TEST) -Fo$@ $<
########################################################################

goal:   ecconsol.exe

!IF "$(CFG)" == "1"
!MESSAGE Building 'debug-partial-debug' symbols chain
ecconsol.exe: ecconsol.lnk ecconsol.obj based.obj calls.obj chain0.obj chain1.obj chain2.obj chain3.obj chain4.obj chainx1.obj chainx2.obj cxx.obj dbg_main.obj f_asm.obj f_char.obj f_float.obj f_int.obj f_long.obj f_recurs.obj f_short.obj f_void.obj foo.obj testg.obj typetest.obj
	cl -c -Od -Zd -GX chain2.c 
	cl -c -Od -Zd -GX chainx2.c 
	link -out:ecconsol.exe -debug:full -debugtype:BOTH -pdbtype:sept -incremental:enc -editandcontinue @ecconsol.lnk

!ELSEIF "$(CFG)" == "2"
!MESSAGE Building 'debug-nodebug-debug' symbols chain
ecconsol.exe: ecconsol.lnk ecconsol.obj based.obj calls.obj chain0.obj chain1.obj chain2.obj chain3.obj chain4.obj chainx1.obj chainx2.obj cxx.obj dbg_main.obj f_asm.obj f_char.obj f_float.obj f_int.obj f_long.obj f_recurs.obj f_short.obj f_void.obj foo.obj testg.obj typetest.obj
	cl -c -GX chain3.c 
	cl -c -GX chainx1.c 
	link -out:ecconsol.exe -debug:full -debugtype:BOTH -pdbtype:sept -incremental:enc -editandcontinue @ecconsol.lnk
 
!ELSE
!MESSAGE Building 'debug-partial-nodebug-debug' symbols chain
ecconsol.exe: ecconsol.lnk ecconsol.obj based.obj calls.obj chain0.obj chain1.obj chain2.obj chain3.obj chain4.obj chainx1.obj chainx2.obj cxx.obj dbg_main.obj f_asm.obj f_char.obj f_float.obj f_int.obj f_long.obj f_recurs.obj f_short.obj f_void.obj foo.obj testg.obj typetest.obj
	cl -c -Od -Zd -GX chain2.c 
	cl -c -Od -Zd -GX chainx2.c 
	cl -c -GX chain3.c 
	cl -c -GX chainx1.c 
	link -out:ecconsol.exe -debug:full -debugtype:CV -pdbtype:sept -incremental:enc -editandcontinue @ecconsol.lnk

!ENDIF

ecconsol.lnk:                   .\ecconsol.mak
	echo ecconsol.obj  > ecconsol.lnk
	echo based.obj  >> ecconsol.lnk
	echo calls.obj  >> ecconsol.lnk
	echo chain0.obj  >> ecconsol.lnk
	echo chain1.obj  >> ecconsol.lnk
	echo chain2.obj  >> ecconsol.lnk
	echo chain3.obj  >> ecconsol.lnk
	echo chain4.obj  >> ecconsol.lnk
	echo chainx1.obj  >> ecconsol.lnk
	echo chainx2.obj  >> ecconsol.lnk
	echo cxx.obj  >> ecconsol.lnk
	echo dbg_main.obj  >> ecconsol.lnk
	echo f_asm.obj  >> ecconsol.lnk
	echo f_char.obj  >> ecconsol.lnk
	echo f_float.obj  >> ecconsol.lnk
	echo f_int.obj  >> ecconsol.lnk
	echo f_long.obj  >> ecconsol.lnk
	echo f_recurs.obj  >> ecconsol.lnk
	echo f_short.obj  >> ecconsol.lnk
	echo f_void.obj  >> ecconsol.lnk
	echo foo.obj  >> ecconsol.lnk
	echo testg.obj  >> ecconsol.lnk
	echo typetest.obj  >> ecconsol.lnk
	echo user32.lib  >> ecconsol.lnk
	echo gdi32.lib  >> ecconsol.lnk

