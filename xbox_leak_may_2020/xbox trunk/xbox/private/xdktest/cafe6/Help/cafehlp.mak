# CAFE Help File makefile
#
# Usage:
#
# nmake /f cafehlp.mak  all          Perform all the following actions:
# nmake /f cafehlp.mak  hlp          Build the .hlp file
#
# Before running this makefile, the following files should be ssync'ed
#
# all            Directories for all targets
# hlp            $(CAFEDIR)\cafe\wb
#                $(CAFEDIR)\cafe\src
#                .
#
# Before running this makefile, the following files must be writable (checked out)
#
# all            Files for all targets
# hlp            $(CAFEDIR)\help\cafe.hlp
#

# ---------------------------------------------
# CAFEDIR must be set to point to root of CAFE
#   enlistment
# ---------------------------------------------
!if "$(CAFEDIR)" == ""
!error CAFEDIR not defined; set CAFEDIR=root of CAFE enlistment.
!endif

# targets

all: hlp

# ---------------------------------------------
# build the CAFE help file
# ---------------------------------------------
hlp:
	@echo Building the CAFE help file
	@cafehelp -l cafehlp.lst -o cafehlp.rtf
# assume cafe.hlp is checked out
        @hcw -n cafe.hpj
