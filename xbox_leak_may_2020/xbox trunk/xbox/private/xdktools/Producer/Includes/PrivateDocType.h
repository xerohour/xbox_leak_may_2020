#if !defined(PRIVATE_DOCTYPE_H__)
#define PRIVATE_DOCTYPE_H__
#include <objbase.h>

////////////////////////////////////////////////////////////////////////////////
// IDMUSProdDocTypeP interface IIDs

// {6F82059F-837C-49fa-8D69-072CF8B571E2}
DEFINE_GUID(IID_IDMUSProdDocTypeP, 0x6f82059f, 0x837c, 0x49fa, 0x8d, 0x69, 0x7, 0x2c, 0xf8, 0xb5, 0x71, 0xe2);

#undef	INTERFACE
#define INTERFACE  IDMUSProdDocTypeP
DECLARE_INTERFACE_(IDMUSProdDocTypeP, IUnknown)
{
	// IUnknown
	STDMETHOD(QueryInterface)		(THIS_ REFIID, LPVOID FAR *) PURE;
	STDMETHOD_(ULONG,AddRef)		(THIS) PURE;
	STDMETHOD_(ULONG,Release)		(THIS) PURE;

	// IDMUSProdDocTypeP
	STDMETHOD(ImportNode)	(THIS_ IUnknown* punkNode, IUnknown* punkTreePositionNode, IUnknown** ppIDocRootNode) PURE;
};

#endif // !defined(PRIVATE_DOCTYPE_H__)
