#ifndef __STYLEREF_H__
#define __STYLEREF_H__ 1

interface IDMUSProdFramework;
interface IDMUSProdNode;

class CStyleRef {
public:
    CStyleRef();
	~CStyleRef();

	HRESULT Load( IStream* pIStream );
	HRESULT Save( IStream* pIStream, CLSID clsidComponent, GUID guidComponent );

	IDMUSProdFramework* m_pIFramework;
	IDMUSProdNode* m_pINode;
};

#endif //__STYLEREF_H__
