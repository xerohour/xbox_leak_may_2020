/*++

Copyright (c) 2001  Microsoft Corporation

Module Name:

    smcdef.h

Abstract:

    This module defines the constants used for accessing the System Management
    Controller (SMC).

--*/

#ifndef _SMCDEF_
#define _SMCDEF_

//
// Define the SMBus slave address of the System Management Controller.
//

#define SMC_SLAVE_ADDRESS                           0x20

//
// Define the command codes for the System Management Controller.
//

#define SMC_COMMAND_FIRMWARE_REVISION               0x01
#define SMC_COMMAND_RESET                           0x02
#define SMC_COMMAND_TRAY_STATE                      0x03
#define SMC_COMMAND_VIDEO_MODE                      0x04
#define SMC_COMMAND_FAN_OVERRIDE                    0x05
#define SMC_COMMAND_REQUEST_FAN_SPEED               0x06
#define SMC_COMMAND_LED_OVERRIDE                    0x07
#define SMC_COMMAND_LED_STATES                      0x08
#define SMC_COMMAND_CPU_TEMPERATURE                 0x09
#define SMC_COMMAND_AIR_TEMPERATURE                 0x0A
#define SMC_COMMAND_AUDIO_CLAMP                     0x0B
#define SMC_COMMAND_DVD_TRAY_OPERATION              0x0C
#define SMC_COMMAND_OS_RESUME                       0x0D
#define SMC_COMMAND_WRITE_ERROR_CODE                0x0E
#define SMC_COMMAND_READ_ERROR_CODE                 0x0F
#define SMC_COMMAND_READ_FAN_SPEED                  0x10
#define SMC_COMMAND_INTERRUPT_REASON                0x11
#define SMC_COMMAND_WRITE_RAM_TEST_RESULTS          0x12
#define SMC_COMMAND_WRITE_RAM_TYPE                  0x13
#define SMC_COMMAND_READ_RAM_TEST_RESULTS           0x14
#define SMC_COMMAND_READ_RAM_TYPE                   0x15
#define SMC_COMMAND_LAST_REGISTER_WRITTEN           0x16
#define SMC_COMMAND_LAST_BYTE_WRITTEN               0x17
#define SMC_COMMAND_SOFTWARE_INTERRUPT              0x18
#define SMC_COMMAND_OVERRIDE_RESET_ON_TRAY_OPEN     0x19
#define SMC_COMMAND_OS_READY                        0x1A
#define SMC_COMMAND_SCRATCH                         0x1B

//
// Define the register flags for SMC_COMMAND_RESET.
//

#define SMC_RESET_ASSERT_RESET                      0x01
#define SMC_RESET_ASSERT_POWERCYCLE                 0x40
#define SMC_RESET_ASSERT_SHUTDOWN                   0x80

//
// Define the register flags for SMC_COMMAND_TRAY_STATE.
//

#define SMC_TRAY_STATE_ACTIVITY                     0x01
#define SMC_TRAY_STATE_STATE_MASK                   0x70
#define SMC_TRAY_STATE_CLOSED                       0x00
#define SMC_TRAY_STATE_OPEN                         0x10
#define SMC_TRAY_STATE_UNLOADING                    0x20
#define SMC_TRAY_STATE_OPENING                      0x30
#define SMC_TRAY_STATE_NO_MEDIA                     0x40
#define SMC_TRAY_STATE_CLOSING                      0x50
#define SMC_TRAY_STATE_MEDIA_DETECT                 0x60
#define SMC_TRAY_STATE_RESET                        0x70

//
// Define the register flags for SMC_COMMAND_VIDEO_MODE.
//

