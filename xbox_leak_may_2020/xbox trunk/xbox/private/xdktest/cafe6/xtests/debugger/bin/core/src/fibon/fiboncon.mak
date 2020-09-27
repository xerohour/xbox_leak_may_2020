########################################################################
# Makefile for EE2 test application program

!CMDSWITCHES +i

CC = cl

CC_FLAGS = -Od -ZI -DWIN32 -D_DEBUG -D_CONSOLE -D_MBCS

OBJDIR = debug
TESTDIR = $(MAKEDIR)

########################################################################
{}.cpp{}.obj:
	$(CC) -c $(CC_FLAGS) $(DEFS) $(TEST) -Fo$@ $<
########################################################################

goal:   fiboncon.exe

!IF "$(CFG)" == "INCREMENTAL"
!MESSAGE Linking ee2 incrementally

fiboncon.exe: fiboncon.lnk fibon.obj main.obj
	link -out:fiboncon.exe -debug:full -debugtype:CV -pdbtype:sept @fiboncon.lnk  /subsystem:console /incremental:yes
!ELSE
!MESSAGE Linking fiboncon non-incrementally

fiboncon.exe: fiboncon.lnk fibon.obj main.obj
	link -out:fiboncon.exe -debug:full -debugtype:CV -pdbtype:sept @fiboncon.lnk  /subsystem:console /incremental:no
!ENDIF

fiboncon.lnk:                   .\fiboncon.mak
	echo fibon.obj  > fiboncon.lnk
	echo main.obj >>fiboncon.lnk
