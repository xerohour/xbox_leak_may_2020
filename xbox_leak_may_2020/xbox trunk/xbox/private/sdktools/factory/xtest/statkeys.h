// xmtakeys.h

#pragma once

#ifndef _XMTA_KEYS_
#define _XMTA_KEYS_

//Key values for ReportStatistic() functions.

//***************************
//Some XGRAFX HDTV key values
//
//pixelcount test stats
#define HDTV_480P_G_PIXELCOUNT _T("HDTV_480P_G_PIXELCOUNT")
#define HDTV_480P_R_PIXELCOUNT _T("HDTV_480P_R_PIXELCOUNT")
#define HDTV_480P_B_PIXELCOUNT _T("HDTV_480P_B_PIXELCOUNT")
#define HDTV_720P_G_PIXELCOUNT _T("HDTV_720P_G_PIXELCOUNT")
#define HDTV_720P_R_PIXELCOUNT _T("HDTV_720P_R_PIXELCOUNT")
#define HDTV_720P_B_PIXELCOUNT _T("HDTV_720P_B_PIXELCOUNT")
#define HDTV_1080I_G_PIXELCOUNT _T("HDTV_1080I_G_PIXELCOUNT")
#define HDTV_1080I_R_PIXELCOUNT _T("HDTV_1080I_R_PIXELCOUNT")
#define HDTV_1080I_B_PIXELCOUNT _T("HDTV_1080I_B_PIXELCOUNT")
#define HDTV_480P_PIXEL_THRESHOLD _T("HDTV_480P_PIXEL_THRESHOLD")
#define HDTV_720P_PIXEL_THRESHOLD _T("HDTV_720P_PIXEL_THRESHOLD")
#define HDTV_1080I_PIXEL_THRESHOLD _T("HDTV_1080I_PIXEL_THRESHOLD")


//dac linearity test stats
#define HDTV_480P_Y_DAC_FULLSCALE _T("HDTV_480P_Y_DAC_FULLSCALE")
#define HDTV_480P_PR_DAC_FULLSCALE _T("HDTV_480P_PR_DAC_FULLSCALE")
#define HDTV_480P_PB_DAC_FULLSCALE _T("HDTV_480P_PB_DAC_FULLSCALE")
#define HDTV_720P_Y_DAC_FULLSCALE _T("HDTV_720P_Y_DAC_FULLSCALE")
#define HDTV_720P_PR_DAC_FULLSCALE _T("HDTV_720P_PR_DAC_FULLSCALE")
#define HDTV_720P_PB_DAC_FULLSCALE _T("HDTV_720P_PB_DAC_FULLSCALE")
#define HDTV_1080I_Y_DAC_FULLSCALE _T("HDTV_1080I_Y_DAC_FULLSCALE")
#define HDTV_1080I_PR_DAC_FULLSCALE _T("HDTV_1080I_PR_DAC_FULLSCALE")
#define HDTV_1080I_PB_DAC_FULLSCALE _T("HDTV_1080I_PB_DAC_FULLSCALE")

//timings test stats
#define HDTV_480P_TIMINGS_HSYNC_HIGH _T("HDTV_480P_TIMINGS_HSYNC_HIGH")
#define HDTV_480P_TIMINGS_HSYNC_LOW _T("HDTV_480P_TIMINGS_HSYNC_LOW")
#define HDTV_480P_TIMINGS_HFPORCH _T("HDTV_480P_TIMINGS_HFPORCH")
#define HDTV_480P_TIMINGS_HBPORCH _T("HDTV_480P_TIMINGS_HBPORCH")
#define HDTV_480P_TIMINGS_VSYNC_HIGH _T("HDTV_480P_TIMINGS_VSYNC_HIGH")
#define HDTV_480P_TIMINGS_VSYNC_LOW _T("HDTV_480P_TIMINGS_VSYNC_LOW")
#define HDTV_480P_TIMINGS_VFPORCH _T("HDTV_480P_TIMINGS_VFPORCH")
#define HDTV_480P_TIMINGS_VBPORCH _T("HDTV_480P_TIMINGS_VBPORCH")
#define HDTV_720P_TIMINGS_HSYNC_HIGH _T("HDTV_720P_TIMINGS_HSYNC_HIGH")
#define HDTV_720P_TIMINGS_HSYNC_LOW _T("HDTV_720P_TIMINGS_HSYNC_LOW")
#define HDTV_720P_TIMINGS_HFPORCH _T("HDTV_720P_TIMINGS_HFPORCH")
#define HDTV_720P_TIMINGS_HBPORCH _T("HDTV_720P_TIMINGS_HBPORCH")
#define HDTV_720P_TIMINGS_VSYNC_HIGH _T("HDTV_720P_TIMINGS_VSYNC_HIGH")
#define HDTV_720P_TIMINGS_VSYNC_LOW _T("HDTV_720P_TIMINGS_VSYNC_LOW")
#define HDTV_720P_TIMINGS_VFPORCH _T("HDTV_720P_TIMINGS_VFPORCH")
#define HDTV_720P_TIMINGS_VBPORCH _T("HDTV_720P_TIMINGS_VBPORCH")
#define HDTV_1080I_TIMINGS_HSYNC_HIGH _T("HDTV_1080I_TIMINGS_HSYNC_HIGH")
#define HDTV_1080I_TIMINGS_HSYNC_LOW _T("HDTV_1080I_TIMINGS_HSYNC_LOW")
#define HDTV_1080I_TIMINGS_HFPORCH _T("HDTV_1080I_TIMINGS_HFPORCH")
#define HDTV_1080I_TIMINGS_HBPORCH _T("HDTV_1080I_TIMINGS_HBPORCH")
#define HDTV_1080I_TIMINGS_VSYNC_HIGH _T("HDTV_1080I_TIMINGS_VSYNC_HIGH")
#define HDTV_1080I_TIMINGS_VSYNC_LOW _T("HDTV_1080I_TIMINGS_VSYNC_LOW")
#define HDTV_1080I_TIMINGS_VFPORCH _T("HDTV_1080I_TIMINGS_VFPORCH")
#define HDTV_1080I_TIMINGS_VBPORCH _T("HDTV_1080I_TIMINGS_VBPORCH")

//tri-level sync test stats
#define HDTV_480P_TRILEVEL_SYNC_MODE2_LEADING _T("HDTV_480P_TRILEVEL_SYNC_MODE2_LEADING")
#define HDTV_480P_TRILEVEL_SYNC_MODE2_TRAILING _T("HDTV_480P_TRILEVEL_SYNC_MODE2_TRAILING")
#define HDTV_720P_TRILEVEL_SYNC_MODE3_LEADING _T("HDTV_720P_TRILEVEL_SYNC_MODE3_LEADING")
#define HDTV_720P_TRILEVEL_SYNC_MODE2_LEADING _T("HDTV_720P_TRILEVEL_SYNC_MODE2_LEADING")
#define HDTV_720P_TRILEVEL_SYNC_MODE2_TRAILING _T("HDTV_720P_TRILEVEL_SYNC_MODE2_TRAILING")
#define HDTV_1080I_TRILEVEL_SYNC_MODE3_LEADING _T("HDTV_1080I_TRILEVEL_SYNC_MODE3_LEADING")
#define HDTV_1080I_TRILEVEL_SYNC_MODE2_LEADING _T("HDTV_1080I_TRILEVEL_SYNC_MODE2_LEADING")
#define HDTV_1080I_TRILEVEL_SYNC_MODE2_TRAILING _T("HDTV_1080I_TRILEVEL_SYNC_MODE2_TRAILING")

//end of XGRAFX HDTV keys
//***********************




