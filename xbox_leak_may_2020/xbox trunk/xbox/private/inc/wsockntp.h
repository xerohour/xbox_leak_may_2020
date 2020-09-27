
#ifndef _WSOCKPNT_H
#define _WSOCKPNT_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    IN_ADDR     ina;                            // IP address (zero if not static/DHCP)
    IN_ADDR     inaOnline;                      // Online IP address (zero if not online)
    WORD        wPortOnline;                    // Online port
    BYTE        abEnet[6];                      // Ethernet MAC address
    BYTE        abOnline[20];                   // Online identification
} XNADDR;

typedef struct {
    BYTE        ab[8];                          // xbox to xbox key identifier
} XNKID;

#define XNET_XNKID_MASK             0xF0        // Mask of flag bits in first byte of XNKID
#define XNET_XNKID_SYSTEM_LINK      0x00        // Peer to peer system link session
#define XNET_XNKID_ONLINE_PEER      0x80        // Peer to peer online session
#define XNET_XNKID_ONLINE_SERVER    0xC0        // Client to server online session

#define XNetXnKidIsSystemLink(pxnkid)           (((pxnkid)->ab[0] & 0xC0) == XNET_XNKID_SYSTEM_LINK)
#define XNetXnKidIsOnlinePeer(pxnkid)           (((pxnkid)->ab[0] & 0xC0) == XNET_XNKID_ONLINE_PEER)
#define XNetXnKidIsOnlineServer(pxnkid)         (((pxnkid)->ab[0] & 0xC0) == XNET_XNKID_ONLINE_SERVER)

typedef struct {
    BYTE        ab[16];                         // xbox to xbox key exchange key
} XNKEY;


#ifdef __cplusplus
}
#endif

#endif  /* _WSOCKPNT_H */
