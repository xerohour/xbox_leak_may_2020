#ifndef __NCBDEF_H__
#define __NCBDEF_H__

// definition for standard BSC interface
// and different ATRs and TYPs
#include "bscext.h"
#include "ncarray.h"
// TYP == BYTE
// ATR == USHORT (we don't use this)
// ATR32 == DWORD

// various kind of streams:
#define SZ_NCB_TARGET_INFO		"/ncb/targetinfo"
#define SZ_NCB_TARGET_PREFIX	"/ncb/target/"
#define SZ_NCB_MODULE_INFO		"/ncb/moduleinfo"
#define SZ_NCB_MODULE_PREFIX	"/ncb/module/"
#define SZ_NCB_STORE_INFO		"/ncb/storeinfo"
#define SZ_NCB_STORE_PREFIX		"/ncb/store/"
#define SZ_NCB_VERSION_INFO		"/ncb/versioninfo"
#define SZ_NCB_IINSTDEFS        "/ncb/iinstdefs"
#define SZ_NCB_REFERENCEINFO	"/ncb/referenceInfo"
#define SZ_NCB_PSEUDO_TARGET    "__NcbPseudoTarget__"

#define NCB_PSEUDO_TARGET		0xFFFFFFFD  // This target is used for files that are not part of the project
											// but that requires parsing. For example, the tli and tlh files
#define NCB_CURRENT_VERSION		0xE

struct NCB_VERSION_INFO
{
	DWORD	m_ver;
};

// for vc99, worry about making attrib 3 bytes or Dword
// consider packing using #pragma. Also make sure
// calculateCRC gives out the right size - Ignatius / ShankarV
// ATR32 is the new ATR type the NCBs will use
struct NCB_ENTITY 
{
	NI			m_ni;		// name index, index used in the name table (nmt)
	TYP			m_typ;		// type of the object, eg: function/var/typedef/class/etc and decl/defn
	ATR32		m_atr;      // attribute value, eg: static, virtual, private, protected
};


struct NCB_PROP
{
	NCB_ENTITY	m_en;		// entity (consists of:
							//	o index to name table (NI)
							//  o type of the object  (TYP)
							//	o attribute of the object (ATR32)
	USHORT	m_iUse;			// index to use (index to NCB_USE table) (iMac index)
	USHORT	m_iParam;		// index to return type and params (index to NI table) (iMac index)
	USHORT	m_lnStart;		// line number of the entity
	USHORT	m_lnEnd;		// end line number of the entity
};

// this is used for lookup 
struct NCB_USE
{
	BYTE	m_kind;			// kind of uses
	USHORT	m_iProp;		// index to prop table
};

// IN MEMORY representation of NCB_PROP (used when module content is loaded for
// write): (ie: loose version)
struct NCB_PROP_INMEM
{
	NCB_ENTITY	m_en;		// entity (consists of:
							// o index to name table (NI)
							// o type of the object (TYP)
							// o attribute of the object (ATR32)
	Array<NCB_USE> m_rgUse;	// array of NCB_USE (for each  prop)
	Array<NI>	m_rgParam;	// return value and parameters (for each prop)
	USHORT	m_lnStart;		// line number of the entity
	USHORT	m_lnEnd;		// end line number of the entity
};


#endif	