#define XAUDIO_ANALOG_DC_OFFSET_PERCENT_LEFT_CHANNEL _T("XAudio Analog DC Offset Percent Left Channel (dB)")
#define XAUDIO_ANALOG_DC_OFFSET_PERCENT_RIGHT_CHANNEL _T("XAudio Analog DC Offset Percent Right Channel (dB)")
#define XAUDIO_SILENCE_DC_OFFSET_PERCENT_LEFT_CHANNEL _T("XAudio Silence DC Offset Percent Left Channel (dB)")
#define XAUDIO_SILENCE_DC_OFFSET_PERCENT_RIGHT_CHANNEL _T("XAudio Silence DC Offset Percent Right Channel (dB)")
#define XAUDIO_DIGITAL_DC_OFFSET_PERCENT_LEFT_CHANNEL _T("XAudio Digital DC Offset Percent Left Channel (dB)")
#define XAUDIO_DIGITAL_DC_OFFSET_PERCENT_RIGHT_CHANNEL _T("XAudio Digital DC Offset Percent Right Channel (dB)")

#define XAUDIO_ANALOG_MAX_FREQUENCY_AMPLITUDE_LEFT_CHANNEL _T("XAudio Analog Maximum Frequency Amplitude Left Channel (dB,Hz)")
#define XAUDIO_ANALOG_MAX_FREQUENCY_AMPLITUDE_RIGHT_CHANNEL _T("XAudio Analog Maximum Frequency Amplitude Right Channel (dB,Hz)")
#define XAUDIO_SILENCE_MAX_FREQUENCY_AMPLITUDE_LEFT_CHANNEL _T("XAudio Silence Maximum Frequency Amplitude Left Channel (dB,Hz)")
#define XAUDIO_SILENCE_MAX_FREQUENCY_AMPLITUDE_RIGHT_CHANNEL _T("XAudio Silence Maximum Frequency Amplitude Right Channel (dB,Hz)")
#define XAUDIO_DIGITAL_MAX_FREQUENCY_AMPLITUDE_LEFT_CHANNEL _T("XAudio Digital Maximum Frequency Amplitude Left Channel (dB,Hz)")
#define XAUDIO_DIGITAL_MAX_FREQUENCY_AMPLITUDE_RIGHT_CHANNEL _T("XAudio Digital Maximum Frequency Amplitude Right Channel (dB,Hz)")

#define XAUDIO_ANALOG_MIN_FREQUENCY_AMPLITUDE_LEFT_CHANNEL _T("XAudio Analog Minimum Frequency Amplitude Left Channel (dB,Hz)")
#define XAUDIO_ANALOG_MIN_FREQUENCY_AMPLITUDE_RIGHT_CHANNEL _T("XAudio Analog Minimum Frequency Amplitude Right Channel (dB,Hz)")
#define XAUDIO_SILENCE_MIN_FREQUENCY_AMPLITUDE_LEFT_CHANNEL _T("XAudio Silence Minimum Frequency Amplitude Left Channel (dB,Hz)")
#define XAUDIO_SILENCE_MIN_FREQUENCY_AMPLITUDE_RIGHT_CHANNEL _T("XAudio Silence Minimum Frequency Amplitude Right Channel (dB,Hz)")
#define XAUDIO_DIGITAL_MIN_FREQUENCY_AMPLITUDE_LEFT_CHANNEL _T("XAudio Digital Minimum Frequency Amplitude Left Channel (dB,Hz)")
#define XAUDIO_DIGITAL_MIN_FREQUENCY_AMPLITUDE_RIGHT_CHANNEL _T("XAudio Digital Minimum Frequency Amplitude Right Channel (dB,Hz)")

#define XAUDIO_ANALOG_MAX_NOISE_LEVEL_LEFT_CHANNEL _T("XAudio Analog Maximum Noise Level Left Channel (dB,Hz)")
#define XAUDIO_ANALOG_MAX_NOISE_LEVEL_RIGHT_CHANNEL _T("XAudio Analog Maximum Noise Level Right Channel (dB,Hz)")
#define XAUDIO_SILENCE_MAX_NOISE_LEVEL_LEFT_CHANNEL _T("XAudio Silence Maximum Noise Level Left Channel (dB,Hz)")
#define XAUDIO_SILENCE_MAX_NOISE_LEVEL_RIGHT_CHANNEL _T("XAudio Silence Maximum Noise Level Right Channel (dB,Hz)")
#define XAUDIO_DIGITAL_MAX_NOISE_LEVEL_LEFT_CHANNEL _T("XAudio Digital Maximum Noise Level Left Channel (dB,Hz)")
#define XAUDIO_DIGITAL_MAX_NOISE_LEVEL_RIGHT_CHANNEL _T("XAudio Digital Maximum Noise Level Right Channel (dB,Hz)")

#define XAUDIO_ANALOG_SIGNAL_TO_NOISE_RATIO_LEFT_CHANNEL _T("XAudio Analog Signal To Noise Ratio Left Channel (dB)")
#define XAUDIO_ANALOG_SIGNAL_TO_NOISE_RATIO_RIGHT_CHANNEL _T("XAudio Analog Signal To Noise Ratio Right Channel (dB)")
#define XAUDIO_SILENCE_SIGNAL_TO_NOISE_RATIO_LEFT_CHANNEL _T("XAudio Silence Signal To Noise Ratio Left Channel (dB)")
#define XAUDIO_SILENCE_SIGNAL_TO_NOISE_RATIO_RIGHT_CHANNEL _T("XAudio Silence Signal To Noise Ratio Right Channel (dB)")
#define XAUDIO_DIGITAL_SIGNAL_TO_NOISE_RATIO_LEFT_CHANNEL _T("XAudio Digital Signal To Noise Ratio Left Channel (dB)")
#define XAUDIO_DIGITAL_SIGNAL_TO_NOISE_RATIO_RIGHT_CHANNEL _T("XAudio Digital Signal To Noise Ratio Right Channel (dB)")

#define XAUDIO_ANALOG_SIGNAL_TO_NOISE_RATIO_IGNORING_CROSSTALK_LEFT_CHANNEL _T("XAudio Analog Signal To Noise Ratio Ignoring Crosstalk Left Channel (dB)")
#define XAUDIO_ANALOG_SIGNAL_TO_NOISE_RATIO_IGNORING_CROSSTALK_RIGHT_CHANNEL _T("XAudio Analog Signal To Noise Ratio Ignoring Crosstalk Right Channel (dB)")
#define XAUDIO_SILENCE_SIGNAL_TO_NOISE_RATIO_IGNORING_CROSSTALK_LEFT_CHANNEL _T("XAudio Silence Signal To Noise Ratio Ignoring Crosstalk Left Channel (dB)")
#define XAUDIO_SILENCE_SIGNAL_TO_NOISE_RATIO_IGNORING_CROSSTALK_RIGHT_CHANNEL _T("XAudio Silence Signal To Noise Ratio Ignoring Crosstalk Right Channel (dB)")
#define XAUDIO_DIGITAL_SIGNAL_TO_NOISE_RATIO_IGNORING_CROSSTALK_LEFT_CHANNEL _T("XAudio Digital Signal To Noise Ratio Ignoring Crosstalk Left Channel (dB)")
#define XAUDIO_DIGITAL_SIGNAL_TO_NOISE_RATIO_IGNORING_CROSSTALK_RIGHT_CHANNEL _T("XAudio Digital Signal To Noise Ratio Ignoring Crosstalk Right Channel (dB)")

