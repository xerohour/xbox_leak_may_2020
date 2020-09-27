TARGETPATH=obj
SOURCES =

LINKER_FLAGS=-ignore:4006
LIBRARIAN_FLAGS=-ignore:4006

!if $(FREEBUILD)
D=
!else
D=d
!endif

!IF "$(CRTLIBTYPE)" == "IMT"

LIBOUTPUT=$(XDK_LIB_PATH)
TARGETNAME=libcimt$D
TARGETTYPE=LIBRARY

OBJLIBFILES=\
    ..\..\..\crtw32\iostream\imt\$O\iostream.lib

!ELSEIF "$(CRTLIBTYPE)" == "IST"

LIBOUTPUT=$(XDK_LIB_PATH)
TARGETNAME=libci$D
TARGETTYPE=LIBRARY

OBJLIBFILES=\
    ..\..\..\crtw32\iostream\ist\$O\iostream.lib

!ELSEIF "$(CRTLIBTYPE)" == "PMT"

LIBOUTPUT=$(XDK_LIB_PATH)
TARGETNAME=libcpmt$D
TARGETTYPE=LIBRARY

OBJLIBFILES=\
    ..\..\..\crtw32\stdcpp\pmt\$O\stdcpp.lib

!ELSEIF "$(CRTLIBTYPE)" == "PST"

LIBOUTPUT=$(XDK_LIB_PATH)
TARGETNAME=libcp$D
TARGETTYPE=LIBRARY

OBJLIBFILES=\
    ..\..\..\crtw32\stdcpp\pst\$O\stdcpp.lib

!ELSEIF "$(CRTLIBTYPE)" == "MT"

LIBOUTPUT=$(XDK_LIB_PATH)
TARGETNAME=libcmt$D
TARGETTYPE=LIBRARY

OBJLIBFILES = \
    ..\..\*\almap.lib \
    ..\..\..\crtw32\awxapi\mt\$O\awxapi.lib \
    ..\..\..\crtw32\convert\mt\$O\convert.lib \
    ..\..\..\crtw32\direct\mt\$O\direct.lib \
    ..\..\..\crtw32\dos\mt\$O\dos.lib \
    ..\..\..\crtw32\eh\mt\$O\eh.lib \
    ..\..\..\crtw32\exec\mt\$O\exec.lib \
    ..\..\..\crtw32\heap\mt\$O\heap.lib \
    ..\..\..\crtw32\helper\mt\$O\helper.lib \
    ..\..\..\crtw32\lowio\mt\$O\lowio.lib \
    ..\..\..\crtw32\mbstring\mt\$O\mbstring.lib \
    ..\..\..\crtw32\misc\mt\$O\misc.lib \
    ..\..\..\crtw32\startup\mt\$O\startup.lib \
    ..\..\..\crtw32\stdio\mt\$O\stdio.lib \
    ..\..\..\crtw32\string\mt\$O\string.lib \
    ..\..\..\crtw32\time\mt\$O\time.lib \
    ..\..\..\fpw32\conv\mt\$O\conv.lib \
    ..\..\..\fpw32\tran\mt\$O\tran.lib

!if !$(FREEBUILD)
OBJLIBFILES=$(OBJLIBFILES) \
    ..\..\..\crtw32\rtc\mt\$O\rtc.lib
!endif

!ELSEIF "$(CRTLIBTYPE)" == "ST"

LIBOUTPUT=$(XDK_LIB_PATH)
TARGETNAME=libc$D
TARGETTYPE=LIBRARY

OBJLIBFILES = \
    ..\..\*\almap.lib \
    ..\..\..\crtw32\awxapi\st\$O\awxapi.lib \
    ..\..\..\crtw32\convert\st\$O\convert.lib \
    ..\..\..\crtw32\direct\st\$O\direct.lib \
    ..\..\..\crtw32\dos\st\$O\dos.lib \
    ..\..\..\crtw32\eh\st\$O\eh.lib \
    ..\..\..\crtw32\exec\st\$O\exec.lib \
    ..\..\..\crtw32\heap\st\$O\heap.lib \
    ..\..\..\crtw32\helper\st\$O\helper.lib \
    ..\..\..\crtw32\lowio\st\$O\lowio.lib \
    ..\..\..\crtw32\mbstring\st\$O\mbstring.lib \
    ..\..\..\crtw32\misc\st\$O\misc.lib \
    ..\..\..\crtw32\startup\st\$O\startup.lib \
    ..\..\..\crtw32\stdio\st\$O\stdio.lib \
    ..\..\..\crtw32\string\st\$O\string.lib \
    ..\..\..\crtw32\time\st\$O\time.lib \
    ..\..\..\fpw32\conv\st\$O\conv.lib \
    ..\..\..\fpw32\tran\st\$O\tran.lib

!if !$(FREEBUILD)
OBJLIBFILES=$(OBJLIBFILES) \
    ..\..\..\crtw32\rtc\st\$O\rtc.lib
!endif

!ELSEIF "$(CRTLIBTYPE)" == "NT"

TARGETNAME=libcntpr
TARGETTYPE=LIBRARY
LIBOUTPUT=$(BASEDIR)\public\sdk\lib\$(TARGET_DIRECTORY)

OBJLIBFILES = \
    ..\..\..\crtw32\convert\nt\$O\convert.lib \
    ..\..\..\crtw32\hack\nt\$O\hack.lib    \
    ..\..\..\crtw32\helper\nt\$O\helper.lib  \
    ..\..\..\crtw32\misc\nt\$O\misc.lib    \
    ..\..\..\crtw32\startup\nt\$O\startup.lib \
    ..\..\..\crtw32\stdio\nt\$O\stdio.lib   \
    ..\..\..\crtw32\string\nt\$O\string.lib

!ELSEIF "$(CRTLIBTYPE)" == "EXSUP"

TARGETNAME=exsup
TARGETTYPE=LIBRARY
LIBOUTPUT=$(BASEDIR)\public\sdk\lib\$(TARGET_DIRECTORY)

OBJLIBFILES = \
    ..\..\..\crtw32\misc\nt\$(O)\exsup.obj \
    ..\..\..\crtw32\misc\nt\$(O)\exsup2.obj \
    ..\..\..\crtw32\misc\nt\$(O)\exsup3.obj \
    ..\..\..\crtw32\misc\nt\$(O)\sehsupp.obj \
    ..\..\..\crtw32\misc\nt\$(O)\setjmp.obj \
    ..\..\..\crtw32\misc\nt\$(O)\setjmp3.obj \
    ..\..\..\crtw32\misc\nt\$(O)\setjmpex.obj \
    ..\..\..\crtw32\misc\nt\$(O)\longjmp.obj

!ELSE
!ERROR Unsupported Library CRTLIBTYPE: $(CRTLIBTYPE)
!ENDIF

TARGETEXEFILES = $(TARGETPATH)\$(TARGET_DIRECTORY)\$(TARGETNAME).$(TARGETEXT)

DESTFILE = $(LIBOUTPUT)\$(TARGETNAME).$(TARGETEXT)

NTTARGETFILE1= $(DESTFILE)
