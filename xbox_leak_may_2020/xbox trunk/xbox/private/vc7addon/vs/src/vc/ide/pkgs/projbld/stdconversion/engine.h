// CBuildEngine
//
// Build engine.
//
// [matthewt]
//
				   
#ifndef _ENGINE_H_
#define _ENGINE_H_

// definition of the CBuildAction class
class CProjItem;
class CBuildTool;

// definition of the CBuildActionList class
class  CBuildActionList : public CVCPtrList
{
public:
	CBuildActionList() : CVCPtrList(1) {}

	// find a tool and return the postion of the action
	VCPOSITION Find(CBuildTool * pTool);
};

#pragma hdrstop

class  CBuildAction : public CObject
{
	DECLARE_DYNAMIC(CBuildAction)
	
public:
	// Associate this action to a particular project item. This project item's
	// configuration reocrd will be accessed to retrieve configuration specific
	// action information.
	// Associate this action with a particular tool. It will access the CBuildTool's
	// methods in order to modify the action's build information.
	CBuildAction() {}
	CBuildAction(CProjItem *, CBuildTool *, BOOL fBuilder = FALSE, CConfigurationRecord * pcr = (CConfigurationRecord *)NULL );
	~CBuildAction();

	// Binding and un-binding to tools
	void Bind(CBuildTool * pTool);
	void UnBind();

	//
	// Assigning/unassigning of actions to project items (default is active config.)
	//
	static void AssignActions
	(
		CProjItem * pItem,
		CVCPtrList * plstSelectTools = (CVCPtrList *)NULL,
		CConfigurationRecord * pcr = (CConfigurationRecord *)NULL,
		BOOL fAssignContained = TRUE,
		BOOL fAssignContainedOnly = FALSE
	);
	static void UnAssignActions
	(
		CProjItem * pItem,
		CVCPtrList * plstSelectTools = (CVCPtrList *)NULL,
		CConfigurationRecord * pcr = (CConfigurationRecord *)NULL,
		BOOL fUnassignContained = TRUE
	);

	//
	// Adding/removing of actions to/from build instances (default is active config.)
	//
	static void BindActions
	(
		CProjItem * pItem,
		CVCPtrList * plstSelectTools = (CVCPtrList *)NULL,
		CConfigurationRecord * pcr = (CConfigurationRecord *)NULL,
		BOOL fBindContained = TRUE
	);
	static void UnBindActions
	(
		CProjItem * pItem,
		CVCPtrList * plstSelectTools = (CVCPtrList *)NULL,
		CConfigurationRecord * pcr = (CConfigurationRecord *)NULL,
		BOOL fUnbindContained = TRUE
	);

	// serialize/deserialize the action slob
	virtual void Serialize(CArchive & ar);

	__inline CBuildTool * BuildTool()	{return m_pTool;}
	__inline CProjItem * Item()			{return m_pItem;}

	// Internal helper to set tool
	void SetToolI(CBuildTool *);

	// Associated project item and tool (+ tool user-data)
	CProjItem * m_pItem;
	CBuildTool * m_pTool;
	CBuildTool * m_pOldTool;

	// The configuration this action is concerning.
	CConfigurationRecord * m_pcrOur;
};

#endif // _ENGINE_H_
