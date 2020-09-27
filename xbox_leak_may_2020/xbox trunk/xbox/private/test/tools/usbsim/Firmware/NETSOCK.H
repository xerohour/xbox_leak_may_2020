//*******************************************************************//
//      NETSOCK.H                                                    //
//                                                                   //
//      Access to Embedded Netsock(tm) TCP/IP stack                  //
//                                                                   //
//      Copyright (c)1998-99 Micro/sys, Inc.                         //
//                                                                   //
//      This file released with Embedded Netsock Version 1.10        //
//*******************************************************************//

#define NETSOCK_H_VERSION       0x1001  // Represents Version 1.10
										// version is byte swapped

#include <dos.h>                        // int86() and int86x() prototypes

//*******************************************************************//
//                                                                   //
//      Type and Structure definitions                               //
//                                                                   //
//*******************************************************************//

typedef int SOCKET;

// standard 32-bit IP address structure (Berkeley)
// confusing because of access by bytes, words, or long
struct in_addr {
	union {
		  struct { unsigned char s_b1, s_b2, s_b3, s_b4; } S_un_b;      // byte access
		  struct { unsigned short s_w1, s_w2; } S_un_w;                 // word access
		  unsigned long S_addr;                                         // long access
	} S_un;
};

#define s_addr  S_un.S_addr     // nickname for accessing in_addr as long

// standard structure for specifying socket address (Berkeley)
struct sockaddr_in {
		short                   sin_family;     // address family (i.e. AF_INET)
		unsigned short          sin_port;       // port for transport layer
		struct in_addr          sin_addr;       // 32-bit IP address structure
		char                    sin_zero[8];    // unused padding
};

// standard structure for information regarding the Sockets implementation
// returned by WSAStartup()
#define WSADESCRIPTION_LEN      256
#define WSASYSSTATUS_LEN        256
typedef struct {
   unsigned int         wVersion;
   unsigned int         wHighVersion;
   char                 szDescription[WSADESCRIPTION_LEN+1];
   char                 szSystemStatus[WSASYSSTATUS_LEN+1];
   unsigned short       iMaxSockets;
   unsigned short       iMaxUdpDg;
   char far *           lpVendorInfo;
}  WSAData;

// Embedded Netsock current operating configuration
// From Flash Setup(tm), possibly modified by DHCP initialization
struct NetsockConfig {
	unsigned char DHCPGetIP_mask_gate;
	unsigned char IPAddr[4];
	unsigned char NetMask[4];
	unsigned char reserved1[4];
	char LocalName[40];
	char reserved2[40];

	unsigned int  AdaptorIOPort;
	unsigned char AdaptorIRQnum;
	unsigned char AdaptorEtherAddr[6];

	unsigned char NumSockets;
	unsigned char reserved3;
	unsigned char SignificantOtherIP[4];
	unsigned char DHCPGetLocalName;
	unsigned char reserved4;
	unsigned char reserved5;
	unsigned char DHCPServerIP[4];
	unsigned char reserved6[4];
	unsigned char reserved7[4];
	unsigned char reserved8;
	unsigned char reserved9;
	};

//*******************************************************************//
//                                                                   //
//      Common functions                                             //
//                                                                   //
//*******************************************************************//

int LoadEmbeddedNetsock(void);
int ENgetnetconfig(struct NetsockConfig * cfg, int len);

#ifndef ALTERNATE_API
//*******************************************************************//
//                                                                   //
//      Winsock API functions                                        //
//                                                                   //
//*******************************************************************//

