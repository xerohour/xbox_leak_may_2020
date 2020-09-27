########################################################################
# Makefile for FIBONWIN test application program

!CMDSWITCHES +i

CC = cl

CC_FLAGS = -MDd -Gm -GX -Od -ZI -DWIN32 -D_DEBUG -D_WINDOWS -D_MBCS

OBJDIR = debug
TESTDIR = $(MAKEDIR)

########################################################################
{}.cpp{}.obj:
	$(CC) -c $(CC_FLAGS) $(DEFS) $(TEST) -Fo$@ $<
########################################################################

goal:   fibonwin.exe

!IF "$(CFG)" == "INCREMENTAL"
!MESSAGE Linking ee2 incrementally

fibonwin.exe: fibonwin.lnk fibon.obj winmain.obj
	link -out:fibonwin.exe -debug:full -debugtype:CV -pdbtype:sept @fibonwin.lnk /incremental:yes
!ELSE
!MESSAGE Linking fibonwin non-incrementally

fibonwin.exe: fibonwin.lnk fibon.obj winmain.obj
	link -out:fibonwin.exe -debug:full -debugtype:CV -pdbtype:sept @fibonwin.lnk /incremental:no
!ENDIF

fibonwin.lnk:                   .\fibonwin.mak
	echo fibon.obj  > fibonwin.lnk
	echo winmain.obj >>fibonwin.lnk
