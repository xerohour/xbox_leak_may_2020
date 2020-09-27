# Cafe Build System
#
# Usage:
#
# Core CAFE:
#
# nmake /f cafebld.mak all          Perform all the following actions:
# nmake /f cafebld.mak cafe         The CAFE system: log, target, gui, shl, res, src, wrk, eds, prj, dbg, ide, bldtools, toolset, subsuite
# nmake /f cafebld.mak log          The log system
# nmake /f cafebld.mak target       The target portion of support system
# nmake /f cafebld.mak gui          The gui portion of support system
# nmake /f cafebld.mak shl          The shl portion of support system
# nmake /f cafebld.mak src          The src portion of support system
# nmake /f cafebld.mak wrk          The wrk portion of support system
# nmake /f cafebld.mak res          The res portion of support system
# nmake /f cafebld.mak eds          The eds portion of support system
# nmake /f cafebld.mak prj          The prj portion of support system
# nmake /f cafebld.mak dbg          The dbg portion of support system
# nmake /f cafebld.mak ide          The ide portion of support system
# nmake /f cafebld.mak dav          The dav portion of support system
# nmake /f cafebld.mak bldtools     The bldtools portion of the support system
# nmake /f cafebld.mak toolset      The toolset portion of support system
# nmake /f cafebld.mak subsuite     The subsuite library
# nmake /f cafebld.mak cafedrv      The CAFE driver
# nmake /f cafebld.mak ideheaders   Munge headers from IDE project
#
# Sniff tests:
#
# nmake /f cafebld.mak sniffs       Build all the sniff tests
# nmake /f cafebld.mak browser          Build browser sniff test
# nmake /f cafebld.mak data         Build data sniff test
# nmake /f cafebld.mak dbg_mbc      Build dbg_mbc sniff test
# nmake /f cafebld.mak debugger     Build debugger sniff test
# nmake /f cafebld.mak editgo           Build edit 'n go sniff test
# nmake /f cafebld.mak editor           Build editor sniff test
# nmake /f cafebld.mak javasys      Build javasys sniff test
# nmake /f cafebld.mak vjdebug      Build vjdebug sniff test
# nmake /f cafebld.mak vjclsvw      Build vjclsvw sniff test
# nmake /f cafebld.mak vjbuild      Build vjbuild sniff test
# nmake /f cafebld.mak vjexe        Build vjexe sniff test
# nmake /f cafebld.mak sys          Build sys sniff test
# nmake /f cafebld.mak sdi          Build sdi sniff test
# nmake /f cafebld.mak vproj        Build vproj sniff test
# nmake /f cafebld.mak vres         Build vres sniff test
# nmake /f cafebld.mak vshell       Build vshell sniff test
# nmake /f cafebld.mak wizards      Build wizards sniff test
# nmake /f cafebld.mak fortran      Build fortran sniff test
# nmake /f cafebld.mak istudio      Build istudio sniff test
#
#
# Additional command line defines: * indicates default
#
# BUILD=        [ 0 | 1* ]                              Build the component(s)
# DEBUG=        [ 0* | 1 | 2 ]                  Build debug (0), retail (1), or retail with debug info (2)
# CLEAN=        [ 0* | 1 ]                              Clean the project before building
# SSYNC=        [ 0* | 1 ]                              Ssync the directory(ies) before building
# SLMOUT=       [ 0* | 1 ]                              Check the writable files for the given component(s) out
# SLMIN=        [ 0* | 1 ]                              Check the writable files for the given component(s) in
# BROWSE=   [ 0 | 1* ]                          Build browser info (1) or don't (0)
# DEPEND=   [ 0 | 1* ]                          Build dependencies (1) or don't (0)
# PLATFORM= [ X86*, MIPS, ALPHA ]       Specify the platform
# AUTODEP=  [ 0* | 1 ]                          Generate dependency file
# VERB=         [ 0* | 1 | 2 ]                  Verbose output: 0 = output to file only; 1 = output to file and screen; 2 = output to screen
# V4LOW         [ 0* | 1 ]                              Build conditionally for V4 Low (VC++ Standard Edition)
#

# ---------------------------------------------
# SNIFFDIR must be set to point to root of 
#   IDESNIFF enlistment
# ---------------------------------------------
!if "$(SNIFFDIR)" == ""
!error SNIFFDIR not defined; set SNIFFDIR=platform directory of IDESNIFF enlistment.
!endif

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

# ---------------------------------------------
# Designate a LOGDIR to save failed logs
# ---------------------------------------------
LOGDIR=$(CAFEDIR)\bldlogs


# ---------------------------------------------
# Which copy command to use depending on OS - NT or Win95
# ---------------------------------------------
!IF "$(OS)"=="Windows_NT"
!MESSAGE Setting copy command for NT
COPYCMD=@echo y | xcopy /f /v /r
!ELSE
!MESSAGE Setting copy command for Win95
COPYCMD=@copy /y /v
!ENDIF


# ---------------------------------------------
# ensure all defines are defined
# ---------------------------------------------
!ifndef BUILD
!message CAFEBld: BUILD not defined; defaulting to BUILD=1
BUILD=1
!endif
!ifndef DEBUG
!message CAFEBld: DEBUG not defined; defaulting to DEBUG=0
DEBUG=0
!endif
!ifndef CLEAN
!message CAFEBld: CLEAN not defined; defaulting to CLEAN=0
CLEAN=0
!endif
!ifndef SSYNC
!message CAFEBld: SSYNC not defined; defaulting to SSYNC=0
SSYNC=0
!endif
!ifndef SLMOUT
!message CAFEBld: SLMOUT not defined; defaulting to SLMOUT=0
SLMOUT=0
!endif
!ifndef SLMIN
!message CAFEBld: SLMIN not defined; defaulting to SLMIN=0
SLMIN=0
!endif
!ifndef BROWSE
!message CAFEBld: BROWSE not defined; defaulting to BROWSE=0
BROWSE=0
!endif
!ifndef DEPEND
!message CAFEBld: DEPEND not defined; defaulting to DEPEND=1
DEPEND=1
!endif
!ifndef PLATFORM
!ifndef PROCESSOR_ARCHITECTURE
!message PLATFORM or PROCESSOR_ARCHITECTURE not defined; defaulting to PLATFORM=X86
PLATFORM=X86
!else
PLATFORM=$(PROCESSOR_ARCHITECTURE)
!endif
!endif
!ifndef AUTODEP
!message CAFEBld: AUTODEP not defined; defaulting to AUTODEP=0
AUTODEP=0
!endif
!ifndef VERB
!message CAFEBld: VERB not defined; defaulting to VERB=0
VERB=0
!endif
!ifndef V4LOW
!message CAFEBld: V4LOW not defined; defaulting to V4LOW=0
V4LOW=0
!endif

# ---------------------------------------------
# Define how to log stderr/out
# ---------------------------------------------
FILE_LOG=$(CAFEDIR)\results.log
ERRORLOG= \
!if "$(OS)" == "Windows_NT"
!if "$(VERB)" == "1"
 | tee $(FILE_LOG) 2>&1
!else
 >> $(FILE_LOG) 2>&1
!endif
!endif
!if "$(VERB)" == "2"
STDERROUT=
!else
STDERROUT=$(ERRORLOG) || copy $(FILE_LOG) $(LOGDIR)\$@.log
!endif

# ---------------------------------------------
# ---------------------------------------------
# targets
# ---------------------------------------------

# ---------------------------------------------
# build all targets (except ideheaders)
# ---------------------------------------------
!if "$(V4LOW)"=="1"
all: set_env cafe javasys vjdebug vjclsvw vjbuild vjexe browser debugger sys vproj vres vshell wizards istudio
!else
all: set_env cafe javasys vjdebug vjclsvw vjbuild vjexe browser data debugger editor sdi sys vproj vres vshell wizards dbg_mbc istudio
!endif

# ---------------------------------------------
# build CAFE
# ---------------------------------------------
cafe: set_env cafedrv

# ---------------------------------------------
# build the sniffs
# ---------------------------------------------
!if "$(V4LOW)"=="1"
sniffs: javasys vjdebug vjclsvw vjbuild vjexe set_env browser debugger sys vproj vres vshell wizards
!else
sniffs: javasys vjdebug vjclsvw vjbuild vjexe set_env dbg_mbc data browser debugger editor sdi sys vproj vres vshell wizards
!endif

# ---------------------------------------------
# build target portion of support system
# ---------------------------------------------
target: set_env
	@echo ----------------------------------
# ssync
!if "$(SSYNC)" == "1"
	@echo Ssyncing Target DLL...
	@cd $(CAFEDIR)\support
	@$(MAKE) -$(MAKEFLAGS) -f target.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) ssync $(STDERROUT)
!endif
# slmout
!if "$(SLMOUT)" == "1"
	@echo Checking out Target DLL...
	@cd $(CAFEDIR)\support
	@$(MAKE) -$(MAKEFLAGS) -f target.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) slmout $(STDERROUT)
!endif
# autodep
!if "$(AUTODEP)" == "1"
	@echo Generating dependency file for Target DLL...
	@cd $(CAFEDIR)\support
	@$(MAKE) -$(MAKEFLAGS) -f target.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) auto.dep $(STDERROUT)
!endif
# clean
!if "$(CLEAN)" == "1"
	@echo Cleaning Target DLL...
	@cd $(CAFEDIR)\support
	@$(MAKE) -$(MAKEFLAGS) -f target.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) clean $(STDERROUT)
!endif
# build
!if "$(BUILD)" == "1"
	@echo Building Target DLL...
	@cd $(CAFEDIR)\support
	@$(MAKE) -$(MAKEFLAGS) -f target.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) $(STDERROUT)
!endif
# slmin
!if "$(SLMIN)" == "1"
	@echo Checking in Target DLL...
	@cd $(CAFEDIR)\support
	@$(MAKE) -$(MAKEFLAGS) -f target.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) slmin $(STDERROUT)
!endif
	@echo ----------------------------------

# ---------------------------------------------
# build log system
# ---------------------------------------------
log: set_env
	@echo ----------------------------------
# ssync
!if "$(SSYNC)" == "1"
	@echo Ssyncing Log DLL...
	@cd $(CAFEDIR)\log
	@$(MAKE) -$(MAKEFLAGS) -f log.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) ssync $(STDERROUT)
!endif
# slmout
!if "$(SLMOUT)" == "1"
	@echo Checking out Log DLL...
	@cd $(CAFEDIR)\log
	@$(MAKE) -$(MAKEFLAGS) -f log.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) slmout $(STDERROUT)
