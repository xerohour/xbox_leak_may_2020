//****************************************************************************
//****************************************************************************
//
// ac97reg.h
//
//
//
// Description:
// This file contains the codec register descriptions.
//
//
// Copyright (c) 2000 nVidia Corporation.
//
//****************************************************************************
//****************************************************************************




#ifndef _AC97REG_H_
#define _AC97REG_H_


//****************************************************************************
//****************************************************************************
//
// Defines
//
//****************************************************************************
//****************************************************************************

//
// These are the AC'97 registers found in the 2.1 specification.
//
typedef enum
{
    AC97REG_RESET = 0,
    AC97REG_FRONT_VOLUME,               // 0x02
    AC97REG_HEADPHONE_VOLUME,           // 0x04
    AC97REG_MONOOUT_VOLUME,             // 0x06
    AC97REG_MASTER_TONE,                // 0x08
    AC97REG_PCBEEP_VOLUME,              // 0x0a
    AC97REG_PHONE_VOLUME,               // 0x0c
    AC97REG_MIC_VOLUME,                 // 0x0e
    AC97REG_LINE_IN_VOLUME,             // 0x10
    AC97REG_CD_VOLUME,                  // 0x12
    AC97REG_VIDEO_VOLUME,               // 0x14
    AC97REG_AUX_VOLUME,                 // 0x16
    AC97REG_PCM_OUT_VOLUME,             // 0x18
    AC97REG_RECORD_SELECT,              // 0x1a
    AC97REG_RECORD_GAIN,                // 0x1c
    AC97REG_RECORD_GAIN_MIC,            // 0x1e
    AC97REG_GENERAL = 0x10,             // 0x20
    AC97REG_3D_CTRL,                    // 0x22
    AC97REG_RESERVED0,                  // 0x24
    AC97REG_POWERDOWN,                  // 0x26

    //
    // AC97 2.0 registers (0x28 - 0x3a)
    //
    AC97REG_EXT_AUDIO_ID,               // 0x28
    AC97REG_EXT_AUDIO_CTRL,             // 0x2a
    AC97REG_FRONT_RATE,                 // 0x2c
    AC97REG_SURR_RATE,                  // 0x2e
    AC97REG_LFE_RATE,                   // 0x30
    AC97REG_LR_RATE,                    // 0x32
    AC97REG_MIC_RATE,                   // 0x34
    AC97REG_6CH_VOL_CLFE,               // 0x36
    AC97REG_6CH_VOL_SURR,               // 0x38
    AC97REG_RESERVED1,                  // 0x3a

    //
    // Modem registers (0x3c - 0x58)
    //
    AC97REG_EXT_MODEM_ID,               // 0x3c
    AC97REG_EXT_MODEM_CTRL,             // 0x3e
    AC97REG_LINE1_RATE = 0x20,          // 0x40
    AC97REG_LINE2_RATE,                 // 0x42
    AC97REG_HANDSET_RATE,               // 0x44
    AC97REG_LINE1_LEVEL,                // 0x46
    AC97REG_LINE2_LEVEL,                // 0x48
    AC97REG_HANDSET_LEVEL,              // 0x4a
    AC97REG_GPIO_CONFIG,                // 0x4c
    AC97REG_GPIO_POLARITY,              // 0x4e
    AC97REG_GPIO_STICKY,                // 0x50
    AC97REG_GPIO_WAKEUP,                // 0x52
    AC97REG_GPIO_STATUS,                // 0x54
    AC97REG_MISC_MODEM_CTRL,            // 0x56
    AC97REG_RESERVED2,                  // 0x58

    //
    // Vendor reserved registers (0x5a - 0x7a)
    //
    AC97REG_VENDOR_RESERVED0,           // 0x5a
    AC97REG_VENDOR_RESERVED1,           // 0x5c
    AC97REG_VENDOR_RESERVED2,           // 0x5e
    AC97REG_VENDOR_RESERVED3 = 0x30,    // 0x60
    AC97REG_VENDOR_RESERVED4,           // 0x62
    AC97REG_VENDOR_RESERVED5,           // 0x64
    AC97REG_VENDOR_RESERVED6,           // 0x66
    AC97REG_VENDOR_RESERVED7,           // 0x68
    AC97REG_VENDOR_RESERVED8,           // 0x6a
    AC97REG_VENDOR_RESERVED9,           // 0x6c
    AC97REG_VENDOR_RESERVED10,          // 0x6e
    AC97REG_VENDOR_RESERVED11,          // 0x70
    AC97REG_VENDOR_RESERVED12,          // 0x72
    AC97REG_VENDOR_RESERVED13,          // 0x74
    AC97REG_VENDOR_RESERVED14,          // 0x76
    AC97REG_VENDOR_RESERVED15,          // 0x78
    AC97REG_VENDOR_RESERVED16,          // 0x7a

    //
    // Vendor IDs (0x7c - 0x7e)
    //
    AC97REG_VENDOR_ID1 = 0x3e,          // 0x7c
    AC97REG_VENDOR_ID2,                 // 0x7e


    AC97REG_INVALID
} AC97REGISTER;
#define FIRST_AC97REG       AC97REG_RESET
#define LAST_AC97REG        AC97REG_VENDOR_ID2
#define NUM_AC97REGS        AC97REG_INVALID


