#pragma once
class Node
{
	virtual HRESULT Initialize() = 0;
	virtual HRESULT Cleanup() = 0;
	virtual HRESULT Advance( float fElapsedTime ) = 0;
	virtual HRESULT Render( DWORD flags ) { return S_OK;}
};