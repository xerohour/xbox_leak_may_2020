#include "std.h"
#include "xapp.h"

// privately exposed functions from Xbox D3D
extern "C" void* WINAPI D3D_AllocNoncontiguousMemory(DWORD Size);
extern "C" void WINAPI D3D_FreeNoncontiguousMemory(void *pMemory);

extern void CleanupImageCache();
extern bool CleanupTextureCache();
extern bool CleanupMeshCache();

int g_nNewFailedState;

int __cdecl NewFailed(size_t nBytes)
{
	TRACE(_T("\001new failed to allocate %d bytes!\n"), nBytes);

	switch (g_nNewFailedState)
	{
	case 0:
		TRACE(_T("\002Cleaning the texture cache!\n"));
		if (CleanupTextureCache())
			return 1;
		break;

	case 1:
		TRACE(_T("\002Cleaning the mesh cache!\n"));
		if (CleanupMeshCache())
			return 1;
		break;

	default:
		// We've run out of things to try!
        DbgPrint("xdash: out of memory (failed to allocate %d bytes)\n", nBytes);
#ifdef DEVKIT
        __asm int 3;
#endif
        HalReturnToFirmware(HalRebootRoutine);
	}

	g_nNewFailedState += 1;
	return 1; // keep trying!
}

void* XAppAllocMemory(int nBytes)
{
	for (;;)
	{
		void* pv = (void*)GlobalAlloc(GMEM_FIXED, nBytes);
		if (pv != NULL)
			return pv;

		if (NewFailed(nBytes) == 0)
			return NULL;
	}
}

void XAppFreeMemory(void* pv)
{
	if (pv != NULL)
		GlobalFree((HGLOBAL)pv);
}

void* XAppD3D_AllocContiguousMemory(DWORD Size, DWORD Alignment)
{
	for (;;)
	{
		void* pv = D3D_AllocContiguousMemory(Size, Alignment);
		if (pv != NULL)
			return pv;

		if (NewFailed(Size) == 0)
			return NULL;
	}
}

void* XAppD3D_AllocNoncontiguousMemory(DWORD Size)
{
	for (;;)
	{
		void* pv = D3D_AllocNoncontiguousMemory(Size);
		if (pv != NULL)
			return pv;

		if (NewFailed(Size) == 0)
			return NULL;
	}
}

void XAppCreateMeshFVF(DWORD NumFaces, DWORD NumVertices, DWORD Options, DWORD FVF, LPD3DXMESH* ppMesh)
{
	for (;;)
	{
		HRESULT hr = D3DXCreateMeshFVF(NumFaces, NumVertices, Options, FVF, XAppGetD3DDev(), ppMesh);
		if (hr != E_OUTOFMEMORY)
		{
			ASSERTHR(hr);
			return;
		}

		if (NewFailed(NumVertices * 32 + NumFaces * 6) == 0)
			return;
	}
}

void Memory_Init()
{
	// Setup our memory allocation failure hook...
	_set_new_handler(NewFailed);
	_set_new_mode(1);
	g_nNewFailedState = 0;
}