//
// Reset Register Bit Defines
//
#define AC97REG_RESET_VALUE                         0x0000

#define AC97REG_RESET_ID0                           0x0001  // Dedicated Mic In Channel
#define AC97REG_RESET_ID1                           0x0002  // Reserved
#define AC97REG_RESET_ID2                           0x0004  // Bass & Treble Control
#define AC97REG_RESET_ID3                           0x0008  // Simulated Stereo
#define AC97REG_RESET_ID4                           0x0010  // Headphone Out Support
#define AC97REG_RESET_ID5                           0x0020  // Loudness Support
#define AC97REG_RESET_ID6                           0x0040  // 18-bit DAC Resolution
#define AC97REG_RESET_ID7                           0x0080  // 20-bit DAC Resolution
#define AC97REG_RESET_ID8                           0x0100  // 18-bit ADC Resolution
#define AC97REG_RESET_ID9                           0x0200  // 20-bit ADC Resolution


//
// 3D Bits Defines
//
#define AC97REG_RESET_3D_MASK                       0x7c00
#define AC97REG_RESET_3D_SHIFT                      10

#define AC97REG_RESET_3D_NONE                       0x0000
#define AC97REG_RESET_3D_ANALOG_DEVICES             0x0400
#define AC97REG_RESET_3D_CREATIVE                   0x0800
#define AC97REG_RESET_3D_NATIONAL                   0x0c00
#define AC97REG_RESET_3D_YAMAHA                     0x1000
#define AC97REG_RESET_3D_BBE                        0x1400
#define AC97REG_RESET_3D_CRYSTAL                    0x1800
#define AC97REG_RESET_3D_QSOUND                     0x1c00
#define AC97REG_RESET_3D_SPATIALIZER                0x2000
#define AC97REG_RESET_3D_SRS                        0x2400
#define AC97REG_RESET_3D_PLATFORM_TECH              0x2800
#define AC97REG_RESET_3D_AKM                        0x2c00
#define AC97REG_RESET_3D_AUREAL                     0x3000
#define AC97REG_RESET_3D_AZTECH                     0x3400
#define AC97REG_RESET_3D_BINAURA                    0x3800
#define AC97REG_RESET_3D_ESS                        0x3c00
#define AC97REG_RESET_3D_HARMAN_INT                 0x4000
#define AC97REG_RESET_3D_NVIDIA                     0x4400
#define AC97REG_RESET_3D_PHILIPS                    0x4800
#define AC97REG_RESET_3D_TI                         0x4c00
#define AC97REG_RESET_3D_VLSI                       0x5000
#define AC97REG_RESET_3D_TRITECH                    0x5400
#define AC97REG_RESET_3D_REALTEK                    0x5800
#define AC97REG_RESET_3D_SAMSUNG                    0x5c00
#define AC97REG_RESET_3D_WOLFSON                    0x6000
#define AC97REG_RESET_3D_DELTA_INTEGRATION          0x6400
#define AC97REG_RESET_3D_SIGMATEL                   0x6800
#define AC97REG_RESET_3D_UNDEFINED0                 0x6c00
#define AC97REG_RESET_3D_ROCKWELL                   0x7000