int WSAStartup(unsigned int wVersionRequested, WSAData *lpWSAData);
int WSACleanup(void);
int closesocket(SOCKET s);
int bind(SOCKET s, const struct sockaddr_in far * name, int namelen);
int getsockopt(SOCKET s, int level, int optname, char far * optval, int far * optlen);
unsigned long htonl(unsigned long hostlong);
unsigned short htons(unsigned short hostshort);
int ioctlsocket(SOCKET s, long cmd, unsigned long far *argp);
unsigned long inet_addr(const char far * cp);
char far *inet_ntoa( struct in_addr in);
unsigned long ntohl(unsigned long netlong);
unsigned short ntohs(unsigned short netshort);
int recvfrom(SOCKET s, char far * buf, int len, int flags, struct sockaddr_in far * from, int far * fromlen);
int sendto(SOCKET s, const char far* buf, int len, int flags, const struct sockaddr_in far * to, int tolen);
int setsockopt(SOCKET s, int level, int optname, const char far * optval, int optlen);
SOCKET socket(int af, int type, int protocol);
int WSAGetLastError(void);
void WSASetLastError(int err);
#endif  // not ALTERNATE_API

#ifdef ALTERNATE_API
//*******************************************************************//
//                                                                   //
//      Alternate API functions                                      //
//                                                                   //
//*******************************************************************//

int ENStartup(void);
int ENsocket(int type, unsigned int localport);
int ENsendto(int sock, char * buffer, int len, unsigned long destIP, unsigned int destport);
int ENrecvfrom(int sock, char * buffer, int len, unsigned long * fromIP, unsigned int * fromport);
int ENclosesocket(int sock);
int ENsetsockopt(SOCKET s, int optname, int optval);
int ENgetsockopt(SOCKET s, int optname);
int ENgetsockrxavail(int sock);
int ENCleanup(void);
unsigned long IPaddress(char * IPstring);
char * IPstring(unsigned long IPaddress);
#endif  // ALTERNATE_API

//*******************************************************************//
//                                                                   //
//      Various constant definitions for function calls              //
//                                                                   //
//*******************************************************************//

//	 run-time access to Netsock functions


//   supported socket types

#define SOCK_DGRAM      2               // UDP datagram sockets
#define SOCK_RAW        3               // raw protocol socket

//      level at which options are defined

#define SOL_SOCKET      0xffff          // socket level options

//   socket options

#define SO_DEBUG        0x01            // turn on debugging info
#define SO_BROADCAST    0x20            // permit sending of broadcast msgs
#define SO_SNDTIMEO     0x1005          // send timeout
#define SO_RCVTIMEO     0x1006          // receive timeout
#define SO_MAX_MSG_SIZE 0x2003                  // max UDP message size (read only)

//      protocols

#define IPPROTO_IP      0               // IP
#define IPPROTO_ICMP    1               // control message protocol
#define IPPROTO_UDP     17              // user datagram protocol
#define IPPROTO_RAW     255             // raw IP packet

//      address families supported

#define AF_INET                 2
#define INADDR_ANY              (unsigned long)0x00000000
#define INADDR_BROADCAST        (unsigned long)0xFFFFFFFF

//      cmd values for ioctlsocket()

#define FIONREAD        (0x4004467FUL)  // UNIX -> Windows -> this strange value

//      various defines

#define FLAGS_ZERO      (0)             // only valid flags value for sendto() and recvfrom()

//*******************************************************************//
//                                                                   //
//      Error return values                                          //
//                                                                   //
//*******************************************************************//

//      Socket creation and use errors

#define         INVALID_SOCKET          (SOCKET)(~0)
#define         SOCKET_ERROR            (-1)

// values returned by WSAGetLastError
#define         WSAEACCES               10013
#define         WSAEFAULT               10014
#define         WSAEINVAL               10022
#define         WSAEMFILE               10024
#define         WSAENOTSOCK             10038
#define         WSAEDESTADDRREQ         10039
#define         WSAEMSGSIZE             10040
#define         WSAEPROTOTYPE           10041
#define         WSAENOPROTOOPT          10042
#define         WSAEPROTONOSUPPORT      10043
#define         WSAEAFNOSUPPORT         10047
#define         WSAENOBUFS              10055
#define         WSAETIMEDOUT            10060
#define         WSAEHOSTUNREACH         10065
#define         WSASYSNOTREADY          10091
#define         WSAVERNOTSUPPORTED      10092
#define         WSANOTINITIALISED       10093

