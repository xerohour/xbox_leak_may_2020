$(O)\data1.cab: $(TARGET)
	%_NTBINDIR%\private\xdktools\Producer\InstallShield\Program\Compile.exe -I"%_NTBINDIR%\private\xdktools\Producer\InstallShield\Include" "%_NTBINDIR%\private\xdktools\Producer\InstallShield\DirectMusic Producer - Xbox\Script Files\setup.rul"
	%_NTBINDIR%\private\xdktools\Producer\InstallShield\Program\ISbuild -p"%_NTBINDIR%\private\xdktools\Producer\InstallShield\DirectMusic Producer - Xbox" -m"Default" -b"%_NTBINDIR%\private\xdktools\Producer\SDKSetup\$(O)"
	xcopy "%_NTBINDIR%\private\xdktools\Producer\SDKSetup\$(O)\Disk Images\disk1\*.*" "%_NTBINDIR%\private\xdktools\Producer\SDKSetup\$(O)\"
	delnode /q "$(O)\Disk Images"
	del /q /f "$(O)\dummy.*"
	del /q /f "$(O)\main.obj"



