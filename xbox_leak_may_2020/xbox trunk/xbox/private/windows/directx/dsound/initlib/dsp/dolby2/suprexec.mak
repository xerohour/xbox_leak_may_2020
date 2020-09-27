#******************************************************************************
#*	Unpublished work.  Copyright 2000 Dolby Laboratories Inc
#*	All Rights Reserved.
#*
#*	File:			suprexec.mak
#*
#*	Contents:		Dolby Game Encoder SuperExec make file
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

OBJDIR			=	encse

CTRL_FILE		=	suprexec.ctl
BASE_CTRL_FILE	=	dsbase.ctl

SUPEREXECINC	= 	suprexec.mak ldr_sip.inc se_sip.inc

AS				=	asm56300
!	IF ("$(SIM_XBOX)" == "1")
AFLAGS			=	-q -d SIM_XBOX_SYS_MEM 1 -b$*.cln -onoidw -l$*.lst
!	ELSE
AFLAGS			=	-q -b$*.cln -onoidw -l$*.lst
!	ENDIF
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

SUPEREXEC_FILES	=	$(OBJDIR)\suprexec.cln	\
!	IF ("$(SIM_XBOX)" == "1")
					$(OBJDIR)\sehelper.cln
!	ENDIF
				
#	Inference Rules
#		$< represents the current dependent file

{}.asm{$(OBJDIR)}.cln:
					$(AS) $(AFLAGS) $<

#	Assemble Dependencies

$(SUPEREXEC_FILES): $(SUPEREXECINC)


#	Link Dependencies
#		-f<< creates a list of files that is input to the linker
#		$** represents all the dependents

$(OBJDIR)\suprexec.cld:				\
					$(BASE_CTRL_FILE)	\
					$(CTRL_FILE)	\
					$(SUPEREXEC_FILES)					    
	$(LN) $(LFLAGS) -f<<$*.lis
$(SUPEREXEC_FILES)
<<NOKEEP
