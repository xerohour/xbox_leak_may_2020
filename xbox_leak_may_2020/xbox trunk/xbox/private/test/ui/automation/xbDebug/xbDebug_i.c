/* this file contains the actual definitions of */
/* the IIDs and CLSIDs */

/* link this file in with the server and any clients */


/* File created by MIDL compiler version 5.01.0164 */
/* at Mon Feb 11 15:00:43 2002
 */
/* Compiler settings for C:\xbox\private\test\ui\automation\xbDebug\xbDebug.idl:
    Oicf (OptLev=i2), W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
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

const IID IID_INetDebug = {0xB6BD872B,0xD6C7,0x4ACD,{0x97,0x04,0x81,0xF4,0xDC,0xE6,0x5C,0xA4}};


const IID LIBID_XBDEBUGLib = {0x2EF1DC7D,0xB7B4,0x4B86,{0x92,0xBE,0x93,0x57,0x20,0xE3,0x25,0xBB}};


const CLSID CLSID_NetDebug = {0xB11C43CA,0x5735,0x43FD,{0x98,0xAF,0x52,0x87,0x5C,0xC5,0x6E,0xA7}};


#ifdef __cplusplus
}
#endif

