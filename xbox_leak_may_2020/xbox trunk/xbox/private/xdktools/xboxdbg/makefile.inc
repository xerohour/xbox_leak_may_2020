$(XDK_LIB_PATH)\$(TARGETNAME).Lib: $(SDK_LIB_PATH)\$(TARGETNAME).Lib
    Copy $? $@

$O\fileapi.obj: fileapi.c filefunc.c

$O\xbconn.obj: xbconn.cpp filefunc.c
