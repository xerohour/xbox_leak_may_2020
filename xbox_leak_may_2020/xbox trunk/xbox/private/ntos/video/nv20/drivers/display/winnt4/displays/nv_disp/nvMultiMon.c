/************************** Module Header *************************************
//                                                                             *
//  Module Name: nvMultiMon.c                                                  *
//                                                                             *
//  This module contains the support functions for the TwinView multi-mon code *
//  driver, the pdev, and the surface.                                         *
//                                                                             *
//  Copyright (c) 1992-1996 Microsoft Corporation                              *
//                                                                             *
//******************************************************************************

/*******************************************************************************
 *                                                                             *
 *   Copyright (C) 1997 NVidia Corporation. All Rights Reserved.               *
 *                                                                             *
 ******************************************************************************/

#include "precomp.h"
#include "driver.h"

#include "excpt.h"
#include "nv32.h"
#include "nv1c_ref.h"
#include "nvalpha.h"
#include "nvsubch.h"

#include "nvos.h"
#include "nvntioctl.h"
#include "nvntddctl.h"
#include "nvapi.h"

#include "ddminint.h"
#include "dmamacro.h"
#include "nvcm.h"
#include "oglDD.h"
#include "nvReg.h"

// To get the NV11 twinview definitions
#include "nvMultiMon.h"

// public prototypes
void UseOnlyGDIDataForModeDetails(PDEV *ppdev);
void UseOnlyRegistryDataForModeDetails(PDEV *ppdev, NVTWINVIEWDATA *pData);
BOOL InitMultiMon(PDEV *ppdev);
void GetDefaultTVFormat(PPDEV ppdev);
void FreeMemoryMultiMon(PDEV *ppdev);
BOOLEAN TwinViewDataOK(PPDEV ppdev, NVTWINVIEWDATA *pData);
BOOLEAN bQuerySupportPhysicalMode(PPDEV ppdev, ULONG head, ULONG ulDeviceMask, MODE_ENTRY *RequestedMode, MODE_ENTRY *BestFitMode);
VOID vBackOffMode(PPDEV ppdev, ULONG ulHead, ULONG ulDeviceMask, MODE_ENTRY *RequestedMode, MODE_ENTRY *BestFitMode);
BOOLEAN bQueryBothPanelsAreInSync(PPDEV ppdev, NVTWINVIEWDATA *pData);
BOOLEAN bConfirmDeviceType(PPDEV ppdev, ULONG Head, ULONG RequestedDeviceMask, ULONG RequestedDeviceType, ULONG RequestedTVType,
                           ULONG * SuggestedDeviceMask, ULONG * SuggestedDeviceType, ULONG * SuggestedTVType);
void PrintPPDEVTwinViewInfo (char *Msg, PPDEV ppdev);
void PrintTwinViewData (char *Msg, ULONG ulDebugLevel, NVTWINVIEWDATA *pData, PPDEV ppdev);
BOOLEAN bGetPhysicalVirtualModeList(PPDEV ppdev, ULONG Head, ULONG DeviceMask, ULONG DeviceType, ULONG TVFormat,
        MODE_ENTRY *PhysModeList, MODE_ENTRY *VirtModeList, ULONG *NumPhysModes, ULONG *NumVirtModes);
void InduceAndUpdateDeviceScanning (PPDEV ppdev);
void   ProcessSynchronousFlags(PPDEV ppdev, NVTWINVIEWDATA *pData);
BOOL BuildModeLists(PPDEV ppdev);
ULONG ValidateRegistryDeviceMasks(PPDEV ppdev, NVTWINVIEWDATA *pData);
VOID ReadToshibaHotKeyMode(HANDLE      hDriver, TOSHIBA_HOTKEY_MODE *pMode);
VOID WriteToshibaHotKeyMode(HANDLE      hDriver, TOSHIBA_HOTKEY_MODE *pMode);


VOID WriteRegistryTwinViewInfo (PPDEV ppdev, NVTWINVIEWDATA *pData);
ULONG GetNumberDacsActiveFromTwinViewInfo (NVTWINVIEWDATA *pData);
VOID InitDevMaskFromTwinViewInfo (PPDEV ppdev, NVTWINVIEWDATA *pData);

ULONG MaskToDeviceType( PPDEV ppdev, ULONG ulDeviceMask);
ULONG TVFormatToTVDeviceType(ULONG ulTVFormat);
ULONG ConvertDeviceMaskToIndex(
    ULONG ulDeviceMask);
ULONG ConvertConnectedMaskToNumDevices(
    ULONG ulConnectedDeviceMask);
ULONG HandleBIOSHeads(PPDEV ppdev);
VOID HandleBandWidth_SingleDacActive(PPDEV ppdev);
VOID HandleBandWidth_DualDacsActive(PPDEV ppdev);

BOOLEAN FindNextLowerRefreshRateMode(PPDEV ppdev, ULONG Head, MODE_ENTRY *pRequestedMode, ULONG *pNextLowerRefreshRate);
BOOLEAN FindNextLowerResolutionMode(PPDEV ppdev, ULONG Head, MODE_ENTRY *pRequestedMode, ULONG *pNextLowerWidth, ULONG *pNextLowerHeight);

BOOLEAN bQueryGDIModePruning(ULONG ulTwinView_State, ULONG ulPrimaryHead, ULONG ulPrimaryDeviceMask,
          ULONG ulHead, ULONG ulDeviceMask, ULONG ulACPISystem, ULONG ulGDIModePruning);

/*  Reg utils */

BOOLEAN GetULONGRegValue(PPDEV ppdev, char *pcKeyName, ULONG *pulData);
VOID SetULONGRegValue(PPDEV ppdev, char *pcKeyName, ULONG ulData);

// The next two functions are used 
// as wrapper to do an IOCTL reg key read/write (it wraps the NV_REG struct setup)
// ( *W functions uses unicode strings ( L"abcde..." ) )

void GetRegValueW(HANDLE hDriver, PWSTR keyName, PVOID keyData, ULONG keyDataSize);
void SetRegValueW(HANDLE hDriver, PWSTR keyName, PVOID keyData, ULONG keyDataSize);

/* ... Reg Utils */


ULONG GetInitiatedRegValue(PPDEV ppdev);
VOID SetInitiatedRegValue(PPDEV ppdev, ULONG ulVal);
ULONG GetPowerStateRegValue(PPDEV ppdev);
VOID SetPowerStateRegValue(PPDEV ppdev, ULONG ulVal);
ULONG GetPrevDevMaskRegValue(PPDEV ppdev);
VOID SetPrevDevMaskRegValue(PPDEV ppdev, ULONG ulVal);

VOID vReadSaveSettings(PPDEV ppdev);
VOID vWriteSaveSettings(PPDEV ppdev);
NVTWINVIEWDATA *pFindSaveSettings(PPDEV ppdev, QUERY_SAVE_SETTINGS *pQuery, NVTWINVIEWDATA * pTwinViewData);
VOID vPrintSaveSettings(PPDEV ppdev, ULONG ulDebugLevel);


#if DBG
extern ULONG DebugLevel;
VOID GetDebugLevelRegValue(PPDEV ppdev);
#endif DBG


extern VOID NVMovePointer(
PDEV*   ppdev,
LONG    x,
LONG    y);


     


//
// This function makes sure that the physical coords and virtual coords of the two heads are correctly set.
// We use only the GDI mode details and completely ignore the registry data.
// This function is to be used only in the case of when the registry entry 'TwinViewInfo' does not exist or the registry 
// entry info is not sane.
//
void  UseOnlyGDIDataForModeDetails(PDEV *ppdev)
{
    ULONG i;
    ULONG ulHead;
    MODE_ENTRY RequestedMode, BestFitMode;

    DISPDBG((5,"Enter UseOnlyGDIDataForModeDetails()\n"));


    // Check to see if it is horizontal orientation
    if (HORIZONTAL_MODE(ppdev->cxScreen, ppdev->cyScreen)) 
    {
        ppdev->TwinView_State = NVTWINVIEW_STATE_SPAN;
        ppdev->TwinView_Orientation = HORIZONTAL_ORIENTATION;
        ppdev->ulNumberDacsActive = 2;
        ppdev->ulDesktopMode = NV_TWO_DACS | NV_SPANNING_MODE ;

        RequestedMode.Width = (USHORT)ppdev->cxScreen / 2;
        RequestedMode.Height = (USHORT)ppdev->cyScreen;
        RequestedMode.Depth = (USHORT)ppdev->cBitsPerPel;
        RequestedMode.RefreshRate = (USHORT)ppdev->ulGDIRefreshRate;

        //
        // Fill in the info for primary device
        //
        ulHead = ppdev->ulDeviceDisplay[0];
        SET_RECT(&ppdev->rclDisplayDesktop[ulHead], 0, 0, ppdev->cxScreen / 2, ppdev->cyScreen);

        if (bQuerySupportPhysicalMode(ppdev, ulHead, ppdev->ulDeviceMask[ulHead], &RequestedMode, &BestFitMode))
        {
            SET_RECT(&ppdev->rclCurrentDisplay[ulHead], 0, 0, RequestedMode.Width, RequestedMode.Height);
            ppdev->ulRefreshRate[ulHead] = RequestedMode.RefreshRate;
        }
        else
        {
            SET_RECT(&ppdev->rclCurrentDisplay[ulHead], 0, 0, BestFitMode.Width, BestFitMode.Height);
            ppdev->ulRefreshRate[ulHead] = BestFitMode.RefreshRate;
        }

        //
        // Fill in the info for secondary device
        //
        ulHead = ppdev->ulDeviceDisplay[1];
        SET_RECT(&ppdev->rclDisplayDesktop[ulHead], ppdev->cxScreen / 2, 0, 
            ppdev->rclDisplayDesktop[ulHead].left + (ppdev->cxScreen / 2), ppdev->cyScreen);

        if (bQuerySupportPhysicalMode(ppdev, ulHead, ppdev->ulDeviceMask[ulHead], &RequestedMode, &BestFitMode))
        {
            SET_RECT(&ppdev->rclCurrentDisplay[ulHead], ppdev->cxScreen / 2, 0, 
                ppdev->rclCurrentDisplay[ulHead].left + RequestedMode.Width, RequestedMode.Height);
            ppdev->ulRefreshRate[ulHead] = RequestedMode.RefreshRate;
        }
        else
        {
            SET_RECT(&ppdev->rclCurrentDisplay[ulHead], ppdev->cxScreen / 2, 0, 
                ppdev->rclCurrentDisplay[ulHead].left + BestFitMode.Width, BestFitMode.Height);
            ppdev->ulRefreshRate[ulHead] = BestFitMode.RefreshRate;
        }
    } // HORIZONTAL_MODE

    // check to see if it is vertical orientation
    if (VERTICAL_MODE(ppdev->cxScreen, ppdev->cyScreen)) 
    {
        ppdev->TwinView_State = NVTWINVIEW_STATE_SPAN;
        ppdev->TwinView_Orientation = VERTICAL_ORIENTATION;
        ppdev->ulNumberDacsActive = 2;
        ppdev->ulDesktopMode = NV_TWO_DACS | NV_SPANNING_MODE ;

        RequestedMode.Width = (USHORT)ppdev->cxScreen;
        RequestedMode.Height =(USHORT) ppdev->cyScreen / 2;
        RequestedMode.Depth = (USHORT)ppdev->cBitsPerPel;
        RequestedMode.RefreshRate = (USHORT)ppdev->ulGDIRefreshRate;

        //
        // Fill in the info for primary device
        //
        ulHead = ppdev->ulDeviceDisplay[0];
        SET_RECT(&ppdev->rclDisplayDesktop[ulHead], 0, 0, ppdev->cxScreen, ppdev->cyScreen / 2);

        if (bQuerySupportPhysicalMode(ppdev, ulHead, ppdev->ulDeviceMask[ulHead], &RequestedMode, &BestFitMode))
        {
            SET_RECT(&ppdev->rclCurrentDisplay[ulHead], 0, 0, RequestedMode.Width, RequestedMode.Height);
            ppdev->ulRefreshRate[ulHead] = RequestedMode.RefreshRate;
        }
        else
        {
            SET_RECT(&ppdev->rclCurrentDisplay[ulHead], 0, 0, BestFitMode.Width, BestFitMode.Height);
            ppdev->ulRefreshRate[ulHead] = BestFitMode.RefreshRate;
        }

        //
        // Fill in the info for secondary device
        //
        ulHead = ppdev->ulDeviceDisplay[1];
        SET_RECT(&ppdev->rclDisplayDesktop[ulHead], 0, ppdev->cyScreen / 2, 
            ppdev->cxScreen, ppdev->rclDisplayDesktop[ulHead].top + (ppdev->cyScreen / 2));

        if (bQuerySupportPhysicalMode(ppdev, ulHead, ppdev->ulDeviceMask[ulHead], &RequestedMode, &BestFitMode))
        {
            SET_RECT(&ppdev->rclCurrentDisplay[ulHead], 0, ppdev->cyScreen / 2,
                RequestedMode.Width, ppdev->rclCurrentDisplay[ulHead].top + RequestedMode.Height);
            ppdev->ulRefreshRate[ulHead] = RequestedMode.RefreshRate;
        }
        else
        {
            SET_RECT(&ppdev->rclCurrentDisplay[ulHead], 0, ppdev->cyScreen / 2,
                BestFitMode.Width, ppdev->rclCurrentDisplay[ulHead].top + BestFitMode.Height);
            ppdev->ulRefreshRate[ulHead] = BestFitMode.RefreshRate;
        }
    } // VERTICAL_MODE

    if (!MULTIMON_MODE(ppdev->cxScreen, ppdev->cyScreen)) 
    {
        // We are in Normal mode. There is no concept of clone mode when we use the GDI data.
        if (ppdev->TwinView_State != NVTWINVIEW_STATE_DUALVIEW)
            ppdev->TwinView_State = NVTWINVIEW_STATE_NORMAL;

        // This is the normal mode where we use only one dac.
        ppdev->ulNumberDacsActive = 1;

        RequestedMode.Width = (USHORT)ppdev->cxScreen;
        RequestedMode.Height = (USHORT)ppdev->cyScreen;
        RequestedMode.Depth = (USHORT)ppdev->cBitsPerPel;
        RequestedMode.RefreshRate = (USHORT)ppdev->ulGDIRefreshRate;

        //
        // Handle the case where nothing is connected to Head0 but something is connected to head1
        //
        if (ppdev->ulNumberDacsOnBoard == 2)
        {
            if (ppdev->ulNumberDacsConnected == 1)
            {
                if (ppdev->TwinView_State == NVTWINVIEW_STATE_NORMAL)
                {
                    switch (ppdev->ulDeviceDisplay[0])
                    {
                
                    case 0:
                        if (ppdev->ulDeviceType[0] == INVALID_DEVICE_TYPE && ppdev->ulDeviceType[1] != INVALID_DEVICE_TYPE)
                        {
                            // Swap the heads so that we use the other head.
                            ppdev->ulDeviceDisplay[0] = 1;
                            ppdev->ulDeviceDisplay[1] = 0;
                        }
                        break;

                    case 1:
                        if (ppdev->ulDeviceType[1] == INVALID_DEVICE_TYPE && ppdev->ulDeviceType[0] != INVALID_DEVICE_TYPE)
                        {
                            // Swap the heads so that we use the other head.
                            ppdev->ulDeviceDisplay[0] = 0;
                            ppdev->ulDeviceDisplay[1] = 1;
                        }
                        break;

                    default:  
                        DISPDBG((0,"Oops! The primary head is bogus: 0x%x",ppdev->ulDeviceDisplay[0]));
                        break;
                    }
                }
            }
        }


        //
        // Fill in the info for primary device
        //
        ulHead = ppdev->ulDeviceDisplay[0];
        SET_RECT(&ppdev->rclDisplayDesktop[ulHead], 0, 0, ppdev->cxScreen, ppdev->cyScreen);
        if (bQuerySupportPhysicalMode(ppdev, ulHead, ppdev->ulDeviceMask[ulHead], &RequestedMode, &BestFitMode))
        {
            SET_RECT(&ppdev->rclCurrentDisplay[ulHead], 0, 0, RequestedMode.Width, RequestedMode.Height);
            ppdev->ulRefreshRate[ulHead] = RequestedMode.RefreshRate;
        }
        else
        {
            SET_RECT(&ppdev->rclCurrentDisplay[ulHead], 0, 0, BestFitMode.Width, BestFitMode.Height);
            ppdev->ulRefreshRate[ulHead] = BestFitMode.RefreshRate;
        }
    } // NORMAL_MODE
}

//
// Returns the number of active dacs from the TwinViewInfo. The returned values are
// 1: standard mode
// 2: clone mode
// 2: spanning mode with a spanning resolution.
// 1: spanning mode with the single screen resolution (6x4, 8x6, 10x7).
// 1: If TwinViewInfo is invalid. This is the safe default value.
//
ULONG GetNumberDacsActiveFromTwinViewInfo (NVTWINVIEWDATA *pData)
{
    ULONG ulNumberDacsActive = 1;

    DISPDBG((5, "Enter GetNumberDacsActiveFromTwinViewInfo()\n"));
    if (pData == NULL)
    {
        DISPDBG((0, "Something Wrong. NULL pointer passed for TwinViewInfo structure"));
        return(ulNumberDacsActive);
    }

    switch (pData->dwState)
    {
        case NVTWINVIEW_STATE_NORMAL:
        case NVTWINVIEW_STATE_DUALVIEW:
            ulNumberDacsActive = 1;
            break;
        case NVTWINVIEW_STATE_CLONE:
            ulNumberDacsActive = 2;
            break;
        case NVTWINVIEW_STATE_SPAN:
            //sanity check
            if (!MULTIMON_MODE((pData->DeskTopRectl.right - pData->DeskTopRectl.left),(pData->DeskTopRectl.bottom - pData->DeskTopRectl.top)))
            {
                ULONG width;
                ULONG height;
                width = pData->DeskTopRectl.right - pData->DeskTopRectl.left;
                height = pData->DeskTopRectl.bottom - pData->DeskTopRectl.top;
                if ((width == 640 && height == 480) ||
                    (width == 800 && height == 600) ||
                    (width == 1024 && height == 768))
                {
                    //
                    // this is ok as we always export these special modes.
                    // The jury is still out on whether we should set the NumberDacsActive to 1 or 2 for this
                    // case, as the more intuitive option. I vote for 1 as being the more intuitive.
                    //
                    ulNumberDacsActive = 1;
                }
                else
                {
                    DISPDBG((0, "Oops! State: 0x%x, inconsistent with the DeskTopRectl: (%d, %d), (%d, %d)\n",pData->dwState,
                        pData->DeskTopRectl.left, pData->DeskTopRectl.top, pData->DeskTopRectl.right,pData->DeskTopRectl.bottom));
                    ulNumberDacsActive = 1;
                }
            }
            else
            {
                // This is a true multimon resolution
                ulNumberDacsActive = 2;
            }
            break;
        default:
            DISPDBG((0, "Oops! Invalid State from the registry pData: 0x%x\n",pData->dwState));
            ulNumberDacsActive = 1;
            break;
    }
    return(ulNumberDacsActive);
}

