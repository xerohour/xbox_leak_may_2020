//
// javadbg.h
//
// Miscellaneous Java debugger header

#ifndef __JAVADBG_H__
#define __JAVADBG_H__

size_t LOADDS PASCAL JavaGetDebuggeeBytes(HTM hTM, size_t cb, void FAR* bBuff, PFRAME pFrame);

#endif // __JAVADBG_H__
