@rem This batch file can be run in one of the following fashions.
@rem 1) build spotless          removes all build targets (like nmake clean)
@rem 2) build debug -a          rebuilds the entire driver for debug
@rem 3) build retail -a         rebuilds the entire driver for retail
@rem 4) build log -a            rebuilds the entire driver for debug with
@rem                                    function logging on by default.
@rem 5) build oemdebug -a       rebuilds the entire driver for debug
@rem                                    with symbols but no source
@rem
@rem If you leave off the -a argument in example 2 and 3 above, then only
@rem those files which have changed will get rebuilt. Also, the command
@rem line options are CASE-SENSITIVE.  Sorry, that's DOS.

@rem The environment variable ENVPATH is used by mkdepend as the directory
@rem containing .env files. A .env file contains any build flags, build
@rem equates, etc.
SET ENVPATH=%BUILD_TOOLS_DIR%\make

@rem The environment variables BIN is used by mkdepend as the location of
@rem the directory containing mkdepend itself! This is so mkdepend can call
@rem itself recursively.
set TEMPBIN=%BIN%
SET BIN=%BUILD_TOOLS_DIR%\make

@rem We also need to have nmake in the PATH -- a mkdepend requirement.
@rem It may already be in the path, but in case it isn't, let's stick
@rem the msvc50 bin directory in the path because a reasonable version
@rem of nmake is in there.
SET TEMPPATH=%PATH%
SET PATH=%PATH%;%BUILD_TOOLS_DIR%\msvc50\bin

@rem Save the current debug flag setting and clear DEBUG
SET TEMPDEBUG=%DEBUG%
SET DEBUG=
SET EXCLUDE_SOURCE=

@rem First, run mkdepend in the root directory
%BIN%\mkdepend

@rem Next run "nmake -a mkdepend" to walk the tree building all the makefiles.
nmake -a mkdepend

@rem Pass the command line argument to this batch file off to
@if "%1" == "debug"      nmake "DEBUG=1" "DEF_CODE_SIZE=USE32" %2
@if "%1" == "log"        nmake "DEBUG=2" "DEF_CODE_SIZE=USE32" %2
@if "%1" == "oemdebug"   nmake "DEBUG=3" "EXCLUDE_SOURCE=1" "DEF_CODE_SIZE=USE32" %2
@if "%1" == "retail"     nmake "DEF_CODE_SIZE=USE32" %2
@if "%1" == "debug16"    nmake "DEBUG=1" "DEF_CODE_SIZE=USE16" "USE_EBP=1" %2
@if "%1" == "log16"      nmake "DEBUG=2" "DEF_CODE_SIZE=USE16" "USE_EBP=1" %2
@if "%1" == "oemdebug16" nmake "DEBUG=3" "EXCLUDE_SOURCE=1" "DEF_CODE_SIZE=USE16" %2
@if "%1" == "retail16"   nmake "DEF_CODE_SIZE=USE16" "USE_EBP=1" %2
@if "%1" == "debug32"    nmake "DEBUG=1" "DEF_CODE_SIZE=USE32" %2
@if "%1" == "log32"      nmake "DEBUG=2" "DEF_CODE_SIZE=USE32" %2
@if "%1" == "oemdebug32" nmake "DEBUG=3" "EXCLUDE_SOURCE=1" "DEF_CODE_SIZE=USE32" %2
@if "%1" == "retail32"   nmake "DEF_CODE_SIZE=USE32" %2
@if "%1" == "spotless"   nmake spotless

@rem Restore the saved environment variables
@SET PATH=%TEMPPATH%
@SET TEMPPATH=
@SET BIN=%TEMPBIN%
@SET TEMPBIN=
@SET DEBUG=%TEMPDEBUG%
@SET TEMPDEBUG=
@SET EXCLUDE_SOURCE=