#define XAUDIO_ANALOG_CHANNEL_SEPERATION_LEFT_CHANNEL _T("XAudio Analog Channel Seperation Left Channel (dB)")
#define XAUDIO_ANALOG_CHANNEL_SEPERATION_RIGHT_CHANNEL _T("XAudio Analog Channel Seperation Right Channel (dB)")
#define XAUDIO_SILENCE_CHANNEL_SEPERATION_LEFT_CHANNEL _T("XAudio Silence Channel Seperation Left Channel (dB)")
#define XAUDIO_SILENCE_CHANNEL_SEPERATION_RIGHT_CHANNEL _T("XAudio Silence Channel Seperation Right Channel (dB)")
#define XAUDIO_DIGITAL_CHANNEL_SEPERATION_LEFT_CHANNEL _T("XAudio Digital Channel Seperation Left Channel (dB)")
#define XAUDIO_DIGITAL_CHANNEL_SEPERATION_RIGHT_CHANNEL _T("XAudio Digital Channel Seperation Right Channel (dB)")

#define XAUDIO_ANALOG_FREQUENCY_RESPONSE_LEFT_CHANNEL _T("XAudio Analog Frequency Response Left Channel (dB)")
#define XAUDIO_ANALOG_FREQUENCY_RESPONSE_RIGHT_CHANNEL _T("XAudio Analog Frequency Response Right Channel (dB)")
#define XAUDIO_SILENCE_FREQUENCY_RESPONSE_LEFT_CHANNEL _T("XAudio Silence Frequency Response Left Channel (dB)")
#define XAUDIO_SILENCE_FREQUENCY_RESPONSE_RIGHT_CHANNEL _T("XAudio Silence Frequency Response Right Channel (dB)")
#define XAUDIO_DIGITAL_FREQUENCY_RESPONSE_LEFT_CHANNEL _T("XAudio Digital Frequency Response Left Channel (dB)")
#define XAUDIO_DIGITAL_FREQUENCY_RESPONSE_RIGHT_CHANNEL _T("XAudio Digital Frequency Response Right Channel (dB)")

//XTV 
//Linearity Tests
#define NTSCM_COMPOSITE_HORIZONTAL_LINEARITY_MAX_RED _T("NTSC-M Composite Horizontal Linearity Max RED")
#define NTSCM_COMPOSITE_HORIZONTAL_LINEARITY_MAX_GREEN _T("NTSC-M Composite Horizontal Linearity Max GREEN")
#define NTSCM_COMPOSITE_HORIZONTAL_LINEARITY_MAX_BLUE _T("NTSC-M Composite Horizontal Linearity Max BLUE")

#define NTSCM_COMPOSITE_VERTICAL_LINEARITY_MAX_RED _T("NTSC-M Composite Vertical Linearity Max RED")
#define NTSCM_COMPOSITE_VERTICAL_LINEARITY_MAX_GREEN _T("NTSC-M Composite Vertical Linearity Max GREEN")
#define NTSCM_COMPOSITE_VERTICAL_LINEARITY_MAX_BLUE _T("NTSC-M Composite Vertical Linearity Max BLUE")


//NTSC-M Composite Color Bar:
#define NTSCM_COMPOSITE_COLOR_BAR_LUMA_IRE_WHITE _T("Ntsc-m Composite CB Luma IRE White Bar")
#define NTSCM_COMPOSITE_COLOR_BAR_LUMA_IRE_YELLOW _T("Ntsc-m Composite CB Luma IRE Yellow Bar")
#define NTSCM_COMPOSITE_COLOR_BAR_LUMA_IRE_CYAN _T("Ntsc-m Composite CB Luma IRE Cyan Bar")
#define NTSCM_COMPOSITE_COLOR_BAR_LUMA_IRE_GREEN _T("Ntsc-m Composite CB Luma IRE Green Bar")
#define NTSCM_COMPOSITE_COLOR_BAR_LUMA_IRE_MAGENTA _T("Ntsc-m Composite CB Luma IRE Magenta Bar")
#define NTSCM_COMPOSITE_COLOR_BAR_LUMA_IRE_RED _T("Ntsc-m Composite CB Luma IRE Red Bar")
#define NTSCM_COMPOSITE_COLOR_BAR_LUMA_IRE_BLUE _T("Ntsc-m Composite CB Luma IRE Blue Bar")
#define NTSCM_COMPOSITE_COLOR_BAR_LUMA_IRE_BLACK _T("Ntsc-m Composite CB Luma IRE Black Bar")

#define NTSCM_COMPOSITE_COLOR_BAR_CHROMA_IRE_WHITE _T("Ntsc-m Composite CB Chroma IRE White Bar")
#define NTSCM_COMPOSITE_COLOR_BAR_CHROMA_IRE_YELLOW _T("Ntsc-m Composite CB Chroma IRE Yellow Bar")
#define NTSCM_COMPOSITE_COLOR_BAR_CHROMA_IRE_CYAN _T("Ntsc-m Composite CB Chroma IRE Cyan Bar")
#define NTSCM_COMPOSITE_COLOR_BAR_CHROMA_IRE_GREEN _T("Ntsc-m Composite CB Chroma IRE Green Bar")
#define NTSCM_COMPOSITE_COLOR_BAR_CHROMA_IRE_MAGENTA _T("Ntsc-m Composite CB Chroma IRE Magenta Bar")
#define NTSCM_COMPOSITE_COLOR_BAR_CHROMA_IRE_RED _T("Ntsc-m Composite CB Chroma IRE Red Bar")
#define NTSCM_COMPOSITE_COLOR_BAR_CHROMA_IRE_BLUE _T("Ntsc-m Composite CB Chroma IRE Blue Bar")
#define NTSCM_COMPOSITE_COLOR_BAR_CHROMA_IRE_BLACK _T("Ntsc-m Composite CB Chroma IRE Black Bar")


//#define NTSCM_COMPOSITE_COLOR_BAR_CHROMA_PHASE_WHITE _T("Ntsc-m Composite CB Chroma Phase White Bar")
#define NTSCM_COMPOSITE_COLOR_BAR_CHROMA_PHASE_YELLOW _T("Ntsc-m Composite CB Chroma Phase Yellow Bar")
#define NTSCM_COMPOSITE_COLOR_BAR_CHROMA_PHASE_CYAN _T("Ntsc-m Composite CB Chroma Phase Cyan Bar")
#define NTSCM_COMPOSITE_COLOR_BAR_CHROMA_PHASE_GREEN _T("Ntsc-m Composite CB Chroma Phase Green Bar")
#define NTSCM_COMPOSITE_COLOR_BAR_CHROMA_PHASE_MAGENTA _T("Ntsc-m Composite CB Chroma Phase Magenta Bar")
#define NTSCM_COMPOSITE_COLOR_BAR_CHROMA_PHASE_RED _T("Ntsc-m Composite CB Chroma Phase Red Bar")
#define NTSCM_COMPOSITE_COLOR_BAR_CHROMA_PHASE_BLUE _T("Ntsc-m Composite CB Chroma Phase Blue Bar")
//#define NTSCM_COMPOSITE_COLOR_BAR_CHROMA_PHASE_BLACK _T("Ntsc-m Composite CB Chroma Phase Black Bar")


//NTSC-M Svideo Color Bar:

#define NTSCM_SVIDEO_COLOR_BAR_LUMA_IRE_WHITE _T("Ntsc-m Svideo CB Luma IRE White Bar")
#define NTSCM_SVIDEO_COLOR_BAR_LUMA_IRE_YELLOW _T("Ntsc-m Svideo CB Luma IRE Yellow Bar")
#define NTSCM_SVIDEO_COLOR_BAR_LUMA_IRE_CYAN _T("Ntsc-m Svideo CB Luma IRE Cyan Bar")
#define NTSCM_SVIDEO_COLOR_BAR_LUMA_IRE_GREEN _T("Ntsc-m Svideo CB Luma IRE Green Bar")
#define NTSCM_SVIDEO_COLOR_BAR_LUMA_IRE_MAGENTA _T("Ntsc-m Svideo CB Luma IRE Magenta Bar")
#define NTSCM_SVIDEO_COLOR_BAR_LUMA_IRE_RED _T("Ntsc-m Svideo CB Luma IRE Red Bar")
#define NTSCM_SVIDEO_COLOR_BAR_LUMA_IRE_BLUE _T("Ntsc-m Svideo CB Luma IRE Blue Bar")
#define NTSCM_SVIDEO_COLOR_BAR_LUMA_IRE_BLACK _T("Ntsc-m Svideo CB Luma IRE Black Bar")

