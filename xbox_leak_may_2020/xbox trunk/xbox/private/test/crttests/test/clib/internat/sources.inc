TARGETTYPE=LIBRARY
TARGETPATH=..\..\lib

SOURCES=\
	_mbslen.c \
	_WCSICOL.C \
	localeco.c \
	mbstowcs.c \
	mbtowc.c \
	WCSCOLL.C \
	wcsftime.c \
	wcstod1.c \
	wcstod2.c \
	wcstod3.c \
	wcstol1.c \
	wcstol2.c \
	wcstol3.c \
	wcstombs.c \
	wcstoul1.c \
	wcstoul2.c \
	wcstoul3.c \
	WCSXFRM.C \
	wctomb.c \
	drv.C

INCLUDES=$(BASEDIR)\private\test\crttests\test\clib;$(SDK_INC_PATH)

MSC_WARNING_LEVEL=/W2
MSC_OPTIMIZATION=/Od

SOURCES_USED=..\sources.inc