//
// This function makes sure that the physical coords and virtual coords of the two heads are correctly set.
// We use only the Registry data set by the nVidia panel and completely ignore the GDI Data (which is a safe thing
// thing to do anyway because at this point, we should have made sure that the GDI data is in complete sync 
// with the registry data.
//
void UseOnlyRegistryDataForModeDetails(PDEV *ppdev, NVTWINVIEWDATA *pData)
{
    ULONG i;
    ULONG ulHead;
    MODE_ENTRY RequestedMode, BestFitMode;
    ULONG SuggestedDeviceMask, SuggestedDeviceType, SuggestedTVType;
    ULONG ulPrimaryHead;

    DISPDBG((5, "Enter UseOnlyRegistryDataForModeDetails()\n"));

    ppdev->TwinView_State = pData->dwState;
    ppdev->TwinView_Orientation = pData->dwOrientation;

    switch (pData->dwState)
    {
        case NVTWINVIEW_STATE_NORMAL:
        case NVTWINVIEW_STATE_DUALVIEW:
            ppdev->ulNumberDacsActive = 1;
            ppdev->ulDesktopMode = 0;
            break;
        case NVTWINVIEW_STATE_CLONE:
            ppdev->ulNumberDacsActive = 2;
            ppdev->ulDesktopMode = NV_CLONE_DESKTOP | NV_TWO_DACS;
            break;
        case NVTWINVIEW_STATE_SPAN:
            //sanity check
            if (!MULTIMON_MODE((pData->DeskTopRectl.right - pData->DeskTopRectl.left),(pData->DeskTopRectl.bottom - pData->DeskTopRectl.top)))
            {
                ULONG width;
                ULONG height;
                width = pData->DeskTopRectl.right - pData->DeskTopRectl.left;
                height = pData->DeskTopRectl.bottom - pData->DeskTopRectl.top;
                if ((width <= 640 && height <= 480) ||
                    (width == 800 && height == 600) ||
                    (width == 1024 && height == 768))
                {
                    //
                    // this is ok as we always export these special modes.
                    // The jury is still out on whether we should set the NumberDacsActive to 1 or 2 for this
                    // case, as the more intuitive option. I vote for 1 as being the more intuitive.
                    //
                    ppdev->ulNumberDacsActive = 1;
                    ppdev->ulDesktopMode = 0;
                }
                else
                {
                    DISPDBG((0, "Oops! State: 0x%x, inconsistent with the DeskTopRectl: (%d, %d), (%d, %d)\n",pData->dwState,
                        pData->DeskTopRectl.left, pData->DeskTopRectl.top, pData->DeskTopRectl.right,pData->DeskTopRectl.bottom));
                    return;
                }
            }
            else
            {
                // This is a true multimon resolution
                ppdev->ulNumberDacsActive = 2;
                ppdev->ulDesktopMode = NV_TWO_DACS | NV_SPANNING_MODE ;
            }
            break;
        default:
            DISPDBG((0, "Oops! Invalid State from the registry pData: 0x%x\n",pData->dwState));
            return;
            break;
    }

    ulPrimaryHead = pData->dwDeviceDisplay[0];
    ppdev->cxScreen = pData->DeskTopRectl.right - pData->DeskTopRectl.left;
    ppdev->cyScreen = pData->DeskTopRectl.bottom - pData->DeskTopRectl.top;
    ppdev->cBitsPerPel = pData->NVTWINVIEWUNION.nvtwdevdata[ulPrimaryHead].dwBpp; // same for both devices
    ppdev->cjPelSize = pData->NVTWINVIEWUNION.nvtwdevdata[ulPrimaryHead].dwBpp / 8; // same for both devices
    ppdev->ulGDIRefreshRate = pData->NVTWINVIEWUNION.nvtwdevdata[ulPrimaryHead].dwRefresh;

    for (i=0; i < ppdev->ulNumberDacsOnBoard; i++)
    {
        // Get the physical head ==> logical head mapping.
        ppdev->ulDeviceDisplay[i] = pData->dwDeviceDisplay[i];
    }

    //
    // We should iterate through only the numberDacsActive for std/clone/span modes.
    // The only exception is the WindowsXP dualView mode where both heads needs to be initialized
    // even though the NumberDacsActive is 1.
    //
    for (i=0; i < (pData->dwState == NVTWINVIEW_STATE_DUALVIEW ? ppdev->ulNumberDacsOnBoard : ppdev->ulNumberDacsActive); i++)
    {
        ulHead = ppdev->ulDeviceDisplay[i];

        // Get the per head flags.
        ppdev->ulEnablePanScan[ulHead] = pData->NVTWINVIEWUNION.nvtwdevdata[ulHead].dwEnablePanScan;
        ppdev->ulEnableDDC[ulHead] = pData->NVTWINVIEWUNION.nvtwdevdata[ulHead].dwEnableDDC;
        ppdev->ulEnableDDC[ulHead] = 1; // Hardcoded for now till control panel adds a button for this.
        ppdev->ulFreezePanScan[ulHead] = pData->NVTWINVIEWUNION.nvtwdevdata[ulHead].dwFreezePanScan;
        ppdev->ulFreezeOriginX[ulHead] = pData->NVTWINVIEWUNION.nvtwdevdata[ulHead].dwFreezeOriginX;
        ppdev->ulFreezeOriginY[ulHead] = pData->NVTWINVIEWUNION.nvtwdevdata[ulHead].dwFreezeOriginY;
        ppdev->ulTimingOverRide[ulHead] = pData->NVTWINVIEWUNION.nvtwdevdata[ulHead].dwTimingOverRide;


        SET_RECT(&ppdev->rclDisplayDesktop[ulHead], pData->NVTWINVIEWUNION.nvtwdevdata[ulHead].VirtualRectl.left,
            pData->NVTWINVIEWUNION.nvtwdevdata[ulHead].VirtualRectl.top, pData->NVTWINVIEWUNION.nvtwdevdata[ulHead].VirtualRectl.right,
            pData->NVTWINVIEWUNION.nvtwdevdata[ulHead].VirtualRectl.bottom);

        //
        // Before we honor the nvPanel's device type selection, lets verify it.
        //
        if (bConfirmDeviceType(ppdev, ulHead, pData->NVTWINVIEWUNION.nvtwdevdata[ulHead].dwDeviceMask, pData->NVTWINVIEWUNION.nvtwdevdata[ulHead].dwDeviceType, 
            pData->NVTWINVIEWUNION.nvtwdevdata[ulHead].dwTVFormat, &SuggestedDeviceMask, &SuggestedDeviceType, &SuggestedTVType))
        {
            DISPDBG((5,"The requested device type is valid.\n"));
            // use the nvPanel's selected device type
            ppdev->ulDeviceMask[ulHead] = pData->NVTWINVIEWUNION.nvtwdevdata[ulHead].dwDeviceMask;
            ppdev->ulDeviceType[ulHead] = pData->NVTWINVIEWUNION.nvtwdevdata[ulHead].dwDeviceType;
            ppdev->ulTVFormat[ulHead] = pData->NVTWINVIEWUNION.nvtwdevdata[ulHead].dwTVFormat;
        }
        else
        {
            DISPDBG((0,"Head: %d, The nvPanel requested an invalid device type: 0x%x, TVFormat: 0x%x\n",ulHead,
                pData->NVTWINVIEWUNION.nvtwdevdata[ulHead].dwDeviceType,pData->NVTWINVIEWUNION.nvtwdevdata[ulHead].dwTVFormat));
            DISPDBG((0,"so using the suggested default device type: 0x%x, TVFormat: 0x%x\n", 
                SuggestedDeviceType,SuggestedTVType));
            ppdev->ulDeviceMask[ulHead] = SuggestedDeviceMask;
            ppdev->ulDeviceType[ulHead] = SuggestedDeviceType;
            ppdev->ulTVFormat[ulHead] = SuggestedTVType;
        }

        //
        // Before we honor the nvPanel's physical rectangle and refrsh, lets verify it.
        //
        RequestedMode.Width = (USHORT)(pData->NVTWINVIEWUNION.nvtwdevdata[ulHead].PhysicalRectl.right - pData->NVTWINVIEWUNION.nvtwdevdata[ulHead].PhysicalRectl.left);
        RequestedMode.Height = (USHORT)(pData->NVTWINVIEWUNION.nvtwdevdata[ulHead].PhysicalRectl.bottom - pData->NVTWINVIEWUNION.nvtwdevdata[ulHead].PhysicalRectl.top);
        RequestedMode.RefreshRate = (USHORT)pData->NVTWINVIEWUNION.nvtwdevdata[ulHead].dwRefresh;
        RequestedMode.Depth = (USHORT)pData->NVTWINVIEWUNION.nvtwdevdata[ulPrimaryHead].dwBpp;

        if (bQuerySupportPhysicalMode(ppdev, ulHead, ppdev->ulDeviceMask[ulHead], &RequestedMode, &BestFitMode))
        {
            // Registry had valid physical mode info. Use it as is.
            ppdev->rclCurrentDisplay[ulHead].left = pData->NVTWINVIEWUNION.nvtwdevdata[ulHead].PhysicalRectl.left;
            ppdev->rclCurrentDisplay[ulHead].top = pData->NVTWINVIEWUNION.nvtwdevdata[ulHead].PhysicalRectl.top;
            ppdev->rclCurrentDisplay[ulHead].right = pData->NVTWINVIEWUNION.nvtwdevdata[ulHead].PhysicalRectl.right;
            ppdev->rclCurrentDisplay[ulHead].bottom = pData->NVTWINVIEWUNION.nvtwdevdata[ulHead].PhysicalRectl.bottom;
            ppdev->ulRefreshRate[ulHead] = pData->NVTWINVIEWUNION.nvtwdevdata[ulHead].dwRefresh;
        }
        else
        {
            // Registry did not have have valid physical mode info. So construct it.
            SET_RECT(&ppdev->rclCurrentDisplay[ulHead], 0, 0, BestFitMode.Width, BestFitMode.Height);
            ppdev->ulRefreshRate[ulHead] = BestFitMode.RefreshRate;
            //
            // If we are on multimon, adjust the coordinates appropriately for the second head
            //
            if (pData->dwState == NVTWINVIEW_STATE_SPAN && i == 1)
            {
                if (HORIZONTAL_MODE(ppdev->cxScreen, ppdev->cyScreen))
                {
                    ppdev->rclCurrentDisplay[ulHead].left += ppdev->cxScreen / 2;
                    ppdev->rclCurrentDisplay[ulHead].right += ppdev->cxScreen / 2;
                }
                if (VERTICAL_MODE(ppdev->cxScreen, ppdev->cyScreen))
                {
                    ppdev->rclCurrentDisplay[ulHead].top += ppdev->cyScreen / 2;
                    ppdev->rclCurrentDisplay[ulHead].bottom += ppdev->cyScreen / 2;
                }
            }
        }
    }
}


//
//  Does a one time initialization of the following info into the pdev.
//
//  TwinView_State: Read from the registry (normal/clone/multimon).
//  ulNumberDacsOnBoard: Number of heads in the system.
//  ulNumberDacsConnected: Number of heads in the system which are connected to a device.
//  ulNumberDacsActive: Number of heads in the system which are active for this mode.
// ulAllDeviceMask: A mask of all output device connectors in the board.
// ulConnectedDeviceMask: A mask of all currently connected output devices in the system.
//  ulMoveDisplayDX, ulMoveDisplayDy: Panning-scanning amaount to move the screen by.
//  ulDeviceMask[NumDacs]: Bitmask for the device connected to each head.defined in nvcm.h
//  ulDeviceType[NumDacs]: defined in nvcm.h, CRT, TV, DFP
//  ulTVFormat[numDacs]: PAL or NTSC
//  ulNumPhysModes[numDacs]
//  ulnumVirtualModes[NumDacs]
//  PhysModeList[numDacs]
//  VirtualModeList[NumDacs]
//  
//
BOOL InitMultiMon(PDEV *ppdev)
{
    ULONG i, j;
    ULONG ReturnedDataLength;
    NVTWINVIEWDATA sData;
    ULONG RegistryDataOK;   // This indicates if the user selections of the nvPanel stored into the registry is OK.
    ULONG UseGDIData;
    ULONG ulDisplayWidth;
    ULONG ulDisplayHeight;
    ULONG ulHead;
    GET_NUMBER_DACS   NumberDacs;
    ULONG ulDeviceType;
    ULONG ulRet;
    ULONG ulDeviceMask;
    ULONG ulTVFormat;
    ULONG UseBIOSHeads;
    ULONG Status;
    ULONG PowerStateRegValue;
    ULONG ulCombinedDeviceMask, ulTmp;
    ULONG ulBIOSDeviceMask;
    


    DISPDBG((6,"InitMultiMon(): Enter"));


    // Get the head info from the miniport such as number of heads on board, number of heads connected and device type of each head.
    if (EngDeviceIoControl(ppdev->hDriver,
                        IOCTL_VIDEO_GET_NUMBER_DACS,
                        NULL,
                        0,
                        &NumberDacs,
                        sizeof(NumberDacs),
                        &ReturnedDataLength))
    {
        DISPDBG((0, "Oops! InitMultiMon() - IOCTL_VIDEO_GET_NUMBER_DACS failed"));
        // Set safe default values.
        ppdev->ulNumberDacsOnBoard = 1;
        ppdev->ulNumberDacsConnected = 1;
        ppdev->ulAllDeviceMask = BITMASK_CRT0;
        ppdev->ulConnectedDeviceMask = BITMASK_CRT0;
        for (i=0; i < NV_NO_DACS; i++)
        {
            ppdev->ulDeviceMask[i] = BITMASK_CRT0;
            ppdev->ulDeviceType[i] = MONITOR_TYPE_VGA;
        }
    }
    else
    {
        // Fill the info into ppdev.
        ppdev->ulNumberDacsOnBoard = NumberDacs.ulNumberDacsOnBoard;
        ppdev->ulNumberDacsConnected = NumberDacs.ulNumberDacsConnected;
        ppdev->ulAllDeviceMask = NumberDacs.ulAllDeviceMask;
        ppdev->ulConnectedDeviceMask = NumberDacs.ulConnectedDeviceMask;
        ppdev->Mobile = NumberDacs.Mobile;
        ppdev->UseHWSelectedDevice = NumberDacs.UseHWSelectedDevice;
        ppdev->ACPISystem = NumberDacs.ACPISystem;
        ppdev->UseHotKeySameMode = NumberDacs.UseHotKeySameMode;
        ppdev->PanScanSelection = NumberDacs.PanScanSelection;
        ppdev->ulDFPSupports1400x1050 = NumberDacs.ulDFPSupports1400x1050;

        for (i = 0; i < NumberDacs.ulNumberDacsOnBoard; i++)
        {
            ppdev->ulDeviceMask[i] = NumberDacs.ulDeviceMask[i];
            ppdev->ulDeviceDisplay[i] = NumberDacs.ulDeviceDisplay[i];
            ppdev->ulDeviceType[i] = NumberDacs.ulDeviceType[i];
            ppdev->ulTVFormat[i] = NumberDacs.ulTVFormat[i];
            ppdev->ulHeadDeviceOptions[i] = NumberDacs.ulHeadDeviceOptions[i];
            if (ppdev->ulHeadDeviceOptions[i] != 0 && ppdev->ulDeviceType[i] != INVALID_DEVICE_TYPE) 
            {
                ppdev->ulDacStatus[i] |= DAC_STATUS_CONNECTED;
            }
        }
    }

    //
    // Initialize the modelist pointers to NULL
    //
    for (i = 0; i < NV_NO_DACS; i++)
    {
        ppdev->PhysModeList[i] = NULL;
        ppdev->VirtualModeList[i] = NULL;
    }

    //
    // Read the default TVFormat from RM
    //
    GetDefaultTVFormat(ppdev);

    //
    // See if we need to turn on the Sony specific STOP_IMAGE method during modesets.
    //
    ppdev->EnableModesetStopImage = 0;
    GetULONGRegValue(ppdev, "EnableModesetStopImage",&ppdev->EnableModesetStopImage);
    
    if (ppdev->ulNumberDacsConnected != ppdev->ulNumberDacsOnBoard) 
    {
        DISPDBG((1,"ulNumberDacsConnected: %d, different than ulNumberDacsOnBoard: %d",
                ppdev->ulNumberDacsConnected, ppdev->ulNumberDacsOnBoard));
        // ppdev->ulNumberDacsOnBoard = ppdev->ulNumberDacsConnected;
    } 

    for (i=0; i < ppdev->ulNumberDacsOnBoard; i++)
    {
        ppdev->ulDacStatus[i] |= DAC_STATUS_ON_BOARD;
        ppdev->ulEnablePanScan[i] = 1;
        ppdev->ulFreezePanScan[i] = 0;
        ppdev->ulFreezeOriginX[i] = 0;
        ppdev->ulFreezeOriginY[i] = 0;
        ppdev->ulEnableDDC[i] = 1;
        ppdev->ulTimingOverRide[i] = NVTWINVIEW_STATE_AUTO;
    }

    if (ppdev->ulNumberDacsConnected == 0) 
    {
        DISPDBG((0,"Oops! No head is connected, assuming a default CRT monitor with single head config\n"));
        ppdev->ulNumberDacsConnected = 1;
        ppdev->ulAllDeviceMask = BITMASK_CRT0;
        ppdev->ulConnectedDeviceMask = BITMASK_CRT0;
        ppdev->ulDeviceMask[0] = BITMASK_CRT0;
        ppdev->ulDeviceType[0] = MONITOR_TYPE_VGA;
        ppdev->ulDacStatus[0] |= DAC_STATUS_CONNECTED;
        DISPDBG((1,"Deivetype[%d] is MONITOR_TYPE_VGA",i));
    }

    //
    // Read the TwinViewState from the registry
    //
    ppdev->TwinView_State = NVTWINVIEW_STATE_NORMAL;
    ppdev->ulNumberDacsActive = 1;
    UseGDIData = 1;
    if (EngDeviceIoControl(ppdev->hDriver,
                        IOCTL_VIDEO_GET_TWINVIEW_STATE,
                        NULL,
                        0,
                        &sData,
                        sizeof(NVTWINVIEWDATA),
                        &ReturnedDataLength))
    {
        DISPDBG((3, "InitMultiMon() - IOCTL_VIDEO_GET_TWINVIEW_STATE failed"));
        
        if(!BuildModeLists(ppdev))
            return(FALSE);
    }
    else
    {
        //
        // Print the registry data the way it was read
        //
        PrintTwinViewData("Registry data as read in InitMultiMon()", 1, &sData, ppdev);

        //
        // Do some sanity tests to make sure we didnt get bogus data from the registry
        //
        if (sData.dwState == NVTWINVIEW_STATE_DUALVIEW)
            ppdev->TwinView_State = sData.dwState;
        if (TwinViewDataOK(ppdev, &sData))
        {
            ULONG ForceUseGDIData = 0;
            DISPDBG((6, "The registry data is sane\n"));
            

            if (!ValidateRegistryDeviceMasks(ppdev, &sData))
            {
                ForceUseGDIData = 1;
            }
            else
            {
                //
                // The requested device masks are valid. Read it into ppdev before building modelists.  
                //
                InitDevMaskFromTwinViewInfo(ppdev, &sData);
            }

            //
            // Build the virtual/physical modelists for the requested devices.
            //
            if(!BuildModeLists(ppdev))
                return(FALSE);

            if (bQueryBothPanelsAreInSync(ppdev, &sData))
            {
                // Now we can trust and use the Registry data as is.
                DISPDBG((6, "Using the registry data\n"));
                UseGDIData = 0;
            }
            else
            {
                DISPDBG((5, "Both panels are not in sync. So using the GDI data\n"));
            }
            if (ForceUseGDIData)
            {
                DISPDBG((1,"ForceUseGDIData is on. So using the GDI data"));
                UseGDIData = 1;
            }
        } 
        else
        {
            DISPDBG((1, "Ooops! The registry data is not sane\n"));
            if(!BuildModeLists(ppdev))
                return(FALSE);
        }
    } // IOCTL


    if (UseGDIData)
    {
        DISPDBG((3,"Using GDI data for mode details\n"));
        UseOnlyGDIDataForModeDetails(ppdev);
    }
    else
    {
        DISPDBG((3,"Using registry data for mode details\n"));
        UseOnlyRegistryDataForModeDetails(ppdev, &sData);
    }

    DISPDBG((1, "HKPending: 0x%x, ulAssertModeTRUE: 0x%x\n", NumberDacs.ulHotKeyPending,ppdev->ulDrvAssertModeTRUE));

    //
    // If this modeset was initiated by the nvPanel, then the TVFormat specified by the user takes precedence
    // over the default TVFormat.
    //
    if (GetInitiatedRegValue(ppdev) == 1)
    {
        // Now go through the active heads and change the TVFormat if we find any active TV device.
        for (i=0; i < ppdev->ulNumberDacsActive; i++)
        {
            ulHead = ppdev->ulDeviceDisplay[i];
            ulDeviceMask = ppdev->ulDeviceMask[ulHead];
            if (ulDeviceMask & BITMASK_ALL_TV)
            {
                DISPDBG((1, "InitMultiMon(): RMDefaultTVFormat: 0x%x, nvPanelTVFormat: 0x%x, PhysHead: 0x%x\n",
                    ppdev->ulDefaultTVFormat, ppdev->ulTVFormat[ulHead], ulHead));
                DISPDBG((1, "So using the user specified TVFormat instead of the RMDefaultTVFormat\n"));
                ppdev->ulDefaultTVFormat = ppdev->ulTVFormat[ulHead];
                ppdev->ulDefaultTVDeviceType = TVFormatToTVDeviceType(ppdev->ulTVFormat[ulHead]);
                break;
            }
        }
    }

    

    // 
    // Assume we have to obey the BIOS heads
    //
    UseBIOSHeads = 1;

    //
    // Miniport handles the BIOS heads at boot time. So need for the display driver to muck with
    // BIOS heads. We do not want to do this if we were called as a result of the hotkey switch.
    //
    if (NumberDacs.ulHotKeyPending == 1)
    {
        UseBIOSHeads = 0;
        //
        // We need to let the control panel thread know that a HK switch has taken place.
        // This is to handle the case when the control panel is open and the user presses HK
        // and the same desktop is maintained by the OS, then the OS does not send any
        // WM_DISPLAYCHANGE message to the control panel app thus causing some device-type-sensitive
        // pages such as TV adjustment to crash.
        //
        if (EngDeviceIoControl(ppdev->hDriver,
                        IOCTL_VIDEO_SIGNAL_NVCPL_HOTKEY_EVENT,
                        NULL,
                        0,
                        NULL,
                        0,
                        &ReturnedDataLength))
        {
            DISPDBG((1, "InitMultiMon() - IOCTL_VIDEO_SIGNAL_NVCPL_HOTKEY_EVENT failed"));
        }   

        //
        // Clear the ulInduceModeChangeDeviceMask so that StopImage will be deactivated if it has been turned on.
        // This handles the pathological case, where for some reason, control panel/NVSVC is not able to
        // do the induced modechange successsfully and the screen stays black since StopImage is activated.
        // Then the user will most likely hit HK to get the laptop back to a usable state.
        // Also, let miniport know too.
        //
        if (ppdev->ulInduceModeChangeDeviceMask != 0)
        {
            DISPDBG((1,"Something wrong. ppdev->ulInduceModeChangeDeviceMask: 0x%x, is being cleared due to HK",
            ppdev->ulInduceModeChangeDeviceMask));
        }
        ppdev->ulInduceModeChangeDeviceMask = 0;
        if (EngDeviceIoControl(ppdev->hDriver,
                        IOCTL_VIDEO_SET_NVSVC_MODESET_DEVMASK,
                        &ppdev->ulInduceModeChangeDeviceMask,
                        sizeof(ULONG),
                        NULL,
                        0,
                        &ReturnedDataLength))
        {
            DISPDBG((1, "InitMultiMon() - IOCTL_VIDEO_SIGNAL_MODESET_DEVMASK failed"));
        }   

    }

    //
    // if we are called directly by the nvPanel, do not use BIOS heads.
    //
    if (GetInitiatedRegValue(ppdev) == 1)
    {
       UseBIOSHeads = 0;
    }

    //
    // The display driver needs to obey BIOS heads only for two cases - returning from FSDOS and resuming from
    // standby/suspend/hibernate. Fortunately for us, Win2K calls DrvAssertMode(TRUE) while returning from 
    // these conditions. So we will use BIOS heads only if this modeset is a result of DrvAssertMode(TRUE) and not
    // DrvEnableSurface().
    //
    if (ppdev->ulDrvAssertModeTRUE == 0)
    {
       UseBIOSHeads = 0;
    }
    else
    {
        //
        // We are here because of a DrvAssertMode(TRUE). Now check if we are enabling
        // the same pdev that was used while going down into VGA mode last. This handles
        // the special case of when we have multiple pdevs active. We compare the 
        // iUnique numbers of the pdevs to distinguish between the cases of
        // resuming from FSDOS/DPMS and toggling between multiple pdevs.
        //
        DISPDBG((1, "Comparing ppdev->iUniqueness: %d, MiniportiUniqueness: %d",
            ppdev->iUniqueness, NumberDacs.ulUniquenessVGA));
        if (NumberDacs.ulUniquenessVGA != ppdev->iUniqueness)
        {
            DISPDBG((1, "Mismatch of iUniqueness so not calling HandleBiosHeads"));
            UseBIOSHeads = 0;
        }
        else
        {
            //
            // We are returning from FSDOS or resuming from DPMS.
            // Clear the PrevDevMask registry entry to ensure that we do the begin/end modeset bracketing.
            //
            SetPrevDevMaskRegValue(ppdev, 0);
        }
    }

    //
    // if we are in spanning mode, we do not obey BIOS heads
    //
    if (ppdev->TwinView_State == NVTWINVIEW_STATE_SPAN)
    {
       //
       // Exception. If one or both of the devices is disconnected, 
       // then we do obey BOOT_DEVICES in spanning mode on laptops.
       //
       if (ppdev->ACPISystem)
       {
            ulCombinedDeviceMask = 0;
            for (i=0; i < ppdev->ulNumberDacsActive; i++)
            {
                ulHead = ppdev->ulDeviceDisplay[i];
                ulCombinedDeviceMask |= ppdev->ulDeviceMask[ulHead];
            }
            ulTmp = ulCombinedDeviceMask;
            if (NvConfigGetEx (ppdev->hDriver, ppdev->hClient, ppdev->hDevice,
                               NV_CFGEX_CHECK_CONNECTED_DEVICES, &ulTmp, sizeof(ULONG)))
            {
                DISPDBG((0, "HandleBIOSHeads - Cannot get CHECK_CONNECTED_DEVICES value from RM"));
                UseBIOSHeads = 0;
            }
            else
            {
                if (ulTmp == ulCombinedDeviceMask)
                {
                   //
                   // Both devices are present. So no need to obey BOOT_DEVICES.
                   //
                   UseBIOSHeads = 0;
                }
            }
        }
    }

    //
    // We don't need to obey BOOT_DEVICES on desktops.
    //
    if (ppdev->ACPISystem == 0)
    {
       UseBIOSHeads = 0;
    }

    //
    // use the BIOS heads only if asked to do so in the registry (for laptops).
    //
    if (ppdev->UseHWSelectedDevice == 0)
    {
       UseBIOSHeads = 0;
    }

    

    //
    // Check the registry to see if this is not the extra DrvAssertMode(TRUE) that the OS calls 
    // while resuming from hibernation. Ugh.. I hate state machines but I could not think of any other
    // way to detect this condition. Note that all these calls act on the same pdev.
    // The situation I was running into a problem on the laptop was as follows:
    //
    // - set clone(DFP+CRT).
    // - hibernate
    // - resume
    // - DrvAssertMode(TRUE):   BOOT_DEVICES correctly indicates DFP+CRT: First call
    // - DrvAssertMode(FALSE):  Display and RM think that we are going into FSDOS
    // - DrvAssertMode(TRUE):   BOOT_DEVICES indicates CRT: We need to identify this extra DrvAssertModeTRUE call
    //                          and not obey the BOOT_DEVICES: Second call.
    //
    // All this jugglery because there is no way for the display driver to uniquely identify when we are really going 
    // down into VDM FSDOS via ALT+ENTER or via an App request. Sigh...
    // Note: '5' corresponds to the VideoPowerHibernateState defined in ddk\src\video\inc\ntddvdeo.h
    // Since I can not include ntddvdeo.h in the display driver, I use the numeral value.
    //
    if (ppdev->ulDrvAssertModeTRUE == 1 && NumberDacs.ulUniquenessVGA == ppdev->iUniqueness)
    {
        #define IGNORE_NEXT_ASSERTMODE 0x1234
        //
        // Read the registry value to see what the power down state was when we went down before resuming.
        //
        PowerStateRegValue = GetPowerStateRegValue(ppdev);

        //
        // See if it is the first call.
        //
        if (PowerStateRegValue == 5)
        {
            //
            // we set the registry to indicate that on the next DrvAssertMode(TRUE) we do not obey the BOOT_DEVICES.
            //
            SetPowerStateRegValue(ppdev, IGNORE_NEXT_ASSERTMODE);
        }

        //
        // See if it is the second call.
        //
        if (PowerStateRegValue == IGNORE_NEXT_ASSERTMODE)
        {
            //
            // we do not read the BOOT_DEVICES for this second extra call.
            // Also, set the registry value back to 0.
            //
            if (ppdev->ulDrvAssertModeTRUE == 1)
            {
                UseBIOSHeads = 0;
                SetPowerStateRegValue(ppdev, 0);
            }
        }
    }
    

    //
    // Now call handleBIOSHEads() if we need to.
    //
    if (UseBIOSHeads)
    {
        ulRet = HandleBIOSHeads(ppdev);
        if (ulRet == FALSE)
        {
            //
            // Since the devices were changed in HandleBIOSHeads(), we need to reconstruct the modelists
            // and make sure we are using valid physical modes.
            // Handle only the standard/clone mode case (just a redundant safety check since we should never be
            // here if we were in spanning mode).
            //
            if (ppdev->TwinView_State == NVTWINVIEW_STATE_NORMAL ||
                ppdev->TwinView_State == NVTWINVIEW_STATE_DUALVIEW ||
                ppdev->TwinView_State == NVTWINVIEW_STATE_CLONE)
            {
                if(!BuildModeLists(ppdev))
                    return(FALSE);
                UseOnlyGDIDataForModeDetails(ppdev);
            }
        }
    }
    else 
    {
        DISPDBG((1,"Not called HandleBIOSHEads\n"));
        //
        // Print the BOOT_DEVICES just for debugging purposes
        //
        if (Status = NvConfigGet (ppdev->hDriver, ppdev->hClient, ppdev->hDevice,
                                  NV_CFG_GET_BOOT_DEVICES, &ulBIOSDeviceMask))
        {
            DISPDBG((0,"Oops!    NV_CFG_GET_BOOT_DEVICES failed"));
        }
        DISPDBG((1,"Disp: Informational only: HandleBIOSHeads(): BIOSDeviceMask: 0x%x",ulBIOSDeviceMask));
    }

    //
    // Now turn off the DrvAssertMode flag since we have used it already.
    //
    ppdev->ulDrvAssertModeTRUE = 0;
    
    switch (ppdev->TwinView_State)
    {
        case NVTWINVIEW_STATE_SPAN:
            if (MULTIMON_MODE(ppdev->cxScreen, ppdev->cyScreen))
            {
                ppdev->ulDesktopMode = NV_TWO_DACS | NV_SPANNING_MODE ;
                ppdev->ulNumberDacsActive = 2;
            }
            else
            {
                ppdev->ulDesktopMode = 0;
                ppdev->ulNumberDacsActive = 1;
            }
            DISPDBG((6,"State Read is NVTWINVIEW_STATE_SPAN"));
            break;
        case NVTWINVIEW_STATE_CLONE:
            ppdev->ulDesktopMode = NV_CLONE_DESKTOP | NV_TWO_DACS;
            ppdev->ulNumberDacsActive = 2;
            DISPDBG((6,"State Read is NVTWINVIEW_STATE_CLONE"));
            break;
        case NVTWINVIEW_STATE_NORMAL:
        case NVTWINVIEW_STATE_DUALVIEW:
            ppdev->ulNumberDacsActive = 1;
            ppdev->ulDesktopMode = 0;
            DISPDBG((6,"State Read is NVTWINVIEW_STATE_NORMAL/NVTWINVIEW_STATE_DUALVIEW"));
            break;
        default:
            ppdev->ulDesktopMode = 0;
            ppdev->ulNumberDacsActive = 1;
            DISPDBG((0,"oops!! Invalid TwinView State: %d\n",ppdev->TwinView_State));
            break;
    }

    // Control panel To Do
    // should set up these value to move screen for panning and scanning
    
    // HW restriction: Set image offset value is a multiple of 4 bytes.  
    // Since move mouse pointer rountine does not deal with 8bpp, 16bpp or 32bpp,
    // set to 4 pixel increment in order to satisfy HW restriction.
    ppdev->ulMoveDisplayDx = 4; 
    ppdev->ulMoveDisplayDy = 4;

    //
    // Convert the device masks to device type
    //
    for (i = 0; i < ppdev->ulNumberDacsOnBoard; i++)
    {
        //
        // Convert the device mask to device type. 
        //
        ulHead = ppdev->ulDeviceDisplay[i];
        ulDeviceType = MaskToDeviceType(ppdev, ppdev->ulDeviceMask[ulHead]);
        ppdev->ulDeviceType[ulHead] = ulDeviceType;
        ppdev->ulTVFormat[ulHead] = ppdev->ulDefaultTVFormat;
    }

    //
    // Make sure the head mapping array is 0,1 or 1,0 for standard mode.
    //
    if (ppdev->TwinView_State == NVTWINVIEW_STATE_NORMAL || ppdev->TwinView_State == NVTWINVIEW_STATE_DUALVIEW)
    {
        ulHead = ppdev->ulDeviceDisplay[0];
        if (ulHead == 1)
        {
            ppdev->ulDeviceDisplay[1] = 0;
        }
        if (ulHead == 0)
        {
            ppdev->ulDeviceDisplay[1] = 1;
        }
    }

    //
    // Handle the BANDWIDTH limitations of some hardware.
    // For eg. on Toshiba, the NV hardware and the SXGA+ EDID can handle 1400x1050x100HZ.
    // But the system does not allow for scanout bandwidth for this mode while in clone mode.
    // We make this test as a generic test.
    //
    if (ppdev->ulNumberDacsActive == 1)
    {
        HandleBandWidth_SingleDacActive(ppdev);
    }
    if (ppdev->ulNumberDacsActive == 2)
    {
        HandleBandWidth_DualDacsActive(ppdev);
    }
    


    for(i = 0; i < ppdev->ulNumberDacsActive; i++)
    {
        ulHead = ppdev->ulDeviceDisplay[i];
        ulDisplayWidth = ppdev->rclCurrentDisplay[ulHead].right - ppdev->rclCurrentDisplay[ulHead].left;
        ulDisplayHeight = ppdev->rclCurrentDisplay[ulHead].bottom - ppdev->rclCurrentDisplay[ulHead].top;
        if((ulDisplayWidth < (ULONG)(ppdev->rclDisplayDesktop[ulHead].right - ppdev->rclDisplayDesktop[ulHead].left)) ||
           (ulDisplayHeight < (ULONG)(ppdev->rclDisplayDesktop[ulHead].bottom - ppdev->rclDisplayDesktop[ulHead].top)) )
        {
            ppdev->ulDesktopMode |= (i ? NV_VIRTUAL_DESKTOP_2 : NV_VIRTUAL_DESKTOP_1); 
        }
    }

    ppdev->ulFlipBase = 0;          // Re-initialized back to 0 base after a mode switch


    // Initialize the Cached Cursor data for each mode set
    ppdev->ulCachedCursorWidth = 0;
    ppdev->ulCachedCursorFormat = 0;


    //
    // Now print the info recorded into the ppdev
    //
    PrintPPDEVTwinViewInfo("Printing PPDEV info after finishing InitMultiMon()", ppdev);

    //
    // Now write the current selection from the ppdev back into the twinviewinfo registry structure.
    //
    WriteRegistryTwinViewInfo (ppdev, NULL);

    //
    // Read the current SaveSettings table into ppdev
    //
    vReadSaveSettings(ppdev);

    //
    // Update the registry SaveSettings table with the just concluded modeset.
    //
    vWriteSaveSettings(ppdev);

    //
    // Print the just written SaveSettings table for debugging purposes
    //
    vPrintSaveSettings(ppdev, 2);

    return(TRUE);
}

