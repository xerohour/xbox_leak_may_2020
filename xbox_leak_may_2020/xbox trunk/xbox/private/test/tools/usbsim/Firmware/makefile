#*******************************************************************************
#
#	makefile
#       Microsoft USB Simulator makefile
#		Copyright © 2000 Microsoft Corp.
#       Author:	Kirk Payne
#       Date Created: 12/1/00
#
#		Makefile containing the dependencies required to create the Microsoft USB simulator
#		executable.
#
#  		Programs
#  		   Make:                 MAKE (Borland Make)
#  		   Assembler:            TASM  (Borland Assembler)
#		   Compiler:             BCC  (Borland C++)
#  		   Linker:               TLINK  (Borland Linker)
#		
#    MKS Source Control
#    $Source: p:/Microsoft/621-USB_Simulator/Code/rcs/makefile $
#    $Revision: 1.10 $
#    $Date: 2001/01/09 21:43:06Z $ 
#    $Author: rjgardner $
#    $Log: makefile $
#    Revision 1.10  2001/01/09 21:43:06Z  rjgardner
#    Add compiler option for huge model
#    Revision 1.9  2000/12/21 14:30:30Z  rjgardner
#    Add version.h as dependency to sim_main.h
#    Revision 1.8  2000/12/13 22:24:23Z  rjgardner
#    Add netsock.h as dependency for ether.c
#    Revision 1.7  2000/12/13 15:20:42Z  klpayne
#    Remove stack checking option
#    Revision 1.6  2000/12/11 21:49:06Z  klpayne
#    Added cflags
#    Revision 1.5  2000/12/06 19:40:51Z  rjgardner
#    add buffers.c to build
#    Revision 1.4  2000/12/06 19:19:40Z  rjgardner
#    add ether.c to build
#    Revision 1.3  2000/12/06 15:32:00Z  klpayne
#    added sim_cmd
#    Revision 1.2  2000/12/05 22:04:09Z  klpayne
#    Added sim_main and sim_init
#    Revision 1.1  2000/12/05 12:48:48Z  klpayne
#    Initial revision
#
#*******************************************************************************

#---------------------- ASSEMBLER OPTIONS (OPTS_A) ----------------------#
#
OPTS_A =
AS = TASM

#---------------------- COMPILER OPTIONS (CFLAGS) -----------------------#
#
#	-1 Generate 80186/286 compatible instructions (non-protected mode)
#	-efilename filename for executable (filename.exe)
#	-f- no floating point
#	-M create full link map
#	-N check stack overflow
#	-Od	disable all optimizations
#	-v turn on source debugging
#	-w display warnings on
CFLAGS = -1 -esim -f- -M -Od -v -w -mh
CC = BCC

#----------------------- LINKER OPTIONS (OPTS_L) ------------------------#
#
LINK  = TLINK
OPTS_L = 

#------------------------- SOURCE FILES ----------------------------#
SOURCE_A = 

#---------------- METARULE FOR ASSEMBLYING & COMPILING FILES ------------#
#%.obj : %.asm
#	$(AS) $*.asm $(OPTS_A)

%.obj : sim_main.c sim_init.c usb_brd.c sim_cmd.c ether.c buffers.c
	$(CC) $(CFLAGS) sim_main.c sim_init.c usb_brd.c sim_cmd.c ether.c buffers.c

#-------------------- CREATE OBJECTS LIST FOR LINKER --------------------#
OBJECTS_A = $(SOURCE_A:.asm=.obj) 
OBJECTS_C = sim_main.obj sim_init.obj usb_brd.obj sim_cmd.obj ether.obj buffers.obj

#------------------------- LINK AND MAKE HEXFILE ------------------------#
sim: $(OBJECTS_C) $(OBJECTS_A) 
	$(LINK)  


#----------------------- FILE DEPENDENCIES -------------------------#
USB_brd.obj: USB_brd.c USB_brd.h sim.h
sim_cmd.obj: sim_cmd.c sim.h sim_cmd.h
sim_init.obj: sim_init.c sim.h
sim_main.obj: sim_main.c sim.h version.h
ether.obj: ether.c ether.h netsock.h
buffers.obj: buffers.c buffers.h
