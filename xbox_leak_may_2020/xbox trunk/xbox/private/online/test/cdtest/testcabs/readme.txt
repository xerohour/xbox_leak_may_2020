These are some test cabs that are encrypted and signed by the keys in ..\keydata. The temporary extension I chose for Xbox CABs is .xcp

There is a set of good CABs that should be installable by XOnlineContentInstall using the public and symmetric keys (..\keydata\pubkey.c and ..\keydata\symmkey.c).

test1.xcp - 2 folders, flat hierarchy
test2.xcp - 2 folders, installs into a directory hierarchy
test3.xcp - 4 folders, large CFFILE structure, installs into a deep hierarchy structure.

There is another set of CABs with manually injected corrupt bits. The details of the corruption and the intended test is documented in the corrupt.txt file. These should all fail XOnlineTaskContinue with HRESULT_FROM_WIN32(ERROR_INVALID_DATA).

All of these files are based on test1.xcp. Please see corrupt.txt for details.

