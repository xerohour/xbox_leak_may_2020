//
// Declare minimal rebuild dependencies interface.
//
// [tomse]
//

#ifndef _INCLUDE_MRDEPEND_H
#define _INCLUDE_MRDEPEND_H

#include <vccolls.h>
#include <path2.h>
#include "fileregistry.h"
#include "bldhelpers.h"

class IBldMreDependencies 
{
public:
	static IBldMreDependencies* GetMreDependenciesIFace(LPCOLESTR bstrIdbFile);
	virtual BOOL GetDependencies(LPCOLESTR szPath, CVCBuildStringCollection* pCollection, BOOL bUsePch) = 0;
	virtual BOOL GetDeploymentDependencies(LPCOLESTR szPath, CVCBuildStringCollection* pCollection) = 0;
	virtual BOOL Release() = 0;
};

// this helper class allows us to cache the IBldMreDependencies interface without having to worry about
// releasing it later (it goes away on destruction)
class CBldMreDependenciesContainer 
{
public:
	CBldMreDependenciesContainer() : m_pMrePtr(NULL)	{}
	CBldMreDependenciesContainer(CStringW& rstrIdbPath) : m_pMrePtr(NULL)
	{
		CalculateIFace(rstrIdbPath);
	}
	~CBldMreDependenciesContainer()
	{
		if (m_pMrePtr)
			m_pMrePtr->Release();
	}
	void CalculateIFace(CStringW& rstrIdbPath)
	{
		if (!rstrIdbPath.IsEmpty())
			m_pMrePtr = IBldMreDependencies::GetMreDependenciesIFace(rstrIdbPath);
	}
	static void UnloadMSPDB();
	IBldMreDependencies* m_pMrePtr;
};

#endif // _INCLUDE_MRDEPEND_H

