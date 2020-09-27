// ======================================================================================
// 
//  Groups.h -- group declarations for vblend.exe
//
// ======================================================================================

#ifndef _GROUPS_H_
#define _GROUPS_H_

//#include "VBlend.h"

class BlendPos : public CVBlend
{
public:
	BlendPos();
	~BlendPos();

	virtual bool CreateShader();
};

class BlendNml : public CVBlend
{
public:
	BlendNml();
	~BlendNml();

	virtual bool CreateShader();
};

class BlendClr : public CVBlend
{
public:
	BlendClr();
	~BlendClr();

	virtual bool CreateShader();
};

class BlendTxc : public CVBlend
{
public:
	BlendTxc();
	~BlendTxc();

	virtual bool CreateShader();
};

class BlendPosNml : public CVBlend
{
public:
	BlendPosNml();
	~BlendPosNml();

	virtual bool CreateShader();
};

class BlendPosClr : public CVBlend
{
public:
	BlendPosClr();
	~BlendPosClr();

	virtual bool CreateShader();
};

class BlendPosTxc : public CVBlend
{
public:
	BlendPosTxc();
	~BlendPosTxc();

	bool CreateShader();
};

class BlendPosNmlClr : public CVBlend
{
public:
	BlendPosNmlClr();
	~BlendPosNmlClr();

	virtual bool CreateShader();
};

class BlendPosNmlTxc : public CVBlend
{
public:
	BlendPosNmlTxc();
	~BlendPosNmlTxc();

	virtual bool CreateShader();
};

class BlendPosNmlClrTxc : public CVBlend
{
public:
	BlendPosNmlClrTxc();
	~BlendPosNmlClrTxc();

	virtual bool CreateShader();
};

class BlendNmlClr : public CVBlend
{
public:
	BlendNmlClr();
	~BlendNmlClr(); 

	virtual bool CreateShader();
};

class BlendNmlTxc : public CVBlend
{
public:
	BlendNmlTxc();
	~BlendNmlTxc();

	virtual bool CreateShader();
};

class BlendNmlClrTxc : public CVBlend
{
public:
	BlendNmlClrTxc();
	~BlendNmlClrTxc();

	virtual bool CreateShader();
};

class BlendClrTxc : public CVBlend
{
public:
	BlendClrTxc();
	~BlendClrTxc();

	virtual bool CreateShader();
};

#endif
