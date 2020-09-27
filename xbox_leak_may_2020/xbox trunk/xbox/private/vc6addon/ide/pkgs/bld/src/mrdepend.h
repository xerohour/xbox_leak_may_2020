//
// Declare minimal rebuild dependencies interface.
//
// [tomse]
//

#ifndef _INCLUDE_MRDEPEND_H
#define _INCLUDE_MRDEPEND_H

class IMreDependencies {
public:
	static IMreDependencies* GetMreDependenciesIFace( LPCTSTR szIdbFile );
	virtual BOOL GetDependencies( LPCTSTR szPath, CStringList& strList, BOOL bUsePch ) = 0;
	virtual BOOL Release() = 0;
};

#endif // _INCLUDE_MRDEPEND_H

