#include "d3dapi.hpp"

LINKME(10);

namespace d3dapi {
class TEX {
public:
	D3DTexture* m_t;

	TEX(): m_pLast(0), m_t(0) {}

	bool Init(int width, int height, int levels, D3DFORMAT format)
	{
		m_w = width; m_h = height; m_l = levels; m_f = format;
		DWORD hr;
		hr = g_pd3dd8->CreateTexture(m_w,m_h,m_l,0,m_f,D3DPOOL_DEFAULT, &m_t);
		if(hr != D3D_OK) {
			xLog(hlog, XLL_FAIL, "TEX: create texture, w%d h%d l%d f%d", m_w, m_h, m_l, m_f);
			return false;
		}
		m_pLast = new DWORD[m_l];
		if(!m_pLast) { 
			xLog(hlog, XLL_FAIL, "TEX: couldn't create dword array, size %d", m_l); 
			return false; 
		}
		m_bpp = D3D::BitsPerPixelOfD3DFORMAT(format);

		return true;
	}

	~TEX() {
		Release();
	}
	bool Release() {
		DWORD hr;
		if(m_t) {
			hr = m_t->Release();
			xLog(hlog, (hr == 0) ? XLL_PASS : XLL_FAIL, "TEX: release texture: %d", hr);
			m_t = 0;
		}
		if(m_pLast) {
			delete[] m_pLast;
			m_pLast = 0;
		}
		return true;
	}


	void Set(int layer, bool raw, D3DLOCKED_RECT* pr) {  
		int r;
		DWORD col;
		int x, y;
		int w = max(m_w >> layer, 1);
		int h = max(m_h >> layer, 1);

		if(layer < 0 || layer >= m_l) _asm {int 3};
		if(!m_t || !m_pLast) _asm {int 3};
		m_pLast[layer] = rnd();
		r = m_pLast[layer];										//store the random number so we can check the location later
		col = (r + (r << 14) + (r << 27)) >> (32 - m_bpp);
		x = (r + (r << 14) + (r << 27)) & (w - 1);
		y = (r + (r << 14) + (r << 27)) / (w);
		SetPoint((x + 0) & (w - 1), (y + 0) & (h - 1), layer, col, raw, pr);  //draw 5 points to a random location
		SetPoint((x + 1) & (w - 1), (y + 0) & (h - 1), layer, col, raw, pr);  
		SetPoint((x + 0) & (w - 1), (y + 1) & (h - 1), layer, col, raw, pr);
		SetPoint((x - 1) & (w - 1), (y + 0) & (h - 1), layer, col, raw, pr);
		SetPoint((x + 0) & (w - 1), (y - 1) & (h - 1), layer, col, raw, pr);
		SetPoint(w - 1, h - 1, layer, col, raw, pr);						  //draw to first pixel & last pixel
		SetPoint(0, 0, layer, col, raw, pr);
	}

	bool Check(int layer, bool raw, D3DLOCKED_RECT* pr) 
	{
		int r;
		DWORD col;
		int x, y;
		int w = max(m_w >> layer, 1);
		int h = max(m_h >> layer, 1);

		if(layer < 0 || layer >= m_l) _asm {int 3};
		if(!m_t || !m_pLast) _asm {int 3};

		r = m_pLast[layer];	//get the random number from last time
		col = (r + (r << 14) + (r << 27)) >> (32 - m_bpp);
		x = (r + (r << 14) + (r << 27)) & (w - 1);
		y = (r + (r << 14) + (r << 27)) / (w);
		if(!GetPoint((x + 0) & (w - 1), (y + 0) & (h - 1), layer, raw, pr, col)) goto bad;
		if(!GetPoint((x + 1) & (w - 1), (y + 0) & (h - 1), layer, raw, pr, col)) goto bad;
		if(!GetPoint((x + 0) & (w - 1), (y + 1) & (h - 1), layer, raw, pr, col)) goto bad;
		if(!GetPoint((x - 1) & (w - 1), (y + 0) & (h - 1), layer, raw, pr, col)) goto bad;
		if(!GetPoint((x + 0) & (w - 1), (y - 1) & (h - 1), layer, raw, pr, col)) goto bad;
		if(!GetPoint(w - 1, h - 1, layer, raw, pr, col)) goto bad;
		if(!GetPoint(0, 0, layer, raw, pr, col)) goto bad;
		return true;
	bad:
		return false;
	}


public: //private:
	int m_w;
	int m_h;
	int m_l;
	int m_bpp;
	D3DFORMAT m_f;
	DWORD* m_pLast;

	void SetPoint(int x, int y, int layer, DWORD col, bool raw, D3DLOCKED_RECT* pr) 
	{
		int i;
		if(raw) {
			SWIZZLER s(max(m_w >> layer, 1), max(m_h >> layer, 1), 0);
			i = s.Convert(x,y,0);
		} else {
			i = (y * (m_w >> layer)) + x;
		}
		switch (m_bpp) {
		case 4:
			i >>= 1; 
			//fall through...
		case 8:
			((BYTE*)pr->pBits)[i] = (BYTE) col;
			break;
		case 16:
			((WORD*)pr->pBits)[i] = (WORD) col;
			break;
		case 32:
			((DWORD*)pr->pBits)[i] = (DWORD) col;
			break;
		default:
			_asm {int 3};
		}
	}	
	bool GetPoint(int x, int y, int layer, bool raw, D3DLOCKED_RECT* pr, DWORD col) 
	{
		int i;
//		DWORD col;
		if(raw) {
			SWIZZLER s(max(m_w >> layer, 1), max(m_h >> layer, 1), 0);
			i = s.Convert(x,y,0);
		} else {
			i = (y * (m_w >> layer)) + x;
		}
		switch (m_bpp) {
		case 4:
			i >>= 1; 
			//fall through...
		case 8:
			if((BYTE)col == (((BYTE*)pr->pBits)[i]))
				return true;
			else
				return false;
		case 16:
			if((WORD)col == (((WORD*)pr->pBits)[i]))
				return true;
			else
				return false;
		case 32:
			if((DWORD)col == (((DWORD*)pr->pBits)[i]))
				return true;
			else
				return false;
		default:
			_asm {int 3};
		}
		return false;
	}	

};
};

