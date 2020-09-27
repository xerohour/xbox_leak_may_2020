#include "d3dapi.hpp"

LINKME(12);
typedef struct {
	int texnum1;
	int layer1;
	bool doraw1;
	int texnum2;
	int layer2;
	bool doraw2;
} VOLUMELOCKTESTSTRUCT;

namespace d3dapi {
class VTEX {
public:
	D3DVolumeTexture* m_t;

	VTEX():m_t(0), m_pLast(0) {}

	bool Init(int width, int height, int depth, int levels, D3DFORMAT format)
	{
		m_w = width; m_h = height; m_d = depth; m_l = levels; m_f = format;
		DWORD hr;
		hr = g_pd3dd8->CreateVolumeTexture(m_w,m_h,m_d,m_l,0,m_f,D3DPOOL_DEFAULT, &m_t);
		if(hr != D3D_OK) {
			xLog(hlog, XLL_FAIL, "VTEX: create volume texture, w%d h%d d%d l%d f%d", m_w, m_h, m_d, m_l, m_f);
			return false;
		}
		m_pLast = new DWORD[m_l];
		if(!m_pLast) { 
			xLog(hlog, XLL_FAIL, "VTEX: couldn't create dword array, size %d", m_l); 
			return false; 
		}
		m_bpp = D3D::BitsPerPixelOfD3DFORMAT(format);

		return true;
	}

	~VTEX() {
		Release();
	}
	bool Release() {
		DWORD hr;
		if(m_t) {
			hr = m_t->Release();
			xLog(hlog, (hr == 0) ? XLL_PASS : XLL_FAIL, "VTEX: release volume texture: %d", hr);
			m_t = 0;
		}
		if(m_pLast) {
			delete[] m_pLast;
			m_pLast = 0;
		}
		return true;
	}

	void Set(int layer, bool raw, D3DLOCKED_BOX* pr) {  
		int r;
		DWORD col;
		int x, y, z;
		int w = max(m_w >> layer, 1);
		int h = max(m_h >> layer, 1);
		int d = max(m_d >> layer, 1);

		if(layer < 0 || layer >= m_l) _asm {int 3};
		if(!m_t || !m_pLast) _asm {int 3};
		m_pLast[layer] = rnd();
		r = m_pLast[layer];							//store the random number so we can check the location later
		col = (r + (r << 14) + (r << 27)) >> (32 - m_bpp);
		x = (r + (r << 14) + (r << 27)) & (w - 1);
		y = (r + (r << 14) + (r << 27)) / (w);
		z = (r + (r << 14) + (r << 27)) / (w * h); 
		SetPoint((x + 0) & (w - 1), (y + 0) & (h - 1), (z + 0) & (d - 1), layer, col, raw, pr);  //draw 5 points to a random location
		SetPoint((x + 1) & (w - 1), (y + 0) & (h - 1), (z + 0) & (d - 1), layer, col, raw, pr);  //draw 5 points to a random location
		SetPoint((x + 0) & (w - 1), (y + 1) & (h - 1), (z + 0) & (d - 1), layer, col, raw, pr);  //draw 5 points to a random location
		SetPoint((x + 0) & (w - 1), (y + 0) & (h - 1), (z + 1) & (d - 1), layer, col, raw, pr);  //draw 5 points to a random location
		SetPoint((x - 1) & (w - 1), (y + 0) & (h - 1), (z + 0) & (d - 1), layer, col, raw, pr);  //draw 5 points to a random location
		SetPoint((x + 0) & (w - 1), (y - 1) & (h - 1), (z + 0) & (d - 1), layer, col, raw, pr);  //draw 5 points to a random location
		SetPoint((x + 0) & (w - 1), (y + 0) & (h - 1), (z - 1) & (d - 1), layer, col, raw, pr);  //draw 5 points to a random location
		SetPoint(w - 1, h - 1, d - 1, layer, col, raw, pr);						  //draw to first pixel & last pixel
		SetPoint(0, 0, 0, layer, col, raw, pr);
	}

