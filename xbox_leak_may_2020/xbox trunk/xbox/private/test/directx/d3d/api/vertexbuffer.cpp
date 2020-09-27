#include "d3dapi.hpp"
/*
D3DVertexBuffer_AddRef(D3DVertexBuffer *pThis) { return D3DResource_AddRef((D3DResource *)pThis); }
D3DVertexBuffer_Release(D3DVertexBuffer *pThis) { return D3DResource_Release((D3DResource *)pThis); }
//D3DVertexBuffer_GetDevice(D3DVertexBuffer *pThis, D3DDevice **ppDevice) { return D3DResource_GetDevice((D3DResource *)pThis, ppDevice); }
//D3DVertexBuffer_GetType(D3DVertexBuffer *pThis) { return D3DResource_GetType((D3DResource *)pThis); }
D3DVertexBuffer_IsBusy(D3DVertexBuffer *pThis) { return D3DResource_IsBusy((D3DResource *)pThis); }
D3DVertexBuffer_MoveResourceMemory(D3DVertexBuffer *pThis, D3DMEMORY where) { D3DResource_MoveResourceMemory((D3DResource *)pThis, where); }
D3DVertexBuffer_Register(D3DVertexBuffer *pThis) { D3DResource_Register((D3DResource *)pThis); }
D3DVertexBuffer_Deregister(D3DVertexBuffer *pThis) { D3DResource_Deregister((D3DResource *)pThis); }
D3DVertexBuffer_SetPrivateData(D3DVertexBuffer *pThis, REFGUID refguid, CONST void *pData, DWORD SizeOfData, DWORD Flags) { return D3DResource_SetPrivateData((D3DResource *)pThis, refguid, pData, SizeOfData, Flags); }
D3DVertexBuffer_GetPrivateData(D3DVertexBuffer *pThis, REFGUID refguid, void *pData, DWORD *pSizeOfData) { return D3DResource_GetPrivateData((D3DResource *)pThis, refguid, pData, pSizeOfData); }
D3DVertexBuffer_FreePrivateData(D3DVertexBuffer *pThis, REFGUID refguid) { return D3DResource_FreePrivateData((D3DResource *)pThis, refguid); }
HRESULT WINAPI D3DVertexBuffer_Lock(D3DVertexBuffer *pThis, UINT OffsetToLock, UINT SizeToLock, BYTE **ppbData, DWORD Flags);
HRESULT WINAPI D3DVertexBuffer_Unlock(D3DVertexBuffer *pThis);
//HRESULT WINAPI D3DVertexBuffer_GetDesc(D3DVertexBuffer *pThis, D3DVERTEXBUFFER_DESC *pDesc);
*/
LINKME(13);

#pragma data_seg(".d3dapi$test030") 

BEGINTEST(testCreateVertexBuffer)
{
	//this is just a call to a Mm allocation routine... 
	//doesn't require a huge amount of testing.
	D3DVertexBuffer * blah;
	DWORD hr;
	int i;

	xStartVariation(hlog, "allocate vbs");
	{
		for(i = 0; i <= 25; i++) {
			xLog(hlog, XLL_INFO, "size: %d", (1 << i));
			hr = g_pd3dd8->CreateVertexBuffer((1 << i), 0,0,(D3DPOOL)0, &blah);
			CHECKRESULT(hr == D3D_OK);
			CHECKRESULT(blah != 0);
			if(WASGOODRESULT()) {
				hr = blah->Release();
				CHECKRESULT(hr == 0);
			}
		}
	}
	xEndVariation(hlog);
}
ENDTEST()

BEGINTEST(testVbGetType)
{
	D3DVertexBuffer * blah;
	DWORD hr;

	xStartVariation(hlog, "gettype");
	{
		hr = g_pd3dd8->CreateVertexBuffer(23,0,0,0,&blah);
		CHECKRESULT(hr == D3D_OK);
		hr = blah->GetType();
		CHECKRESULT(hr == D3DRTYPE_VERTEXBUFFER);
	}
	xEndVariation(hlog);
}
ENDTEST()

BEGINTEST(testVbGetDesc)
{
	D3DVertexBuffer * blah;
	DWORD hr;
	D3DVERTEXBUFFER_DESC d;

	xStartVariation(hlog, "getdesc");
	{
		hr = g_pd3dd8->CreateVertexBuffer(23,0,0,0,&blah);
		CHECKRESULT(hr == D3D_OK);
		hr = blah->GetDesc(&d);
		CHECKRESULT(hr == D3D_OK);
		CHECKRESULT(d.Format == D3DFMT_VERTEXDATA);
		CHECKRESULT(d.Type == D3DRTYPE_VERTEXBUFFER);
		hr = blah->Release();
		CHECKRESULT(hr == 0);
	}
	xEndVariation(hlog);
}
ENDTEST()

BEGINTEST(testVbGetDevice)
{
	D3DVertexBuffer * blah;
	DWORD hr;
	D3DDevice* d;

	xStartVariation(hlog, "getdevice");
	{
		hr = g_pd3dd8->CreateVertexBuffer(23,0,0,0,&blah);
		CHECKRESULT(hr == D3D_OK);
		if(WASGOODRESULT()) {
			hr = blah->GetDevice(&d);
			CHECKRESULT(hr == D3D_OK);
			CHECKRESULT(d == g_pd3dd8);
			d->Release();
			hr = blah->Release();
			CHECKRESULT(hr == 0);
		}
	}
	xEndVariation(hlog);
}
ENDTEST()