#pragma data_seg(".d3dapi$test051") //after device creation, before other tests

BEGINTEST(testLock)
{
	DWORD hr;
	bool br;
	TEX t[2];
	D3DLOCKED_RECT r;

	STARTLEAKCHECK();
	xStartVariation(hlog, "lock, init");
	{
		br = t[0].Init(256, 256, 9, D3DFMT_R8G8B8A8);
		CHECKRESULT(br == true);
		if(WASBADRESULT()) goto cleanup;

		br = t[1].Init(512, 256, 2, D3DFMT_R8G8B8A8);
		CHECKRESULT(br == true);
		if(WASBADRESULT()) goto cleanup;
	}
	xEndVariation(hlog);
	xStartVariation(hlog, "lock 0L0 raw, check swizzled");
	{
		int layer = 0;

		hr = t[0].m_t->LockRect(layer, &r, NULL, D3DLOCK_RAWDATA);
		CHECKRESULT(hr == D3D_OK);
		t[0].Set(layer, true, &r);
		hr = t[0].m_t->UnlockRect(layer);
		CHECKRESULT(hr == D3D_OK);

		hr = t[0].m_t->LockRect(layer, &r, NULL, 0);
		CHECKRESULT(hr == D3D_OK);
		br = t[0].Check(layer, false, &r);
		CHECKRESULT(true == br);
		hr = t[0].m_t->UnlockRect(layer);
		CHECKRESULT(hr == D3D_OK);
	}
	xEndVariation(hlog);
	xStartVariation(hlog, "lock 0L1 swizzled, check raw");
	{
		int layer = 1;

		hr = t[0].m_t->LockRect(layer, &r, NULL, 0);
		CHECKRESULT(hr == D3D_OK);
		t[0].Set(layer, false, &r);
		hr = t[0].m_t->UnlockRect(layer);
		CHECKRESULT(hr == D3D_OK);

		hr = t[0].m_t->LockRect(layer, &r, NULL, D3DLOCK_RAWDATA);
		CHECKRESULT(hr == D3D_OK);
		br = t[0].Check(layer, true, &r);
		CHECKRESULT(true == br);
		hr = t[0].m_t->UnlockRect(layer);
		CHECKRESULT(hr == D3D_OK);
	}
	xEndVariation(hlog);
/*
	lock order        1
	0 lock type       2 
	1 lock type       4
	unlock order      8
	2nd lock order   10
	0 lock type      20
	1 lock type      40
	..
	2nd texture      80

*/
/*
	xStartVariation(hlog, "LockLockUnlockUnlock");
	for (i = 0; i < 256; i++) {
		int Lock1Type1 = (i & 0x02) ? D3DLOCK_RAWDATA : 0;
		int Lock1Type2 = (i & 0x04) ? D3DLOCK_RAWDATA : 0;
		int Lock2Type1 = (i & 0x20) ? D3DLOCK_RAWDATA : 0;
		int Lock2Type2 = (i & 0x40) ? D3DLOCK_RAWDATA : 0;
		int TexNum = (i & 0x80) ? 1 : 0;
		int LayerNum = (i & 0x80) ? 0 : 1;
		
		xLog(hlog, XLL_INFO, "Lock1 order:%d, Lock0Type:%d, Lock1Type:%d, UnlockOrder:%d", i&1,(i&2)>>1,(i&4)>>2,(i&8)>>3);
		xLog(hlog, XLL_INFO, "Lock2 order:%d, Lock0Type:%d, Lock1Type:%d, Layer/Tex", (i&16)>>4,(i&32)>>5,(i&64)>>6,(i&128)>>7);

/////Lock and set
		if(i & 1) goto lluui1_2;
lluui1_1:
		hr = t[0].m_t->LockRect(0, &r, NULL, Lock1Type1);
		CHECKRESULT(hr == D3D_OK && "Lock Tex1");
		t[0].Set(0, Lock1Type1 ? true : false, &r);
		if(i & 1) goto lluui1_3;

lluui1_2:
		hr = t[TexNum].m_t->LockRect(LayerNum, &r, NULL, Lock1Type2);
		CHECKRESULT(hr == D3D_OK && "Lock Tex2");
		t[TexNum].Set(LayerNum, Lock1Type2 ? true : false, &r);
		if(i & 1) goto lluui1_1;
lluui1_3:

//////unlock

		if(i & 8) goto lluui8_2;
lluui8_1:
		hr = t[0].m_t->UnlockRect(0);
		CHECKRESULT(hr == D3D_OK && "Unlock Tex1");
		if(i & 8) goto lluui8_3;
lluui8_2:
		hr = t[TexNum].m_t->UnlockRect(LayerNum);
		CHECKRESULT(hr == D3D_OK && "Unlock Tex2");
		if(i & 8) goto lluui8_1;
lluui8_3:


/////Lock and check
		if(i & 16) goto lluui16_2;
lluui16_1:
		hr = t[0].m_t->LockRect(0, &r, NULL, Lock2Type1);
		CHECKRESULT(hr == D3D_OK && "Lock tex1");
		br = t[0].Check(0, Lock2Type1 ? true : false, &r);
		CHECKRESULT((br == true) && "Check Tex1");
		if(i & 16) goto lluui16_3;
lluui16_2:
		hr = t[TexNum].m_t->LockRect(LayerNum, &r, NULL, Lock2Type2);
		CHECKRESULT(hr == D3D_OK && "Lock tex2");
		br = t[TexNum].Check(LayerNum, Lock2Type2 ? true : false, &r);
		CHECKRESULT((br == true) && "Check Tex2");
		if(i & 16) goto lluui16_1;
lluui16_3:

		hr = t[0].m_t->UnlockRect(0);
		CHECKRESULT(hr == D3D_OK && "unlock tex1");
		hr = t[TexNum].m_t->UnlockRect(LayerNum);
		CHECKRESULT(hr == D3D_OK && "unlock tex2");
	}
	xEndVariation(hlog);
*/
	xStartVariation(hlog, "0L0 & 0L1 swizzled, ch raw");
	{
		hr = t[0].m_t->LockRect(0, &r, NULL, 0);
		CHECKRESULT(hr == D3D_OK);
		t[0].Set(0, false, &r);

		hr = t[0].m_t->LockRect(1, &r, NULL, 0);
		CHECKRESULT(hr == D3D_OK);
		t[0].Set(1, false, &r);

		hr = t[0].m_t->UnlockRect(0);
		CHECKRESULT(hr == D3D_OK);
		hr = t[0].m_t->UnlockRect(1);
		CHECKRESULT(hr == D3D_OK);


		hr = t[0].m_t->LockRect(0, &r, NULL, D3DLOCK_RAWDATA);
		CHECKRESULT(hr == D3D_OK);
		br = t[0].Check(0, true, &r);
		CHECKRESULT(true == br);

		hr = t[0].m_t->LockRect(1, &r, NULL, D3DLOCK_RAWDATA);
		CHECKRESULT(hr == D3D_OK);
		br = t[0].Check(1, true, &r);
		CHECKRESULT(true == br);

		hr = t[0].m_t->UnlockRect(0);
		CHECKRESULT(hr == D3D_OK);
		hr = t[0].m_t->UnlockRect(1);
		CHECKRESULT(hr == D3D_OK);
	}
	xEndVariation(hlog);
	xStartVariation(hlog, "0L0 & 0L1 swizzled, ch sw");
	{
		hr = t[0].m_t->LockRect(0, &r, NULL, 0);
		CHECKRESULT(hr == D3D_OK);
		t[0].Set(0, false, &r);

		hr = t[0].m_t->LockRect(1, &r, NULL, 0);
		CHECKRESULT(hr == D3D_OK);
		t[0].Set(1, false, &r);

		hr = t[0].m_t->UnlockRect(0);
		CHECKRESULT(hr == D3D_OK);
		hr = t[0].m_t->UnlockRect(1);
		CHECKRESULT(hr == D3D_OK);


		hr = t[0].m_t->LockRect(0, &r, NULL, 0);
		CHECKRESULT(hr == D3D_OK);
		br = t[0].Check(0, false, &r);
		CHECKRESULT(true == br);

		hr = t[0].m_t->LockRect(1, &r, NULL, 0);
		CHECKRESULT(hr == D3D_OK);
		br = t[0].Check(1, false, &r);
		CHECKRESULT(true == br);

		hr = t[0].m_t->UnlockRect(0);
		CHECKRESULT(hr == D3D_OK);
		hr = t[0].m_t->UnlockRect(1);
		CHECKRESULT(hr == D3D_OK);
	}
	xEndVariation(hlog);
	xStartVariation(hlog, "0L0, 1L0 sw, ch sw");
	{
		hr = t[0].m_t->LockRect(0, &r, NULL, 0);
		CHECKRESULT(hr == D3D_OK);
		t[0].Set(0, false, &r);

		hr = t[1].m_t->LockRect(0, &r, NULL, 0);
		CHECKRESULT(hr == D3D_OK);
		t[1].Set(0, false, &r);

		hr = t[1].m_t->UnlockRect(0);
		CHECKRESULT(hr == D3D_OK);
		hr = t[0].m_t->UnlockRect(0);
		CHECKRESULT(hr == D3D_OK);


		hr = t[0].m_t->LockRect(0, &r, NULL, 0);
		CHECKRESULT(hr == D3D_OK);
		br = t[0].Check(0, false, &r);
		CHECKRESULT(true == br);

		hr = t[1].m_t->LockRect(0, &r, NULL, 0);
		CHECKRESULT(hr == D3D_OK);
		br = t[1].Check(0, false, &r);
		CHECKRESULT(true == br);

		hr = t[1].m_t->UnlockRect(0);
		CHECKRESULT(hr == D3D_OK);
		hr = t[0].m_t->UnlockRect(0);
		CHECKRESULT(hr == D3D_OK);
	}
	xEndVariation(hlog);
	xStartVariation(hlog, "0L0, 0L1, 0L9, 1L0");
	{
		hr = t[0].m_t->LockRect(0, &r, NULL, 0);
		CHECKRESULT(hr == D3D_OK);
		t[0].Set(0, false, &r);
		hr = t[0].m_t->LockRect(1, &r, NULL, 0);
		CHECKRESULT(hr == D3D_OK);
		t[0].Set(1, false, &r);
		hr = t[0].m_t->LockRect(8, &r, NULL, 0);
		CHECKRESULT(hr == D3D_OK);
		t[0].Set(8, false, &r);
		hr = t[1].m_t->LockRect(0, &r, NULL, 0);
		CHECKRESULT(hr == D3D_OK);
		t[1].Set(0, false, &r);
		hr = t[0].m_t->UnlockRect(1);
		CHECKRESULT(hr == D3D_OK);
		hr = t[0].m_t->UnlockRect(0);
		CHECKRESULT(hr == D3D_OK);
		hr = t[0].m_t->UnlockRect(8);
		CHECKRESULT(hr == D3D_OK);
		hr = t[1].m_t->UnlockRect(0);
		CHECKRESULT(hr == D3D_OK);

		hr = t[0].m_t->LockRect(8, &r, NULL, 0);
		CHECKRESULT(hr == D3D_OK);
		br = t[0].Check(8, false, &r);
		CHECKRESULT(true == br);
		
		hr = t[1].m_t->LockRect(0, &r, NULL, 0);
		CHECKRESULT(hr == D3D_OK);
		br = t[1].Check(0, false, &r);
		CHECKRESULT(true == br);
		
		hr = t[0].m_t->LockRect(0, &r, NULL, 0);
		CHECKRESULT(hr == D3D_OK);
		br = t[0].Check(0, false, &r);
		CHECKRESULT(true == br);

		hr = t[0].m_t->LockRect(1, &r, NULL, 0);
		CHECKRESULT(hr == D3D_OK);
		br = t[0].Check(1, false, &r);
		CHECKRESULT(true == br);

		hr = t[1].m_t->UnlockRect(0);
		CHECKRESULT(hr == D3D_OK);
		hr = t[0].m_t->UnlockRect(1);
		CHECKRESULT(hr == D3D_OK);
		hr = t[0].m_t->UnlockRect(8);
		CHECKRESULT(hr == D3D_OK);
		hr = t[0].m_t->UnlockRect(0);
		CHECKRESULT(hr == D3D_OK);
	}
	xEndVariation(hlog);
	xStartVariation(hlog, "Switch to big textures");
	{
		t[0].Release();
		br = t[0].Init(4096, 4096, 2, D3DFMT_A8);
		CHECKRESULT(br == true && "Init(4096 4096 2 D3DFMT_A8)");
		if(WASBADRESULT()) 
			goto cleanup;

		t[1].Release();
		br = t[1].Init(256, 256, 1, D3DFMT_LIN_R8G8B8A8);
		CHECKRESULT(br == true && "Init(256,256,1,D3DFMT_LIN_R8G8B8A8)");
		if(WASBADRESULT()) 
			goto cleanup;
	}
	xEndVariation(hlog);
	xStartVariation(hlog, "big1: 0L0 & 0L1 raw, ch raw");
	{
		hr = t[0].m_t->LockRect(0, &r, NULL, D3DLOCK_RAWDATA);
		CHECKRESULT(hr == D3D_OK);
		t[0].Set(0, true, &r);

		hr = t[0].m_t->LockRect(1, &r, NULL, D3DLOCK_RAWDATA);
		CHECKRESULT(hr == D3D_OK);
		t[0].Set(1, true, &r);

		hr = t[0].m_t->UnlockRect(0);
		CHECKRESULT(hr == D3D_OK);
		hr = t[0].m_t->UnlockRect(1);
		CHECKRESULT(hr == D3D_OK);


		hr = t[0].m_t->LockRect(0, &r, NULL, D3DLOCK_RAWDATA);
		CHECKRESULT(hr == D3D_OK);
		br = t[0].Check(0, true, &r);
		CHECKRESULT(true == br); 

		hr = t[0].m_t->LockRect(1, &r, NULL, D3DLOCK_RAWDATA);
		CHECKRESULT(hr == D3D_OK);
		br = t[0].Check(1, true, &r);
		CHECKRESULT(true == br);

		hr = t[0].m_t->UnlockRect(0);
		CHECKRESULT(hr == D3D_OK);
		hr = t[0].m_t->UnlockRect(1);
		CHECKRESULT(hr == D3D_OK);
	}
	xEndVariation(hlog);
	xStartVariation(hlog, "big: 0L0 & 0L1 swizzled, ch sw");
	{
		hr = t[0].m_t->LockRect(0, &r, NULL, 0);
		CHECKRESULT(hr == D3D_OK);
		t[0].Set(0, false, &r);

		hr = t[0].m_t->LockRect(1, &r, NULL, 0);
		CHECKRESULT(hr == D3D_OK);
		t[0].Set(1, false, &r);

		hr = t[0].m_t->UnlockRect(0);
		CHECKRESULT(hr == D3D_OK);       
		hr = t[0].m_t->UnlockRect(1); 
		CHECKRESULT(hr == D3D_OK);       


		hr = t[0].m_t->LockRect(0, &r, NULL, 0);
		CHECKRESULT(hr == D3D_OK);
		br = t[0].Check(0, false, &r);
		CHECKRESULT(true == br);

		hr = t[0].m_t->LockRect(1, &r, NULL, 0);
		CHECKRESULT(hr == D3D_OK);
		br = t[0].Check(1, false, &r);
		CHECKRESULT(true == br);

		hr = t[0].m_t->UnlockRect(0);
		CHECKRESULT(hr == D3D_OK);
		hr = t[0].m_t->UnlockRect(1);
		CHECKRESULT(hr == D3D_OK);
	}

cleanup:
	CHECKLEAKS();
	xEndVariation(hlog);

}
ENDTEST()