	bool Check(int layer, bool raw, D3DLOCKED_BOX* pr) 
	{
		int r;
		DWORD col;
		int x, y, z;
		int w = max(m_w >> layer, 1);
		int h = max(m_h >> layer, 1);
		int d = max(m_d >> layer, 1);

		if(layer < 0 || layer >= m_l) _asm {int 3};
		if(!m_t || !m_pLast) _asm {int 3};

		r = m_pLast[layer];	//get the random number from last time
		col = (r + (r << 14) + (r << 27)) >> (32 - m_bpp);
		x = (r + (r << 14) + (r << 27)) & (w - 1);
		y = (r + (r << 14) + (r << 27)) / (w);
		z = (r + (r << 14) + (r << 27)) / (w * h); 
		if(!GetPoint((x + 0) & (w - 1), (y + 0) & (h - 1), (z + 0) & (d - 1), layer, raw, pr, col)) goto bad;
		if(!GetPoint((x + 1) & (w - 1), (y + 0) & (h - 1), (z + 0) & (d - 1), layer, raw, pr, col)) goto bad;
		if(!GetPoint((x + 0) & (w - 1), (y + 1) & (h - 1), (z + 0) & (d - 1), layer, raw, pr, col)) goto bad;
		if(!GetPoint((x + 0) & (w - 1), (y + 0) & (h - 1), (z + 1) & (d - 1), layer, raw, pr, col)) goto bad;
		if(!GetPoint((x - 1) & (w - 1), (y + 0) & (h - 1), (z + 0) & (d - 1), layer, raw, pr, col)) goto bad;
		if(!GetPoint((x + 0) & (w - 1), (y - 1) & (h - 1), (z + 0) & (d - 1), layer, raw, pr, col)) goto bad;
		if(!GetPoint((x + 0) & (w - 1), (y + 0) & (h - 1), (z - 1) & (d - 1), layer, raw, pr, col)) goto bad;
		if(!GetPoint(w - 1, h - 1, d - 1, layer, raw, pr, col)) goto bad;
		if(!GetPoint(0, 0, 0, layer, raw, pr, col)) goto bad;
		return true;
	bad:
		return false;
	}


public: //private:
	int m_w;
	int m_h;
	int m_d;
	int m_l;
	int m_bpp;
	D3DFORMAT m_f;
	DWORD* m_pLast;

