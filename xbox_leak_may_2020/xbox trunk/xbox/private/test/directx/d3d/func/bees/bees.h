/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    bees.h

Author:

    Matt Bronder

Description:

    Direct3D test functions.

*******************************************************************************/

#ifndef __BEES_H__
#define __BEES_H__

#define	NUM_BEES		1024
#define MAX_FORCE		0.02f
#define MAX_VELOCITY	0.8
#define DAMP			1.0f

typedef struct t_bee {
	int			rot;	// 0..8 index for rotation
	float		scale;
	D3DXVECTOR3	position;
	D3DXVECTOR3	velocity;
	D3DCOLOR	color;
} Bee;

//******************************************************************************
class CBees : public CScene {

protected:

    CTexture8*                      m_pd3dtSparkle;

	float		                    m_tic;

public:

                                    CBees();
                                    ~CBees();

    virtual BOOL                    Create(CDisplay* pDisplay);
    virtual int                     Exhibit(int *pnExitCode);

protected:

    virtual BOOL                    Prepare();
    virtual BOOL                    Setup();
    virtual BOOL                    Initialize();
    virtual void                    Efface();
    virtual void                    Update();
    virtual BOOL                    Render();

    virtual void                    ProcessInput();
    virtual BOOL                    InitView();

    virtual BOOL                    WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, 
                                            WPARAM wParam, LPARAM lParam);
};

#endif //__BEES_H__
