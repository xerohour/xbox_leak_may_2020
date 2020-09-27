########################################################################
# Makefile for seh01 test application program

!CMDSWITCHES +i

CC = cl

CC_FLAGS = -GX -Od -ZI 

OBJDIR = debug
TESTDIR = $(MAKEDIR)

########################################################################
{}.cpp{}.obj:
	$(CC) -c $(CC_FLAGS) $(DEFS) $(TEST) -Fo$@ $<
########################################################################

goal:   seh01.exe
seh01.exe: seh01.lnk seh01.obj sehutil.obj
	link -out:seh01.exe -debug:full -debugtype:CV -pdbtype:sept @seh01.lnk

seh01.lnk:                   .\seh01.mak
	echo seh01.obj  > seh01.lnk
	echo sehutil.obj  >> seh01.lnk



