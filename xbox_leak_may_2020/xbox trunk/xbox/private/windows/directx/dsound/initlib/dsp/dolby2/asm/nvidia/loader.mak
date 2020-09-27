#******************************************************************************
#*	Unpublished work.  Copyright 2000 Dolby Laboratories Inc
#*	All Rights Reserved.
#*
#*	File:			loader.mak
#*
#*	Contents:		Dolby Game Encoder Loader Make File
#*
#*
#*	Author:			jmw
#*
#*	Date Created:	9/12/2000
#*
#*	History:
#*
#*		Rev 1.00	9/12/2000 12:07:49 PM		jmw
#*	Created Make File.
#*
#******************************************************************************

#	Macros

OBJDIR			=	encldr

CTRL_FILE		=	loader.ctl
BASE_CTRL_FILE	=	dsbase.ctl

LOADERINC		=	loader.mak ldr_sip.inc

AS				=	$(BUILD_TOOLS_DIR)\Motorola\DSP\dsp\bin\asm56300
AFLAGS			=	-q -b$*.cln -onoidw -l$*.lst
#	-d			=	Define symbols, same as assembler DEFINE directive
#	-q			=	Suppress the sign on banner
#	-b			=	Compile without linking
#	-g			=	Include source file line number information
#	-onoidw		=	Disable pipeline stall warnings
#	-l			=	Specifies the assembler listing file (output)
LN				=	$(BUILD_TOOLS_DIR)\Motorola\DSP\dsp\bin\dsplnk
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

LOADER_FILES	=	$(OBJDIR)\loader.cln	\
					$(OBJDIR)\ldr_tab.cln
				
#	Inference Rules
#		$< represents the current dependent file

{}.asm{$(OBJDIR)}.cln:
					$(AS) $(AFLAGS) $<


#	Assemble Dependencies

$(LOADER_FILES):	$(LOADERINC)


#	Link Dependencies
#		-f<< creates a list of files that is input to the linker
#		$** represents all the dependents

$(OBJDIR)\loader.cld:				\
					$(BASE_CTRL_FILE)	\
					$(CTRL_FILE)	\
					$(LOADER_FILES)
	$(LN) $(LFLAGS) -f<<$*.lis
$(LOADER_FILES)
<<NOKEEP
