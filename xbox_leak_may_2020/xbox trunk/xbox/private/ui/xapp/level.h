#pragma once

class CLevel : public CGroup
{
	DECLARE_NODE(CLevel, CGroup)
public:
	CLevel();
	~CLevel();

	CNode* m_control;	// a joystick
	CNode* m_tunnel;	// the tunnel that leads here
	CNode* m_path;		// the path through the tunnel
	CNode* m_shell;		// the sphere around the level
	TCHAR* m_archive;
	bool m_unloadable;
    bool m_fade;

	// NYI: path for the tunnel
	// NYI: ambient music controls

	float m_timeToNextLevel;

	void GoTo();
	void GoBackTo();

	void Advance(float nSeconds);
	void Render();

protected:
	void Activate();
	void Deactivate();

	class CXipFile* m_pXipFile;
	bool m_bArrive;

	DECLARE_NODE_PROPS()
	DECLARE_NODE_FUNCTIONS()
};
