########################################################################
# Makefile for SPCFC01 test application program

!CMDSWITCHES +i

CC = cl

CC_FLAGS = -Od -Zi

OBJDIR = debug
TESTDIR = $(MAKEDIR)

########################################################################
{}.cpp{}.obj:
	$(CC) -c $(CC_FLAGS) $(DEFS) $(TEST) -Fo$@ $<
########################################################################

goal:   spcfc01.exe

spcfc01.exe: spcfc01.lnk spcfc01.obj
	link -out:spcfc01.exe -debug:full -debugtype:CV -pdbtype:sept @spcfc01.lnk

spcfc01.lnk:                   .\spcfc01.mak
	echo spcfc01.obj  > spcfc01.lnk

