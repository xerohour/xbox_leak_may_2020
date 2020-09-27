//  PROPERTY.CPP
//
//		Text property handling
//
//	Created 15-Mar-2000 [JonT]

#include "xcmain.h"

//---------------------------------------------------------------------
//  CPropertyBundle - public

//  CPropertyBundle::GetProperty
//		Returns the value for a given property.
//		Note that properties can come from different places:
//			*from command line switches: imgbld /d1 makes a property 'd' with value '1'
//			*from a configuration file
//			*from a previous call to SetProp
//		Properties are returned as constant strings.
//		NULL means property does not exist.

LPCTSTR
CPropertyBundle::Get(
	LPCTSTR lpszName
	)
{
	CProperty* pp;

	if ((pp = Find(lpszName)) == NULL)
		return NULL;
	else
		return pp->GetValue();
}


//  CPropertyBundle::SetProperty
//		Sets a value for the given property, creates the property if necessary.

void
CPropertyBundle::Set(
	LPCTSTR lpszName,
	LPCTSTR lpszValue
	)
{
	CProperty* pp;
	int nHashBucket;

	// See if we need to create it
	if ((pp = Find(lpszName)) == NULL)
	{
		// Create the new property
		pp = new CProperty;
		if (!pp)
			g_ib.Exit(-1, "Error: Out of Memory");
		pp->SetName(lpszName);
		pp->SetValue(lpszValue);

		// Create a list node to link it into
		//pnode = new CNode;
		//pnode->SetData(pp);

		// Find the hash bucket to link it into
		nHashBucket = HashNameToIndex(lpszName);

		// Link the node into the end of the bucket list
		pp->Link(&m_lhHashTable[nHashBucket], NULL);
	}

	// Already exists, so just replace the value
	else
	{
		pp->SetValue(lpszValue);
	}
}


//---------------------------------------------------------------------
//  CPropertyBundle - private

CProperty*
CPropertyBundle::Find(
	LPCTSTR lpszName
	)
{
	int nBucket;
	CNode* pnode;

	// Get the bucket number
	nBucket = HashNameToIndex(lpszName);

	// Search this bucket for the name
	for (pnode = m_lhHashTable[nBucket].GetHead() ; pnode ; pnode = pnode->Next())
		if (_stricmp(lpszName, ((CProperty*)pnode)->GetName()) == 0)
			break;

	if (pnode)
		return (CProperty*)pnode;
	else
		return NULL;
}


int
CPropertyBundle::HashNameToIndex(
	LPCTSTR lpszName
	)
{
	DWORD dwHash = 0;
	TCHAR ch;
	int i;
	int nIncr = strlen(lpszName) / 3;

	ASSERT(lpszName != NULL && *lpszName != '\0');

	for (i = 0 ; i < 3 ; i++)
	{
		// Get the next character
		if ((ch = *lpszName) == 0)
			break;
		lpszName += nIncr;

		// Compute next random number in sequence using character to modify
		dwHash =  1664525 * (dwHash ^ ch) + 1013904223;
	}

	dwHash = dwHash % TABLE_SIZE;

	return (int)dwHash;
}
