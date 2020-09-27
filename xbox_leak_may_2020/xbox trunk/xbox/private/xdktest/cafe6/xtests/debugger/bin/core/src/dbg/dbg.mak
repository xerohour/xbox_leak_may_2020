########################################################################
# Makefile for DBG test application program
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

goal:   dbg.exe

dbg.xbe: dbg.exe
	imagebld -debug dbg.exe -out:dbg.xbe

!IF "$(CFG)" == "1"
!MESSAGE Building 'debug-partial-debug' symbols chain
dbg.exe: dbg.lnk main.obj chain0.obj chain1.obj chain2.obj chain3.obj chain4.obj cxx.obj eh.obj foo0x.obj foox.obj 
	cl -c -Od -Zd -GX chain2.c 
	link -out:dbg.exe -debug:full -debugtype:BOTH -pdbtype:sept @dbg.lnk

!ELSEIF "$(CFG)" == "2"
!MESSAGE Building 'debug-nodebug-debug' symbols chain
dbg.exe: dbg.lnk main.obj chain0.obj chain1.obj chain2.obj chain3.obj chain4.obj cxx.obj eh.obj foo0x.obj foox.obj 
	cl -c -GX chain3.c 
	link -out:dbg.exe -debug:full -debugtype:BOTH -pdbtype:sept @dbg.lnk
 
!ELSE
!MESSAGE Building 'debug-partial-nodebug-debug' symbols chain
dbg.exe: dbg.lnk main.obj chain0.obj chain1.obj chain2.obj chain3.obj chain4.obj cxx.obj eh.obj foo0x.obj foox.obj 
	cl -c -Od -Zd -GX chain2.c 
	cl -c -GX chain3.c 
	link -out:dbg.exe -debug:full -debugtype:CV -pdbtype:sept @dbg.lnk

!ENDIF

dbg.lnk:                   .\dbg.mak
	echo main.obj  > dbg.lnk
	echo chain0.obj  >> dbg.lnk
	echo chain1.obj  >> dbg.lnk
	echo chain2.obj  >> dbg.lnk
	echo chain3.obj  >> dbg.lnk
	echo chain4.obj  >> dbg.lnk
	echo cxx.obj  >> dbg.lnk
	echo eh.obj  >> dbg.lnk
	echo foo0x.obj  >> dbg.lnk
	echo foox.obj  >> dbg.lnk
	echo xboxkrnl.lib >> dbg.lnk
	echo xapilib.lib >> dbg.lnk

