#include "d3dapi.hpp"

LINKME(11);
typedef struct {
	int texnum1;
	int layer1;
	int face1;
	bool doraw1;
	int texnum2;
	int layer2;
	int face2;
	bool doraw2;
} CUBELOCKTESTSTRUCT;

namespace d3dapi {
class CTEX {
public:
	CTEX():m_t(0), m_pLast(0) {}

	bool Init(int width, int levels, D3DFORMAT format)
	{
		m_w = width; m_l = levels; m_f = format;
		DWORD hr;
		hr = g_pd3dd8->CreateCubeTexture(m_w,m_l,0,m_f,D3DPOOL_DEFAULT, &m_t);
		if(hr != D3D_OK) {
			xLog(hlog, XLL_FAIL, "CTEX: create cube texture, w%d l%d f%d", m_w, m_l, m_f);
			return false;
		}
		m_pLast = new DWORD[m_l * 6];
		if(!m_pLast) { 
			xLog(hlog, XLL_FAIL, "CTEX: couldn't create dword array, size %d", m_l * 6); 
			return false; 
		}
		m_bpp = D3D::BitsPerPixelOfD3DFORMAT(format);

		return true;
	}

	~CTEX() {
		Release();
	}
	bool Release() {
		DWORD hr;
		if(m_t) {
			hr = m_t->Release();
			xLog(hlog, (hr == 0) ? XLL_PASS : XLL_FAIL, "CTEX: release cube texture: %d", hr);
			m_t = 0;
		}
		if(m_pLast) {
			delete[] m_pLast;
			m_pLast = 0;
		}
		return true;
	}


