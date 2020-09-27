#ifndef __LCP_H__
#define __LCP_H__

#define LCP_DEFAULT_MRU         1500

#define LCP_OPTION_MRU          0x01
#define LCP_OPTION_ACCM         0x02
#define LCP_OPTION_AUTHENT      0x03
#define LCP_OPTION_MAGIC        0x05
#define LCP_OPTION_PFC          0x07
#define LCP_OPTION_ACFC         0x08
#define LCP_OPTION_CALLBACK     0x0D
#define LCP_OPTION_MRRU         0x11
#define LCP_OPTION_SHORT_SEQ    0x12
#define LCP_OPTION_ENDPOINT     0x13
#define LCP_OPTION_LINK_DISCRIM 0x17
#define LCP_OPTION_LIMIT        0x17    // highest # we can handle

#define LCP_N_MRU               (1 << LCP_OPTION_MRU)
#define LCP_N_ACCM              (1 << LCP_OPTION_ACCM)
#define LCP_N_AUTHENT           (1 << LCP_OPTION_AUTHENT)
#define LCP_N_MAGIC             (1 << LCP_OPTION_MAGIC)
#define LCP_N_PFC               (1 << LCP_OPTION_PFC)
#define LCP_N_ACFC              (1 << LCP_OPTION_ACFC)
#define LCP_N_CALLBACK          (1 << LCP_OPTION_CALLBACK)
#define LCP_N_MRRU              (1 << LCP_OPTION_MRRU)
#define LCP_N_SHORT_SEQ         (1 << LCP_OPTION_SHORT_SEQ)
#define LCP_N_ENDPOINT          (1 << LCP_OPTION_ENDPOINT)
#define LCP_N_LINK_DISCRIM      (1 << LCP_OPTION_LINK_DISCRIM)

#define  LCP_AP_FIRST           0x00000001
#define  LCP_AP_EAP             0x00000001
#define  LCP_AP_CHAP_MS_NEW     0x00000002
#define  LCP_AP_CHAP_MS         0x00000004
#define  LCP_AP_CHAP_MD5        0x00000008
#define  LCP_AP_SPAP_NEW        0x00000010
#define  LCP_AP_SPAP_OLD        0x00000020
#define  LCP_AP_PAP             0x00000040
#define  LCP_AP_MAX             0x00000080

// Local.Want - options to request, contains desired values, only
// non-default options need to be negotiated.
// Local.WillNegotiate - options to accept in a NAK from remote.
// Local.Work - options currently being negotiated
// Remote.Want - options to suggest by NAK if not present in REQ.
// Remote.WillNegotiate - options to accept in a REQ from remote.
// Remote.Work - options currently being negotiated.

typedef struct _LCP_OPTIONS {

    DWORD Negotiate;            // negotiation flags
    DWORD MRU;                  // Maximum Receive Unit
    DWORD ACCM;                 // Async Control Char Map
    DWORD AP;                   // Authentication protocol
    DWORD APDataSize;           // Auth. protocol data size in bytes
    PBYTE pAPData;              // Pointer Auth. protocol data
    DWORD MagicNumber;          // Magic number value
    DWORD PFC;                  // Protocol field compression.
    DWORD ACFC;                 // Address and Control Field Compression.
    DWORD Callback;             // Callback
    DWORD MRRU;                 // Maximum Reconstructed Receive Unit
    DWORD ShortSequence;        // Short Sequence Number Header Format
    BYTE  EndpointDiscr[21];    // Endpoint Discriminator.
    DWORD dwEDLength;           // Length of Endpoint Discriminator
    DWORD dwLinkDiscriminator;  // Link Discriminator (for BAP/BACP)

} LCP_OPTIONS;

typedef struct _LCP_SIDE {
    DWORD WillNegotiate;
    DWORD APsAvailable;
    LCP_OPTIONS Want;
    LCP_OPTIONS Work;
} LCP_SIDE;

typedef struct _LCP_INFO {
    LCP_SIDE Local;
    LCP_SIDE Remote;
    DWORD dwMagicNumberFailureCount;
} LCP_INFO;

#endif __LCP_H__

