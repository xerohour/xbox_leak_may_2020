@REM The following script enable building binaires for Sundown.
@REM It does not set the _NTxxxxTREE variables so the binaires are not
@REM binplaced.
@
@set MSTOOLS_WIN64=%SystemRoot%\mstools\win64
@if NOT "%MSTOOLS_DIR%" == "" set MSTOOLS_WIN64=%MSTOOLS_DIR%\win64
@if  EXIST %MSTOOLS_WIN64% goto begin
@echo Cannot find mstools\win64 directory. Set MSTOOLS_DIR.
@goto done
@:begin
@set HOST_TOOLS="PATH=%PATH%"
@set PATH=%MSTOOLS_WIN64%;%PATH%
@if "%PROCESSOR_ARCHITECTURE%" == "ALPHA" goto alpha
@
@set BUILD_DEFAULT_TARGETS=-ia64
@set HOST_TARGETCPU=i386
@set HOST_TARGET_DIRECTORY=ia64
@set HOST_TARGET_DEFINES=IA64=1 i386=0 GENIA64=1
@set IA64=1
@set NTIA64DEFAULT=1
@goto done
@
@:alpha
@
@set BUILD_DEFAULT_TARGETS=-axp64
@set HOST_TARGETCPU=alpha
@set HOST_TARGET_DIRECTORY=axp64
@set HOST_TARGET_DEFINES=AXP64=1 ALPHA=0 GENAXP64=1
@set NTAXP64DEFAULT=1
@set NTALPHADEFAULT=
@set AXP64=1
@set NTDEBUG=ntsd
@set NTDEBUGTYPE=windbg
@
@:done