!endif
# autodep
!if "$(AUTODEP)" == "1"
	@echo Generating dependency file for Log DLL...
	@cd $(CAFEDIR)\log
	@$(MAKE) -$(MAKEFLAGS) -f log.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) auto.dep $(STDERROUT)
!endif
# clean
!if "$(CLEAN)" == "1"
	@echo Cleaning Log DLL...
	@cd $(CAFEDIR)\log
	@$(MAKE) -$(MAKEFLAGS) -f log.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) clean $(STDERROUT)
!endif
# build
!if "$(BUILD)" == "1"
	@echo Building Log DLL...
	@cd $(CAFEDIR)\log
	@$(MAKE) -$(MAKEFLAGS) -f log.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) $(STDERROUT)
!endif
# slmin
!if "$(SLMIN)" == "1"
	@echo Checking in Log DLL...
	@cd $(CAFEDIR)\log
	@$(MAKE) -$(MAKEFLAGS) -f log.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) slmin $(STDERROUT)
!endif
	@echo ----------------------------------

# ---------------------------------------------
# build gui portion of support system
# ---------------------------------------------
gui: set_env
	@echo ----------------------------------
# ssync
!if "$(SSYNC)" == "1"
	@echo Ssyncing GUI DLL...
	@cd $(CAFEDIR)\support\guitools
	@$(MAKE) -$(MAKEFLAGS) -f gui.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) ssync $(STDERROUT)
!endif
# slmout
!if "$(SLMOUT)" == "1"
	@echo Checking out GUI DLL...
	@cd $(CAFEDIR)\support\guitools
	@$(MAKE) -$(MAKEFLAGS) -f gui.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) slmout $(STDERROUT)
!endif
# autodep
!if "$(AUTODEP)" == "1"
	@echo Generating dependency file for GUI DLL...
	@cd $(CAFEDIR)\support\guitools
	@$(MAKE) -$(MAKEFLAGS) -f gui.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) auto.dep $(STDERROUT)
!endif
# clean
!if "$(CLEAN)" == "1"
	@echo Cleaning GUI DLL...
	@cd $(CAFEDIR)\support\guitools
	@$(MAKE) -$(MAKEFLAGS) -f gui.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) clean   $(STDERROUT)
!endif
# build
!if "$(BUILD)" == "1"
	@echo Building GUI DLL...
	@cd $(CAFEDIR)\support\guitools
	@$(MAKE) -$(MAKEFLAGS) -f gui.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) $(STDERROUT)
!endif
# slmin
!if "$(SLMIN)" == "1"
	@echo Checking in GUI DLL...
	@cd $(CAFEDIR)\support\guitools
	@$(MAKE) -$(MAKEFLAGS) -f gui.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) slmin $(STDERROUT)
!endif
	@echo ----------------------------------

# ---------------------------------------------
# build shl portion of support system
# ---------------------------------------------
shl: set_env
	@echo ----------------------------------
# ssync
!if "$(SSYNC)" == "1"
	@echo Ssyncing SHL DLL...
	@cd $(CAFEDIR)\support\guitools\ide\shl
	@$(MAKE) -$(MAKEFLAGS) -f shl.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) IDESRCDIR=$(IDESRCDIR) ssync $(STDERROUT)
!endif
# slmout
!if "$(SLMOUT)" == "1"
	@echo Checking out SHL DLL...
	@cd $(CAFEDIR)\support\guitools\ide\shl
	@$(MAKE) -$(MAKEFLAGS) -f shl.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) IDESRCDIR=$(IDESRCDIR) slmout $(STDERROUT)
!endif
# autodep
!if "$(AUTODEP)" == "1"
	@echo Generating dependency file for SHL DLL...
	@cd $(CAFEDIR)\support\guitools\ide\shl
	@$(MAKE) -$(MAKEFLAGS) -f shl.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) auto.dep $(STDERROUT)
!endif
# clean
!if "$(CLEAN)" == "1"
	@echo Cleaning SHL DLL...
	@cd $(CAFEDIR)\support\guitools\ide\shl
	@$(MAKE) -$(MAKEFLAGS) -f shl.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) IDESRCDIR=$(IDESRCDIR) clean $(STDERROUT)
!endif
# build
!if "$(BUILD)" == "1"
	@echo Building SHL DLL...
	@cd $(CAFEDIR)\support\guitools\ide\shl
	@$(MAKE) -$(MAKEFLAGS) -f shl.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) IDESRCDIR=$(IDESRCDIR) $(STDERROUT)
!endif
# slmin
!if "$(SLMIN)" == "1"
	@echo Checking in SHL DLL...
	@cd $(CAFEDIR)\support\guitools\ide\shl
	@$(MAKE) -$(MAKEFLAGS) -f shl.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) IDESRCDIR=$(IDESRCDIR) slmin $(STDERROUT)
!endif
	@echo ----------------------------------

# ---------------------------------------------
# build res portion of support system
# ---------------------------------------------
res: set_env
	@echo ----------------------------------
# ssync
!if "$(SSYNC)" == "1"
	@echo Ssyncing RES DLL...
	@cd $(CAFEDIR)\support\guitools\ide\res
	@$(MAKE) -$(MAKEFLAGS) -f res.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) ssync $(STDERROUT)
!endif
# slmout
!if "$(SLMOUT)" == "1"
	@echo Checking out RES DLL...
	@cd $(CAFEDIR)\support\guitools\ide\res
	@$(MAKE) -$(MAKEFLAGS) -f res.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) slmout $(STDERROUT)
!endif
# autodep
!if "$(AUTODEP)" == "1"
	@echo Generating dependency file for RES DLL...
	@cd $(CAFEDIR)\support\guitools\ide\res
	@$(MAKE) -$(MAKEFLAGS) -f res.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) auto.dep $(STDERROUT)
!endif
# clean
!if "$(CLEAN)" == "1"
	@echo Cleaning RES DLL...
	@cd $(CAFEDIR)\support\guitools\ide\res
	@$(MAKE) -$(MAKEFLAGS) -f res.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) clean $(STDERROUT)
!endif
# build
!if "$(BUILD)" == "1"
	@echo Building RES DLL...
	@cd $(CAFEDIR)\support\guitools\ide\res
	@$(MAKE) -$(MAKEFLAGS) -f res.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) $(STDERROUT)
!endif
# slmin
!if "$(SLMIN)" == "1"
	@echo Checking in RES DLL...
	@cd $(CAFEDIR)\support\guitools\ide\res
	@$(MAKE) -$(MAKEFLAGS) -f res.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) slmin   $(STDERROUT)
!endif
	@echo ----------------------------------



# ---------------------------------------------
# build src portion of support system
# ---------------------------------------------
src: set_env
	@echo ----------------------------------
# ssync
!if "$(SSYNC)" == "1"
	@echo Ssyncing SRC DLL...
	@cd $(CAFEDIR)\support\guitools\ide\src
	@$(MAKE) -$(MAKEFLAGS) -f src.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) ssync $(STDERROUT)
!endif
# slmout
!if "$(SLMOUT)" == "1"
	@echo Checking out SRC DLL...
	@cd $(CAFEDIR)\support\guitools\ide\src
	@$(MAKE) -$(MAKEFLAGS) -f src.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) slmout $(STDERROUT)
!endif
# autodep
!if "$(AUTODEP)" == "1"
	@echo Generating dependency file for SRC DLL...
	@cd $(CAFEDIR)\support\guitools\ide\src
	@$(MAKE) -$(MAKEFLAGS) -f src.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) auto.dep $(STDERROUT)
!endif
# clean
!if "$(CLEAN)" == "1"
	@echo Cleaning SRC DLL...
	@cd $(CAFEDIR)\support\guitools\ide\src
	@$(MAKE) -$(MAKEFLAGS) -f src.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) clean $(STDERROUT)
!endif
# build
!if "$(BUILD)" == "1"
	@echo Building SRC DLL...
	@cd $(CAFEDIR)\support\guitools\ide\src
	@$(MAKE) -$(MAKEFLAGS) -f src.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) $(STDERROUT)
!endif
# slmin
!if "$(SLMIN)" == "1"
	@echo Checking in SRC DLL...
	@cd $(CAFEDIR)\support\guitools\ide\src
	@$(MAKE) -$(MAKEFLAGS) -f src.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) slmin   $(STDERROUT)
!endif
	@echo ----------------------------------

# ---------------------------------------------
# build eds portion of support system
# ---------------------------------------------
eds: set_env
	@echo ----------------------------------
# ssync
!if "$(SSYNC)" == "1"
	@echo Ssyncing EDS DLL...
	@cd $(CAFEDIR)\support\guitools\ide\eds
	@$(MAKE) -$(MAKEFLAGS) -f eds.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) ssync $(STDERROUT)
!endif
# slmout
!if "$(SLMOUT)" == "1"
	@echo Checking out EDS DLL...
	@cd $(CAFEDIR)\support\guitools\ide\eds
	@$(MAKE) -$(MAKEFLAGS) -f eds.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) slmout $(STDERROUT)
!endif
# autodep
!if "$(AUTODEP)" == "1"
	@echo Generating dependency file for EDS DLL...
	@cd $(CAFEDIR)\support\guitools\ide\eds
	@$(MAKE) -$(MAKEFLAGS) -f eds.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) auto.dep $(STDERROUT)
!endif
# clean
!if "$(CLEAN)" == "1"
	@echo Cleaning EDS DLL...
	@cd $(CAFEDIR)\support\guitools\ide\eds
	@$(MAKE) -$(MAKEFLAGS) -f eds.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) clean $(STDERROUT)
!endif
# build
!if "$(BUILD)" == "1"
	@echo Building EDS DLL...
	@cd $(CAFEDIR)\support\guitools\ide\eds
	@$(MAKE) -$(MAKEFLAGS) -f eds.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) $(STDERROUT)
!endif
# slmin
!if "$(SLMIN)" == "1"
	@echo Checking in EDS DLL...
	@cd $(CAFEDIR)\support\guitools\ide\eds
	@$(MAKE) -$(MAKEFLAGS) -f eds.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) slmin   $(STDERROUT)
!endif
	@echo ----------------------------------

# ---------------------------------------------
# build wrk portion of support system
# ---------------------------------------------
wrk: set_env
	@echo ----------------------------------
# ssync
!if "$(SSYNC)" == "1"
	@echo Ssyncing WRK DLL...
	@cd $(CAFEDIR)\support\guitools\ide\wrk
	@$(MAKE) -$(MAKEFLAGS) -f wrk.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) ssync $(STDERROUT)
!endif
# slmout
!if "$(SLMOUT)" == "1"
	@echo Checking out WRK DLL...
	@cd $(CAFEDIR)\support\guitools\ide\wrk
	@$(MAKE) -$(MAKEFLAGS) -f wrk.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) slmout $(STDERROUT)
