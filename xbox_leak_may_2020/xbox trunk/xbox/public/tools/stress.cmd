:::
::: STRESS BATCH FILE
:::

:::
::: Turn off echo, clear screen
:::

	@echo off
	cls

:::
::: check to see if debugger info is set
:::

	if "%_NT_DEBUG_BAUD_RATE%"=="" goto USAGE
	if "%_NT_DEBUG_PORT%"=="" goto USAGE
	if "%_NTROOT%"=="" goto USAGE
	if "%_NTBINDIR%"=="" goto USAGE

:::
::: make sure xboxname was supplied
:::

	if "%1"=="" goto USAGE
	xbcp -x %1

:::
::: get the build number
:::

	for /f %%a in ('type \\xbuilds\release\usa\latest.txt') do set BNUM=%%a

:::
::: set the root of the source tree based on kelvin or free
:::

	if NOT "%KELVIN%"=="" goto KELVIN
	set BTREE=\\xbuilds\release\usa\%BNUM%\checked
	goto AFTERKELVIN

:KELVIN

	set BTREE=\\xbuilds\release\usa\%BNUM%Kelvin\checked

:AFTERKELVIN

:::
::: set the symbol path up
:::

	set _NT_SYMBOL_PATH=%BTREE%\Symbols\xboxtest\exe;%BTREE%\Symbols\devkit\exe;%BTREE%\Symbols\dump\exe;%BTREE%\Symbols\retail\exe

:::
::: Make the harness tree
:::

	xbmkdir xc:\tdata\a7049955
	xbmkdir xc:\tdata\a7049955\media
	xbmkdir xc:\tdata\a7049955\media\audio
	xbmkdir xc:\tdata\a7049955\media\audio\pcm

:::
::: remove the harness and the testinin
:::

	xbdel /q xc:\tdata\a7049955\testini.ini
	xbdel /q xe:\stress.xbe

:::
::: copy over the new ini and new harness
:::

	xbcp /q /y %_NTROOT%\private\test\buildlab\ini\stress\testini.ini xc:\tdata\a7049955\testini.ini
	xbcp /q /y %BTREE%\dump\harness.xbe xe:\stress.xbe

:::
::: copy over random media files
:::

	echo on
	echo Copying over media files, this may take up to 10 minutes
	for /f %%f in ('randfile.exe \\danrose\content\xbox_media 50') do xbcp /q /y %%f xc:\tdata\a7049955\media\audio\pcm
	echo Done copying media files
	echo off

:::
::: update the kernel, the dashboard, reboot, and inform the user what to do
:::

	xbdel -q -r xc:\xbox\*.*
	if not "0" == "%errorlevel%" goto end
	xbcp -q -y %BTREE%\xboxkrnl.exe xc:\xbox
	xbcp -q -y %BTREE%\boot\xboxldr.com xc:\xbox
	xbcp -q -y %BTREE%\boot\xboxbldr.bin xc:\xbox
	xbcp -q -y %BTREE%\devkit\xbdm.dll xc:\xbox

:::
::: check for kelvin, copy video drivers
:::

	if not "" == "%KELVIN%" goto AFTERNV15DRIVERS
	xbcp -q -y %BTREE%\nv4.sys xc:\xbox
	xbcp -q -y %BTREE%\videoprt.sys xc:\xbox

:AFTERNV15DRIVERS

:::
::: copy cydrive access
:::

	xbcp -q -y %BTREE%\devkit\cydrive.exe xe:\dxt\cydrive.dxt

:::
::: update default title
:::

	xbdel -q xy:\*.*
	xbcp -q -y %BTREE%\dump\xshell.xbe xy:\default.xbe
	xbcp -q -y -r %_NTBINDIR%\private\test\ui\xshell\tdata\ xy:\

:::
::: copy launch.xbx
:::

	xbcp -q -y %_NTBINDIR%\private\test\buildlab\launch.xbx xc:\launch.xbx

:::
::: start the remote debugger
:::

	start remote /s "i386kd" %_NTUSER%

:::
::: reboot the xbox
:::

	xbreboot

::: 
::: prompt the user
:::

	cls
	goto END

:::
::: prompt the user they screwed up
:::

:USAGE
	echo USAGE: stress XBOXNAME
	echo You must have _NTROOT, _NTBINDIR, _NT_DEBUG_BAUD_RATE and _NT_DEBUG_PORT set

:END