// Embedded Netsock Alternate API errors

#define         ENE_LDERR_BIOS          (-1)
#define         ENE_LDERR_ADAPTER       (-2)
#define         ENE_LDERR_MEM           (-3)
#define         ENE_LDERR_NETSOCK       (-4)

#define         ENE_NETWORKSTART        (-5)
#define         ENE_NOTRUNNING          (-6)
#define         ENE_SOCKTYPE            (-7)
#define         ENE_INVALIDSOCKET       (-8)
#define         ENE_MAXSOCKETS          (-9)
#define         ENE_MEMORY              (-10)
#define         ENE_BUFFERTOOSMALL      (-11)
#define         ENE_MSGTOOBIG           (-12)
#define         ENE_TIMEOUT             (-13)
#define         ENE_PARAM               (-14)
#define         ENE_NOBROADCAST         (-15)
#define         ENE_HOSTUNREACH         (-16)


#ifdef NETSOCK_MASTER
//*******************************************************************//
//                                                                   //
//      Access to built-in Netsock functions                         //
//                                                                   //
//*******************************************************************//

#define MSYS_INT                (0x1A)  // Netsock uses INT 1A for access
#define MSYS_FUNC_AH			(0xDD)	// Use AH = 0xDD for access
#define MSYS_FUNC_AX			(0xDD00)// Use for 16-bit loads, AL will
										//   filled in by caller

//      Loading functions

#define     ENOP_ENSIZE         (0x80)
#define     ENOP_GETCFG         (0x81)
#define     ENOP_LOADNETSOCK    (0x82)

//      Operating functions

#define FIRSTFUNC 0x90

// Winsock API functions

#define     ENOP_START              FIRSTFUNC
#define     ENOP_CLEAN              FIRSTFUNC+1
#define     ENOP_SOCKET             FIRSTFUNC+2
#define     ENOP_SETOPT             FIRSTFUNC+3
#define     ENOP_GETOPT             FIRSTFUNC+4
#define     ENOP_BIND               FIRSTFUNC+5
#define     ENOP_SENDTO             FIRSTFUNC+6
#define     ENOP_RECVFR             FIRSTFUNC+7
#define     ENOP_GETLASTER          FIRSTFUNC+8
#define     ENOP_IOCTL              FIRSTFUNC+9
#define     ENOP_CLOSE              FIRSTFUNC+10
#define     ENOP_GETCURCONFIG       FIRSTFUNC+11
#define     ENOP_SETLASTER          FIRSTFUNC+12

// Alternate API functions

#define     ENOP_ALTSTART           FIRSTFUNC+30
#define     ENOP_ALTSOCK            FIRSTFUNC+31
#define     ENOP_ALTSEND            FIRSTFUNC+32
#define     ENOP_ALTRECV            FIRSTFUNC+33
#define     ENOP_ALTCLOSE           FIRSTFUNC+34
#define     ENOP_ALTRXAVAIL         FIRSTFUNC+35
#define     ENOP_ALTCLEAN           FIRSTFUNC+36
#define     ENOP_ALTGETOPT          FIRSTFUNC+37
#define     ENOP_ALTSETOPT          FIRSTFUNC+38

char ddstring[20];              // return buffer for inet_ntoa()

//  Global values related to loading Embedded Netsock
unsigned int EmbeddedNetsockBaseSegment = 0;
unsigned int EmbeddedNetsockSize = 0;
unsigned int EmbeddedNetsockConfigSize = 0;
unsigned int EmbeddedNetsockVersion = 0;
unsigned int EmbeddedNetsockLoadError = ENE_LDERR_NETSOCK;

//*******************************************************************//
//                                                                   //
//      Common functions                                             //
//                                                                   //
//*******************************************************************//

