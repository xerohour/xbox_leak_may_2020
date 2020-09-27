xbmkdir xc:\tdata\a7049955
xbmkdir xc:\tdata\a7049955\media
xbmkdir xc:\tdata\a7049955\media\audio
xbmkdir xc:\tdata\a7049955\media\audio\pcm
xbmkdir xc:\tdata\a7049955\media\audio\wma

xbcp -f -r %_NTBINDIR%\private\test\multimedia\dsound\media\*.* xc:\tdata\a7049955\media
xbcp -f -r %_NTBINDIR%\private\test\buildlab\ini\bvt\dsound.ini xc:\tdata\a7049955\testini.ini
xbcp -f -r %_NT386TREE%\dump\harness.xbe xe:\

xbreboot xe:\harness.xbe