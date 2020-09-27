TARGETTYPE=LIBRARY
TARGETPATH=..\..\lib

SOURCES=\
	_ecvt.c \
	drv.c

INCLUDES=$(BASEDIR)\private\test\crttests\test\clib;$(SDK_INC_PATH)

MSC_WARNING_LEVEL=/W2
MSC_OPTIMIZATION=/Od

SOURCES_USED=..\sources.inc