//
// Returns TRUE if the given head supports the "RequestedMode". "BestFitMode" is not modified.
// Returns FALSE if the given head and does not support the requested mode and returns a best fit mode supported by the device
// which is the nearest match this device actually supports.
// BUGBUG: For now we hardcode a max mode of 1024x768x32ppx60HZ for a DFP and 800x600x32bppx60HZ for TV.
// We assume the CRT can do all modes for now.
//
BOOLEAN bQuerySupportPhysicalMode(PPDEV ppdev, ULONG head, ULONG ulDeviceMask, MODE_ENTRY *RequestedMode, MODE_ENTRY *BestFitMode)
{
    BOOLEAN bRet = TRUE;
    ULONG i;
    MODE_ENTRY *PhysModePtr;
    ULONG ulDeviceType;
    ULONG ulPrimaryHead, ulPrimaryDeviceMask;

    if (ulDeviceMask & BITMASK_ALL_CRT)
    {
        ulDeviceType = MONITOR_TYPE_VGA;
    }
    if (ulDeviceMask & BITMASK_ALL_DFP)
    {
        ulDeviceType = MONITOR_TYPE_FLAT_PANEL;
    }
    if (ulDeviceMask & BITMASK_ALL_TV)
    {
        ulDeviceType = ppdev->ulDefaultTVDeviceType;
    }

    // We will set the BestFitMode to be equal to the Requested mode and then change any fields if need be.
    *BestFitMode = *RequestedMode;

    ulPrimaryHead = ppdev->ulDeviceDisplay[0];
    ulPrimaryDeviceMask = ppdev->ulDeviceMask[ulPrimaryHead];

#if _WIN32_WINNT >= 0x500
    //
    // Since we don't expose TV EDID to GDI, GDI has no idea of existence of TV.  Prune TV anyway.
    //
    if (ulDeviceType != ppdev->ulDefaultTVDeviceType)
    {
        //
        // See whether the display driver or the GDI should do the mode pruning for this head.
        //
        if (bQueryGDIModePruning(ppdev->TwinView_State, ulPrimaryHead, ulPrimaryDeviceMask, 
                head,  ulDeviceMask, ppdev->ACPISystem, ppdev->GDIModePruning))
        {
            //
            // We let GDI do the mode pruning for the EDID
            //
            return(TRUE);
        }
    }
#endif


    switch (ulDeviceType)
    {
        case MONITOR_TYPE_VGA:
        case MONITOR_TYPE_FLAT_PANEL:
            PhysModePtr = ppdev->PhysModeList[head];
            for (i=0; i < ppdev->ulNumPhysModes[head]; i++)
            {
                if (PhysModePtr->ValidMode && 
                    PhysModePtr->Width == RequestedMode->Width &&
                    PhysModePtr->Height == RequestedMode->Height &&
                    PhysModePtr->Depth == RequestedMode->Depth &&
                    PhysModePtr->RefreshRate == RequestedMode->RefreshRate)
                {
                    // Found a match.
                    bRet = TRUE;
                    return(bRet);
                }
                PhysModePtr++;
            }
            // Did not find a match.
            // So backoff to find the next best mode.
            vBackOffMode(ppdev, head, ulDeviceMask, RequestedMode, BestFitMode);
            
            bRet = FALSE;
            break;
        case MONITOR_TYPE_NTSC:
        case MONITOR_TYPE_PAL:
            {
                NV_CFGEX_GET_TV_ENCODER_MULTI_PARAMS TVInfo;
                ULONG ReturnedDataLength;
                DWORD ulRet=0;
                

                // The IOCTL assumes head0.
                ulRet = EngDeviceIoControl(ppdev->hDriver,
                                    IOCTL_VIDEO_GET_TV_ENCODER_TYPE,
                                    NULL,
                                    0,
                                    &TVInfo,
                                    sizeof(TVInfo),
                                    &ReturnedDataLength);
                DISPDBG((2, "TVEncoderType: 0x%x, ID=0x%x, Connected=0x%x",TVInfo.EncoderType, TVInfo.EncoderID, TVInfo.TVConnected));
                if (ulRet)
                {
                    DISPDBG((0, "InitMultiMon() - IOCTL_VIDEO_GET_TV_ENCODER_TYPE failed, returned %d",ulRet));
                    // Set safe default values.
                    BestFitMode->Width  = 800;
                    BestFitMode->Height = 600;
                    bRet = FALSE;
                }
                // Conexant871 == Brooktree871
                else if ( (TVInfo.EncoderType == NV_ENCODER_BROOKTREE_871) && (RequestedMode->Width >= 1024 || RequestedMode->Height >= 768)) 
                {
                    BestFitMode->Width = 1024;
                    BestFitMode->Height = 768;
                    bRet = FALSE;
                }
                else if (RequestedMode->Width > 800 || RequestedMode->Height > 600)
                {
                    BestFitMode->Width  = 800;
                    BestFitMode->Height = 600;
                    bRet = FALSE;
                }
                if (RequestedMode->RefreshRate != 60)
                {
                    BestFitMode->RefreshRate = 60;
                    bRet = FALSE;
                }
                break;
            }

            
        default:
            DISPDBG((1,"bQuerySupportPhysicalMode(): Invalid devicetype: 0x%x\n",ppdev->ulDeviceType[head]));
            BestFitMode->Width = 1024;
            BestFitMode->Height = 768;
            BestFitMode->RefreshRate = 60;
            bRet = FALSE;
            break;
    } //switch


    return(bRet);
}

//
// Finds the nearest match for the requestedMode by backing off in the order - refresh rate, height, width.
// Note: Can be called for any output device type.
//
VOID vBackOffMode(PPDEV ppdev, ULONG ulHead, ULONG ulDeviceMask, MODE_ENTRY *RequestedMode, MODE_ENTRY *BestFitMode)
{
    ULONG i;
    MODE_ENTRY *PhysModePtr;
    DAC_TIMING_VALUES sTimingDac;
    HEAD_RESOLUTION_INFO HeadResolution;
    DWORD ReturnedDataLength;

    // Assume a true match. Then we back off the appropriate attribute.
    BestFitMode->Width = RequestedMode->Width;
    BestFitMode->Height = RequestedMode->Height;
    BestFitMode->Depth = RequestedMode->Depth;
    BestFitMode->RefreshRate = RequestedMode->RefreshRate;

    PhysModePtr = ppdev->PhysModeList[ulHead];
    // sanity check. First see if the mode actually find a true match.
    for (i=0; i < ppdev->ulNumPhysModes[ulHead]; i++)
    {
        if (PhysModePtr->ValidMode && 
            PhysModePtr->Width == RequestedMode->Width &&
            PhysModePtr->Height == RequestedMode->Height &&
            PhysModePtr->Depth == RequestedMode->Depth &&
            PhysModePtr->RefreshRate == RequestedMode->RefreshRate)
        {
            // Found a match.
            return;
        }
        PhysModePtr++;
    }
    
    //
    // Did not find a match. Let the common modeset code FindModeEntry() do the back off for us.
    // Fill up the head resolution info structure used for the common modeset code.
    //
    HeadResolution.ulDisplayWidth = RequestedMode->Width;
    HeadResolution.ulDisplayHeight= RequestedMode->Height;      
    HeadResolution.ulDisplayRefresh = RequestedMode->RefreshRate;   
    
    HeadResolution.ulHead = ulHead;              
    HeadResolution.ulDeviceMask = ulDeviceMask;        
    HeadResolution.ulDeviceType = ppdev->ulDeviceType[ulHead];        
    HeadResolution.ulTVFormat = ppdev->ulTVFormat[ulHead];
    HeadResolution.ulDisplayPixelDepth = RequestedMode->Depth;
    HeadResolution.ulOption = 0;
    

    if (EngDeviceIoControl(ppdev->hDriver,
                           IOCTL_VIDEO_GET_TIMING_DAC,
                           &HeadResolution,  // input buffer
                           sizeof(HEAD_RESOLUTION_INFO),
                           &sTimingDac,
                           sizeof(DAC_TIMING_VALUES),
                           &ReturnedDataLength))
    {
        DISPDBG((0, "nvMultiMon.c - IOCTL_VIDEO_GET_TIMING_DAC failed"));
        BestFitMode->Width = 800;
        BestFitMode->Height = 600;
        BestFitMode->Depth = RequestedMode->Depth;
        BestFitMode->RefreshRate = 60;
        return;
    }

    

    // Use the returned mode as the supported physical mode.
    // BUGBUG: FindModeEntry() could have tweaked either the refresh rate or width or height.
    // Not sure what tweak the user would prefer. So for now, I take whatever FindModeEntry() gives.
    // Fortunately it does not tweak the depth!
    BestFitMode->Width = (USHORT)sTimingDac.HorizontalVisible;
    BestFitMode->Height = (USHORT)sTimingDac.VerticalVisible;
    BestFitMode->Depth = RequestedMode->Depth;
    BestFitMode->RefreshRate = (USHORT)sTimingDac.Refresh;
    return;
}


//
// Frees up any memory alloced by the driver for multimon purposes.
// Should be called from DrvDisablePdev().
//
void FreeMemoryMultiMon(PDEV *ppdev)
{
    ULONG   i;

    for (i=0; i < ppdev->ulNumberDacsOnBoard; i++)
    {
        if (ppdev->PhysModeList[i] != NULL)
        {
            EngFreeMem(ppdev->PhysModeList[i]);
            ppdev->PhysModeList[i] = NULL;
        }
        if (ppdev->VirtualModeList[i] != NULL)
        {
            EngFreeMem(ppdev->VirtualModeList[i]);
            ppdev->VirtualModeList[i] = NULL;
        }
    }
}

//
// Returns true if the twinviewData is safe. Otherwise returns FALSE>
//
BOOLEAN TwinViewDataOK(PPDEV ppdev, NVTWINVIEWDATA *pData)
{
    BOOLEAN bRet = TRUE;
    if (pData->DeskTopRectl.left == 0 && pData->DeskTopRectl.top == 0 && 
        pData->DeskTopRectl.right == 0 && pData->DeskTopRectl.bottom == 0)
    {
        DISPDBG((1,"DeskTopRectl is all zeroed out\n"));
        bRet = FALSE;
    }

    if (pData->dwState != NVTWINVIEW_STATE_SPAN && pData->dwState != NVTWINVIEW_STATE_CLONE &&
        pData->dwState != NVTWINVIEW_STATE_DUALVIEW && pData->dwState != NVTWINVIEW_STATE_NORMAL) 
    {
       DISPDBG((0,"OOps! Invalid State: 0x%x\n", pData->dwState));
       bRet = FALSE;
    }

    if ((pData->dwState == NVTWINVIEW_STATE_NORMAL || pData->dwState == NVTWINVIEW_STATE_DUALVIEW)
        && pData->dwDeviceDisplay[0] > 1)
    {
       DISPDBG((0,"Oops! Invalid Head Mapping.  dwDviceDisplay[0]: 0x%x for NORMAL state\n", pData->dwDeviceDisplay[0]));
       bRet = FALSE;
    }

    if (pData->dwState == NVTWINVIEW_STATE_SPAN || pData->dwState == NVTWINVIEW_STATE_CLONE)
    {
        if (pData->dwDeviceDisplay[0] > 1 || 
            pData->dwDeviceDisplay[1] > 1 ||
            (pData->dwDeviceDisplay[0] == 0 && pData->dwDeviceDisplay[1] == 0) ||
            (pData->dwDeviceDisplay[0] == 1 && pData->dwDeviceDisplay[1] == 1))
        {
           DISPDBG((0,"OOps! Invalid Head Mapping.  dwDeviceDisplay: 0x%x, 0x%x for MULTIMON state\n", 
            pData->dwDeviceDisplay[0], pData->dwDeviceDisplay[1]));
            DISPDBG((0, "Patching the Head Mappings to 0 and 1 respectively\n"));
            // rather than return false, let me try to patch this error.
            pData->dwDeviceDisplay[0] = 0;
            pData->dwDeviceDisplay[1] = 1;
        }
    }

    if (pData->dwState == NVTWINVIEW_STATE_SPAN || pData->dwState == NVTWINVIEW_STATE_CLONE) 
    {
        if (pData->NVTWINVIEWUNION.nvtwdevdata[0].dwBpp != pData->NVTWINVIEWUNION.nvtwdevdata[1].dwBpp)
        {
            DISPDBG((0,"OOps! bit depths differ for head0: %d and head1: %d \n",
                pData->NVTWINVIEWUNION.nvtwdevdata[0].dwBpp,pData->NVTWINVIEWUNION.nvtwdevdata[1].dwBpp));
            bRet = FALSE;
        }
    }


    if (pData->NVTWINVIEWUNION.nvtwdevdata[0].dwBpp == 0 && pData->NVTWINVIEWUNION.nvtwdevdata[1].dwBpp == 0)
    {
       DISPDBG((0,"OOps! bit depths are Zero for both head0 and head1: %d, %d\n",
            pData->NVTWINVIEWUNION.nvtwdevdata[0].dwBpp,pData->NVTWINVIEWUNION.nvtwdevdata[1].dwBpp));
        bRet = FALSE;
    }

    if (pData->NVTWINVIEWUNION.nvtwdevdata[0].dwRefresh == 0 && pData->NVTWINVIEWUNION.nvtwdevdata[1].dwRefresh == 0)
    {
        DISPDBG((0,"OOps! ulRefreshRate is all zeroed out for both heads\n"));
        bRet = FALSE;
    }
    if (pData->NVTWINVIEWUNION.nvtwdevdata[0].VirtualRectl.left == 0 && pData->NVTWINVIEWUNION.nvtwdevdata[0].VirtualRectl.top == 0 
        && pData->NVTWINVIEWUNION.nvtwdevdata[0].VirtualRectl.right == 0 && pData->NVTWINVIEWUNION.nvtwdevdata[0].VirtualRectl.bottom == 0 
        && pData->NVTWINVIEWUNION.nvtwdevdata[1].VirtualRectl.left == 0 && pData->NVTWINVIEWUNION.nvtwdevdata[1].VirtualRectl.top == 0 
        && pData->NVTWINVIEWUNION.nvtwdevdata[1].VirtualRectl.right == 0 && pData->NVTWINVIEWUNION.nvtwdevdata[1].VirtualRectl.bottom == 0)
    {
        DISPDBG((0,"OOps! VirtualRectl is all zeroed out for both heads\n"));
        bRet = FALSE;
    }

    if (pData->NVTWINVIEWUNION.nvtwdevdata[0].PhysicalRectl.left == 0 && pData->NVTWINVIEWUNION.nvtwdevdata[0].PhysicalRectl.top == 0 
        && pData->NVTWINVIEWUNION.nvtwdevdata[0].PhysicalRectl.right == 0 && pData->NVTWINVIEWUNION.nvtwdevdata[0].PhysicalRectl.bottom == 0 
        && pData->NVTWINVIEWUNION.nvtwdevdata[1].PhysicalRectl.left == 0 && pData->NVTWINVIEWUNION.nvtwdevdata[1].PhysicalRectl.top == 0 
        && pData->NVTWINVIEWUNION.nvtwdevdata[1].PhysicalRectl.right == 0 && pData->NVTWINVIEWUNION.nvtwdevdata[1].PhysicalRectl.bottom == 0)
    {
        DISPDBG((0,"OOps! PhysicalRectl is all zeroed out for both heads\n"));
        bRet = FALSE;
    }

    // If there is just one head on the board and the registry says any mode other than normal, mark it as invalid.
    if (ppdev)
    {

        if (ppdev->ulNumberDacsOnBoard == 1 &&
            (pData->dwState == NVTWINVIEW_STATE_SPAN || pData->dwState == NVTWINVIEW_STATE_CLONE))
        {
            DISPDBG((0,"OOps! Registry state is invalid for the number of heads on the board or for the number of connected heads\n"));
            bRet = FALSE;
        }
    }

    return(bRet);
}



