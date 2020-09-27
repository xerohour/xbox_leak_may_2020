// Factory.cpp: implementation of the CFactory class.
//
//////////////////////////////////////////////////////////////////////

#pragma warning( disable : 4786)

#include "Factory.h"
#include <map>
#include <algorithm>
#include <objbase.h>
#include "guiddef.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


static HRESULT Register (GUID clsid, CFactory::createFn Create);    // Register a CLSID with the factory
static HRESULT Unregister(GUID clsid);                              // Remove this CLSID from the factory

// Typedefs for convience
typedef std::map <GUID, CFactory::createFn> factoryMap;
typedef factoryMap::iterator                itterator;
typedef factoryMap::const_iterator          const_itterator;

static factoryMap* pList;

// Some overloads to allow us to use the std::map features
static bool operator < (GUID const& lhs, GUID const& rhs)
{
    return (memcmp (&lhs, &rhs, sizeof (lhs)) < 0 ? true : false);
}

static bool operator == (factoryMap::value_type const& lhs, GUID const& rhs)
{
    return (memcmp (&lhs.first, &rhs, sizeof (lhs.first)) == 0 ? true : false);
}

HRESULT CFactory::Create(GUID clsid, GUID iid, void** ppv)
{
    // Make sure the map isn't empty and the pointer is valid
    if (!pList || !ppv)
    {
        return (E_INVALIDARG);
    }

    // Search the map 
    itterator itt = pList->find (clsid);
    if (itt == pList->end ())
    {
        return (E_INVALIDARG);
    }

    // Create the object using its registered creation function
    IUnknown* pIUnk = NULL;
    createFn Create = (*itt).second;

    HRESULT hr = Create (&pIUnk);
    if (FAILED (hr))
    {
        return (hr);
    }

    // Ask for the required interface
    hr = pIUnk->QueryInterface (iid, ppv);
    
    if (FAILED (hr))
    {
        *ppv = NULL;
    }

    // Release once to set the refcount back to 1
    pIUnk->Release ();

    return (hr);

}

CFactory::CRegister::CRegister (GUID clsid, createFn Create) : m_clsid (clsid)
{
    HRESULT hr = Register (clsid, Create);
}

CFactory::CRegister::~CRegister()
{
    Unregister (m_clsid);
}

static HRESULT Register(GUID clsid, CFactory::createFn Create)
{
    // Create a map if we don't have one
    if (!pList)
    {
        pList = new factoryMap;
    }

    // Handle out of memory situations
    if (!pList)
    {
        return (E_OUTOFMEMORY); // out of memeory
    }

    // Search for this clsid
    if (pList->find (clsid) != pList->end ())
    {
        return (E_INVALIDARG);
    }

    // Add this to the map
    try
    {
        (*pList)[clsid] = Create;
    }
    catch (...)
    {
        return (S_FALSE);
    }

    return (S_OK);
}


static HRESULT Unregister(GUID clsid)
{
    // If the map is empty, there is nothing to remove
    if (!pList)
    {
        return (E_INVALIDARG);
    }

    // Search the map
    itterator itt = pList->find (clsid);
    if (itt == pList->end ())
    {
        return (E_INVALIDARG);
    }

    // Remove the element from the map
    pList->erase (itt);

    // If the map is empty, clean up
    if (!pList->size ())
    {
        delete pList;
        pList = NULL;
    }

    return (S_OK);
}
