/****************************************************************************************
*                                    SPARTA project                                     *
*                                                                                       *
* (TCP/IP test team)                                                                    *
*                                                                                       *
* Filename: ownwinsock.h                                                                *
* Description: These are handy utility functions usually exported by winsock for our    *
*              own edification. We basically include this here so we don't need to      *
*              link in winsock.                                                         *
*                                                                                       *
*              This code is 'borrowed' from the winsock implementation code.            *
*                                                                                       *
* Revision history:     name          date         modifications                        *
*                                                                                       *
*                       jbekmann      3/30/2000    made modifications to existing code  *
*                                                                                       *
*                (C) Copyright Microsoft Corporation 1999-2000                          *
*****************************************************************************************/

// these functions are intended to be used so that winsock is not required for SPARTA
// they are for convenience only and should not be used for any winsock API related
// work. 

#ifndef _WINSOCKAPI_


#ifndef __OWNWINSOCK_H__
#define __OWNWINSOCK_H__

#ifdef __cplusplus
extern "C"
{
#endif

#define WSAAPI

#define u_long                  ULONG
#define u_short                 USHORT
#define u_char                  UCHAR

/*
 * Internet address (old style... should be updated)
 */
struct in_addr {
        union {
                struct { u_char s_b1,s_b2,s_b3,s_b4; } S_un_b;
                struct { u_short s_w1,s_w2; } S_un_w;
                u_long S_addr;
        } S_un;
#define s_addr  S_un.S_addr
                                /* can be used for most tcp & ip code */
#define s_host  S_un.S_un_b.s_b2
                                /* host on imp */
#define s_net   S_un.S_un_b.s_b1
                                /* network */
#define s_imp   S_un.S_un_w.s_w2
                                /* imp */
#define s_impno S_un.S_un_b.s_b4
                                /* imp # */
#define s_lh    S_un.S_un_b.s_b3
                                /* logical host */
};

/*
 * Definitions of bits in internet address integers.
 * On subnets, the decomposition of addresses to host and net parts
 * is done according to subnet mask, not the masks here.
 */
#define IN_CLASSA(i)            (((long)(i) & 0x80000000) == 0)
#define IN_CLASSA_NET           0xff000000
#define IN_CLASSA_NSHIFT        24
#define IN_CLASSA_HOST          0x00ffffff
#define IN_CLASSA_MAX           128

#define IN_CLASSB(i)            (((long)(i) & 0xc0000000) == 0x80000000)
#define IN_CLASSB_NET           0xffff0000
#define IN_CLASSB_NSHIFT        16
#define IN_CLASSB_HOST          0x0000ffff
#define IN_CLASSB_MAX           65536

#define IN_CLASSC(i)            (((long)(i) & 0xe0000000) == 0xc0000000)
#define IN_CLASSC_NET           0xffffff00
#define IN_CLASSC_NSHIFT        8
#define IN_CLASSC_HOST          0x000000ff

#define INADDR_ANY              (u_long)0x00000000
#define INADDR_LOOPBACK         0x7f000001
#define INADDR_BROADCAST        (u_long)0xffffffff
#define INADDR_NONE             0xffffffff

/*
 * Socket address, internet style.
 */
struct sockaddr_in {
        short   sin_family;
        u_short sin_port;
        struct  in_addr sin_addr;
        char    sin_zero[8];
};

u_long WSAAPI
htonl (
    IN u_long hostlong
    );

u_short WSAAPI
htons (
    IN u_short hostshort
    );

u_long WSAAPI
ntohl (
    IN u_long netlong
    );

u_short WSAAPI
ntohs (
    IN u_short netshort
    );

unsigned long WSAAPI
inet_addr (
           IN const char FAR * cp
           );

char FAR * WSAAPI
inet_ntoa (
    IN struct in_addr in
    );


#ifdef __cplusplus
} // closing brace for extern "C"
#endif

#endif // __OWNWINSOCK_H__

#endif // _WINSOCKAPI_