!endif
# autodep
!if "$(AUTODEP)" == "1"
	@echo Generating dependency file for WRK DLL...
	@cd $(CAFEDIR)\support\guitools\ide\wrk
	@$(MAKE) -$(MAKEFLAGS) -f wrk.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) auto.dep $(STDERROUT)
!endif
# clean
!if "$(CLEAN)" == "1"
	@echo Cleaning WRK DLL...
	@cd $(CAFEDIR)\support\guitools\ide\wrk
	@$(MAKE) -$(MAKEFLAGS) -f wrk.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) clean $(STDERROUT)
!endif
# build
!if "$(BUILD)" == "1"
	@echo Building WRK DLL...
	@cd $(CAFEDIR)\support\guitools\ide\wrk
	@$(MAKE) -$(MAKEFLAGS) -f wrk.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) $(STDERROUT)
!endif
# slmin
!if "$(SLMIN)" == "1"
	@echo Checking in WRK DLL...
	@cd $(CAFEDIR)\support\guitools\ide\wrk
	@$(MAKE) -$(MAKEFLAGS) -f wrk.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) slmin $(STDERROUT)
!endif
	@echo ----------------------------------

# ---------------------------------------------
# build prj portion of support system
# ---------------------------------------------
prj: set_env
	@echo ----------------------------------
# ssync
!if "$(SSYNC)" == "1"
	@echo Ssyncing PRJ DLL...
	@cd $(CAFEDIR)\support\guitools\ide\prj
	@$(MAKE) -$(MAKEFLAGS) -f prj.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) ssync $(STDERROUT)
!endif
# slmout
!if "$(SLMOUT)" == "1"
	@echo Checking out PRJ DLL...
	@cd $(CAFEDIR)\support\guitools\ide\prj
	@$(MAKE) -$(MAKEFLAGS) -f prj.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) slmout $(STDERROUT)
!endif
# autodep
!if "$(AUTODEP)" == "1"
	@echo Generating dependency file for PRJ DLL...
	@cd $(CAFEDIR)\support\guitools\ide\prj
	@$(MAKE) -$(MAKEFLAGS) -f prj.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) auto.dep $(STDERROUT)
!endif
# clean
!if "$(CLEAN)" == "1"
	@echo Cleaning PRJ DLL...
	@cd $(CAFEDIR)\support\guitools\ide\prj
	@$(MAKE) -$(MAKEFLAGS) -f prj.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) clean $(STDERROUT)
!endif
# build
!if "$(BUILD)" == "1"
	@echo Building PRJ DLL...
	@cd $(CAFEDIR)\support\guitools\ide\prj
	@$(MAKE) -$(MAKEFLAGS) -f prj.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) $(STDERROUT)
!endif
# slmin
!if "$(SLMIN)" == "1"
	@echo Checking in PRJ DLL...
	@cd $(CAFEDIR)\support\guitools\ide\prj
	@$(MAKE) -$(MAKEFLAGS) -f prj.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) slmin $(STDERROUT)
!endif
	@echo ----------------------------------

# ---------------------------------------------
# build dbg portion of support system
# ---------------------------------------------
dbg: set_env
	@echo ----------------------------------
# ssync
!if "$(SSYNC)" == "1"
	@echo Ssyncing DBG DLL...
	@cd $(CAFEDIR)\support\guitools\ide\dbg
	@$(MAKE) -$(MAKEFLAGS) -f dbg.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) ssync $(STDERROUT)
!endif
# slmout
!if "$(SLMOUT)" == "1"
	@echo Checking out DBG DLL...
	@cd $(CAFEDIR)\support\guitools\ide\dbg
	@$(MAKE) -$(MAKEFLAGS) -f dbg.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) slmout $(STDERROUT)
!endif
# autodep
!if "$(AUTODEP)" == "1"
	@echo Generating dependency file for DBG DLL...
	@cd $(CAFEDIR)\support\guitools\ide\dbg
	@$(MAKE) -$(MAKEFLAGS) -f dbg.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) auto.dep $(STDERROUT)
!endif
# clean
!if "$(CLEAN)" == "1"
	@echo Cleaning DBG DLL...
	@cd $(CAFEDIR)\support\guitools\ide\dbg
	@$(MAKE) -$(MAKEFLAGS) -f dbg.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) clean $(STDERROUT)
!endif
# build
!if "$(BUILD)" == "1"
	@echo Building DBG DLL...
	@cd $(CAFEDIR)\support\guitools\ide\dbg
	@$(MAKE) -$(MAKEFLAGS) -f dbg.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) $(STDERROUT)
!endif
# slmin
!if "$(SLMIN)" == "1"
	@echo Checking in DBG DLL...
	@cd $(CAFEDIR)\support\guitools\ide\dbg
	@$(MAKE) -$(MAKEFLAGS) -f dbg.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) slmin $(STDERROUT)
!endif
	@echo ----------------------------------

# ---------------------------------------------
# build ide portion of support system
# ---------------------------------------------
ide: set_env
	@echo ----------------------------------
# ssync
!if "$(SSYNC)" == "1"
	@echo Ssyncing IDE DLL...
	@cd $(CAFEDIR)\support\guitools\ide
	@$(MAKE) -$(MAKEFLAGS) -f ide.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) ssync $(STDERROUT)
!endif
# slmout
!if "$(SLMOUT)" == "1"
	@echo Checking out IDE DLL...
	@cd $(CAFEDIR)\support\guitools\ide
	@$(MAKE) -$(MAKEFLAGS) -f ide.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) slmout $(STDERROUT)
!endif
# autodep
!if "$(AUTODEP)" == "1"
	@echo Generating dependency file for IDE DLL...
	@cd $(CAFEDIR)\support\guitools\ide
	@$(MAKE) -$(MAKEFLAGS) -f ide.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) auto.dep $(STDERROUT)
!endif
# clean
!if "$(CLEAN)" == "1"
	@echo Cleaning IDE DLL...
	@cd $(CAFEDIR)\support\guitools\ide
	@$(MAKE) -$(MAKEFLAGS) -f ide.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) clean $(STDERROUT)
!endif
# build
!if "$(BUILD)" == "1"
	@echo Building IDE DLL...
	@cd $(CAFEDIR)\support\guitools\ide
	@$(MAKE) -$(MAKEFLAGS) -f ide.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) $(STDERROUT)
!endif
# slmin
!if "$(SLMIN)" == "1"
	@echo Checking in IDE DLL...
	@cd $(CAFEDIR)\support\guitools\ide
	@$(MAKE) -$(MAKEFLAGS) -f ide.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) slmin $(STDERROUT)
!endif
	@echo ----------------------------------

# ---------------------------------------------
# build bldtools portion of support system
# ---------------------------------------------
bldtools: set_env
	@echo ----------------------------------
# ssync
!if "$(SSYNC)" == "1"
	@echo Ssyncing BldTools DLL...
	@cd $(CAFEDIR)\support\bldtools
	@$(MAKE) -$(MAKEFLAGS) -f bldtools.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) ssync $(STDERROUT)
!endif
# slmout
!if "$(SLMOUT)" == "1"
	@echo Checking out BldTools DLL...
	@cd $(CAFEDIR)\support\bldtools
	@$(MAKE) -$(MAKEFLAGS) -f bldtools.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) slmout $(STDERROUT)
!endif
# autodep
!if "$(AUTODEP)" == "1"
	@echo Generating dependency file for BldTools DLL...
	@cd $(CAFEDIR)\support\bldtools
	@$(MAKE) -$(MAKEFLAGS) -f bldtools.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) auto.dep $(STDERROUT)
!endif
# clean
!if "$(CLEAN)" == "1"
	@echo Cleaning BldTools DLL...
	@cd $(CAFEDIR)\support\bldtools
	@$(MAKE) -$(MAKEFLAGS) -f bldtools.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) clean $(STDERROUT)
!endif
# build
!if "$(BUILD)" == "1"
	@echo Building BldTools DLL...
	@cd $(CAFEDIR)\support\bldtools
	@$(MAKE) -$(MAKEFLAGS) -f bldtools.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) $(STDERROUT)
!endif
# slmin
!if "$(SLMIN)" == "1"
	@echo Checking in BldTools DLL...
	@cd $(CAFEDIR)\support\bldtools
	@$(MAKE) -$(MAKEFLAGS) -f bldtools.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) slmin $(STDERROUT)
!endif
	@echo ----------------------------------

# ---------------------------------------------
# build toolset portion of support system
# ---------------------------------------------
toolset: set_env
	@echo ----------------------------------
# ssync
!if "$(SSYNC)" == "1"
	@echo Ssyncing Toolset DLL...
	@cd $(CAFEDIR)\support
	@$(MAKE) -$(MAKEFLAGS) -f toolset.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) ssync $(STDERROUT)
!endif
# slmout
!if "$(SLMOUT)" == "1"
	@echo Checking out Toolset DLL...
	@cd $(CAFEDIR)\support
	@$(MAKE) -$(MAKEFLAGS) -f toolset.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) slmout $(STDERROUT)
!endif
# autodep
!if "$(AUTODEP)" == "1"
	@echo Generating dependency file for Toolset DLL...
	@cd $(CAFEDIR)\support
	@$(MAKE) -$(MAKEFLAGS) -f toolset.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) auto.dep $(STDERROUT)
!endif
# clean
!if "$(CLEAN)" == "1"
	@echo Cleaning Toolset DLL...
	@cd $(CAFEDIR)\support
	@$(MAKE) -$(MAKEFLAGS) -f toolset.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) clean       $(STDERROUT)
!endif
# build
!if "$(BUILD)" == "1"
	@echo Building Toolset DLL...
	@cd $(CAFEDIR)\support
	@$(MAKE) -$(MAKEFLAGS) -f toolset.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) $(STDERROUT)
!endif
# slmin
!if "$(SLMIN)" == "1"
	@echo Checking in Toolset DLL...
	@cd $(CAFEDIR)\support
	@$(MAKE) -$(MAKEFLAGS) -f toolset.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) slmin $(STDERROUT)
!endif
	@echo ----------------------------------

# ---------------------------------------------
# build subsuite library
# ---------------------------------------------
subsuite: set_env
	@echo ----------------------------------
# ssync
!if "$(SSYNC)" == "1"
	@echo Ssyncing SubSuite LIB...
	@cd $(CAFEDIR)\subsuite
	@$(MAKE) -$(MAKEFLAGS) -f subsuite.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) ssync $(STDERROUT)
