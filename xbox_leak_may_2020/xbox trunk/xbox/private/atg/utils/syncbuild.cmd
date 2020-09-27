REM
REM Does a sync and clean build in the current enlistment.
REM Outputs to %_NTDRIVE%%_NTROOT%\syncbuild.log
REM

REM Set up to run
cd /d %_NTDRIVE%%_NTROOT%
Date /T > syncbuild.log
Time /T >> syncbuild.log
Echo "Sync-ing enlistment..." >> syncbuild.log

REM Sync
sd sync ...
Echo "Sync done - performing clean build..." >> syncbuild.log
echo "NTDEBUG=%NTDEBUG%" >> syncbuild.log

REM Build
cd private
build -cZ

REM Finish logging
cd %_NTDRIVE%%_NTROOT%
echo "SyncBuild completed" >> syncbuild.log
Date /T >> syncbuild.log
Time /T >> syncbuild.log
