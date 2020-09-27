:::
::: STRESS BATCH FILE
:::

:::
::: Turn off echo, clear screen
:::

	@echo off
	cls

:::
::: check to see if xbox name was passed in
:::

	if "%1"=="" goto USAGE
	set XBOXNAME=%1

:::
::: check to see if video card type was passed in
:::

	if "%2"=="" goto USAGE
	if "%2"=="Celcius" set CELCIUS=Celcius
	if "%2"=="Kelvin" set CELCIUS=

:::
::: check to see if External / Internal was passed in
:::

	if "%3"=="" goto USAGE
	if "%3"=="Internal" set INTERNAL=Internal
	if "%3"=="External" set INTERNAL=

:::
::: check to see if com port was passed in
:::
	
	if "%4"=="" goto USAGE
	set _NT_DEBUG_PORT=%4

:::
::: set baud rate, comport and kelvin build
:::

	set _NT_DEBUG_BAUD_RATE=115200

:::
::: Setup paths
:::

	set STRESSROOT=\\danrose\stress

	set XBCP=%STRESSROOT%\xbcp.exe /x %XBOXNAME%
	set XBMKDIR=%STRESSROOT%\xbmkdir.exe /x %XBOXNAME%
	set XBDEL=%STRESSROOT%\xbdel.exe /x %XBOXNAME%
        set XBREBOOT=%STRESSROOT%\xbreboot.exe /x %XBOXNAME% /c
	set REMOTE=start %STRESSROOT%\remote.exe /s
        set KD=%STRESSROOT%\i386kd.exe
	set RANDFILE=%STRESSROOT%\randfile.exe

	if "%CELCIUS%"=="Celcius" set TESTINI=%STRESSROOT%\celcius.ini
	if "%CELCIUS%"=="" set TESTINI=%STRESSROOT%\kelvin.ini
	set LAUNCH=%STRESSROOT%\launch.xbx
	set USERS=%STRESSROOT%\users.txt

	set XBUILDS=\\xbuilds\release\usa
	set LATESTBNUM=%XBUILDS%\latest.txt
	set KELVIN_CHECKED=%CELCIUS%\checked

	set MEDIADIR=\\danrose\content\xbox_media
	set MEDIAFILES=50

	if "%INTERNAL%"=="Internal" set TDATA=xc:\TDATA\a7049955
	if "%INTERNAL%"=="" set TDATA=xs:\a7049955

	set X_TESTINI=%TDATA%\testini.ini

	set TDATA_MEDIA=%TDATA%\media
	set TDATA_MEDIA_AUDIO=%TDATA_MEDIA%\audio
	set TDATA_MEDIA_AUDIO_PCM=%TDATA_MEDIA_AUDIO%\pcm
	set TDATA_MEDIA_AUDIO_PCM_FILES=%TDATA_MEDIA_AUDIO_PCM%\*.*

	set X_HARNESS=xe:\stress.xbe

	set XBOX=xc:\xbox
	set KERNEL_FILES=%XBOX%\*.*

	set X_YDRIVE=xe:\dxt\cydrive.dxt
	set X_LAUNCH=xc:\launch.xbx

:::
::: get the build number
:::

	for /f %%a in ('type %LATESTBNUM%') do set BNUM=%%a

:::
::: set the root of the source tree based on kelvin or celcius
::

	set BTREE=%XBUILDS%\%BNUM%%KELVIN_CHECKED%
	set BTREE_DUMP=%BTREE%\dump
	set HARNESS=%BTREE_DUMP%\harness.xbe
	set CYDRIVE=%BTREE%\devkit\cydrive.exe

:::
::: set the symbol path up
:::

	set _NT_SYMBOL_PATH=%BTREE%\Symbols\xboxtest;%BTREE%\Symbols\devkit;%BTREE%\Symbols\dump;%BTREE%\Symbols\retail

:::
::: setup xbox with devkit
:::

	%XBCP%

:::
::: Make the harness tree
:::

	%XBMKDIR% %TDATA%
	%XBMKDIR% %TDATA_MEDIA%
	%XBMKDIR% %TDATA_MEDIA_AUDIO%
	%XBMKDIR% %TDATA_MEDIA_AUDIO_PCM%

:::
::: remove the harness and the testini
:::

	%XBDEL% %X_TESTINI%
	%XBDEL% %X_HARNESS%

:::
::: copy over the new ini and new harness
:::

	%XBCP% %TESTINI% %X_TESTINI%
	%XBCP% %HARNESS% %X_HARNESS%

:::
::: copy over random media files
:::

	%XBDEL% %TDATA_MEDIA_AUDIO_PCM_FILES%

	echo Copying over media files, this may take up to 10 minutes
	for /f %%f in ('%RANDFILE% %MEDIADIR% %MEDIAFILES%') do %XBCP% /y %%f %TDATA_MEDIA_AUDIO_PCM%
	echo Done copying media files

:::
::: update the kernel, the dashboard, reboot, and inform the user what to do
:::

	if "%INTERNAL%"=="" goto AFTERSYSTEMCOPY

	%XBDEL% %KERNEL_FILES%
	%XBCP% %BTREE%\xboxkrnl.exe %XBOX%
	%XBCP% %BTREE%\boot\xboxldr.com %XBOX%
	%XBCP% %BTREE%\boot\xboxbldr.bin %XBOX%
	%XBCP% %BTREE%\devkit\xbdm.dll %XBOX%

:::
::: check for kelvin, copy video drivers
:::

	if "" == "%CELCIUS%" goto AFTERNV15DRIVERS
	%XBCP% -q -y %BTREE%\nv4.sys xc:\xbox
	%XBCP% -q -y %BTREE%\videoprt.sys xc:\xbox

:AFTERNV15DRIVERS

:::
::: copy cydrive access
:::

	%XBDEL% %X_YDRIVE%
	%XBCP% %CYDRIVE% %X_YDRIVE%

:::
::: copy launch.xbx
:::

	%XBDEL% %X_LAUNCH%
	%XBCP% %LAUNCH% %X_LAUNCH%

:AFTERSYSTEMCOPY

:::
::: start the remote debugger
:::

	%REMOTE% "%KD%" %XBOXNAME%

:::
::: reboot the xbox
:::

	%XBREBOOT%

:::
::: add the user to the database
:::

	for /f "delims=" %%d in ('date /t') do set DATE=%%d
	for /f "delims=" %%t in ('time /t') do set TIME=%%t
	echo %USERNAME% %COMPUTERNAME% %XBOXNAME% %DATE% %TIME% %BNUM% %CELCIUS% >> %USERS%

::: 
::: Clean up
:::

	cls
	goto END

:::
::: prompt the user they screwed up
:::

:USAGE
	echo USAGE:   stress XBOXNAME CARDTYPE DISCTYPE COMPORT
	echo EXAMPLE: stress danrosexbox Kelvin Internal com1

:END