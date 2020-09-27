/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    phy.h

Abstract:

    Interface to Ethernet transceiver code inside the ROM

Revision History:

    04/05/2001 davidx
        Created it.

--*/

#ifndef _PHY_H_
#define _PHY_H_

//
// Initialize the PHY
//
NTKERNELAPI
NTSTATUS
PhyInitialize(
    BOOL forceReset,
    VOID* param OPTIONAL
    );

//
// Get the current link state
//
NTKERNELAPI
DWORD
PhyGetLinkState(
    BOOL update
    );

#define BIT(n)                  (1u << (n))

//
// MII/PHY related declarations
//
#define MIIREG_CONTROL 0
#define MIIREG_STATUS  1
#define MIIREG_ANAR    4
#define MIIREG_LPANAR  5

#define MIICONTROL_RESET                        BIT(15)
#define MIICONTROL_LOOPBACK_ENABLED             BIT(14)
#define MIICONTROL_SPEED_SELECTION_BIT1         BIT(13)
#define MIICONTROL_ENABLE_AUTO_NEGOTIATION      BIT(12)
#define MIICONTROL_POWER_DOWN                   BIT(11)
#define MIICONTROL_ELECTRICALLY_ISOLATE_PHY     BIT(10)
#define MIICONTROL_RESTART_AUTO_NEGOTIATION     BIT(9)
#define MIICONTROL_FULL_DUPLEX_MODE             BIT(8)
#define MIICONTROL_ENABLE_COLLISION_SIGNAL_TEST BIT(7)
#define MIICONTROL_SPEED_SELECTION_BIT0         BIT(6)

#define MIISTATUS_100MBS_T4_CAPABLE             BIT(15)
#define MIISTATUS_100MBS_X_FULL_DUPLEX_CAPABLE  BIT(14)
#define MIISTATUS_100MBS_X_HALF_DUPLEX_CAPABLE  BIT(13)
#define MIISTATUS_10MBS_FULL_DUPLEX_CAPABLE     BIT(12)
#define MIISTATUS_10MBS_HALF_DUPLEX_CAPABLE     BIT(11)
#define MIISTATUS_100MBS_T2_FULL_DUPLEX_CAPABLE BIT(10)
#define MIISTATUS_100MBS_T2_HALF_DUPLEX_CAPABLE BIT(9)
#define MIISTATUS_EXTENDED_STATUS_AVAILABLE     BIT(8)
#define MIISTATUS_PREAMBLE_SUPPRESSED_FRAME_OK  BIT(6)
#define MIISTATUS_AUTO_NEGOTIATION_COMPLETE     BIT(5)
#define MIISTATUS_REMOTE_FAULT_DETECTED         BIT(4)
#define MIISTATUS_AUTO_NEGOTIATION_CAPABLE      BIT(3)
#define MIISTATUS_LINK_IS_UP                    BIT(2)
#define MIISTATUS_JABBER_DETECTED               BIT(1)
#define MIISTATUS_EXTENDED_CAPABILITIES_PRESENT BIT(0)

#define MII4_MULTIPLE_PAGES                     BIT(15)
#define MII4_REMOTE_FAULT                       BIT(13)
#define MII4_ASYMETRIC_PAUSE                    BIT(11)
#define MII4_PAUSE                              BIT(10)
#define MII4_100BASE_T4                         BIT(9)
#define MII4_100BASE_T_FULL_DUPLEX              BIT(8)
#define MII4_100BASE_T_HALF_DUPLEX              BIT(7)
#define MII4_10BASE_T_FULL_DUPLEX               BIT(6)
#define MII4_10BASE_T_HALF_DUPLEX               BIT(5)

#define XNET_ETHERNET_LINK_ACTIVE               0x01    // Ethernet cable is connected and active
#define XNET_ETHERNET_LINK_100MBPS              0x02    // Ethernet link is set to 100 Mbps
#define XNET_ETHERNET_LINK_10MBPS               0x04    // Ethernet link is set to 10 Mbps
#define XNET_ETHERNET_LINK_FULL_DUPLEX          0x08    // Ethernet link is in full duplex mode
#define XNET_ETHERNET_LINK_HALF_DUPLEX          0x10    // Ethernet link is in half duplex mode

