// vad.h: interface for the CVpn class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VAD_H__5F567D61_C0A2_4170_B7EB_F0D51EF38E01__INCLUDED_)
#define AFX_VAD_H__5F567D61_C0A2_4170_B7EB_F0D51EF38E01__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CVadArray  
{
public:
	CVadArray();
	virtual ~CVadArray();

	bool Add(const MMVAD* vad);

	void ResetContent();

	ULONG GetAllocationProtecion(size_t element) const;
	ULONG GetEndingVpn(size_t element) const;
	ULONG GetStartingVpn(size_t element) const;
	size_t GetArraySize() const;

protected:
	bool GrowBy(size_t elements);

	size_t m_numberOfVad;
	PMMVAD m_vadArray;
	size_t m_arraySize;
};

#endif // !defined(AFX_VAD_H__5F567D61_C0A2_4170_B7EB_F0D51EF38E01__INCLUDED_)
