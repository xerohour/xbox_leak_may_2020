#pragma once

HRESULT StartDSStress(DWORD dwThreads, DWORD dwBuffers2D, DWORD dwBuffers3D, DWORD dwStreams2D, DWORD dwStreams3D, DWORD dwMixinBuffers2D, DWORD dwMixinBuffers3D, DWORD dwSends2D, DWORD dwSends3D, DWORD dwWait);
HRESULT StopDSStress(void);