//
// Handle the case when the user makes some selection through nvPanel causing the rgistry entry "TwinViewInfo" 
// to be written and then uses the NT panel (or programmatically through an app) 
// to make mode changes (refresh rate, color depth, resolution, choosing a
// spanning mode resolution). So we need to sync the NT and nvPanel now.
// This function does not modify the 'pdm' or the 'pdev'. It Returns TRUE if the NTPanel selections and the
// nvPanel's selections are identical else returns FALSE.
//
BOOLEAN bQueryBothPanelsAreInSync(PPDEV ppdev, NVTWINVIEWDATA *pData)
{
    ULONG NumberDacsOnBoard;
    ULONG i;
    ULONG NTWidth, NTHeight, nvWidth, nvHeight; // Width, height of the NT panel selections and nvPanel selections.
    ULONG ulHeadPrimary, ulHead;
    MODE_ENTRY RequestedMode, BestFitMode;

    DISPDBG((5,"Enter bQueryBothPanelsAreInSync()\n"));
    ulHeadPrimary = pData->dwDeviceDisplay[0];

    // Now see if the registry data needs to be synced.
    // If the color depth, primary refresh rate, width and height are the same as in the registry, then the registry data
    // is already in sync and we don't need to do anything.
    if ((ULONG)ppdev->cBitsPerPel == pData->NVTWINVIEWUNION.nvtwdevdata[ulHeadPrimary].dwBpp &&
        (ULONG)ppdev->ulGDIRefreshRate == pData->NVTWINVIEWUNION.nvtwdevdata[ulHeadPrimary].dwRefresh &&
        ppdev->cxScreen == (pData->DeskTopRectl.right - pData->DeskTopRectl.left) &&
        ppdev->cyScreen == (pData->DeskTopRectl.bottom - pData->DeskTopRectl.top))
    {
        DISPDBG((5,"The registry data and the GDI data are in sync.\n"));
        return(TRUE);
    }

    // Pull out theright pieces of info from GDI into the registry structure
    //
    // GDI _solely_ determines the following pieces of info
    // DEsktopRectl, depth for both heads
    // Head0: bpp, Refresh rate, VirtualRect
    // Head1: bpp, VirtualRect
    //
    // GDI _may_ determine the following
    // Head0: physical rect
    // Head1: physical rect
    //
    // Registry _soley_ determines the following pieces of info
    // dwState, dwOrientation
    //
    // Registry _may_ determine the following pieces of info
    // Head0: physical rect
    // head1: physical rect, refresh rate

    // see if the color depth has changed for the primary head.
    if ((ULONG)ppdev->cBitsPerPel != pData->NVTWINVIEWUNION.nvtwdevdata[ulHeadPrimary].dwBpp)
    {
        for (i=0; i < ppdev->ulNumberDacsOnBoard; i++)
        {
            ulHead = pData->dwDeviceDisplay[i];
            pData->NVTWINVIEWUNION.nvtwdevdata[ulHead].dwBpp = (ULONG)ppdev->cBitsPerPel;
        }
    }

    // see if the refresh rate has changed for the primary head.
    if ((ULONG)ppdev->ulGDIRefreshRate != pData->NVTWINVIEWUNION.nvtwdevdata[ulHeadPrimary].dwRefresh)
    {
        pData->NVTWINVIEWUNION.nvtwdevdata[ulHeadPrimary].dwRefresh = (ULONG)ppdev->ulGDIRefreshRate;
    }

    // See if the resolution has changed. If so, we need to change each head's virtual rectl and also
    // carefully change each head's physical rect.
    if (ppdev->cxScreen != (pData->DeskTopRectl.right - pData->DeskTopRectl.left) ||
        ppdev->cyScreen != (pData->DeskTopRectl.bottom - pData->DeskTopRectl.top))
    {
        //
        // TODO: GK: Do a sanity check of the ulDeviceDisplay in pData. If it is not sane, set the default 0,1 values
        // into the pData
        //
        SET_RECT(&pData->DeskTopRectl, 0, 0, ppdev->cxScreen, ppdev->cyScreen);

        // set up the virtualrect for all the heads corresponding to the state.
        // Check to see if it is horizontal orientation
        if (HORIZONTAL_MODE(ppdev->cxScreen, ppdev->cyScreen)) 
        {
            if (pData->dwState != NVTWINVIEW_STATE_SPAN  || 
                pData->dwOrientation != HORIZONTAL_ORIENTATION)
            {
                DISPDBG((0,"Oops! Invalid state: 0x%x or orientation: 0x%x for requested GDI resolution (%d, %d)\n",
                    pData->dwState,pData->dwState, ppdev->cxScreen, ppdev->cyScreen));
                DISPDBG((0, "Patch it up by setting the correct state and orientation in pdev\n"));
                pData->dwState = NVTWINVIEW_STATE_SPAN;
                pData->dwState = HORIZONTAL_ORIENTATION;
            }

            RequestedMode.Width = (USHORT)ppdev->cxScreen / 2;
            RequestedMode.Height = (USHORT)ppdev->cyScreen;
            RequestedMode.Depth = (USHORT)ppdev->cBitsPerPel;
            RequestedMode.RefreshRate = (USHORT)ppdev->ulGDIRefreshRate;

            //
            // Fill in the info for primary device
            //

            //
            // TODO: GK: Do a sanity check of the ulDeviceDisplay in pData. If it is not sane, set the default 0,1 values
            // into the pData
            //

            ulHead = pData->dwDeviceDisplay[0];
            SET_RECT(&pData->NVTWINVIEWUNION.nvtwdevdata[ulHead].VirtualRectl, 0, 0, ppdev->cxScreen / 2, ppdev->cyScreen);
            if (bQuerySupportPhysicalMode(ppdev, ulHead, ppdev->ulDeviceMask[ulHead], &RequestedMode, &BestFitMode))
            {
                SET_RECT(&pData->NVTWINVIEWUNION.nvtwdevdata[ulHead].PhysicalRectl, 0, 0,
                    RequestedMode.Width, RequestedMode.Height);
                pData->NVTWINVIEWUNION.nvtwdevdata[ulHead].dwRefresh = RequestedMode.RefreshRate;
            }
            else
            {
                SET_RECT(&pData->NVTWINVIEWUNION.nvtwdevdata[ulHead].PhysicalRectl, 0, 0, 
                    BestFitMode.Width, BestFitMode.Height);
                pData->NVTWINVIEWUNION.nvtwdevdata[ulHead].dwRefresh = BestFitMode.RefreshRate;
            }

            //
            // Fill in the info for secondary device
            //
            ulHead = pData->dwDeviceDisplay[1];
            SET_RECT(&pData->NVTWINVIEWUNION.nvtwdevdata[ulHead].VirtualRectl, ppdev->cxScreen / 2, 0, 
                (ppdev->cxScreen / 2) + (ppdev->cxScreen / 2), ppdev->cyScreen);
            if (bQuerySupportPhysicalMode(ppdev, ulHead, ppdev->ulDeviceMask[ulHead], &RequestedMode, &BestFitMode))
            {
                SET_RECT(&pData->NVTWINVIEWUNION.nvtwdevdata[ulHead].PhysicalRectl, ppdev->cxScreen / 2, 0,
                    (ppdev->cxScreen / 2) + RequestedMode.Width, RequestedMode.Height);
                pData->NVTWINVIEWUNION.nvtwdevdata[ulHead].dwRefresh = RequestedMode.RefreshRate;
            }
            else
            {
                SET_RECT(&pData->NVTWINVIEWUNION.nvtwdevdata[ulHead].PhysicalRectl, ppdev->cxScreen / 2, 0,
                    (ppdev->cxScreen / 2) + BestFitMode.Width, BestFitMode.Height);
                pData->NVTWINVIEWUNION.nvtwdevdata[ulHead].dwRefresh = BestFitMode.RefreshRate;
            }
        } // HORIZONTAL_MODE

        // check to see if it is vertical orientation
        if (VERTICAL_MODE(ppdev->cxScreen, ppdev->cyScreen)) 
        {   
            if (pData->dwState != NVTWINVIEW_STATE_SPAN  || 
                pData->dwOrientation != VERTICAL_ORIENTATION)
            {
                DISPDBG((0,"Oops! Invalid state: 0x%x or orientation: 0x%x for requested GDI resolution (%d, %d)\n",
                    pData->dwState,pData->dwOrientation, ppdev->cxScreen, ppdev->cyScreen));
                DISPDBG((0, "Patch it up by setting the correct state and orientation in pdev\n"));
                pData->dwState = NVTWINVIEW_STATE_SPAN;
                pData->dwOrientation = VERTICAL_ORIENTATION;
            }

            RequestedMode.Width = (USHORT)ppdev->cxScreen;
            RequestedMode.Height =(USHORT) ppdev->cyScreen / 2;
            RequestedMode.Depth = (USHORT)ppdev->cBitsPerPel;
            RequestedMode.RefreshRate = (USHORT)ppdev->ulGDIRefreshRate;

            //
            // Fill in the info for primary device
            //
            ulHead = pData->dwDeviceDisplay[0];
            SET_RECT(&pData->NVTWINVIEWUNION.nvtwdevdata[ulHead].VirtualRectl, 0, 0, 
                ppdev->cxScreen, ppdev->cyScreen / 2);
            if (bQuerySupportPhysicalMode(ppdev, ulHead, ppdev->ulDeviceMask[ulHead], &RequestedMode, &BestFitMode))
            {
                SET_RECT(&pData->NVTWINVIEWUNION.nvtwdevdata[ulHead].PhysicalRectl, 0, 0,
                    RequestedMode.Width, RequestedMode.Height);
                pData->NVTWINVIEWUNION.nvtwdevdata[ulHead].dwRefresh = RequestedMode.RefreshRate;
            }
            else
            {
                SET_RECT(&pData->NVTWINVIEWUNION.nvtwdevdata[ulHead].PhysicalRectl, 0, 0,
                    BestFitMode.Width, BestFitMode.Height);
                pData->NVTWINVIEWUNION.nvtwdevdata[ulHead].dwRefresh = BestFitMode.RefreshRate;
            }

            //
            // Fill in the info for secondary device
            //
            ulHead = pData->dwDeviceDisplay[1];
            SET_RECT(&pData->NVTWINVIEWUNION.nvtwdevdata[ulHead].VirtualRectl, 0, ppdev->cyScreen / 2,
                ppdev->cxScreen, (ppdev->cyScreen / 2) + (ppdev->cyScreen / 2));
            if (bQuerySupportPhysicalMode(ppdev, ulHead, ppdev->ulDeviceMask[ulHead], &RequestedMode, &BestFitMode))
            {
                SET_RECT(&pData->NVTWINVIEWUNION.nvtwdevdata[ulHead].PhysicalRectl, 0, ppdev->cyScreen / 2,
                    RequestedMode.Width, (ppdev->cyScreen / 2) + RequestedMode.Height);
                pData->NVTWINVIEWUNION.nvtwdevdata[ulHead].dwRefresh = RequestedMode.RefreshRate;
            }
            else
            {
                SET_RECT(&pData->NVTWINVIEWUNION.nvtwdevdata[ulHead].PhysicalRectl, 0, ppdev->cyScreen / 2,
                    BestFitMode.Width,(ppdev->cyScreen / 2) + BestFitMode.Height);
                pData->NVTWINVIEWUNION.nvtwdevdata[ulHead].dwRefresh = BestFitMode.RefreshRate;
            }
        } // VERTICAL_MODE

        if (!MULTIMON_MODE(ppdev->cxScreen, ppdev->cyScreen)) 
        {   
            // Note we could still be in the state of multimon except the user has chosen the 6x4 or 8x6 mode.

            RequestedMode.Width = (USHORT)ppdev->cxScreen;
            RequestedMode.Height = (USHORT)ppdev->cyScreen;
            RequestedMode.Depth = (USHORT)ppdev->cBitsPerPel;
            RequestedMode.RefreshRate = (USHORT)ppdev->ulGDIRefreshRate;

            //
            // Fill in the info for primary device
            //
            ulHead = pData->dwDeviceDisplay[0];
            SET_RECT(&pData->NVTWINVIEWUNION.nvtwdevdata[ulHead].VirtualRectl, 0, 0, ppdev->cxScreen, ppdev->cyScreen);
            if (bQuerySupportPhysicalMode(ppdev, ulHead, ppdev->ulDeviceMask[ulHead], &RequestedMode, &BestFitMode))
            {
                SET_RECT(&pData->NVTWINVIEWUNION.nvtwdevdata[ulHead].PhysicalRectl, 0, 0, 
                    RequestedMode.Width, RequestedMode.Height);
                pData->NVTWINVIEWUNION.nvtwdevdata[ulHead].dwRefresh = RequestedMode.RefreshRate;
            }
            else
            {
                SET_RECT(&pData->NVTWINVIEWUNION.nvtwdevdata[ulHead].PhysicalRectl, 0, 0, 
                    BestFitMode.Width,BestFitMode.Height);
                pData->NVTWINVIEWUNION.nvtwdevdata[ulHead].dwRefresh = BestFitMode.RefreshRate;
            }

            // fill up the second head info also. It will be used in clone mode but ignored in the normal mode.
            if (pData->dwState == NVTWINVIEW_STATE_CLONE && ppdev->ulNumberDacsOnBoard > 1)
            {
                ulHead = pData->dwDeviceDisplay[1];
                SET_RECT(&pData->NVTWINVIEWUNION.nvtwdevdata[ulHead].VirtualRectl, 0, 0, ppdev->cxScreen, ppdev->cyScreen);
                if (bQuerySupportPhysicalMode(ppdev, ulHead, ppdev->ulDeviceMask[ulHead], &RequestedMode, &BestFitMode))
                {
                    SET_RECT(&pData->NVTWINVIEWUNION.nvtwdevdata[ulHead].PhysicalRectl, 0, 0, 
                        RequestedMode.Width,RequestedMode.Height);
                    pData->NVTWINVIEWUNION.nvtwdevdata[ulHead].dwRefresh = RequestedMode.RefreshRate;
                }
                else
                {
                    SET_RECT(&pData->NVTWINVIEWUNION.nvtwdevdata[ulHead].PhysicalRectl, 0, 0, 
                        BestFitMode.Width,BestFitMode.Height);
                    pData->NVTWINVIEWUNION.nvtwdevdata[ulHead].dwRefresh = BestFitMode.RefreshRate;
                }
            }
        } // single screen resolution
    } // if the GDI resolution is different than registry resolution
    

    return(TRUE);
}


//
// Returns TRUE if the RequestedDeviceType (and the requested TV type in the case of TV) is actually present and
// connected on the 'Head'. If not, returns FALSE and returns a safe valid device type in the *SuggestedDeviceType field.
//
BOOLEAN bConfirmDeviceType(PPDEV ppdev, ULONG Head, ULONG RequestedDeviceMask, ULONG RequestedDeviceType, ULONG RequestedTVType,
                           ULONG * SuggestedDeviceMask, ULONG * SuggestedDeviceType, ULONG * SuggestedTVType)
{
    BOOLEAN bRet = FALSE;
    BOOLEAN bFoundSuggested = FALSE;

    // We ignore the ulDeviceType arguement here.
    if (ppdev->ulConnectedDeviceMask & RequestedDeviceMask)
    {
        return(TRUE);
    }
    else
    {
        *SuggestedDeviceMask = ppdev->ulDeviceMask[Head];
        *SuggestedTVType = ppdev->ulTVFormat[Head];
        return(FALSE);
    }

} // End function bConfirmDeviceType()

//
// This functions returns the physical modelist and virtual modelist for each head.
// Returns TRUE if successful else reurns FALSE.
// if the ModeList pointer is NULL, then only the number of modes is returned, else the actual modelist is returned 
// for the passed in NumModes.
//
BOOLEAN bGetPhysicalVirtualModeList(PPDEV ppdev, ULONG Head, ULONG DeviceMask, ULONG DeviceType, ULONG TVFormat,
        MODE_ENTRY *PhysModeList, MODE_ENTRY *VirtModeList, ULONG *NumPhysModes, ULONG *NumVirtModes)
{
    MODE_ENTRY * SrcPtr;
    ULONG DoSanityCheck = 1;
    ULONG j;
    BOOLEAN bRet = TRUE;
    ULONG ulTmp, ulRet, ulRetSize;
    
    // For the requested devicetype, get the number of virtual modes and fillup the virtual modelist pointer if asked for.
    if (NumVirtModes != NULL)
    {
        *NumVirtModes = ppdev->ulNumVirtualModes[Head];
        //
        // Fill up the virtual modelist.
        // This is device type independent.
        //
        if (VirtModeList != NULL)
        {
            SrcPtr = ppdev->VirtualModeList[Head];
            for (j=0; j < ppdev->ulNumVirtualModes[Head]; j++)
            {
                *VirtModeList = *SrcPtr;
                VirtModeList++;
                SrcPtr++;
            }
        }
    } // NumVirtModes != NULL

    //
    // Fill up the Physical modelist.
    // This is device type dependent.
    // We start off with the virtual modelist and eliminate those modes not supported by this device. Eventually we will do
    // EDID filtering in this chunk of code.
    //
    if (NumPhysModes != NULL)
    {
        HEAD_MODE_LIST_INFO InputInfo;
        

        InputInfo.ulHead = Head;
        InputInfo.ulDeviceMask = DeviceMask;
        InputInfo.ulDeviceType = DeviceType;
        InputInfo.ulTVType = TVFormat;
        InputInfo.ulEnableDDC = ppdev->ulEnableDDC[Head];
        InputInfo.ulEnableDDC = 1; // Hardcoded for now till control panel adds a button for this.

        InputInfo.pVirtualModeList = ppdev->VirtualModeList[Head];
        InputInfo.ulNumVirtualModes = ppdev->ulNumVirtualModes[Head];
        InputInfo.pPhysicalModeList = PhysModeList;

        // Call the IOCTL for the miniport to return the physical modes.
        ulRet = EngDeviceIoControl(ppdev->hDriver,
                                   IOCTL_VIDEO_GET_HEAD_PHYSICAL_MODE_LIST,
                                   (HEAD_MODE_LIST_INFO *)&InputInfo,
                                   sizeof(HEAD_MODE_LIST_INFO),
                                   NULL,
                                   0,
                                   &(ulRetSize));
        if (ulRet)
        {
            DISPDBG((0,"Oops! IOCTL_VIDEO_GET_HEAD_PHYSICAL_MODE_LIST failed!"));
            return(FALSE);
        }
        else
        {
            // Copy the number physical modes into ppdev. The modelist has already been copied into 
            // by the miniport.
            *NumPhysModes = ulRetSize / sizeof(MODE_ENTRY);
            if (*NumPhysModes == 0)
            {
                DISPDBG((0,"Number of physical modes for head: %d is 0",Head));
                return(FALSE);
            }
            return(TRUE);
        }

    } // NumPhysModes != NULL

    return(bRet);
} // end bGetPhysicalVirtualModeList()


//
// Prints the relevant pieces of TwinView related info from the ppdev
//
void PrintPPDEVTwinViewInfo (char *Msg, PPDEV ppdev)
{
    ULONG i;

    DISPDBG((1,".......%s......",Msg));
    DISPDBG((1,"DeskTopMode: 0x%x, NotSupportPanScan: 0x%x, PuntCursor: 0x%x, DacsOnBoard: 0x%x, DacsConnected: 0x%x, DacsActive: 0x%x",
        ppdev->ulDesktopMode, ppdev->ulDriverNotSupportPanScan, ppdev->ulPuntCursorToGDI, ppdev->ulNumberDacsOnBoard,
        ppdev->ulNumberDacsConnected,ppdev->ulNumberDacsActive));
    DISPDBG((1,"TwinViewState: 0x%x, Orientation: 0x%x, DisplayDX: 0x%x, DisplayDy: 0x%x, PrimarySurfOffset: 0x%x",
        ppdev->TwinView_State, ppdev->TwinView_Orientation, ppdev->ulMoveDisplayDx, ppdev->ulMoveDisplayDy,
        ppdev->ulPrimarySurfaceOffset));
    DISPDBG((1,"GDI Requested mode: (%d, %d) at %d bpp, at %d HZ",
        ppdev->cxScreen, ppdev->cyScreen, ppdev->cBitsPerPel, ppdev->ulGDIRefreshRate));
    DISPDBG((1,"Head Mapping: %d, %d",ppdev->ulDeviceDisplay[0], ppdev->ulDeviceDisplay[1]));
    DISPDBG((1,"HeadDeviceOptions: 0x%x, 0x%x",ppdev->ulHeadDeviceOptions[0], ppdev->ulHeadDeviceOptions[1]));
    DISPDBG((1,"AllDeviceMask: 0x%x\n",ppdev->ulAllDeviceMask));
    DISPDBG((1,"ConnectedDeviceMask: 0x%x\n",ppdev->ulConnectedDeviceMask));

    for (i=0; i < ppdev->ulNumberDacsOnBoard; i++)
    {
        DISPDBG((1,"Head %d: DacStatus: 0x%x, NumPhysModes: 0x%x, NumVirtualModes: 0x%x, DeviceMask: 0x%x, DeviceType: 0x%x, TVFormat: 0x%x",
            i, ppdev->ulDacStatus[i], ppdev->ulNumPhysModes[i], ppdev->ulNumVirtualModes[i], ppdev->ulDeviceMask[i], ppdev->ulDeviceType[i],
            ppdev->ulTVFormat[i]));
        DISPDBG((1, "Virtual: (%d, %d), (%d, %d), Physical: (%d, %d), (%d, %d) at %d HZ",
            ppdev->rclDisplayDesktop[i].left, ppdev->rclDisplayDesktop[i].top,
            ppdev->rclDisplayDesktop[i].right,ppdev->rclDisplayDesktop[i].bottom,    
            ppdev->rclCurrentDisplay[i].left, ppdev->rclCurrentDisplay[i].top,
            ppdev->rclCurrentDisplay[i].right,ppdev->rclCurrentDisplay[i].bottom,    
            ppdev->ulRefreshRate[i]));

    }
    DISPDBG((1,"................................."));
}

//
// Prints the entire twinviewdata structure
//
void PrintTwinViewData (char *Msg, ULONG ulDebugLevel, NVTWINVIEWDATA *pData, PPDEV ppdev)
{
    ULONG i;

    DISPDBG((ulDebugLevel,"..........%s...........", Msg));
    DISPDBG((ulDebugLevel,"State: 0x%x, Orientation: 0x%x, DeskTopRectl: (%d, %d), (%d, %d), AllDeviceMask: 0x%x, ConnectedDevMask: 0x%x, Head Mapping: %d, %d",
        pData->dwState, pData->dwOrientation, pData->DeskTopRectl.left,pData->DeskTopRectl.top,pData->DeskTopRectl.right,
        pData->DeskTopRectl.bottom, pData->dwAllDeviceMask, pData->dwConnectedDeviceMask, pData->dwDeviceDisplay[0],pData->dwDeviceDisplay[1]));
    for (i=0; i < ppdev->ulNumberDacsOnBoard; i++)
    {
        DISPDBG((ulDebugLevel,"Head %d:    DeviceMask: 0x%x, DeviceType: 0x%x, TVFormat: 0x%x, NumPhysModes: 0x%x, NumVirtualModes: 0x%x",
            i, pData->NVTWINVIEWUNION.nvtwdevdata[i].dwDeviceMask, pData->NVTWINVIEWUNION.nvtwdevdata[i].dwDeviceType, pData->NVTWINVIEWUNION.nvtwdevdata[i].dwTVFormat, 
            pData->NVTWINVIEWUNION.nvtwdevdata[i].dwNumPhysModes,pData->NVTWINVIEWUNION.nvtwdevdata[i].dwNumVirtualModes));
        DISPDBG((ulDebugLevel,"        Virtual: (%d, %d), (%d, %d), Physical: (%d, %d), (%d, %d)",
            pData->NVTWINVIEWUNION.nvtwdevdata[i].VirtualRectl.left,pData->NVTWINVIEWUNION.nvtwdevdata[i].VirtualRectl.top,
            pData->NVTWINVIEWUNION.nvtwdevdata[i].VirtualRectl.right,pData->NVTWINVIEWUNION.nvtwdevdata[i].VirtualRectl.bottom,
            pData->NVTWINVIEWUNION.nvtwdevdata[i].PhysicalRectl.left,pData->NVTWINVIEWUNION.nvtwdevdata[i].PhysicalRectl.top,
            pData->NVTWINVIEWUNION.nvtwdevdata[i].PhysicalRectl.right,pData->NVTWINVIEWUNION.nvtwdevdata[i].PhysicalRectl.bottom));
        DISPDBG((ulDebugLevel,"        %d bpp, %d HZ",
            pData->NVTWINVIEWUNION.nvtwdevdata[i].dwBpp, pData->NVTWINVIEWUNION.nvtwdevdata[i].dwRefresh));
    }
    DISPDBG((ulDebugLevel,"........................"));
}

