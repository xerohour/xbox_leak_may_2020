copy %1.enc in.bin > nul
del out.bin
..\dostest.com
echo Comparing decrypted %1.dec to %1.org
copy out.bin %1.dec > nul
fc %1.dec %1.org

