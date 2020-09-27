What's in this directory:

- dxt is the VC debug module, built as a debugger extension.  It gets put into
  ntbins\devkit\msvc.dll and lives on the Xbox as msvc.dll
- win32 is the same VC debug module, built to live on the host and use the
  network protocol to talk to the Xbox.  It gets built into
  ntbins\mstools\tlxbox.dll and lives in your MSVC install tree
- wintl is the host-side transport layer that talks to the dxt.  It gets put
  into ntbins\devkit\tlxbox.dll and lives in your MSVC install tree.

Most of the time you'll want to build win32 only and use that version of
tlxbox.dll.  If you want to test out debugger extensibility, you build dxt and
wintl and use that version of tlxbox.dll instead of the default one, and copy
msvc.dll over to the Xbox alongside xbdm.dll
