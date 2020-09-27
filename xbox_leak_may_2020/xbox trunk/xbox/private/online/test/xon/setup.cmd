@echo off
if not exist C:\WINNT\system32\drivers\vlan.sys goto NOVLAN
if not exist obj\i386\testxon.exe goto NOTESTXON

echo Note: The first time this setup runs, it may fail.  This is expected.

obj\i386\testxon.exe account_create
copy xonline.ini c:\$xbox\%COMPUTERNAME%_1\partition1\devkit
copy xonline.ini c:\$xbox\%COMPUTERNAME%_2\partition2\devkit

echo ---------------------
echo You can now start this test suite any time by running obj\i386\testxon.exe

goto DONE

:NOVLAN
echo You must install vlan.sys.  Instructions:
echo 1. Sync and build the //depot/xonline tree.
echo 2. Sync the //depot/xbox tree.
echo 3. Copy drop\debug\sg\sgadmin.exe from the //depot/xonline tree into the
echo    private\lib\i386 directory in the //depot/xbox tree.
echo 4. Run: sgadmin install ms_vlan netvlan_m.inf
echo 5. Rerun this setup
goto DONE

:NOTESTXON
echo You haven't built testxon yet.  Please sync and build the XOnline client
echo and run this again.
goto DONE

:DONE
