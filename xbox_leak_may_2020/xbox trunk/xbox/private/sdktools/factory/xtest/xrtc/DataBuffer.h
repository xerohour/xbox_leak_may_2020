class CDataBuffer //: public CObject  
{
public:
	void RotateDown();
	void RotateUp();
	void FillRandom(UINT *uiRandScratch);
	void FillLinear();
	DWORD random(UINT *pScratch);
	UCHAR operator[](int nIndex) {return m_pData[m_uBase + nIndex];}
	operator PUCHAR () {return &m_pData[m_uBase];}
	CDataBuffer(ULONG uSize);
	virtual ~CDataBuffer();

protected:
	PUCHAR m_pData;
	ULONG m_uSize;
	ULONG m_uBase;
};