//
// Calls the miniport to do a device scaning.
// Then gets __only__ the updated device options matrix and stuffs it into the panel. 
// BUGBUG: Should I extract all the info? Think about it and talk to Terry. What happens if say the seconda\ry DFP
// is disconnected before inducing the scan? The mode has already been set.
//
void InduceAndUpdateDeviceScanning (PPDEV ppdev)
{
    ULONG  ReturnedDataLength;
    ULONG  i;
    GET_NUMBER_DACS   NumberDacs;

    if (EngDeviceIoControl(ppdev->hDriver,
                           IOCTL_VIDEO_INDUCE_HEAD_DEVICE_SCANNING,
                           NULL,
                           0,
                           NULL,
                           0,
                           &ReturnedDataLength))
    {
        DISPDBG((0, "Oops! DrvEnablePDEV - IOCTL_VIDEO_INDUCE_HEAD_DEVICE_SCANNING failed"));
        return;
    }

    // Get the head info from the miniport such as number of heads on board, number of heads connected and device type of each head.
    if (EngDeviceIoControl(ppdev->hDriver,
                        IOCTL_VIDEO_GET_NUMBER_DACS,
                        NULL,
                        0,
                        &NumberDacs,
                        sizeof(NumberDacs),
                        &ReturnedDataLength))
    {
        DISPDBG((0, "Oops! InitMultiMon() - IOCTL_VIDEO_GET_NUMBER_DACS failed"));
        return;
    }
    else
    {
        // Fill the info into ppdev.
        // BUGBUG: Should I extract all the info? Think about it and talk to Terry. What happens if say the seconda\ry DFP
        // is disconnected before inducing the scan? The mode has already been set.
        ppdev->ulNumberDacsConnected = NumberDacs.ulNumberDacsConnected;
        ppdev->ulNumberDacsOnBoard = NumberDacs.ulNumberDacsOnBoard;
        ppdev->ulAllDeviceMask = NumberDacs.ulAllDeviceMask;
        ppdev->ulConnectedDeviceMask = NumberDacs.ulConnectedDeviceMask;
        DISPDBG((1,"NumDacsOnBoard: 0x%x, NumDacsConnected: 0x%x",ppdev->ulNumberDacsOnBoard,
                 ppdev->ulNumberDacsConnected));
        DISPDBG((1,"ulAllDeviceMask: 0x%x, ulConnectedDeviceMask: 0x%x",ppdev->ulAllDeviceMask,
                 ppdev->ulConnectedDeviceMask));
        for (i=0; i < NumberDacs.ulNumberDacsOnBoard; i++)
        {
            ppdev->ulHeadDeviceOptions[i] = NumberDacs.ulHeadDeviceOptions[i];
            DISPDBG((1,"Physical Head: %d, DeviceOptions: 0x%x",i,ppdev->ulHeadDeviceOptions[i]));
        }
    }

}

//
// Some of the flags in the TwinViewInfo structure are synchronous. i.e., they are read into the incore ppdev
// immediately rather than being read in at the modeset time. These synchronous flags are:
// - ulEnablePanScan
// - ulFreezePanScan
// - ulEnableDDC
//
void   ProcessSynchronousFlags(PPDEV ppdev, NVTWINVIEWDATA *pData)
{
    ULONG i;
    NVTWINVIEW_DEVICE_TYPE_DATA HeadData;
    ULONG PhysWidth, PhysHeight;
    ULONG ReturnedDataLength;

    for (i=0; i< NV_NO_DACS; i++)
    {
        HeadData = pData->NVTWINVIEWUNION.nvtwdevdata[i];
        ppdev->ulEnablePanScan[i] = HeadData.dwEnablePanScan;
        ppdev->ulEnableDDC[i] = HeadData.dwEnableDDC;
        ppdev->ulEnableDDC[i] = 1; // Hardcoded for now till control panel adds a button for this.
        ppdev->ulFreezePanScan[i] = HeadData.dwFreezePanScan;
        ppdev->ulTimingOverRide[i] = HeadData.dwTimingOverRide;

        if (HeadData.dwFreezePanScan == 0)   
        {
            DISPDBG((3, "Enabling PanScan for Head: %d",i));
            if (i == 0)
            {
                ppdev->ulDesktopMode |= NV_VIRTUAL_DESKTOP_1;
            }
            else
            {
                ppdev->ulDesktopMode |= NV_VIRTUAL_DESKTOP_2;
            }
    
        } 
        else if( ppdev->ulDesktopMode & (i ? NV_VIRTUAL_DESKTOP_2 : NV_VIRTUAL_DESKTOP_1))
        {
            DISPDBG((3, "Disabling PanScan for Head: %d",i));

            //
            // Write the desired freeze location into ppdev.
            //
            ppdev->ulFreezeOriginX[i] = HeadData.dwFreezeOriginX;
            ppdev->ulFreezeOriginY[i] = HeadData.dwFreezeOriginY;
            //
            // Update the physical rectangle.
            //
            PhysWidth = ppdev->rclCurrentDisplay[i].right - ppdev->rclCurrentDisplay[i].left;
            PhysHeight = ppdev->rclCurrentDisplay[i].bottom - ppdev->rclCurrentDisplay[i].top;
            ppdev->rclCurrentDisplay[i].left = HeadData.dwFreezeOriginX;
            ppdev->rclCurrentDisplay[i].top = HeadData.dwFreezeOriginY;
            ppdev->rclCurrentDisplay[i].right = ppdev->rclCurrentDisplay[i].left + PhysWidth;
            ppdev->rclCurrentDisplay[i].bottom = ppdev->rclCurrentDisplay[i].top + PhysHeight;
            //
            // Move the pointer (and the scanout area) to the desired origin if we are not in software cursor mode.
            //
            if (ppdev->ulDriverNotSupportPanScan == FALSE)
            {
                NVMovePointer(ppdev, HeadData.dwFreezeOriginX, HeadData.dwFreezeOriginY);
            }
            //
            // Now turn off the virtual desktop flags to disable pan-scan in the pointer routines.
            //
            if (i == 0)
            {
                ppdev->ulDesktopMode &= ~(NV_VIRTUAL_DESKTOP_1);
            }
            else
            {
                ppdev->ulDesktopMode &= ~(NV_VIRTUAL_DESKTOP_2);
            }
        }
    } // for each dac

    // Get the timing override to take effect immediately.
    if (EngDeviceIoControl(ppdev->hDriver,
                           IOCTL_VIDEO_INDUCE_TIMING_OVERRIDE,
                           NULL,
                           0,
                           NULL,
                           0,
                           &ReturnedDataLength))
    {
        DISPDBG((0, "Oops! DrvEnablePDEV - IOCTL_VIDEO_INDUCE_TIMING_OVERRIDE failed"));
        return;
    }

}


//
// Does a one time building of mode lists. Should be called only from InitMultiMon().
//
BOOL BuildModeLists(PPDEV ppdev)
{
    PVIDEO_MODE_INFORMATION pVideoModeInformation, SrcPtr;
    DWORD                   cbModeSize;
    ULONG cModes;
    ULONG i, j;
    MODE_ENTRY *ModeListPtr;
    ULONG ReturnedDataLength;
    ULONG ulHead;
    DWORD cNumGDIModes;
    
   
    // Now construc the per-head virtual and physical mode lists.
    // The modes returned by getAvailableModes() will have all of the single,horizontal and vertical modes.
    cModes = getAvailableModes(ppdev->hDriver,
                            (PVIDEO_MODE_INFORMATION *) &pVideoModeInformation,
                            &cbModeSize, &cNumGDIModes);

    if (cModes == 0 || cNumGDIModes == 0)
    {
        // Something wrong. There were no valid modes.
        DISPDBG((0, "BuildModeLists(): error. Invalid cModes: %d, cNumGDIModes: %d",cModes, cNumGDIModes));
        // free up any previously alloced memory. This can happen if BuildModeLists() is called twice in a row.
        FreeMemoryMultiMon(ppdev);
        return(FALSE);
    }
    

    // free up any previously alloced memory. This can happen if BuildModeLists() is called twice in a row.
    FreeMemoryMultiMon(ppdev);


    //
    // Create the single screen virtual mode lists for both heads
    // Note that we need to create virtual mode lists for _both_ heads since this list is not
    // EDID-specific. However, we need to compute the physical mode list for the active heads only.
    //
    for (i=0; i < ppdev->ulNumberDacsOnBoard; i++)
    {
        ulHead = ppdev->ulDeviceDisplay[i];
        // Read the virtual single head modes into the VirtualModeList.
        ppdev->VirtualModeList[ulHead] = EngAllocMem(FL_ZERO_MEMORY,
                                   cModes * sizeof(MODE_ENTRY),
                                   ALLOC_TAG);
        if (ppdev->VirtualModeList[ulHead] == NULL)
        {
            DISPDBG((0, "Oops! InitMultiMon(): EngAllocMEm failed!\n"));
            //
            // free up any memory we might have alloced in the loop before this failure.
            //
            FreeMemoryMultiMon(ppdev);
            return(FALSE);
        }

        ppdev->ulNumVirtualModes[ulHead] = 0;
        ModeListPtr = ppdev->VirtualModeList[ulHead];
        SrcPtr = pVideoModeInformation;
        for (j=0; j < cModes; j++) 
        {
            // We select only the single screen virtual modes because thats what the panel wants.
            if (MULTIMON_MODE(SrcPtr->VisScreenWidth, SrcPtr->VisScreenHeight)) 
            {
                SrcPtr++;
                continue;
            }
            ModeListPtr->ValidMode = 1;
            ModeListPtr->Width = (USHORT)SrcPtr->VisScreenWidth;
            ModeListPtr->Height = (USHORT)SrcPtr->VisScreenHeight;
            ModeListPtr->Depth = (USHORT) (SrcPtr->BitsPerPlane * SrcPtr->NumberOfPlanes);
            ModeListPtr->RefreshRate = (USHORT)SrcPtr->Frequency;
            ppdev->ulNumVirtualModes[ulHead]++;
            ModeListPtr++;
            SrcPtr++;
        }
    } // for

    // Create the Physical mode lists for the active heads.
    for (i=0; i < ppdev->ulNumberDacsOnBoard; i++)
    {
        ppdev->PhysModeList[i] = EngAllocMem(FL_ZERO_MEMORY,
                                   cModes * sizeof(MODE_ENTRY),
                                   ALLOC_TAG);
        if (ppdev->PhysModeList[i] == NULL)
        {
            DISPDBG((0, "Oops! InitMultiMon(): EngAllocMEm failed!\n"));
            //
            // free up any memory we might have alloced in the loop before this failure.
            //
            FreeMemoryMultiMon(ppdev);
            return(FALSE);
        }
        ppdev->ulNumPhysModes[i] = 0;
    }

    for (i=0; i < ppdev->ulNumberDacsActive; i++)
    {
        ulHead = ppdev->ulDeviceDisplay[i];
        // Get the physical mode info for this head and the current device.
        if (!bGetPhysicalVirtualModeList(ppdev, ulHead, 
                                    ppdev->ulDeviceMask[ulHead],
                                    ppdev->ulDeviceType[ulHead],
                                    ppdev->ulTVFormat[ulHead],
                                    ppdev->PhysModeList[ulHead],
                                    NULL,
                                    &ppdev->ulNumPhysModes[ulHead],
                                    NULL))
        {
            //  Ganesh
            //  This is not an error if the head is the unused head in a standard mode.
            
            if(ppdev->TwinView_State == NVTWINVIEW_STATE_NORMAL && ppdev->ulDeviceDisplay[0] != i)
            {
                //  Benign error since nobody will be using this head info.
                //  Do not print any warning message also.

                continue;
            }

            DISPDBG((0,"InitMultiMon(): oops! bGetPhysicalVirtualModeList() failed for head: %d, devMask: 0x%x, devType: 0x%x, TVFormat: 0x%x",
                i, ppdev->ulDeviceMask[i], ppdev->ulDeviceType[i], ppdev->ulTVFormat[i]));

            return(FALSE);
        }
    } // for

    
   // Free the modelist memory alloced by getAvailableModes()
    EngFreeMem(pVideoModeInformation);

    return(TRUE);
}


//
// Does a sanity check of the device masks specified in the registry data. If they are not sane, returns FALSE.
// else returns TRUE. No state is changed.
//
ULONG ValidateRegistryDeviceMasks(PPDEV ppdev, NVTWINVIEWDATA *pData)
{
    ULONG i;
    ULONG ulDevices, ulHead;
    ULONG ulNumDacsActive;
    ULONG ulWidth, ulHeight;

    if (pData->dwAllDeviceMask != ppdev->ulAllDeviceMask)
    {
        DISPDBG((0,"Oops! Mismatch in AllDeviceMask, ppdev: 0x%x, RegData: 0x%x",
                 ppdev->ulAllDeviceMask,pData->dwAllDeviceMask));
        return(FALSE);
    }

    // Initialize the number of active dacs, to keep the compiler happy.
    ulNumDacsActive = 1;

    switch (pData->dwState)
    {
        case NVTWINVIEW_STATE_NORMAL:
        case NVTWINVIEW_STATE_DUALVIEW:
            ulHead = pData->dwDeviceDisplay[0];
            ulDevices = pData->NVTWINVIEWUNION.nvtwdevdata[ulHead].dwDeviceMask;
            ulNumDacsActive = 1;
            break;
        case NVTWINVIEW_STATE_CLONE:
            ulNumDacsActive = 2;
            ulDevices = pData->NVTWINVIEWUNION.nvtwdevdata[0].dwDeviceMask |
                        pData->NVTWINVIEWUNION.nvtwdevdata[1].dwDeviceMask;
            break;
        case NVTWINVIEW_STATE_SPAN:
            ulDevices = pData->NVTWINVIEWUNION.nvtwdevdata[0].dwDeviceMask |
                        pData->NVTWINVIEWUNION.nvtwdevdata[1].dwDeviceMask;
            ulWidth = pData->DeskTopRectl.right - pData->DeskTopRectl.left;
            ulHeight = pData->DeskTopRectl.bottom - pData->DeskTopRectl.top;
            if (MULTIMON_MODE(ulWidth, ulHeight))
            {
                ulNumDacsActive = 2;
            }
            else
            {
                if( ulWidth != 480 && ulHeight != 360 &&
                    ulWidth <= 640 && ulHeight <= 480)
                {
                     //
                     // These are the special single resolution modes allowed in spanning mode
                     // in order to support DDraw, low resolutions.
                     // 
                     ulNumDacsActive = 1;
                }
                else
                {
                // Also export low resolution for spanning mode for DirectDraw
                    if ((ulWidth == 640 && ulHeight == 480) ||
                        (ulWidth == 800 && ulHeight == 600) ||
                        (ulWidth == 1024 && ulHeight == 768))
                    {
                        //
                        // These are the special single resolution modes allowed in spanning mode.
                        //
                        ulNumDacsActive = 1;
                    }
                    else
                    {
                        DISPDBG((0, "Invalid resolution specified in the registry for spanning mode: %d, %d", ulWidth, ulHeight));
                        return(FALSE);
                    }
                }
            }
            break;
        default:
            DISPDBG((0, "Invalid State in pData\n"));
            return(FALSE);
    }

    if (pData->dwConnectedDeviceMask != ppdev->ulConnectedDeviceMask)
    {
        DISPDBG((0,"Oops! Mismatch in ConnectedDeviceMask, ppdev: 0x%x, RegData: 0x%x",
                 ppdev->ulConnectedDeviceMask,pData->dwConnectedDeviceMask));
        //
        // Before flagging error, see if the currently chosen devices are still valid. If so, some device
        // which was not being used anyway was removed. This is a benign condition so no need to flag an error.
        //
        if ((ulDevices & ppdev->ulConnectedDeviceMask) == ulDevices)
        {
            //
            // All active devices are still connected, so this is a benign condition and hence don't 
            // flag an error
            //
            return(TRUE);
        }
        else
        {
            //
            // An active device has been removed and hence we have to flag an error.
            //
            return(FALSE);
        }
    }

    // Check if the specified per-head devices are correct.
    for (i=0; i < ppdev->ulNumberDacsOnBoard; i++)
    {
        ulHead = pData->dwDeviceDisplay[i];
        if (!(pData->NVTWINVIEWUNION.nvtwdevdata[ulHead].dwDeviceMask & ppdev->ulConnectedDeviceMask))
        {
            DISPDBG((0,"Oops! Head: %d, Invalid Registry Device mask for head: 0x%x, ConnectedMask: 0x%x",
                             ulHead, pData->NVTWINVIEWUNION.nvtwdevdata[ulHead].dwDeviceMask,ppdev->ulConnectedDeviceMask));
        }
    }
    return(TRUE);
}

//
// Copies the device mask, twinview_state and initializes ulNumberDacsActive from the TwinViewInfo into
// the ppdev. These values are needed to build physical mode lists for the new configuration requested
// by the TwinViewInfo structure.
// Note: Before calling this function, the ValidateRegistryDeviceMasks() should have been successfully called
// to make sure the specified device masks are valid.
//
VOID InitDevMaskFromTwinViewInfo(PPDEV ppdev, NVTWINVIEWDATA *pData)
{
    ULONG i;
    ULONG ulHead;
    MODE_ENTRY RequestedMode, BestFitMode;
    ULONG SuggestedDeviceMask, SuggestedDeviceType, SuggestedTVType;
    ULONG ulPrimaryHead;

    //
    // Safety check. Should never happen.
    //
    if (pData == NULL)
    {
        DISPDBG((0, "InitDevMaskFromTwinViewInfo(): NULL pointer for TwinViewInfo"));
        return;
    }

    ppdev->TwinView_State = pData->dwState;
    ppdev->TwinView_Orientation = pData->dwOrientation;
    ppdev->ulNumberDacsActive = GetNumberDacsActiveFromTwinViewInfo(pData);

    for (i=0; i < NV_NO_DACS; i++)
    {
        //
        // Get the physical head ==> logical head mapping.
        //
        ppdev->ulDeviceDisplay[i] = pData->dwDeviceDisplay[i];
    }


    //
    // Copy the device mask
    //
    for (i=0; i<ppdev->ulNumberDacsOnBoard; i++)
    {
        ulHead = ppdev->ulDeviceDisplay[i];
        ppdev->ulDeviceMask[ulHead] = pData->NVTWINVIEWUNION.nvtwdevdata[ulHead].dwDeviceMask;
        ppdev->ulDeviceType[ulHead] = pData->NVTWINVIEWUNION.nvtwdevdata[ulHead].dwDeviceType;
        ppdev->ulTVFormat[ulHead] = pData->NVTWINVIEWUNION.nvtwdevdata[ulHead].dwTVFormat;
    }
}


//
// Input: bitmask for a single device
// Return: device type of this device.
//
ULONG MaskToDeviceType( PPDEV ppdev, ULONG ulDeviceMask)
{
    ULONG ulRet;

    if (ulDeviceMask & BITMASK_ALL_CRT) ulRet = MONITOR_TYPE_VGA; 
    else 
    if (ulDeviceMask & BITMASK_ALL_DFP) ulRet = MONITOR_TYPE_FLAT_PANEL;
    else 
    if (ulDeviceMask & BITMASK_ALL_TV)
    {
        ulRet = ppdev->ulDefaultTVDeviceType;
    }
    else ulRet = INVALID_DEVICE_TYPE;

    return(ulRet);
}