int LoadEmbeddedNetsock(void)
  {
	union REGS ENRegs;
	struct SREGS ENSregs;
	//unsigned int major, minor;

	ENRegs.h.ah = MSYS_FUNC_AH;                     // Request Version Number
	ENRegs.h.al = 0x00;
	int86(MSYS_INT, &ENRegs, &ENRegs);
	if (ENRegs.x.cflag)                     // if Micro/sys BIOS support not present
	   return(ENE_LDERR_BIOS);

        ENRegs.h.ah = MSYS_FUNC_AH;
	ENRegs.h.al = ENOP_ENSIZE;
	int86(MSYS_INT, &ENRegs, &ENRegs);
	if (!ENRegs.h.al)                       // if 0 then no Ethernet hardware
		return(ENE_LDERR_ADAPTER);
	if (!ENRegs.x.bx)                       // if 0 then no NETSOCK
		return(ENE_LDERR_NETSOCK);

		// save sizes in global variables
	EmbeddedNetsockSize = ENRegs.x.bx;
	EmbeddedNetsockConfigSize = ENRegs.x.cx;

	// allocate memory for Embedded Netsock, fail if not enough memory
	if(_dos_allocmem(ENRegs.x.bx, &EmbeddedNetsockBaseSegment))
	  {
		EmbeddedNetsockBaseSegment = 0;
		return(ENE_LDERR_MEM);
	  }

	// load Embedded Netsock into memory
	ENRegs.h.ah = MSYS_FUNC_AH;
	ENRegs.h.al = ENOP_LOADNETSOCK;
	ENSregs.es = EmbeddedNetsockBaseSegment;
	int86x(MSYS_INT, &ENRegs, &ENRegs, &ENSregs);
	EmbeddedNetsockLoadError = 0;
	EmbeddedNetsockBaseSegment += 0x20;     // account for 512 byte header
	return(0);
  }

int ENgetnetconfig(struct NetsockConfig * cfg, int len)
  {
	union REGS ENRegs;

        if (EmbeddedNetsockLoadError)           // no successful WSAStartup yet
           return(ENE_NOTRUNNING);

	ENRegs.x.ax = MSYS_FUNC_AX | ENOP_GETCURCONFIG;
	ENRegs.x.bx = FP_OFF(cfg);
	ENRegs.x.cx = FP_SEG(cfg);
	ENRegs.x.dx = len;
	int86(MSYS_INT, &ENRegs, &ENRegs);
	return ENRegs.x.ax;
  }

#ifndef ALTERNATE_API
//*******************************************************************//
//                                                                   //
//      Winsock API functions                                        //
//                                                                   //
//*******************************************************************//

int WSAStartup(unsigned int wVersionRequested, WSAData *lpWSAData)
  {
   union REGS ENRegs;
   //char message[50];
   //unsigned int major, minor;
	int err;

   if(wVersionRequested != 0x0101)      // Embedded Netsock emulates
          return(WSAVERNOTSUPPORTED);   // Windows Sockets version 1.1 only!

        EmbeddedNetsockLoadError = 0;   // Assume no errors in loading
        err = LoadEmbeddedNetsock();    // Load system, any error placed in global
        if (err)
	  {
           EmbeddedNetsockLoadError = err;
           return(WSASYSNOTREADY);
	  }

   ENRegs.x.ax = MSYS_FUNC_AX | ENOP_START;
   ENRegs.x.bx = NETSOCK_H_VERSION;
   ENRegs.x.cx = FP_OFF(lpWSAData);
   ENRegs.x.dx = FP_SEG(lpWSAData);
   int86(MSYS_INT, &ENRegs, &ENRegs);

   EmbeddedNetsockVersion = (*(lpWSAData->lpVendorInfo) << 8) | *(lpWSAData->lpVendorInfo+1);
   return ENRegs.x.ax;
  }



