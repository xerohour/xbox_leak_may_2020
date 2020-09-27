TARGETTYPE=LIBRARY
TARGETPATH=..\..\lib

SOURCES=\
	gmtime1.cpp \
	localtime1.cpp \
	mktime1.cpp \
	mktime2.cpp \
	futime1.cpp \
	futime2.cpp \
	utime1.cpp \
	utime2.cpp \
	ftime1.cpp \
	time1.cpp \
	ctime1.cpp \
	CRunTime.cpp \
    testcrt.cpp \
	drv.cpp

# excluded mktime3.cpp, strftime1.cpp

INCLUDES=$(BASEDIR)\private\test\crttests\test\clib;$(SDK_INC_PATH)

MSC_WARNING_LEVEL=/W2
MSC_OPTIMIZATION=/Od

SOURCES_USED=..\sources.inc