typedef struct                              // base address 0xFEF00000
{
    DWORD intr;                             // 000
        // Interrupt register
        //  bit 7: reserved
        //  bit 6 (MINT): MII interrupt
        //  bit 5 (STINT): software timer interrupt
        //  bit 4 (TCINT): transmit complete w/o error interrupt
        //  bit 3 (TEINT): transmit complete with error interrupt
        //  bit 2 (MISS): missed a frame
        //  bit 1 (RCINT): receive complete w/o error interrupt
        //  bit 0 (REINT): receive complete with error interrupt

    #define INTR_MINT   BIT(6)
    #define INTR_STINT  BIT(5)
    #define INTR_TCINT  BIT(4)
    #define INTR_TEINT  BIT(3)
    #define INTR_MISS   BIT(2)
    #define INTR_RCINT  BIT(1)
    #define INTR_REINT  BIT(0)

    #define INTR_ALL    (INTR_MINT | \
                         INTR_TCINT | \
                         INTR_TEINT | \
                         INTR_MISS | \
                         INTR_RCINT | \
                         INTR_REINT)
    
    DWORD intr_mk;                          // 004
        // Master interrupt mask
        //  bit 7: reserved
        //  bit 6: MINT mask - 1 = enable and 0 = disable
        //  bit 5: STINT mask
        //  bit 4: TCINT mask
        //  bit 3: TEINT mask
        //  bit 2: MISS mask
        //  bit 1: RCINT mask
        //  bit 0: REINT mask

    DWORD swtr_cntl;                        // 008
        // Software timer control register
        //  bit 1 (STEN): software timer enable
        //  bit 0 (STREN): software timer reload enable

    DWORD swtr_itc;                         // 00c
        // Software timer register
        //  bit 31-16: current software timer count
        //  bit 15-0: software timer interval

    BYTE gap1[0x80 - 0x10];

    DWORD tx_cntl;                          // 080
        // Transmit control register
        //  bit 31-22: reserved
        //  bit 21 (UFLOM): underflow error mask
        //  bit 20 (TCOLM): transmit late collision mask
        //  bit 19 (LCARM): loss of carrier mask
        //  bit 18 (DEFM): deferred mask
        //  bit 17 (EXDEFM): excessive deferral mask
        //  bit 16 (RTRYM): retry error mask
        //  bit 15-12: reserved
        //  bit 11-8: maximum number of retries on collisions
        //  bit 7-6: MAC-PHY interface
        //  bit 5 (TDEFEN): two-part deferral enable
        //  bit 4 (FCSEN): FCS append enable
        //  bit 3 (PADEN): pad enable
        //  bit 2 (RTRYEN): retry enable
        //  bit 1 (HDEN): half-duplex enable
        //  bit 0: reserved
    
    #define TXCNTL_UFLOM    BIT(21)
    #define TXCNTL_TCOLM    BIT(20)
    #define TXCNTL_LCARM    BIT(19)
    #define TXCNTL_DEFM     BIT(18)
    #define TXCNTL_EXDEFM   BIT(17)
    #define TXCNTL_RTRYM    BIT(16)
    #define TXCNTL_RCSHIFT  8
    #define TXCNTL_PHYSHIFT 6
    #define TXCNTL_TDEFEN   BIT(5)
    #define TXCNTL_FCSEN    BIT(4)
    #define TXCNTL_PADEN    BIT(3)
    #define TXCNTL_RTRYEN   BIT(2)
    #define TXCNTL_HDEN     BIT(1)

    #define PHY_TYPE_MII    0
    #define TXCNTL_DEFAULT  (TXCNTL_UFLOM | \
                             TXCNTL_TCOLM | \
                             TXCNTL_LCARM | \
                             TXCNTL_EXDEFM | \
                             TXCNTL_RTRYM | \
                             TXCNTL_TDEFEN | \
                             TXCNTL_FCSEN | \
                             TXCNTL_PADEN | \
                             TXCNTL_RTRYEN | \
                             TXCNTL_HDEN | \
                             (PHY_TYPE_MII << TXCNTL_PHYSHIFT) | \
                             (15 << TXCNTL_RCSHIFT))

    DWORD tx_en;                            // 084
        // Transmit enable register
        //  bit 1: transmit enable

    #define TXEN_ENABLE     BIT(0)

    DWORD tx_sta;                           // 088
        // Transmit status register
        //  bit 31-22: reserved
        //  bit 21 (UFLO): underflow error
        //  bit 20 (TCOL): transmit late collision
        //  bit 19 (LCAR): loss of carrier
        //  bit 18 (DEF): deferred
        //  bit 17 (EXDEF): excessive deferral
        //  bit 16 (RTRY): retry error
        //  bit 15-1: reserved
        //  bit 0: transmit channel idle status
    
    #define TXSTA_UFLO      BIT(21)
    #define TXSTA_TCOL      BIT(20)
    #define TXSTA_LCAR      BIT(19)
    #define TXSTA_DEF       BIT(18)
    #define TXSTA_EXDEF     BIT(17)
    #define TXSTA_RTRY      BIT(16)
    #define TXSTA_BUSY      BIT(0)

    DWORD rx_cntl_0;                        // 08c
        // Receive control
        //  bit 31-23: reserved
        //  bit 22 (FRAMM): frame alignment error mask
        //  bit 21 (OFOLM): overflow error mask
        //  bit 20 (CRCM): FCS error mask
        //  bit 19 (LFERM): length error mask
        //  bit 18 (MAXM): maximum length error mask
        //  bit 17 (RLCOLM): receive late collision mask
        //  bit 16 (RUNTM): runt receive mask
        //  bit 15-9: reserved
        //  bit 8 (RDEFEN): receive deferral enable
        //  bit 7 (BRDIS): broadcast receive disable
        //  bit 6 (RUNTEN): runt packet receive enable
        //  bit 5 (AFEN): address filtering enable
        //  bit 4 (LBEN): loopback enable
        //  bit 3 (PAEN): pause enable
        //  bit 2 (FCSREN): FCS relay enable
        //  bit 1 (PADSEN): pad strip enable
        //  bit 0: reserved

    #define RXCNTL_FRAMM    BIT(22)
    #define RXCNTL_OFLOM    BIT(21)
    #define RXCNTL_CRCM     BIT(20)
    #define RXCNTL_LFERM    BIT(19)
    #define RXCNTL_MAXM     BIT(18)
    #define RXCNTL_RLCOLM   BIT(17)
    #define RXCNTL_RUNTM    BIT(16)
    #define RXCNTL_RDEFEN   BIT(8)
    #define RXCNTL_BRDIS    BIT(7)
    #define RXCNTL_RUNTEN   BIT(6)
    #define RXCNTL_AFEN     BIT(5)
    #define RXCNTL_LBEN     BIT(4)
    #define RXCNTL_PAEN     BIT(3)
    #define RXCNTL_FCSREN   BIT(2)
    #define RXCNTL_PADSEN   BIT(1)

    #define RXCNTL_DEFAULT  (RXCNTL_FRAMM | \
                             RXCNTL_OFLOM | \
                             RXCNTL_CRCM | \
                             RXCNTL_LFERM | \
                             RXCNTL_MAXM | \
                             RXCNTL_RLCOLM | \
                             RXCNTL_RUNTM | \
                             RXCNTL_AFEN)

    DWORD rx_cntl_1;                        // 090
        // Maximum receive frame size register

    DWORD rx_en;                            // 094
        // Receive enable register
        //  bit 1: receive enable

    #define RXEN_ENABLE     BIT(0)

    DWORD rx_sta;                           // 098
        // Receive status register
        //  bit 31-23: reserved
        //  bit 22 (FRAM): frame alignment error
        //  bit 21 (OFOL): overflow error
        //  bit 20 (CRC): FCS error
        //  bit 19 (LFER): length error
        //  bit 18 (MAX): maximum length error
        //  bit 17 (RLCOL): receive late collision
        //  bit 16 (RUNT): runt receive
        //  bit 15-1: reserved
        //  bit 0: receive channel idle status
    
    #define RXSTA_FRAM      BIT(22)
    #define RXSTA_OFLO      BIT(21)
    #define RXSTA_CRC       BIT(20)
    #define RXSTA_LFER      BIT(19)
    #define RXSTA_MAX       BIT(18)
    #define RXSTA_RLCOL     BIT(17)
    #define RXSTA_RUNT      BIT(16)
    #define RXSTA_BUSY      BIT(0)

    DWORD bkoff_cntl;                       // 09c
        // Backoff control register (for HomePNA)
        //  bit 31-16: reserved
        //  bit 15-8: slot time, 127 for IEEE 802.3
        //  bit 7-0: random seed

    #define BKOFFCNTL_RSSHIFT   0
    #define BKOFFCNTL_STSHIFT   8

    #define BKOFFCNTL_DEFAULT   ((8 << BKOFFCNTL_RSSHIFT) | \
                                 (127 << BKOFFCNTL_STSHIFT))

    DWORD tx_def;                           // 0a0
        // Transmit deferral timing register
        //  bit 31-24: reserved
        //  bit 23-16 (TIFG): number of clocks for inter-frame gap
        //      when two-part deferral is disabled
        //  bit 15-8 (TIFG2): number of clocks for the second part
        //      inter-frame gap for two-part deferral
        //  bit 7-0 (TIFG1): number of clocks for the first part
        //      inter-frame gap for two-part deferral

    #define TXDEF_GSHIFT    16
    #define TXDEF_G2SHIFT   8
    #define TXDEF_G1SHIFT   0

    // Default values for IEEE 802.3
    #define TXDEF_DEFAULT   ((15 << TXDEF_G1SHIFT) | \
                             (7 << TXDEF_G2SHIFT) | \
                             (22 << TXDEF_GSHIFT))

    DWORD rx_def;                           // 0a4
        // Receive deferral register
        //  bit 7-0 (RIFG): number of clocks for inter-frame gap
        //      when receive deferral is enabled

    #define RXDEF_DEFAULT   0x16

    DWORD uni0;                             // 0a8
        // Lower 32-bits of the unicast address

    DWORD uni1;                             // 0ac
        // Higher 16-bits of the unicast address

    DWORD mult0;                            // 0b0
        // Lower 32-bits of the multicast address

    DWORD mult1;                            // 0b4
        // Higher 16-bits of the multicast address

    DWORD mult_mk0;                         // 0b8
        // Lower 32-bits of the multicast address mask

    DWORD mult_mk1;                         // 0bc
        // Higher 16-bits of the multicast address mask

    BYTE gap2[0x100 - 0xc0];

    DWORD tx_dadr;                          // 100
        // Transmit descriptor ring physical address

    DWORD rx_dadr;                          // 104
        // Receive descriptor ring physical address

    DWORD dlen;                             // 108
        // Descriptor ring length register
        //  bit 31-26: reserved
        //  bit 25-16 (RDLEN): receive descriptor block length (-1)
        //  bit 15-10: reserved
        //  bit 9-0 (TDLEN): transmit descriptor block length (-1)

    DWORD tx_poll;                          // 10c
        // Transmit descriptor poll register
        //  bit 31-17: reserved
        //  bit 16 (TPEN): transmit poll enable
        //  bit 15-0: transmit poll interval

    DWORD rx_poll;                          // 110
        // Receive descriptor poll register
        //  bit 31-17: reserved
        //  bit 16 (RPEN): receive poll enable
        //  bit 15-0: receive poll interval
        //      this is measured in 66MHz / 15ns clock cycles

    #define RXPOLL_EN           BIT(16)
    #define RXPOLL_FREQ_100MPS  100
    #define RXPOLL_FREQ_10MPS   1000

    DWORD tx_pcnt;                          // 114
        // Current transmit poll count

    DWORD rx_pcnt;                          // 118
        // Current receive poll count

    DWORD tx_cur_dadr;                      // 11c
        // Current transmit descriptor physical address

    DWORD rx_cur_dadr;                      // 120
        // Current receive descriptor physical address

    DWORD tx_cur_prd0;                      // 124
        // Current transmit physical address

    DWORD tx_cur_prd1;                      // 128
        // bit 31-16: current status of actively transmited frame
        // bit 15-0: current number of bytes remaining

    DWORD rx_cur_prd0;                      // 12c
        // Current receive physical address

    DWORD rx_cur_prd1;                      // 130
        // bit 31-16: current status of actively received frame
        // bit 15-0: current number of bytes remaining

    DWORD tx_nxt_dadr;                      // 134
        // Next transmit descriptor physical address

    DWORD rx_nxt_dadr;                      // 138
        // Next receive descriptor physical address

    DWORD tx_fifo_wm;                       // 13c
        // Transmit FIFO watermarks
        //  bit 31-24: reserved
        //  bit 23-16: High watermark
        //  bit 15-8: reserved
        //  bit 7-0: Low watermark

    #define TXFIFOWM_HWSHIFT    16
    #define TXFIFOWM_LWSHIFT    0

    #define TXFIFOWM_DEFAULT    ((0x10 << TXFIFOWM_LWSHIFT) | \
                                 (0x30 << TXFIFOWM_HWSHIFT))

    DWORD rx_fifo_wm;                       // 140
        // Receive FIFO watermarks
        //  bit 31-24: reserved
        //  bit 23-16: High watermark
        //  bit 15-8: reserved
        //  bit 7-0: Low watermark

    #define RXFIFOWM_HWSHIFT    16
    #define RXFIFOWM_LWSHIFT    0

    #define RXFIFOWM_DEFAULT    ((0x10 << RXFIFOWM_LWSHIFT) | \
                                 (0x30 << RXFIFOWM_HWSHIFT))
    DWORD mode;                             // 144
        // Mode register
        //  bit 4: buffer management reset
        //      need to be set for >= 3.2us before it's cleared
        //  bit 3: there is no active DMA transfer in progress
        //  bit 2: disable DMA transfer
        //  bit 1 (RXDM): receive demand
        //  bit 0 (TXDM): transmit demand

    #define MODE_RESET_BUFFERS  BIT(4)
    #define MODE_DMA_IDLE       BIT(3)
    #define MODE_DISABLE_DMA    BIT(2)
    #define MODE_RXDM           BIT(1)
    #define MODE_TXDM           BIT(0)

    BYTE gap3[0x180 - 0x148];

    DWORD mintr;                            // 180
        // MII interrupt register
        //  bit 7-5: reserved
        //  bit 4 (MPDI): MII PHY detect interrupt
        //  bit 3 (MAPI): MII auto-polling interrupt
        //  bit 2 (MCCI): MII command complete interrupt
        //  bit 1 (MCCII): MII command complete internal interrupt
        //  bit 0 (MREI): MII read error interrupt
    
    #define MINTR_MPDI  BIT(4)
    #define MINTR_MAPI  BIT(3)
    #define MINTR_MCCI  BIT(2)
    #define MINTR_MCCII BIT(1)
    #define MINTR_MREI  BIT(0)

    DWORD mintr_mk;                         // 184
        // MII interrupt mask register
        //  bit 7-5: reserved
        //  bit 4: MPDI mask - 1 to enable and 0 to disable
        //  bit 3: MAPI mask
        //  bit 2: MCCI mask
        //  bit 1: MCCII mask
        //  bit 0: MREI mask

    DWORD mii_cs;                           // 188
        // MII control and status register
        //  bit 31-29: reserved
        //  bit 28-24: PHY address
        //  bit 23-21: reserved
        //  bit 20 (APEN): auto-polling enable
        //  bit 19-16 (APTI): auto-polling time interval
        //  bit 15 (T4): 100BASE-T4
        //  bit 14 (XFD): 100BASE-X full duplex
        //  bit 13 (XHD): 100BASE-X half duplex
        //  bit 12 (10FD): 10Mb/s full-duplex
        //  bit 11 (10HD): 10Mb/s half-duplex
        //  bit 10 (ET2FD): 100BASE-T2 full-duplex
        //  bit 9 (T2HD): 100BASE-T2 half-duplex
        //  bit 8 (EXST): extended status
        //  bit 7: reserved
        //  bit 6 (MFPS): MF preamble suppression
        //  bit 5 (ANC): auto negotiation complete
        //  bit 4 (RF): remote fault
        //  bit 3 (ANA): auto-negotiation ability
        //  bit 2 (LS): link status
        //  bit 1 (JD): Jabber detect
        //  bit 0 (EC): extended capability
    
    #define MIICS_PADRSHIFT 24
    #define MIICS_APEN      BIT(20)
    #define MIICS_APSHIFT   16
    #define MIICS_T4        BIT(15)
    #define MIICS_XFD       BIT(14)
    #define MIICS_XHD       BIT(13)
    #define MIICS_10FD      BIT(12)
    #define MIICS_10HD      BIT(11)
    #define MIICS_ET2FD     BIT(10)
    #define MIICS_T2HD      BIT(9)
    #define MIICS_EXST      BIT(8)
    #define MIICS_MFPS      BIT(6)
    #define MIICS_ANC       BIT(5)
    #define MIICS_RF        BIT(4)
    #define MIICS_ANA       BIT(3)
    #define MIICS_LS        BIT(2)
    #define MIICS_JD        BIT(1)
    #define MIICS_EC        BIT(0)

    #define PHY_ADDR        1
    #define MIICS_DEFAULT   ((PHY_ADDR << MIICS_PADRSHIFT) | \
                             (4 << MIICS_APSHIFT) | \
                             MIICS_APEN)

    DWORD mii_tm;                           // 18c
        // MII clock timer register
        //  bit 15: MII timer status
        //  bit 14-9: reserved
        //  bit 8: MII timer enable
        //  bit 7-0: MII timer interval

    #define MIITM_BUSY      BIT(15)
    #define MIITM_EN        BIT(8)
    #define MIITM_TISHIFT   0

    #define MIITM_INTERVAL  5
    #define MIITM_DEFAULT   ((MIITM_INTERVAL << MIITM_TISHIFT) | MIITM_EN)
    #define PHYRW_TIMEOUT   ((64*2*2*400*MIITM_INTERVAL/1000)*16)

    DWORD mdio_adr;                         // 190
        // MDIO address register
        //  bit 15 (MDLK): MDIO lock
        //  bit 14-11: reserved
        //  bit 10 (MDRW): MDIO read/write
        //  bit 9-5 (PHYADR): physical address of the PHY to be accessed
        //  bit 4-0 (PHYREG): register address of the PHY to be accessed

    #define MDIOADR_LOCK        BIT(15)
    #define MDIOADR_WRITE       BIT(10)
    #define MDIOADR_PHYSHIFT    5
    #define MDIOADR_REGSHIFT    0

    DWORD mdio_data;                        // 194
        // MDIO data register
        //  bit 15-0 (PHYD): data for the last PHY read/write access

    BYTE gap4[0x200 - 0x198];

    DWORD pm_cntl;                          // 200
        // Power management control register

    struct {
        DWORD crc;
        DWORD mask0;
        DWORD mask1;
        DWORD mask2;
        DWORD mask3;
    } pmc_crc[5];
        // Pattern match CRC registers

    DWORD pmc_alias;
        // PCI power management register 0 alias

    DWORD pmcsr_alias;
        // PCI power management register 1 alias
} volatile * PNICCSR;

#define PNicCsr() ((PNICCSR)XPCICFG_NIC_MEMORY_REGISTER_BASE_0)

#endif // !_PHY_H_
