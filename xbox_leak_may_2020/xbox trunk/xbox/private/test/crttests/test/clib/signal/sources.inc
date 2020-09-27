TARGETTYPE=LIBRARY
TARGETPATH=..\..\lib

SOURCES=\
	e_sig1.c \
	f_sig1.c \
	f_sig2.c \
	f_sig3.c \
	i_sig1.c \
	i_sig2.c \
	drv.c

INCLUDES=$(BASEDIR)\private\test\crttests\test\clib;$(SDK_INC_PATH)

MSC_WARNING_LEVEL=/W2
MSC_OPTIMIZATION=/Od

SOURCES_USED=..\sources.inc
