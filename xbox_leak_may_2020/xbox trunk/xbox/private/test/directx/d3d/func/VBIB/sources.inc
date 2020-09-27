!include ..\..\..\sources.inc

TARGETNAME=l_vbib
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

SYNCHRONIZE_BLOCK=1

SOURCES=\
	CVertexIndexBufferTest.cpp  \
	NormalBuffer.cpp  \
	LockActive.cpp  \
	MultipleLocks.cpp  \
	HugeBuffer.cpp  \
#	Priority.cpp  \
#	SwitchRenderState.cpp  \
#	LostOnReset.cpp  \
#	RelockDynamic.cpp  \
#	DiscardDynamic.cpp  \
#	LockPointerAlias.cpp  \
#	LockPointerAlias7.cpp  \
#	LockPointerAlias8.cpp  \
