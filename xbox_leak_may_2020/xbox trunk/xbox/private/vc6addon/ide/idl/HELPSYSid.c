/* this file contains the actual definitions of */
/* the IIDs and CLSIDs */

/* link this file in with the server and any clients */


/* File created by MIDL compiler version 3.02.88 */
/* at Wed Sep 24 14:58:41 1997
 */
/* Compiler settings for x:\dev-vs\devbin\htmlhelp\v6\idl\HELPSYS.idl:
    Os (OptLev=s), W1, Zp8, env=Win32, ms_ext, c_ext
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

const IID IID_IVsHelpSystem = {0x854d7ac0,0xbc3d,0x11d0,{0xb4,0x21,0x00,0xa0,0xc9,0x0f,0x9d,0xc4}};


#ifdef __cplusplus
}
#endif

