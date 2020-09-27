#ifndef __BLDPACK_H__
#define __BLDPACK_H__

class CProjComponentMgr;

class CPackage: public CObject {
public:
	virtual BOOL DoBldSysCompRegister(CProjComponentMgr * pcompmgr, DWORD blc_type, DWORD blc_id){return TRUE;};

	// Help information (including legal resource, command, and window id ranges)
	enum RANGE_TYPE
	{
		MIN_RESOURCE, MAX_RESOURCE, 
		MIN_COMMAND, MAX_COMMAND, 
		MIN_WINDOW, MAX_WINDOW,
		MIN_STRING, MAX_STRING,
	};

};

#endif
