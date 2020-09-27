 /***************************************************************************\
|*                                                                           *|
|*       Copyright 1993-1998 NVIDIA, Corporation.  All rights reserved.      *|
|*                                                                           *|
|*     NOTICE TO USER:   The source code  is copyrighted under  U.S. and     *|
|*     international laws.  NVIDIA, Corp. of Sunnyvale,  California owns     *|
|*     copyrights, patents, and has design patents pending on the design     *|
|*     and  interface  of the NV chips.   Users and  possessors  of this     *|
|*     source code are hereby granted a nonexclusive, royalty-free copy-     *|
|*     right  and design patent license  to use this code  in individual     *|
|*     and commercial software.                                              *|
|*                                                                           *|
|*     Any use of this source code must include,  in the user documenta-     *|
|*     tion and  internal comments to the code,  notices to the end user     *|
|*     as follows:                                                           *|
|*                                                                           *|
|*     Copyright  1993-1998  NVIDIA,  Corporation.   NVIDIA  has  design     *|
|*     patents and patents pending in the U.S. and foreign countries.        *|
|*                                                                           *|
|*     NVIDIA, CORPORATION MAKES NO REPRESENTATION ABOUT THE SUITABILITY     *|
|*     OF THIS SOURCE CODE FOR ANY PURPOSE. IT IS PROVIDED "AS IS" WITH-     *|
|*     OUT EXPRESS OR IMPLIED WARRANTY OF ANY KIND.  NVIDIA, CORPORATION     *|
|*     DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOURCE CODE, INCLUD-     *|
|*     ING ALL IMPLIED WARRANTIES  OF MERCHANTABILITY  AND FITNESS FOR A     *|
|*     PARTICULAR  PURPOSE.  IN NO EVENT  SHALL NVIDIA,  CORPORATION  BE     *|
|*     LIABLE FOR ANY SPECIAL,  INDIRECT,  INCIDENTAL,  OR CONSEQUENTIAL     *|
|*     DAMAGES, OR ANY DAMAGES  WHATSOEVER  RESULTING  FROM LOSS OF USE,     *|
|*     DATA OR PROFITS,  WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR     *|
|*     OTHER TORTIOUS ACTION,  ARISING OUT OF OR IN CONNECTION  WITH THE     *|
|*     USE OR PERFORMANCE OF THIS SOURCE CODE.                               *|
|*                                                                           *|
|*     RESTRICTED RIGHTS LEGEND:  Use, duplication, or disclosure by the     *|
|*     Government is subject  to restrictions  as set forth  in subpara-     *|
|*     graph (c) (1) (ii) of the Rights  in Technical Data  and Computer     *|
|*     Software  clause  at DFARS  52.227-7013 and in similar clauses in     *|
|*     the FAR and NASA FAR Supplement.                                      *|
|*                                                                           *|
 \***************************************************************************/

/*
 * nvmisc.h
 */
#ifndef __NV_MISC_H
#define __NV_MISC_H

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

// 
// control struct and defines for NvRmI2CAccess()
//
typedef struct
{
    unsigned long	token;
    unsigned long	cmd;
    unsigned long	port;
    unsigned long	flags;
    unsigned long	data;
    unsigned long	status;
} NVRM_I2C_ACCESS_CONTROL;

// commands
#define	NVRM_I2C_ACCESS_CMD_ACQUIRE     1
#define	NVRM_I2C_ACCESS_CMD_RELEASE     2
#define	NVRM_I2C_ACCESS_CMD_WRITE_BYTE  3
#define	NVRM_I2C_ACCESS_CMD_READ_BYTE   4
#define	NVRM_I2C_ACCESS_CMD_NULL        5
#define	NVRM_I2C_ACCESS_CMD_RESET       6
#define NVRM_I2C_ACCESS_CMD_READ_SDA    7
#define NVRM_I2C_ACCESS_CMD_READ_SCL    8
#define NVRM_I2C_ACCESS_CMD_WRITE_SDA   9
#define NVRM_I2C_ACCESS_CMD_WRITE_SCL   10

// flags
#define NVRM_I2C_ACCESS_FLAG_START	    0x1
#define	NVRM_I2C_ACCESS_FLAG_STOP	    0x2
#define	NVRM_I2C_ACCESS_FLAG_ACK	    0x4

// port
#define	NVRM_I2C_ACCESS_PORT_PRIMARY    1
#define	NVRM_I2C_ACCESS_PORT_SECONDARY  2

// status
#define	NVRM_I2C_ACCESS_STATUS_SUCCESS         0
#define	NVRM_I2C_ACCESS_STATUS_ERROR           1
#define	NVRM_I2C_ACCESS_STATUS_PROTOCOL_ERROR  2
#define	NVRM_I2C_ACCESS_STATUS_DEVICE_BUSY     3

//
// misc string definitions for registry manipulation
//
#define STR_NVIDIA                  "NVidia"
#define STR_DEV_NODE_RM             "SOFTWARE\\NVIDIA Corporation\\RIVA TNT\\System"
#define STR_DEV_NODE_DISPLAY        "SOFTWARE\\NVIDIA Corporation\\RIVA TNT\\Display";
#define STR_DEV_NODE_DISPLAY_NUMBER "SOFTWARE\\NVIDIA Corporation\\RIVA TNT\\Display\\DeviceX";
#define STR_RM_CORE_NAME            "RmCoreName"

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // __NV_MISC_H

