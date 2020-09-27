
#ifndef _IMAGE_H_
#define _IMAGE_H_

#ifdef __cplusplus
extern "C" {
#endif

#define IMAGE_CLASSFILE_SIGNATURE	0xbebafeca	// CafeBabe in Big Endian


enum IMAGE_TYPE {
	IMAGE_TYPE_UNKNOWN,
	IMAGE_TYPE_DOS_STUB,
	IMAGE_TYPE_WIN16_OR_OS2,
	IMAGE_TYPE_WIN32,
	IMAGE_TYPE_CLASSFILE,
	IMAGE_TYPE_CRASHDUMP
};

#include "pshpack4.h"

typedef struct _IMAGE_INFO {

	ULONG	Size;
	ULONG	ImageType;

	union {
		struct {
			ULONG	Subsystem;
			WORD	Characteristics;
			WORD	Machine;
		} Win32;

		struct {
			BOOL	fUserMode;
			WORD	Machine;
		} CrashDump;
	} u;

} IMAGE_INFO;

#include "poppack.h"


BOOL WINAPI
GetImageInfo(
	LPCTSTR		ImageName,
	IMAGE_INFO*	ImageInfo
	);


//
// Is this image a valid Win32 subsystem image
//

#define IS_WIN32_SUBSYSTEM_IMAGE(ii)	\
	((ii).ImageType == IMAGE_TYPE_WIN32 &&	\
	((ii).u.Win32.Subsystem == IMAGE_SUBSYSTEM_WINDOWS_CUI ||	\
	 (ii).u.Win32.Subsystem == IMAGE_SUBSYSTEM_WINDOWS_GUI))

#ifdef __cplusplus
};		// extern "C"
#endif

#endif // _IMAGE_H_
