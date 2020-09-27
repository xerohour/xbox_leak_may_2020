/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    phy.c

Abstract:

    Ethernet transceiver code inside the ROM

Revision History:

    04//5/2001 davidx
        Created it.

--*/

#include "precomp.h"
#include "phy.h"
#include "nettypes.h"
#include "netutil.h"
#include "ethernet.h"
#ifdef SILVER
#include "i82558.h"
#else
#include "xnic.h"
#endif

//
// Global variables
//  NOTE: The init count persists across quick reboots.
//
DECLSPEC_STICKY DWORD PhyInitFlag;
DWORD PhyLinkState;
LONG PhyLockFlag;

#define PhyLock() InterlockedCompareExchange(&PhyLockFlag, 1, 0)
#define PhyUnlock() (PhyLockFlag = 0)

//
// Macro for spewing debug message
//
#if DBG
BOOL PhyVerboseMode = TRUE;
#define WARNING_ DbgPrint
#define VERBOSE_ !PhyVerboseMode ? (void)0 : (void)DbgPrint
#else !DBG
#define WARNING_ 1 ? (void)0 : (void)
#define VERBOSE_ 1 ? (void)0 : (void)
#endif

//
// Don't declaration private functions as static
//
#ifdef PRIVATE
#undef PRIVATE
#define PRIVATE
#endif

PRIVATE BOOL PhyWriteReg(PNIC_CSR csr, DWORD phyreg, DWORD val);
PRIVATE BOOL PhyReadReg(PNIC_CSR csr, DWORD phyreg, DWORD* val);

PRIVATE BOOL
PhyUpdateLinkState(
    PNIC_CSR csr
    )

/*++

Routine Description:

    Update PHY link state information
    (read the information from the PHY registers)

Arguments:

    csr - Points to the NIC registers

Return Value:

    TRUE if successful, FALSE if there is an error

--*/

{
    DWORD anar, lpanar, miiStatus, state = 0;

    if (!PhyReadReg(csr, MIIREG_ANAR, &anar) ||
        !PhyReadReg(csr, MIIREG_LPANAR, &lpanar) ||
        !PhyReadReg(csr, MIIREG_STATUS, &miiStatus))
        return FALSE;

    anar &= lpanar;
    if (anar & (MII4_100BASE_T_FULL_DUPLEX | MII4_100BASE_T_HALF_DUPLEX))
        state |= XNET_LINK_100MBPS;
    else if (anar & (MII4_10BASE_T_FULL_DUPLEX | MII4_10BASE_T_HALF_DUPLEX))
        state |= XNET_LINK_10MBPS;

    if (anar & (MII4_10BASE_T_FULL_DUPLEX | MII4_100BASE_T_FULL_DUPLEX))
        state |= XNET_LINK_FULL_DUPLEX;
    else if (anar & (MII4_10BASE_T_HALF_DUPLEX | MII4_100BASE_T_HALF_DUPLEX))
        state |= XNET_LINK_HALF_DUPLEX;

    if (miiStatus & MIISTATUS_LINK_IS_UP)
        state |= XNET_LINK_IS_UP;

    PhyLinkState = state;
    return TRUE;
}


//
// Wait for up to 500ms until the link to be up.
//
INLINE DWORD PhyWaitForLinkUp(PNIC_CSR csr) {
    DWORD miiStatus = 0;
    INT timeout = 1000;
    while (timeout-- && !(miiStatus & MIISTATUS_LINK_IS_UP)) {
        KeStallExecutionProcessor(500);
        if (!PhyReadReg(csr, MIIREG_STATUS, &miiStatus)) break;
    }
    return miiStatus;
}


#ifndef SILVER

// Clear MDIOADR_LOCK bit
PRIVATE VOID PhyClearMDIOLOCK(PNIC_CSR csr)
{
    INT timeout;

    csr->mdio_adr = MDIOADR_LOCK;
    WARNING_("PHY: MDIOADR_LOCK is set\n");

    timeout = PHYRW_TIMEOUT;
    do {
        KeStallExecutionProcessor(50);
        timeout -= 50;
    } while (timeout > 0 && (csr->mdio_adr & MDIOADR_LOCK));
}


PRIVATE BOOL
PhyReadReg(
    PNIC_CSR csr,
    DWORD phyreg,
    DWORD* val
    )

/*++

Routine Description:

    Read the value of a PHY register

Arguments:

    csr - Points to the NIC registers
    phyreg - Specifies the PHY register to be read
    val - Return the PHY register value

Return Value:

    TRUE if successful, FALSE if there is an error

--*/

