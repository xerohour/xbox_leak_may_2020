#pragma once


class DSoundManager 
  {
  public:
      static DSoundManager* Instance();
	  ~DSoundManager();
	  HRESULT Initialize();
	  HRESULT Cleanup();
	  HRESULT DSoundCreateSoundBuffer(IN WAVEFORMATEX* pwfx, IN int nByteCount, IN DWORD dwFlags, OUT LPDIRECTSOUNDBUFFER* pDirectBuf);
	  HRESULT DSoundSetSoundBufferData(LPDIRECTSOUNDBUFFER pDirectSoundBuffer, UINT nByteOffset, UINT nByteCount, const void* pvData);
	  HRESULT DSoundSetSoundBufferNotify(LPDIRECTSOUNDBUFFER pDirectSoundBuffer, int nPositionCount, DSBPOSITIONNOTIFY* positions);
	  
	  bool m_bShutdown;

  protected:
      DSoundManager();
      DSoundManager(const DSoundManager&);
      DSoundManager& operator= (const DSoundManager&);
  private:

      static DSoundManager* pinstance;
	  LPDIRECTSOUND8 m_pDirectSound;
	  HANDLE m_hDirectSoundThread;


 };


