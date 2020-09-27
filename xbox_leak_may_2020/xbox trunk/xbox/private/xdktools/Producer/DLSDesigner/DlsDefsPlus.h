// DlsDefsPlus.h : header file
//

#ifndef DLSDEFSPLUS_H
#define DLSDEFSPLUS_H

#define BUFFER_64	64
#define BUFFER_128	128
#define BUFFER_256	256

#define DLS_ACTION_EDIT_ARTICULATION	1
#define DLS_ACTION_EDIT_REGION			2
#define DLS_ACTION_SELECT_REGION		3

#define MSB_MASK (0x00007F00)
#define LSB_MASK (0x0000007F)

#define MAKE_BANK(DRUM, MSB, LSB) ( ((DWORD) ((DRUM) == 0 ? 0 : 0x80000000))  | ((DWORD)(BYTE)(MSB) << 8) | (DWORD)(LSB))
#define MSB(ulBank) (((ulBank) & MSB_MASK) >> 8)
#define LSB(ulBank) ((ulBank) & LSB_MASK)

#define DLS_ERR_FIRST					((HRESULT)0x80040100L) //(OLE_E_LAST + 1)

#define SC_PATCH_DUP_GM					(0x00000001)		// patch number in GM set
#define SC_PATCH_DUP_COLLECTION			(0x00000002)		// duplicate patch in collection
#define SC_PATCH_DUP_SYNTH				(0x00000008)		// duplicate patch in other collection
#define SC_NO_VALID_PATCHES				(0x00000010)		// All patches taken


#define	DLS_PATCH_DUP_GM				(DLS_ERR_FIRST | SC_PATCH_DUP_GM)
#define DLS_PATCH_DUP_COLLECTION		(DLS_ERR_FIRST | SC_PATCH_DUP_COLLECTION)
#define DLS_PATCH_DUP_GM_AND_COLLECTION	(DLS_ERR_FIRST | SC_PATCH_DUP_GM | SC_PATCH_DUP_COLLECTION)
#define DLS_PATCH_DUP_SYNTH				(DLS_ERR_FIRST | SC_PATCH_DUP_SYNTH)
#define DLS_PATCH_DUP_GM_AND_SYNTH		(DLS_ERR_FIRST | SC_PATCH_DUP_GM | SC_PATCH_DUP_SYNTH)
#define DLS_PATCH_DUP_ALL				(DLS_ERR_FIRST | SC_PATCH_DUP_GM | SC_PATCH_DUP_COLLECTION | SC_PATCH_DUP_SYNTH)
#define DLS_NO_VALID_PATCHES			(DLS_ERR_FIRST | SC_NO_VALID_PATCHES) 

#define	DLS_DLG_FIX_ALL					0x00000010
#define DLS_DLG_IGNORE_ALL_GM			0x00000020
#define DLS_DLG_IGNORE_ALL_COLLECTION	0x00000040
#define DLS_DLG_IGNORE_ALL_SYNTH		0x00000080

#define SC_BAD_INST_HEADER				(0x00000200)
#define DLS_1_INST_HEADER_SIZE			(12)

#define	DEFAULT_VELOCITY				127
#define WAVE_PREVIEW_VELOCITY			DEFAULT_VELOCITY
#define INSTR_PREVIEW_VELOCITY			DEFAULT_VELOCITY

//Legacy chunks these should be recognized and promptly ignored in Loading code
#define FOURCC_EDIT	mmioFOURCC('e','d','i','t')
#define FOURCC_MSYN mmioFOURCC('m','s','y','n')


#endif // #ifndef DLSDEFSPLUS_H