{
    DWORD mdioadr;
    INT timeout;

    // The lock bit shouldn't be set.
    // Clear it just in case it's stuck.

    if (csr->mdio_adr & MDIOADR_LOCK) {
        PhyClearMDIOLOCK(csr);
    }

    // Write the PHY register address

    mdioadr = (PHY_ADDR << MDIOADR_PHYSHIFT) | (phyreg << MDIOADR_REGSHIFT);
    csr->mdio_adr = mdioadr;

    mdioadr |= MDIOADR_LOCK;
    for (timeout=PHYRW_TIMEOUT; timeout > 0 && (mdioadr & MDIOADR_LOCK); timeout -= 50) {
        KeStallExecutionProcessor(50);
        mdioadr = csr->mdio_adr;
    }

    // Read the PHY register value
    *val = csr->mdio_data;

    if (mdioadr & MDIOADR_LOCK) {
        WARNING_("PHY read failed: reg %d.\n", phyreg);
        ASSERT(FALSE);
        return FALSE;
    }

    return TRUE;
}


PRIVATE BOOL
PhyWriteReg(
    PNIC_CSR csr,
    DWORD phyreg,
    DWORD val
    )

/*++

Routine Description:

    Write the specified value to a PHY register

Arguments:

    csr - Points to the NIC registers
    phyreg - Specifies the PHY register to be written
    val - Specifies the value for the PHY register

Return Value:

    TRUE if successful, FALSE if there is an error

--*/

{
    DWORD mdioadr;
    INT timeout;

    // The lock bit shouldn't be set.
    // Clear it just in case it's stuck.

    if (csr->mdio_adr & MDIOADR_LOCK) {
        PhyClearMDIOLOCK(csr);
    }

    // Write the data first

    csr->mdio_data = val;

    // Write the PHY register address

    mdioadr = (PHY_ADDR << MDIOADR_PHYSHIFT) | (phyreg << MDIOADR_REGSHIFT) | MDIOADR_WRITE;
    csr->mdio_adr = mdioadr;

    mdioadr |= MDIOADR_LOCK;
    for (timeout=PHYRW_TIMEOUT; timeout > 0 && (mdioadr & MDIOADR_LOCK); timeout -= 50) {
        KeStallExecutionProcessor(50);
        mdioadr = csr->mdio_adr;
    }

    if (mdioadr & MDIOADR_LOCK) {
        WARNING_("PHY write failed: reg %d.\n", phyreg);
        ASSERT(FALSE);
        return FALSE;
    }

    return TRUE;
}


NTSTATUS
PhyInitialize(
    BOOL forceReset,
    VOID* param OPTIONAL
    )

/*++

Routine Description:

    Initialize the Ethernet PHY interface

Arguments:

    forceReset - Whether to force a PHY reset
    param -optional parameters

Return Value:

    Status code

--*/

{
    PNIC_CSR csr = NicCsr;
    DWORD miiControl, miiStatus;
    INT timeout;
    NTSTATUS status = NETERR_HARDWARE;

    if (PhyLock() != 0)
        return NETERR(ERROR_BUSY);
    
    if (forceReset) {
        PhyInitFlag = 0;
        PhyLinkState = 0;

        //
        // Force the PHY to reset
        //
        miiControl = MIICONTROL_RESET;
        if (!PhyWriteReg(csr, MIIREG_CONTROL, miiControl)) goto err;

        // Wait for up to 500ms
        timeout = 1000;
        while (timeout-- && (miiControl & MIICONTROL_RESET)) {
            KeStallExecutionProcessor(500);
            if (!PhyReadReg(csr, MIIREG_CONTROL, &miiControl)) goto err;
        }

        // If the reset is still asserted, return error
        if (miiControl & MIICONTROL_RESET) goto err;
    } else if (PhyInitFlag) {
        //
        // If PHY is already initialized, just update the link state
        //
        PhyUpdateLinkState(csr);
        status = NETERR_OK;
        goto exit;
    }

    // The auto-negotiation should be started by now.
    // Wait for a max of 3 seconds for it to complete.
    timeout = 6000;
    miiStatus = 0;
    while (timeout-- && !(miiStatus & MIISTATUS_AUTO_NEGOTIATION_COMPLETE)) {
        KeStallExecutionProcessor(500);
        if (!PhyReadReg(csr, MIIREG_STATUS, &miiStatus)) goto err;
    }

    // NOTE: Workaround for ICS PHY problems with some 10base-t hubs
    //  e.g. Garret Communications Magnum Personal Hub H50
    if (XboxHardwareInfo.McpRevision != 0xa1) {
        DWORD icshack;
        // clear bit 8 of undocumented register 0x18
        if (PhyReadReg(csr, 0x18, &icshack)) {
            icshack &= ~0x0100;
            PhyWriteReg(csr, 0x18, icshack);
        }
    }

    //
    // Use auto-negotiation
    //
    if (!PhyReadReg(csr, MIIREG_CONTROL, &miiControl)) goto err;

    if (miiControl & MIICONTROL_RESTART_AUTO_NEGOTIATION) {
        // If the restart-auto-negotiation bit is set,
        // default to the highest available speed in half-duplex mode.
        WARNING_("Auto-negotiation didn't succeed.\n");

        if (miiStatus & (MIISTATUS_100MBS_T4_CAPABLE |
                         MIISTATUS_100MBS_X_HALF_DUPLEX_CAPABLE |
                         MIISTATUS_100MBS_T2_HALF_DUPLEX_CAPABLE)) {
            // We can do 100Mbps
            miiControl |=  MIICONTROL_SPEED_SELECTION_BIT1;
            miiControl &= ~MIICONTROL_SPEED_SELECTION_BIT0;
            PhyLinkState |= XNET_LINK_100MBPS;
        } else if (miiStatus & MIISTATUS_10MBS_HALF_DUPLEX_CAPABLE) {
            // We can do 10Mbps
            miiControl &= ~MIICONTROL_SPEED_SELECTION_BIT1;
            miiControl |=  MIICONTROL_SPEED_SELECTION_BIT0;
            PhyLinkState |= XNET_LINK_10MBPS;
        } else
            goto err;

        PhyLinkState |= XNET_LINK_HALF_DUPLEX;

        // Set the desired speed if the auto-negotiation never completed
        PhyWriteReg(csr, MIIREG_CONTROL, miiControl);

        miiStatus = PhyWaitForLinkUp(csr);
        if (miiStatus & MIISTATUS_LINK_IS_UP)
            PhyLinkState |= XNET_LINK_IS_UP;
    } else {
        // Auto-negotiation worked.
        PhyWaitForLinkUp(csr);
        if (!PhyUpdateLinkState(csr)) goto err;
    }

    PhyInitFlag = 1;
    status = NETERR_OK;

exit:
    PhyUnlock();
    return status;

err:
    WARNING_("Ethernet PHY initialization failed.\n");
    goto exit;
}


