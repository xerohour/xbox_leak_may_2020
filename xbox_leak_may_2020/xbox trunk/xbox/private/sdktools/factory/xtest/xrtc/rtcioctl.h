#ifndef _RTCIOCTL_H

#define _RTCIOCTL_H

/*#ifdef _RTC_DRIVER
#include <devioctl.h>
#else
#include <winioctl.h>
#endif
*/
//
// Driver version number - must be consistent with the driver version resource
//
#define RTC_DRIVER_VERSION_STR		"version_number=1.03"
#define RTC_DRIVER_VERSION_NUM		0x103

//
// IOCTL's
//
#define IOCTL_RTC_WRITE_RAM \
	CTL_CODE(FILE_DEVICE_UNKNOWN,0x801,METHOD_BUFFERED,FILE_ANY_ACCESS)

#define IOCTL_RTC_READ_RAM \
	CTL_CODE(FILE_DEVICE_UNKNOWN,0x802,METHOD_BUFFERED,FILE_ANY_ACCESS)

#define IOCTL_RTC_ATOMIC_WRITE_READ_RAM \
	CTL_CODE(FILE_DEVICE_UNKNOWN,0x803,METHOD_BUFFERED,FILE_ANY_ACCESS)

#define IOCTL_RTC_SET_DATE_AND_TIME \
	CTL_CODE(FILE_DEVICE_UNKNOWN,0x804,METHOD_BUFFERED,FILE_ANY_ACCESS)

#define IOCTL_RTC_GET_DATE_AND_TIME \
	CTL_CODE(FILE_DEVICE_UNKNOWN,0x805,METHOD_BUFFERED,FILE_ANY_ACCESS)

#define IOCTL_RTC_MEASURE_CYCLE	\
	CTL_CODE(FILE_DEVICE_UNKNOWN,0x806,METHOD_BUFFERED,FILE_ANY_ACCESS)

#define IOCTL_RTC_CHECK_INTERRUPT	\
	CTL_CODE(FILE_DEVICE_UNKNOWN,0x807,METHOD_BUFFERED,FILE_ANY_ACCESS)

#define IOCTL_RTC_DRIVER_CONFIG	\
	CTL_CODE(FILE_DEVICE_UNKNOWN,0x810,METHOD_BUFFERED,FILE_ANY_ACCESS)

#define IOCTL_RTC_DRIVER_GET_INFO	\
	CTL_CODE(FILE_DEVICE_UNKNOWN,0x811,METHOD_BUFFERED,FILE_ANY_ACCESS)

enum _RTC_STATUS_TAG {RTC_STATUS_SUCCESS,RTC_STATUS_NO_CLEAR,
	RTC_STATUS_NO_SET,RTC_STATUS_UNEXP_INTR,RTC_STATUS_NO_INTR};

//
// Input buffer for IOCTL_RTC_WRITE_RAM & IOCTL_RTC_READ_RAM
//
#pragma warning(disable:4200)

typedef struct {
	ULONG Start;					// starting offset (0x0E - 0x7F)
	ULONG Length;					// number of bytes
	UCHAR Data[];					// input data (if write)
} RTC_RAM_BLOCK,*PRTC_RAM_BLOCK;

#pragma warning(default:4200)

//
// Input buffer for IOCTL_RTC_SET_DATE_AND_TIME
// Output buffer for IOCTL_RTC_GET_DATE_AND_TIME
//
#pragma warning(disable:4201)
typedef union {
	UCHAR Data[7];
	struct {
		USHORT Second;
		USHORT Minute;
		USHORT Hour;
		USHORT DayOfWeek;
		USHORT DayOfMonth;
		USHORT Month;
		USHORT Year;
	};
} RTC_DATE_AND_TIME_BLOCK,*PRTC_DATE_AND_TIME_BLOCK;

#pragma warning(default:4201)
//
// Output buffer for IOCTL_RTC_MEASURE_CYCLE
//
typedef struct {
	ULONG			Reserved;		// for internal driver use
	ULONG			TotalSeconds;	// number of seconds as measured by RTC clock
	LARGE_INTEGER	CpuCycles;		// number of CPU cycles taken
	struct {						// For performance reasons, the driver doesn't
		LARGE_INTEGER CpuCount;		// do floating point operations. The CPU freq
		LARGE_INTEGER TimerCount;	// should be computed by the app as:
		LARGE_INTEGER TimerFreq;	// double CpuFreq = (double)TimerFreq / TimerCount * CpuCount
	} CpuFreq;
} RTC_MEASURE_CYCLE_BLOCK,*PRTC_MEASURE_CYCLE_BLOCK;

//
// Input buffer for IOCTL_RTC_DRIVER_CONFIG
//
typedef struct {
	ULONG				HwrRefTimerId;			// reference hardware timer id
	USHORT				HwrRefTimerPort;		// reference hardware timer port
} RTC_DRIVER_CONFIG_BLOCK,*PRTC_DRIVER_CONFIG_BLOCK;

//
// Output buffer for IOCTL_RTC_DRIVER_GET_INFO
//
#pragma warning(disable:4201)
typedef struct {
	union {
		ULONG	Version;
		struct {
			UCHAR MinorVersion;
			UCHAR MajorVersion;
			USHORT Reserved1;
		};
	};
	UCHAR Reserved2[4];
} RTC_DRIVER_INFO_BLOCK,*PRTC_DRIVER_INFO_BLOCK;
#pragma warning(default:4201)


#endif // _RTCIOCTL_H