//
// Volume most significant bit masks
//
#define AC97REG_VOLUME_LEFT_MSB_MASK                0x2000  // Determine bit resolution
#define AC97REG_VOLUME_RIGHT_MSB_MASK               0x0020  // Determine bit resolution
#define AC97REG_VOLUME_LEFT_MSB_UNSUPPORTED_MASK    0x1f00  // Mask off unsupported bits
#define AC97REG_VOLUME_RIGHT_MSB_UNSUPPORTED_MASK   0x001f  // Mask off unsupported bits


//
// Front Volume Register bit defines
//
#define AC97REG_FRONT_DEFAULT                       0x8000  // The default register value
#define AC97REG_FRONT_VOLUME_UNMUTED                0x0000  // The unmuted value
#define AC97REG_FRONT_MSB_MASK                      (AC97REG_VOLUME_LEFT_MSB_MASK | AC97REG_VOLUME_RIGHT_MSB_MASK)


//
// Headphone Volume Register bit defines
//
#define AC97REG_HEADPHONE_DEFAULT                   0x8000  // The default register value
#define AC97REG_HPHONE_MSB_MASK                     (AC97REG_VOLUME_LEFT_MSB_MASK | AC97REG_VOLUME_RIGHT_MSB_MASK)


//
// Master Mono Volume Register bit defines
//
#define AC97REG_MONOOUT_DEFAULT                     0x8000  // The default register value
#define AC97REG_MONOOUT_MSB_MASK                    AC97REG_VOLUME_RIGHT_MSB_MASK


//
// Tone Register bit defines
//
#define AC97REG_TONE_BYPASS_VALUE                   0x0f0f  // Tone bypass value
#define AC97REG_BASS_LSB_MASK                       0x0100  // Determine bit resolution
#define AC97REG_TREBLE_LSB_MASK                     0x0001  // Determine bit resolution


//
// PC BEEP Register bit defines
//
#define AC97REG_PCBEEP_DEFAULT                      0x8000  // The default register value


//
// Phone Register bit defines
//
#define AC97REG_PHONE_DEFAULT                       0x8008  // The default register value


//
// Video Register bit defines
//
#define AC97REG_VIDEO_DEFAULT                       0x8808  // The default register value


//
// Aux Register bit defines
//
#define AC97REG_AUX_DEFAULT                         0x8808  // The default register value


//
// Record Select Control Register bit defines
//
#define AC97REG_RSC_MIC                             0x0000  // Microphone
#define AC97REG_RSC_CD                              0x0001  // CD
#define AC97REG_RSC_VIDEO                           0x0002  // Video
#define AC97REG_RSC_AUX                             0x0003  // AUX
#define AC97REG_RSC_LINEIN                          0x0004  // Line In
#define AC97REG_RSC_STEREO_MIX                      0x0005  // Stereo Mix
#define AC97REG_RSC_MONO_MIX                        0x0006  // Mono Mix
#define AC97REG_RSC_PHONE                           0x0007  // Phone


//
// Record Gain Mic Register bit defines
//
#define AC97REG_RGM_DEFAULT                         0x8000  // The default register value


