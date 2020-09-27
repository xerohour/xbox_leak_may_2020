########################################################################
# Makefile for ecwinapp test application program
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

goal:   ecwinapp.exe

!IF "$(CFG)" == "1"
!MESSAGE Building 'debug-partial-debug' symbols chain
ecwinapp.exe: ecwinapp.lnk ecwinapp.obj based.obj calls.obj chain0.obj chain1.obj chain2.obj chain3.obj chain4.obj chainx1.obj chainx2.obj cxx.obj dbg_main.obj f_asm.obj f_char.obj f_float.obj f_int.obj f_long.obj f_recurs.obj f_short.obj f_void.obj foo.obj testg.obj typetest.obj
	cl -c -Od -Zd -GX chain2.c 
	cl -c -Od -Zd -GX chainx2.c 
	link -out:ecwinapp.exe -debug:full -debugtype:BOTH -pdbtype:sept -incremental:enc -editandcontinue @ecwinapp.lnk

!ELSEIF "$(CFG)" == "2"
!MESSAGE Building 'debug-nodebug-debug' symbols chain
ecwinapp.exe: ecwinapp.lnk ecwinapp.obj based.obj calls.obj chain0.obj chain1.obj chain2.obj chain3.obj chain4.obj chainx1.obj chainx2.obj cxx.obj dbg_main.obj f_asm.obj f_char.obj f_float.obj f_int.obj f_long.obj f_recurs.obj f_short.obj f_void.obj foo.obj testg.obj typetest.obj
	cl -c -GX chain3.c 
	cl -c -GX chainx1.c 
	link -out:ecwinapp.exe -debug:full -debugtype:BOTH -pdbtype:sept -incremental:enc -editandcontinue @ecwinapp.lnk
 
!ELSE
!MESSAGE Building 'debug-partial-nodebug-debug' symbols chain
ecwinapp.exe: ecwinapp.lnk ecwinapp.obj based.obj calls.obj chain0.obj chain1.obj chain2.obj chain3.obj chain4.obj chainx1.obj chainx2.obj cxx.obj dbg_main.obj f_asm.obj f_char.obj f_float.obj f_int.obj f_long.obj f_recurs.obj f_short.obj f_void.obj foo.obj testg.obj typetest.obj
	cl -c -Od -Zd -GX chain2.c 
	cl -c -Od -Zd -GX chainx2.c 
	cl -c -GX chain3.c 
	cl -c -GX chainx1.c 
	link -out:ecwinapp.exe -debug:full -debugtype:CV -pdbtype:sept -incremental:enc -editandcontinue @ecwinapp.lnk

!ENDIF

ecwinapp.lnk:                   .\ecwinapp.mak
	echo ecwinapp.obj  > ecwinapp.lnk
	echo based.obj  >> ecwinapp.lnk
	echo calls.obj  >> ecwinapp.lnk
	echo chain0.obj  >> ecwinapp.lnk
	echo chain1.obj  >> ecwinapp.lnk
	echo chain2.obj  >> ecwinapp.lnk
	echo chain3.obj  >> ecwinapp.lnk
	echo chain4.obj  >> ecwinapp.lnk
	echo chainx1.obj  >> ecwinapp.lnk
	echo chainx2.obj  >> ecwinapp.lnk
	echo cxx.obj  >> ecwinapp.lnk
	echo dbg_main.obj  >> ecwinapp.lnk
	echo f_asm.obj  >> ecwinapp.lnk
	echo f_char.obj  >> ecwinapp.lnk
	echo f_float.obj  >> ecwinapp.lnk
	echo f_int.obj  >> ecwinapp.lnk
	echo f_long.obj  >> ecwinapp.lnk
	echo f_recurs.obj  >> ecwinapp.lnk
	echo f_short.obj  >> ecwinapp.lnk
	echo f_void.obj  >> ecwinapp.lnk
	echo foo.obj  >> ecwinapp.lnk
	echo testg.obj  >> ecwinapp.lnk
	echo typetest.obj  >> ecwinapp.lnk
	echo user32.lib  >> ecwinapp.lnk
	echo gdi32.lib  >> ecwinapp.lnk