BEGINTEST(testVbLockUnlock)
{
	D3DVertexBuffer * blah;
	DWORD hr;

	BYTE* pb[3];

	xStartVariation(hlog, "Init");
	{
		hr = g_pd3dd8->CreateVertexBuffer(100,0,0,0,&blah);
		CHECKRESULT(hr == D3D_OK);
		if(WASGOODRESULT()) {
			xEndVariation(hlog);
			xStartVariation(hlog, "Multiple Read-Only");
			{
				if(WASGOODRESULT()) {
					hr = blah->Lock(0, 0, &pb[0], 0);
					CHECKRESULT(hr == D3D_OK);
					hr = blah->Lock(0, 0, &pb[1], D3DLOCK_READONLY);
					CHECKRESULT(hr == D3D_OK);
					hr = blah->Lock(25, 1, &pb[2], D3DLOCK_READONLY);

					CHECKRESULT(hr == D3D_OK);
					CHECKRESULT(pb[0] == pb[1]);
					CHECKRESULT(pb[2] == pb[0] + 25);

					hr = blah->Unlock();
					CHECKRESULT(hr == D3D_OK);
					hr = blah->Unlock();
					CHECKRESULT(hr == D3D_OK);
					hr = blah->Unlock();
					CHECKRESULT(hr == D3D_OK);
				}
			}
			xEndVariation(hlog);
			xStartVariation(hlog, "Multiple locks");
			{
				hr = g_pd3dd8->CreateVertexBuffer(100,0,0,0,&blah);
				CHECKRESULT(hr == D3D_OK);
				if(WASGOODRESULT()) {
					hr = blah->Lock(0, 0, &pb[0], 0);
					CHECKRESULT(hr == D3D_OK);
					hr = blah->Lock(0, 0, &pb[1], 0);
					CHECKRESULT(hr == D3D_OK);
					hr = blah->Lock(25, 1, &pb[2], 0);

					CHECKRESULT(hr == D3D_OK);
					CHECKRESULT(pb[0] == pb[1]);
					CHECKRESULT(pb[2] == pb[0] + 25);

					hr = blah->Unlock();
					CHECKRESULT(hr == D3D_OK);
					hr = blah->Unlock();
					CHECKRESULT(hr == D3D_OK);
					hr = blah->Unlock();
					CHECKRESULT(hr == D3D_OK);
				}
			}
		}
	}
	xEndVariation(hlog);
}
ENDTEST()

BEGINTEST(testVbUma)
{
	D3DVertexBuffer * blah;
	DWORD hr;

	BYTE* pb[3];

	xStartVariation(hlog, "Init");
	{
		hr = g_pd3dd8->CreateVertexBuffer(2052,0,0,0,&blah);
		CHECKRESULT(hr == D3D_OK);
		if(WASGOODRESULT()) {
			xEndVariation(hlog);
			xStartVariation(hlog, "Multiple Read-Only");
			{
				if(WASGOODRESULT()) {
					hr = blah->Lock(0, 0, &pb[0], 0);
					CHECKRESULT(hr == D3D_OK);
					hr = blah->Lock(0, 0, &pb[1], D3DLOCK_READONLY);
					CHECKRESULT(hr == D3D_OK);
					hr = blah->Lock(25, 1, &pb[2], D3DLOCK_READONLY);

					CHECKRESULT(hr == D3D_OK);
					CHECKRESULT(pb[0] == pb[1]);
					CHECKRESULT(pb[2] == pb[0] + 25);

					hr = blah->Unlock();
					CHECKRESULT(hr == D3D_OK);
					hr = blah->Unlock();
					CHECKRESULT(hr == D3D_OK);
					hr = blah->Unlock();
					CHECKRESULT(hr == D3D_OK);
				}
			}
			xEndVariation(hlog);
			xStartVariation(hlog, "Multiple locks");
			{
				hr = g_pd3dd8->CreateVertexBuffer(100,0,0,0,&blah);
				CHECKRESULT(hr == D3D_OK);
				if(WASGOODRESULT()) {
					hr = blah->Lock(0, 0, &pb[0], 0);
					CHECKRESULT(hr == D3D_OK);
					hr = blah->Lock(0, 0, &pb[1], 0);
					CHECKRESULT(hr == D3D_OK);
					hr = blah->Lock(25, 1, &pb[2], 0);

					CHECKRESULT(hr == D3D_OK);
					CHECKRESULT(pb[0] == pb[1]);
					CHECKRESULT(pb[2] == pb[0] + 25);

					hr = blah->Unlock();
					CHECKRESULT(hr == D3D_OK);
					hr = blah->Unlock();
					CHECKRESULT(hr == D3D_OK);
					hr = blah->Unlock();
					CHECKRESULT(hr == D3D_OK);
				}
			}
		}
	}
	xEndVariation(hlog);
}
ENDTEST()

#pragma data_seg() 