//
// If the system is a desktop, this routine does nothing.
// If the system is a laptop, we should use the output deive posted by the BIOS for the desktop.
// BUGBUG: Currently this function only handles a single output device posted by the BIOS.
// The code does not yet support when the BIOS posts to two devices (sort of BIOS clone mode).
// Returns: TRUE if the config matches with the BIOS heads.
// FALSE if the config differs and the diff has been handled.
//
ULONG HandleBIOSHeads(PPDEV ppdev)
{

    ULONG ulHead;
    ULONG Status, RetSize;
    char * KeyName;
    ULONG KeyNameLen;
    WCHAR KeyNameUniCode[NV_MAX_REG_KEYNAME_LEN];
    ULONG KeyNameUniCodeLen;
    NV_CFGEX_GET_DEVICES_CONFIGURATION_PARAMS nvConfig;
    ULONG ulCombinedDeviceMask;
    ULONG ulDeviceMask;
    ULONG DeviceMask[NV_NO_DACS];
    ULONG i, status;
    ULONG ReturnedDataLength;
    ULONG ulBIOSDeviceMask;
    
    
    if (Status = NvConfigGet (ppdev->hDriver, ppdev->hClient, ppdev->hDevice,
                                  NV_CFG_GET_BOOT_DEVICES, &ulBIOSDeviceMask))
        {
            DISPDBG((0,"Oops!    NV_CFG_GET_BOOT_DEVICES failed"));
        }

    DISPDBG((1,"Disp: HandleBIOSHeads(): BIOSDeviceMask: 0x%x",ulBIOSDeviceMask));

    //
    // This case should not happen but just a safety check.
    // When we are in multimon mode, allow BOOT_HEADS only if the NVSVC service has started
    //
    if (ppdev->TwinView_State == NVTWINVIEW_STATE_SPAN)
    {
        if (ppdev->ulNVSVCStatus != NVSVC_STATUS_STARTED)
        {
            return(TRUE);
        }
    }

    
    if (ppdev->UseHWSelectedDevice)
    {
        
        //
        // We don't compare BOOT_HEADS if we are explicitly being called by the NVPanel
        //
        if (GetInitiatedRegValue(ppdev) == 1)
        {
            return(TRUE);
        }

        // see if the BIOS had posted on a single device or two devices.
        if (ConvertConnectedMaskToNumDevices(ulBIOSDeviceMask) == 1)
        {
            // BIOS posted on a single device.

            // Check for the cases when we have to do nothing, i.e. windows is in sync with BIOS.
            if (ppdev->TwinView_State == NVTWINVIEW_STATE_NORMAL || ppdev->TwinView_State == NVTWINVIEW_STATE_DUALVIEW)
            {
                ulHead = ppdev->ulDeviceDisplay[0];
                if (ppdev->ulDeviceMask[ulHead] == ulBIOSDeviceMask)
                {
                    return(TRUE);
                }
            }


            //
            // Invoke NVSVC to do the modeset and return, provided NVSVC service has started.
            //
            if (ppdev->ulNVSVCStatus == NVSVC_STATUS_STARTED)
            {
                ppdev->ulInduceModeChangeDeviceMask = ulBIOSDeviceMask;
                if (EngDeviceIoControl(ppdev->hDriver,
                        IOCTL_VIDEO_SIGNAL_NVSVC_MODESET_EVENT,
                        NULL,
                        0,
                        NULL,
                        0,
                        &ReturnedDataLength))
                {
                    DISPDBG((1, "InitMultiMon() - IOCTL_VIDEO_SIGNAL_MODESET_EVENT failed"));
                }   
                if (EngDeviceIoControl(ppdev->hDriver,
                        IOCTL_VIDEO_SET_NVSVC_MODESET_DEVMASK,
                        &ppdev->ulInduceModeChangeDeviceMask,
                        sizeof(ULONG),
                        NULL,
                        0,
                        &ReturnedDataLength))
                {
                    DISPDBG((1, "InitMultiMon() - IOCTL_VIDEO_SIGNAL_MODESET_DEVMASK failed"));
                }   

                return(TRUE);
            }

            //
            // Since NVSVC has not started, we need to do the device switch ourselves
            // If we are in standard or clone mode, make sure we use this device.
            // We don't handle spanning modes yet.
            //
            if (ppdev->TwinView_State == NVTWINVIEW_STATE_NORMAL ||
                ppdev->TwinView_State == NVTWINVIEW_STATE_CLONE)
            {
                // Ask ResMan for which head to use.
                // Ask ResMan what head to use.
                ppdev->TwinView_State = NVTWINVIEW_STATE_NORMAL;

                nvConfig.DevicesConfig = ulBIOSDeviceMask;
                nvConfig.OldDevicesConfig = 0;
                nvConfig.OldDevicesAllocationMap = 0;
                nvConfig.DevicesAllocationMap = 0;
                if (NvConfigGetEx (ppdev->hDriver, ppdev->hClient, ppdev->hDevice,
                                   NV_CFGEX_GET_DEVICES_CONFIGURATION, &nvConfig, sizeof(NV_CFGEX_GET_DEVICES_CONFIGURATION_PARAMS)))
                {

                    DISPDBG((0, "HandleBIOSHeads - Cannot get DEVICES_CONFIG value from RM"));
                }
                else
                {
                    if (nvConfig.DevicesAllocationMap > 1)
                    {
                        DISPDBG((0,"Oops! Invalid DevAllocationMap from RM: 0x%x",nvConfig.DevicesAllocationMap));
                    }
                    if (nvConfig.DevicesAllocationMap == 0)
                    {
                        ppdev->ulDeviceMask[0] = ulBIOSDeviceMask;
                        ppdev->ulDeviceMask[1] = BITMASK_INVALID_DEVICE;
                        ppdev->ulDeviceDisplay[0] = 0;
                        ppdev->ulDeviceDisplay[1] = 1;
                    }
                    if (nvConfig.DevicesAllocationMap == 1)
                    {
                        ppdev->ulDeviceMask[1] = ulBIOSDeviceMask;
                        ppdev->ulDeviceMask[0] = BITMASK_INVALID_DEVICE;
                        ppdev->ulDeviceDisplay[0] = 1;
                        ppdev->ulDeviceDisplay[1] = 0;
                    }
                }

                // Now fill in all the relevant info into the ppdev.
                ulHead = ppdev->ulDeviceDisplay[0];
                ulDeviceMask = ppdev->ulDeviceMask[ulHead];
                ppdev->ulDeviceType[ulHead] = MaskToDeviceType(ppdev, ulDeviceMask);
                ppdev->ulTVFormat[ulHead] = ppdev->ulDefaultTVFormat; 

                // The resolution (virtual and physical) for the head and desktop will be filled in by the
                // UseOnlyGDIForModeDEtails() which will be called later after HandleBiosHeads().

                // Configuration has changed. So return FALSE.
                return(FALSE);
            } // Normal or Clone
        } // BIOS on single device.

        if (ConvertConnectedMaskToNumDevices(ulBIOSDeviceMask) >= 2)
        {
            // BIOS posted on dual devices.

            // Check for the cases when we have to do nothing, i.e. windows is in sync with BIOS.
            if (ppdev->TwinView_State == NVTWINVIEW_STATE_CLONE)
            {
                ulCombinedDeviceMask = ppdev->ulDeviceMask[0] | ppdev->ulDeviceMask[1];
                if (ulCombinedDeviceMask == ulBIOSDeviceMask)
                {
                    return(TRUE);
                }
            }


            //
            // Invoke NVSVC to do the modeset and return, provided NVSVC service has started.
            //
            if (ppdev->ulNVSVCStatus == NVSVC_STATUS_STARTED)
            {
                ppdev->ulInduceModeChangeDeviceMask = ulBIOSDeviceMask;
                if (EngDeviceIoControl(ppdev->hDriver,
                        IOCTL_VIDEO_SIGNAL_NVSVC_MODESET_EVENT,
                        NULL,
                        0,
                        NULL,
                        0,
                        &ReturnedDataLength))
                {
                    DISPDBG((1, "InitMultiMon() - IOCTL_VIDEO_SIGNAL_MODESET_EVENT failed"));
                }
                return(TRUE);
            }

            //
            // Since NVSVC has not started, we need to do the device switch ourselves.
            // If we are in standard or clone mode, make sure we use this device.
            // We don't handle spanning modes yet.
            //
            if (ppdev->TwinView_State == NVTWINVIEW_STATE_NORMAL ||
                ppdev->TwinView_State == NVTWINVIEW_STATE_CLONE)
            {
                // Ask ResMan for which head to use.
                // Ask ResMan what head to use.
                ppdev->TwinView_State = NVTWINVIEW_STATE_CLONE;
                ppdev->ulNumberDacsActive =2;

                ppdev->ulDeviceDisplay[0] = 0;
                ppdev->ulDeviceDisplay[1] = 1;

                // Grab the first output device.
                for (i=0; i < NV_NO_CONNECTORS; i++)
                {
                    if (ulBIOSDeviceMask & (1 << i))
                    {
                        DeviceMask[0] = (1 << i);
                        i++;
                        break;
                    }
                }
                // Continue further and grab the next device.
                for (; i < NV_NO_CONNECTORS; i++)
                {
                    if (ulBIOSDeviceMask & (1 << i))
                    {
                        DeviceMask[1] = (1 << i);
                        break;
                    }
                }

                // Ask ResMan what head to use for the devices.
                nvConfig.DevicesConfig = DeviceMask[0] | DeviceMask[1];
                nvConfig.OldDevicesConfig = 0;
                nvConfig.OldDevicesAllocationMap = 0;
                nvConfig.DevicesAllocationMap = 0;
                status = NvConfigGetEx (ppdev->hDriver, ppdev->hClient, ppdev->hDevice,
                                NV_CFGEX_GET_DEVICES_CONFIGURATION, &nvConfig, sizeof(NV_CFGEX_GET_DEVICES_CONFIGURATION_PARAMS));
                if (status != 0)
                {

                    DISPDBG((0, "Oops! HandleBIOSHeads - Cannot get DEVICES_CONFIG value from RM"));
                    // Set to a safe single head mode.
                    ppdev->ulNumberDacsConnected = 1;
                    ppdev->ulConnectedDeviceMask = DeviceMask[0];
                    ppdev->ulAllDeviceMask = DeviceMask[0];
                    ppdev->ulDeviceMask[0] = DeviceMask[0];
                    ppdev->ulDeviceMask[1] = BITMASK_INVALID_DEVICE;
                }
                else
                {
                    // We got a successful head assignment for both the devices.
                    switch (nvConfig.DevicesAllocationMap)
                    {
                    case 0x10:
                        // Head assignment 1, 0
                        ppdev->ulDeviceMask[0] = DeviceMask[0];
                        ppdev->ulDeviceMask[1] = DeviceMask[1];
                        break;

                    case 0x01:
                        // Head assignment 0, 1
                        ppdev->ulDeviceMask[0] = DeviceMask[1];
                        ppdev->ulDeviceMask[1] = DeviceMask[0];
                        break;

                    default:
                        DISPDBG((0,"Oops! Unexpected head allocationMap: 0x%x",  nvConfig.DevicesAllocationMap));
                        // Set to a safe single head mode.
                        ppdev->ulNumberDacsConnected = 1;
                        ppdev->ulConnectedDeviceMask = DeviceMask[0];
                        ppdev->ulAllDeviceMask = DeviceMask[0];
                        ppdev->ulDeviceMask[0] = DeviceMask[0];
                        ppdev->ulDeviceMask[1] = BITMASK_INVALID_DEVICE;
                        break;
                    }

                    for (i=0; i < ppdev->ulNumberDacsOnBoard; i++)
                    {
                        //
                        // Convert the device mask to device type. 
                        //
                        ppdev->ulDeviceType[i] = MaskToDeviceType(ppdev, ppdev->ulDeviceMask[i]);
                        ppdev->ulTVFormat[ulHead] = ppdev->ulDefaultTVFormat;
                    }
                } // status != RM_OK

                // The resolution (virtual and physical) for the head and desktop will be filled in by the
                // UseOnlyGDIForModeDEtails() which will be called later after HandleBiosHeads().

                // Configuration has changed. So return FALSE.
                return(FALSE);
            } // Normal or Clone
        } // BIOS selected two devices.
    } // UseHwSelectedDevice
 
     return (TRUE);
}

//
// Returns the index from the device mask.
// Input: A 32bit bitmask with only one of the 32bits set specifying a unique device
// Return Value: Index of this bitmask
//      For example: 
//      input: 0x4, Return: 2
//      input: 0x20, Return: 5
//      input: 0x1, Return: 0
//      input: 0x0, Return: 0
//
ULONG ConvertDeviceMaskToIndex(
    ULONG ulDeviceMask)
{
    ULONG i;
    ULONG ulTmp;
    ulTmp = ulDeviceMask;

  
    i=0;
    for (;;)
    {
        if (ulTmp == 0)
        {
            DISPDBG((1,"Oops! ConvertDeviceMaskToIndex(): Invalid ulDeviceMask: 0x%x\n",ulDeviceMask));
            return(0);
        }
        if (ulTmp & 0x1)
        {
            return(i);
        }
        // shift right by one and increment the index
        ulTmp >>= 1;
        i++;
    }
}

//
// Returns the number of bits that are set in the mask.
// 
ULONG ConvertConnectedMaskToNumDevices(
    ULONG ulConnectedDeviceMask)
{
    ULONG i, NumDevices, ulDeviceMask;
    NumDevices = 0;
    for(i=0; i < NV_NO_CONNECTORS; i++)
    {
        ulDeviceMask = (1 << i);
        if(ulConnectedDeviceMask & ulDeviceMask)
        {
            NumDevices++;
        }
    }
    return(NumDevices);

}

//
// Reads the ToshibaHotKeyMode registry entry.
//
VOID ReadToshibaHotKeyMode(HANDLE      hDriver, TOSHIBA_HOTKEY_MODE *pMode)
{
    char * KeyName = "ForceGDIMode";
    ULONG KeyNameLen;
    WCHAR KeyNameUniCode[NV_MAX_REG_KEYNAME_LEN];
    ULONG KeyNameUniCodeLen;
    NV_REGISTRY_STRUCT regStructInput;
    ULONG Status;
    ULONG returnedDataLen;

    KeyNameLen = strlen(KeyName) + 1;
    EngMultiByteToUnicodeN(KeyNameUniCode,
                           NV_MAX_REG_KEYNAME_LEN,
                           &KeyNameUniCodeLen,
                           (PCHAR)KeyName,
                           KeyNameLen*sizeof(CHAR));

    regStructInput.keyName = (PWSTR)KeyNameUniCode;
    regStructInput.keyVal = pMode;
    regStructInput.keyValSize = sizeof(TOSHIBA_HOTKEY_MODE);

    Status = EngDeviceIoControl(hDriver,
                                IOCTL_VIDEO_QUERY_REGISTRY_BINARY_VAL,
                                (LPVOID)&regStructInput,
                                sizeof(NV_REGISTRY_STRUCT),
                                (LPVOID) (pMode),
                                sizeof(TOSHIBA_HOTKEY_MODE),
                                &(returnedDataLen));

    DISPDBG((1,"KeyValSize returned from miniport: 0x%x",returnedDataLen));
}

//
// Writes the ToshibaHotKeyMode registry entry
//
VOID WriteToshibaHotKeyMode(HANDLE      hDriver, TOSHIBA_HOTKEY_MODE *pMode)
{
    char * KeyName = "ForceGDIMode";
    ULONG KeyNameLen;
    WCHAR KeyNameUniCode[NV_MAX_REG_KEYNAME_LEN];
    ULONG KeyNameUniCodeLen;
    NV_REGISTRY_STRUCT regStruct;
    ULONG Status;
    ULONG returnedDataLen;

    KeyNameLen = strlen(KeyName) + 1;
    EngMultiByteToUnicodeN(KeyNameUniCode,
                           NV_MAX_REG_KEYNAME_LEN,
                           &KeyNameUniCodeLen,
                           (PCHAR)KeyName,
                           KeyNameLen*sizeof(CHAR));

    regStruct.keyName = (PWSTR)KeyNameUniCode;
    regStruct.keyVal = pMode;
    regStruct.keyValSize = sizeof(TOSHIBA_HOTKEY_MODE);

    Status = EngDeviceIoControl(hDriver,
                                   IOCTL_VIDEO_SET_REGISTRY_VALUE,
                                   (LPVOID)(&regStruct),
                                   sizeof(NV_REGISTRY_STRUCT),
                                   (LPVOID) NULL,
                                   0,
                                   &returnedDataLen);
}

//
// if pData is non-NULL, writes the pData as is to the registry.
// if pData is NULL, then constructs the TwinViewInfo from ppdev information and writes to the registry.
//
VOID WriteRegistryTwinViewInfo (PPDEV ppdev, NVTWINVIEWDATA *pData)
{
    ULONG returnedDataLength;
    NVTWINVIEWDATA sData;
    ULONG i;
    NVTWINVIEW_DEVICE_TYPE_DATA * pHeadData;

    if (pData)
    {
        if (EngDeviceIoControl(ppdev->hDriver,
                               IOCTL_VIDEO_SET_TWINVIEW_STATE,
                               pData,
                               sizeof(NVTWINVIEWDATA),
                               NULL,
                               0,
                               &returnedDataLength))
        {
            DISPDBG((0, "WriteRegistryTwinViewInfo pData: - IOCTL_VIDEO_SET_TWINVIEW_STATE failed"));
        }
        return;
    }

    // Zero the memory.
    memset(&sData, 0, sizeof(NVTWINVIEWDATA));

    // We need to costruct the TwinViewData from the ppdev info.
    // Set the desktop resolution.
    sData.DeskTopRectl.left = 0;
    sData.DeskTopRectl.top = 0;
    sData.DeskTopRectl.right = ppdev->cxScreen;
    sData.DeskTopRectl.bottom = ppdev->cyScreen;

    sData.dwState = ppdev->TwinView_State;
    sData.dwOrientation = ppdev->TwinView_Orientation;
    sData.dwAllDeviceMask = ppdev->ulAllDeviceMask;
    sData.dwConnectedDeviceMask = ppdev->ulConnectedDeviceMask;


    for (i=0; i < NV_NO_DACS; i++)
    {
        sData.dwDeviceDisplay[i] = ppdev->ulDeviceDisplay[i];

        pHeadData = &(sData.NVTWINVIEWUNION.nvtwdevdata[i]);

        pHeadData->dwDeviceMask = ppdev->ulDeviceMask[i];
        pHeadData->dwDeviceType = ppdev->ulDeviceType[i];
        pHeadData->dwTVFormat = ppdev->ulTVFormat[i];

        pHeadData->dwBpp = ppdev->cBitsPerPel;
        pHeadData->dwRefresh = ppdev->ulRefreshRate[i];
        pHeadData->VirtualRectl.left = ppdev->rclDisplayDesktop[i].left;
        pHeadData->VirtualRectl.top = ppdev->rclDisplayDesktop[i].top;
        pHeadData->VirtualRectl.right = ppdev->rclDisplayDesktop[i].right;
        pHeadData->VirtualRectl.bottom = ppdev->rclDisplayDesktop[i].bottom;
        pHeadData->PhysicalRectl.left = ppdev->rclCurrentDisplay[i].left;
        pHeadData->PhysicalRectl.top = ppdev->rclCurrentDisplay[i].top;
        pHeadData->PhysicalRectl.right = ppdev->rclCurrentDisplay[i].right;
        pHeadData->PhysicalRectl.bottom = ppdev->rclCurrentDisplay[i].bottom;

        pHeadData->dwNumPhysModes = ppdev->ulNumPhysModes[i];
        pHeadData->dwNumVirtualModes = ppdev->ulNumVirtualModes[i];

        pHeadData->dwEnableDDC = ppdev->ulEnableDDC[i];
        pHeadData->dwEnablePanScan = ppdev->ulEnablePanScan[i];
        pHeadData->dwFreezePanScan = ppdev->ulFreezePanScan[i];
        pHeadData->dwFreezeOriginX = ppdev->ulFreezeOriginX[i];
        pHeadData->dwFreezeOriginY = ppdev->ulFreezeOriginY[i];
        pHeadData->dwTimingOverRide = ppdev->ulTimingOverRide[i];
    }

    //
    // Now write the structure to the registry.
    //
    if (EngDeviceIoControl(ppdev->hDriver,
                           IOCTL_VIDEO_SET_TWINVIEW_STATE,
                           &sData,
                           sizeof(NVTWINVIEWDATA),
                           NULL,
                           0,
                           &returnedDataLength))
    {
        DISPDBG((0, "WriteRegistryTwinViewInfo sData: - IOCTL_VIDEO_SET_TWINVIEW_STATE failed"));
    }
    return;
}



// Read a (named) reg key through IOCTL call (through miniport) (DeviceIO Wrapper)
//
// (see dspioctl.h for NV_REG_STRUCT )


void GetRegValueW(
                     HANDLE  hDriver,            // Handle to our phys device
                     PWSTR   keyName,            // Unicode name of the key     : IN
                     PVOID   keyData,            // Ptr to the data of the key  : OUT (IN not used in the IOCTL )
                     ULONG   keyDataSize)        // The sizeof(data) of the key : IN
{
    NV_REGISTRY_STRUCT regStruct;
    ULONG              IOCTLRetDataLen;     // not used
    ULONG              status;

    regStruct.keyName    = keyName;
    regStruct.keyVal     = keyData;
    regStruct.keyValSize = keyDataSize;

    status = EngDeviceIoControl(hDriver,
                                IOCTL_VIDEO_QUERY_REGISTRY_BINARY_VAL,
                                
                                (LPVOID)&regStruct,           // only the keyname and datasize is used
                                                              // 
                                sizeof(NV_REGISTRY_STRUCT),
                                
                                (LPVOID) keyData,             // only *keyData is written
                                keyDataSize,                  //
                                                              // NOTE: For this particular IOCTL (see NV.C),
                                                              // the return buffer is used for the reg key data
                                                              // itself

                                
                                &(IOCTLRetDataLen));

} // ...GetRegValueW()


//
// Writes a (named) reg key through IOCTL call (through miniport) (DeviceIO Wrapper)
//

void SetRegValueW(
                      HANDLE  hDriver,            // Handle to our phys device
                      PWSTR   keyName,            // Unicode name of the key     : IN
                      PVOID   keyData,            // The data of the key         : IN
                      ULONG   keyDataSize)        // The sizeof(data) of the key : IN
{
    NV_REGISTRY_STRUCT regStruct;
    ULONG              IOCTLRetDataLen;     // not used
    ULONG              status;

    regStruct.keyName    = keyName;
    regStruct.keyVal     = keyData;
    regStruct.keyValSize = keyDataSize;

    status = EngDeviceIoControl(hDriver,
                                IOCTL_VIDEO_SET_REGISTRY_VALUE,
                                
                                (LPVOID)&regStruct,    
                                                       
                                sizeof(NV_REGISTRY_STRUCT),
                                
                                NULL,
                                0,
                                
                                &(IOCTLRetDataLen));

} // ...SetRegValueW()



//
// Reads the registry entry "Key Name".
// Returns TRUE if this entry exists and the value is returned in pulData.
// Returns FALSE if this registry entry is absent and pulData is not modified.
//
BOOLEAN GetULONGRegValue(PPDEV ppdev, char *pcKeyName, ULONG *pulData)
{
    ULONG KeyNameLen;
    WCHAR KeyNameUniCode[NV_MAX_REG_KEYNAME_LEN];
    ULONG KeyNameUniCodeLen;
    ULONG Status;
    ULONG RetSize = 0;
    
    KeyNameLen = strlen(pcKeyName) + 1;


    EngMultiByteToUnicodeN(KeyNameUniCode,
                           NV_MAX_REG_KEYNAME_LEN,
                           &KeyNameUniCodeLen,
                           (PCHAR)pcKeyName,
                           KeyNameLen*sizeof(CHAR));

    Status = EngDeviceIoControl(ppdev->hDriver,
                                IOCTL_VIDEO_QUERY_REGISTRY_VAL,
                                (LPVOID)KeyNameUniCode,
                                KeyNameUniCodeLen,
                                (LPVOID)pulData,
                                sizeof(ULONG),
                                &RetSize);
    if (Status == 0)
    {
        //
        // The call succeeded
        //
        DISPDBG((1,"Disp: GetULONGRegValue(): RegKey: %s, Value: 0x%x",pcKeyName, *pulData));
        return(TRUE);

    }
    else
    {    
        //
        // The call failed.
        //
        DISPDBG((1,"Disp: GetULONGRegValue(): RegKey: %s, Does not exist. Error status: 0x%x",pcKeyName, Status));
        return(FALSE);
    }
}

//
// Sets the registry key "pcKeyName" with the ulong value in pulData.
// Creates the registry entry if it does not exist.
//
VOID SetULONGRegValue(PPDEV ppdev, char *pcKeyName, ULONG ulData)
{
    ULONG KeyNameLen;
    WCHAR KeyNameUniCode[NV_MAX_REG_KEYNAME_LEN];
    NV_REGISTRY_STRUCT regStruct;
    ULONG KeyNameUniCodeLen;
    ULONG Status;
    ULONG returnedDataLen = 0;

    KeyNameLen = strlen(pcKeyName) + 1;
    EngMultiByteToUnicodeN(KeyNameUniCode,
                           NV_MAX_REG_KEYNAME_LEN,
                           &KeyNameUniCodeLen,
                           (PCHAR)pcKeyName,
                           KeyNameLen*sizeof(CHAR));

    regStruct.keyName = (PWSTR)KeyNameUniCode;
    regStruct.keyVal = &ulData;
    regStruct.keyValSize = sizeof(ULONG);

    Status = EngDeviceIoControl(ppdev->hDriver,
                                   IOCTL_VIDEO_SET_REGISTRY_VALUE,
                                   (LPVOID)(&regStruct),
                                   sizeof(NV_REGISTRY_STRUCT),
                                   (LPVOID) NULL,
                                   0,
                                   &returnedDataLen);
    return;
}

//
// Reads the ModeSetInitiated registry entry.
// Returns 1 if this entry exists and has a non-zero value.
// Returns 0 if this registry entry is absent or is present with a value of 0.
//
ULONG GetInitiatedRegValue(PPDEV ppdev)
{
    char * KeyName;
    ULONG KeyNameLen;
    WCHAR KeyNameUniCode[NV_MAX_REG_KEYNAME_LEN];
    ULONG KeyNameUniCodeLen;
    ULONG ModeSetInitiated;
    ULONG Status;
    ULONG RetSize;
    
    // Read the registry entry "NvCplInduceModeSetInitiated".
    ModeSetInitiated = 0;
    KeyName = "NvCplInduceModeSetInitiated";
    KeyNameLen = strlen(KeyName) + 1;

    ModeSetInitiated = 0;
    EngMultiByteToUnicodeN(KeyNameUniCode,
                           NV_MAX_REG_KEYNAME_LEN,
                           &KeyNameUniCodeLen,
                           (PCHAR)KeyName,
                           KeyNameLen*sizeof(CHAR));

    Status = EngDeviceIoControl(ppdev->hDriver,
                                IOCTL_VIDEO_QUERY_REGISTRY_VAL,
                                (LPVOID)KeyNameUniCode,
                                KeyNameUniCodeLen,
                                (LPVOID)&ModeSetInitiated,
                                sizeof(ULONG),
                                &RetSize);
    DISPDBG((1,"Disp: GetInitiatedRegValue(): ModeSetInitiated registry value: 0x%x",ModeSetInitiated));
    return(ModeSetInitiated);

}

//
// Sets the ModeSetInitiated registry entry to 'ulVal'.
// Creates the registry entry if it does not exist.
//
VOID SetInitiatedRegValue(PPDEV ppdev, ULONG ulVal)
{
    char * KeyName = "NvCplInduceModeSetInitiated";
    ULONG KeyNameLen;
    WCHAR KeyNameUniCode[NV_MAX_REG_KEYNAME_LEN];
    NV_REGISTRY_STRUCT regStruct;
    ULONG KeyNameUniCodeLen;
    ULONG Status;
    ULONG returnedDataLen;

    KeyNameLen = strlen(KeyName) + 1;
    EngMultiByteToUnicodeN(KeyNameUniCode,
                           NV_MAX_REG_KEYNAME_LEN,
                           &KeyNameUniCodeLen,
                           (PCHAR)KeyName,
                           KeyNameLen*sizeof(CHAR));

    regStruct.keyName = (PWSTR)KeyNameUniCode;
    regStruct.keyVal = &ulVal;
    regStruct.keyValSize = sizeof(ULONG);

    Status = EngDeviceIoControl(ppdev->hDriver,
                                   IOCTL_VIDEO_SET_REGISTRY_VALUE,
                                   (LPVOID)(&regStruct),
                                   sizeof(NV_REGISTRY_STRUCT),
                                   (LPVOID) NULL,
                                   0,
                                   &returnedDataLen);
    return;
}

