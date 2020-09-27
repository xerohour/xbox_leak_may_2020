#ifndef KEYCHECK_H

#define KEYCHECK_H	1

#ifdef _DEBUG

#define	KEYENTRY				long	nKEY;

#define	KEYSET(ptr, key) ((ptr) -> nKEY = key)
BOOL AuxPrintf(
	LPSTR text,
	...);

#define	KEYCHECKRET(ptr, key, ret)	\
if(!(ptr) || ((ptr) -> nKEY != key)) \
{ _KeyError(__FILE__, __LINE__); return ret;}

#define	KEY0CHECKRET(ptr, key, ret)	\
if((ptr) && ((ptr) -> nKEY != key)) \
{ _KeyError(__FILE__, __LINE__); return ret;}

#define	KEYCHECK(ptr, key)	\
if(!(ptr) || ((ptr) -> nKEY != key)) { _KeyError(__FILE__, __LINE__); }

#define	KEY0CHECK(ptr, key)	\
if((ptr) && ((ptr) -> nKEY != key)) { _KeyError(__FILE__, __LINE__); }

void	_KeyError(LPSTR pszFileName, UINT nLine);
#else

#define	KEYENTRY
#define	KEYSET(ptr, key)

#define	KEYCHECKRET(ptr, key, ret)
#define	KEY0CHECKRET(ptr, key, ret)
#define	KEYCHECK(ptr, key)
#define	KEY0CHECK(ptr, key)

#endif

#endif
