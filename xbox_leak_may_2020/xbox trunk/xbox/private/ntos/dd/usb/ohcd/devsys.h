#ifndef __DEVSYS_H__
#define __DEVSYS_H__

#ifdef PERFORM_DEVSYS_OPERATIONS
//
//  If development system only operations are on
//  the macros call functions
//

#define OHCD_DEVSYS_CHECK_HARDWARE(_DeviceExtension_)   OHCD_DevSysCheckHardware(_DeviceExtension_);
#define OHCD_DEVSYS_TAKE_CONTROL(_DeviceExtension_) OHCD_DevSysTakeControl(_DeviceExtension_);

// 
// The functions behind the macros
//
VOID
OHCD_DevSysCheckHardware(
    IN POHCD_DEVICE_EXTENSION DeviceExtension
    );

VOID
OHCD_DevSysTakeControl(
    IN POHCD_DEVICE_EXTENSION DeviceExtension
    );

#else

//
//  If development system only operations are off
//  the macros are NOPs.
//
#define OHCD_DEVSYS_CHECK_HARDWARE(_DeviceExtension_)
#define OHCD_DEVSYS_TAKE_CONTROL(_DeviceExtension_)

#endif


#endif//__DEVSYS_H__