//
// Reads the ResumingFromAdapterPowerDownState registry entry.
// Returns 1 if this entry exists and has a non-zero value.
// Returns 0 if this registry entry is absent or is present with a value of 0.
//
ULONG GetPowerStateRegValue(PPDEV ppdev)
{
    char * KeyName;
    ULONG KeyNameLen;
    WCHAR KeyNameUniCode[NV_MAX_REG_KEYNAME_LEN];
    ULONG KeyNameUniCodeLen;
    ULONG ResumingFromAdapterPowerDownState;
    ULONG Status;
    ULONG RetSize;
    
    // Read the registry entry "ResumingFromAdapterPowerDownState".
    ResumingFromAdapterPowerDownState = 0;
    KeyName = "ResumingFromAdapterPowerDownState";
    KeyNameLen = strlen(KeyName) + 1;

    ResumingFromAdapterPowerDownState = 0;
    EngMultiByteToUnicodeN(KeyNameUniCode,
                           NV_MAX_REG_KEYNAME_LEN,
                           &KeyNameUniCodeLen,
                           (PCHAR)KeyName,
                           KeyNameLen*sizeof(CHAR));

    Status = EngDeviceIoControl(ppdev->hDriver,
                                IOCTL_VIDEO_QUERY_REGISTRY_VAL,
                                (LPVOID)KeyNameUniCode,
                                KeyNameUniCodeLen,
                                (LPVOID)&ResumingFromAdapterPowerDownState,
                                sizeof(ULONG),
                                &RetSize);
    DISPDBG((1,"Disp: GetPowerStateRegValue(): ResumingFromAdapterPowerDownState registry value: 0x%x",ResumingFromAdapterPowerDownState));
    
    return(ResumingFromAdapterPowerDownState);

}

//
// Sets the ResumingFromAdapterPowerDownState registry entry to 'ulVal'.
// Creates the registry entry if it does not exist.
//
VOID SetPowerStateRegValue(PPDEV ppdev, ULONG ulVal)
{
    char * KeyName = "ResumingFromAdapterPowerDownState";
    ULONG KeyNameLen;
    WCHAR KeyNameUniCode[NV_MAX_REG_KEYNAME_LEN];
    NV_REGISTRY_STRUCT regStruct;
    ULONG KeyNameUniCodeLen;
    ULONG Status;
    ULONG returnedDataLen;

    KeyNameLen = strlen(KeyName) + 1;
    EngMultiByteToUnicodeN(KeyNameUniCode,
                           NV_MAX_REG_KEYNAME_LEN,
                           &KeyNameUniCodeLen,
                           (PCHAR)KeyName,
                           KeyNameLen*sizeof(CHAR));

    regStruct.keyName = (PWSTR)KeyNameUniCode;
    regStruct.keyVal = &ulVal;
    regStruct.keyValSize = sizeof(ULONG);

    Status = EngDeviceIoControl(ppdev->hDriver,
                                   IOCTL_VIDEO_SET_REGISTRY_VALUE,
                                   (LPVOID)(&regStruct),
                                   sizeof(NV_REGISTRY_STRUCT),
                                   (LPVOID) NULL,
                                   0,
                                   &returnedDataLen);
    return;
}

//
// Reads the PrevDevMask registry entry.
// Returns the non-zero value if this entry exists and has a non-zero value.
// Returns 0 if this registry entry is absent or is present with a value of 0.
//
ULONG GetPrevDevMaskRegValue(PPDEV ppdev)
{
    char * KeyName;
    ULONG KeyNameLen;
    WCHAR KeyNameUniCode[NV_MAX_REG_KEYNAME_LEN];
    ULONG KeyNameUniCodeLen;
    ULONG PrevDevMask;
    ULONG Status;
    ULONG RetSize;
    
    // Read the registry entry "PrevDevMask".
    PrevDevMask = 0;
    KeyName = "PrevDevMask";
    KeyNameLen = strlen(KeyName) + 1;

    PrevDevMask = 0;
    EngMultiByteToUnicodeN(KeyNameUniCode,
                           NV_MAX_REG_KEYNAME_LEN,
                           &KeyNameUniCodeLen,
                           (PCHAR)KeyName,
                           KeyNameLen*sizeof(CHAR));

    Status = EngDeviceIoControl(ppdev->hDriver,
                                IOCTL_VIDEO_QUERY_REGISTRY_VAL,
                                (LPVOID)KeyNameUniCode,
                                KeyNameUniCodeLen,
                                (LPVOID)&PrevDevMask,
                                sizeof(ULONG),
                                &RetSize);
    DISPDBG((1,"Disp: GetPrevDevMaskRegValue(): PrevDevMask registry value: 0x%x",PrevDevMask));
    
    return(PrevDevMask);

}

//
// Sets the PrevDevMask registry entry to 'ulVal'.
// Creates the registry entry if it does not exist.
//
VOID SetPrevDevMaskRegValue(PPDEV ppdev, ULONG ulVal)
{
    char * KeyName = "PrevDevMask";
    ULONG KeyNameLen;
    WCHAR KeyNameUniCode[NV_MAX_REG_KEYNAME_LEN];
    NV_REGISTRY_STRUCT regStruct;
    ULONG KeyNameUniCodeLen;
    ULONG Status;
    ULONG returnedDataLen;

    KeyNameLen = strlen(KeyName) + 1;
    EngMultiByteToUnicodeN(KeyNameUniCode,
                           NV_MAX_REG_KEYNAME_LEN,
                           &KeyNameUniCodeLen,
                           (PCHAR)KeyName,
                           KeyNameLen*sizeof(CHAR));

    regStruct.keyName = (PWSTR)KeyNameUniCode;
    regStruct.keyVal = &ulVal;
    regStruct.keyValSize = sizeof(ULONG);

    Status = EngDeviceIoControl(ppdev->hDriver,
                                   IOCTL_VIDEO_SET_REGISTRY_VALUE,
                                   (LPVOID)(&regStruct),
                                   sizeof(NV_REGISTRY_STRUCT),
                                   (LPVOID) NULL,
                                   0,
                                   &returnedDataLen);
    return;
}




#if DBG
//
// Reads the "nvDispDebugLevel" registry entry.
// If it exists, then sets this value to the global DebugLevel variable.
//
VOID GetDebugLevelRegValue(PPDEV ppdev)
{
    char * KeyName;
    ULONG KeyNameLen;
    WCHAR KeyNameUniCode[NV_MAX_REG_KEYNAME_LEN];
    ULONG KeyNameUniCodeLen;
    ULONG ulVal;
    ULONG Status;
    ULONG RetSize;

    //
    // We should not override the DebugLevel value that might have been set, say by a developer by hand.
    // If there is no "nvDispDebugLevel" override, we will simply restore the original DebugLevel value.
    //
    ulVal = DebugLevel;

    //
    // Read the registry entry "nvDispDebugLevel".
    //
    KeyName = "nvDispDebugLevel";
    KeyNameLen = strlen(KeyName) + 1;
    EngMultiByteToUnicodeN(KeyNameUniCode,
                           NV_MAX_REG_KEYNAME_LEN,
                           &KeyNameUniCodeLen,
                           (PCHAR)KeyName,
                           KeyNameLen*sizeof(CHAR));

    Status = EngDeviceIoControl(ppdev->hDriver,
                                IOCTL_VIDEO_QUERY_REGISTRY_VAL,
                                (LPVOID)KeyNameUniCode,
                                KeyNameUniCodeLen,
                                (LPVOID)&ulVal,
                                sizeof(ULONG),
                                &RetSize);
    DISPDBG((1,"Disp: nvDispDebugLevel(): nvDispDebugLevel registry value: 0x%x",ulVal));
    
    DebugLevel = ulVal;
}
#endif DBG

//
// Reads in the latest and correct TV format the driver should use for a default value and caches in the ppdev.
// Note: Currently keeps track of a single TV format since all our cards currently have only one TV encoder but
// in the future we may have cards with say two TV encoders. Then we would have to keep track of TV format per
// TV encoder (or per TV device mask).
//
void GetDefaultTVFormat(PPDEV ppdev)
{
    ULONG Status;
    NV_CFGEX_GET_VIDEO_TV_STANDARD_PARAMS Params;

    ppdev->ulDefaultTVFormat = 0;
    Params.DevicesConfig = TV_ENCODER_0;
    Params.TVStandard = 0;
    
    if (Status = NvConfigGetEx (ppdev->hDriver, ppdev->hClient, ppdev->hDevice,
                          NV_CFGEX_GET_VIDEO_TV_STANDARD, &Params, sizeof(NV_CFGEX_GET_VIDEO_TV_STANDARD_PARAMS)))
    {
        DISPDBG((0,"Oops!    NV_CFGEX_GET_VIDEO_TV_STANDARD failed"));
    }

    ppdev->ulDefaultTVFormat = Params.TVStandard;

    DISPDBG((1,"Disp: GetDefaultTVFormat(): ulDefaultTVFormat: 0x%x",ppdev->ulDefaultTVFormat));
    ppdev->ulDefaultTVDeviceType = TVFormatToTVDeviceType(ppdev->ulDefaultTVFormat);
    DISPDBG((1,"Disp: GetDefaultTVFormat(): ulDefaultTVDeviceType: 0x%x",ppdev->ulDefaultTVDeviceType));
}

//
// Converts a TV FOrmat to the appropriate tV deviceType.
//
ULONG TVFormatToTVDeviceType(ULONG ulTVFormat)
{
    ULONG ulDeviceType;
    switch (ulTVFormat)
    {
    case NTSC_M:
    case NTSC_J:
        ulDeviceType = MONITOR_TYPE_NTSC;
        break;
    case PAL_M:
    case PAL_A:
    case PAL_N:
    case PAL_NC:
        ulDeviceType = MONITOR_TYPE_PAL;
        break;
    default:
        ulDeviceType = MONITOR_TYPE_NTSC;
        break;
    }
    return(ulDeviceType);
}

BOOLEAN ConvertHeadResToTiming(PPDEV ppdev,
                               ULONG ulHead,
                               HEAD_RESOLUTION_INFO *pHeadResolution,
                               NV_CFGEX_VALIDATE_BANDWIDTH_PARAMS *pParam)
{
    ULONG             ReturnedDataLength;
    DAC_TIMING_VALUES sTimingDac;

    if (EngDeviceIoControl(ppdev->hDriver,
                           IOCTL_VIDEO_GET_TIMING_DAC,
                           pHeadResolution,  // input buffer
                           sizeof(HEAD_RESOLUTION_INFO),
                           &sTimingDac,
                           sizeof(DAC_TIMING_VALUES),
                           &ReturnedDataLength))
    {
        DISPDBG((0, "HandleBandWidth_SingleDacActive() - IOCTL_VIDEO_GET_TIMING_DAC failed"));
        return FALSE;
    }

    //
    // Sanity check. Make sure we got the requested timings. This should not happen.
    //
    if (sTimingDac.HorizontalVisible != pHeadResolution->ulDisplayWidth || 
        sTimingDac.VerticalVisible != pHeadResolution->ulDisplayHeight ||
        sTimingDac.Refresh != pHeadResolution->ulDisplayRefresh)
    {
        return FALSE;
    }

    if (ulHead == 0)
    {
        pParam->TotalWidth0 = sTimingDac.HorizontalTotal;
        pParam->VisibleWidth0 = sTimingDac.HorizontalVisible;
        pParam->TotalHeight0 = sTimingDac.VerticalTotal;
        pParam->VisibleHeight0 = sTimingDac.VerticalVisible;
        pParam->PixelDepth0 = ppdev->cBitsPerPel;
        pParam->PixelClock0 = sTimingDac.PixelClock;
    }
    else
    {
        pParam->TotalWidth1 = sTimingDac.HorizontalTotal;
        pParam->VisibleWidth1 = sTimingDac.HorizontalVisible;
        pParam->TotalHeight1 = sTimingDac.VerticalTotal;
        pParam->VisibleHeight1 = sTimingDac.VerticalVisible;
        pParam->PixelDepth1 = ppdev->cBitsPerPel;
        pParam->PixelClock1 = sTimingDac.PixelClock;
    }

    return TRUE;
}

//
// Verifies that the physical modes can be supported by the Bandwidth restrictions of the system.
// If not, lowers the physical resolution while keeping the same refresh rate.
// If the same refresh rate can not be kept, then keeps the resolution and lowers the refresh rate.
//
VOID HandleBandWidth_SingleDacActive(PPDEV ppdev)
{
    NV_CFGEX_VALIDATE_BANDWIDTH_PARAMS Params;
    HEAD_RESOLUTION_INFO HeadResolution;
    ULONG ulHead;
    MODE_ENTRY RequestedMode;
    ULONG RefreshLoop, ResolutionLoop;
    ULONG NextLowerWidth, NextLowerHeight, NextLowerRefreshRate;

    RtlZeroMemory(&Params, sizeof(Params));
    Params.VidScalerHead = NV_CFGEX_VALIDATE_BANDWIDTH_SCALER_WORST_CASE;

    ulHead = ppdev->ulDeviceDisplay[0];
    if (ulHead == 0)
    {
        Params.HeadActive0 = 1;
    }
    if (ulHead == 1)
    {
        Params.HeadActive1 = 1;
    }
    //
    // Get the timing info for the physical mode.
    //
    HeadResolution.ulDisplayWidth = ppdev->rclCurrentDisplay[ulHead].right - ppdev->rclCurrentDisplay[ulHead].left;
    HeadResolution.ulDisplayHeight= ppdev->rclCurrentDisplay[ulHead].bottom - ppdev->rclCurrentDisplay[ulHead].top;
    HeadResolution.ulDisplayRefresh = ppdev->ulRefreshRate[ulHead];   

    HeadResolution.ulHead = ulHead;              
    HeadResolution.ulDeviceMask = ppdev->ulDeviceMask[ulHead];        
    HeadResolution.ulDeviceType = ppdev->ulDeviceType[ulHead];        
    HeadResolution.ulTVFormat = ppdev->ulTVFormat[ulHead];
    HeadResolution.ulDisplayPixelDepth = ppdev->cBitsPerPel;
    HeadResolution.ulOption = 0;

    DISPDBG((1,"PhysMode Head%d: %d x %d, %d BPP, %d HZ, devMask: 0x%x, devType: %d",
             ulHead,
             HeadResolution.ulDisplayWidth, HeadResolution.ulDisplayHeight,
             HeadResolution.ulDisplayPixelDepth, HeadResolution.ulDisplayRefresh,
             HeadResolution.ulDeviceMask, HeadResolution.ulDeviceType));

    if (!ConvertHeadResToTiming(ppdev, ulHead, &HeadResolution, &Params))
        return;

    RefreshLoop = ResolutionLoop = 1;

    RequestedMode.Width = (USHORT)(ppdev->rclCurrentDisplay[ulHead].right - ppdev->rclCurrentDisplay[ulHead].left);
    RequestedMode.Height= (USHORT)(ppdev->rclCurrentDisplay[ulHead].bottom - ppdev->rclCurrentDisplay[ulHead].top);
    RequestedMode.RefreshRate = (USHORT)(ppdev->ulRefreshRate[ulHead]);   
    RequestedMode.Depth = (USHORT)(ppdev->cBitsPerPel);
    RequestedMode.ValidMode = 1;

    //
    // We have now built up the NV_CFGEX_VALIDATE_BANDWIDTH_PARAMS structure. 
    // Now the quick success case. See if this mode passes. This is the most common case.
    //
    while (NvConfigGetEx (ppdev->hDriver, ppdev->hClient, ppdev->hDevice,
                          NV_CFGEX_VALIDATE_BANDWIDTH, &Params, sizeof(NV_CFGEX_VALIDATE_BANDWIDTH_PARAMS)))
    {
        //
        // We are failing the bandwidth requirements. We need to back off and try a lower physical mode.
        // See if there is a next lower refresh rate physical mode at the same resolution and color depth.
        // If cannot find the next lower refresh rate, try the next lower resolution at the same color depth
        // and refresh rate.
        //
        if (RefreshLoop || ResolutionLoop)
        {
            BOOLEAN bFoundNextLowerMode;

            if (RefreshLoop)
            {
                bFoundNextLowerMode = FindNextLowerRefreshRateMode(ppdev, ulHead, &RequestedMode, &NextLowerRefreshRate);
                if (bFoundNextLowerMode)
                {
                    HeadResolution.ulDisplayRefresh = NextLowerRefreshRate;
                }
            }
            else
            {
                bFoundNextLowerMode = FindNextLowerResolutionMode(ppdev, ulHead, &RequestedMode, &NextLowerWidth, &NextLowerHeight);
                if (bFoundNextLowerMode)
                {
                    HeadResolution.ulDisplayWidth = NextLowerWidth;
                    HeadResolution.ulDisplayHeight= NextLowerHeight;
                    HeadResolution.ulDisplayRefresh = ppdev->ulRefreshRate[ulHead];
                }
            }

            if (bFoundNextLowerMode)
            {
                HeadResolution.ulHead = ulHead;              
                HeadResolution.ulDeviceMask = ppdev->ulDeviceMask[ulHead];        
                HeadResolution.ulDeviceType = ppdev->ulDeviceType[ulHead];        
                HeadResolution.ulTVFormat = ppdev->ulTVFormat[ulHead];
                HeadResolution.ulDisplayPixelDepth = ppdev->cBitsPerPel;

                if (!ConvertHeadResToTiming(ppdev, ulHead, &HeadResolution, &Params))
                {
                    return;
                }

                //
                // Stick in lowered refresh rate or lowered resolution into the requested mode as we may need to iterate the next time.
                //
                if (RefreshLoop)
                {
                    RequestedMode.RefreshRate = (USHORT)NextLowerRefreshRate;
                }
                else
                {
                    RequestedMode.Width  = (USHORT)NextLowerWidth;
                    RequestedMode.Height = (USHORT)NextLowerHeight;
                }

                if (ResolutionLoop)
                {
                    RefreshLoop = 1;
                }

                //
                // Continue onto validating this combo.
                //
                continue;
            } // if bFoundNextLowerMode
            else
            {
                if (RefreshLoop)
                {
                    RefreshLoop = 0;
                    HeadResolution.ulDisplayRefresh = ppdev->ulRefreshRate[ulHead];
                    RequestedMode.RefreshRate = (USHORT)HeadResolution.ulDisplayRefresh;

                    if (!ConvertHeadResToTiming(ppdev, ulHead, &HeadResolution, &Params))
                        return;
                }
                else
                {
                    ResolutionLoop = 0;
                }
            }
        }
        else
        {
            DISPDBG((0, "Too bad. Could not find any combo which passes the bandwidth requirements. Returning"));
            return;
        }
    } // while GetExBandwidth()

    //
    // Update the physical mode fields in the ppdev for the head we are varying.
    //
    ppdev->rclCurrentDisplay[ulHead].right = ppdev->rclCurrentDisplay[ulHead].left + RequestedMode.Width;
    ppdev->rclCurrentDisplay[ulHead].bottom = ppdev->rclCurrentDisplay[ulHead].top + RequestedMode.Height;
    ppdev->ulRefreshRate[ulHead] = RequestedMode.RefreshRate;
    
    DISPDBG((1, "Head: %d, DevMask: 0x%x, Final Phys: (%d, %d), (%d, %d) %d bpp, %d Hz",
             ulHead, ppdev->ulDeviceMask[ulHead], 
             ppdev->rclCurrentDisplay[ulHead].left, ppdev->rclCurrentDisplay[ulHead].top,
             ppdev->rclCurrentDisplay[ulHead].right, ppdev->rclCurrentDisplay[ulHead].bottom,
             ppdev->cBitsPerPel, ppdev->ulRefreshRate[ulHead]));

    return;
}

//
// Verifies that the physical modes can be supported by the Bandwidth restrictions of the system.
// If not, lowers the physical resolution while keeping the same refresh rate.
// If the same refresh rate can not be kept, then keeps the resolution and lowers the refresh rate.
//
VOID HandleBandWidth_DualDacsActive(PPDEV ppdev)
{
    NV_CFGEX_VALIDATE_BANDWIDTH_PARAMS Params;
    HEAD_RESOLUTION_INFO HeadResolution[2];
    ULONG ulHead;
    MODE_ENTRY RequestedMode[2];
    ULONG i;
    ULONG RefreshLoop[2], ResolutionLoop[2];
    ULONG NextLowerWidth, NextLowerHeight, NextLowerRefreshRate;

    RtlZeroMemory(&Params, sizeof(Params));
    Params.VidScalerHead = NV_CFGEX_VALIDATE_BANDWIDTH_SCALER_WORST_CASE;
    Params.HeadActive0 = Params.HeadActive1 = 1;

    for (i = 0; i < ppdev->ulNumberDacsActive; i++)
    {
        ulHead = ppdev->ulDeviceDisplay[i];
        //
        // Get the timing info for the physical mode.
        //
        HeadResolution[i].ulDisplayWidth = ppdev->rclCurrentDisplay[ulHead].right - ppdev->rclCurrentDisplay[ulHead].left;
        HeadResolution[i].ulDisplayHeight= ppdev->rclCurrentDisplay[ulHead].bottom - ppdev->rclCurrentDisplay[ulHead].top;
        HeadResolution[i].ulDisplayRefresh = ppdev->ulRefreshRate[ulHead];   

        HeadResolution[i].ulHead = ulHead;              
        HeadResolution[i].ulDeviceMask = ppdev->ulDeviceMask[ulHead];        
        HeadResolution[i].ulDeviceType = ppdev->ulDeviceType[ulHead];        
        HeadResolution[i].ulTVFormat = ppdev->ulTVFormat[ulHead];
        HeadResolution[i].ulDisplayPixelDepth = ppdev->cBitsPerPel;
        HeadResolution[i].ulOption = 0;

        DISPDBG((1,"PhysMode Head%d: %d x %d, %d BPP, %d HZ, devMask: 0x%x, devType: %d",
                 ulHead,
                 HeadResolution[i].ulDisplayWidth, HeadResolution[i].ulDisplayHeight,
                 HeadResolution[i].ulDisplayPixelDepth, HeadResolution[i].ulDisplayRefresh,
                 HeadResolution[i].ulDeviceMask, HeadResolution[i].ulDeviceType));

        if (!ConvertHeadResToTiming(ppdev, ulHead, &HeadResolution[i], &Params))
            return;
    } // for NumberDacsActive


    for (i = 0; i < 2; i++)
    {
        RefreshLoop[i] = ResolutionLoop[i] = 1;

        RequestedMode[i].Width = (USHORT)(ppdev->rclCurrentDisplay[i].right  - ppdev->rclCurrentDisplay[i].left);
        RequestedMode[i].Height= (USHORT)(ppdev->rclCurrentDisplay[i].bottom - ppdev->rclCurrentDisplay[i].top);
        RequestedMode[i].RefreshRate = (USHORT)(ppdev->ulRefreshRate[i]);   
        RequestedMode[i].Depth = (USHORT)(ppdev->cBitsPerPel);
        RequestedMode[i].ValidMode = 1;
    }

    //
    // We have now built up the NV_CFGEX_VALIDATE_BANDWIDTH_PARAMS structure. 
    // Now the quick success case. See if this mode passes. This is the most common case.
    //
    i = 0;
    
    while (NvConfigGetEx (ppdev->hDriver, ppdev->hClient, ppdev->hDevice,
                          NV_CFGEX_VALIDATE_BANDWIDTH, &Params, sizeof(NV_CFGEX_VALIDATE_BANDWIDTH_PARAMS)))
    {
        //
        // We are failing the bandwidth requirements. We need to back off and try a lower physical mode.
        // See if there is a next lower refresh rate physical mode at the same resolution and color depth.
        // If cannot find the next lower refresh rate, try the next lower resolution at the same color depth
        // and refresh rate.
        //
        if (!RefreshLoop[i] && !ResolutionLoop[i])
        {
            if (i == 1)
            {
                DISPDBG((0, "Too bad. Could not find any combo which passes the bandwidth requirements. Returning"));
                return;
            }
            else
            {
                RefreshLoop[0] = ResolutionLoop[0] = 1;

                ulHead = ppdev->ulDeviceDisplay[0];
                HeadResolution[0].ulDisplayWidth = ppdev->rclCurrentDisplay[ulHead].right - ppdev->rclCurrentDisplay[ulHead].left;
                HeadResolution[0].ulDisplayHeight= ppdev->rclCurrentDisplay[ulHead].bottom - ppdev->rclCurrentDisplay[ulHead].top;
                HeadResolution[0].ulDisplayRefresh = ppdev->ulRefreshRate[ulHead];
                RequestedMode[0].Width = (USHORT)HeadResolution[0].ulDisplayWidth;
                RequestedMode[0].Height= (USHORT)HeadResolution[0].ulDisplayHeight;
                RequestedMode[0].RefreshRate = (USHORT)HeadResolution[0].ulDisplayRefresh;

                if (!ConvertHeadResToTiming(ppdev, ulHead, &HeadResolution[0], &Params))
                    return;

                i = 1;
            }
        }
        else
        {
            BOOLEAN bFoundNextLowerMode;

            ulHead = ppdev->ulDeviceDisplay[i];

            if (RefreshLoop[i])
            {
                bFoundNextLowerMode = FindNextLowerRefreshRateMode(ppdev, ulHead, &RequestedMode[i], &NextLowerRefreshRate);
                if (bFoundNextLowerMode)
                {
                    HeadResolution[i].ulDisplayRefresh = NextLowerRefreshRate;
                }
            }
            else
            {
                bFoundNextLowerMode = FindNextLowerResolutionMode(ppdev, ulHead, &RequestedMode[i], &NextLowerWidth, &NextLowerHeight);
                if (bFoundNextLowerMode)
                {
                    HeadResolution[i].ulDisplayWidth = NextLowerWidth;
                    HeadResolution[i].ulDisplayHeight= NextLowerHeight;
                    HeadResolution[i].ulDisplayRefresh = ppdev->ulRefreshRate[ulHead];
                }
            }

            if (bFoundNextLowerMode)
            {
                //
                // Found a mode. Now we need to validate against the bandwidth. 
                // So fill the info into the BANDWIDTH_PARAMS structure. First get the TIMING info.
                //
                HeadResolution[i].ulHead = ulHead;              
                HeadResolution[i].ulDeviceMask = ppdev->ulDeviceMask[ulHead];        
                HeadResolution[i].ulDeviceType = ppdev->ulDeviceType[ulHead];        
                HeadResolution[i].ulTVFormat = ppdev->ulTVFormat[ulHead];
                HeadResolution[i].ulDisplayPixelDepth = ppdev->cBitsPerPel;

                if (!ConvertHeadResToTiming(ppdev, ulHead, &HeadResolution[i], &Params))
                    return;

                //
                // Stick in lowered refresh rate or lowered resolution into the requested mode as we may need to iterate the next time.
                //
                if (RefreshLoop[i])
                {
                    RequestedMode[i].RefreshRate = (USHORT)NextLowerRefreshRate;
                }
                else
                {
                    RequestedMode[i].Width  = (USHORT)NextLowerWidth;
                    RequestedMode[i].Height = (USHORT)NextLowerHeight;
                }

                if (ResolutionLoop[i])
                {
                    RefreshLoop[i] = 1;
                }
                //
                // Continue onto validating this combo.
                // For any Head1 change, reset head0 and restart Head0 validation
                //
                if (i == 1)
                {
                    i = 0;
                }
                continue;
            } // if bFoundNextLowerMode
            else
            {
                if (RefreshLoop[i])
                {
                    RefreshLoop[i] = 0;

                    ulHead = ppdev->ulDeviceDisplay[i];
                    HeadResolution[i].ulDisplayRefresh = ppdev->ulRefreshRate[ulHead];
                    RequestedMode[i].RefreshRate = (USHORT)HeadResolution[i].ulDisplayRefresh;
                    
                    if (!ConvertHeadResToTiming(ppdev, ulHead, &HeadResolution[i], &Params))
                        return;
                }
                else
                {
                    ResolutionLoop[i] = 0;
                }
            }
        }
    }

    //
    // Update the physical mode fields in the ppdev.
    //
    for (i = 0; i < ppdev->ulNumberDacsActive; i++)
    {
        ulHead = ppdev->ulDeviceDisplay[i];
        ppdev->rclCurrentDisplay[ulHead].right  = ppdev->rclCurrentDisplay[ulHead].left + RequestedMode[ulHead].Width;
        ppdev->rclCurrentDisplay[ulHead].bottom = ppdev->rclCurrentDisplay[ulHead].top  + RequestedMode[ulHead].Height;
        ppdev->ulRefreshRate[ulHead] = RequestedMode[ulHead].RefreshRate;

        DISPDBG((1, "Head: %d, PhysHead: %d DevMask: 0x%x, Final Phys: (%d, %d), (%d, %d) %d bpp, %d Hz",
                 i, ulHead, ppdev->ulDeviceMask[i], 
                 ppdev->rclCurrentDisplay[i].left, ppdev->rclCurrentDisplay[i].top,
                 ppdev->rclCurrentDisplay[i].right, ppdev->rclCurrentDisplay[i].bottom,
                 ppdev->cBitsPerPel, ppdev->ulRefreshRate[i]));
    }

    return;
}


