!include ..\..\..\sources.inc

TARGETNAME=l_event
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

SYNCHRONIZE_BLOCK=1

SOURCES=\
	event.cpp \
	Camera.cpp \
	Ball.cpp \
	WireBall.cpp \
	InvertedBall.cpp
