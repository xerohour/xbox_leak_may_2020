/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    phy.c

Abstract:

    Ethernet transceiver code inside the ROM

Revision History:

    04/5/2001 davidx
        Created it.
    06/5/2001 dinartem
        Moved to ntos\phy and made it independent of the xnet codebase

--*/

#include <stddef.h>
#include <ntos.h>
#include <nturtl.h>
#include <xtl.h>
#include "phy.h"

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

#define NETERR(_err)        HRESULT_FROM_WIN32(_err)
#define NETERR_OK           STATUS_SUCCESS
#define NETERR_HARDWARE     0x801f0001  // hardware not responding

BOOL PhyWriteReg(PNICCSR csr, DWORD phyreg, DWORD val);
BOOL PhyReadReg(PNICCSR csr, DWORD phyreg, DWORD* val);

BOOL
PhyUpdateLinkState(
    PNICCSR csr
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
        state |= XNET_ETHERNET_LINK_100MBPS;
    else if (anar & (MII4_10BASE_T_FULL_DUPLEX | MII4_10BASE_T_HALF_DUPLEX))
        state |= XNET_ETHERNET_LINK_10MBPS;

    if (anar & (MII4_10BASE_T_FULL_DUPLEX | MII4_100BASE_T_FULL_DUPLEX))
        state |= XNET_ETHERNET_LINK_FULL_DUPLEX;
    else if (anar & (MII4_10BASE_T_HALF_DUPLEX | MII4_100BASE_T_HALF_DUPLEX))
        state |= XNET_ETHERNET_LINK_HALF_DUPLEX;

    if (miiStatus & MIISTATUS_LINK_IS_UP)
        state |= XNET_ETHERNET_LINK_ACTIVE;

    PhyLinkState = state;
    return TRUE;
}


//
// Wait for up to 500ms until the link to be up.
//
DWORD PhyWaitForLinkUp(PNICCSR csr)
{
    DWORD miiStatus = 0;
    INT timeout = 1000;
    while (timeout-- && !(miiStatus & MIISTATUS_LINK_IS_UP)) {
        KeStallExecutionProcessor(500);
        if (!PhyReadReg(csr, MIIREG_STATUS, &miiStatus)) break;
    }
    return miiStatus;
}


// Clear MDIOADR_LOCK bit
VOID PhyClearMDIOLOCK(PNICCSR csr)
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


BOOL
PhyReadReg(
    PNICCSR csr,
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


BOOL
PhyWriteReg(
    PNICCSR csr,
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
    PNICCSR csr = PNicCsr();
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
            PhyLinkState |= XNET_ETHERNET_LINK_100MBPS;
        } else if (miiStatus & MIISTATUS_10MBS_HALF_DUPLEX_CAPABLE) {
            // We can do 10Mbps
            miiControl &= ~MIICONTROL_SPEED_SELECTION_BIT1;
            miiControl |=  MIICONTROL_SPEED_SELECTION_BIT0;
            PhyLinkState |= XNET_ETHERNET_LINK_10MBPS;
        } else
            goto err;

        PhyLinkState |= XNET_ETHERNET_LINK_HALF_DUPLEX;

        // Set the desired speed if the auto-negotiation never completed
        PhyWriteReg(csr, MIIREG_CONTROL, miiControl);

        miiStatus = PhyWaitForLinkUp(csr);
        if (miiStatus & MIISTATUS_LINK_IS_UP)
            PhyLinkState |= XNET_ETHERNET_LINK_ACTIVE;
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
        PhyUpdateLinkState(PNicCsr());
        PhyUnlock();
    }
    return PhyLinkState;
}
