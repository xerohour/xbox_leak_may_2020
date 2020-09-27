/*****************************************************************************
 *
 *	 xboxinit.h - Wrapper for WININET stuff
 *
 *****************************************************************************/

#ifndef _XBOXINET_H
#define _XBOXINET_H


HINTERNET GetWininetSessionHandle(void);
void UnloadWininet(void);


#endif // _XBOXINET_H
