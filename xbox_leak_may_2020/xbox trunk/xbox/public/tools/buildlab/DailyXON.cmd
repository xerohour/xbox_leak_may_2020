@echo on
echo Build Xonline Tree %TIME% > %XBLD_LOG%
if "%1"=="-nosync" goto build

cd /d %_NTBINDIR%\
rd /s /q drop
call clean
call sd sync >> %XBLD_LOG%

:setversion
echo :setversion %XBLABTYPE% start %TIME%  >> %XBLD_LOG%
rem find verion number in private\inc\xboxverp.h and increment by one
for /f "tokens=5" %%s in ('grep "#define VER_PRODUCTBUILD " %_NTROOT%\private\inc\xboxverp.h') do set oldver=%%s
set /a newver=%oldver%+1

rem set new version number into current environment
set _BUILDVER=%newver%
set SDFORMEDITOR=sdforms.exe -c "VER_PRODUCTBUILD <- %newver%"

cd %_NTROOT%\private\inc
call sd edit xboxverp.h
type xboxverp.h | perl -p -e "s/VER_PRODUCTBUILD\s+%oldver%/VER_PRODUCTBUILD            %newver%/;s/_XTL_VER\s+%oldver%/_XTL_VER                   %newver%/" > tmpf
move /y tmpf xboxverp.h
call sd submit xboxverp.h

:build
cd /d %_NTBINDIR%\private
call bldnt clean all
cd ..
call robocopy drop %XBLD_REL%\%_BUILDVER%\xonline /MIR /NP >> %XBLD_LOG%
xcopy private\build* %XBLD_REL%\%_BUILDVER%\xonline

if "%_XOB%"=="" (echo %_BUILDVER% > %XBLD_REL%\latest_xon.txt
)else echo %_BUILDVER% > %XBLD_REL%\branch_xon.txt

:notify
cd /d %_NTBINDIR%\private
if exist build.err (
    call sendfile xonchkin -f "Default Outlook Profile" -s "Build ERRORS (%_BUILDVER% xonline)"  -t build.err -a %XBLD_LOG%
    ) else call sendfile xonchkin -f "Default Outlook Profile" -s "No Build Errors (%_BUILDVER% xonline)" -a %XBLD_LOG%
:end
