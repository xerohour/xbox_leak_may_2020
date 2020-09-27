#ifndef __PROPSTYLE_REF_H_
#define __PROPSTYLE_REF_H_

#include <DMUSProd.h>
#include <dmusici.h>

#pragma pack(2)

typedef struct StyleListInfo
{
	IDMUSProdProject*	pIProject;
	CString				strProjectName;
	CString				strName;
	CString				strDescriptor;
	GUID				guidFile;
} StyleListInfo;

#pragma pack()

class CStyleRefItem;

class CPropStyleRef 
{
public:
	CPropStyleRef();
	CPropStyleRef( const CStyleRefItem *pStyleRefItem );
	~CPropStyleRef(); 
	void ApplyToStyleRefItem( CStyleRefItem *pStyleRefItem );

    DWORD				m_dwMeasure;		// What measure this style reference falls on
	DWORD				m_dwBits;			// Various bits
	StyleListInfo		m_StyleListInfo;
	GUID				m_guidProject;
	IDMUSProdNode*		m_pIStyleDocRootNode;
	DMUS_TIMESIGNATURE	m_TimeSignature;
};

#define UD_DRAGSELECT		0x0001
#define UD_CURRENTSELECTION 0x0002

#endif // __PROPSTYLE_REF_H_
