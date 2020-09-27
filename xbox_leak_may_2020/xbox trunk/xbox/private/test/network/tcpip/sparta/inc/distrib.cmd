@echo Distribute... %1
pause
pushd D:\sdnt\net\testsrc\component\transports\src\util\TCPIP\sparta\distribution\inc
sd edit %1
copy ..\..\apiinc\%1 .
sd submit %1
popd