int WSACleanup(void)
  {
	union REGS ENRegs;

	if(EmbeddedNetsockLoadError)            // no successful WSAStartup yet
	  return(SOCKET_ERROR);

	ENRegs.x.ax = MSYS_FUNC_AX | ENOP_CLEAN;
	int86(MSYS_INT, &ENRegs, &ENRegs);
	_dos_freemem(EmbeddedNetsockBaseSegment - 0x20);
	EmbeddedNetsockBaseSegment = 0;
	EmbeddedNetsockSize = 0;
	EmbeddedNetsockConfigSize = 0;
	EmbeddedNetsockVersion = 0;
	EmbeddedNetsockLoadError = ENE_LDERR_NETSOCK;
	return ENRegs.x.ax;
  }

SOCKET socket(int af,int stype,int prot)
  {
	union REGS ENRegs;

	if(EmbeddedNetsockLoadError)            // no successful WSAStartup yet
	  return(SOCKET_ERROR);

	ENRegs.x.ax = MSYS_FUNC_AX | ENOP_SOCKET;
	ENRegs.x.bx = af;
	ENRegs.x.cx = stype;
	ENRegs.x.dx = prot;
	int86(MSYS_INT, &ENRegs, &ENRegs);
	return ENRegs.x.ax;
  }

int closesocket(SOCKET s)
  {
	union REGS ENRegs;

	if(EmbeddedNetsockLoadError)            // no successful WSAStartup yet
	  return(SOCKET_ERROR);

	ENRegs.x.ax = MSYS_FUNC_AX | ENOP_CLOSE;
	ENRegs.x.bx = s;
	int86(MSYS_INT, &ENRegs, &ENRegs);
	return(ENRegs.x.ax);
  }

int setsockopt(SOCKET sock,int level,int name,const char far *optval,int optlen)
  {
	union REGS ENRegs;
	int ENParams[6];
	void far * fp;

	if(EmbeddedNetsockLoadError)            // no successful WSAStartup yet
	  return(SOCKET_ERROR);

	ENParams[0] = sock;
	ENParams[1] = level;
	ENParams[2] = name;
	ENParams[3] = FP_OFF(optval);
	ENParams[4] = FP_SEG(optval);
	ENParams[5] = optlen;
	ENRegs.x.ax = MSYS_FUNC_AX | ENOP_SETOPT;
	fp=(void far *)ENParams;
	ENRegs.x.dx = FP_OFF(fp);
	ENRegs.x.cx = FP_SEG(fp);
	int86(MSYS_INT, &ENRegs, &ENRegs);
	return ENRegs.x.ax;
  }

int getsockopt(SOCKET sock,int level,int name,char far *optval,int far *optlen)
  {
	union REGS ENRegs;
	int ENParams[7];
	void far * fp;

	if(EmbeddedNetsockLoadError)            // no successful WSAStartup yet
	  return(SOCKET_ERROR);

	ENParams[0] = sock;
	ENParams[1] = level;
	ENParams[2] = name;
	ENParams[3] = FP_OFF(optval);
	ENParams[4] = FP_SEG(optval);
	ENParams[5] = FP_OFF(optlen);
	ENParams[6] = FP_SEG(optlen);
	ENRegs.x.ax = MSYS_FUNC_AX | ENOP_GETOPT;
	fp=(void far *)ENParams;
	ENRegs.x.dx = FP_OFF(fp);
	ENRegs.x.cx = FP_SEG(fp);
	int86(MSYS_INT, &ENRegs, &ENRegs);
	return ENRegs.x.ax;
  }

int bind(SOCKET sock, const struct sockaddr_in far *addr, int addrlen)
  {
	union REGS ENRegs;
	struct SREGS ENSregs;

	if(EmbeddedNetsockLoadError)            // no successful WSAStartup yet
	  return(SOCKET_ERROR);

	ENRegs.x.ax = MSYS_FUNC_AX | ENOP_BIND;
	ENRegs.x.bx = sock;
	ENRegs.x.cx = addrlen;
	ENRegs.x.dx = FP_OFF(addr);
	ENSregs.es = FP_SEG(addr);
	int86x(MSYS_INT, &ENRegs, &ENRegs, &ENSregs);
	return ENRegs.x.ax;
  }