!endif
# slmout
!if "$(SLMOUT)" == "1"
	@echo Checking out SubSuite LIB...
	@cd $(CAFEDIR)\subsuite
	@$(MAKE) -$(MAKEFLAGS) -f subsuite.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) slmout $(STDERROUT)
!endif
# autodep
!if "$(AUTODEP)" == "1"
	@echo Generating dependency file for SubSuite LIB...
	@cd $(CAFEDIR)\subsuite
	@$(MAKE) -$(MAKEFLAGS) -f subsuite.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) auto.dep $(STDERROUT)
!endif
# clean
!if "$(CLEAN)" == "1"
	@echo Cleaning SubSuite LIB...
	@cd $(CAFEDIR)\subsuite
	@$(MAKE) -$(MAKEFLAGS) -f subsuite.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) clean $(STDERROUT)
!endif
# build
!if "$(BUILD)" == "1"
	@echo Building SubSuite LIB...
	@cd $(CAFEDIR)\subsuite
	@$(MAKE) -$(MAKEFLAGS) -f subsuite.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) $(STDERROUT)
!endif
# slmin
!if "$(SLMIN)" == "1"
	@echo Checking in SubSuite LIB...
	@cd $(CAFEDIR)\subsuite
	@$(MAKE) -$(MAKEFLAGS) -f subsuite.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) slmin $(STDERROUT)
!endif
	@echo ----------------------------------

# ---------------------------------------------
# build dav portion of support system
# ---------------------------------------------
dav: set_env
	@echo ----------------------------------
# ssync
!if "$(SSYNC)" == "1"
	@echo Ssyncing DAV DLL...
	@cd $(CAFEDIR)\support\guitools\ide\dav
	@$(MAKE) -$(MAKEFLAGS) -f dav.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) ssync $(STDERROUT)
!endif
# slmout
!if "$(SLMOUT)" == "1"
	@echo Checking out DAV DLL...
	@cd $(CAFEDIR)\support\guitools\ide\dav
	@$(MAKE) -$(MAKEFLAGS) -f dav.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) slmout $(STDERROUT)
!endif
# autodep
!if "$(AUTODEP)" == "1"
	@echo Generating dependency file for DAV DLL...
	@cd $(CAFEDIR)\support\guitools\ide\dav
	@$(MAKE) -$(MAKEFLAGS) -f dav.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) auto.dep $(STDERROUT)
!endif
# clean
!if "$(CLEAN)" == "1"
	@echo Cleaning DAV DLL...
	@cd $(CAFEDIR)\support\guitools\ide\dav
	@$(MAKE) -$(MAKEFLAGS) -f dav.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) clean $(STDERROUT)
!endif
# build
!if "$(BUILD)" == "1"
	@echo Building DAV DLL...
	@cd $(CAFEDIR)\support\guitools\ide\dav
	@$(MAKE) -$(MAKEFLAGS) -f dav.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) $(STDERROUT)
!endif
# slmin
!if "$(SLMIN)" == "1"
	@echo Checking in DAV DLL...
	@cd $(CAFEDIR)\support\guitools\ide\dav
	@$(MAKE) -$(MAKEFLAGS) -f dav.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) slmin $(STDERROUT)
!endif
	@echo ----------------------------------

# ---------------------------------------------
# build CAFE driver
# ---------------------------------------------
cafedrv: set_env
	@echo ----------------------------------
# ssync
!if "$(SSYNC)" == "1"
	@echo Ssyncing CAFE driver EXE...
	@cd $(CAFEDIR)\cafedrv
	@$(MAKE) -$(MAKEFLAGS) -f cafedrv.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) ssync $(STDERROUT)
!endif
# slmout
!if "$(SLMOUT)" == "1"
	@echo Checking out CAFE driver EXE...
	@cd $(CAFEDIR)\cafedrv
	@$(MAKE) -$(MAKEFLAGS) -f cafedrv.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) slmout $(STDERROUT)
!endif
# autodep
!if "$(AUTODEP)" == "1"
	@echo Generating dependency file for CAFE driver EXE...
	@cd $(CAFEDIR)\cafedrv
	@$(MAKE) -$(MAKEFLAGS) -f cafedrv.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) auto.dep $(STDERROUT)
!endif
# clean
!if "$(CLEAN)" == "1"
	@echo Cleaning CAFE driver EXE...
	@cd $(CAFEDIR)\cafedrv
	@$(MAKE) -$(MAKEFLAGS) -f cafedrv.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) clean $(STDERROUT)
!endif
# build
!if "$(BUILD)" == "1"
	@echo Building CAFE driver EXE...
	@cd $(CAFEDIR)\cafedrv
	@$(MAKE) -$(MAKEFLAGS) -f cafedrv.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) $(STDERROUT)
!endif
# slmin
!if "$(SLMIN)" == "1"
	@echo Checking in CAFE driver EXE...
	@cd $(CAFEDIR)\cafedrv
	@$(MAKE) -$(MAKEFLAGS) -f cafedrv.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) slmin $(STDERROUT)
!endif
	@echo ----------------------------------

# ---------------------------------------------
# munge IDE headers for our use
# ---------------------------------------------
ideheaders: set_env
	@echo ----------------------------------
	@echo Munging IDE headers...
	@echo This isn't working yet...
#       @cd $(CAFEDIR)\cafe\wb\sym
#       @$(MAKE) /f mungehdr.mak all $(STDERROUT)
	@echo ----------------------------------



# ---------------------------------------------
# ---------------------------------------------
# sniff tests
# ---------------------------------------------

# ---------------------------------------------
# build browser sniff
# ---------------------------------------------
browser: set_env
	@echo ----------------------------------
# ssync
!if "$(SSYNC)" == "1"
	@echo Ssyncing Browser sniff test...
	@cd $(CAFEDIR)\sniff\browser
	@$(MAKE) -$(MAKEFLAGS) -f browser.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) EXEDIR=$(SNIFFDIR)\browser ssync $(STDERROUT)
!endif
# slmout
!if "$(SLMOUT)" == "1"
	@echo Checking out Browser sniff test...
	@cd $(CAFEDIR)\sniff\browser
	@$(MAKE) -$(MAKEFLAGS) -f browser.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) EXEDIR=$(SNIFFDIR)\browser slmout $(STDERROUT)
!endif
# clean
!if "$(CLEAN)" == "1"
	@echo Cleaning Browser sniff test...
	@cd $(CAFEDIR)\sniff\browser
	@$(MAKE) -$(MAKEFLAGS) -f browser.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) EXEDIR=$(SNIFFDIR)\browser clean $(STDERROUT)
!endif
# build
!if "$(BUILD)" == "1"
	@echo Building Browser sniff test...
	@cd $(CAFEDIR)\sniff\browser
	@$(MAKE) -$(MAKEFLAGS) -f browser.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) EXEDIR=$(SNIFFDIR)\browser $(STDERROUT)
!endif
# slmin
!if "$(SLMIN)" == "1"
	@echo Checking in Browser sniff test...
	@cd $(CAFEDIR)\sniff\browser
	@$(MAKE) -$(MAKEFLAGS) -f browser.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) EXEDIR=$(SNIFFDIR)\browser slmin $(STDERROUT)
!endif
	@echo ----------------------------------

# ---------------------------------------------
# build data sniff
# ---------------------------------------------
data: set_env
	@echo ----------------------------------
# ssync
!if "$(SSYNC)" == "1"
	@echo Ssyncing Data sniff test...
	@cd $(CAFEDIR)\sniff\data
	@$(MAKE) -$(MAKEFLAGS) -f data.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) EXEDIR=$(SNIFFDIR)\data ssync $(STDERROUT)
!endif
# slmout
!if "$(SLMOUT)" == "1"
	@echo Checking out Data sniff test...
	@cd $(CAFEDIR)\sniff\data
	@$(MAKE) -$(MAKEFLAGS) -f data.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) EXEDIR=$(SNIFFDIR)\data slmout $(STDERROUT)
!endif
# clean
!if "$(CLEAN)" == "1"
	@echo Cleaning Data sniff test...
	@cd $(CAFEDIR)\sniff\data
	@$(MAKE) -$(MAKEFLAGS) -f data.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) EXEDIR=$(SNIFFDIR)\data clean $(STDERROUT)
!endif
# build
!if "$(BUILD)" == "1"
	@echo Building Data sniff test...
	@cd $(CAFEDIR)\sniff\data
	@$(MAKE) -$(MAKEFLAGS) -f data.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) EXEDIR=$(SNIFFDIR)\data $(STDERROUT)
!endif
# slmin
!if "$(SLMIN)" == "1"
	@echo Checking in Data sniff test...
	@cd $(CAFEDIR)\sniff\data
	@$(MAKE) -$(MAKEFLAGS) -f data.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) EXEDIR=$(SNIFFDIR)\data slmin $(STDERROUT)
!endif
	@echo ----------------------------------

# ---------------------------------------------
# build sdi sniff
# ---------------------------------------------
sdi: set_env
	@echo ----------------------------------
# ssync
!if "$(SSYNC)" == "1"
	@echo Ssyncing Sdi sniff test...
	@cd $(CAFEDIR)\sniff\sdi
	@$(MAKE) -$(MAKEFLAGS) -f sdi.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) EXEDIR=$(SNIFFDIR)\sdi ssync $(STDERROUT)
!endif
# slmout
!if "$(SLMOUT)" == "1"
	@echo Checking out Sdi sniff test...
	@cd $(CAFEDIR)\sniff\sdi
	@$(MAKE) -$(MAKEFLAGS) -f sdi.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) EXEDIR=$(SNIFFDIR)\sdi slmout $(STDERROUT)
!endif
# clean
!if "$(CLEAN)" == "1"
	@echo Cleaning Sdi sniff test...
	@cd $(CAFEDIR)\sniff\sdi
	@$(MAKE) -$(MAKEFLAGS) -f sdi.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) EXEDIR=$(SNIFFDIR)\sdi clean $(STDERROUT)
!endif
# build
!if "$(BUILD)" == "1"
	@echo Building Sdi sniff test...
	@cd $(CAFEDIR)\sniff\sdi
	@$(MAKE) -$(MAKEFLAGS) -f sdi.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) EXEDIR=$(SNIFFDIR)\sdi $(STDERROUT)
!endif
# slmin
!if "$(SLMIN)" == "1"
	@echo Checking in Sdi sniff test...
	@cd $(CAFEDIR)\sniff\sdi
	@$(MAKE) -$(MAKEFLAGS) -f sdi.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) EXEDIR=$(SNIFFDIR)\sdi slmin $(STDERROUT)
!endif
	@echo ----------------------------------

