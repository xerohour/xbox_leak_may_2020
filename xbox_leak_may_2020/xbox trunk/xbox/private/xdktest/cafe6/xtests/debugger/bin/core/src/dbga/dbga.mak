########################################################################
# Makefile for DBGA test application program
#

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

goal:   dbga.exe

dbga.exe: dbga.lnk main2.obj cpp.obj 
	link -out:dbga.exe -debug:full -debugtype:CV -pdbtype:sept @dbga.lnk

dbga.lnk:                   .\dbga.mak
	echo main2.obj  > dbga.lnk
	echo cpp.obj  >> dbga.lnk
	echo user32.lib  >> dbga.lnk
	echo gdi32.lib  >> dbga.lnk
