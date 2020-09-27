xbcp -f %_NT386TREE%\dump\devtest.xbe xe:\devtest
%_NT386TREE%\mstools\filegen media\soundbank1.ini media\soundbank1.bin
xbcp -f media\*.bin xe:\devtest\media
xbcp -f %_NT386TREE%\dsstdfx.bin xe:\devtest\media
xbreboot -c xe:\devtest\devtest.xbe


