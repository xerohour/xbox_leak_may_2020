
build -cz

xgpimage fximg.ini %_NT386TREE%\fximg.bin fximg.h
xbcp -f %_NT386TREE%\fximg.bin xe:\samples\voicedsp\media

xbcp -f %_NT386TREE%\dump\voicedsp.xbe xe:\samples\voicedsp
xbreboot -c xe:\samples\voicedsp\voicedsp.xbe
