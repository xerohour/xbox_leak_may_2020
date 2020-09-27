#include <windows.h>
#define DEVICE_TYPE ULONG
#include <inc\scsi.h>
#pragma warning(disable:4200)
#include <inc\ntdddisk.h>
#include <inc\ntddcdvd.h>
#include <AmcMsEmuApi.h>
#include <AmcMsEmuApiError.h>
#include <emucore.h>
#include <dvdx2.h>
#include "emudev.h"

#define DVDEMU_ADSENSE_READ_ERROR 0x11
#define DVDEMU_ADSENSE_PARAMETER_LIST_LENGTH 0x1A
#define DVDEMU_ADSENSE_INVALID_PARAMETER_FIELD 0x26
#define DVDEMU_ADSENSE_SAVING_NOT_SUPPORTED 0x39