//
// General Purpose Register bit defines
//
#define AC97REG_GENERAL_DEFAULT                     0x0000  // The default register value
#define AC97REG_GENERAL_POP                         0x8000  // 3D Bypass bit
#define AC97REG_GENERAL_ST                          0x4000  // Simulated stereo bit
#define AC97REG_GENERAL_3D                          0x2000  // 3D stereo enable bit
#define AC97REG_GENERAL_LD                          0x1000  // Loudness bit
#define AC97REG_GENERAL_MIX                         0x0200  // Mix select bit
#define AC97REG_GENERAL_MS                          0x0100  // Mic Select bit


//
// 3D Stereo Enhancement Register bit defines
//
#define AC97REG_3DCTRL_DEPTH_MASK                   0x00ff  // Masks off center bits
#define AC97REG_3DCTRL_DEPTH_MASK1                  0x0000  // Determine if control is fixed
#define AC97REG_3DCTRL_DEPTH_MASK2                  0x000f  // Determine if control is fixed
#define AC97REG_3DCTRL_CENTER_MASK                  0xff00  // Masks off depth bits
#define AC97REG_3DCTRL_CENTER_MASK1                 0x0000  // Determine if control is fixed
#define AC97REG_3DCTRL_CENTER_MASK2                 0x0f00  // Determine if control is fixed


//
// Powerdown Control/Status Register bit defines
//
#define AC97REG_PCS_EAPD                            0x8000  // External Amp Powerdown
#define AC97REG_PCS_PR6                             0x4000  // Alt Line Output Buffer Powerdown
#define AC97REG_PCS_PR5                             0x2000  // Internal Clock Disable
#define AC97REG_PCS_PR4                             0x1000  // AC-Link Power Down (BIT_CLK off)
#define AC97REG_PCS_PR3                             0x0800  // Analog Mixer Off (Vref off)
#define AC97REG_PCS_PR2                             0x0400  // Analog Mixer Off (Vref still on)
#define AC97REG_PCS_PR1                             0x0200  // DACs Powerdown
#define AC97REG_PCS_PR0                             0x0100  // ADCs and Input Mux Powerdown

#define AC97REG_PCS_POWER_OFF                       0xff00
#define AC97REG_PCS_FULL_POWER                      0x0000

#define AC97REG_PCS_REF                             0x0008  // Vreg at nominal levels
#define AC97REG_PCS_ANL                             0x0004  // Mixer, Mux, and Volume ctrls ready
#define AC97REG_PCS_DAC                             0x0002  // DAC ready to accept data
#define AC97REG_PCS_ADC                             0x0001  // ADC ready to transmit data

#define AC97REG_PCS_STATUS_READY                    (AC97REG_PCS_REF | AC97REG_PCS_ANL | AC97REG_PCS_DAC | AC97REG_PCS_ADC)


//
// Extended Audio ID Register bit defines
//

//
// Codec Config Bits (Primary = 0x00, Secondary = 0x01, 0x10, 0x11)
//
#define AC97REG_EAID_ID1                            0x8000  // Codec config ID1 bit
#define AC97REG_EAID_ID0                            0x4000  // Codec config ID0 bit
#define AC97REG_EAID_CONFIG_MASK                    (AC97REG_EAID_ID1 | AC97REG_EAID_ID0)

#define AC97REG_EAID_AMAP                           0x0200  // Optional slot/DAC mappings bit

#define AC97REG_EAID_LDAC                           0x0100  // LFE DAC bit
#define AC97REG_EAID_SDAC                           0x0080  // Surround DAC bit
#define AC97REG_EAID_CDAC                           0x0040  // Center DAC bit
#define AC97REG_EAID_CHANNEL_MASK                   (AC97REG_EAID_LDAC | AC97REG_EAID_SDAC | AC97REG_EAID_CDAC)

#define AC97REG_EAID_VRM                            0x0008  // Variable Rate Mic input bit
#define AC97REG_EAID_DRA                            0x0002  // Double Rate bit
#define AC97REG_EAID_VRA                            0x0001  // Variable sample Rate bit
#define AC97REG_EAID_VARIABLE_RATE_MASK             (AC97REG_EAID_VRM | AC97REG_EAID_DRA | AC97REG_EAID_VRA)


