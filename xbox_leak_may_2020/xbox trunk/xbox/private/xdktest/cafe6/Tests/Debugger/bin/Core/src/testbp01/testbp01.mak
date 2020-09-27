########################################################################
# Makefile for TESTBP test application program

!CMDSWITCHES +i

CC = cl

CC_FLAGS = -Od -ZI 

OBJDIR = debug
TESTDIR = $(MAKEDIR)

bp_obj = testbp.obj bp__cc.obj bp__ucuc.obj bp__dd.obj bp__ff.obj bp__ldld.obj bp__ll.obj bp__narr.obj \
	  bp__rec.obj bp__ss.obj bp__ulul.obj bp__usus.obj bp__vv.obj

########################################################################
{}.c{}.obj:
	$(CC) -c $(CC_FLAGS) $(DEFS) $(TEST) -Fo$@ $<
########################################################################

goal: 	testbp01.exe

testbp01.exe: testbp01.lnk $(bp_obj)
	link -out:testbp01.exe -debug:full -debugtype:CV -pdbtype:sept @testbp01.lnk

testbp01.lnk:                   .\testbp01.mak
	echo $(bp_obj)  > testbp01.lnk

