# IDE header munge build system
#
# Usage:
#
# nmake /f mungehdr.mak all          Perform all the following actions:
# nmake /f mungehdr.mak getprop      Copy headers from IDE project
# nmake /f mungehdr.mak getres       Copy and munge headers from IDE project
#
# Before munging, the following files should be ssync'ed
#
# all            Directories for all targets
# getprop        $(IDESRCDIR)\include\projprop.h, props.h, slob.h
# getres         $(IDESRCDIR)\appwiz\resource.h
#                $(IDESRCDIR)\src\rsc\resource.h
#                $(IDESRCDIR)\bld\resource.h
#                $(IDESRCDIR)\bld\common\*.opt
#                $(IDESRCDIR)\bld\add_ons\mac68k\*.opt
#                $(IDESRCDIR)\res\resource.h
#                $(IDESRCDIR)\shell\resource.h
#                $(IDESRCDIR)\include\shrdres.h
#                $(IDESRCDIR)\include\cmds.h
#                $(IDESRCDIR)\clswiz\resource.h
#
# Before building libraries/sniffs, the following files must be writable (checked out)
#
# all            Files for all targets
# getprop        $(CAFEDIR)\cafe\wb\sym\projprop.h, props.h, slob.h
# getres         $(CAFEDIR)\v3\support\guitools\ide\sym\appwz.h
#                $(CAFEDIR)\v3\support\guitools\ide\sym\vcpp32.h
#                $(CAFEDIR)\v3\support\guitools\ide\sym\vproj.h
#                $(CAFEDIR)\v3\support\guitools\ide\sym\optn*.h
#                $(CAFEDIR)\v3\support\guitools\ide\sym\68k\optn*.h
#                $(CAFEDIR)\v3\support\guitools\ide\sym\vres.h
#                $(CAFEDIR)\v3\support\guitools\ide\sym\vshell.h
#                $(CAFEDIR)\v3\support\guitools\ide\sym\clswiz.h
#                $(CAFEDIR)\v3\support\guitools\ide\sym\shrdres.h
#                $(CAFEDIR)\v3\support\guitools\ide\sym\cmdids.h
#

# ---------------------------------------------
# CAFEDIR must be set to point to root of CAFE
#   enlistment
# ---------------------------------------------
!if "$(CAFEDIR)" == ""
!error CAFEDIR not defined; set CAFEDIR=root of CAFE enlistment.
!endif

# ---------------------------------------------
# IDESRCDIR must be set to point to root of IDE
#   enlistment
# ---------------------------------------------
!if "$(IDESRCDIR)" == ""
!error IDESRCDIR not defined; set IDESRCDIR=root of IDE enlistment.
!endif

# targets

all: getprop getres

# ---------------------------------------------
# copy the property headers to $(CAFEDIR)\v3\support\guitools\ide\sym
# also copy the package registration header (reg.h)
# ---------------------------------------------
getprop:
        @echo Copying the property resources to $(CAFEDIR)\v3\support\guitools\ide\sym
# assume projprop.h, props.h, and slob.h are checked out
	@copy $(IDESRCDIR)\include\projprop.h
	@copy $(IDESRCDIR)\include\props.h
	@copy $(IDESRCDIR)\include\slob.h
	@copy $(IDESRCDIR)\include\reg.h 

# ---------------------------------------------
# copy the resource headers to $(CAFEDIR)\v3\support\guitools\ide\sym and munge
# ---------------------------------------------
getres:
        @echo Copying the resource headers to $(CAFEDIR)\v3\support\guitools\ide\sym
# assume appwz.h, vcpp32.h, vproj.h, optn*.h, 68k\optn*.h, vres.h, vshell.h, clswiz.h, shrdres.h, and cmdids.h are checked out
	@copy $(IDESRCDIR)\appwiz\resource.h         appwz.h
	@copy $(IDESRCDIR)\src\rsc\resource.h        vcpp32.h
	@copy $(IDESRCDIR)\bld\resource.h            vproj.h
	@copy $(IDESRCDIR)\bld\common\*.opt          *.h
#	@copy $(IDESRCDIR)\bld\add_ons\mac68k\*.opt  68k\*.h
	@copy $(IDESRCDIR)\res\resource.h            vres.h
	@copy $(IDESRCDIR)\shell\resource.h          vshell.h
	@copy $(IDESRCDIR)\include\shrdres.h         shrdres.h
	@copy $(IDESRCDIR)\include\cmds.h            cmdids.h
	@copy $(IDESRCDIR)\clswiz\resource.h         clswiz.h
	@echo Munging resource files and adding prefixes to ids
	@parseres -s APPWZ_    appwz.h         appwz.h2
	@parseres -s CLSWIZ_   clswiz.h        clswiz.h2
	@parseres -s VCPP32_   vcpp32.h        vcpp32.h2
	@parseres -s VPROJ_    vproj.h         vproj.h2
	@parseres -s VRES_     vres.h          vres.h2
	@parseres -s VSHELL_   vshell.h        vshell.h2
	@parseres -s SHRDRES_  shrdres.h       shrdres.h2
	@parseres -s OPTNBSC_  optnbsc.h       optnbsc.h2
	@parseres -s OPTNCPLR_ optncplr.h      optncplr.h2
	@parseres -s OPTNLIB_  optnlib.h       optnlib.h2
	@parseres -s OPTNLINK_ optnlink.h      optnlink.h2
	@parseres -s OPTNMTL_  optnmtl.h       optnmtl.h2
	@parseres -s OPTNRC_   optnrc.h        optnrc.h2
#	@parseres -s OPTNRC_   68k\optnmrc.h   68k\optnmrc.h2
#	@parseres -s OPTNCPLR_ 68k\optncplr.h  68k\optncplr.h2
#	@parseres -s OPTNLINK_ 68k\optnlink.h  68k\optnlink.h2
	@echo Moving the munged files to header files
	@copy appwz.h2        appwz.h
	@copy clswiz.h2       clswiz.h
	@copy vcpp32.h2       vcpp32.h
	@copy vproj.h2        vproj.h
	@copy vres.h2         vres.h
	@copy vshell.h2       vshell.h
	@copy shrdres.h2      shrdres.h
	@copy optnbsc.h2      optnbsc.h
	@copy optncplr.h2     optncplr.h
	@copy optnlib.h2      optnlib.h
	@copy optnlink.h2     optnlink.h
	@copy optnmtl.h2      optnmtl.h
	@copy optnrc.h2       optnrc.h
#	@copy 68k\optnmrc.h2  68k\optnmrc.h
#	@copy 68k\optncplr.h2 68k\optncplr.h
#	@copy 68k\optnlink.h2 68k\optnlink.h
	@del *.h2
#	@del 68k\*.h2