BEGINTEST(testLockTextureFlags)
{
	DWORD hr;
	bool br;
	TEX t[2];
	D3DLOCKED_RECT r, oldr;

	__int64 StartTime, EndTime;

	STARTLEAKCHECK();
	xStartVariation(hlog, "init");
	{
/*
		br = t[0].Init(1024, 1024, 1, D3DFMT_R8G8B8A8);	
/*/
		br = t[0].Init(1024, 1024, 1, D3DFMT_R5G6B5);	
/**/
		CHECKRESULT(br == true);
		if(WASBADRESULT()) goto cleanup;

/*
		br = t[1].Init(1024, 1024, 1, D3DFMT_LIN_R8G8B8A8);	
/*/
		br = t[1].Init(1024, 1024, 1, D3DFMT_LIN_R5G6B5);	
/**/
		CHECKRESULT(br == true);
		if(WASBADRESULT()) goto cleanup;
	}
	xEndVariation(hlog);

	xStartVariation(hlog, "SW'0");
	{
		hr = t[0].m_t->LockRect(0, &r, NULL, D3DLOCK_RAWDATA);
		oldr = r;
		CHECKRESULT(hr == D3D_OK);
		t[0].Set(0, true, &r);
		t[0].m_t->UnlockRect(0);

		hr = t[0].m_t->LockRect(0, &r, NULL, 0);
		CHECKRESULT(oldr.pBits != r.pBits);
		CHECKRESULT(hr == D3D_OK);
		br = t[0].Check(0, false, &r);
		CHECKRESULT(br == true);
		t[0].Set(0, false, &r);
		hr = t[0].m_t->UnlockRect(0);

		hr = t[0].m_t->LockRect(0, &r, NULL, D3DLOCK_RAWDATA);
		CHECKRESULT(oldr.pBits == r.pBits);
		CHECKRESULT(hr == D3D_OK);
		br = t[0].Check(0, true, &r);
		CHECKRESULT(br == true);
		t[0].m_t->UnlockRect(0);
	}
	xEndVariation(hlog);

	xStartVariation(hlog, "SW'R/O");
	{
		hr = t[0].m_t->LockRect(0, &r, NULL, D3DLOCK_RAWDATA);
		oldr = r;
		CHECKRESULT(hr == D3D_OK);
		t[0].Set(0, true, &r);
		t[0].m_t->UnlockRect(0);

		hr = t[0].m_t->LockRect(0, &r, NULL, D3DLOCK_READONLY);
		CHECKRESULT(oldr.pBits != r.pBits);
		CHECKRESULT(hr == D3D_OK);
		br = t[0].Check(0, false, &r);
		CHECKRESULT(br == true);
//		t[0].Set(0, false, &r);

		StartTime = GetUsCount(); {
			hr = t[0].m_t->UnlockRect(0);
		} EndTime = GetUsCount();
		CHECKRESULT(EndTime - StartTime < 10000); //it shouldn't take more than 10 milliseconds to unlock.
		if(WASBADRESULT()) xLog(hlog, XLL_INFO, "time to unlock: %d uS", EndTime - StartTime);

		hr = t[0].m_t->LockRect(0, &r, NULL, D3DLOCK_RAWDATA);
		CHECKRESULT(oldr.pBits == r.pBits);
		CHECKRESULT(hr == D3D_OK);
//		br = t[0].Check(0, true, &r);
//		CHECKRESULT(br == true);
		t[0].m_t->UnlockRect(0);
	}
	xEndVariation(hlog);

	xStartVariation(hlog, "SW'Dis");
	{
		hr = t[0].m_t->LockRect(0, &r, NULL, D3DLOCK_RAWDATA);
		oldr = r;
		CHECKRESULT(hr == D3D_OK);
//		t[0].Set(0, true, &r);
		t[0].m_t->UnlockRect(0);

		StartTime = GetUsCount(); {
			hr = t[0].m_t->LockRect(0, &r, NULL, D3DLOCK_DISCARD);
		} EndTime = GetUsCount();
		CHECKRESULT(EndTime - StartTime < 10000); //it shouldn't take more than 10 milliseconds to lock.
		if(WASBADRESULT()) xLog(hlog, XLL_INFO, "time to lock: %d uS", EndTime - StartTime);

		CHECKRESULT(oldr.pBits != r.pBits);
		CHECKRESULT(hr == D3D_OK);
//		br = t[0].Check(0, false, &r);
//		CHECKRESULT(br == true);
		t[0].Set(0, false, &r);
		hr = t[0].m_t->UnlockRect(0);

		hr = t[0].m_t->LockRect(0, &r, NULL, D3DLOCK_RAWDATA);
		CHECKRESULT(oldr.pBits == r.pBits);
		CHECKRESULT(hr == D3D_OK);
		br = t[0].Check(0, true, &r);
		CHECKRESULT(br == true);
		t[0].m_t->UnlockRect(0);
	}
	xEndVariation(hlog);

	xStartVariation(hlog, "SW'Raw");
	{
		hr = t[0].m_t->LockRect(0, &r, NULL, D3DLOCK_RAWDATA);
		oldr = r;
		CHECKRESULT(hr == D3D_OK);
		t[0].Set(0, true, &r);
		t[0].m_t->UnlockRect(0);

		StartTime = GetUsCount(); {
			hr = t[0].m_t->LockRect(0, &r, NULL, D3DLOCK_RAWDATA);
		} EndTime = GetUsCount();
		CHECKRESULT(EndTime - StartTime < 10000); //it shouldn't take more than 10 milliseconds to lock.
		if(WASBADRESULT()) xLog(hlog, XLL_INFO, "time to lock: %d uS", EndTime - StartTime);

		CHECKRESULT(oldr.pBits == r.pBits);
		CHECKRESULT(hr == D3D_OK);
		br = t[0].Check(0, true, &r);
		CHECKRESULT(br == true);
		t[0].Set(0, true, &r);
		StartTime = GetUsCount(); {
			hr = t[0].m_t->UnlockRect(0);
		} EndTime = GetUsCount();
		CHECKRESULT(EndTime - StartTime < 10000); //it shouldn't take more than 10 milliseconds to lock.
		if(WASBADRESULT()) xLog(hlog, XLL_INFO, "time to unlock: %d uS", EndTime - StartTime);

		hr = t[0].m_t->LockRect(0, &r, NULL, D3DLOCK_RAWDATA);
		CHECKRESULT(oldr.pBits == r.pBits);
		CHECKRESULT(hr == D3D_OK);
		br = t[0].Check(0, true, &r);
		CHECKRESULT(br == true);
		t[0].m_t->UnlockRect(0);
	}
	xEndVariation(hlog);

	xStartVariation(hlog, "SW'Raw/RO");
	{
		hr = t[0].m_t->LockRect(0, &r, NULL, D3DLOCK_RAWDATA);
		oldr = r;
		CHECKRESULT(hr == D3D_OK);
		t[0].Set(0, true, &r);
		t[0].m_t->UnlockRect(0);

		StartTime = GetUsCount(); {
			hr = t[0].m_t->LockRect(0, &r, NULL, D3DLOCK_RAWDATA | D3DLOCK_READONLY);
		} EndTime = GetUsCount();
		CHECKRESULT(EndTime - StartTime < 10000); //it shouldn't take more than 10 milliseconds to lock.
		if(WASBADRESULT()) xLog(hlog, XLL_INFO, "time to lock: %d uS", EndTime - StartTime);

		CHECKRESULT(oldr.pBits == r.pBits);
		CHECKRESULT(hr == D3D_OK);
		br = t[0].Check(0, true, &r);
		CHECKRESULT(br == true);
//		t[0].Set(0, true, &r);
		StartTime = GetUsCount(); {
			hr = t[0].m_t->UnlockRect(0);
		} EndTime = GetUsCount();
		CHECKRESULT(EndTime - StartTime < 10000); //it shouldn't take more than 10 milliseconds to lock.
		if(WASBADRESULT()) xLog(hlog, XLL_INFO, "time to unlock: %d uS", EndTime - StartTime);

//		hr = t[0].m_t->LockRect(0, &r, NULL, D3DLOCK_RAWDATA);
//		CHECKRESULT(oldr.pBits == r.pBits);
//		CHECKRESULT(hr == D3D_OK);
//		br = t[0].Check(0, true, &r);
//		CHECKRESULT(br == true);
//		t[0].m_t->UnlockRect(0);
	}
	xEndVariation(hlog);

	xStartVariation(hlog, "SW'Raw/Dis");
	{
		hr = t[0].m_t->LockRect(0, &r, NULL, D3DLOCK_RAWDATA);
		oldr = r;
		CHECKRESULT(hr == D3D_OK);
//		t[0].Set(0, true, &r);
		t[0].m_t->UnlockRect(0);

		StartTime = GetUsCount(); {
			hr = t[0].m_t->LockRect(0, &r, NULL, D3DLOCK_RAWDATA | D3DLOCK_READONLY);
		} EndTime = GetUsCount();
		CHECKRESULT(EndTime - StartTime < 10000); //it shouldn't take more than 10 milliseconds to lock.
		if(WASBADRESULT()) xLog(hlog, XLL_INFO, "time to lock: %d uS", EndTime - StartTime);

		CHECKRESULT(oldr.pBits == r.pBits);
		CHECKRESULT(hr == D3D_OK);
//		br = t[0].Check(0, true, &r);
//		CHECKRESULT(br == true);
		t[0].Set(0, true, &r);
		StartTime = GetUsCount(); {
			hr = t[0].m_t->UnlockRect(0);
		} EndTime = GetUsCount();
		CHECKRESULT(EndTime - StartTime < 10000); //it shouldn't take more than 10 milliseconds to lock.
		if(WASBADRESULT()) xLog(hlog, XLL_INFO, "time to unlock: %d uS", EndTime - StartTime);

		hr = t[0].m_t->LockRect(0, &r, NULL, D3DLOCK_RAWDATA);
		CHECKRESULT(oldr.pBits == r.pBits);
		CHECKRESULT(hr == D3D_OK);
		br = t[0].Check(0, true, &r);
		CHECKRESULT(br == true);
		t[0].m_t->UnlockRect(0);
	}
	xEndVariation(hlog);

	xStartVariation(hlog, "lin'0");
	{
		hr = t[1].m_t->LockRect(0, &r, NULL, 0);
		oldr = r;
		CHECKRESULT(hr == D3D_OK);
		t[1].Set(0, true, &r);
		t[1].m_t->UnlockRect(0);

		StartTime = GetUsCount(); {
			hr = t[1].m_t->LockRect(0, &r, NULL, 0);
		} EndTime = GetUsCount();
		CHECKRESULT(EndTime - StartTime < 10000); //it shouldn't take more than 10 milliseconds to lock.
		if(WASBADRESULT()) xLog(hlog, XLL_INFO, "time to lock: %d uS", EndTime - StartTime);

		CHECKRESULT(oldr.pBits == r.pBits);
		CHECKRESULT(hr == D3D_OK);
		br = t[1].Check(0, true, &r);
		CHECKRESULT(br == true);
		t[1].Set(0, true, &r);
		StartTime = GetUsCount(); {
			hr = t[1].m_t->UnlockRect(0);
		} EndTime = GetUsCount();
		CHECKRESULT(EndTime - StartTime < 10000); //it shouldn't take more than 10 milliseconds to lock.
		if(WASBADRESULT()) xLog(hlog, XLL_INFO, "time to unlock: %d uS", EndTime - StartTime);

		hr = t[1].m_t->LockRect(0, &r, NULL, 0);
		CHECKRESULT(oldr.pBits == r.pBits);
		CHECKRESULT(hr == D3D_OK);
		br = t[1].Check(0, true, &r);
		CHECKRESULT(br == true);
		t[1].m_t->UnlockRect(0);
	}
	xEndVariation(hlog);
	xStartVariation(hlog, "lin'Discard");
	{
		hr = t[1].m_t->LockRect(0, &r, NULL, 0);
		oldr = r;
		CHECKRESULT(hr == D3D_OK);
//		t[1].Set(0, true, &r);
		t[1].m_t->UnlockRect(0);

		StartTime = GetUsCount(); {
			hr = t[1].m_t->LockRect(0, &r, NULL, 0);
		} EndTime = GetUsCount();
		CHECKRESULT(EndTime - StartTime < 10000); //it shouldn't take more than 10 milliseconds to lock.
		if(WASBADRESULT()) xLog(hlog, XLL_INFO, "time to lock: %d uS", EndTime - StartTime);

		CHECKRESULT(oldr.pBits == r.pBits);
		CHECKRESULT(hr == D3D_OK);
//		br = t[1].Check(0, true, &r);
//		CHECKRESULT(br == true);
		t[1].Set(0, true, &r);
		StartTime = GetUsCount(); {
			hr = t[1].m_t->UnlockRect(0);
		} EndTime = GetUsCount();
		CHECKRESULT(EndTime - StartTime < 10000); //it shouldn't take more than 10 milliseconds to lock.
		if(WASBADRESULT()) xLog(hlog, XLL_INFO, "time to unlock: %d uS", EndTime - StartTime);

		hr = t[1].m_t->LockRect(0, &r, NULL, 0);
		CHECKRESULT(oldr.pBits == r.pBits);
		CHECKRESULT(hr == D3D_OK);
		br = t[1].Check(0, true, &r);
		CHECKRESULT(br == true);
		t[1].m_t->UnlockRect(0);
	}
	xEndVariation(hlog);
	xStartVariation(hlog, "lin'RO");
	{
		hr = t[1].m_t->LockRect(0, &r, NULL, 0);
		oldr = r;
		CHECKRESULT(hr == D3D_OK);
		t[1].Set(0, true, &r);
		t[1].m_t->UnlockRect(0);

		StartTime = GetUsCount(); {
			hr = t[1].m_t->LockRect(0, &r, NULL, D3DLOCK_READONLY);
		} EndTime = GetUsCount();
		CHECKRESULT(EndTime - StartTime < 10000); //it shouldn't take more than 10 milliseconds to lock.
		if(WASBADRESULT()) xLog(hlog, XLL_INFO, "time to lock: %d uS", EndTime - StartTime);

		CHECKRESULT(oldr.pBits == r.pBits);
		CHECKRESULT(hr == D3D_OK);
		br = t[1].Check(0, true, &r);
		CHECKRESULT(br == true);
//		t[1].Set(0, true, &r);
		StartTime = GetUsCount(); {
			hr = t[1].m_t->UnlockRect(0);
		} EndTime = GetUsCount();
		CHECKRESULT(EndTime - StartTime < 10000); //it shouldn't take more than 10 milliseconds to lock.
		if(WASBADRESULT()) xLog(hlog, XLL_INFO, "time to unlock: %d uS", EndTime - StartTime);

		hr = t[1].m_t->LockRect(0, &r, NULL, 0);
		CHECKRESULT(oldr.pBits == r.pBits);
		CHECKRESULT(hr == D3D_OK);
//		br = t[1].Check(0, true, &r);
//		CHECKRESULT(br == true);
		t[1].m_t->UnlockRect(0);
	}
	xEndVariation(hlog);

cleanup:
	xStartVariation(hlog, "Cleanup");
	{
		t[0].Release();
		t[1].Release();
		CHECKLEAKS();
	}
	xEndVariation(hlog);
}
ENDTEST()



