

REM ---------- Sync test files------------------------------------
pushd .

cd /d %_NTDRIVE%%_NTROOT%\private\test\buildxbe\dmharn
sd sync ...
cd /d %_NTDRIVE%%_NTROOT%\private\test\multimedia\dmusic\dmth8
sd sync ...
cd /d %_NTDRIVE%%_NTROOT%\private\test\multimedia\dmusic\dmtest1
sd sync ...

popd

