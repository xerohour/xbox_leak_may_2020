/*----------------------------------------------------------------------------*/
/*
 * vddi2c.h
 *
 */

/* 
 * common defines and typedefs so we don't have to include all the windows stuff 8
 */

#ifndef FALSE
#define	FALSE	0
#endif

#ifndef TRUE
#define	TRUE	1
#endif

typedef unsigned int	UINT;
typedef unsigned char	UCHAR;
typedef unsigned short	USHORT;
typedef unsigned long	ULONG;

typedef long		NTSTATUS;	/* from ntdef.h in Win98 DDK */
typedef void*		PDEVICE_OBJECT; /* since we don't use this object */

/* from ntstatus.h */
#define STATUS_SUCCESS                   ((NTSTATUS)0x00000000L) // ntsubauth
#define STATUS_OPEN_FAILED               ((NTSTATUS)0xC0000136L)
#define STATUS_INVALID_HANDLE            ((NTSTATUS)0xC0000008L)    // winnt
#define STATUS_ADAPTER_HARDWARE_ERROR    ((NTSTATUS)0xC00000C2L)
#define STATUS_INVALID_PARAMETER         ((NTSTATUS)0xC000000DL)
#define STATUS_DEVICE_BUSY               ((NTSTATUS)0x80000011L)


/*----------------------------------------------------------------------------*/
/*
 * XXX	The relevant portion of i2c.h (from either the nt5 ddk or win98 ddk) is 
 *	copied here so we don't have to include i2c.h explicitly.  This is not 
 *	good but including i2c.h is difficult.
 */

// used below if neccessary
#ifndef BYTE
#define BYTE UCHAR
#endif
#ifndef DWORD
#define DWORD ULONG
#endif

//
// I2C section
//
// I2C Commands
#define I2C_COMMAND_NULL         0X0000
#define I2C_COMMAND_READ         0X0001
#define I2C_COMMAND_WRITE        0X0002
#define I2C_COMMAND_STATUS       0X0004
#define I2C_COMMAND_RESET        0X0008

// The following flags are provided on a READ or WRITE command
#define I2C_FLAGS_START          0X0001 // START + addx
#define I2C_FLAGS_STOP           0X0002 // STOP
#define I2C_FLAGS_DATACHAINING   0X0004 // STOP, START + addx 
#define I2C_FLAGS_ACK            0X0010 // ACKNOWLEDGE (normally set)

// The following status flags are returned on completion of the operation
#define I2C_STATUS_NOERROR       0X0000  
#define I2C_STATUS_BUSY          0X0001
#define I2C_STATUS_ERROR         0X0002

typedef struct _I2CControl {
        ULONG Command;          // I2C_COMMAND_*
        DWORD dwCookie;         // Context identifier returned on Open
        BYTE  Data;             // Data to write, or returned byte
        BYTE  Reserved[3];      // Filler
        ULONG Flags;            // I2C_FLAGS_*
        ULONG Status;           // I2C_STATUS_*
        ULONG ClockRate;        // Bus clockrate in Hz.
} I2CControl, *PI2CControl;