	void Set(int side, int layer, bool raw, D3DLOCKED_RECT* pr) {  
		int r;
		DWORD col;
		int x, y;
		int w = max(m_w >> layer, 1);
		int h = max(m_w >> layer, 1);

		if(layer < 0 || layer >= m_l) _asm {int 3};
		if(!m_t || !m_pLast) _asm {int 3};
		m_pLast[layer * 6 + side] = rnd();
		r = m_pLast[layer * 6 + side];							//store the random number so we can check the location later
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

	bool Check(int side, int layer, bool raw, D3DLOCKED_RECT* pr) 
	{
		int r;
		DWORD col;
		int x, y;
		int w = max(m_w >> layer, 1);
		int h = max(m_w >> layer, 1);

		if(layer < 0 || layer >= m_l) _asm {int 3};
		if(!m_t || !m_pLast) _asm {int 3};

		r = m_pLast[layer * 6 + side];	//get the random number from last time
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
	int m_l;
	int m_bpp;
	D3DFORMAT m_f;
	DWORD* m_pLast;
	D3DCubeTexture* m_t;

	void SetPoint(int x, int y, int layer, DWORD col, bool raw, D3DLOCKED_RECT* pr) 
	{
		int i;
		if(raw) {
			SWIZZLER s(max(m_w >> layer, 1), max(m_w >> layer, 1), 0);
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
			SWIZZLER s(max(m_w >> layer, 1), max(m_w >> layer, 1), 0);
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

#pragma data_seg(".d3dapi$test052") //
BEGINTEST(testCubeLock)
{
	DWORD hr;
	bool br;
	CTEX t[2];
	D3DLOCKED_RECT r;
	int i, j;
	int face;
	//D3DCUBEMAP_FACE_POSITIVE_X;

	STARTLEAKCHECK();
	xStartVariation(hlog, "Cubelock, init");
	{
		br = t[0].Init(256, 9, D3DFMT_R8G8B8A8);
		CHECKRESULT(br == true);
		if(WASBADRESULT()) goto cleanup;

		br = t[1].Init(512, 2, D3DFMT_R8G8B8A8);
		CHECKRESULT(br == true);
		if(WASBADRESULT()) goto cleanup;
	}
	xEndVariation(hlog);
	xStartVariation(hlog, "cube:0L0 & 0L1 swizzled, ch sw");
	{
		for(face = D3DCUBEMAP_FACE_POSITIVE_X; face <= D3DCUBEMAP_FACE_NEGATIVE_Z; face++) {
			hr = t[0].m_t->LockRect((D3DCUBEMAP_FACES)face, 0, &r, NULL, 0); //Exception is happening in this call, on D3DCUBEMAP_FACE_NEGATIVE_Z
			CHECKRESULT(hr == D3D_OK && "LockRect (and unswizzle)");
			t[0].Set(face, 0, false, &r);

			hr = t[0].m_t->UnlockRect((D3DCUBEMAP_FACES)face, 0);
			CHECKRESULT(hr == D3D_OK && "UnlockRect (and reswizzle)");       

			hr = t[0].m_t->LockRect((D3DCUBEMAP_FACES)face, 0, &r, NULL, 0);
			CHECKRESULT(hr == D3D_OK && "LockRect again (and unswizzle)");
			br = t[0].Check(face, 0, false, &r);
			CHECKRESULT(true == br && "check for set pixels");         

			hr = t[0].m_t->UnlockRect((D3DCUBEMAP_FACES)face, 0);
			CHECKRESULT(hr == D3D_OK && "Unlock again and reswizzle");
		}
	}
	xEndVariation(hlog);
	xStartVariation(hlog, "swizzle all sides");
	{
		for(face = D3DCUBEMAP_FACE_POSITIVE_X; face <= D3DCUBEMAP_FACE_NEGATIVE_Z; face++) {
			hr = t[0].m_t->LockRect((D3DCUBEMAP_FACES)face, 0, &r, NULL, 0); //Exception is happening in this call, on D3DCUBEMAP_FACE_NEGATIVE_Z
			CHECKRESULT(hr == D3D_OK && "LockRect (and unswizzle)");
			t[0].Set(face, 0, false, &r);
		}
		for(face = D3DCUBEMAP_FACE_POSITIVE_X; face <= D3DCUBEMAP_FACE_NEGATIVE_Z; face++) {
			hr = t[0].m_t->UnlockRect((D3DCUBEMAP_FACES)face, 0);
			CHECKRESULT(hr == D3D_OK && "UnlockRect (and reswizzle)");       
		}
		for(face = D3DCUBEMAP_FACE_NEGATIVE_Z; face >= D3DCUBEMAP_FACE_POSITIVE_X; face--) {
			hr = t[0].m_t->LockRect((D3DCUBEMAP_FACES)face, 0, &r, NULL, D3DLOCK_RAWDATA);
			CHECKRESULT(hr == D3D_OK && "LockRect again (no unswizzle)");
			br = t[0].Check(face, 0, true, &r);
			CHECKRESULT(true == br && "check for set pixels");         
		}
		for(face = D3DCUBEMAP_FACE_NEGATIVE_Z; face >= D3DCUBEMAP_FACE_POSITIVE_X; face--) {
			hr = t[0].m_t->UnlockRect((D3DCUBEMAP_FACES)face, 0);
			CHECKRESULT(hr == D3D_OK && "unlock again");
		}
	}
	xEndVariation(hlog);
	for(j = 0; j < 2; j++) {
		xStartVariation(hlog, "multi-layer locks");
		{
			CUBELOCKTESTSTRUCT tests [] = {
				{0, 0, D3DCUBEMAP_FACE_NEGATIVE_Z, false, 0, 1, D3DCUBEMAP_FACE_POSITIVE_X, true},
				{0, 0, D3DCUBEMAP_FACE_POSITIVE_X, false, 1, 0, D3DCUBEMAP_FACE_POSITIVE_X, false},
				{0, 0, D3DCUBEMAP_FACE_NEGATIVE_Z, false, 1, 1, D3DCUBEMAP_FACE_NEGATIVE_Z, false},
				{0, 0, D3DCUBEMAP_FACE_NEGATIVE_Z, true,  0, 1, D3DCUBEMAP_FACE_POSITIVE_X, true},
				{0, 0, D3DCUBEMAP_FACE_POSITIVE_X, true,  1, 0, D3DCUBEMAP_FACE_POSITIVE_X, false},
				{0, 0, D3DCUBEMAP_FACE_NEGATIVE_Z, false, 1, 0, D3DCUBEMAP_FACE_NEGATIVE_Z, true},
				{0, 0, D3DCUBEMAP_FACE_NEGATIVE_Z, false, 0, 1, D3DCUBEMAP_FACE_NEGATIVE_Z, true},
				{0, 0, D3DCUBEMAP_FACE_POSITIVE_X, false, 0, 1, D3DCUBEMAP_FACE_POSITIVE_X, true},
				{0, 0, D3DCUBEMAP_FACE_POSITIVE_Y, false, 0, 1, D3DCUBEMAP_FACE_POSITIVE_Y, true},
			};

			for(i = 0; i < COUNTOF(tests); i++) {
				xLog(hlog, XLL_INFO, "cubenum%d, layer%d, Face%d, as%d,  cubenum%d, layer%d, Face%d, as%d", 
					tests[i].texnum1, tests[i].layer1, tests[i].face1, tests[i].doraw1,
					tests[i].texnum2, tests[i].layer2, tests[i].face2, tests[i].doraw2);
				hr = t[tests[i].texnum1].m_t->LockRect((D3DCUBEMAP_FACES)tests[i].face1, tests[i].layer1, &r, NULL, tests[i].doraw1?D3DLOCK_RAWDATA:0); 
				CHECKRESULT(hr == D3D_OK && "Lock1");
				t[tests[i].texnum1].Set(tests[i].face1, tests[i].layer1, tests[i].doraw1, &r);
				hr = t[tests[i].texnum2].m_t->LockRect((D3DCUBEMAP_FACES)tests[i].face2, tests[i].layer2, &r, NULL, tests[i].doraw2?D3DLOCK_RAWDATA:0); 
				CHECKRESULT(hr == D3D_OK && "Lock2");
				t[tests[i].texnum2].Set(tests[i].face2, tests[i].layer2, tests[i].doraw2, &r);

				hr = t[tests[i].texnum1].m_t->UnlockRect((D3DCUBEMAP_FACES)tests[i].face1, tests[i].layer1);
				CHECKRESULT(hr == D3D_OK && "Unlock1");       
				hr = t[tests[i].texnum2].m_t->UnlockRect((D3DCUBEMAP_FACES)tests[i].face2, tests[i].layer2);
				CHECKRESULT(hr == D3D_OK && "Unlock2");       

				hr = t[tests[i].texnum1].m_t->LockRect((D3DCUBEMAP_FACES)tests[i].face1, tests[i].layer1, &r, NULL, tests[i].doraw1?D3DLOCK_RAWDATA:0); 
				CHECKRESULT(hr == D3D_OK && "Lock1 again");
				br = t[tests[i].texnum1].Check(tests[i].face1, tests[i].layer1, tests[i].doraw1, &r);
				CHECKRESULT(true == br && "check for set pixels");         
				hr = t[tests[i].texnum1].m_t->UnlockRect((D3DCUBEMAP_FACES)tests[i].face1, tests[i].layer1);
				CHECKRESULT(hr == D3D_OK && "Unlock1");       

				hr = t[tests[i].texnum2].m_t->LockRect((D3DCUBEMAP_FACES)tests[i].face2, tests[i].layer2, &r, NULL, tests[i].doraw2?D3DLOCK_RAWDATA:0); 
				CHECKRESULT(hr == D3D_OK && "Lock2 again");
				br = t[tests[i].texnum2].Check(tests[i].face2, tests[i].layer2, tests[i].doraw2, &r);
				CHECKRESULT(true == br && "check for set pixels");         
				hr = t[tests[i].texnum2].m_t->UnlockRect((D3DCUBEMAP_FACES)tests[i].face2, tests[i].layer2);
				CHECKRESULT(hr == D3D_OK && "Unlock2");       
			}
		}
		xEndVariation(hlog);
		if(j == 0) {
			xStartVariation(hlog, "cube:makebig");
			{
				t[0].Release();
				t[1].Release();
				CHECKLEAKS();

				br = t[0].Init(2048, 2, D3DFMT_A8);
				CHECKRESULT(br == true);
				if(WASBADRESULT()) goto cleanup;
				br = t[1].Init(2, 2, D3DFMT_A8);
				CHECKRESULT(br == true);
				if(WASBADRESULT()) goto cleanup;
			}
			xEndVariation(hlog);
		}
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

BEGINTEST(testCubeUpdateTexture)
{
	DWORD hr;
	bool br;
	CTEX t[2];
	D3DLOCKED_RECT r;
	int i, j, j2, s;

	struct {
		int w1;
		int l1;
		int w2;
		int l2;
		D3DFORMAT format;
	} tests[] = {
		{1024,  2,  512,  1, D3DFMT_A8          },
		{ 512, 10,    1,  1, D3DFMT_A8          },
		{   2,  2,    1,  1, D3DFMT_A8          },
		{ 512,  9,  256,  8, D3DFMT_A8          },
		{1024,  9,  512,  8, D3DFMT_DXT1        },
		{ 512, 10,    1,  1, D3DFMT_R5G6B5      },
		{   2,  2,    1,  1, D3DFMT_R5G6B5      },
		{ 256,  9,  256,  9, D3DFMT_R5G6B5      },
		{ 512, 10,   64,  1, D3DFMT_R8G8B8A8    },
		{   4,  2,    2,  1, D3DFMT_R8G8B8A8    },
		{ 256,  9,  256,  9, D3DFMT_R8G8B8A8    },
	};

	STARTLEAKCHECK();
	for(i = 0; i < COUNTOF(tests); i++) {
		xStartVariation(hlog, "lock, new texture");
		{
			t[0].Release();
			t[1].Release();

			xLog(hlog, XLL_INFO, "cube 1 size: %d, num layers: %d,  cube 2 size: %d, num layers: %d, fmt: %d", tests[i].w1, tests[i].l1, tests[i].w2, tests[i].l2, tests[i].format);

			br = t[0].Init(tests[i].w1, tests[i].l1, tests[i].format);
			CHECKRESULT(br == true);
			if(WASBADRESULT()) goto cleanup;

			br = t[1].Init(tests[i].w2, tests[i].l2, tests[i].format);
			CHECKRESULT(br == true);
			if(WASBADRESULT()) goto cleanup;

			for(j2 = 0; j2 < tests[i].l2; j2++) {
				for(s = 0; s < 6; s++) {
					hr = t[1].m_t->LockRect((D3DCUBEMAP_FACES)s, j2, &r, NULL, D3DLOCK_RAWDATA);
					CHECKRESULT(hr == D3D_OK && "Lock");
					t[1].Set(s, j2, true, &r);
					hr = t[1].m_t->UnlockRect((D3DCUBEMAP_FACES)s, j2);
					CHECKRESULT(hr == D3D_OK && "Unlock");
				}
			}
			for(j = 0; j < tests[i].l1; j++) {
				for(s = 0; s < 6; s++) {
					hr = t[0].m_t->LockRect((D3DCUBEMAP_FACES)s, j, &r, NULL, D3DLOCK_RAWDATA);
					CHECKRESULT(hr == D3D_OK && "Lock");
					t[0].Set(s, j, true, &r);
					hr = t[0].m_t->UnlockRect((D3DCUBEMAP_FACES)s, j);
					CHECKRESULT(hr == D3D_OK && "Unlock");

					j2 = j - Log2(tests[i].w1) + Log2(tests[i].w2);
					if(j2 >= 0 && j2 < tests[i].l2) {
						t[1].m_pLast[j2 * 6 + s] = t[0].m_pLast[j * 6 + s];
					}
				}
			}
			hr = g_pd3dd8->UpdateTexture(t[0].m_t, t[1].m_t);
			CHECKRESULT(hr == D3D_OK && "Update");

			for(j2 = 0; j2 < tests[i].l2; j2++) {
				for(s = 0; s < 6; s++) {
					hr = t[1].m_t->LockRect((D3DCUBEMAP_FACES)s, j2, &r, NULL, D3DLOCK_RAWDATA);
					CHECKRESULT(hr == D3D_OK && "Lock");
					br = t[1].Check(s, j2, true, &r);
					CHECKRESULT(br == true && "CHECK!");
					hr = t[1].m_t->UnlockRect((D3DCUBEMAP_FACES)s, j2);
					CHECKRESULT(hr == D3D_OK && "Unlock");
				}
			}
		}
		xEndVariation(hlog);
	}
	goto aftercleanup;
cleanup:
	xEndVariation(hlog);
aftercleanup: 
	xStartVariation(hlog, "leak check");
	t[0].Release();
	t[1].Release();
	CHECKLEAKS();
	xEndVariation(hlog);

}
ENDTEST()

BEGINTEST(testCubeLockTextureFlags)
{
	DWORD hr;
	bool br;
	CTEX t[1];
	D3DLOCKED_RECT r, oldr;

	__int64 StartTime, EndTime;

	STARTLEAKCHECK();
	xStartVariation(hlog, "init");
	{
/*
		br = t[0].Init(512, 1, D3DFMT_R8G8B8A8);	
/*/
		br = t[0].Init(512, 1, D3DFMT_R5G6B5);	
/**/
		CHECKRESULT(br == true);
		if(WASBADRESULT()) goto cleanup;

	}
	xEndVariation(hlog);

	xStartVariation(hlog, "SW'0");
	{
		hr = t[0].m_t->LockRect(D3DCUBEMAP_FACE_POSITIVE_X, 0, &r, NULL, D3DLOCK_RAWDATA);
		oldr = r;
		CHECKRESULT(hr == D3D_OK);
		t[0].Set(0, 0, true, &r);
		t[0].m_t->UnlockRect(D3DCUBEMAP_FACE_POSITIVE_X, 0);

		hr = t[0].m_t->LockRect(D3DCUBEMAP_FACE_POSITIVE_X, 0, &r, NULL, 0);
		CHECKRESULT(oldr.pBits != r.pBits);
		CHECKRESULT(hr == D3D_OK);
		br = t[0].Check(0, 0, false, &r);
		CHECKRESULT(br == true);
		t[0].Set(0, 0, false, &r);
		hr = t[0].m_t->UnlockRect(D3DCUBEMAP_FACE_POSITIVE_X, 0);

		hr = t[0].m_t->LockRect(D3DCUBEMAP_FACE_POSITIVE_X, 0, &r, NULL, D3DLOCK_RAWDATA);
		CHECKRESULT(oldr.pBits == r.pBits);
		CHECKRESULT(hr == D3D_OK);
		br = t[0].Check(0, 0, true, &r);
		CHECKRESULT(br == true);
		t[0].m_t->UnlockRect(D3DCUBEMAP_FACE_POSITIVE_X, 0);
	}
	xEndVariation(hlog);

	xStartVariation(hlog, "SW'R/O");
	{
		hr = t[0].m_t->LockRect(D3DCUBEMAP_FACE_POSITIVE_X, 0, &r, NULL, D3DLOCK_RAWDATA);
		oldr = r;
		CHECKRESULT(hr == D3D_OK);
		t[0].Set(0, 0, true, &r);
		t[0].m_t->UnlockRect(D3DCUBEMAP_FACE_POSITIVE_X, 0);

		hr = t[0].m_t->LockRect(D3DCUBEMAP_FACE_POSITIVE_X, 0, &r, NULL, D3DLOCK_READONLY);
		CHECKRESULT(oldr.pBits != r.pBits);
		CHECKRESULT(hr == D3D_OK);
		br = t[0].Check(0, 0, false, &r);
		CHECKRESULT(br == true);
//		t[0].Set(0, 0, false, &r);

		StartTime = GetUsCount(); {
			hr = t[0].m_t->UnlockRect(D3DCUBEMAP_FACE_POSITIVE_X, 0);
		} EndTime = GetUsCount();
		CHECKRESULT(EndTime - StartTime < 10000); //it shouldn't take more than 10 milliseconds to unlock.
		if(WASBADRESULT()) xLog(hlog, XLL_INFO, "time to unlock: %d uS", EndTime - StartTime);

		hr = t[0].m_t->LockRect(D3DCUBEMAP_FACE_POSITIVE_X, 0, &r, NULL, D3DLOCK_RAWDATA);
		CHECKRESULT(oldr.pBits == r.pBits);
		CHECKRESULT(hr == D3D_OK);
//		br = t[0].Check(0, 0, true, &r);
//		CHECKRESULT(br == true);
		t[0].m_t->UnlockRect(D3DCUBEMAP_FACE_POSITIVE_X, 0);
	}
	xEndVariation(hlog);

	xStartVariation(hlog, "SW'Dis");
	{
		hr = t[0].m_t->LockRect(D3DCUBEMAP_FACE_POSITIVE_X, 0, &r, NULL, D3DLOCK_RAWDATA);
		oldr = r;
		CHECKRESULT(hr == D3D_OK);
//		t[0].Set(0, 0, true, &r);
		t[0].m_t->UnlockRect(D3DCUBEMAP_FACE_POSITIVE_X, 0);

		StartTime = GetUsCount(); {
			hr = t[0].m_t->LockRect(D3DCUBEMAP_FACE_POSITIVE_X, 0, &r, NULL, D3DLOCK_DISCARD);
		} EndTime = GetUsCount();
		CHECKRESULT(EndTime - StartTime < 10000); //it shouldn't take more than 10 milliseconds to lock.
		if(WASBADRESULT()) xLog(hlog, XLL_INFO, "time to lock: %d uS", EndTime - StartTime);

		CHECKRESULT(oldr.pBits != r.pBits);
		CHECKRESULT(hr == D3D_OK);
//		br = t[0].Check(0, 0, false, &r);
//		CHECKRESULT(br == true);
		t[0].Set(0, 0, false, &r);
		hr = t[0].m_t->UnlockRect(D3DCUBEMAP_FACE_POSITIVE_X, 0);

		hr = t[0].m_t->LockRect(D3DCUBEMAP_FACE_POSITIVE_X, 0, &r, NULL, D3DLOCK_RAWDATA);
		CHECKRESULT(oldr.pBits == r.pBits);
		CHECKRESULT(hr == D3D_OK);
		br = t[0].Check(0, 0, true, &r);
		CHECKRESULT(br == true);
		t[0].m_t->UnlockRect(D3DCUBEMAP_FACE_POSITIVE_X, 0);
	}
	xEndVariation(hlog);

	xStartVariation(hlog, "SW'Raw");
	{
		hr = t[0].m_t->LockRect(D3DCUBEMAP_FACE_POSITIVE_X, 0, &r, NULL, D3DLOCK_RAWDATA);
		oldr = r;
		CHECKRESULT(hr == D3D_OK);
		t[0].Set(0, 0, true, &r);
		t[0].m_t->UnlockRect(D3DCUBEMAP_FACE_POSITIVE_X, 0);

		StartTime = GetUsCount(); {
			hr = t[0].m_t->LockRect(D3DCUBEMAP_FACE_POSITIVE_X, 0, &r, NULL, D3DLOCK_RAWDATA);
		} EndTime = GetUsCount();
		CHECKRESULT(EndTime - StartTime < 10000); //it shouldn't take more than 10 milliseconds to lock.
		if(WASBADRESULT()) xLog(hlog, XLL_INFO, "time to lock: %d uS", EndTime - StartTime);

		CHECKRESULT(oldr.pBits == r.pBits);
		CHECKRESULT(hr == D3D_OK);
		br = t[0].Check(0, 0, true, &r);
		CHECKRESULT(br == true);
		t[0].Set(0, 0, true, &r);
		StartTime = GetUsCount(); {
			hr = t[0].m_t->UnlockRect(D3DCUBEMAP_FACE_POSITIVE_X, 0);
		} EndTime = GetUsCount();
		CHECKRESULT(EndTime - StartTime < 10000); //it shouldn't take more than 10 milliseconds to lock.
		if(WASBADRESULT()) xLog(hlog, XLL_INFO, "time to unlock: %d uS", EndTime - StartTime);

		hr = t[0].m_t->LockRect(D3DCUBEMAP_FACE_POSITIVE_X, 0, &r, NULL, D3DLOCK_RAWDATA);
		CHECKRESULT(oldr.pBits == r.pBits);
		CHECKRESULT(hr == D3D_OK);
		br = t[0].Check(0, 0, true, &r);
		CHECKRESULT(br == true);
		t[0].m_t->UnlockRect(D3DCUBEMAP_FACE_POSITIVE_X, 0);
	}
	xEndVariation(hlog);

	xStartVariation(hlog, "SW'Raw/RO");
	{
		hr = t[0].m_t->LockRect(D3DCUBEMAP_FACE_POSITIVE_X, 0, &r, NULL, D3DLOCK_RAWDATA);
		oldr = r;
		CHECKRESULT(hr == D3D_OK);
		t[0].Set(0, 0, true, &r);
		t[0].m_t->UnlockRect(D3DCUBEMAP_FACE_POSITIVE_X, 0);

		StartTime = GetUsCount(); {
			hr = t[0].m_t->LockRect(D3DCUBEMAP_FACE_POSITIVE_X, 0, &r, NULL, D3DLOCK_RAWDATA | D3DLOCK_READONLY);
		} EndTime = GetUsCount();
		CHECKRESULT(EndTime - StartTime < 10000); //it shouldn't take more than 10 milliseconds to lock.
		if(WASBADRESULT()) xLog(hlog, XLL_INFO, "time to lock: %d uS", EndTime - StartTime);

		CHECKRESULT(oldr.pBits == r.pBits);
		CHECKRESULT(hr == D3D_OK);
		br = t[0].Check(0, 0, true, &r);
		CHECKRESULT(br == true);
//		t[0].Set(0, 0, true, &r);
		StartTime = GetUsCount(); {
			hr = t[0].m_t->UnlockRect(D3DCUBEMAP_FACE_POSITIVE_X, 0);
		} EndTime = GetUsCount();
		CHECKRESULT(EndTime - StartTime < 10000); //it shouldn't take more than 10 milliseconds to lock.
		if(WASBADRESULT()) xLog(hlog, XLL_INFO, "time to unlock: %d uS", EndTime - StartTime);

//		hr = t[0].m_t->LockRect(D3DCUBEMAP_FACE_POSITIVE_X, 0, &r, NULL, D3DLOCK_RAWDATA);
//		CHECKRESULT(oldr.pBits == r.pBits);
//		CHECKRESULT(hr == D3D_OK);
//		br = t[0].Check(0, 0, true, &r);
//		CHECKRESULT(br == true);
//		t[0].m_t->UnlockRect(D3DCUBEMAP_FACE_POSITIVE_X, 0);
	}
	xEndVariation(hlog);

	xStartVariation(hlog, "SW'Raw/Dis");
	{
		hr = t[0].m_t->LockRect(D3DCUBEMAP_FACE_POSITIVE_X, 0, &r, NULL, D3DLOCK_RAWDATA);
		oldr = r;
		CHECKRESULT(hr == D3D_OK);
//		t[0].Set(0, 0, true, &r);
		t[0].m_t->UnlockRect(D3DCUBEMAP_FACE_POSITIVE_X, 0);

		StartTime = GetUsCount(); {
			hr = t[0].m_t->LockRect(D3DCUBEMAP_FACE_POSITIVE_X, 0, &r, NULL, D3DLOCK_RAWDATA | D3DLOCK_READONLY);
		} EndTime = GetUsCount();
		CHECKRESULT(EndTime - StartTime < 10000); //it shouldn't take more than 10 milliseconds to lock.
		if(WASBADRESULT()) xLog(hlog, XLL_INFO, "time to lock: %d uS", EndTime - StartTime);

		CHECKRESULT(oldr.pBits == r.pBits);
		CHECKRESULT(hr == D3D_OK);
//		br = t[0].Check(0, 0, true, &r);
//		CHECKRESULT(br == true);
		t[0].Set(0, 0, true, &r);
		StartTime = GetUsCount(); {
			hr = t[0].m_t->UnlockRect(D3DCUBEMAP_FACE_POSITIVE_X, 0);
		} EndTime = GetUsCount();
		CHECKRESULT(EndTime - StartTime < 10000); //it shouldn't take more than 10 milliseconds to lock.
		if(WASBADRESULT()) xLog(hlog, XLL_INFO, "time to unlock: %d uS", EndTime - StartTime);

		hr = t[0].m_t->LockRect(D3DCUBEMAP_FACE_POSITIVE_X, 0, &r, NULL, D3DLOCK_RAWDATA);
		CHECKRESULT(oldr.pBits == r.pBits);
		CHECKRESULT(hr == D3D_OK);
		br = t[0].Check(0, 0, true, &r);
		CHECKRESULT(br == true);
		t[0].m_t->UnlockRect(D3DCUBEMAP_FACE_POSITIVE_X, 0);
	}
	xEndVariation(hlog);

cleanup:
	xStartVariation(hlog, "Cleanup");
	{
		t[0].Release();
		CHECKLEAKS();
	}
	xEndVariation(hlog);
}
ENDTEST()

#pragma data_seg()

