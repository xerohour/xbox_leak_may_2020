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

goal:   ee2.exe

!IF "$(CFG)" == "INCREMENTAL"
!MESSAGE Linking ee2 incrementally

ee2.exe: ee2.lnk eetest.obj
	link -out:ee2.exe -debug:full -debugtype:CV -pdbtype:sept @ee2.lnk  /subsystem:console /incremental:yes
!ELSE
!MESSAGE Linking ee2 non-incrementally

ee2.exe: ee2.lnk eetest.obj
	link -out:ee2.exe -debug:full -debugtype:CV -pdbtype:sept @ee2.lnk  /subsystem:console /incremental:no
!ENDIF

ee2.lnk:                   .\ee2.mak
	echo eetest.obj  > ee2.lnk
	echo oleaut32.lib >>ee2.lnk
