xbcp /r /y recmedia xe:\samples
cd ..\..
copy recoverhd\recoverhd\obj\i386\recoverhd.exe .
recoverhd\makerecimg\obj\i386\makerecimg rr_recovery\cdfiles
xbcp /y RecoverHD.xbe xe:\samples
del recoverhd.exe
cd recoverhd\recoverhd