#define NTSCM_SVIDEO_COLOR_BAR_CHROMA_IRE_WHITE _T("Ntsc-m Svideo CB Chroma IRE White Bar")
#define NTSCM_SVIDEO_COLOR_BAR_CHROMA_IRE_YELLOW _T("Ntsc-m Svideo CB Chroma IRE Yellow Bar")
#define NTSCM_SVIDEO_COLOR_BAR_CHROMA_IRE_CYAN _T("Ntsc-m  Svideo CB Chroma IRE Cyan Bar")
#define NTSCM_SVIDEO_COLOR_BAR_CHROMA_IRE_GREEN _T("Ntsc-m Svideo CB Chroma IRE Green Bar")
#define NTSCM_SVIDEO_COLOR_BAR_CHROMA_IRE_MAGENTA _T("Ntsc-m Svideo CB Chroma IRE Magenta Bar")
#define NTSCM_SVIDEO_COLOR_BAR_CHROMA_IRE_RED _T("Ntsc-m Svideo CB Chroma IRE Red Bar")
#define NTSCM_SVIDEO_COLOR_BAR_CHROMA_IRE_BLUE _T("Ntsc-m Svideo CB Chroma IRE Blue Bar")
#define NTSCM_SVIDEO_COLOR_BAR_CHROMA_IRE_BLACK _T("Ntsc-m Svideo CB Chroma IRE Black Bar")


//#define NTSCM_SVIDEO_COLOR_BAR_CHROMA_PHASE_WHITE _T("Ntsc-m Svideo CB Chroma Phase White Bar")
#define NTSCM_SVIDEO_COLOR_BAR_CHROMA_PHASE_YELLOW _T("Ntsc-m Svideo CB Chroma Phase Yellow Bar")
#define NTSCM_SVIDEO_COLOR_BAR_CHROMA_PHASE_CYAN _T("Ntsc-m Svideo CB Chroma Phase Cyan Bar")
#define NTSCM_SVIDEO_COLOR_BAR_CHROMA_PHASE_GREEN _T("Ntsc-m Svideo CB Chroma Phase Green Bar")
#define NTSCM_SVIDEO_COLOR_BAR_CHROMA_PHASE_MAGENTA _T("Ntsc-m Svideo CB Chroma Phase Magenta Bar")
#define NTSCM_SVIDEO_COLOR_BAR_CHROMA_PHASE_RED _T("Ntsc-m Svideo CB Chroma Phase Red Bar")
#define NTSCM_SVIDEO_COLOR_BAR_CHROMA_PHASE_BLUE _T("Ntsc-m Svideo CB Chroma Phase Blue Bar")
//#define NTSCM_SVIDEO_COLOR_BAR_CHROMA_PHASE_BLACK _T("Ntsc-m  Svideo CB Chroma Phase Black Bar")


//NTSC-M Composite Multiburst
#define NTSCM_COMPOSITE_MULTIBURST_FREQUEN_PACKET1 _T("Ntsc-m Composite Multiburst Frequency Packet #1")
#define NTSCM_COMPOSITE_MULTIBURST_FREQUEN_PACKET2 _T("Ntsc-m Composite Multiburst Frequency Packet #2")
#define NTSCM_COMPOSITE_MULTIBURST_FREQUEN_PACKET3 _T("Ntsc-m Composite Multiburst Frequency Packet #3")
#define NTSCM_COMPOSITE_MULTIBURST_FREQUEN_PACKET4 _T("Ntsc-m Composite Multiburst Frequency Packet #4")
#define NTSCM_COMPOSITE_MULTIBURST_FREQUEN_PACKET5 _T("Ntsc-m Composite Multiburst Frequency Packet #5")
#define NTSCM_COMPOSITE_MULTIBURST_FREQUEN_PACKET6 _T("Ntsc-m Composite Multiburst Frequency Packet #6")


//NTSC-M Svideo Multiburst
#define NTSCM_SVIDEO_MULTIBURST_FREQUEN_PACKET1 _T("Ntsc-m Svideo Multiburst Frequency Packet #1")
#define NTSCM_SVIDEO_MULTIBURST_FREQUEN_PACKET2 _T("Ntsc-m Svideo Multiburst Frequency Packet #2")
#define NTSCM_SVIDEO_MULTIBURST_FREQUEN_PACKET3 _T("Ntsc-m Svideo Multiburst Frequency Packet #3")
#define NTSCM_SVIDEO_MULTIBURST_FREQUEN_PACKET4 _T("Ntsc-m Svideo Multiburst Frequency Packet #4")
#define NTSCM_SVIDEO_MULTIBURST_FREQUEN_PACKET5 _T("Ntsc-m Svideo Multiburst Frequency Packet #5")
#define NTSCM_SVIDEO_MULTIBURST_FREQUEN_PACKET6 _T("Ntsc-m Svideo Multiburst Frequency Packet #6")




//NTSC-Japan Composite Color Bar:
#define NTSCJ_COMPOSITE_COLOR_BAR_LUMA_IRE_WHITE _T("Ntsc-Japan Composite CB Luma IRE White Bar")
#define NTSCJ_COMPOSITE_COLOR_BAR_LUMA_IRE_YELLOW _T("Ntsc-Japan Composite CB Luma IRE Yellow Bar")
#define NTSCJ_COMPOSITE_COLOR_BAR_LUMA_IRE_CYAN _T("Ntsc-Japan Composite CB Luma IRE Cyan Bar")
#define NTSCJ_COMPOSITE_COLOR_BAR_LUMA_IRE_GREEN _T("Ntsc-Japan Composite CB Luma IRE Green Bar")
#define NTSCJ_COMPOSITE_COLOR_BAR_LUMA_IRE_MAGENTA _T("Ntsc-Japan Composite CB Luma IRE Magenta Bar")
#define NTSCJ_COMPOSITE_COLOR_BAR_LUMA_IRE_RED _T("Ntsc-Japan Composite CB Luma IRE Red Bar")
#define NTSCJ_COMPOSITE_COLOR_BAR_LUMA_IRE_BLUE _T("Ntsc-Japan Composite CB Luma IRE Blue Bar")
#define NTSCJ_COMPOSITE_COLOR_BAR_LUMA_IRE_BLACK _T("Ntsc-Japan Composite CB Luma IRE Black Bar")

#define NTSCJ_COMPOSITE_COLOR_BAR_CHROMA_IRE_WHITE _T("Ntsc-Japan Composite CB Chroma IRE White Bar")
#define NTSCJ_COMPOSITE_COLOR_BAR_CHROMA_IRE_YELLOW _T("Ntsc-Japan Composite CB Chroma IRE Yellow Bar")
#define NTSCJ_COMPOSITE_COLOR_BAR_CHROMA_IRE_CYAN _T("Ntsc-Japan Composite CB Chroma IRE Cyan Bar")
#define NTSCJ_COMPOSITE_COLOR_BAR_CHROMA_IRE_GREEN _T("Ntsc-Japan Composite CB Chroma IRE Green Bar")
#define NTSCJ_COMPOSITE_COLOR_BAR_CHROMA_IRE_MAGENTA _T("Ntsc-Japan Composite CB Chroma IRE Magenta Bar")
#define NTSCJ_COMPOSITE_COLOR_BAR_CHROMA_IRE_RED _T("Ntsc-Japan Composite CB Chroma IRE Red Bar")
#define NTSCJ_COMPOSITE_COLOR_BAR_CHROMA_IRE_BLUE _T("Ntsc-Japan Composite CB Chroma IRE Blue Bar")
#define NTSCJ_COMPOSITE_COLOR_BAR_CHROMA_IRE_BLACK _T("Ntsc-Japan Composite CB Chroma IRE Black Bar")