int sendto(SOCKET sock, const char far *buf, int len, int flags,
		   const struct sockaddr_in far *to, int tolen)
  {
	union REGS ENRegs;
	int ENParams[8];
	void far * fp;

	if(EmbeddedNetsockLoadError)            // no successful WSAStartup yet
	  return(SOCKET_ERROR);

	ENParams[0] = sock;
	ENParams[1] = len;
	ENParams[2] = flags;
	ENParams[3] = tolen;
	ENParams[4] = FP_OFF(buf);
	ENParams[5] = FP_SEG(buf);
	ENParams[6] = FP_OFF(to);
	ENParams[7] = FP_SEG(to);
	ENRegs.x.ax = MSYS_FUNC_AX | ENOP_SENDTO;
	fp=(void far *)ENParams;
	ENRegs.x.dx = FP_OFF(fp);
	ENRegs.x.cx = FP_SEG(fp);
	int86(MSYS_INT, &ENRegs, &ENRegs);
	return ENRegs.x.ax;
  }

int recvfrom(SOCKET sock, char far *buf, int len, int flags,
		   struct sockaddr_in far *from, int far * fromlen)
  {
	union REGS ENRegs;
	int ENParams[9];
	void far * fp;

	if(EmbeddedNetsockLoadError)            // no successful WSAStartup yet
	  return(SOCKET_ERROR);

	ENParams[0] = sock;
	ENParams[1] = len;
	ENParams[2] = flags;
	ENParams[3] = FP_OFF(buf);
	ENParams[4] = FP_SEG(buf);
	ENParams[5] = FP_OFF(from);
	ENParams[6] = FP_SEG(from);
	ENParams[7] = FP_OFF(fromlen);
	ENParams[8] = FP_SEG(fromlen);
	ENRegs.x.ax = MSYS_FUNC_AX | ENOP_RECVFR;
	fp=(void far *)ENParams;
	ENRegs.x.dx = FP_OFF(fp);
	ENRegs.x.cx = FP_SEG(fp);
	int86(MSYS_INT, &ENRegs, &ENRegs);
	return ENRegs.x.ax;
  }

unsigned long htonl(unsigned long hlong)
  {
	register unsigned int nethi, netlo;

	netlo = (unsigned int)((hlong & 0xffff0000L)>>16);        // host high to net low
	nethi = (unsigned int)(hlong & 0xffff);                                 // host low to net high
	nethi = ((nethi & 0xff00)>>8) | ((nethi & 0xff)<<8);    // swap bytes
	netlo = ((netlo & 0xff00)>>8) | ((netlo & 0xff)<<8);
	return ((long)nethi<<16) | (long)netlo;                 // recombine
  }

unsigned short htons(unsigned short hostshort)
  {
	return (((hostshort >> 8) & 0xff) | (hostshort << 8));
  }

void fstrcpy(char far* dest, char far* src)
{
	int n;

	/* copy each element of the string */
	for (n=0; src[n]; n++)
		dest[n] = src[n];

	dest[n] = 0;    /* null-terminate the string */
}

unsigned long inet_addr(const char far * addstr)
  {
   unsigned long a,b,c,d;
   unsigned long netIP;
   char localstr[40];

	fstrcpy((char far *)localstr, (char far *)addstr);
	sscanf(localstr,"%ld.%ld.%ld.%ld",&a,&b,&c,&d);
	netIP = ((d<<24)|(c<<16)|(b<<8) | a);
	return (netIP) ;
  }

char far *inet_ntoa(struct in_addr in)
  {
	unsigned int a,b,c,d;

	a = in.S_un.S_un_b.s_b1;
	b = in.S_un.S_un_b.s_b2;
	c = in.S_un.S_un_b.s_b3;
	d = in.S_un.S_un_b.s_b4;
	sprintf(ddstring,"%d.%d.%d.%d",d,c,b,a);
	return (char far *)ddstring;
  }

