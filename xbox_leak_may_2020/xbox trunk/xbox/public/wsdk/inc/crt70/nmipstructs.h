//=============================================================================
//  Microsoft (R) Network Monitor (tm). 
//  Copyright (C) 1992-1999. All rights reserved.
//
//  MODULE: nmipstructs.h
//
//  IP structures & #defines
//=============================================================================

#ifndef NMIPSTRUCTS_H
#define NMIPSTRUCTS_H

#ifdef __cplusplus
extern "C"
{
#endif

//
// IP Packet Structure
//

#pragma warning(disable:4200)

typedef struct _IP 
{
    union 
    {
        BYTE   Version;
        BYTE   HdrLen;
    };
    BYTE ServiceType;
    WORD TotalLen;
    WORD ID;
    union 
    {
        WORD   Flags;
        WORD   FragOff;
    };
    BYTE TimeToLive;
    BYTE Protocol;
    WORD HdrChksum;
    DWORD   SrcAddr;
    DWORD   DstAddr;
    BYTE Options[0];
} IP;

typedef IP * LPIP;
typedef IP UNALIGNED * ULPIP;

#pragma warning(default:4200)

// Psuedo Header used for CheckSum Calculations
typedef struct _PSUHDR 
{
    DWORD   ph_SrcIP;
    DWORD   ph_DstIP;
    UCHAR   ph_Zero;
    UCHAR   ph_Proto;
    WORD    ph_ProtLen;
} PSUHDR;
typedef PSUHDR UNALIGNED * LPPSUHDR;

//
// IP Bitmasks that are useful
// (and the appropriate bit shifts, as well)
//

#define IP_VERSION_MASK ((BYTE) 0xf0)
#define IP_VERSION_SHIFT (4)
#define IP_HDRLEN_MASK  ((BYTE) 0x0f)
#define IP_HDRLEN_SHIFT (0)
#define IP_PRECEDENCE_MASK ((BYTE) 0xE0)
#define IP_PRECEDENCE_SHIFT   (5)
#define IP_TOS_MASK ((BYTE) 0x1E)
#define IP_TOS_SHIFT   (1)
#define IP_DELAY_MASK   ((BYTE) 0x10)
#define IP_THROUGHPUT_MASK ((BYTE) 0x08)
#define IP_RELIABILITY_MASK   ((BYTE) 0x04)
#define IP_FLAGS_MASK   ((BYTE) 0xE0)
#define IP_FLAGS_SHIFT  (13)
#define IP_DF_MASK   ((BYTE) 0x40)
#define IP_MF_MASK   ((BYTE) 0x20)
#define IP_MF_SHIFT     (5)
#define IP_FRAGOFF_MASK ((WORD) 0x1FFF)
#define IP_FRAGOFF_SHIFT   (3)
#define IP_TCC_MASK  ((DWORD) 0xFFFFFF00)
#define IP_TIME_OPTS_MASK  ((BYTE) 0x0F)
#define IP_MISS_STNS_MASK  ((BYTE) 0xF0)

#define IP_TIME_OPTS_SHIFT (0)
#define IP_MISS_STNS_SHIFT  (4)

#ifndef XCHG
#define XCHG(x)         MAKEWORD( HIBYTE(x), LOBYTE(x) )
#endif

#ifndef DXCHG
#define DXCHG(x)        MAKELONG( XCHG(HIWORD(x)), XCHG(LOWORD(x)) )
#endif

//
// Offset to checksum field in ip header
//
#define IP_CHKSUM_OFF   10

INLINE BYTE IP_Version(ULPIP pIP)
{
    return (pIP->Version & IP_VERSION_MASK) >> IP_VERSION_SHIFT;
}

INLINE DWORD IP_HdrLen(ULPIP pIP)
{
    return ((pIP->HdrLen & IP_HDRLEN_MASK) >> IP_HDRLEN_SHIFT) << 2;
}

INLINE WORD IP_FragOff(ULPIP pIP)
{
    return (XCHG(pIP->FragOff) & IP_FRAGOFF_MASK) << IP_FRAGOFF_SHIFT;
}

INLINE DWORD IP_TotalLen(ULPIP pIP)
{
    return XCHG(pIP->TotalLen);
}

INLINE DWORD IP_MoreFragments(ULPIP pIP)
{
    return (pIP->Flags & IP_MF_MASK) >> IP_MF_SHIFT;
}


//
// Well known ports in the TCP/IP protocol (See RFC 1060)
//
#define PORT_TCPMUX              1  // TCP Port Service Multiplexer
#define PORT_RJE                 5  // Remote Job Entry
#define PORT_ECHO                7  // Echo
#define PORT_DISCARD             9  // Discard
#define PORT_USERS              11  // Active users
#define PORT_DAYTIME            13  // Daytime
#define PORT_NETSTAT            15  // Netstat
#define PORT_QUOTE              17  // Quote of the day
#define PORT_CHARGEN            19  // Character Generator
#define PORT_FTPDATA            20  // File transfer [default data]
#define PORT_FTP                21  // File transfer [Control]
#define PORT_TELNET             23  // Telnet
#define PORT_SMTP               25  // Simple Mail Transfer
#define PORT_NSWFE              27  // NSW User System FE
#define PORT_MSGICP             29  // MSG ICP
#define PORT_MSGAUTH            31  // MSG Authentication
#define PORT_DSP                33  // Display Support
#define PORT_PRTSERVER          35  // any private printer server
#define PORT_TIME               37  // Time
#define PORT_RLP                39  // Resource Location Protocol
#define PORT_GRAPHICS           41  // Graphics
#define PORT_NAMESERVER         42  // Host Name Server
#define PORT_NICNAME            43  // Who is
#define PORT_MPMFLAGS           44  // MPM Flags 
#define PORT_MPM                45  // Message Processing Module [recv]
#define PORT_MPMSND             46  // MPM [default send]
#define PORT_NIFTP              47  // NI FTP
#define PORT_LOGIN              49  // Login Host Protocol
#define PORT_LAMAINT            51  // IMP Logical Address Maintenance
#define PORT_DOMAIN             53  // Domain Name Server
#define PORT_ISIGL              55  // ISI Graphics Language
#define PORT_ANYTERMACC         57  // any private terminal access
#define PORT_ANYFILESYS         59  // any private file service
#define PORT_NIMAIL             61  // NI Mail
#define PORT_VIAFTP             63  // VIA Systems - FTP
#define PORT_TACACSDS           65  // TACACS - Database Service
#define PORT_BOOTPS             67  // Bootstrap Protocol server
#define PORT_BOOTPC             68  // Bootstrap Protocol client
#define PORT_TFTP               69  // Trivial File Transfer
#define PORT_NETRJS1            71  // Remote Job service
#define PORT_NETRJS2            72  // Remote Job service
#define PORT_NETRJS3            73  // Remote Job service
#define PORT_NETRJS4            74  // Remote Job service
#define PORT_ANYDIALOUT         75  // any private dial out service
#define PORT_ANYRJE             77  // any private RJE service
#define PORT_FINGER             79  // Finger
#define PORT_HTTP               80  // HTTP (www)
#define PORT_HOSTS2NS           81  // Hosts2 Name Server
#define PORT_MITMLDEV1          83  // MIT ML Device
#define PORT_MITMLDEV2          85  // MIT ML Device
#define PORT_ANYTERMLINK        87  // any private terminal link
#define PORT_SUMITTG            89  // SU/MIT Telnet Gateway
#define PORT_MITDOV             91  // MIT Dover Spooler
#define PORT_DCP                93  // Device Control Protocol
#define PORT_SUPDUP             95  // SUPDUP
#define PORT_SWIFTRVF           97  // Swift Remote Vitural File Protocol
#define PORT_TACNEWS            98  // TAC News
#define PORT_METAGRAM           99  // Metagram Relay
#define PORT_NEWACCT           100  // [Unauthorized use]
#define PORT_HOSTNAME          101  // NIC Host Name Server
#define PORT_ISOTSAP           102  // ISO-TSAP
#define PORT_X400              103  // X400
#define PORT_X400SND           104  // X400 - SND
#define PORT_CSNETNS           105  // Mailbox Name Nameserver
#define PORT_RTELNET           107  // Remote Telnet Service
#define PORT_POP2              109  // Post Office Protocol - version 2
#define PORT_POP3              110  // Post Office Protocol - version 3
#define PORT_SUNRPC            111  // SUN Remote Procedure Call
#define PORT_AUTH              113  // Authentication
#define PORT_SFTP              115  // Simple File Transfer Protocol
#define PORT_UUCPPATH          117  // UUCP Path Service
#define PORT_NNTP              119  // Network News Transfer Protocol
#define PORT_ERPC              121  // Encore Expedited Remote Proc. Call
#define PORT_NTP               123  // Network Time Protocol
#define PORT_LOCUSMAP          125  // Locus PC-Interface Net Map Sesrver
#define PORT_LOCUSCON          127  // Locus PC-Interface Conn Server
#define PORT_PWDGEN            129  // Password Generator Protocol
#define PORT_CISCOFNA          130  // CISCO FNATIVE
#define PORT_CISCOTNA          131  // CISCO TNATIVE
#define PORT_CISCOSYS          132  // CISCO SYSMAINT
#define PORT_STATSRV           133  // Statistics Service
#define PORT_INGRESNET         134  // Ingres net service
#define PORT_LOCSRV            135  // Location Service
#define PORT_PROFILE           136  // PROFILE Naming System
#define PORT_NETBIOSNS         137  // NETBIOS Name Service
#define PORT_NETBIOSDGM        138  // NETBIOS Datagram Service
#define PORT_NETBIOSSSN        139  // NETBIOS Session Service
#define PORT_EMFISDATA         140  // EMFIS Data Service
#define PORT_EMFISCNTL         141  // EMFIS Control Service
#define PORT_BLIDM             142  // Britton-Lee IDM
#define PORT_IMAP2             143  // Interim Mail Access Protocol v2
#define PORT_NEWS              144  // NewS
#define PORT_UAAC              145  // UAAC protocol
#define PORT_ISOTP0            146  // ISO-IP0
#define PORT_ISOIP             147  // ISO-IP
#define PORT_CRONUS            148  // CRONUS-Support
#define PORT_AED512            149  // AED 512 Emulation Service
#define PORT_SQLNET            150  // SQL-NET
#define PORT_HEMS              151  // HEMS
#define PORT_BFTP              152  // Background File Transfer Protocol
#define PORT_SGMP              153  // SGMP
#define PORT_NETSCPROD         154  // NETSC
#define PORT_NETSCDEV          155  // NETSC
#define PORT_SQLSRV            156  // SQL service
#define PORT_KNETCMP           157  // KNET/VM Command/Message Protocol
#define PORT_PCMAILSRV         158  // PCMail server
#define PORT_NSSROUTING        159  // NSS routing
#define PORT_SGMPTRAPS         160  // SGMP-TRAPS
#define PORT_SNMP              161  // SNMP
#define PORT_SNMPTRAP          162  // SNMPTRAP
#define PORT_CMIPMANAGE        163  // CMIP/TCP Manager
#define PORT_CMIPAGENT         164  // CMIP/TCP Agent
#define PORT_XNSCOURIER        165  // Xerox
#define PORT_SNET              166  // Sirius Systems
#define PORT_NAMP              167  // NAMP
#define PORT_RSVD              168  // RSVC
#define PORT_SEND              169  // SEND
#define PORT_PRINTSRV          170  // Network Postscript
#define PORT_MULTIPLEX         171  // Network Innovations Multiples
#define PORT_CL1               172  // Network Innovations CL/1
#define PORT_XYPLEXMUX         173  // Xyplex
#define PORT_MAILQ             174  // MAILQ
#define PORT_VMNET             175  // VMNET
#define PORT_GENRADMUX         176  // GENRAD-MUX
#define PORT_XDMCP             177  // X Display Manager Control Protocol
#define PORT_NEXTSTEP          178  // NextStep Window Server
#define PORT_BGP               179  // Border Gateway Protocol
#define PORT_RIS               180  // Intergraph
#define PORT_UNIFY             181  // Unify
#define PORT_UNISYSCAM         182  // Unisys-Cam
#define PORT_OCBINDER          183  // OCBinder
#define PORT_OCSERVER          184  // OCServer
#define PORT_REMOTEKIS         185  // Remote-KIS
#define PORT_KIS               186  // KIS protocol
#define PORT_ACI               187  // Application Communication Interface
#define PORT_MUMPS             188  // MUMPS
#define PORT_QFT               189  // Queued File Transport
#define PORT_GACP              190  // Gateway Access Control Protocol
#define PORT_PROSPERO          191  // Prospero
#define PORT_OSUNMS            192  // OSU Network Monitoring System
#define PORT_SRMP              193  // Spider Remote Monitoring Protocol
#define PORT_IRC               194  // Internet Relay Chat Protocol
#define PORT_DN6NLMAUD         195  // DNSIX Network Level Module Audit
#define PORT_DN6SMMRED         196  // DSNIX Session Mgt Module Audit Redirector
#define PORT_DLS               197  // Directory Location Service
#define PORT_DLSMON            198  // Directory Location Service Monitor
#define PORT_ATRMTP            201  // AppleTalk Routing Maintenance
#define PORT_ATNBP             202  // AppleTalk Name Binding
#define PORT_AT3               203  // AppleTalk Unused
#define PORT_ATECHO            204  // AppleTalk Echo
#define PORT_AT5               205  // AppleTalk Unused
#define PORT_ATZIS             206  // AppleTalk Zone Information
#define PORT_AT7               207  // AppleTalk Unused
#define PORT_AT8               208  // AppleTalk Unused
#define PORT_SURMEAS           243  // Survey Measurement
#define PORT_LINK              245  // LINK
#define PORT_DSP3270           246  // Display Systems Protocol
#define PORT_LDAP1             389  // LDAP
#define PORT_ISAKMP            500  // ISAKMP
#define PORT_REXEC             512  // Remote Process Execution
#define PORT_RLOGIN            513  // Remote login a la telnet
#define PORT_RSH               514  // Remote command
#define PORT_LPD               515  // Line printer spooler - LPD
#define PORT_RIP               520  // TCP=? / UDP=RIP
#define PORT_TEMPO             526  // Newdate
#define PORT_COURIER           530  // rpc
#define PORT_NETNEWS           532  // READNEWS
#define PORT_UUCPD             540  // UUCPD
#define PORT_KLOGIN            543  //
#define PORT_KSHELL            544  // krcmd
#define PORT_DSF               555  //
#define PORT_REMOTEEFS         556  // RFS server
#define PORT_CHSHELL           562  // chmod
#define PORT_METER             570  // METER
#define PORT_PCSERVER          600  // SUN IPC Server
#define PORT_NQS               607  // NQS
#define PORT_HMMP_INDICATION   612  //     
#define PORT_HMMP_OPERATION    613  //     
#define PORT_MDQS              666  // MDQS
#define PORT_LPD721            721  // LPD Client (lpd client ports 721 - 731)
#define PORT_LPD722            722  // LPD Client (see RFC 1179)
#define PORT_LPD723            723  // LPD Client
#define PORT_LPD724            724  // LPD Client
#define PORT_LPD725            725  // LPD Client
#define PORT_LPD726            726  // LPD Client
#define PORT_LPD727            727  // LPD Client
#define PORT_LPD728            728  // LPD Client
#define PORT_LPD729            729  // LPD Client
#define PORT_LPD730            730  // LPD Client
#define PORT_LPD731            731  // LPD Client
#define PORT_RFILE             750  // RFILE
#define PORT_PUMP              751  // PUMP
#define PORT_QRH               752  // QRH
#define PORT_RRH               753  // RRH
#define PORT_TELL              754  // TELL
#define PORT_NLOGIN            758  // NLOGIN
#define PORT_CON               759  // CON
#define PORT_NS                760  // NS
#define PORT_RXE               761  // RXE
#define PORT_QUOTAD            762  // QUOTAD
#define PORT_CYCLESERV         763  // CYCLESERV
#define PORT_OMSERV            764  // OMSERV
#define PORT_WEBSTER           765  // WEBSTER
#define PORT_PHONEBOOK         767  // PHONE
#define PORT_VID               769  // VID
#define PORT_RTIP              771  // RTIP
#define PORT_CYCLESERV2        772  // CYCLESERV-2
#define PORT_SUBMIT            773  // submit
#define PORT_RPASSWD           774  // RPASSWD
#define PORT_ENTOMB            775  // ENTOMB
#define PORT_WPAGES            776  // WPAGES
#define PORT_WPGS              780  // wpgs
#define PORT_MDBSDAEMON        800  // MDBS DAEMON
#define PORT_DEVICE            801  // DEVICE
#define PORT_MAITRD            997  // MAITRD
#define PORT_BUSBOY            998  // BUSBOY
#define PORT_GARCON            999  // GARCON
#define PORT_NFS              2049  // NFS
#define PORT_LDAP2            3268  // LDAP
#define PORT_PPTP             5678  // PPTP

#ifdef __cplusplus
}
#endif

#endif // NMIPSTRUCTS_H