# ---------------------------------------------
# build dbg_mbc sniff
# ---------------------------------------------
dbg_mbc: set_env
	@echo ----------------------------------
# ssync
!if "$(SSYNC)" == "1"
	@echo Ssyncing Dbg_mbc sniff test...
	@cd $(CAFEDIR)\sniff\dbg_mbc
	@$(MAKE) -$(MAKEFLAGS) -f dbg_mbc.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) EXEDIR=$(SNIFFDIR)\dbg_mbc ssync $(STDERROUT)
!endif
# slmout
!if "$(SLMOUT)" == "1"
	@echo Checking out Dbg_mbc sniff test...
	@cd $(CAFEDIR)\sniff\dbg_mbc
	@$(MAKE) -$(MAKEFLAGS) -f dbg_mbc.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) EXEDIR=$(SNIFFDIR)\dbg_mbc slmout $(STDERROUT)
!endif
# clean
!if "$(CLEAN)" == "1"
	@echo Cleaning Dbg_mbc sniff test...
	@cd $(CAFEDIR)\sniff\dbg_mbc
	@$(MAKE) -$(MAKEFLAGS) -f dbg_mbc.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) EXEDIR=$(SNIFFDIR)\dbg_mbc clean $(STDERROUT)
!endif
# build
!if "$(BUILD)" == "1"
	@echo Building Dbg_mbc sniff test...
	@cd $(CAFEDIR)\sniff\dbg_mbc
	@$(MAKE) -$(MAKEFLAGS) -f dbg_mbc.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) EXEDIR=$(SNIFFDIR)\dbg_mbc $(STDERROUT)
!endif
# slmin
!if "$(SLMIN)" == "1"
	@echo Checking in Dbg_mbc sniff test...
	@cd $(CAFEDIR)\sniff\dbg_mbc
	@$(MAKE) -$(MAKEFLAGS) -f dbg_mbc.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) EXEDIR=$(SNIFFDIR)\dbg_mbc slmin $(STDERROUT)
!endif
	@echo ----------------------------------

# ---------------------------------------------
# build debugger sniff
# ---------------------------------------------
debugger: set_env
	@echo ----------------------------------
# ssync
!if "$(SSYNC)" == "1"
	@echo Ssyncing Debugger sniff test...
	@cd $(CAFEDIR)\sniff\debugger
	@$(MAKE) -$(MAKEFLAGS) -f debug.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) EXEDIR=$(SNIFFDIR)\debugger ssync $(STDERROUT)
!endif
# slmout
!if "$(SLMOUT)" == "1"
	@echo Checking out Debugger sniff test...
	@cd $(CAFEDIR)\sniff\debugger
	@$(MAKE) -$(MAKEFLAGS) -f debug.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) EXEDIR=$(SNIFFDIR)\debugger slmout $(STDERROUT)
!endif
# clean
!if "$(CLEAN)" == "1"
	@echo Cleaning Debugger sniff test...
	@cd $(CAFEDIR)\sniff\debugger
	@$(MAKE) -$(MAKEFLAGS) -f debug.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) EXEDIR=$(SNIFFDIR)\debugger clean $(STDERROUT)
!endif
# build
!if "$(BUILD)" == "1"
	@echo Building Debugger sniff test...
	@cd $(CAFEDIR)\sniff\debugger
	@$(MAKE) -$(MAKEFLAGS) -f debug.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) EXEDIR=$(SNIFFDIR)\debugger $(STDERROUT)
!endif
# slmin
!if "$(SLMIN)" == "1"
	@echo Checking in Debugger sniff test...
	@cd $(CAFEDIR)\sniff\debugger
	@$(MAKE) -$(MAKEFLAGS) -f debug.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) EXEDIR=$(SNIFFDIR)\debugger slmin $(STDERROUT)
!endif
	@echo ----------------------------------

# ---------------------------------------------
# build editgo sniff
# ---------------------------------------------
#editgo: set_env
#!if "$(EDITGO_BUILD)" == "1"
#	@echo ----------------------------------
# ssync
#!if "$(SSYNC)" == "1"
#	@echo Ssyncing EditGo sniff test...
#	@cd $(CAFEDIR)\sniff\editgo
#	@$(MAKE) -$(MAKEFLAGS) -f editgo.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) EXEDIR=$(SNIFFDIR)\editgo ssync $(STDERROUT)
#!endif
# slmout
#!if "$(SLMOUT)" == "1"
#	@echo Checking out EditGo sniff test...
#	@cd $(CAFEDIR)\sniff\editgo
#	@$(MAKE) -$(MAKEFLAGS) -f editgo.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) EXEDIR=$(SNIFFDIR)\editgo slmout $(STDERROUT)
#!endif
# clean
#!if "$(CLEAN)" == "1"
#	@echo Cleaning EditGo sniff test...
#	@cd $(CAFEDIR)\sniff\editgo
#	@$(MAKE) -$(MAKEFLAGS) -f editgo.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) EXEDIR=$(SNIFFDIR)\editgo clean $(STDERROUT)
#!endif
# build
#!if "$(BUILD)" == "1"
#	@echo Building EditGo sniff test...
#	@cd $(CAFEDIR)\sniff\editgo
#	@$(MAKE) -$(MAKEFLAGS) -f editgo.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) EXEDIR=$(SNIFFDIR)\editgo $(STDERROUT)
#!endif
# slmin
#!if "$(SLMIN)" == "1"
#	@echo Checking in EditGo sniff test...
#	@cd $(CAFEDIR)\sniff\editgo
#	@$(MAKE) -$(MAKEFLAGS) -f editgo.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) EXEDIR=$(SNIFFDIR)\editgo slmin $(STDERROUT)
#!endif
#	@echo ----------------------------------
#!endif

# ---------------------------------------------
# build editor sniff
# ---------------------------------------------
editor: set_env
	@echo ----------------------------------
# ssync
!if "$(SSYNC)" == "1"
	@echo Ssyncing Editor sniff test...
	@cd $(CAFEDIR)\sniff\editor
	@$(MAKE) -$(MAKEFLAGS) -f editor.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) EXEDIR=$(SNIFFDIR)\editor ssync $(STDERROUT)
!endif
# slmout
!if "$(SLMOUT)" == "1"
	@echo Checking out Editor sniff test...
	@cd $(CAFEDIR)\sniff\editor
	@$(MAKE) -$(MAKEFLAGS) -f editor.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) EXEDIR=$(SNIFFDIR)\editor slmout $(STDERROUT)
!endif
# clean
!if "$(CLEAN)" == "1"
	@echo Cleaning Editor sniff test...
	@cd $(CAFEDIR)\sniff\editor
	@$(MAKE) -$(MAKEFLAGS) -f editor.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) EXEDIR=$(SNIFFDIR)\editor clean $(STDERROUT)
!endif
# build
!if "$(BUILD)" == "1"
	@echo Building Editor sniff test...
	@cd $(CAFEDIR)\sniff\editor
	@$(MAKE) -$(MAKEFLAGS) -f editor.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) EXEDIR=$(SNIFFDIR)\editor $(STDERROUT)
!endif
# slmin
!if "$(SLMIN)" == "1"
	@echo Checking in Editor sniff test...
	@cd $(CAFEDIR)\sniff\editor
	@$(MAKE) -$(MAKEFLAGS) -f editor.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) EXEDIR=$(SNIFFDIR)\editor slmin $(STDERROUT)
!endif
	@echo ----------------------------------

# ---------------------------------------------
# build sys sniff
# ---------------------------------------------
sys: set_env
	@echo ----------------------------------
# ssync
!if "$(SSYNC)" == "1"
	@echo Ssyncing Sys sniff test...
	@cd $(CAFEDIR)\sniff\sys
	@$(MAKE) -$(MAKEFLAGS) -f sys.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) EXEDIR=$(SNIFFDIR)\sys ssync $(STDERROUT)
!endif
# slmout
!if "$(SLMOUT)" == "1"
	@echo Checking out Sys sniff test...
	@cd $(CAFEDIR)\sniff\sys
	@$(MAKE) -$(MAKEFLAGS) -f sys.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) EXEDIR=$(SNIFFDIR)\sys slmout $(STDERROUT)
!endif
# clean
!if "$(CLEAN)" == "1"
	@echo Cleaning Sys sniff test...
	@cd $(CAFEDIR)\sniff\sys
	@$(MAKE) -$(MAKEFLAGS) -f sys.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) EXEDIR=$(SNIFFDIR)\sys clean $(STDERROUT)
!endif
# build
!if "$(BUILD)" == "1"
	@echo Building Sys sniff test...
	@cd $(CAFEDIR)\sniff\sys
	@$(MAKE) -$(MAKEFLAGS) -f sys.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) EXEDIR=$(SNIFFDIR)\sys $(STDERROUT)
!endif
# slmin
!if "$(SLMIN)" == "1"
	@echo Checking in Sys sniff test...
	@cd $(CAFEDIR)\sniff\sys
	@$(MAKE) -$(MAKEFLAGS) -f sys.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) EXEDIR=$(SNIFFDIR)\sys slmin $(STDERROUT)
!endif
	@echo ----------------------------------

# ---------------------------------------------
# build vproj sniff
# ---------------------------------------------
vproj: set_env
	@echo ----------------------------------
# ssync
!if "$(SSYNC)" == "1"
	@echo Ssyncing VProj sniff test...
	@cd $(CAFEDIR)\sniff\vproj
	@$(MAKE) -$(MAKEFLAGS) -f vproj.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) EXEDIR=$(SNIFFDIR)\vproj ssync $(STDERROUT)
!endif
# slmout
!if "$(SLMOUT)" == "1"
	@echo Checking out VProj sniff test...
	@cd $(CAFEDIR)\sniff\vproj
	@$(MAKE) -$(MAKEFLAGS) -f vproj.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) EXEDIR=$(SNIFFDIR)\vproj slmout $(STDERROUT)
!endif
# clean
!if "$(CLEAN)" == "1"
	@echo Cleaning VProj sniff test...
	@cd $(CAFEDIR)\sniff\vproj
	@$(MAKE) -$(MAKEFLAGS) -f vproj.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) EXEDIR=$(SNIFFDIR)\vproj clean $(STDERROUT)
!endif
# build
!if "$(BUILD)" == "1"
	@echo Building VProj sniff test...
	@cd $(CAFEDIR)\sniff\vproj
	@$(MAKE) -$(MAKEFLAGS) -f vproj.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) EXEDIR=$(SNIFFDIR)\vproj $(STDERROUT)
