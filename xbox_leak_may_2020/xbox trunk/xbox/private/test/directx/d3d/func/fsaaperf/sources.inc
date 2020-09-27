!include ..\..\..\sources.inc

TARGETNAME=l_fsaaperf
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

SYNCHRONIZE_BLOCK=1

SOURCES=\
	fsaaperf.cpp \
	Camera.cpp \
	Ball.cpp \
	WireBall.cpp \
	InvertedBall.cpp