//#define NTSCJ_COMPOSITE_COLOR_BAR_CHROMA_PHASE_WHITE _T("Ntsc-m Composite CB Chroma Phase White Bar")
#define NTSCJ_COMPOSITE_COLOR_BAR_CHROMA_PHASE_YELLOW _T("Ntsc-Japan Composite CB Chroma Phase Yellow Bar")
#define NTSCJ_COMPOSITE_COLOR_BAR_CHROMA_PHASE_CYAN _T("Ntsc-Japan Composite CB Chroma Phase Cyan Bar")
#define NTSCJ_COMPOSITE_COLOR_BAR_CHROMA_PHASE_GREEN _T("Ntsc-Japan Composite CB Chroma Phase Green Bar")
#define NTSCJ_COMPOSITE_COLOR_BAR_CHROMA_PHASE_MAGENTA _T("Ntsc-Japan Composite CB Chroma Phase Magenta Bar")
#define NTSCJ_COMPOSITE_COLOR_BAR_CHROMA_PHASE_RED _T("Ntsc-Japan Composite CB Chroma Phase Red Bar")
#define NTSCJ_COMPOSITE_COLOR_BAR_CHROMA_PHASE_BLUE _T("Ntsc-Japan Composite CB Chroma Phase Blue Bar")
//#define NTSCJ_COMPOSITE_COLOR_BAR_CHROMA_PHASE_BLACK _T("Ntsc-m Composite CB Chroma Phase Black Bar")

//NTSC-Japan Svideo Color Bar:

#define NTSCJ_SVIDEO_COLOR_BAR_LUMA_IRE_WHITE _T("Ntsc-Japan Svideo CB Luma IRE White Bar")
#define NTSCJ_SVIDEO_COLOR_BAR_LUMA_IRE_YELLOW _T("Ntsc-Japan Svideo CB Luma IRE Yellow Bar")
#define NTSCJ_SVIDEO_COLOR_BAR_LUMA_IRE_CYAN _T("Ntsc-Japan Svideo CB Luma IRE Cyan Bar")
#define NTSCJ_SVIDEO_COLOR_BAR_LUMA_IRE_GREEN _T("Ntsc-Japan Svideo CB Luma IRE Green Bar")
#define NTSCJ_SVIDEO_COLOR_BAR_LUMA_IRE_MAGENTA _T("Ntsc-Japan Svideo CB Luma IRE Magenta Bar")
#define NTSCJ_SVIDEO_COLOR_BAR_LUMA_IRE_RED _T("Ntsc-Japan Svideo CB Luma IRE Red Bar")
#define NTSCJ_SVIDEO_COLOR_BAR_LUMA_IRE_BLUE _T("Ntsc-Japan Svideo CB Luma IRE Blue Bar")
#define NTSCJ_SVIDEO_COLOR_BAR_LUMA_IRE_BLACK _T("Ntsc-Japan Svideo CB Luma IRE Black Bar")

#define NTSCJ_SVIDEO_COLOR_BAR_CHROMA_IRE_WHITE _T("Ntsc-Japan Svideo CB Chroma IRE White Bar")
#define NTSCJ_SVIDEO_COLOR_BAR_CHROMA_IRE_YELLOW _T("Ntsc-Japan Svideo CB Chroma IRE Yellow Bar")
#define NTSCJ_SVIDEO_COLOR_BAR_CHROMA_IRE_CYAN _T("Ntsc-Japan  Svideo CB Chroma IRE Cyan Bar")
#define NTSCJ_SVIDEO_COLOR_BAR_CHROMA_IRE_GREEN _T("Ntsc-Japan Svideo CB Chroma IRE Green Bar")
#define NTSCJ_SVIDEO_COLOR_BAR_CHROMA_IRE_MAGENTA _T("Ntsc-Japan Svideo CB Chroma IRE Magenta Bar")
#define NTSCJ_SVIDEO_COLOR_BAR_CHROMA_IRE_RED _T("Ntsc-Japan Svideo CB Chroma IRE Red Bar")
#define NTSCJ_SVIDEO_COLOR_BAR_CHROMA_IRE_BLUE _T("Ntsc-Japan Svideo CB Chroma IRE Blue Bar")
#define NTSCJ_SVIDEO_COLOR_BAR_CHROMA_IRE_BLACK _T("Ntsc-Japan Svideo CB Chroma IRE Black Bar")


//#define NTSCJ_SVIDEO_COLOR_BAR_CHROMA_PHASE_WHITE _T("Ntsc-m Svideo CB Chroma Phase White Bar")
#define NTSCJ_SVIDEO_COLOR_BAR_CHROMA_PHASE_YELLOW _T("Ntsc-Japan Svideo CB Chroma Phase Yellow Bar")
#define NTSCJ_SVIDEO_COLOR_BAR_CHROMA_PHASE_CYAN _T("Ntsc-Japan Svideo CB Chroma Phase Cyan Bar")
#define NTSCJ_SVIDEO_COLOR_BAR_CHROMA_PHASE_GREEN _T("Ntsc-Japan Svideo CB Chroma Phase Green Bar")
#define NTSCJ_SVIDEO_COLOR_BAR_CHROMA_PHASE_MAGENTA _T("Ntsc-Japan Svideo CB Chroma Phase Magenta Bar")
#define NTSCJ_SVIDEO_COLOR_BAR_CHROMA_PHASE_RED _T("Ntsc-Japan Svideo CB Chroma Phase Red Bar")
#define NTSCJ_SVIDEO_COLOR_BAR_CHROMA_PHASE_BLUE _T("Ntsc-Japan Svideo CB Chroma Phase Blue Bar")
//#define NTSCJ_SVIDEO_COLOR_BAR_CHROMA_PHASE_BLACK _T("Ntsc-m  Svideo CB Chroma Phase Black Bar")

//NTSC-Japan Composite Multiburst
#define NTSCJ_COMPOSITE_MULTIBURST_FREQUEN_PACKET1 _T("Ntsc-Japan Composite Multiburst Frequency Packet #1")
#define NTSCJ_COMPOSITE_MULTIBURST_FREQUEN_PACKET2 _T("Ntsc-Japan Composite Multiburst Frequency Packet #2")
#define NTSCJ_COMPOSITE_MULTIBURST_FREQUEN_PACKET3 _T("Ntsc-Japan Composite Multiburst Frequency Packet #3")
#define NTSCJ_COMPOSITE_MULTIBURST_FREQUEN_PACKET4 _T("Ntsc-Japan Composite Multiburst Frequency Packet #4")
#define NTSCJ_COMPOSITE_MULTIBURST_FREQUEN_PACKET5 _T("Ntsc-Japan Composite Multiburst Frequency Packet #5")
#define NTSCJ_COMPOSITE_MULTIBURST_FREQUEN_PACKET6 _T("Ntsc-Japan Composite Multiburst Frequency Packet #6")


