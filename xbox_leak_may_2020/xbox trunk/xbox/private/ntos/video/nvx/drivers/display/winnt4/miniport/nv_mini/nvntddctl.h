// display driver - to - miniport IOCTL's

// device type and base control code
#define FILE_DEVICE_NVRM 	0x00008000
#define NVRM_IOCTL_INDEX 	0x00000800
#define NV_IOCTL_CODE(f)		  	\
	CTL_CODE(                   	\
		FILE_DEVICE_NVRM+(f),   	\
		NVRM_IOCTL_INDEX,     		\
		METHOD_BUFFERED,        	\
		FILE_ANY_ACCESS         	\
	)

//
#define NVDD_IOCTL_CLOSE_RESOURCE_MANAGER	NV_IOCTL_CODE(0x0FFC)
#define NVDD_IOCTL_OPEN_RESOURCE_MANAGER	NV_IOCTL_CODE(0x0FFD)