//
// For the physical head, finds the next lower refresh rate for the requested mode while keeping the same depth
// and resolution.
// Returns TRUE if a lower refresh rate was found.
// Returns FALSE if a lower refresh rate was not found.
// Note: The physical mode list contains low res modes so weed out modes less than 640x480 since GDI does not
// accept them.
// The lower refresh rate is returned in pNextLowerRefreshRate.
//
BOOLEAN FindNextLowerRefreshRateMode(PPDEV ppdev, ULONG ulHead, MODE_ENTRY *pRequestedMode, ULONG *pNextLowerRefreshRate)
{
    ULONG i;
    MODE_ENTRY *PhysModePtr;
    ULONG LowerRate;

    DISPDBG((1, "FindNextLowerRefreshRateMode() for: %d x %d,  %d bpp, %d HZ",
             pRequestedMode->Width, pRequestedMode->Height, pRequestedMode->Depth, pRequestedMode->RefreshRate));
             

    LowerRate = 0;
    PhysModePtr = ppdev->PhysModeList[ulHead];
    for (i=0; i < ppdev->ulNumPhysModes[ulHead]; i++)
    {
        if (PhysModePtr->ValidMode && 
            PhysModePtr->Width == pRequestedMode->Width &&
            PhysModePtr->Width >= 640 &&
            PhysModePtr->Height == pRequestedMode->Height &&
            PhysModePtr->Height >= 480 &&
            PhysModePtr->Depth == pRequestedMode->Depth &&
            PhysModePtr->RefreshRate  < pRequestedMode->RefreshRate)
        {
            //
            // Found a match. But we still need to scan the rest of the list as we want the highest 
            // among the lower rates.
            //
            if (PhysModePtr->RefreshRate > LowerRate)
            {
                LowerRate =  PhysModePtr->RefreshRate;
            }
            
        }
        PhysModePtr++;
    }

    if (LowerRate != 0)
    {
        //
        // We found a valid lower rate.
        //
        *pNextLowerRefreshRate = LowerRate;
        DISPDBG((1,"Found Lower Rate: %d HZ",LowerRate));
        return(TRUE);

    }
    else
    {
        //
        // We did not find a valid lower rate.
        //
        DISPDBG((1, "Could not find a lower refresh rate"));
        return(FALSE);
    }
}


//
// For the physical head, finds the next lower resolution for the requested mode while keeping the same depth
// and refreshrate.
// Returns TRUE if a lower resolution was found.
// Returns FALSE if a lower resolution rate was not found.
// Note: The physical mode list contains low res modes so weed out modes less than 640x480 since GDI does not
// accept them.
// The lower width is returned in pNextLowerWidth.
// The lower height is returned in pNextLowerHeight.
//
BOOLEAN FindNextLowerResolutionMode(PPDEV ppdev, ULONG ulHead, MODE_ENTRY *pRequestedMode, ULONG *pNextLowerWidth, ULONG *pNextLowerHeight)
{
    ULONG i;
    MODE_ENTRY *PhysModePtr;
    ULONG LowerWidth, LowerHeight;

    DISPDBG((1, "FindNextLowerResolutionMode() for: %d x %d,  %d bpp, %d HZ",
             pRequestedMode->Width, pRequestedMode->Height, pRequestedMode->Depth, pRequestedMode->RefreshRate));

    //
    // Vary the height. Depth and Refresh should be the same. Width should be <= the requested width.
    //
    LowerHeight = 0;
    LowerWidth =  0;
    PhysModePtr = ppdev->PhysModeList[ulHead];
    for (i=0; i < ppdev->ulNumPhysModes[ulHead]; i++)
    {
        if (PhysModePtr->ValidMode && 
            PhysModePtr->Width <= pRequestedMode->Width &&
            PhysModePtr->Width >= 640 &&
            PhysModePtr->Height < pRequestedMode->Height &&
            PhysModePtr->Height >= 480 &&
            PhysModePtr->Depth == pRequestedMode->Depth &&
            PhysModePtr->RefreshRate  == pRequestedMode->RefreshRate)
        {
            //
            // Found a match. But we still need to scan the rest of the list as we want the highest among
            // the lower resolutions.
            //
            if (PhysModePtr->Height > LowerHeight)
            {
                LowerHeight =  PhysModePtr->Height;
                LowerWidth =  PhysModePtr->Width;
            }
            
        }
        PhysModePtr++;
    }

    if (LowerHeight != 0)
    {
        //
        // We found a valid lower resolution.
        //
        *pNextLowerHeight = LowerHeight;
        *pNextLowerWidth =  LowerWidth;
        DISPDBG((1,"Found Lower resolution: %d x %d",LowerWidth, LowerHeight));
        return(TRUE);
    }
    else
    {
        //
        // We did not find a valid lower resolution.
        //
        DISPDBG((1, "Could not find a lower resolution"));
        return(FALSE);
    }
}

//
// Returns true if the GDI should do the EDID mode pruning.
// Returns FALSE to indicate that the display driver should the EDID mode pruning.
// We need to explicitly pass in all the required pieces of data as arguements since we
// don't have ppdev available at DrvGetModes() time.
//
BOOLEAN bQueryGDIModePruning(ULONG ulTwinView_State, ULONG ulPrimaryHead, ULONG ulPrimaryDeviceMask,
        ULONG ulHead, ULONG ulDeviceMask, ULONG ulACPISystem, ULONG ulGDIModePruning)
{
    //
    // If spanning mode, we always let the driver do the pruning
    //
    if (ulTwinView_State == NVTWINVIEW_STATE_SPAN)
    {
        return(FALSE);
    }

    //
    // If DualView mode, we always let the GDI do the pruning
    //
    if (ulTwinView_State == NVTWINVIEW_STATE_DUALVIEW)
    {
        return(TRUE);
    }

    //
    // If we are not dealing with the primary head, then let the driver handle the pruning.
    //
    if (ulHead != ulPrimaryHead || ulDeviceMask != ulPrimaryDeviceMask)
    {
        return(FALSE);
    }

    //
    // Handle  CRT depending on the registry key.
    //
    if ((ulPrimaryDeviceMask & BITMASK_ALL_CRT) &&
        (ulGDIModePruning == GDI_MODE_PRUNING_DESKTOP_LAPTOP_CRT || 
         ulGDIModePruning == GDI_MODE_PRUNING_DESKTOP_CRT_DFP))
    {
        return(TRUE);
    }

    //
    // Handle  DFP depending on the registry key.
    // Note we always want to let the driver handle mode pruning on laptops for DFPs.
    //
    if ((ulPrimaryDeviceMask & BITMASK_ALL_DFP) && ulACPISystem == 0 && 
          ulGDIModePruning == GDI_MODE_PRUNING_DESKTOP_CRT_DFP)
    {
        return(TRUE);
    }
    
    return(FALSE);
}

//
// Reads the SaveSettinngs registry entry into ppdev.
// No need to do extensive sanity checks since miniport would have done the sanity checks at boot time to ensure
// a valid SaveSettings structureexists in the regsistry.
//
VOID vReadSaveSettings(PPDEV ppdev)
{
    char * KeyName = "SaveSettings";
    ULONG KeyNameLen;
    WCHAR KeyNameUniCode[NV_MAX_REG_KEYNAME_LEN];
    ULONG KeyNameUniCodeLen;
    NV_REGISTRY_STRUCT regStructInput;
    ULONG Status;
    ULONG returnedDataLen;

    KeyNameLen = strlen(KeyName) + 1;
    EngMultiByteToUnicodeN(KeyNameUniCode,
                           NV_MAX_REG_KEYNAME_LEN,
                           &KeyNameUniCodeLen,
                           (PCHAR)KeyName,
                           KeyNameLen*sizeof(CHAR));

    regStructInput.keyName = (PWSTR)KeyNameUniCode;
    regStructInput.keyVal = ppdev->SaveSettings;
    regStructInput.keyValSize = sizeof(ppdev->SaveSettings);

    Status = EngDeviceIoControl(ppdev->hDriver,
                                IOCTL_VIDEO_QUERY_REGISTRY_BINARY_VAL,
                                (LPVOID)&regStructInput,
                                sizeof(NV_REGISTRY_STRUCT),
                                (LPVOID) (ppdev->SaveSettings),
                                sizeof(ppdev->SaveSettings),
                                &(returnedDataLen));

    DISPDBG((2,"ReadSaveSettings(): KeyValSize: 0x%x, returnedDataLen: 0x%x",regStructInput.keyValSize,
                returnedDataLen));
}

//
// Finds the slot in the SaveSettings table for the requested device combo using the 'pQuery' or 'pData' structure if
// provided, else uses the device combo specified in the ppdev.
// Should always return a valid pointer to a slot.
// Note: SaveSettings is not implemented for DualView mode for WinXP since the OS handles it fully.
//
NVTWINVIEWDATA *pFindSaveSettings(PPDEV ppdev, QUERY_SAVE_SETTINGS *pQuery, NVTWINVIEWDATA *pTwinViewData)
{
    ULONG i, ulPrimaryHead;
    ULONG ulState, ulOrientation, ulPrimaryDeviceMask, ulDeviceMask0, ulDeviceMask1;
    ULONG ulStateTable, ulOrientationTable, ulPrimaryDeviceMaskTable, ulDeviceMask0Table, ulDeviceMask1Table;
    NVTWINVIEWDATA *pData;
    NVTWINVIEW_DEVICE_TYPE_DATA *pDevData0, *pDevData1;

    if (pQuery)
    {
        ulState = pQuery->ulState;
        ulOrientation = pQuery->ulOrientation;
        ulPrimaryHead = pQuery->ulDeviceDisplay[0];
        ulPrimaryDeviceMask = pQuery->ulDeviceMask[ulPrimaryHead];
        ulDeviceMask0 = pQuery->ulDeviceMask[0];
        ulDeviceMask1 = pQuery->ulDeviceMask[1];
    }
    else
    if (pTwinViewData)
    {
        ulState = pTwinViewData->dwState;
        ulOrientation = pTwinViewData->dwOrientation;
        ulPrimaryHead = pTwinViewData->dwDeviceDisplay[0];
        ulPrimaryDeviceMask = pTwinViewData->NVTWINVIEWUNION.nvtwdevdata[ulPrimaryHead].dwDeviceMask;
        ulDeviceMask0 = pTwinViewData->NVTWINVIEWUNION.nvtwdevdata[0].dwDeviceMask;
        ulDeviceMask1 = pTwinViewData->NVTWINVIEWUNION.nvtwdevdata[1].dwDeviceMask;
    }
    else
    {
        ulState = ppdev->TwinView_State;
        ulOrientation = ppdev->TwinView_Orientation;
        ulPrimaryHead = ppdev->ulDeviceDisplay[0];
        ulPrimaryDeviceMask = ppdev->ulDeviceMask[ulPrimaryHead];
        ulDeviceMask0 = ppdev->ulDeviceMask[0];
        ulDeviceMask1 = ppdev->ulDeviceMask[1];
    }

    // Only for Rel10 and main.
    if (ulState == NVTWINVIEW_STATE_DUALVIEW)
    {
        DISPDBG((1,"pFindSaveSettings(): SaveSettings is not implemented for DualView for XP. Returnign NULL"));
        return(NULL);
    }
    
    //
    // Search for the slot for the standard mode case.
    //
    if (ulState == NVTWINVIEW_STATE_NORMAL)
    {
        for (i=0; i < NUM_ENTRIES_SAVE_SETTINGS; i++)
        {
            pData = &ppdev->SaveSettings[i];
            ulStateTable = pData->dwState;
            ulPrimaryHead = pData->dwDeviceDisplay[0];
            ulPrimaryDeviceMaskTable = pData->NVTWINVIEWUNION.nvtwdevdata[ulPrimaryHead].dwDeviceMask;
            if (ulStateTable == ulState && ulPrimaryDeviceMaskTable == ulPrimaryDeviceMask)
            {
                //
                // Found the slot.
                //
                return(pData);
            }
        }
    }

    //
    // Search for the slot for the clone mode case.
    //
    if (ulState == NVTWINVIEW_STATE_CLONE)
    {
        for (i=0; i < NUM_ENTRIES_SAVE_SETTINGS; i++)
        {
            pData = &ppdev->SaveSettings[i];
            ulStateTable = pData->dwState;
            ulDeviceMask0Table = pData->NVTWINVIEWUNION.nvtwdevdata[0].dwDeviceMask;
            ulDeviceMask1Table = pData->NVTWINVIEWUNION.nvtwdevdata[1].dwDeviceMask;
            if (ulStateTable == ulState && 
                ulDeviceMask0Table == ulDeviceMask0 && ulDeviceMask1Table == ulDeviceMask1)
            {
                //
                // Found the slot.
                //
                return(pData);
            }
        }
    }

    //
    // Search for the slot for the span mode case.
    //
    if (ulState == NVTWINVIEW_STATE_SPAN)
    {
        for (i=0; i < NUM_ENTRIES_SAVE_SETTINGS; i++)
        {
            pData = &ppdev->SaveSettings[i];
            ulStateTable = pData->dwState;
            ulOrientationTable = pData->dwOrientation;
            ulDeviceMask0Table = pData->NVTWINVIEWUNION.nvtwdevdata[0].dwDeviceMask;
            ulDeviceMask1Table = pData->NVTWINVIEWUNION.nvtwdevdata[1].dwDeviceMask;
            if (ulStateTable == ulState && ulOrientationTable == ulOrientation &&
                ulDeviceMask0Table == ulDeviceMask0 && ulDeviceMask1Table == ulDeviceMask1)
            {
                //
                // Found the slot.
                //
                return(pData);
            }
        }
    }

    DISPDBG((1,"pFindSaveSettings(): Something went wrong. Returning NULL"));
    return(NULL);
}
        


//
// Writes the NVTWINVIEWDATA for the current mode into the SaveSettingsTable in the
// appropriate slot.
// Note: It is assumed that the current mode in the ppdev already reflects the pData.
//
VOID vWriteSaveSettings(PPDEV ppdev)
{
    char * KeyName = "SaveSettings";
    ULONG KeyNameLen;
    WCHAR KeyNameUniCode[NV_MAX_REG_KEYNAME_LEN];
    ULONG KeyNameUniCodeLen;
    NV_REGISTRY_STRUCT regStruct;
    ULONG Status;
    ULONG returnedDataLen;
    NVTWINVIEWDATA *pFind;
    NVTWINVIEWDATA sCurrentData;
    ULONG ReturnedDataLength;

    //
    // First readin the TwnViewInfo structure for the just concluded modeset.
    //
    if (EngDeviceIoControl(ppdev->hDriver,
                        IOCTL_VIDEO_GET_TWINVIEW_STATE,
                        NULL,
                        0,
                        &sCurrentData,
                        sizeof(NVTWINVIEWDATA),
                        &ReturnedDataLength))
    {
        DISPDBG((1, "vWriteSaveSettings() something wrong: - IOCTL_VIDEO_GET_TWINVIEW_STATE failed"));
        return;
    }


    pFind = pFindSaveSettings(ppdev, NULL, NULL);
    if (pFind == NULL)
    {   
        DISPDBG((1,"vWriteSaveSettings(): Returning since pFindSaveSettings() returned failure"));
        return;
    }

    //
    // Copy the current mode into the SaveSettings Table of the ppdev in the correct slot.
    //
    *pFind = sCurrentData;
    
    //
    // Now copy the full SaveSettings table into the registry.
    //
    KeyNameLen = strlen(KeyName) + 1;
    EngMultiByteToUnicodeN(KeyNameUniCode,
                           NV_MAX_REG_KEYNAME_LEN,
                           &KeyNameUniCodeLen,
                           (PCHAR)KeyName,
                           KeyNameLen*sizeof(CHAR));

    regStruct.keyName = (PWSTR)KeyNameUniCode;
    regStruct.keyVal = ppdev->SaveSettings;
    regStruct.keyValSize = sizeof(ppdev->SaveSettings);

    Status = EngDeviceIoControl(ppdev->hDriver,
                                   IOCTL_VIDEO_SET_REGISTRY_VALUE,
                                   (LPVOID)(&regStruct),
                                   sizeof(NV_REGISTRY_STRUCT),
                                   (LPVOID) NULL,
                                   0,
                                   &returnedDataLen);
}

//
// Debug helper routine to print the SaveSettings table.
//
VOID vPrintSaveSettings(PPDEV ppdev, ULONG ulDebugLevel)
{
    ULONG i, ulHead;
    ULONG ulDeviceMask, ulDeviceMask0, ulDeviceMask1;
    NVTWINVIEWDATA *pData;
    NVTWINVIEW_DEVICE_TYPE_DATA *pDevData0, *pDevData1;
    CHAR *pcMode;

    for (i=0; i < NUM_ENTRIES_SAVE_SETTINGS; i++)
    {
        pData = &ppdev->SaveSettings[i];
        pDevData0 = &pData->NVTWINVIEWUNION.nvtwdevdata[0];
        pDevData1 = &pData->NVTWINVIEWUNION.nvtwdevdata[1];
        switch (pData->dwState)
        {
        case NVTWINVIEW_STATE_NORMAL:
            pcMode = "Standard";
            break;
        case NVTWINVIEW_STATE_CLONE:
            pcMode = "Clone";
            break;
        case NVTWINVIEW_STATE_SPAN:
            pcMode = "Spanning";
            break;
        default:
            pcMode = "Invalid mode";
            break;
        }
        DISPDBG((ulDebugLevel,"%d: %s,%d,  (%d,%d), (0x%x, %d), (0x%x, %d)",i,
            pcMode, pData->dwOrientation, pData->dwDeviceDisplay[0], pData->dwDeviceDisplay[1], 
            pDevData0->dwDeviceMask, pDevData0->dwTVFormat, pDevData1->dwDeviceMask, pDevData1->dwTVFormat));

        if (pData->DeskTopRectl.left == 0 && pData->DeskTopRectl.top == 0 &&
            pData->DeskTopRectl.right == 0 && pData->DeskTopRectl.bottom == 0)   
        {
            // don't print this since this slot is not valid.
        }
        else
        {
            // This slot is valid and occupied. Print the mode info.
            DISPDBG((ulDebugLevel, "   Desktop: (%d, %d), (%d, %d)", 
                pData->DeskTopRectl.left,pData->DeskTopRectl.top,pData->DeskTopRectl.right, pData->DeskTopRectl.bottom));
            DISPDBG((ulDebugLevel,"    Head0 virt: (%d, %d), (%d, %d)",
                pDevData0->VirtualRectl.left,pDevData0->VirtualRectl.top,pDevData0->VirtualRectl.right,pDevData0->VirtualRectl.bottom));
            DISPDBG((ulDebugLevel,"    Head0 phys: (%d, %d), (%d, %d), %dbpp, %dHZ\n",
                pDevData0->PhysicalRectl.left,pDevData0->PhysicalRectl.top,pDevData0->PhysicalRectl.right,pDevData0->PhysicalRectl.bottom,
                pDevData0->dwBpp, pDevData0->dwRefresh));
            DISPDBG((ulDebugLevel,"    Head1 virt: (%d, %d), (%d, %d)",
                pDevData1->VirtualRectl.left,pDevData1->VirtualRectl.top,pDevData1->VirtualRectl.right,pDevData1->VirtualRectl.bottom));
            DISPDBG((ulDebugLevel,"    Head0 phys: (%d, %d), (%d, %d), %dbpp, %dHZ",
                pDevData1->PhysicalRectl.left,pDevData1->PhysicalRectl.top,pDevData1->PhysicalRectl.right,pDevData1->PhysicalRectl.bottom,
                pDevData1->dwBpp, pDevData1->dwRefresh));
            DISPDBG((ulDebugLevel,""));
        }
        
    }

    return;
}
