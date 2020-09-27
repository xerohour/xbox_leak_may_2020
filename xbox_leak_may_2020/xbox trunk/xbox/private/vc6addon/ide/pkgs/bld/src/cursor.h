//
// cursor.h
//
// Defines a simple cursor manager
//
// Implementation is in : cursor.cpp
//
// History:
// Date				Who			What
// 01/26/94			colint			Created
//////////////////////////////////////////////////////////////////////////

#ifndef __CURSORMGR_H__
#define __CURSORMGR_H__

class CCursorManager
{
public:
	CCursorManager();
	~CCursorManager();

	HCURSOR GetDDCur( int iBase )
	{	return GetCursor(iBase*4); }
	HCURSOR GetDDCopyCur( int iBase )
	{	return GetCursor(iBase*4 + 1); }
	HCURSOR GetDDMultiCur( int iBase )
	{	return GetCursor(iBase*4 + 2); }
	HCURSOR GetDDMultiCopyCur( int iBase )
	{	return GetCursor(iBase*4 + 3); }

	void Flush();
	void Cleanup();

	// Indices into the aCursorIDs static array.
	// Because they are indices, order does matter here.
	enum {
		iFileItem,			// = 0
	};

protected:
	HCURSOR GetCursor( int iCursor );

	HCURSOR* m_phCurs;
	int      m_nAllocCurs;
};

extern CCursorManager g_Cursors;

#endif // __CURSORMGR_H__