#define SMC_VIDEO_MODE_VMODE_MASK                   0x07
#define SMC_VIDEO_MODE_VMODE0                       0x01
#define SMC_VIDEO_MODE_VMODE1                       0x02
#define SMC_VIDEO_MODE_VMODE2                       0x04
#define SMC_VIDEO_MODE_SCART                        0x00
#define SMC_VIDEO_MODE_HDTV                         0x01
#define SMC_VIDEO_MODE_VGA                          0x02
#define SMC_VIDEO_MODE_RFU                          0x03
#define SMC_VIDEO_MODE_SVIDEO                       0x04
#define SMC_VIDEO_MODE_STANDARD                     0x06
#define SMC_VIDEO_MODE_NONE                         0x07

//
// Define the register flags for SMC_COMMAND_FAN_OVERRIDE.
//

#define SMC_FAN_OVERRIDE_DEFAULT                    0x00
#define SMC_FAN_OVERRIDE_USE_REQUESTED_FAN_SPEED    0x01

//
// Define the register flags for SMC_COMMAND_LED_OVERRIDE.
//

#define SMC_LED_OVERRIDE_DEFAULT                    0x00
#define SMC_LED_OVERRIDE_USE_REQUESTED_LED_STATES   0x01

//
// Define the register flags for SMC_COMMAND_LED_STATES.
//

#define SMC_LED_STATES_GREEN_STATE0                 0x01
#define SMC_LED_STATES_GREEN_STATE1                 0x02
#define SMC_LED_STATES_GREEN_STATE2                 0x04
#define SMC_LED_STATES_GREEN_STATE3                 0x08
#define SMC_LED_STATES_RED_STATE0                   0x10
#define SMC_LED_STATES_RED_STATE1                   0x20
#define SMC_LED_STATES_RED_STATE2                   0x40
#define SMC_LED_STATES_RED_STATE3                   0x80

//
// Define the register flags for SMC_COMMAND_AUDIO_CLAMP.
//

#define SMC_AUDIO_CLAMP_RELEASE                     0x00
#define SMC_AUDIO_CLAMP_CLAMP                       0x01

//
// Define the register flags for SMC_COMMAND_DVD_TRAY_OPERATION.
//

#define SMC_DVD_TRAY_OPERATION_OPEN                 0x00
#define SMC_DVD_TRAY_OPERATION_CLOSE                0x01

//
// Define the register flags for SMC_COMMAND_OS_RESUME.
//

#define SMC_OS_RESUME_SHUTDOWN_COMPLETE             0x01
#define SMC_OS_RESUME_SHUTDOWN_IN_PROGRESS          0x02
#define SMC_OS_RESUME_RESUME                        0x04
#define SMC_OS_RESUME_REBOOT                        0x08

//
// Define the register flags for SMC_COMMAND_INTERRUPT_REASON.
//

#define SMC_INTERRUPT_REASON_SHUTDOWN               0x01
#define SMC_INTERRUPT_REASON_TRAY_DETECTED          0x02
#define SMC_INTERRUPT_REASON_TRAY_OPENED            0x04
#define SMC_INTERRUPT_REASON_NEW_AV_PACK            0x08
#define SMC_INTERRUPT_REASON_NO_AV_PACK             0x10
#define SMC_INTERRUPT_REASON_TRAY_EJECT             0x20
#define SMC_INTERRUPT_REASON_TRAY_CLOSED            0x40

//
// Define the register values for SMC_COMMAND_OVERRIDE_RESET_ON_TRAY_OPEN.
//

#define SMC_RESET_ON_TRAY_OPEN_SECURE_MODE          0x00
#define SMC_RESET_ON_TRAY_OPEN_NONSECURE_MODE       0x01

//
// Define the register flags for SMC_COMMAND_OS_READY.
//

#define SMC_OS_READY_READY                          0x01

//
// Define the register flags for SMC_COMMAND_SCRATCH.  This register is defined
// by the operating system, so these flags can be freely added to or modified.
//

#define SMC_SCRATCH_TRAY_EJECT_PENDING              0x01
#define SMC_SCRATCH_DISPLAY_FATAL_ERROR             0x02
#define SMC_SCRATCH_SHORT_ANIMATION                 0x04
#define SMC_SCRATCH_DASHBOARD_BOOT                  0x08

#endif  // SMCDEF
