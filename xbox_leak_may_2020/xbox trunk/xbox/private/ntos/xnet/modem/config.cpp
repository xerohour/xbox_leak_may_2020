/*++

Copyright (c) 2000 Microsoft Corporation

    usb.cpp

Abstract:
    
    USB code to talk to the modem.
    
Revision History:

    06-21-00    vadimg      created

--*/

#include "precomp.h"

/***************************************************************************\
* CloseEndpoints3
*
\***************************************************************************/

VOID CloseEndpoints3(PURB purb, PVOID)
{
    ASSERT_DISPATCH_LEVEL();

    if (gfRemoved) {
        gDevice.pDevice->RemoveComplete();
        gDevice.pDevice = NULL;
        gfRemoved = FALSE;
    }
}

/***************************************************************************\
* CloseEndpoints2
*
\***************************************************************************/

VOID CloseEndpoints2(PURB purb, PVOID)
{
    ASSERT_DISPATCH_LEVEL();

    if (gDevice.pIntInEndpointHandle != NULL) {

        USB_BUILD_CLOSE_ENDPOINT(
                (PURB_CLOSE_ENDPOINT)purb,
                gDevice.pIntInEndpointHandle,
                (PURB_COMPLETE_PROC)CloseEndpoints3,
                NULL);

        gDevice.pDevice->SubmitRequest((PURB)purb);

        gDevice.pIntInEndpointHandle = NULL;
    }
}

/***************************************************************************\
* CloseEndpoints1
*
\***************************************************************************/

VOID CloseEndpoints1(PURB purb, PVOID)
{
    ASSERT_DISPATCH_LEVEL();

    if (gDevice.pBulkOutEndpointHandle != NULL) {

        USB_BUILD_CLOSE_ENDPOINT(
                (PURB_CLOSE_ENDPOINT)purb,
                gDevice.pBulkOutEndpointHandle,
                (PURB_COMPLETE_PROC)CloseEndpoints2,
                NULL);

        gDevice.pDevice->SubmitRequest((PURB)purb);

        gDevice.pBulkOutEndpointHandle = NULL;
    }
}

/***************************************************************************\
* CloseEndpoints
*
\***************************************************************************/

VOID CloseEndpoints(VOID)
{
    PURB purb = &gUrb;

    ASSERT_DISPATCH_LEVEL();

    if (gDevice.pBulkInEndpointHandle != NULL) {

        USB_BUILD_CLOSE_ENDPOINT(
                (PURB_CLOSE_ENDPOINT)purb,
                gDevice.pBulkInEndpointHandle,
                (PURB_COMPLETE_PROC)CloseEndpoints1,
                NULL);

        gDevice.pDevice->SubmitRequest((PURB)purb);

        gDevice.pBulkInEndpointHandle = NULL;
    }
}

/***************************************************************************\
* OpenEndpoints
*
\***************************************************************************/

BOOL OpenEndpoints(VOID)
{
    USBD_STATUS status;
    PURB_OPEN_ENDPOINT purb = (PURB_OPEN_ENDPOINT)&gUrb;

    ASSERT_DISPATCH_LEVEL();

    if (gDevice.pBulkInEndpointHandle == NULL) {

        USB_BUILD_OPEN_ENDPOINT(
                purb,
                gDevice.bBulkInEndpointAddress,
                USB_ENDPOINT_TYPE_BULK,
                gDevice.wBulkInMaxPacket,
                2);
    
        status = gDevice.pDevice->SubmitRequest((PURB)purb);
        if (USBD_SUCCESS(status)) {
            gDevice.pBulkInEndpointHandle = purb->EndpointHandle;
        } else {
            return FALSE;
        }
    }

    if (gDevice.pBulkOutEndpointHandle == NULL) {

        USB_BUILD_OPEN_ENDPOINT(
                purb,
                gDevice.bBulkOutEndpointAddress,
                USB_ENDPOINT_TYPE_BULK,
                gDevice.wBulkOutMaxPacket,
                2);
    
        status = gDevice.pDevice->SubmitRequest((PURB)purb);
        if (USBD_SUCCESS(status)) {
            gDevice.pBulkOutEndpointHandle = purb->EndpointHandle;
        } else {
            return FALSE;
        }
    }

    if (gDevice.pIntInEndpointHandle == NULL) {

        USB_BUILD_OPEN_ENDPOINT(
                purb,
                gDevice.bIntInEndpointAddress,
                USB_ENDPOINT_TYPE_INTERRUPT,
                gDevice.wIntInMaxPacket,
                2);
    
        status = gDevice.pDevice->SubmitRequest((PURB)purb);
        if (USBD_SUCCESS(status)) {
            gDevice.pIntInEndpointHandle = purb->EndpointHandle;
        } else {
            return FALSE;
        }
    }

    return TRUE;
}

/***************************************************************************\
* EnumSetConfiguration1
*
\***************************************************************************/

VOID EnumSetConfiguration1(PURB purb, IUsbDevice *pDevice)
{
    ASSERT_DISPATCH_LEVEL();

    if (USBD_ERROR(purb->Header.Status)) {
        pDevice->AddComplete(USBD_STATUS_UNSUPPORTED_DEVICE, NULL);
        return;
    }

    gDevice.pDevice = pDevice;
    pDevice->SetClassSpecificType(0);
    pDevice->AddComplete(USBD_STATUS_SUCCESS, NULL);
}

/***************************************************************************\
* EnumGetConfiguration1
*
\***************************************************************************/