//
// Extended Audio Status and Control Register bit defines
//
#define AC97REG_EASCR_PRL                           0x4000  // MIC ADC off
#define AC97REG_EASCR_PRK                           0x2000  // PCM LFE DAC off
#define AC97REG_EASCR_PRJ                           0x1000  // PCM Surround DAC off
#define AC97REG_EASCR_PRI                           0x0800  // PCM Center DAC off
#define AC97REG_EASCR_POWER_MASK                    (AC97REG_EASCR_PRI | AC97REG_EASCR_PRJ | AC97REG_EASCR_PRK | AC97REG_EASCR_PRL)

#define AC97REG_EASCR_MADC                          0x0200  // Mic ADC bit
#define AC97REG_EASCR_LDAC                          0x0100  // LFE DAC bit
#define AC97REG_EASCR_SDAC                          0x0080  // Surround DAC bit
#define AC97REG_EASCR_CDAC                          0x0040  // Center DAC bit

#define AC97REG_EASCR_VRM                           0x0008  // Variable Rate Mic input bit
#define AC97REG_EASCR_DRA                           0x0002  // Double Rate bit
#define AC97REG_EASCR_VRA                           0x0001  // Variable sample Rate bit

//
// Double Rate Bit Mask
//
#define AC97REG_DR_MASK                             0xffff0000

//
// Default Audio Sample Rate
//
#define AC97REG_ASRC_DEFAULT                        0xbb80

//
// Default Six Channel Volume Control Value
//
#define AC97REG_6CVC_DEFAULT                        0x8080
#define AC97REG_CENTER_MUTE_MASK                    0x0080
#define AC97REG_LFE_MUTE_MASK                       0x8000


//
// Extended Modem ID Register Bits
//
#define AC97REG_EMID_INVALID                        0xffff  // Register is not supported


//
// Codec ID Register Bits
//
#define AC97REG_VID_MASK                            0xffff  // Allow all ID bits
#define AC97REG_VID1_F_MASK                         0xff00  // First character of ID
#define AC97REG_VID1_S_MASK                         0x00ff  // Second character of ID
#define AC97REG_VID2_T_MASK                         0xff00  // Third character of ID
#define AC97REG_VID2_REV_MASK                       0x00ff  // Revision ID mask

//
// Codec IDs
//
#define AC97REG_VID1_ADI                            0x4144  // "AD"I ID1
#define AC97REG_VID2_ADI                            0x5300  // AD"I" ID2
#define AC97REG_VID2_ADI_REVISION_MASK              0x00ff  // Rev ID Mask
#define AC97REG_VID2_ADI_REV_1881                   0x0040  // ADI1881
#define AC97REG_VID2_ADI_REV_1885                   0x0060  // ADI1885

#define AC97REG_VID1_CRYSTAL                        0x4352  // "CR"ystal ID1
#define AC97REG_VID2_CRYSTAL                        0x5900  // Cr"Y"stal ID2
#define AC97REG_VID2_CRYSTAL_PID_MASK               0x0070  // Part ID Mask
#define AC97REG_VID2_CRYSTAL_RID_MASK               0x0070  // Rev ID Mask
#define AC97REG_VID2_CRYSTAL_PID_4297               0x0000  // CS4297
#define AC97REG_VID2_CRYSTAL_PID_4297A              0x0010  // CS4297A
#define AC97REG_VID2_CRYSTAL_PID_4294C              0x0020  // CS4294 Rev C
#define AC97REG_VID2_CRYSTAL_PID_4298               0x0020  // CS4298
#define AC97REG_VID2_CRYSTAL_PID_4299               0x0030  // CS4299
#define AC97REG_VID2_CRYSTAL_PID_4201               0x0040  // CS4201



#endif
