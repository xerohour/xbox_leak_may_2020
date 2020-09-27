!include ..\..\..\sources.inc

TARGETNAME=l_cva
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

SYNCHRONIZE_BLOCK=1

SOURCES=\
	cva.cpp \
	Camera.cpp \
	Ball.cpp \
	WireBall.cpp \
	InvertedBall.cpp