//NTSC-Japan Svideo Multiburst
#define NTSCJ_SVIDEO_MULTIBURST_FREQUEN_PACKET1 _T("Ntsc-Japan Svideo Multiburst Frequency Packet #1")
#define NTSCJ_SVIDEO_MULTIBURST_FREQUEN_PACKET2 _T("Ntsc-Japan Svideo Multiburst Frequency Packet #2")
#define NTSCJ_SVIDEO_MULTIBURST_FREQUEN_PACKET3 _T("Ntsc-Japan Svideo Multiburst Frequency Packet #3")
#define NTSCJ_SVIDEO_MULTIBURST_FREQUEN_PACKET4 _T("Ntsc-Japan Svideo Multiburst Frequency Packet #4")
#define NTSCJ_SVIDEO_MULTIBURST_FREQUEN_PACKET5 _T("Ntsc-Japan Svideo Multiburst Frequency Packet #5")
#define NTSCJ_SVIDEO_MULTIBURST_FREQUEN_PACKET6 _T("Ntsc-Japan Svideo Multiburst Frequency Packet #6")





//PAL-BDGHI Composite Color Bar for Luma IRE:
#define PAL_BDGHI_COMPOSITE_COLOR_BAR_LUMA_IRE_WHITE _T("Pal-bdghi Composite CB Luma IRE White Bar")
#define PAL_BDGHI_COMPOSITE_COLOR_BAR_LUMA_IRE_YELLOW _T("Pal-bdghi Composite CB Luma IRE Yellow Bar")
#define PAL_BDGHI_COMPOSITE_COLOR_BAR_LUMA_IRE_CYAN _T("Pal-bdghi Composite CB Luma IRE Cyan Bar")
#define PAL_BDGHI_COMPOSITE_COLOR_BAR_LUMA_IRE_GREEN _T("Pal-bdghi Composite CB Luma IRE Green Bar")
#define PAL_BDGHI_COMPOSITE_COLOR_BAR_LUMA_IRE_MAGENTA _T("Pal-bdghi Composite CB Luma IRE Magenta Bar")
#define PAL_BDGHI_COMPOSITE_COLOR_BAR_LUMA_IRE_RED _T("Pal-bdghi Composite CB Luma IRE Red Bar")
#define PAL_BDGHI_COMPOSITE_COLOR_BAR_LUMA_IRE_BLUE _T("Pal-bdghi Composite CB Luma IRE Blue Bar")
#define PAL_BDGHI_COMPOSITE_COLOR_BAR_LUMA_IRE_BLACK _T("Pal-bdghi Composite CB Luma IRE Balck Bar")
// PAL-BDGHI Composite Color Bar for Chroma IRE:
#define PAL_BDGHI_COMPOSITE_COLOR_BAR_CHROMA_IRE_WHITE _T("Pal-bdghi Composite CB Chroma IRE White Bar")
#define PAL_BDGHI_COMPOSITE_COLOR_BAR_CHROMA_IRE_YELLOW _T("Pal-bdghi Composite CB Chroma IRE Yellow Bar")
#define PAL_BDGHI_COMPOSITE_COLOR_BAR_CHROMA_IRE_CYAN _T("Pal-bdghi Composite CB Chroma IRE Cyan Bar")
#define PAL_BDGHI_COMPOSITE_COLOR_BAR_CHROMA_IRE_GREEN _T("Pal-bdghi Composite CB Chroma IRE Green Bar")
#define PAL_BDGHI_COMPOSITE_COLOR_BAR_CHROMA_IRE_MAGENTA _T("Pal-bdghi Composite CB Chroma IRE Magenta Bar")
#define PAL_BDGHI_COMPOSITE_COLOR_BAR_CHROMA_IRE_RED _T("Pal-bdghi Composite CB Chroma IRE Red Bar")
#define PAL_BDGHI_COMPOSITE_COLOR_BAR_CHROMA_IRE_BLUE _T("Pal-bdghi Composite CB Chroma IRE Blue Bar")
#define PAL_BDGHI_COMPOSITE_COLOR_BAR_CHROMA_IRE_BLACK _T("Pal-bdghi Composite CB Chroma IRE Balck Bar")
// PAL-BDGHI Composite Color Bar for Chroma Phase
//#define PAL_BDGHI_COMPOSITE_COLOR_BAR_CHROMA_PHASE_WHITE _T("Pal-bdghi Composite CB Chroma Phase White Bar")
#define PAL_BDGHI_COMPOSITE_COLOR_BAR_CHROMA_PHASE_YELLOW _T("Pal-bdghi Composite CB Chroma Phase Yellow Bar")
#define PAL_BDGHI_COMPOSITE_COLOR_BAR_CHROMA_PHASE_CYAN _T("Pal-bdghi Composite CB Chroma Phase Cyan Bar")
#define PAL_BDGHI_COMPOSITE_COLOR_BAR_CHROMA_PHASE_GREEN _T("Pal-bdghi Composite CB Chroma Phase Green Bar")
#define PAL_BDGHI_COMPOSITE_COLOR_BAR_CHROMA_PHASE_MAGENTA _T("Pal-bdghi Composite CB Chroma Phase Magenta Bar")
#define PAL_BDGHI_COMPOSITE_COLOR_BAR_CHROMA_PHASE_RED _T("Pal-bdghi Composite CB Chroma Phase Red Bar")
#define PAL_BDGHI_COMPOSITE_COLOR_BAR_CHROMA_PHASE_BLUE _T("Pal-bdghi Composite CB Chroma Phase Blue Bar")
//#define PAL_BDGHI_COMPOSITE_COLOR_BAR_CHROMA_PHASE_BLACK _T("Pal-bdghi Composite CB Chroma Phase Balck Bar")