//
// Public function for retrieving link state information
//
DWORD PhyGetLinkState(BOOL update)
{
    if ((!PhyLinkState || update) && PhyLock() == 0) {
        PhyUpdateLinkState(NicCsr);
        PhyUnlock();
    }
    return PhyLinkState;
}

#else // !SILVER

PRIVATE BOOL
PhyReadReg(
    PNIC_CSR csr,
    DWORD phyreg,
    DWORD* val
    )

/*++

Routine Description:

    Read the value of a PHY register

Arguments:

    csr - Points to the NIC registers
    phyreg - Specifies the PHY register to be read
    val - Return the PHY register value

Return Value:

    TRUE if successful, FALSE if there is an error

--*/

{
    DWORD mdictrl;
    INT timeout;

    if (!(csr->mdiCtrl & MDI_READY)) {
        WARNING_("PHY isn't ready.\n");
    }

    // Initiate the PHY read
    mdictrl = MDI_PHY_REG_ADDR(phyreg) | MDI_PHY_ADDR(1) | MDIOP_READ;
    csr->mdiCtrl = mdictrl;

    // Wait for max 10msecs
    for (timeout = 2000; timeout > 0 && !(mdictrl & MDI_READY); timeout -= 50) {
        KeStallExecutionProcessor(50);
        mdictrl = csr->mdiCtrl;
    }

    if (mdictrl & MDI_READY) {
        *val = (mdictrl & 0xffff);
        return TRUE;
    }
    
    WARNING_("PHY read failed: reg %d.\n", phyreg);
    return FALSE;
}


PRIVATE BOOL
PhyWriteReg(
    PNIC_CSR csr,
    DWORD phyreg,
    DWORD val
    )

/*++

Routine Description:

    Write the specified value to a PHY register

Arguments:

    csr - Points to the NIC registers
    phyreg - Specifies the PHY register to be written
    val - Specifies the value for the PHY register

Return Value:

    TRUE if successful, FALSE if there is an error

--*/

{
    DWORD mdictrl;
    INT timeout;

    if (!(csr->mdiCtrl & MDI_READY)) {
        WARNING_("PHY isn't ready.\n");
    }

    // Initiate the PHY write
    mdictrl = MDI_PHY_REG_ADDR(phyreg) | MDI_PHY_ADDR(1) | MDIOP_WRITE | val;
    csr->mdiCtrl = mdictrl;

    // Wait for max 10msecs
    for (timeout = 2000; timeout > 0 && !(mdictrl & MDI_READY); timeout -= 50) {
        KeStallExecutionProcessor(50);
        mdictrl = csr->mdiCtrl;
    }

    if (mdictrl & MDI_READY)
        return TRUE;

    WARNING_("PHY write failed: reg %d.\n", phyreg);
    return FALSE;
}

//
// NOTE: PHY related functions are not fully implemented on the silver xdk box.
//  It's going away anyhow.
//
DWORD PhyGetLinkState(BOOL update)
{
    return PhyLinkState;
}

NTSTATUS PhyInitialize(BOOL forceReset, VOID* param)
{
    PNIC_CSR csr;

    if (PhyLock() != 0)
        return NETERR(ERROR_BUSY);
    
    // On silver xdk box, param points to the NIC CSR register space
    ASSERT(param != NULL && !forceReset);
    csr = (PNIC_CSR) param;

    // Just wait for up to half a second until the link is up
    // and then update the link status
    PhyWaitForLinkUp(csr);
    PhyUpdateLinkState(csr);
    
    PhyInitFlag = 1;
    PhyUnlock();
    return NETERR_OK;
}

#endif // !SILVER

