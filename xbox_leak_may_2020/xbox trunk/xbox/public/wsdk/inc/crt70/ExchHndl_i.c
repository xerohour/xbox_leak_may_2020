/* this file contains the actual definitions of */
/* the IIDs and CLSIDs */

/* link this file in with the server and any clients */


/* File created by MIDL compiler version 3.01.75 */
/* at Fri Mar 27 08:32:24 1998
 */
/* Compiler settings for J:\events\src\SS\exchhndl.idl:
    Oicf (OptLev=i2), W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: none
*/
//@@MIDL_FILE_HEADING(  )
#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __IID_DEFINED__
#define __IID_DEFINED__

typedef struct _IID
{
    unsigned long x;
    unsigned short s1;
    unsigned short s2;
    unsigned char  c[8];
} IID;

#endif // __IID_DEFINED__

#ifndef CLSID_DEFINED
#define CLSID_DEFINED
typedef IID CLSID;
#endif // CLSID_DEFINED

const IID IID_IExchangeEventHandler = {0x69E54156,0xB371,0x11D0,{0xBC,0xD9,0x00,0xAA,0x00,0xC1,0xAB,0x1C}};


const IID IID_IEventSource = {0x69E54152,0xB371,0x11D0,{0xBC,0xD9,0x00,0xAA,0x00,0xC1,0xAB,0x1C}};


#ifdef __cplusplus
}
#endif