//PAL-BDGHI Svideo Color Bar for Luma IRE:
#define PAL_BDGHI_SVIDEO_COLOR_BAR_LUMA_IRE_WHITE _T("Pal-bdghi Svideo CB Luma IRE White Bar")
#define PAL_BDGHI_SVIDEO_COLOR_BAR_LUMA_IRE_YELLOW _T("Pal-bdghi Svideo CB Luma IRE Yellow Bar")
#define PAL_BDGHI_SVIDEO_COLOR_BAR_LUMA_IRE_CYAN _T("Pal-bdghi Svideo CB Luma IRE Cyan Bar")
#define PAL_BDGHI_SVIDEO_COLOR_BAR_LUMA_IRE_GREEN _T("Pal-bdghi Svideo CB Luma IRE Green Bar")
#define PAL_BDGHI_SVIDEO_COLOR_BAR_LUMA_IRE_MAGENTA _T("Pal-bdghi Svideo CB Luma IRE Magenta Bar")
#define PAL_BDGHI_SVIDEO_COLOR_BAR_LUMA_IRE_RED _T("Pal-bdghi Svideo CB Luma IRE Red Bar")
#define PAL_BDGHI_SVIDEO_COLOR_BAR_LUMA_IRE_BLUE _T("Pal-bdghi Svideo CB Luma IRE Blue Bar")
#define PAL_BDGHI_SVIDEO_COLOR_BAR_LUMA_IRE_BLACK _T("Pal-bdghi Svideo CB Luma IRE Balck Bar")
// PAL-BDGHI Svideo Color Bar for Chroma IRE:
#define PAL_BDGHI_SVIDEO_COLOR_BAR_CHROMA_IRE_WHITE _T("Pal-bdghi Svideo CB Chroma IRE White Bar")
#define PAL_BDGHI_SVIDEO_COLOR_BAR_CHROMA_IRE_YELLOW _T("Pal-bdghi Svideo CB Chroma IRE Yellow Bar")
#define PAL_BDGHI_SVIDEO_COLOR_BAR_CHROMA_IRE_CYAN _T("Pal-bdghi Svideo CB Chroma IRE Cyan Bar")
#define PAL_BDGHI_SVIDEO_COLOR_BAR_CHROMA_IRE_GREEN _T("Pal-bdghi Svideo CB Chroma IRE Green Bar")
#define PAL_BDGHI_SVIDEO_COLOR_BAR_CHROMA_IRE_MAGENTA _T("Pal-bdghi Svideo CB Chroma IRE Magenta Bar")
#define PAL_BDGHI_SVIDEO_COLOR_BAR_CHROMA_IRE_RED _T("Pal-bdghi Svideo CB Chroma IRE Red Bar")
#define PAL_BDGHI_SVIDEO_COLOR_BAR_CHROMA_IRE_BLUE _T("Pal-bdghi Svideo CB Chroma IRE Blue Bar")
#define PAL_BDGHI_SVIDEO_COLOR_BAR_CHROMA_IRE_BLACK _T("Pal-bdghi Svideo CB Chroma IRE Balck Bar")
// PAL-BDGHI Svideo Color Bar for Chroma Phase
//#define PAL_BDGHI_COMPOSITE_COLOR_BAR_CHROMA_PHASE_WHITE _T("Pal-bdghi Composite CB Chroma Phase White Bar")
#define PAL_BDGHI_SVIDEO_COLOR_BAR_CHROMA_PHASE_YELLOW _T("Pal-bdghi Svideo CB Chroma Phase Yellow Bar")
#define PAL_BDGHI_SVIDEO_COLOR_BAR_CHROMA_PHASE_CYAN _T("Pal-bdghi Svideo CB Chroma Phase Cyan Bar")
#define PAL_BDGHI_SVIDEO_COLOR_BAR_CHROMA_PHASE_GREEN _T("Pal-bdghi Svideo CB Chroma Phase Green Bar")
#define PAL_BDGHI_SVIDEO_COLOR_BAR_CHROMA_PHASE_MAGENTA _T("Pal-bdghi Svideo CB Chroma Phase Magenta Bar")
#define PAL_BDGHI_SVIDEO_COLOR_BAR_CHROMA_PHASE_RED _T("Pal-bdghi Svideo CB Chroma Phase Red Bar")
#define PAL_BDGHI_SVIDEO_COLOR_BAR_CHROMA_PHASE_BLUE _T("Pal-bdghi Svideo CB Chroma Phase Blue Bar")
//#define PAL_BDGHI_COMPOSITE_COLOR_BAR_CHROMA_PHASE_BLACK _T("Pal-bdghi Composite CB Chroma Phase Balck Bar")


//PAL-BDGHI Composite Multiburst
#define PAL_BDGHI_COMPOSITE_MULTIBURST_FREQUEN_PACKET1 _T("pal-bdghi Composite Multiburst Frequency Packet #1")
#define PAL_BDGHI_COMPOSITE_MULTIBURST_FREQUEN_PACKET2 _T("pal-bdghi Composite Multiburst Frequency Packet #2")
#define PAL_BDGHI_COMPOSITE_MULTIBURST_FREQUEN_PACKET3 _T("pal-bdghi Composite Multiburst Frequency Packet #3")
#define PAL_BDGHI_COMPOSITE_MULTIBURST_FREQUEN_PACKET4 _T("pal-bdghi Composite Multiburst Frequency Packet #4")
#define PAL_BDGHI_COMPOSITE_MULTIBURST_FREQUEN_PACKET5 _T("pal-bdghi Composite Multiburst Frequency Packet #5")
#define PAL_BDGHI_COMPOSITE_MULTIBURST_FREQUEN_PACKET6 _T("pal-bdghi Composite Multiburst Frequency Packet #6")


//PAL-BDGHI Svideo Multiburst
#define PAL_BDGHI_SVIDEO_MULTIBURST_FREQUEN_PACKET1 _T("pal-bdghi Svideo Multiburst Frequency Packet #1")
#define PAL_BDGHI_SVIDEO_MULTIBURST_FREQUEN_PACKET2 _T("pal-bdghi Svideo Multiburst Frequency Packet #2")
#define PAL_BDGHI_SVIDEO_MULTIBURST_FREQUEN_PACKET3 _T("pal-bdghi Svideo Multiburst Frequency Packet #3")
#define PAL_BDGHI_SVIDEO_MULTIBURST_FREQUEN_PACKET4 _T("pal-bdghi Svideo Multiburst Frequency Packet #4")
#define PAL_BDGHI_SVIDEO_MULTIBURST_FREQUEN_PACKET5 _T("pal-bdghi Svideo Multiburst Frequency Packet #5")
#define PAL_BDGHI_SVIDEO_MULTIBURST_FREQUEN_PACKET6 _T("pal-bdghi Svideo Multiburst Frequency Packet #6")





//PAL-M Composite Color Bar for Luma IRE:
#define PAL_M_COMPOSITE_COLOR_BAR_LUMA_IRE_WHITE _T("Pal-m Composite CB Luma IRE White Bar")
#define PAL_M_COMPOSITE_COLOR_BAR_LUMA_IRE_YELLOW _T("Pal-m Composite CB Luma IRE Yellow Bar")
#define PAL_M_COMPOSITE_COLOR_BAR_LUMA_IRE_CYAN _T("Pal-m Composite CB Luma IRE Cyan Bar")
#define PAL_M_COMPOSITE_COLOR_BAR_LUMA_IRE_GREEN _T("Pal-m Composite CB Luma IRE Green Bar")
#define PAL_M_COMPOSITE_COLOR_BAR_LUMA_IRE_MAGENTA _T("Pal-m Composite CB Luma IRE Magenta Bar")
#define PAL_M_COMPOSITE_COLOR_BAR_LUMA_IRE_RED _T("Pal-m Composite CB Luma IRE Red Bar")
#define PAL_M_COMPOSITE_COLOR_BAR_LUMA_IRE_BLUE _T("Pal-m Composite CB Luma IRE Blue Bar")
#define PAL_M_COMPOSITE_COLOR_BAR_LUMA_IRE_BLACK _T("Pal-m Composite CB Luma IRE Balck Bar")
// PAL-M Composite Color Bar for Chroma IRE:
#define PAL_M_COMPOSITE_COLOR_BAR_CHROMA_IRE_WHITE _T("Pal-m Composite CB Chroma IRE White Bar")
#define PAL_M_COMPOSITE_COLOR_BAR_CHROMA_IRE_YELLOW _T("Pal-m Composite CB Chroma IRE Yellow Bar")
#define PAL_M_COMPOSITE_COLOR_BAR_CHROMA_IRE_CYAN _T("Pal-m Composite CB Chroma IRE Cyan Bar")
#define PAL_M_COMPOSITE_COLOR_BAR_CHROMA_IRE_GREEN _T("Pal-m Composite CB Chroma IRE Green Bar")
#define PAL_M_COMPOSITE_COLOR_BAR_CHROMA_IRE_MAGENTA _T("Pal-m Composite CB Chroma IRE Magenta Bar")
#define PAL_M_COMPOSITE_COLOR_BAR_CHROMA_IRE_RED _T("Pal-m Composite CB Chroma IRE Red Bar")
#define PAL_M_COMPOSITE_COLOR_BAR_CHROMA_IRE_BLUE _T("Pal-m Composite CB Chroma IRE Blue Bar")
#define PAL_M_COMPOSITE_COLOR_BAR_CHROMA_IRE_BLACK _T("Pal-m Composite CB Chroma IRE Balck Bar")
// PAL-M Composite Color Bar for Chroma Phase
//#define PAL_BDGHI_COMPOSITE_COLOR_BAR_CHROMA_PHASE_WHITE _T("Pal-bdghi Composite CB Chroma Phase White Bar")
#define PAL_M_COMPOSITE_COLOR_BAR_CHROMA_PHASE_YELLOW _T("Pal-m Composite CB Chroma Phase Yellow Bar")
#define PAL_M_COMPOSITE_COLOR_BAR_CHROMA_PHASE_CYAN _T("Pal-m Composite CB Chroma Phase Cyan Bar")
#define PAL_M_COMPOSITE_COLOR_BAR_CHROMA_PHASE_GREEN _T("Pal-m Composite CB Chroma Phase Green Bar")
#define PAL_M_COMPOSITE_COLOR_BAR_CHROMA_PHASE_MAGENTA _T("Pal-m Composite CB Chroma Phase Magenta Bar")
#define PAL_M_COMPOSITE_COLOR_BAR_CHROMA_PHASE_RED _T("Pal-m Composite CB Chroma Phase Red Bar")
#define PAL_M_COMPOSITE_COLOR_BAR_CHROMA_PHASE_BLUE _T("Pal-m Composite CB Chroma Phase Blue Bar")
//#define PAL_BDGHI_COMPOSITE_COLOR_BAR_CHROMA_PHASE_BLACK _T("Pal-bdghi Composite CB Chroma Phase Balck Bar")


