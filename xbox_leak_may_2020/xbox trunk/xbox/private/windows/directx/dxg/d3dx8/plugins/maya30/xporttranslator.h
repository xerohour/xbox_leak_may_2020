#ifndef xfileTranslator_h
#define xfileTranslator_h




#include <maya/MPxFileTranslator.h>

#include <MDt.h>



class xfileTranslator: public MPxFileTranslator 
{
	public:

		xfileTranslator() {};
	   ~xfileTranslator() {};

		static void*	creator ()	{ return new xfileTranslator(); };

		MStatus			reader(const MFileObject& file,
							   const MString& optionsString,
							   MPxFileTranslator::FileAccessMode mode)	{ return MS::kSuccess; };

		MStatus         writer(const MFileObject& file,
		                       const MString& optionsString,
		                       MPxFileTranslator::FileAccessMode mode);

		bool            haveReadMethod()	const	{ return false; };
		bool            haveWriteMethod()	const	{ return true; };
		bool            canBeOpened()		const	{ return false; };
		MString         defaultExtension()	const	{ return ""; };
		MFileKind       identifyFile (const MFileObject& fileName, const char *buffer, short size)	const	
							{ return kIsMyFileType; };

};






#define WRITE_FLOAT(pbCur, _float)	{												\
										float* __pfloat	= (float*)pbCur;			\
										*(__pfloat++)	= _float;					\
										pbCur			= (PBYTE)__pfloat;			\
									}

#define WRITE_PCHAR(pbCur, pchar)	{												\
										char** __ppchar	= (char**)pbCur;			\
										*(__ppchar++)	= pchar;					\
										pbCur			= (PBYTE)__ppchar;			\
									}

#define WRITE_DWORD(pbCur, dword)	{												\
										DWORD* __pdword	= (DWORD*)pbCur;			\
										*(__pdword++)	= dword;					\
										pbCur			= (PBYTE)__pdword;			\
									}

#define WRITE_WORD(pbCur, word)		{												\
										WORD* __pword	= (WORD*)pbCur;				\
										*(__pword++)	= word;						\
										pbCur			= (PBYTE)__pword;			\
									}


#define	SCENE_ROOT	"SCENE_ROOT"












#endif