!endif
# slmin
!if "$(SLMIN)" == "1"
	@echo Checking in VProj sniff test...
	@cd $(CAFEDIR)\sniff\vproj
	@$(MAKE) -$(MAKEFLAGS) -f vproj.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) EXEDIR=$(SNIFFDIR)\vproj slmin $(STDERROUT)
!endif
	@echo ----------------------------------

# ---------------------------------------------
# build vres sniff
# ---------------------------------------------
vres: set_env
	@echo ----------------------------------
# ssync
!if "$(SSYNC)" == "1"
	@echo Ssyncing VRes sniff test...
	@cd $(CAFEDIR)\sniff\vres
	@$(MAKE) -$(MAKEFLAGS) -f vres.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) EXEDIR=$(SNIFFDIR)\vres ssync $(STDERROUT)
!endif
# slmout
!if "$(SLMOUT)" == "1"
	@echo Checking out VRes sniff test...
	@cd $(CAFEDIR)\sniff\vres
	@$(MAKE) -$(MAKEFLAGS) -f vres.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) EXEDIR=$(SNIFFDIR)\vres slmout $(STDERROUT)
!endif
# clean
!if "$(CLEAN)" == "1"
	@echo Cleaning VRes sniff test...
	@cd $(CAFEDIR)\sniff\vres
	@$(MAKE) -$(MAKEFLAGS) -f vres.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) EXEDIR=$(SNIFFDIR)\vres clean $(STDERROUT)
!endif
# build
!if "$(BUILD)" == "1"
	@echo Building VRes sniff test...
	@cd $(CAFEDIR)\sniff\vres
	@$(MAKE) -$(MAKEFLAGS) -f vres.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) EXEDIR=$(SNIFFDIR)\vres $(STDERROUT)
!endif
# slmin
!if "$(SLMIN)" == "1"
	@echo Checking in VRes sniff test...
	@cd $(CAFEDIR)\sniff\vres
	@$(MAKE) -$(MAKEFLAGS) -f vres.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) EXEDIR=$(SNIFFDIR)\vres slmin $(STDERROUT)
!endif
	@echo ----------------------------------

# ---------------------------------------------
# build vshell sniff
# ---------------------------------------------
vshell: set_env
	@echo ----------------------------------
# ssync
!if "$(SSYNC)" == "1"
	@echo Ssyncing VShell sniff test...
	@cd $(CAFEDIR)\sniff\vshell
	@$(MAKE) -$(MAKEFLAGS) -f vshell.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) EXEDIR=$(SNIFFDIR)\vshell ssync $(STDERROUT)
!endif
# slmout
!if "$(SLMOUT)" == "1"
	@echo Checking out VShell sniff test...
	@cd $(CAFEDIR)\sniff\vshell
	@$(MAKE) -$(MAKEFLAGS) -f vshell.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) EXEDIR=$(SNIFFDIR)\vshell slmout $(STDERROUT)
!endif
# clean
!if "$(CLEAN)" == "1"
	@echo Cleaning VShell sniff test...
	@cd $(CAFEDIR)\sniff\vshell
	@$(MAKE) -$(MAKEFLAGS) -f vshell.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) EXEDIR=$(SNIFFDIR)\vshell clean $(STDERROUT)
!endif
# build
!if "$(BUILD)" == "1"
	@echo Building VShell sniff test...
	@cd $(CAFEDIR)\sniff\vshell
	@$(MAKE) -$(MAKEFLAGS) -f vshell.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) EXEDIR=$(SNIFFDIR)\vshell $(STDERROUT)
!endif
# slmin
!if "$(SLMIN)" == "1"
	@echo Checking in VShell sniff test...
	@cd $(CAFEDIR)\sniff\vshell
	@$(MAKE) -$(MAKEFLAGS) -f vshell.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) EXEDIR=$(SNIFFDIR)\vshell slmin $(STDERROUT)
!endif
	@echo ----------------------------------

# ---------------------------------------------
# build wizards sniff
# ---------------------------------------------
wizards: set_env
	@echo ----------------------------------
# ssync
!if "$(SSYNC)" == "1"
	@echo Ssyncing Wizards sniff test...
	@cd $(CAFEDIR)\sniff\wizards
	@$(MAKE) -$(MAKEFLAGS) -f wizards.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) EXEDIR=$(SNIFFDIR)\wizards ssync $(STDERROUT)
!endif
# slmout
!if "$(SLMOUT)" == "1"
	@echo Checking out Wizards sniff test...
	@cd $(CAFEDIR)\sniff\wizards
	@$(MAKE) -$(MAKEFLAGS) -f wizards.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) EXEDIR=$(SNIFFDIR)\wizards slmout $(STDERROUT)
!endif
# clean
!if "$(CLEAN)" == "1"
	@echo Cleaning Wizards sniff test...
	@cd $(CAFEDIR)\sniff\wizards
	@$(MAKE) -$(MAKEFLAGS) -f wizards.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) EXEDIR=$(SNIFFDIR)\wizards clean $(STDERROUT)
!endif
# build
!if "$(BUILD)" == "1"
	@echo Building Wizards sniff test...
	@cd $(CAFEDIR)\sniff\wizards
	@$(MAKE) -$(MAKEFLAGS) -f wizards.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) EXEDIR=$(SNIFFDIR)\wizards $(STDERROUT)
!endif
# slmin
!if "$(SLMIN)" == "1"
	@echo Checking in Wizards sniff test...
	@cd $(CAFEDIR)\sniff\wizards
	@$(MAKE) -$(MAKEFLAGS) -f wizards.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) EXEDIR=$(SNIFFDIR)\wizards slmin $(STDERROUT)
!endif
	@echo ----------------------------------

# ---------------------------------------------
# build fortran sniff
# ---------------------------------------------
fortran: set_env
	@echo ----------------------------------
# ssync
!if "$(SSYNC)" == "1"
	@echo Ssyncing Fortran sniff test...
	@cd $(CAFEDIR)\sniff\fortran
	@$(MAKE) -$(MAKEFLAGS) -f fortran.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) EXEDIR=$(SNIFFDIR)\fortran ssync $(STDERROUT)
!endif
# slmout
!if "$(SLMOUT)" == "1"
	@echo Checking out Fortran sniff test...
	@cd $(CAFEDIR)\sniff\fortran
	@$(MAKE) -$(MAKEFLAGS) -f fortran.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) EXEDIR=$(SNIFFDIR)\fortran slmout $(STDERROUT)
!endif
# clean
!if "$(CLEAN)" == "1"
	@echo Cleaning Fortran sniff test...
	@cd $(CAFEDIR)\sniff\fortran
	@$(MAKE) -$(MAKEFLAGS) -f fortran.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) EXEDIR=$(SNIFFDIR)\fortran clean $(STDERROUT)
!endif
# build
!if "$(BUILD)" == "1"
	@echo Building Fortran sniff test...
	@cd $(CAFEDIR)\sniff\fortran
	@$(MAKE) -$(MAKEFLAGS) -f fortran.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) EXEDIR=$(SNIFFDIR)\fortran $(STDERROUT)
!endif
# slmin
!if "$(SLMIN)" == "1"
	@echo Checking in Fortran sniff test...
	@cd $(CAFEDIR)\sniff\fortran
	@$(MAKE) -$(MAKEFLAGS) -f fortran.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) EXEDIR=$(SNIFFDIR)\fortran slmin $(STDERROUT)
!endif
	@echo ----------------------------------

# ---------------------------------------------
# build javasys sniff
# ---------------------------------------------
javasys: set_env
	@echo ----------------------------------
# ssync
!if "$(SSYNC)" == "1"
        @echo Ssyncing javasys sniff test...
        @cd $(CAFEDIR)\sniff\javasys
        @$(MAKE) -$(MAKEFLAGS) -f javasys.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) EXEDIR=$(SNIFFDIR)\javasys ssync $(STDERROUT)
!endif
# slmout
!if "$(SLMOUT)" == "1"
        @echo Checking out javasys sniff test...
        @cd $(CAFEDIR)\sniff\javasys
        @$(MAKE) -$(MAKEFLAGS) -f javasys.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) EXEDIR=$(SNIFFDIR)\javasys slmout $(STDERROUT)
!endif
# clean
!if "$(CLEAN)" == "1"
        @echo Cleaning javasys sniff test...
        @cd $(CAFEDIR)\sniff\javasys
        @$(MAKE) -$(MAKEFLAGS) -f javasys.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) EXEDIR=$(SNIFFDIR)\javasys clean $(STDERROUT)
!endif
# build
!if "$(BUILD)" == "1"
        @echo Building javasys sniff test...
        @cd $(CAFEDIR)\sniff\javasys
        @$(MAKE) -$(MAKEFLAGS) -f javasys.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) EXEDIR=$(SNIFFDIR)\javasys $(STDERROUT)
!endif
# slmin
!if "$(SLMIN)" == "1"
        @echo Checking in javasys sniff test...
        @cd $(CAFEDIR)\sniff\javasys
        @$(MAKE) -$(MAKEFLAGS) -f javasys.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) EXEDIR=$(SNIFFDIR)\javasys slmin $(STDERROUT)
!endif
	@echo ----------------------------------

# ---------------------------------------------
# build vjdebug sniff
# ---------------------------------------------
vjdebug: set_env
	@echo ----------------------------------
# ssync
!if "$(SSYNC)" == "1"
        @echo Ssyncing vjdebug sniff test...
        @cd $(CAFEDIR)\sniff\vjdebug
        @$(MAKE) -$(MAKEFLAGS) -f vjdebug.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) EXEDIR=$(SNIFFDIR)\vjdebug ssync $(STDERROUT)
!endif
# slmout
!if "$(SLMOUT)" == "1"
        @echo Checking out vjdebug sniff test...
        @cd $(CAFEDIR)\sniff\vjdebug
        @$(MAKE) -$(MAKEFLAGS) -f vjdebug.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) EXEDIR=$(SNIFFDIR)\vjdebug slmout $(STDERROUT)
!endif
# clean
!if "$(CLEAN)" == "1"
        @echo Cleaning vjdebug sniff test...
        @cd $(CAFEDIR)\sniff\vjdebug
        @$(MAKE) -$(MAKEFLAGS) -f vjdebug.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) EXEDIR=$(SNIFFDIR)\vjdebug clean $(STDERROUT)
!endif
# build
!if "$(BUILD)" == "1"
        @echo Building vjdebug sniff test...
        @cd $(CAFEDIR)\sniff\vjdebug
        @$(MAKE) -$(MAKEFLAGS) -f vjdebug.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) EXEDIR=$(SNIFFDIR)\vjdebug $(STDERROUT)
