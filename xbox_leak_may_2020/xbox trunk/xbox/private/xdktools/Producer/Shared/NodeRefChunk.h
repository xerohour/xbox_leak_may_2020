#ifndef __STYLEREF_H__
#define __STYLEREF_H__ 1

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "RiffStrm.h"

interface IDMUSProdFramework;
interface IDMUSProdNode;

class CNodeRefChunk {
public:
	CNodeRefChunk( IDMUSProdFramework* pIFramework, IDMUSProdNode* pINode );
	~CNodeRefChunk();

	HRESULT Load( IStream* pIStream );
	HRESULT Save( IStream* pIStream );

	IDMUSProdFramework* m_pIFramework;
	IDMUSProdNode* m_pINode;

protected:
	HRESULT SaveChildNode( IStream* pIStream, IDMUSProdNode *pDocRootNode );
	HRESULT LoadChildNode( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO *pckMain );
	IDMUSProdNode *FindChildNode( IDMUSProdNode *pDocRootNode, const CString *pcstrName, const REFGUID rguidID );
};

#endif //__STYLEREF_H__
