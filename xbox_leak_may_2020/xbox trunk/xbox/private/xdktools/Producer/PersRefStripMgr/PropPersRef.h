#ifndef __PROPPERS_REF_H_
#define __PROPPERS_REF_H_

#include <DMUSProd.h>

#pragma pack(2)

typedef struct PersListInfo
{
	IDMUSProdProject*	pIProject;
	CString				strProjectName;
	CString				strName;
	CString				strDescriptor;
	GUID				guidFile;
} PersListInfo;

#pragma pack()

class CPersRefItem;

class CPropPersRef 
{
public:
	CPropPersRef();
	CPropPersRef( const CPersRefItem *pPersRefItem );
	~CPropPersRef(); 
	void ApplyToPersRefItem( CPersRefItem *pPersRefItem );

	long			m_mtTime;			// What time this personality reference falls on
    DWORD			m_dwMeasure;		// What measure this personality reference falls on
    BYTE			m_bBeat;			// What beat this personality reference falls on
	DWORD			m_dwBits;			// Various bits
	PersListInfo	m_PersListInfo;
	GUID			m_guidProject;
	IDMUSProdNode*	m_pIPersDocRootNode;
};

#define UD_DRAGSELECT 0x0001

#endif // __PROPPERS_REF_H_