!endif
# slmin
!if "$(SLMIN)" == "1"
        @echo Checking in vjdebug sniff test...
        @cd $(CAFEDIR)\sniff\vjdebug
        @$(MAKE) -$(MAKEFLAGS) -f vjdebug.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) EXEDIR=$(SNIFFDIR)\vjdebug slmin $(STDERROUT)
!endif
	@echo ----------------------------------

# ---------------------------------------------
# build vjclsvw sniff
# ---------------------------------------------
vjclsvw: set_env
	@echo ----------------------------------
# ssync
!if "$(SSYNC)" == "1"
        @echo Ssyncing vjclsvw sniff test...
        @cd $(CAFEDIR)\sniff\vjclsvw
        @$(MAKE) -$(MAKEFLAGS) -f vjclsvw.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) EXEDIR=$(SNIFFDIR)\vjclsvw ssync $(STDERROUT)
!endif
# slmout
!if "$(SLMOUT)" == "1"
        @echo Checking out vjclsvw sniff test...
        @cd $(CAFEDIR)\sniff\vjclsvw
        @$(MAKE) -$(MAKEFLAGS) -f vjclsvw.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) EXEDIR=$(SNIFFDIR)\vjclsvw slmout $(STDERROUT)
!endif
# clean
!if "$(CLEAN)" == "1"
        @echo Cleaning vjclsvw sniff test...
        @cd $(CAFEDIR)\sniff\vjclsvw
        @$(MAKE) -$(MAKEFLAGS) -f vjclsvw.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) EXEDIR=$(SNIFFDIR)\vjclsvw clean $(STDERROUT)
!endif
# build
!if "$(BUILD)" == "1"
        @echo Building vjclsvw sniff test...
        @cd $(CAFEDIR)\sniff\vjclsvw
        @$(MAKE) -$(MAKEFLAGS) -f vjclsvw.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) EXEDIR=$(SNIFFDIR)\vjclsvw $(STDERROUT)
!endif
# slmin
!if "$(SLMIN)" == "1"
        @echo Checking in vjclsvw sniff test...
        @cd $(CAFEDIR)\sniff\vjclsvw
        @$(MAKE) -$(MAKEFLAGS) -f vjclsvw.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) EXEDIR=$(SNIFFDIR)\vjclsvw slmin $(STDERROUT)
!endif
	@echo ----------------------------------

# ---------------------------------------------
# build vjbuild sniff
# ---------------------------------------------
vjbuild: set_env
	@echo ----------------------------------
# ssync
!if "$(SSYNC)" == "1"
        @echo Ssyncing vjbuild sniff test...
        @cd $(CAFEDIR)\sniff\vjbuild
        @$(MAKE) -$(MAKEFLAGS) -f vjbuild.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) EXEDIR=$(SNIFFDIR)\vjbuild ssync $(STDERROUT)
!endif
# slmout
!if "$(SLMOUT)" == "1"
        @echo Checking out vjbuild sniff test...
        @cd $(CAFEDIR)\sniff\vjbuild
        @$(MAKE) -$(MAKEFLAGS) -f vjbuild.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) EXEDIR=$(SNIFFDIR)\vjbuild slmout $(STDERROUT)
!endif
# clean
!if "$(CLEAN)" == "1"
        @echo Cleaning vjbuild sniff test...
        @cd $(CAFEDIR)\sniff\vjbuild
        @$(MAKE) -$(MAKEFLAGS) -f vjbuild.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) EXEDIR=$(SNIFFDIR)\vjbuild clean $(STDERROUT)
!endif
# build
!if "$(BUILD)" == "1"
        @echo Building vjbuild sniff test...
        @cd $(CAFEDIR)\sniff\vjbuild
        @$(MAKE) -$(MAKEFLAGS) -f vjbuild.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) EXEDIR=$(SNIFFDIR)\vjbuild $(STDERROUT)
!endif
# slmin
!if "$(SLMIN)" == "1"
        @echo Checking in vjbuild sniff test...
        @cd $(CAFEDIR)\sniff\vjbuild
        @$(MAKE) -$(MAKEFLAGS) -f vjbuild.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) EXEDIR=$(SNIFFDIR)\vjbuild slmin $(STDERROUT)
!endif
	@echo ----------------------------------

# ---------------------------------------------
# build vjexe sniff
# ---------------------------------------------
vjexe: set_env
	@echo ----------------------------------
# ssync
!if "$(SSYNC)" == "1"
        @echo Ssyncing vjexe sniff test...
        @cd $(CAFEDIR)\sniff\vjexe
        @$(MAKE) -$(MAKEFLAGS) -f vjexe.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) EXEDIR=$(SNIFFDIR)\vjexe ssync $(STDERROUT)
!endif
# slmout
!if "$(SLMOUT)" == "1"
        @echo Checking out vjexe sniff test...
        @cd $(CAFEDIR)\sniff\vjexe
        @$(MAKE) -$(MAKEFLAGS) -f vjexe.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) EXEDIR=$(SNIFFDIR)\vjexe slmout $(STDERROUT)
!endif
# clean
!if "$(CLEAN)" == "1"
        @echo Cleaning vjexe sniff test...
        @cd $(CAFEDIR)\sniff\vjexe
        @$(MAKE) -$(MAKEFLAGS) -f vjexe.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) EXEDIR=$(SNIFFDIR)\vjexe clean $(STDERROUT)
!endif
# build
!if "$(BUILD)" == "1"
        @echo Building vjexe sniff test...
        @cd $(CAFEDIR)\sniff\vjexe
        @$(MAKE) -$(MAKEFLAGS) -f vjexe.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) EXEDIR=$(SNIFFDIR)\vjexe $(STDERROUT)
!endif
# slmin
!if "$(SLMIN)" == "1"
        @echo Checking in vjexe sniff test...
        @cd $(CAFEDIR)\sniff\vjexe
        @$(MAKE) -$(MAKEFLAGS) -f vjexe.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) EXEDIR=$(SNIFFDIR)\vjexe slmin $(STDERROUT)
!endif
	@echo ----------------------------------

# ---------------------------------------------
# build istudio sniff
# ---------------------------------------------
istudio: set_env
	@echo ----------------------------------
# ssync
!if "$(SSYNC)" == "1"
	@echo Ssyncing IStudio sniff test...
	@cd $(CAFEDIR)\sniff\istudio
	@$(MAKE) -$(MAKEFLAGS) -f is.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) EXEDIR=$(SNIFFDIR)\istudio ssync $(STDERROUT)
!endif
# slmout
!if "$(SLMOUT)" == "1"
	@echo Checking out IStudio sniff test...
	@cd $(CAFEDIR)\sniff\istudio
	@$(MAKE) -$(MAKEFLAGS) -f is.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) EXEDIR=$(SNIFFDIR)\istudio slmout $(STDERROUT)
!endif
# clean
!if "$(CLEAN)" == "1"
	@echo Cleaning istudio sniff test...
	@cd $(CAFEDIR)\sniff\istudio
	@$(MAKE) -$(MAKEFLAGS) -f is.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) EXEDIR=$(SNIFFDIR)\istudio clean $(STDERROUT)
!endif
# build
!if "$(BUILD)" == "1"
	@echo Building istudio sniff test...
	@cd $(CAFEDIR)\sniff\istudio 
	@$(MAKE) -$(MAKEFLAGS) -f is.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) EXEDIR=$(SNIFFDIR)\istudio $(STDERROUT)
!endif
# slmin
!if "$(SLMIN)" == "1"
	@echo Checking in istudio sniff test...
	@cd $(CAFEDIR)\sniff\istudio
	@$(MAKE) -$(MAKEFLAGS) -f is.mk DEBUG=$(DEBUG) BROWSE=$(BROWSE) DEPEND=$(DEPEND) V4LOW=$(V4LOW) EXEDIR=$(SNIFFDIR)\istudio slmin $(STDERROUT)
!endif
	@echo ----------------------------------


# ---------------------------------------------
# build dropcafe sniff
# ---------------------------------------------
dropcafe: set_env
!IF "$(DEBUG)"=="1"
	-$(COPYCMD)   $(CAFEDIR)\sniff\browser\WinDebug\browserd.pdb      $(SNIFFDIR)\browser
	-$(COPYCMD)   $(CAFEDIR)\sniff\data\WinDebug\datad.pdb            $(SNIFFDIR)\data
	-$(COPYCMD)   $(CAFEDIR)\sniff\debugger\WinDebug\debugd.pdb       $(SNIFFDIR)\debugger
