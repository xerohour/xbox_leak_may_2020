/*************************************************************
Module name: PrfDB.H
*************************************************************/

#ifndef _PRFDB_H_
#define _PRFDB_H_

typedef const PERF_DATA_BLOCK * PCPRFDATABLK;
typedef const PERF_OBJECT_TYPE * PCPRFOBJTYPE;
typedef const PERF_INSTANCE_DEFINITION * PCPRFINSTDEF;
typedef const PERF_COUNTER_DEFINITION * PCPRFCNTRDEF;
typedef const PERF_COUNTER_BLOCK * PCPRFCNTRBLK;
	
class CPRFDB
{
	PCPRFDATABLK m_pPrfDataBlk;

	public:
	CPRFDB (PCPRFDATABLK pPrfDataBlk)
	{
		m_pPrfDataBlk = pPrfDataBlk;
	}
	~CPRFDB (void) {}

	private:
	PCPRFOBJTYPE GetFirstObject(void) const;
	PCPRFOBJTYPE GetNextObject(PCPRFOBJTYPE pPrfObjType) const;
	int GetNumObjects(void) const;

	public:
	PCPRFOBJTYPE FindObjFromTitleIndex(DWORD dwTitleIndex) const;
	PCPRFOBJTYPE FindObjFromIndex(int nIndexObj) const;

	PCPRFINSTDEF FindInstFromIndex(PCPRFOBJTYPE pPrfObjType, int nIndexInst) const;
	int GetNumInstances(PCPRFOBJTYPE pPrfObjType) const;
	PCPRFINSTDEF GetFirstInst(PCPRFOBJTYPE pPrfObjType) const;
	PCPRFINSTDEF GetNextInst(PCPRFINSTDEF pPrfInstDef) const;
	PCPRFINSTDEF GetInst(PCPRFOBJTYPE pPrfObjType, int nIndexInst) const;
	LPCTSTR GetInstName(PCPRFINSTDEF pPrfInstDef) const;
	LPCWSTR GetInstName(PCPRFOBJTYPE pPrfObjType, int nIndexInst) const;
	PCPRFINSTDEF FindInstPrnt(PCPRFINSTDEF pPrfInstDef) const;

	int GetNumCntrs(PCPRFOBJTYPE pPrfObjType) const;
	PCPRFCNTRDEF GetFirstCntr(PCPRFOBJTYPE pPrfObjType) const;
	PCPRFCNTRDEF GetNextCntr(PCPRFCNTRDEF pPrfCntrDef) const;
	PCPRFCNTRDEF GetCntrFromTitleIndex(DWORD dwTitleIndex, PCPRFOBJTYPE pPrfObjType) const;
	void * const GetCntrData(PCPRFINSTDEF pPrfInstDef, PCPRFCNTRDEF pPrfCntrDef) const;
	PCPRFCNTRDEF FindCntrFromTitleIndex(DWORD dwTitleIndex, PCPRFOBJTYPE pPrfObjType) const;
};

#endif	// _PRFDB_H_

//////////////////////// End Of File /////////////////////////