BEGINTEST(testUpdateTexture)
{
	DWORD hr;
	bool br;
	TEX t[2];
	D3DLOCKED_RECT r;
	int i, j, j2;

	struct {
		int w1;
		int h1;
		int l1;
		int w2;
		int h2;
		int l2;
		D3DFORMAT format;
	} tests[] = {
		{2048, 2048,  2, 1024, 1024,  1,D3DFMT_A8          },
		{ 512,  512, 10,    1,    1,  1,D3DFMT_A8          },
		{   2,    2,  2,    1,    1,  1,D3DFMT_A8          },
		{ 256,  512, 10,  256,  512, 10,D3DFMT_A8          },
		{1024,  512,  7,  512,  256,  6,D3DFMT_A8          },
		{   1,  512,  1,    1,  512,  1,D3DFMT_LIN_A8      },
		{ 512,  512, 10,    1,    1,  1,D3DFMT_R5G6B5      },
		{   2,    2,  2,    1,    1,  1,D3DFMT_R5G6B5      },
		{ 256,  512, 10,  256,  512, 10,D3DFMT_R5G6B5      },
		{ 256,  512,  1,  256,  512,  1,D3DFMT_LIN_R5G6B5  },
		{ 512,  512, 10,    1,    1,  1,D3DFMT_R8G8B8A8    },
		{   2,    2,  2,    1,    1,  1,D3DFMT_R8G8B8A8    },
		{ 256,  512, 10,  256,  512, 10,D3DFMT_R8G8B8A8    },
		{   1,  512,  1,    1,  512,  1,D3DFMT_LIN_R8G8B8A8},
		{1024,  512,  7,  512,  256,  6,D3DFMT_DXT1        },
	};

	STARTLEAKCHECK();
	for(i = 0; i < COUNTOF(tests); i++) {
		xStartVariation(hlog, "lock, new texture");
		{
			t[0].Release();
			t[1].Release();

			br = t[0].Init(tests[i].w1, tests[i].h1, tests[i].l1, tests[i].format);
			CHECKRESULT(br == true);
			if(WASBADRESULT()) goto cleanup;

			br = t[1].Init(tests[i].w2, tests[i].h2, tests[i].l2, tests[i].format);
			CHECKRESULT(br == true);
			if(WASBADRESULT()) goto cleanup;

			for(j2 = 0; j2 < tests[i].l2; j2++) {
				hr = t[1].m_t->LockRect(j2, &r, NULL, D3DLOCK_RAWDATA);
				CHECKRESULT(hr == D3D_OK && "Lock");
				t[1].Set(j2, true, &r);
				hr = t[1].m_t->UnlockRect(j2);
				CHECKRESULT(hr == D3D_OK && "Unlock");
			}
			for(j = 0; j < tests[i].l1; j++) {
				hr = t[0].m_t->LockRect(j, &r, NULL, D3DLOCK_RAWDATA);
				CHECKRESULT(hr == D3D_OK && "Lock");
				t[0].Set(j, true, &r);
				hr = t[0].m_t->UnlockRect(j);
				CHECKRESULT(hr == D3D_OK && "Unlock");
				j2 = (j + tests[i].l2 - tests[i].l1);
				if(j2 >= 0 && j2 < tests[i].l2) {
					t[1].m_pLast[j2] = t[0].m_pLast[j];
				}
			}
			hr = g_pd3dd8->UpdateTexture(t[0].m_t, t[1].m_t);
			CHECKRESULT(hr == D3D_OK && "Update");

			for(j2 = 0; j2 < tests[i].l2; j2++) {
				hr = t[1].m_t->LockRect(j2, &r, NULL, D3DLOCK_RAWDATA);
				CHECKRESULT(hr == D3D_OK && "Lock");
				br = t[1].Check(j2, true, &r);
				CHECKRESULT(br == true && "CHECK!");
				hr = t[1].m_t->UnlockRect(j2);
				CHECKRESULT(hr == D3D_OK && "Unlock");
			}
		}
		xEndVariation(hlog);
	}	
	goto aftercleanup;

cleanup:
	xEndVariation(hlog);
aftercleanup:;
	xStartVariation(hlog, "leak check");
	t[0].Release();
	t[1].Release();
	CHECKLEAKS();
	xEndVariation(hlog);

}
ENDTEST()