#	-$(COPYCMD)   $(CAFEDIR)\sniff\editgo\WinDebug\editgod.pdb        $(SNIFFDIR)\editgo
	-$(COPYCMD)   $(CAFEDIR)\sniff\editor\WinDebug\editord.pdb        $(SNIFFDIR)\editor
	-$(COPYCMD)   $(CAFEDIR)\sniff\fortran\WinDebug\fortrand.pdb      $(SNIFFDIR)\fortran
		-$(COPYCMD)   $(CAFEDIR)\sniff\javasys\WinDebug\javasysd.pdb      $(SNIFFDIR)\javasys
        -$(COPYCMD)   $(CAFEDIR)\sniff\vjdebug\WinDebug\vjdebugd.pdb      $(SNIFFDIR)\vjdebug
        -$(COPYCMD)   $(CAFEDIR)\sniff\vjclsvw\WinDebug\vjclsvwd.pdb      $(SNIFFDIR)\vjclsvw
        -$(COPYCMD)   $(CAFEDIR)\sniff\vjbuild\WinDebug\vjbuildd.pdb      $(SNIFFDIR)\vjbuild
        -$(COPYCMD)   $(CAFEDIR)\sniff\vjexe\WinDebug\vjexed.pdb      $(SNIFFDIR)\vjexe
	-$(COPYCMD)   $(CAFEDIR)\sniff\istudio\WinDebug\isd.pdb           $(SNIFFDIR)\istudio
	-$(COPYCMD)   $(CAFEDIR)\sniff\sdi\WinDebug\sdid.pdb              $(SNIFFDIR)\sdi
	-$(COPYCMD)   $(CAFEDIR)\sniff\sys\WinDebug\sysd.pdb              $(SNIFFDIR)\sys
	-$(COPYCMD)   $(CAFEDIR)\sniff\vproj\WinDebug\vprojd.pdb          $(SNIFFDIR)\vproj
	-$(COPYCMD)   $(CAFEDIR)\sniff\vres\WinDebug\vresd.pdb            $(SNIFFDIR)\vres
	-$(COPYCMD)   $(CAFEDIR)\sniff\vshell\WinDebug\vshelld.pdb        $(SNIFFDIR)\vshell
	-$(COPYCMD)   $(CAFEDIR)\sniff\wizards\WinDebug\wizardsd.pdb      $(SNIFFDIR)\wizards
	-$(COPYCMD)   $(CAFEDIR)\sniff\dbg_mbc\WinDebug\dbg_mbcd.pdb      $(SNIFFDIR)\dbg_mbc
	-$(COPYCMD)   $(CAFEDIR)\x86\bin\logd.pdb                         $(SNIFFDIR)
	-$(COPYCMD)   $(CAFEDIR)\x86\bin\targetd.pdb                      $(SNIFFDIR)
	-$(COPYCMD)   $(CAFEDIR)\x86\bin\guid.pdb                         $(SNIFFDIR)
	-$(COPYCMD)   $(CAFEDIR)\x86\bin\shld.pdb                         $(SNIFFDIR)
	-$(COPYCMD)   $(CAFEDIR)\x86\bin\edsd.pdb                         $(SNIFFDIR)
	-$(COPYCMD)   $(CAFEDIR)\x86\bin\prjd.pdb                         $(SNIFFDIR)
	-$(COPYCMD)   $(CAFEDIR)\x86\bin\davd.pdb                         $(SNIFFDIR)
	-$(COPYCMD)   $(CAFEDIR)\x86\bin\dbgd.pdb                         $(SNIFFDIR)
	-$(COPYCMD)   $(CAFEDIR)\x86\bin\ided.pdb                         $(SNIFFDIR)
	-$(COPYCMD)   $(CAFEDIR)\x86\bin\bldtoold.pdb                     $(SNIFFDIR)
	-$(COPYCMD)   $(CAFEDIR)\x86\bin\toolsetd.pdb                     $(SNIFFDIR)
	-$(COPYCMD)   $(CAFEDIR)\x86\bin\cafedrvd.pdb                     $(SNIFFDIR)
	-$(COPYCMD)   $(CAFEDIR)\x86\bin\logd.dll                        $(SNIFFDIR)
	-$(COPYCMD)   $(CAFEDIR)\x86\bin\targetd.dll                     $(SNIFFDIR)
	-$(COPYCMD)   $(CAFEDIR)\x86\bin\guid.dll                        $(SNIFFDIR)
	-$(COPYCMD)   $(CAFEDIR)\x86\bin\shld.dll                        $(SNIFFDIR)
	-$(COPYCMD)   $(CAFEDIR)\x86\bin\edsd.dll                        $(SNIFFDIR)
	-$(COPYCMD)   $(CAFEDIR)\x86\bin\MSTEST40.dll                     $(SNIFFDIR)
	-$(COPYCMD)   $(CAFEDIR)\x86\bin\prjd.dll                        $(SNIFFDIR)
	-$(COPYCMD)   $(CAFEDIR)\x86\bin\davd.dll                        $(SNIFFDIR)
	-$(COPYCMD)   $(CAFEDIR)\x86\bin\dbgd.dll                        $(SNIFFDIR)
	-$(COPYCMD)   $(CAFEDIR)\x86\bin\ided.dll                        $(SNIFFDIR)
	-$(COPYCMD)   $(CAFEDIR)\x86\bin\bldtoold.dll                    $(SNIFFDIR)
	-$(COPYCMD)   $(CAFEDIR)\x86\bin\toolsetd.dll                    $(SNIFFDIR)
	-$(COPYCMD)   $(CAFEDIR)\mips\bin\MSTEST40.dll                    $(SNIFFDIR)
	-$(COPYCMD)   $(CAFEDIR)\x86\bin\cafedrvd.exe                    $(SNIFFDIR)
!ENDIF
!IF "$(DEBUG)"=="0"
	-$(COPYCMD)   $(CAFEDIR)\x86\bin\log.dll                         $(SNIFFDIR)
	-$(COPYCMD)   $(CAFEDIR)\x86\bin\target.dll                      $(SNIFFDIR)
	-$(COPYCMD)   $(CAFEDIR)\x86\bin\gui.dll                         $(SNIFFDIR)
	-$(COPYCMD)   $(CAFEDIR)\x86\bin\shl.dll                         $(SNIFFDIR)
	-$(COPYCMD)   $(CAFEDIR)\x86\bin\eds.dll                         $(SNIFFDIR)
	-$(COPYCMD)   $(CAFEDIR)\x86\bin\prj.dll                         $(SNIFFDIR)
	-$(COPYCMD)   $(CAFEDIR)\x86\bin\dav.dll                         $(SNIFFDIR)
	-$(COPYCMD)   $(CAFEDIR)\x86\bin\dbg.dll                         $(SNIFFDIR)
	-$(COPYCMD)   $(CAFEDIR)\x86\bin\ide.dll                         $(SNIFFDIR)
	-$(COPYCMD)   $(CAFEDIR)\x86\bin\bldtools.dll                    $(SNIFFDIR)
	-$(COPYCMD)   $(CAFEDIR)\x86\bin\toolset.dll                     $(SNIFFDIR)
	-$(COPYCMD)   $(CAFEDIR)\x86\bin\cafedrv.exe                     $(SNIFFDIR)
!ENDIF


# ---------------------------------------------
# set environment (INCLUDE and LIB)
# ---------------------------------------------
set_env:
	@set INCLUDE=$(CAFEDIR)\include;$(INCLUDE)
	@set LIB=$(CAFEDIR)\$(PLATFORM)\lib;$(LIB)
	@if not exist $(LOGDIR) md $(LOGDIR)

help:
!MESSAGE  Cafe Build System
!MESSAGE
!MESSAGE  Usage:
!MESSAGE
!MESSAGE  Core CAFE:
!MESSAGE
!MESSAGE  nmake /f cafebld.mak all          Perform all the following actions:
!MESSAGE  nmake /f cafebld.mak cafe         The CAFE system: log, target, gui, shl, eds, prj, dbg, ide, bldtools, toolset, subsuite, dav
!MESSAGE  nmake /f cafebld.mak log                      The log system
!MESSAGE  nmake /f cafebld.mak target           The target portion of support system
!MESSAGE  nmake /f cafebld.mak gui                      The gui portion of support system
!MESSAGE  nmake /f cafebld.mak shl                      The shl portion of support system
!MESSAGE  nmake /f cafebld.mak ueditor                  The ueditor portion of support system
!MESSAGE  nmake /f cafebld.mak ufindrep                 The ufindrep portion of support system
!MESSAGE  nmake /f cafebld.mak uiwrkspc                 The uiwrkspc portion of support system
!MESSAGE  nmake /f cafebld.mak cofile                   The cofile portion of support system
!MESSAGE  nmake /f cafebld.mak cosource                 The cosource portion of support system
!MESSAGE  nmake /f cafebld.mak eds                      The eds portion of support system
!MESSAGE  nmake /f cafebld.mak prj                      The prj portion of support system
!MESSAGE  nmake /f cafebld.mak dav                      The dav portion of support system
!MESSAGE  nmake /f cafebld.mak dbg                      The dbg portion of support system
!MESSAGE  nmake /f cafebld.mak ide                      The ide portion of support system
!MESSAGE  nmake /f cafebld.mak bldtools         The bldtools portion of the support system
!MESSAGE  nmake /f cafebld.mak toolset          The toolset portion of support system
!MESSAGE  nmake /f cafebld.mak subsuite     The subsuite library
!MESSAGE  nmake /f cafebld.mak cafedrv      The CAFE driver
!MESSAGE  nmake /f cafebld.mak ideheaders   Munge headers from IDE project
!MESSAGE
!MESSAGE  Sniff tests:
!MESSAGE
!MESSAGE  nmake /f cafebld.mak sniffs       Build all the sniff tests
!MESSAGE  nmake /f cafebld.mak browser          Build browser sniff test
!MESSAGE  nmake /f cafebld.mak data         Build data sniff test
!MESSAGE  nmake /f cafebld.mak dbg_mbc      Build dbg_mbc sniff test
!MESSAGE  nmake /f cafebld.mak debugger     Build debugger sniff test
#!MESSAGE  nmake /f cafebld.mak editgo           Build edit 'n go sniff test
!MESSAGE  nmake /f cafebld.mak editor           Build editor sniff test
!MESSAGE  nmake /f cafebld.mak javasys      Build javasys sniff test
!MESSAGE  nmake /f cafebld.mak vjdebug      Build vjdebug sniff test
!MESSAGE  nmake /f cafebld.mak vjclsvw      Build vjclsvw sniff test
!MESSAGE  nmake /f cafebld.mak vjbuild      Build vjbuild sniff test
!MESSAGE  nmake /f cafebld.mak vjexe        Build vjexe sniff test
!MESSAGE  nmake /f cafebld.mak sdi          Build sdi sniff test
!MESSAGE  nmake /f cafebld.mak sys          Build sys sniff test
!MESSAGE  nmake /f cafebld.mak vproj        Build vproj sniff test
!MESSAGE  nmake /f cafebld.mak vres         Build vres sniff test
!MESSAGE  nmake /f cafebld.mak vshell       Build vshell sniff test
!MESSAGE  nmake /f cafebld.mak wizards      Build wizards sniff test
!MESSAGE  nmake /f cafebld.mak vjdebug      Build vjdebug sniff test
!MESSAGE  nmake /f cafebld.mak istudio      Build istudio sniff test
!MESSAGE
!MESSAGE
!MESSAGE  Additional command line defines: * indicates default
!MESSAGE
!MESSAGE  BUILD=        [ 0 | 1* ]                              Build the component(s)
!MESSAGE  DEBUG=        [ 0* | 1 | 2 ]                  Build debug (0), retail (1), or retail with debug info (2)
!MESSAGE  CLEAN=        [ 0* | 1 ]                              Clean the project before building
!MESSAGE  SSYNC=        [ 0* | 1 ]                              Ssync the directory(ies) before building
!MESSAGE  SLMOUT=       [ 0* | 1 ]                              Check the writable files for the given component(s) out
!MESSAGE  SLMIN=        [ 0* | 1 ]                              Check the writable files for the given component(s) in
!MESSAGE  BROWSE=   [ 0 | 1* ]                          Build browser info (1) or don't (0)
!MESSAGE  DEPEND=   [ 0 | 1* ]                          Build dependencies (1) or don't (0)
!MESSAGE  PLATFORM= [ X86*, MIPS, ALPHA ]       Specify the platform
!MESSAGE  AUTODEP=  [ 0* | 1 ]                          Generate dependency file
!MESSAGE  VERB=         [ 0* | 1 | 2 ]                  Verbose output: 0 = output to file only; 1 = output to file and screen; 2 = output to screen
!MESSAGE  V4LOW         [ 0* | 1 ]                              Build conditionally for V4 Low (VC++ Standard Edition)
!MESSAGE