VOID EnumGetConfiguration1(PURB purb, IUsbDevice *pDevice)
{
    PUSB_COMMON_DESCRIPTOR pCommonDesc;
    PUSB_CONFIGURATION_DESCRIPTOR pConfigDesc;
    PUSB_INTERFACE_DESCRIPTOR pInterDesc;
    PUSB_ENDPOINT_DESCRIPTOR pEndDesc;
    UCHAR bType, bNumInter, bNumEnd, iInter, iEnd;
    PUCHAR pParse;
    BOOL fCommFound = FALSE;

    ASSERT_DISPATCH_LEVEL();

    if (USBD_ERROR(purb->Header.Status)) {
        pDevice->AddComplete(USBD_STATUS_UNSUPPORTED_DEVICE, NULL);
        return;
    }

    pConfigDesc = (PUSB_CONFIGURATION_DESCRIPTOR)&gMem.ConfigDesc;

    // pointer to the first interface
    pParse = (PUCHAR)pConfigDesc + pConfigDesc->bLength;
    pCommonDesc = (PUSB_COMMON_DESCRIPTOR)pParse;
    iInter = iEnd = 0;
    bNumInter = pConfigDesc->bNumInterfaces;

    while (TRUE) {

        if (pCommonDesc->bDescriptorType == USB_INTERFACE_DESCRIPTOR_TYPE) {

            pInterDesc = (PUSB_INTERFACE_DESCRIPTOR)pCommonDesc;
            bNumEnd = pInterDesc->bNumEndpoints;
            iInter++;
            iEnd = 0;

            if (pInterDesc->bInterfaceClass == USB_COMM_COMMUNICATION_CLASS_CODE) {
                gDevice.bCommInterface = pInterDesc->bInterfaceNumber;
                fCommFound = TRUE;
            }

            if ((iInter == bNumInter) && (bNumEnd == 0)) {
                break;
            }

        } else if (pCommonDesc->bDescriptorType == USB_ENDPOINT_DESCRIPTOR_TYPE) {

            pEndDesc = (PUSB_ENDPOINT_DESCRIPTOR)pCommonDesc;
            bType = (pEndDesc->bmAttributes & USB_ENDPOINT_TYPE_MASK);
            iEnd++;

            if (USB_ENDPOINT_DIRECTION_IN(pEndDesc->bEndpointAddress)) {
                if (bType == USB_ENDPOINT_TYPE_BULK) {
                    gDevice.bBulkInEndpointAddress = pEndDesc->bEndpointAddress;
                    gDevice.wBulkInMaxPacket = pEndDesc->wMaxPacketSize;
                    gDevice.pBulkInEndpointHandle = NULL;
                } else if (bType == USB_ENDPOINT_TYPE_INTERRUPT) {
                    gDevice.bIntInEndpointAddress = pEndDesc->bEndpointAddress;
                    gDevice.wIntInMaxPacket = pEndDesc->wMaxPacketSize;
                    gDevice.pIntInEndpointHandle = NULL;
                }
            } else {
                if (bType == USB_ENDPOINT_TYPE_BULK) {
                    gDevice.bBulkOutEndpointAddress = pEndDesc->bEndpointAddress;
                    gDevice.wBulkOutMaxPacket = pEndDesc->wMaxPacketSize;
                    gDevice.pBulkOutEndpointHandle = NULL;
                }
            }

            if ((iInter == bNumInter) && (iEnd == bNumEnd)) {
                break;
            }
        } 

        pParse += pCommonDesc->bLength;
        pCommonDesc = (PUSB_COMMON_DESCRIPTOR)pParse;
    }

    if (!fCommFound) {
        pDevice->AddComplete(USBD_STATUS_UNSUPPORTED_DEVICE, NULL);
        return;
    }

    USB_BUILD_SET_CONFIGURATION(
            (PURB_CONTROL_TRANSFER)purb,
            pConfigDesc->bConfigurationValue,
            (PURB_COMPLETE_PROC)EnumSetConfiguration1,
            (PVOID)pDevice);

    ((PURB_CONTROL_TRANSFER)purb)->InterruptDelay = 3;
    pDevice->SubmitRequest(purb);
}

/***************************************************************************\
* ConfigureDevice
*
\***************************************************************************/

VOID ConfigureDevice(IUsbDevice *pDevice)
{
    PURB purb = &gUrb;

    ASSERT_DISPATCH_LEVEL();

    DbgPrint("xmodem: ConfigureDevice\n");

    USB_BUILD_OPEN_DEFAULT_ENDPOINT((PURB_OPEN_ENDPOINT)purb);
    pDevice->SubmitRequest(purb);

    if (USBD_ERROR(purb->Header.Status)) {
        pDevice->AddComplete(USBD_STATUS_UNSUPPORTED_DEVICE, NULL);
        return;
    }

    MmInitializeMdl(gpMdl, &gMem, sizeof(gMem));

    USB_BUILD_GET_DESCRIPTOR(
            (PURB_CONTROL_TRANSFER)purb,
            USB_CONFIGURATION_DESCRIPTOR_TYPE,
            1,
            0,
            gpMdl,
            CONFIG_DESC_SIZE,
            (PURB_COMPLETE_PROC)EnumGetConfiguration1,
            (PVOID)pDevice);

    ((PURB_CONTROL_TRANSFER)purb)->InterruptDelay = 3;
    pDevice->SubmitRequest(purb);
}

