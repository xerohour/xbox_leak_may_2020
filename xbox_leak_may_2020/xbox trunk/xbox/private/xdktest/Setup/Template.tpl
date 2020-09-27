'$include 'install.inc'


Scenario "SDK Clean Full Setup - Cancel at Welcome Screen, using buttons"
	Dim xdkpath$

	if IsInstalled then
		xdkpath = GetXDKPath()
		Uninstall
		if exists(xdkpath, "+d") then killdir(xdkpath)
	end if

	Install(BuildNumber, false, false, 2)

Scenario cleanup
	if IsInstalled then
		xdkpath = GetXDKPath()
		Uninstall
		if exists(xdkpath, "+d") then killdir(xdkpath)
	else
		Sleep 5
	end if
End Scenario

' *************************
' ***** END TEST CASE *****
' *************************
End
 