int ioctlsocket(SOCKET sock, long cmd, unsigned long far *argp)
  {
	union REGS ENRegs;
	int ENParams[5];
	void far * fp;

	if(EmbeddedNetsockLoadError)            // no successful WSAStartup yet
	  return(SOCKET_ERROR);

	ENParams[0] = sock;
	ENParams[1] = (int)(cmd & 0xffff);
	ENParams[2] = (int)(cmd >> 16);
	ENParams[3] = FP_OFF(argp);
	ENParams[4] = FP_SEG(argp);
	ENRegs.x.ax = MSYS_FUNC_AX | ENOP_IOCTL;
	fp=(void far *)ENParams;
	ENRegs.x.dx = FP_OFF(fp);
	ENRegs.x.cx = FP_SEG(fp);
	int86(MSYS_INT, &ENRegs, &ENRegs);
	return ENRegs.x.ax;
  }

unsigned long ntohl(unsigned long nlong)
  {
	register int hosthi, hostlo;

	hostlo = (int)((nlong & 0xffff0000L)>>16);              // net high to host low
	hosthi = (int)(nlong & 0xffff);                         // net low to host high
	hosthi = ((hosthi & 0xff00)>>8) | ((hosthi & 0xff)<<8); // swap bytes
	hostlo = ((hostlo & 0xff00)>>8) | ((hostlo & 0xff)<<8);
	return ((long)hosthi<<16) | (long)hostlo;               // recombine
  }

unsigned short ntohs(unsigned short netshort)
  {
	return (((netshort >> 8) & 0xff) | (netshort << 8));
  }

int WSAGetLastError(void)
  {
	union REGS ENRegs;

	if(EmbeddedNetsockLoadError)            // no successful WSAStartup yet
	  return(WSANOTINITIALISED);

	ENRegs.x.ax = MSYS_FUNC_AX | ENOP_GETLASTER;
	int86(MSYS_INT, &ENRegs, &ENRegs);
	return ENRegs.x.ax;
  }

void WSASetLastError(int err)
  {
	union REGS ENRegs;

	if(EmbeddedNetsockLoadError)
	  return;

	ENRegs.x.ax = MSYS_FUNC_AX | ENOP_SETLASTER;
	ENRegs.x.bx = err;
	int86(MSYS_INT, &ENRegs, &ENRegs);
	return;
  }
#endif  // not ALTERNATE_API

#ifdef ALTERNATE_API
//*******************************************************************//
//                                                                   //
//      Alternate API functions                                      //
//                                                                   //
//*******************************************************************//

int ENStartup(void)
  {
	union REGS ENRegs;
	int err;

	err = LoadEmbeddedNetsock();
	if(err)
	  {
		EmbeddedNetsockLoadError = err;
		return(err);
	  }
	ENRegs.x.ax = MSYS_FUNC_AX | ENOP_ALTSTART;
	int86(MSYS_INT, &ENRegs, &ENRegs);
	return ENRegs.x.ax;
  }

int ENsocket(int type, unsigned int localport)
  {
	union REGS ENRegs;

	if(EmbeddedNetsockLoadError)            // no successful WSAStartup yet
	  return(ENE_NOTRUNNING);

	ENRegs.x.ax = MSYS_FUNC_AX | ENOP_ALTSOCK;
	ENRegs.x.bx = type;
	ENRegs.x.cx = localport;
	int86(MSYS_INT, &ENRegs, &ENRegs);
	return ENRegs.x.ax;
  }

int ENsendto(int sock, char * buffer, int len, unsigned long destIP, unsigned int destport)
  {
	union REGS ENRegs;
	int ENParams[7];
	void far * fp;

	if(EmbeddedNetsockLoadError)            // no successful WSAStartup yet
	  return(ENE_NOTRUNNING);

	ENParams[0] = sock;
	ENParams[1] = FP_OFF(buffer);
	ENParams[2] = FP_SEG(buffer);
	ENParams[3] = len;
	ENParams[4] = FP_OFF(destIP);
	ENParams[5] = FP_SEG(destIP);
	ENParams[6] = destport;
	ENRegs.x.ax = MSYS_FUNC_AX | ENOP_ALTSEND;
	fp=(void far *)ENParams;
	ENRegs.x.dx = FP_OFF(fp);
	ENRegs.x.cx = FP_SEG(fp);
	int86(MSYS_INT, &ENRegs, &ENRegs);
	return ENRegs.x.ax;
  }