	void SetPoint(int x, int y, int z, int layer, DWORD col, bool raw, D3DLOCKED_BOX* pr) 
	{
		int i;
		if(raw) {
			SWIZZLER s(max(m_w >> layer, 1), max(m_h >> layer, 1), max(m_d >> layer, 1));
			i = s.Convert(x,y,z);
		} else {
			i = (z * (max(m_h >> layer, 1)) * (max(m_w >> layer, 1))) + (y * (max(m_w >> layer, 1))) + x;
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
	bool GetPoint(int x, int y, int z, int layer, bool raw, D3DLOCKED_BOX* pr, DWORD col) 
	{
		int i;
//		DWORD col;
		if(raw) {
			SWIZZLER s(max(m_w >> layer, 1), max(m_h >> layer, 1), max(m_d >> layer, 1));
			i = s.Convert(x,y,z);
		} else {
			i = (z * (max(m_h >> layer, 1)) * (max(m_w >> layer, 1))) + (y * (max(m_w >> layer, 1))) + x;
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

#pragma data_seg(".d3dapi$test053") //
BEGINTEST(testVolLock)
{
	DWORD hr;
	bool br;
	VTEX t[2];
	D3DLOCKED_BOX r;
	int i;

	STARTLEAKCHECK();
	xStartVariation(hlog, "Volumelock, init");
	{
		br = t[0].Init(128, 128, 128, 8, D3DFMT_R8G8B8A8);
		CHECKRESULT(br == true);
		if(WASBADRESULT()) goto cleanup;

		br = t[1].Init(512, 16, 256, 10, D3DFMT_R5G6B5);
		CHECKRESULT(br == true);
		if(WASBADRESULT()) goto cleanup;
	}
	xEndVariation(hlog);
	xStartVariation(hlog, "multi-layer locks");
	{
		VOLUMELOCKTESTSTRUCT tests [] = {
			{0, 0, false, 1, 0, false},
			{0, 1, false, 0, 0, false},
			{0, 7, false, 1, 9, false},
			{0, 0, true,  1, 0, true},
			{1, 1, true,  1, 0, false},
			{1, 0, false, 1, 9, true},
		};

		for(i = 0; i < COUNTOF(tests); i++) {
			xLog(hlog, XLL_INFO, "cubenum%d, layer%d, as%d,  cubenum%d, layer%d, as%d", 
				tests[i].texnum1, tests[i].layer1, tests[i].doraw1,
				tests[i].texnum2, tests[i].layer2, tests[i].doraw2);
			hr = t[tests[i].texnum1].m_t->LockBox(tests[i].layer1, &r, NULL, tests[i].doraw1?D3DLOCK_RAWDATA:0); 
			CHECKRESULT(hr == D3D_OK && "Lock1");
			t[tests[i].texnum1].Set(tests[i].layer1, tests[i].doraw1, &r);
			hr = t[tests[i].texnum2].m_t->LockBox(tests[i].layer2, &r, NULL, tests[i].doraw2?D3DLOCK_RAWDATA:0); 
			CHECKRESULT(hr == D3D_OK && "Lock2");
			t[tests[i].texnum2].Set(tests[i].layer2, tests[i].doraw2, &r);

			hr = t[tests[i].texnum1].m_t->UnlockBox(tests[i].layer1);
			CHECKRESULT(hr == D3D_OK && "Unlock1");       
			hr = t[tests[i].texnum2].m_t->UnlockBox(tests[i].layer2);
			CHECKRESULT(hr == D3D_OK && "Unlock2");       

			hr = t[tests[i].texnum1].m_t->LockBox(tests[i].layer1, &r, NULL, tests[i].doraw1?D3DLOCK_RAWDATA:0); 
			CHECKRESULT(hr == D3D_OK && "Lock1 again");
			br = t[tests[i].texnum1].Check(tests[i].layer1, tests[i].doraw1, &r);
			CHECKRESULT(true == br && "check for set pixels");         
			hr = t[tests[i].texnum1].m_t->UnlockBox(tests[i].layer1);
			CHECKRESULT(hr == D3D_OK && "Unlock1");       

			hr = t[tests[i].texnum2].m_t->LockBox(tests[i].layer2, &r, NULL, tests[i].doraw2?D3DLOCK_RAWDATA:0); 
			CHECKRESULT(hr == D3D_OK && "Lock2 again");
			br = t[tests[i].texnum2].Check(tests[i].layer2, tests[i].doraw2, &r);
			CHECKRESULT(true == br && "check for set pixels");         
			hr = t[tests[i].texnum2].m_t->UnlockBox(tests[i].layer2);
			CHECKRESULT(hr == D3D_OK && "Unlock2");       
		}
	}
	xEndVariation(hlog);
	xStartVariation(hlog, "volume:makebig");
	{
		t[0].Release();
		br = t[0].Init(128,64,512, 10, D3DFMT_A8R8G8B8);
		CHECKRESULT(br == true);
		if(WASBADRESULT()) goto cleanup;

		t[1].Release();
		br = t[1].Init(512,128,64, 10, D3DFMT_A8R8G8B8);
		CHECKRESULT(br == true);
		if(WASBADRESULT()) goto cleanup;
	}
	xEndVariation(hlog);
	xStartVariation(hlog, "multi-layer locks2");
	{
		VOLUMELOCKTESTSTRUCT tests [] = {
			{0, 0, false, 1, 0, true},
			{0, 1, false, 0, 0, true},
			{0, 3, false, 1, 1, false},
			{0, 8, false, 0, 1, true},
			{0, 9, false, 1, 9, false},
		};

		for(i = 0; i < COUNTOF(tests); i++) {
			xLog(hlog, XLL_INFO, "cubenum%d, layer%d, as%d,  cubenum%d, layer%d, as%d", 
				tests[i].texnum1, tests[i].layer1, tests[i].doraw1,
				tests[i].texnum2, tests[i].layer2, tests[i].doraw2);
			hr = t[tests[i].texnum1].m_t->LockBox(tests[i].layer1, &r, NULL, tests[i].doraw1?D3DLOCK_RAWDATA:0); 
			CHECKRESULT(hr == D3D_OK && "Lock1");
			t[tests[i].texnum1].Set(tests[i].layer1, tests[i].doraw1, &r);
			hr = t[tests[i].texnum2].m_t->LockBox(tests[i].layer2, &r, NULL, tests[i].doraw2?D3DLOCK_RAWDATA:0); 
			CHECKRESULT(hr == D3D_OK && "Lock2");
			t[tests[i].texnum2].Set(tests[i].layer2, tests[i].doraw2, &r);

			hr = t[tests[i].texnum1].m_t->UnlockBox(tests[i].layer1);
			CHECKRESULT(hr == D3D_OK && "Unlock1");       
			hr = t[tests[i].texnum2].m_t->UnlockBox(tests[i].layer2);
			CHECKRESULT(hr == D3D_OK && "Unlock2");       

			hr = t[tests[i].texnum1].m_t->LockBox(tests[i].layer1, &r, NULL, tests[i].doraw1?D3DLOCK_RAWDATA:0); 
			CHECKRESULT(hr == D3D_OK && "Lock1 again");
			br = t[tests[i].texnum1].Check(tests[i].layer1, tests[i].doraw1, &r);
			CHECKRESULT(true == br && "check for set pixels");         
			hr = t[tests[i].texnum1].m_t->UnlockBox(tests[i].layer1);
			CHECKRESULT(hr == D3D_OK && "Unlock1");       

			hr = t[tests[i].texnum2].m_t->LockBox(tests[i].layer2, &r, NULL, tests[i].doraw2?D3DLOCK_RAWDATA:0); 
			CHECKRESULT(hr == D3D_OK && "Lock2 again");
			br = t[tests[i].texnum2].Check(tests[i].layer2, tests[i].doraw2, &r);
			CHECKRESULT(true == br && "check for set pixels");         
			hr = t[tests[i].texnum2].m_t->UnlockBox(tests[i].layer2);
			CHECKRESULT(hr == D3D_OK && "Unlock2");       
		}
	}
	xEndVariation(hlog);
	xStartVariation(hlog, "D3DVOLUME lock");
	{
		D3DVolume *v;
		t[1].Release();
		br = t[1].Init(128, 128, 256, 2, D3DFMT_A8);
		CHECKRESULT(br == true);
		if(WASBADRESULT()) goto cleanup;

		hr = t[0].m_t->LockBox(1, &r, NULL, D3DLOCK_RAWDATA); 
		CHECKRESULT(hr == D3D_OK && "Lock1");
		t[0].Set(1, true, &r);

		hr = t[1].m_t->GetVolumeLevel(0, &v);
		CHECKRESULT(hr == D3D_OK && "GetVolume");
		hr = v->LockBox(&r, NULL, D3DLOCK_RAWDATA); 
		CHECKRESULT(hr == D3D_OK && "Lock2");
		t[1].Set(0, true, &r);

		hr = t[0].m_t->UnlockBox(1);
		CHECKRESULT(hr == D3D_OK && "Unlock1");       
		hr = v->UnlockBox();
		CHECKRESULT(hr == D3D_OK && "Unlock2");       
		v->Release();


		
		hr = t[1].m_t->LockBox(0, &r, NULL, D3DLOCK_RAWDATA); 
		CHECKRESULT(hr == D3D_OK && "Lock1");
		br = t[1].Check(0, true, &r);
		CHECKRESULT(true == br && "check for set pixels1");         

		hr = t[0].m_t->GetVolumeLevel(1, &v);
		CHECKRESULT(hr == D3D_OK && "GetVolume");
		hr = v->LockBox(&r, NULL, 0); 
		CHECKRESULT(hr == D3D_OK && "Lock2");
		br = t[0].Check(1, false, &r);
		CHECKRESULT(true == br && "check for set pixels2");         

		hr = t[1].m_t->UnlockBox(0);
		CHECKRESULT(hr == D3D_OK && "Unlock1");       
		hr = v->UnlockBox();
		CHECKRESULT(hr == D3D_OK && "Unlock2");       
		v->Release();
		
	}
	xEndVariation(hlog);
	xStartVariation(hlog, "cube:maketiny");
	{
		t[1].Release();
		br = t[1].Init(1, 1, 2, 2, D3DFMT_A8);
		CHECKRESULT(br == true);
		if(WASBADRESULT()) goto cleanup;
	}
	xEndVariation(hlog);
	xStartVariation(hlog, "tinycheck");
	{
		int layer;
		for(layer = 0; layer < 2; layer++) {
			hr = t[0].m_t->LockBox(layer, &r, NULL, D3DLOCK_RAWDATA);
			CHECKRESULT(hr == D3D_OK);
			t[0].Set(layer, true, &r);
			hr = t[0].m_t->UnlockBox(layer);
			CHECKRESULT(hr == D3D_OK);

			hr = t[0].m_t->LockBox(layer, &r, NULL, 0);
			CHECKRESULT(hr == D3D_OK);
			br = t[0].Check(layer, false, &r);
			CHECKRESULT(true == br);
			hr = t[0].m_t->UnlockBox(layer);
			CHECKRESULT(hr == D3D_OK);
		}
	}

cleanup:
	CHECKLEAKS();
	xEndVariation(hlog);

}
ENDTEST()

BEGINTEST(testVolumeUpdateTexture)
{
	DWORD hr;
	bool br;
	VTEX t[2];
	D3DLOCKED_BOX r;
	int i, j, j2;

	struct {
		int w1;
		int h1;
		int d1;
		int l1;
		int w2;
		int h2;
		int d2;
		int l2;
		D3DFORMAT format;
	} tests[] = {
		{256, 256, 256,  9, 256, 256, 256,  9, D3DFMT_A8          },
		{256, 256, 128,  9, 256, 256, 128,  9, D3DFMT_R5G6B5      },
		{  1, 256, 256,  9,   1, 256, 256,  9, D3DFMT_R5G6B5      },
		{256,   1, 256,  9, 256,   1, 256,  9, D3DFMT_R5G6B5      },
		{256, 256,   1,  9, 256, 256,   1,  9, D3DFMT_R5G6B5      },
		{256, 256, 256,  3, 128, 128, 128,  2, D3DFMT_R5G6B5      },
		{256,   8,  64,  9,   1,   1,   1,  1, D3DFMT_R5G6B5      },
		{ 64,  64,  64,  7,  64,  64,  64,  7, D3DFMT_R8G8B8A8    },
//		{512, 512, 256,  8,   4,   4,   4,  1, D3DFMT_DXT1        },
	};

	STARTLEAKCHECK();
	for(i = 0; i < COUNTOF(tests); i++) {
		xStartVariation(hlog, "lock, new texture");
		{
			t[0].Release();
			t[1].Release();

			br = t[0].Init(tests[i].w1, tests[i].h1, tests[i].d1, tests[i].l1, tests[i].format);
			CHECKRESULT(br == true);
			if(WASBADRESULT()) goto cleanup;

			br = t[1].Init(tests[i].w2, tests[i].h2, tests[i].d2, tests[i].l2, tests[i].format);
			CHECKRESULT(br == true);
			if(WASBADRESULT()) goto cleanup;

			for(j2 = 0; j2 < tests[i].l2; j2++) {
				hr = t[1].m_t->LockBox(j2, &r, NULL, D3DLOCK_RAWDATA);
				CHECKRESULT(hr == D3D_OK && "Lock");
				t[1].Set(j2, true, &r);
				hr = t[1].m_t->UnlockBox(j2);
				CHECKRESULT(hr == D3D_OK && "Unlock");
			}
			for(j = 0; j < tests[i].l1; j++) {
				hr = t[0].m_t->LockBox(j, &r, NULL, D3DLOCK_RAWDATA);
				CHECKRESULT(hr == D3D_OK && "Lock");
				t[0].Set(j, true, &r);
				hr = t[0].m_t->UnlockBox(j);
				CHECKRESULT(hr == D3D_OK && "Unlock");
				j2 = (j + tests[i].l2 - tests[i].l1);
				if(j2 >= 0 && j2 < tests[i].l2) {
					t[1].m_pLast[j2] = t[0].m_pLast[j];
				}
			}
			hr = g_pd3dd8->UpdateTexture(t[0].m_t, t[1].m_t);
			CHECKRESULT(hr == D3D_OK && "Update");

			for(j2 = 0; j2 < tests[i].l2; j2++) {
				hr = t[1].m_t->LockBox(j2, &r, NULL, D3DLOCK_RAWDATA);
				CHECKRESULT(hr == D3D_OK && "Lock");
				br = t[1].Check(j2, true, &r);
				CHECKRESULT(br == true && "CHECK!");
				hr = t[1].m_t->UnlockBox(j2);
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
	CHECKLEAKS();
	xEndVariation(hlog);
}
ENDTEST()


BEGINTEST(testVolLockTextureFlags)
{
	DWORD hr;
	bool br;
	VTEX t[1];
	D3DLOCKED_BOX r, oldr;

	__int64 StartTime, EndTime;

	xStartVariation(hlog, "init");
	{
/*
		br = t[0].Init(256, 128, 128, 1, D3DFMT_R8G8B8A8);	
/*/
		br = t[0].Init(256, 128, 128, 1, D3DFMT_R5G6B5);	
/**/
		CHECKRESULT(br == true);
		if(WASBADRESULT()) goto cleanup;

	}
	xEndVariation(hlog);

	xStartVariation(hlog, "SW'0");
	{
		hr = t[0].m_t->LockBox(0, &r, NULL, D3DLOCK_RAWDATA);
		oldr = r;
		CHECKRESULT(hr == D3D_OK);
		t[0].Set(0, true, &r);
		t[0].m_t->UnlockBox(0);

		hr = t[0].m_t->LockBox(0, &r, NULL, 0);
		CHECKRESULT(oldr.pBits != r.pBits);
		CHECKRESULT(hr == D3D_OK);
		br = t[0].Check(0, false, &r);
		CHECKRESULT(br == true);
		t[0].Set(0, false, &r);
		hr = t[0].m_t->UnlockBox(0);

		hr = t[0].m_t->LockBox(0, &r, NULL, D3DLOCK_RAWDATA);
		CHECKRESULT(oldr.pBits == r.pBits);
		CHECKRESULT(hr == D3D_OK);
		br = t[0].Check(0, true, &r);
		CHECKRESULT(br == true);
		t[0].m_t->UnlockBox(0);
	}
	xEndVariation(hlog);

	xStartVariation(hlog, "SW'R/O");
	{
		hr = t[0].m_t->LockBox(0, &r, NULL, D3DLOCK_RAWDATA);
		oldr = r;
		CHECKRESULT(hr == D3D_OK);
		t[0].Set(0, true, &r);
		t[0].m_t->UnlockBox(0);

		hr = t[0].m_t->LockBox(0, &r, NULL, D3DLOCK_READONLY);
		CHECKRESULT(oldr.pBits != r.pBits);
		CHECKRESULT(hr == D3D_OK);
		br = t[0].Check(0, false, &r);
		CHECKRESULT(br == true);
//		t[0].Set(0, false, &r);

		StartTime = GetUsCount(); {
			hr = t[0].m_t->UnlockBox(0);
		} EndTime = GetUsCount();
		CHECKRESULT(EndTime - StartTime < 10000); //it shouldn't take more than 10 milliseconds to unlock.
		if(WASBADRESULT()) xLog(hlog, XLL_INFO, "time to unlock: %d uS", EndTime - StartTime);

		hr = t[0].m_t->LockBox(0, &r, NULL, D3DLOCK_RAWDATA);
		CHECKRESULT(oldr.pBits == r.pBits);
		CHECKRESULT(hr == D3D_OK);
//		br = t[0].Check(0, true, &r);
//		CHECKRESULT(br == true);
		t[0].m_t->UnlockBox(0);
	}
	xEndVariation(hlog);

	xStartVariation(hlog, "SW'Dis");
	{
		hr = t[0].m_t->LockBox(0, &r, NULL, D3DLOCK_RAWDATA);
		oldr = r;
		CHECKRESULT(hr == D3D_OK);
//		t[0].Set(0, true, &r);
		t[0].m_t->UnlockBox(0);

		StartTime = GetUsCount(); {
			hr = t[0].m_t->LockBox(0, &r, NULL, D3DLOCK_DISCARD);
		} EndTime = GetUsCount();
		CHECKRESULT(EndTime - StartTime < 10000); //it shouldn't take more than 10 milliseconds to lock.
		if(WASBADRESULT()) xLog(hlog, XLL_INFO, "time to lock: %d uS", EndTime - StartTime);

		CHECKRESULT(oldr.pBits != r.pBits);
		CHECKRESULT(hr == D3D_OK);
//		br = t[0].Check(0, false, &r);
//		CHECKRESULT(br == true);
		t[0].Set(0, false, &r);
		hr = t[0].m_t->UnlockBox(0);

		hr = t[0].m_t->LockBox(0, &r, NULL, D3DLOCK_RAWDATA);
		CHECKRESULT(oldr.pBits == r.pBits);
		CHECKRESULT(hr == D3D_OK);
		br = t[0].Check(0, true, &r);
		CHECKRESULT(br == true);
		t[0].m_t->UnlockBox(0);
	}
	xEndVariation(hlog);

	xStartVariation(hlog, "SW'Raw");
	{
		hr = t[0].m_t->LockBox(0, &r, NULL, D3DLOCK_RAWDATA);
		oldr = r;
		CHECKRESULT(hr == D3D_OK);
		t[0].Set(0, true, &r);
		t[0].m_t->UnlockBox(0);

		StartTime = GetUsCount(); {
			hr = t[0].m_t->LockBox(0, &r, NULL, D3DLOCK_RAWDATA);
		} EndTime = GetUsCount();
		CHECKRESULT(EndTime - StartTime < 10000); //it shouldn't take more than 10 milliseconds to lock.
		if(WASBADRESULT()) xLog(hlog, XLL_INFO, "time to lock: %d uS", EndTime - StartTime);

		CHECKRESULT(oldr.pBits == r.pBits);
		CHECKRESULT(hr == D3D_OK);
		br = t[0].Check(0, true, &r);
		CHECKRESULT(br == true);
		t[0].Set(0, true, &r);
		StartTime = GetUsCount(); {
			hr = t[0].m_t->UnlockBox(0);
		} EndTime = GetUsCount();
		CHECKRESULT(EndTime - StartTime < 10000); //it shouldn't take more than 10 milliseconds to lock.
		if(WASBADRESULT()) xLog(hlog, XLL_INFO, "time to unlock: %d uS", EndTime - StartTime);

		hr = t[0].m_t->LockBox(0, &r, NULL, D3DLOCK_RAWDATA);
		CHECKRESULT(oldr.pBits == r.pBits);
		CHECKRESULT(hr == D3D_OK);
		br = t[0].Check(0, true, &r);
		CHECKRESULT(br == true);
		t[0].m_t->UnlockBox(0);
	}
	xEndVariation(hlog);

	xStartVariation(hlog, "SW'Raw/RO");
	{
		hr = t[0].m_t->LockBox(0, &r, NULL, D3DLOCK_RAWDATA);
		oldr = r;
		CHECKRESULT(hr == D3D_OK);
		t[0].Set(0, true, &r);
		t[0].m_t->UnlockBox(0);

		StartTime = GetUsCount(); {
			hr = t[0].m_t->LockBox(0, &r, NULL, D3DLOCK_RAWDATA | D3DLOCK_READONLY);
		} EndTime = GetUsCount();
		CHECKRESULT(EndTime - StartTime < 10000); //it shouldn't take more than 10 milliseconds to lock.
		if(WASBADRESULT()) xLog(hlog, XLL_INFO, "time to lock: %d uS", EndTime - StartTime);

		CHECKRESULT(oldr.pBits == r.pBits);
		CHECKRESULT(hr == D3D_OK);
		br = t[0].Check(0, true, &r);
		CHECKRESULT(br == true);
//		t[0].Set(0, true, &r);
		StartTime = GetUsCount(); {
			hr = t[0].m_t->UnlockBox(0);
		} EndTime = GetUsCount();
		CHECKRESULT(EndTime - StartTime < 10000); //it shouldn't take more than 10 milliseconds to lock.
		if(WASBADRESULT()) xLog(hlog, XLL_INFO, "time to unlock: %d uS", EndTime - StartTime);

//		hr = t[0].m_t->LockBox(0, &r, NULL, D3DLOCK_RAWDATA);
//		CHECKRESULT(oldr.pBits == r.pBits);
//		CHECKRESULT(hr == D3D_OK);
//		br = t[0].Check(0, true, &r);
//		CHECKRESULT(br == true);
//		t[0].m_t->UnlockBox(0);
	}
	xEndVariation(hlog);

	xStartVariation(hlog, "SW'Raw/Dis");
	{
		hr = t[0].m_t->LockBox(0, &r, NULL, D3DLOCK_RAWDATA);
		oldr = r;
		CHECKRESULT(hr == D3D_OK);
//		t[0].Set(0, true, &r);
		t[0].m_t->UnlockBox(0);

		StartTime = GetUsCount(); {
			hr = t[0].m_t->LockBox(0, &r, NULL, D3DLOCK_RAWDATA | D3DLOCK_READONLY);
		} EndTime = GetUsCount();
		CHECKRESULT(EndTime - StartTime < 10000); //it shouldn't take more than 10 milliseconds to lock.
		if(WASBADRESULT()) xLog(hlog, XLL_INFO, "time to lock: %d uS", EndTime - StartTime);

		CHECKRESULT(oldr.pBits == r.pBits);
		CHECKRESULT(hr == D3D_OK);
//		br = t[0].Check(0, true, &r);
//		CHECKRESULT(br == true);
		t[0].Set(0, true, &r);
		StartTime = GetUsCount(); {
			hr = t[0].m_t->UnlockBox(0);
		} EndTime = GetUsCount();
		CHECKRESULT(EndTime - StartTime < 10000); //it shouldn't take more than 10 milliseconds to lock.
		if(WASBADRESULT()) xLog(hlog, XLL_INFO, "time to unlock: %d uS", EndTime - StartTime);

		hr = t[0].m_t->LockBox(0, &r, NULL, D3DLOCK_RAWDATA);
		CHECKRESULT(oldr.pBits == r.pBits);
		CHECKRESULT(hr == D3D_OK);
		br = t[0].Check(0, true, &r);
		CHECKRESULT(br == true);
		t[0].m_t->UnlockBox(0);
	}
	xEndVariation(hlog);

cleanup:
	xStartVariation(hlog, "Cleanup");
	{
		t[0].Release();
	}
	xEndVariation(hlog);
}
ENDTEST()

#pragma data_seg()

