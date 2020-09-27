#include "stdafx.h"
#include "3dobject.h"

float divPI = 0.01745f;


// Constructor
C3DObject::C3DObject( void )
{
}


// Desctructor
C3DObject::~C3DObject( void )
{
}


void C3DObject::translate( float x, float y, float z )
{
	m_TranslateVector = m_TranslateVector + D3DXVECTOR4( x, y, z, 0.0f );
}


void C3DObject::rotateX( float x )
{
	D3DXMatrixRotationX( &m_WorkMatrix, x * divPI );

	m_ObjectMatrix = m_WorkMatrix * m_ObjectMatrix;
	D3DXVec4Transform( &m_TranslateVector, &m_TranslateVector, &m_WorkMatrix );
}


void C3DObject::rotateY( float y )
{
	D3DXMatrixRotationY( &m_WorkMatrix, y * divPI );

	m_ObjectMatrix = m_WorkMatrix * m_ObjectMatrix;
	D3DXVec4Transform( &m_TranslateVector, &m_TranslateVector, &m_WorkMatrix );
}


void C3DObject::rotateZ( float z )
{
	D3DXMatrixRotationZ( &m_WorkMatrix, z * divPI );

	m_ObjectMatrix = m_WorkMatrix * m_ObjectMatrix;
	D3DXVec4Transform( &m_TranslateVector, &m_TranslateVector, &m_WorkMatrix );
}


void C3DObject::spinX( float x )
{
	D3DXMatrixRotationX( &m_WorkMatrix, x * divPI );

	m_ObjectMatrix = m_WorkMatrix * m_ObjectMatrix;
}


void C3DObject::spinY( float y )
{
	D3DXMatrixRotationY( &m_WorkMatrix, y * divPI );

	m_ObjectMatrix = m_WorkMatrix * m_ObjectMatrix;
}


void C3DObject::spinZ( float z )
{
	D3DXMatrixRotationZ( &m_WorkMatrix, z * divPI );

	m_ObjectMatrix = m_WorkMatrix * m_ObjectMatrix;
}


void C3DObject::localRotateX( float x )
{
	D3DXMatrixRotationX( &m_WorkMatrix, x * divPI );

	m_ObjectMatrix = m_ObjectMatrix * m_WorkMatrix;
}


void C3DObject::localRotateY( float y )
{
	D3DXMatrixRotationY( &m_WorkMatrix, y * divPI );

	m_ObjectMatrix = m_ObjectMatrix * m_WorkMatrix;
}


void C3DObject::localRotateZ( float z )
{
	D3DXMatrixRotationZ( &m_WorkMatrix, z * divPI );

	m_ObjectMatrix = m_ObjectMatrix * m_WorkMatrix;
}

