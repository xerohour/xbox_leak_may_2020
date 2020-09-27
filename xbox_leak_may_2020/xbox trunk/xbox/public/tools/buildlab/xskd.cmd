if "%1"=="" goto usage
if "%2"=="" goto usage
if "%3"=="" goto usage
if "%4"=="" goto usage
if "%5"=="" goto usage

set XboxName=%1
set ComPort=%2
set Source=%3
set BuildVer=%4
set K_Htype=%5

if /i "%BuildVer%"=="latest" for /f %%a in (%Source%\latest.txt) do set BuildVer=%%a

if /i "%K_Htype%"=="random" (
    set KernelType=checked
    set HarnessType=checked
)
if /i "%K_Htype%"=="chk_chk" (
    set KernelType=checked
    set HarnessType=checked
)
if /i "%K_Htype%"=="chk_fre" (
    set KernelType=checked
    set HarnessType=free
)
if /i "%K_Htype%"=="fre_fre" (
    set KernelType=free
    set HarnessType=free
)
if /i "%K_Htype%"=="fre_chk" (
    set KernelType=free
    set HarnessType=checked
)

set LocalTools=%TEMP%\xstress
set IDWTools=%Source%\%BuildVer%\%HarnessType%\public\idw
set MSTools=%Source%\%BuildVer%\%HarnessType%\public\mstools

rem  copy tools

if not exist %LocalTools% mkdir %LocalTools%
cd /d %LocalTools%
copy /y \\xstress\stress\MSVCRTD.dll    %LocalTools%
copy /y %MSTools%\dbgeng.dll            %LocalTools%
copy /y %MSTools%\dbghelp.dll           %LocalTools%
copy /y %MSTools%\i386kd.exe            %LocalTools%
copy /y %MSTools%\kdextx86.dll          %LocalTools%
copy /y %IDWTools%\remote.exe           %LocalTools%
copy /y %IDWTools%\xboxdbg.dll          %LocalTools%
copy /y %IDWTools%\xbcp.exe             %LocalTools%
copy /y %IDWTools%\xbmkdir.exe          %LocalTools%
copy /y %IDWTools%\xbdel.exe            %LocalTools%
copy /y %IDWTools%\xbreboot.exe         %LocalTools%

set REMOTE=start %LocalTools%\remote.exe /s
set KD=%LocalTools%\i386kd.exe
set KTREE=%Source%\%BuildVer%\%KernelType%
set HTREE=%Source%\%BuildVer%\%HarnessType%

set _NT_SYMBOL_PATH=%HTREE%\xboxtest;%KTREE%;%KTREE%\devkit;%KTREE%\dump
set _NT_DEBUGGER_EXTENSION_PATH=%MSTools"
set _NT_DEBUG_PORT=%ComPort%
set _NT_DEBUG_BAUD_RATE=115200

%REMOTE% "%KD%" %XBOXNAME%

goto end

:usage
echo xskd XboxName ComPort Source_Dir BuildVer Kernel_HarnessType

:end
