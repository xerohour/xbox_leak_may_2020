/**************************************************************
 * File:	OEMSupport.h
 * Descr:	Has definitions for any OEM specific features
 *		    such as NT4.0 power management for gateway etc..
 **************************************************************/


//
// Following are the definitions and escape call to support NT4.0 power management for Gateway.
// Escape call made by NVSVC to set the power state.
// Input: ULONG:    Indicating the "PowerEvent". 
//                  For nVidia, this can be either POWER_EVENT_SUSPEND or POWER_EVENT_RESUME_NORMAL.
// Output: None: The call is always assumed to pass.
//
#define NV_ESC_GW_NT40_PM  0x7012

//
// possible values for 'PowerEvent' above
//
#define	POWER_EVENT_QUERY_SUSPEND	0x01
#define	POWER_EVENT_SUSPEND		0x02
#define	POWER_EVENT_RESUME_CRITICAL	0x03
#define	POWER_EVENT_RESUME_NORMAL	0x03

//
// The registry entry "APMSupport" will control if the NT4.0 APM support features are enabled for 
// a specific OEM. Possible values are given below
//
#define APM_SUPPORT_REGENTRY "APMSupport"
#define APM_SUPPORT_NONE    0
#define APM_SUPPORT_GATEWAY 1
#define APM_SUPPORT_IBM     2   