int ENrecvfrom(int sock, char * buffer, int len, unsigned long * fromIP, unsigned int * fromport)
  {
	union REGS ENRegs;
	int ENParams[8];
	void far * fp;

	if(EmbeddedNetsockLoadError)            // no successful WSAStartup yet
	  return(ENE_NOTRUNNING);

	ENParams[0] = sock;
	ENParams[1] = len;
	ENParams[2] = FP_OFF(buffer);
	ENParams[3] = FP_SEG(buffer);
	ENParams[4] = FP_OFF(fromIP);
	ENParams[5] = FP_SEG(fromIP);
	ENParams[6] = FP_OFF(fromport);
	ENParams[7] = FP_SEG(fromport);
	ENRegs.x.ax = MSYS_FUNC_AX | ENOP_ALTRECV;
	fp=(void far *)ENParams;
	ENRegs.x.dx = FP_OFF(fp);
	ENRegs.x.cx = FP_SEG(fp);
	int86(MSYS_INT, &ENRegs, &ENRegs);
	return ENRegs.x.ax;
  }

int ENclosesocket(int sock)
  {
	union REGS ENRegs;

	if(EmbeddedNetsockLoadError)            // no successful WSAStartup yet
	  return(ENE_NOTRUNNING);

	ENRegs.x.ax = MSYS_FUNC_AX | ENOP_ALTCLOSE;
	ENRegs.x.bx = sock;
	int86(MSYS_INT, &ENRegs, &ENRegs);
	return ENRegs.x.ax;
  }

int ENgetsockrxavail(int sock)
  {
	union REGS ENRegs;

	if(EmbeddedNetsockLoadError)            // no successful WSAStartup yet
	  return(ENE_NOTRUNNING);

	ENRegs.x.ax = MSYS_FUNC_AX | ENOP_ALTRXAVAIL;
	ENRegs.x.bx = sock;
	int86(MSYS_INT, &ENRegs, &ENRegs);
	return ENRegs.x.ax;
  }

int ENgetsockopt(int sock, int optname)
  {
	union REGS ENRegs;

	if(EmbeddedNetsockLoadError)            // no successful WSAStartup yet
	  return(ENE_NOTRUNNING);

	ENRegs.x.ax = MSYS_FUNC_AX | ENOP_ALTGETOPT;
	ENRegs.x.bx = sock;
	ENRegs.x.cx = optname;
	int86(MSYS_INT, &ENRegs, &ENRegs);
	return ENRegs.x.ax;
  }

int ENsetsockopt(int sock, int optname, int optval)
  {
	union REGS ENRegs;

	if(EmbeddedNetsockLoadError)            // no successful WSAStartup yet
	  return(ENE_NOTRUNNING);

	ENRegs.x.ax = MSYS_FUNC_AX | ENOP_ALTSETOPT;
	ENRegs.x.bx = sock;
	ENRegs.x.cx = optname;
	ENRegs.x.dx = optval;
	int86(MSYS_INT, &ENRegs, &ENRegs);
	return ENRegs.x.ax;
  }

int ENCleanup(void)
  {
	union REGS ENRegs;

	if(EmbeddedNetsockLoadError)            // no successful WSAStartup yet
	  return(ENE_NOTRUNNING);

	ENRegs.x.ax = MSYS_FUNC_AX | ENOP_ALTCLEAN;
	int86(MSYS_INT, &ENRegs, &ENRegs);
	return ENRegs.x.ax;
  }

#endif  // ALTERNATE_API

#endif  // NETSOCK_MASTER