//PAL-M Svideo Color Bar for Luma IRE:
#define PAL_M_SVIDEO_COLOR_BAR_LUMA_IRE_WHITE _T("Pal-m Svideo CB Luma IRE White Bar")
#define PAL_M_SVIDEO_COLOR_BAR_LUMA_IRE_YELLOW _T("Pal-m Svideo CB Luma IRE Yellow Bar")
#define PAL_M_SVIDEO_COLOR_BAR_LUMA_IRE_CYAN _T("Pal-m Svideo CB Luma IRE Cyan Bar")
#define PAL_M_SVIDEO_COLOR_BAR_LUMA_IRE_GREEN _T("Pal-m Svideo CB Luma IRE Green Bar")
#define PAL_M_SVIDEO_COLOR_BAR_LUMA_IRE_MAGENTA _T("Pal-m Svideo CB Luma IRE Magenta Bar")
#define PAL_M_SVIDEO_COLOR_BAR_LUMA_IRE_RED _T("Pal-m Svideo CB Luma IRE Red Bar")
#define PAL_M_SVIDEO_COLOR_BAR_LUMA_IRE_BLUE _T("Pal-m Svideo CB Luma IRE Blue Bar")
#define PAL_M_SVIDEO_COLOR_BAR_LUMA_IRE_BLACK _T("Pal-m Svideo CB Luma IRE Balck Bar")
// PAL-M Svideo Color Bar for Chroma IRE:
#define PAL_M_SVIDEO_COLOR_BAR_CHROMA_IRE_WHITE _T("Pal-m Svideo CB Chroma IRE White Bar")
#define PAL_M_SVIDEO_COLOR_BAR_CHROMA_IRE_YELLOW _T("Pal-m Svideo CB Chroma IRE Yellow Bar")
#define PAL_M_SVIDEO_COLOR_BAR_CHROMA_IRE_CYAN _T("Pal-m Svideo CB Chroma IRE Cyan Bar")
#define PAL_M_SVIDEO_COLOR_BAR_CHROMA_IRE_GREEN _T("Pal-m Svideo CB Chroma IRE Green Bar")
#define PAL_M_SVIDEO_COLOR_BAR_CHROMA_IRE_MAGENTA _T("Pal-m Svideo CB Chroma IRE Magenta Bar")
#define PAL_M_SVIDEO_COLOR_BAR_CHROMA_IRE_RED _T("Pal-m Svideo CB Chroma IRE Red Bar")
#define PAL_M_SVIDEO_COLOR_BAR_CHROMA_IRE_BLUE _T("Pal-m Svideo CB Chroma IRE Blue Bar")
#define PAL_M_SVIDEO_COLOR_BAR_CHROMA_IRE_BLACK _T("Pal-m Svideo CB Chroma IRE Balck Bar")
// PAL-M Svideo Color Bar for Chroma Phase
//#define PAL_M_COMPOSITE_COLOR_BAR_CHROMA_PHASE_WHITE _T("Pal-bdghi Composite CB Chroma Phase White Bar")
#define PAL_M_SVIDEO_COLOR_BAR_CHROMA_PHASE_YELLOW _T("Pal-m Svideo CB Chroma Phase Yellow Bar")
#define PAL_M_SVIDEO_COLOR_BAR_CHROMA_PHASE_CYAN _T("Pal-m Svideo CB Chroma Phase Cyan Bar")
#define PAL_M_SVIDEO_COLOR_BAR_CHROMA_PHASE_GREEN _T("Pal-m Svideo CB Chroma Phase Green Bar")
#define PAL_M_SVIDEO_COLOR_BAR_CHROMA_PHASE_MAGENTA _T("Pal-m Svideo CB Chroma Phase Magenta Bar")
#define PAL_M_SVIDEO_COLOR_BAR_CHROMA_PHASE_RED _T("Pal-m Svideo CB Chroma Phase Red Bar")
#define PAL_M_SVIDEO_COLOR_BAR_CHROMA_PHASE_BLUE _T("Pal-m Svideo CB Chroma Phase Blue Bar")
//#define PAL_M_COMPOSITE_COLOR_BAR_CHROMA_PHASE_BLACK _T("Pal-bdghi Composite CB Chroma Phase Balck Bar")


//PAL-M Composite Multiburst
#define PAL_M_COMPOSITE_MULTIBURST_FREQUEN_PACKET1 _T("pal-m Composite Multiburst Frequency Packet #1")
#define PAL_M_COMPOSITE_MULTIBURST_FREQUEN_PACKET2 _T("pal-m Composite Multiburst Frequency Packet #2")
#define PAL_M_COMPOSITE_MULTIBURST_FREQUEN_PACKET3 _T("pal-m Composite Multiburst Frequency Packet #3")
#define PAL_M_COMPOSITE_MULTIBURST_FREQUEN_PACKET4 _T("pal-m Composite Multiburst Frequency Packet #4")
#define PAL_M_COMPOSITE_MULTIBURST_FREQUEN_PACKET5 _T("pal-m Composite Multiburst Frequency Packet #5")
#define PAL_M_COMPOSITE_MULTIBURST_FREQUEN_PACKET6 _T("pal-m Composite Multiburst Frequency Packet #6")


//PAL-M Svideo Multiburst
#define PAL_M_SVIDEO_MULTIBURST_FREQUEN_PACKET1 _T("pal-m Svideo Multiburst Frequency Packet #1")
#define PAL_M_SVIDEO_MULTIBURST_FREQUEN_PACKET2 _T("pal-m Svideo Multiburst Frequency Packet #2")
#define PAL_M_SVIDEO_MULTIBURST_FREQUEN_PACKET3 _T("pal-m Svideo Multiburst Frequency Packet #3")
#define PAL_M_SVIDEO_MULTIBURST_FREQUEN_PACKET4 _T("pal-m Svideo Multiburst Frequency Packet #4")
#define PAL_M_SVIDEO_MULTIBURST_FREQUEN_PACKET5 _T("pal-m Svideo Multiburst Frequency Packet #5")
#define PAL_M_SVIDEO_MULTIBURST_FREQUEN_PACKET6 _T("pal-m Svideo Multiburst Frequency Packet #6")


// SMC temperature stats
#define CPU_JUNCTION_TEMP _T("CPU Junction Temp")
#define INTERNAL_AIR_TEMP _T("Internal Air Temp")

// Chipset revision stats
#define GPU_REVISION _T("GPU Revision")
#define MCP_REVISION _T("MCP Revision")

#define DVD_FIRMWARE_REVISION _T("DVD Firmware Revision")
#define HD_TEST_TIME _T("Hard Drive test time (seconds)")
#define BOOT_COUNT _T("Boot Count")


#endif
