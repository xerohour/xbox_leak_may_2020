#******************************************************************************
#*	Unpublished work.  Copyright 2000 Dolby Laboratories Inc
#*	All Rights Reserved.
#*
#*	File:			sysmem.mak
#*
#*	Contents:		Dolby Game Encoder System Memory make file
#*
#*
#*	Author:			jmw
#*
#*	Date Created:	9/12/2000
#*
#*	History:
#*
#*		Rev 1.00	9/12/2000 12:08:37 PM		jmw
#*	Created Make File.
#*
#******************************************************************************

#	Macros

OBJDIR			=	encsm

CTRL_FILE		=	sysmem.ctl
BASE_CTRL_FILE	=	dsbase.ctl

SYSMEMINC		= 	sysmem.mak ldr_sip.inc

AS				=	asm56300
AFLAGS			=	-q -b$*.cln -onoidw -l$*.lst
#	-d			=	Define symbols, same as assembler DEFINE directive
#	-q			=	Suppress the sign on banner
#	-b			=	Compile without linking
#	-g			=	Include source file line number information
#	-onoidw		=	Disable pipeline stall warnings
#	-l			=	Specifies the assembler listing file (output)

LN				=	dsplnk
LFLAGS			=	-q -a -b$*.cld -m$*.map -r$(CTRL_FILE) -xOVLP -xWEX
#	-i			=	Link incrementally
#	-q			=	Suppress the sign on banner
#	-g			=	Include source file line number information
#	-a			=	Optimize circular buffer placement
#	-b			=	Specifies the incrementally-linked object file (output)
#	-m			=	Specifies the map file (output)
#	-f			=	Specifies the linker list file (input)
#	-r			=	Specifies the memory control file (input)
#	-xOVLP		=	Warn on section overlap
#	-xWEX		=	Report warnings in exit status

#	Component Files

SYSMEM_FILES	=	$(OBJDIR)\sysmem.cln
				
#	Inference Rules
#		$< represents the current dependent file

{}.asm{$(OBJDIR)}.cln:
					$(AS) $(AFLAGS) $<

#	Assemble Dependencies

$(SYSMEM_FILES): $(SYSMEMINC) encds.asm llef13.asm llef23.asm llef33.asm lleyrom.asm lleconfg.asm


#	Link Dependencies
#		-f<< creates a list of files that is input to the linker
#		$** represents all the dependents

$(OBJDIR)\sysmem.cld:				\
					$(BASE_CTRL_FILE)	\
					$(CTRL_FILE)	\
					$(SYSMEM_FILES)
	$(LN) $(LFLAGS) -f<<$*.lis
$(SYSMEM_FILES)
<<NOKEEP
