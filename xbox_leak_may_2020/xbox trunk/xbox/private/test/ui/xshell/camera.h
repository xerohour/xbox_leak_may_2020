#ifndef _CAMERA_H_
#define _CAMERA_H_

class Camera
{

public:
    // Constructors and Destructors
    Camera(void);
    ~Camera(void);

    // Public Properties
	D3DXVECTOR3   m_CameraBase;
	D3DXVECTOR3   m_CameraTarget;
	float		  m_fFOV;
	D3DXVECTOR3   m_Axis;
	float         m_Angle;
	
	D3DXMATRIX    m_MatView;
	D3DXMATRIX    m_MatProj;
	D3DXMATRIX    m_ObjectMatrix;

	// Noise variables
	int			  m_iXframeCount[2];
	int			  m_iYframeCount[2];
	int			  m_iZframeCount[2];
	int			  m_iXframeDuration[2];
	int			  m_iYframeDuration[2];
	int			  m_iZframeDuration[2];
	D3DXVECTOR3   m_PreviousPos[2];
	D3DXVECTOR3   m_NewPos[2];
    float         m_fXinc[2], m_fYinc[2], m_fZinc[2];

	// Public Methods
	void update( IDirect3DDevice8* pd3dDevice );
	void noise( IDirect3DDevice8* pd3dDevice );
	void readCAM( char* pszFilename );
    void SetWidescreen( BOOL bValue );
};

#endif // _CAMERA_H_