BEGINTEST(testUmaTexture)
{
	DWORD hr;
	bool br;
	TEX t[6];
	D3DLOCKED_RECT r;

	STARTLEAKCHECK();
	xStartVariation(hlog, "lock, init");
	{
		br = t[0].Init(1, 1, 1, D3DFMT_A8);					//1 byte 
		CHECKRESULT(br == true);
		if(WASBADRESULT()) goto cleanup;				
		br = t[1].Init(512, 16, 1, D3DFMT_R5G6B5);			//8k (a boundry condition)
		CHECKRESULT(br == true);
		if(WASBADRESULT()) goto cleanup;
		br = t[2].Init(4096, 4096, 1, D3DFMT_A8);			//16mb, exactly
		CHECKRESULT(br == true);
		if(WASBADRESULT()) goto cleanup;
		br = t[3].Init(4082, 2, 1, D3DFMT_LIN_A8);		    //next step below 8k
		CHECKRESULT(br == true);
		if(WASBADRESULT()) goto cleanup;
		br = t[4].Init(256, 256, 9, D3DFMT_R8G8B8A8);		//abt 300k
		CHECKRESULT(br == true);
		if(WASBADRESULT()) goto cleanup;
		br = t[5].Init(2048, 2048, 9, D3DFMT_R8G8B8A8);		//22mb
		CHECKRESULT(br == true);
		if(WASBADRESULT()) goto cleanup;

	}
	xEndVariation(hlog);
	xStartVariation(hlog, "");
	{
		int location, locktype, layer, i;

		for(i = 0; i <= LASTOF(t); i++) {
		for(layer = 0; layer <= (i / 4); layer++) {
		for(locktype = 0; locktype <= 1; locktype++) {
		for(location = D3DMEM_AGP; location <= D3DMEM_VIDEO; location++) {
			xLog(hlog, XLL_INFO, "Width: %d, Height: %d, Layer: %d, locktype: %d, loc: %d", t[i].m_w, t[i].m_h, layer, locktype, location);
			t[i].m_t->MoveResourceMemory((D3DMEMORY)location);

			hr = t[i].m_t->LockRect(layer, &r, NULL, locktype ? D3DLOCK_RAWDATA : 0);
			CHECKRESULT(hr == D3D_OK && "Lock");
			t[i].Set(layer, locktype ? true : false, &r);
			hr = t[i].m_t->UnlockRect(layer);
			CHECKRESULT(hr == D3D_OK && "Unlock");

			t[i].m_t->MoveResourceMemory((D3DMEMORY)(location ^ D3DMEM_AGP ^ D3DMEM_VIDEO));

			hr = t[i].m_t->LockRect(layer, &r, NULL, locktype ? D3DLOCK_RAWDATA : 0);
			CHECKRESULT(hr == D3D_OK && "Lock");
			br = t[i].Check(layer, locktype ? true : false, &r);
			CHECKRESULT(true == br && "Check");
			hr = t[i].m_t->UnlockRect(layer);
			CHECKRESULT(hr == D3D_OK && "Unlock");
		}
		}
		}
		}
	}
	xEndVariation(hlog);
	xStartVariation(hlog, "deinit");
	{
cleanup:
		t[0].Release();
		t[1].Release();
		t[2].Release();
		t[3].Release();
		t[4].Release();
		t[5].Release();
		CHECKLEAKS();
	}
}
ENDTEST()
#pragma data_seg()

