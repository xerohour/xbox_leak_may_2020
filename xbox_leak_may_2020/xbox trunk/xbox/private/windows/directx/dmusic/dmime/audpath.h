// Copyright (c) 1998-1999 Microsoft Corporation
// DMSStObj.h : Declaration of the CSegState

#ifndef __AUDIOPATH_H_
#define __AUDIOPATH_H_

#include "dmusicip.h"
#include "dmusicf.h"
#include "TrkList.h"
#include "alist.h"
#include "..\shared\dmstrm.h"
#include "dmscriptautguids.h"
#include "..\shared\dmusiccp.h"

class CPerformance;
class CSegState;
class CGraph;
class CAudioPathConfig;
class CBufferConfig;
class CBuffer;
struct IDirectSoundConnect;

class CBufferNode : public AListItem
{
friend class CBufferManager;
friend class CAudioPath;
public:
    CBufferNode();
    ~CBufferNode();
    ULONG AddRef();
    ULONG Release();
    IDirectSoundBuffer *GetBuffer() { if (m_pBuffer) m_pBuffer->AddRef(); return m_pBuffer;}
    HRESULT Activate(BOOL fActivate);
private:
    void FinalDeactivate();
    CBufferNode* GetNext() { return (CBufferNode*)AListItem::GetNext();}
    IDirectSoundBuffer *   m_pBuffer;           // DSound buffer that this manages. 
    DMUS_IO_BUFFER_ATTRIBUTES_HEADER m_BufferHeader;  // GUID that identifies this buffer and flags (can it be shared?).
    CBufferManager * m_pManager; // Pointer to parent list. 
#ifdef XBOX
    DWORD   m_dwMixBin;     // If this represents a Mix Bin instead of a buffer, which one.
    DMUS_IO_BUFFER_MIXBINS_HEADER    m_MixBinsHeader;  // If this is an array of mix bin destinations.
#endif
    long m_lActivateCount;  // How many times this has been activated.
    long m_cRef;            // How many objects point at this.
};

class CBufferManager : public AList
{    

public:
    CBufferManager(); 
    ~CBufferManager() ;
    HRESULT Init(CPerformance *pPerf, DMUS_AUDIOPARAMS *pAudioParams) ;
#ifdef DXAPI
    HRESULT InitSink();
#endif
    void AddHead(CBufferNode* pBufferNode) { AList::AddHead((AListItem*)pBufferNode);}
    CBufferNode* GetHead(){return (CBufferNode*)AList::GetHead();}
    CBufferNode* RemoveHead() {return (CBufferNode *) AList::RemoveHead();}
    void Remove(CBufferNode* pBufferNode){AList::Remove((AListItem*)pBufferNode);}
    void AddTail(CBufferNode* pBufferNode){AList::AddTail((AListItem*)pBufferNode);}
    void Clear();
    void FinalDeactivate();
    CBufferNode * GetBufferNode(REFGUID guidBufferID);
    HRESULT CreateBuffer(CBufferConfig *pConfig, CBufferConfig **pNew); 
private:
    CPerformance *m_pPerf;                  // Pointer to parent performance. 
#ifndef XBOX
    CBufferConfig *m_pFirstBuffer;          // Actual buffer created by config.
#endif
public:
#ifndef XBOX
    IDirectSoundConnect* m_pSinkConnect;    // The sink object which manages creation of buffers in dsound.
    IDirectSoundSynthSink *m_pSynthSink;    // Used to activate sink.
#endif
    DMUS_AUDIOPARAMS m_AudioParams;
};

#ifdef SILVER
#define DSMIXBIN_FRONT_LEFT         0x00000001
#define DSMIXBIN_FRONT_RIGHT        0x00000002
#define DSMIXBIN_FRONT_CENTER       0x00000004
#define DSMIXBIN_LOW_FREQUENCY      0x00000008
#define DSMIXBIN_BACK_LEFT          0x00000010
#define DSMIXBIN_BACK_RIGHT         0x00000020

#define DSMIXBIN_SPEAKER_MASK       0x0000003F

#define DSMIXBIN_XTLK_FRONT_LEFT    0x00000040
#define DSMIXBIN_XTLK_FRONT_RIGHT   0x00000080
#define DSMIXBIN_XTLK_BACK_LEFT     0x00000100
#define DSMIXBIN_XTLK_BACK_RIGHT    0x00000200
#define DSMIXBIN_XTLK_MASK          0x000003C0

#define DSMIXBIN_I3DL2              0x00000400

#define DSMIXBIN_FXSEND_0           0x00000800
#define DSMIXBIN_FXSEND_1           0x00001000
#define DSMIXBIN_FXSEND_2           0x00002000
#define DSMIXBIN_FXSEND_3           0x00004000
#define DSMIXBIN_FXSEND_4           0x00008000
#define DSMIXBIN_FXSEND_5           0x00010000
#define DSMIXBIN_FXSEND_6           0x00020000
#define DSMIXBIN_FXSEND_7           0x00040000
#define DSMIXBIN_FXSEND_8           0x00080000
#define DSMIXBIN_FXSEND_9           0x00100000
#define DSMIXBIN_FXSEND_10          0x00200000
#define DSMIXBIN_FXSEND_11          0x00400000
#define DSMIXBIN_FXSEND_12          0x00800000
#define DSMIXBIN_FXSEND_13          0x01000000
#define DSMIXBIN_FXSEND_14          0x02000000
#define DSMIXBIN_FXSEND_15          0x04000000
#define DSMIXBIN_FXSEND_16          0x08000000
#define DSMIXBIN_FXSEND_17          0x10000000
#define DSMIXBIN_FXSEND_18          0x20000000
#define DSMIXBIN_FXSEND_19          0x40000000
#define DSMIXBIN_FXSEND_MASK        0x7FFFF800
#endif

#define BUFFER_REVERB       1
#define BUFFER_ENVREVERB    2
#define BUFFER_3D           3
#define BUFFER_3D_DRY       4
#define BUFFER_MONO         6
#define BUFFER_STEREO       7
#define BUFFER_MUSIC        8
#define BUFFER_CHORUS       9
#define BUFFER_MIXBINS      10

DEFINE_GUID(GUID_Buffer_Primary,0x186cc544, 0xdb29, 0x11d3, 0x9b, 0xd1, 0x0, 0x80, 0xc7, 0x15, 0xa, 0x74);

#define DMUS_BUFFERF_PRIMARY    4   // This buffer flag is a private definition.

#define DMUS_STANDARD_PATH_DX7  20 // This is for internal use only.

#define MAX_CONNECTIONS     32  // Maximum of 32 buffers connected to one PChannel.

class CBufferConfig : public AListItem
{
public:
    CBufferConfig(DWORD dwType) ;
    ~CBufferConfig();
    void DecideType();         // Looks at embedded GUID and infers default type, if one exists.
    HRESULT Load(IStream *pStream);
    CBufferConfig* GetNext() { return (CBufferConfig*)AListItem::GetNext();}
    IUnknown *              m_pBufferConfig;     // Buffer definition. 
    CBufferNode *           m_pBufferNode;      // Manages the DSound buffer.
    DMUS_IO_BUFFER_ATTRIBUTES_HEADER m_BufferHeader;  // GUID that identifies this buffer and flags.
    DWORD                   m_dwStandardBufferID;// Optionally, one of the standard buffer types.
    HRESULT Activate(BOOL fActivate);
#ifdef XBOX
    DMUS_IO_BUFFER_MIXBINS_HEADER    m_MixBinsHeader;  // If this is an array of mix bin destinations.
#endif
};

class CBufferConfigList : public AList
{
public:
    void AddHead(CBufferConfig* pBufferConfig) { AList::AddHead((AListItem*)pBufferConfig);}
    CBufferConfig* GetHead(){return (CBufferConfig*)AList::GetHead();}
    CBufferConfig* RemoveHead() {return (CBufferConfig *) AList::RemoveHead();}
    void Remove(CBufferConfig* pBufferConfig){AList::Remove((AListItem*)pBufferConfig);}
    void AddTail(CBufferConfig* pBufferConfig){AList::AddTail((AListItem*)pBufferConfig);}
    void Clear();
    CBufferNode * GetBufferNode(REFGUID guidBufferID);
    HRESULT CreateRunTimeVersion(CBufferConfigList *pCopy, CBufferManager *pManager);
    HRESULT Activate(BOOL fActivate);
};

class CPortConfig;

class CBufferConnect : public AListItem
{
public:
    CBufferConnect() ;
    ~CBufferConnect();
    CBufferConnect *CreateRunTimeVersion(CPortConfig *pParent);
    HRESULT Load(CRiffParser *pParser);
    CBufferConnect* GetNext() { return (CBufferConnect*)AListItem::GetNext();}
    DMUS_IO_PCHANNELTOBUFFER_HEADER m_ConnectHeader;
    GUID *                          m_pguidBufferIDs;// Set of guids identifying buffers to connect to.
    CBufferNode **                  m_ppBufferNodes; // Array of equivalent Buffer nodes. 
};

class CBufferConnectList : public AList
{
public:
    void AddHead(CBufferConnect* pBufferConnect) { AList::AddHead((AListItem*)pBufferConnect);}
    CBufferConnect* GetHead(){return (CBufferConnect*)AList::GetHead();}
    CBufferConnect* RemoveHead() {return (CBufferConnect *) AList::RemoveHead();}
    void Remove(CBufferConnect* pBufferConnect){AList::Remove((AListItem*)pBufferConnect);}
    void AddTail(CBufferConnect* pBufferConnect){AList::AddTail((AListItem*)pBufferConnect);}
    HRESULT CreateRunTimeVersion(CBufferConnectList *pDestination, CPortConfig *pParent);
    void Clear();
};

class CAudioPath;

class CPortConfig : public AListItem
{
public:
    CPortConfig() ;
    ~CPortConfig();
    HRESULT CreateRunTimeVersion(CPortConfig ** ppCopy, CAudioPath *pParent,CBufferManager *pManager);
    CPortConfig* GetNext() { return (CPortConfig*)AListItem::GetNext();}
    CBufferNode * GetBufferNode(REFGUID guidBuffer);
    HRESULT Activate(BOOL fActivate);
    HRESULT CPortConfig::Load(CRiffParser *pParser);
    DMUS_IO_PORTCONFIG_HEADER   m_PortHeader; // Port configuration header, from file.
    DMUS_PORTPARAMS8            m_PortParams; // Port params for opening the port, also from file. 
    DWORD                       m_dwPortID;   // Used to track the index of a created port, once connecting.
    CBufferConnectList          m_BufferConnectList; // Pchannel to buffer mappings. 
    CBufferConfigList           m_BufferConfigList; // List of dsound buffer configurations.
    CAudioPath *                m_pParent;    // Parent audio path.
    IDirectMusicPort *         m_pPort;      // Pointer to port, if run time version.
	BOOL						m_fAlreadyHere; // Check for recursive case looking for buffer nodes.
};

class CPortConfigList : public AList
{
public:
    void AddHead(CPortConfig* pPortConfig) { AList::AddHead((AListItem*)pPortConfig);}
    CPortConfig* GetHead(){return (CPortConfig*)AList::GetHead();}
    CPortConfig* RemoveHead() {return (CPortConfig *) AList::RemoveHead();}
    void Remove(CPortConfig* pPortConfig){AList::Remove((AListItem*)pPortConfig);}
    void AddTail(CPortConfig* pPortConfig){AList::AddTail((AListItem*)pPortConfig);}
    HRESULT CreateRunTimeVersion(CPortConfigList *pDestination,CAudioPath *pParent,CBufferManager *pManager);
    void Clear();
    HRESULT Activate(BOOL fActivate);
    BOOL UsesPort(IDirectMusicPort *pPort);
};

class CAudioPath;

DEFINE_GUID(IID_CAudioPath,0xb06c0c23, 0xd3c7, 0x11d3, 0x9b, 0xd1, 0x44, 0x45, 0x53, 0x54, 0x0, 0x0);


/*  The AudioPath represents a configuration of pchannels, ports, and buffers to play
    segments. The performance creates CAudioPath objects from CAudioPathConfig file
    objects and stores them in a linked list, which it uses to keep track of them.
*/

class CAudioPath : 
    public IDirectMusicAudioPath, 
	public IDirectMusicGraph,
    public AListItem
{
public:
    CAudioPath();
    ~CAudioPath();
    CAudioPath* GetNext() { return (CAudioPath*)AListItem::GetNext();}
// IUnknown
    STDMETHODIMP QueryInterface(const IID &iid, void **ppv);
    STDMETHODIMP_(ULONG) AddRef();
    STDMETHODIMP_(ULONG) Release();

// IDirectMusicAudioPath 
    STDMETHODIMP GetObjectInPath( DWORD dwPChannel,DWORD dwStage,DWORD dwBuffer, REFGUID guidObject,
                    DWORD dwIndex,REFGUID iidInterface, void ** ppObject);
    STDMETHODIMP Activate(BOOL fActivate) ;
    STDMETHODIMP SetVolume(long lVolume,DWORD dwDuration) ;
    STDMETHODIMP ConvertPChannel( DWORD dwPChannelIn,DWORD *pdwPChannelOut) ; 
#ifdef XBOX
    STDMETHODIMP SetPitch(long lPitch,DWORD dwDuration) ;
#endif

// IDirectMusicGraph
	STDMETHODIMP Shutdown();
    STDMETHODIMP InsertTool(IDirectMusicTool *pTool,DWORD *pdwPChannels,DWORD cPChannels,LONG lIndex);
    STDMETHODIMP GetTool(DWORD,IDirectMusicTool**);
    STDMETHODIMP RemoveTool(IDirectMusicTool*);
    STDMETHODIMP StampPMsg(DMUS_PMSG* pEvent);

    HRESULT Init(IUnknown *pSourceConfig,CPerformance *pPerf);
    HRESULT ConnectToPorts(CPerformance *pPerf,DWORD dwSampleRate);
    CBufferNode * GetBufferNode(REFGUID guidBuffer);
    void SetGraph(CGraph *pGraph);
    CGraph *GetGraph();
    void Deactivate();
    BOOL IsActive() { return m_fActive; }
    BOOL NoPorts() { return m_PortConfigList.IsEmpty(); }
    BOOL UsesPort(IDirectMusicPort *pPort) { return m_PortConfigList.UsesPort(pPort); }
private:
    CMemTrack           m_MemTrack;         // Used for tracking memory allocations.
	DWORD				m_dwTrackID;		// Each Audiopath is assigned a track id for invalidations of broadcast messages.
    REFERENCE_TIME      m_rtLastVolChange;  // End point of last volume change request.
    REFERENCE_TIME      m_rtLastPitchChange;// End point of last pitch change request.
    BOOL                m_fDeactivating;    // State variable.
//    BYTE                m_bLastVol;         // Last volume change.
    CRITICAL_SECTION    m_CriticalSection;
	BOOL				m_fActive;			// Indicates that the audio path is active.
    long			    m_cRef;             // Reference counter. 
    DWORD *             m_pdwVChannels;     // Array of source VChannels.
    DWORD *             m_pdwPChannels;     // Array of destination PChannels.
    DWORD               m_dwChannelCount;   // Size of arrays.
    CGraph *            m_pGraph;           // Route tool graph.
    CPerformance *      m_pPerformance;     // Pointer to performance, needed to release PChannels.
    CAudioPathConfig *  m_pConfig;          // Source configuration.
    CPortConfigList     m_PortConfigList;   // List of port definitions (copied from source.)
    CBufferConfigList   m_BufferConfigList; // List of DSound buffers connected to the ports.
    IUnknown *          m_pUnkDispatch;     // holds the controlling unknown of the scripting object that implements IDispatch
};

class CAudioPathList : public AList
{
public:
    void Clear();
    void AddHead(CAudioPath* pAudioPath) { AList::AddHead((AListItem*)pAudioPath);}
    CAudioPath* GetHead(){return (CAudioPath*)AList::GetHead();}
    CAudioPath* RemoveHead() {return (CAudioPath *) AList::RemoveHead();}
    void Remove(CAudioPath* pAudioPath){AList::Remove((AListItem*)pAudioPath);}
    void AddTail(CAudioPath* pAudioPath){AList::AddTail((AListItem*)pAudioPath);}
    BOOL IsMember(CAudioPath* pAudioPath){return AList::IsMember((AListItem*)pAudioPath);}
    BOOL UsesPort(IDirectMusicPort *pPort); 
    CBufferNode * GetBufferNode(REFGUID guidBufferID);
};

DEFINE_GUID(IID_CAudioPathConfig,0xe9874261, 0xab52, 0x11d3, 0x9b, 0xd1, 0x44, 0x45, 0x53, 0x54, 0x0, 0x0);

class CAudioPathConfig : CMemTrack,
    public IPersistStream, 
    public IDirectMusicObject
{
friend class CAudioPath;
public:
    CAudioPathConfig();
    ~CAudioPathConfig();
    static CAudioPathConfig *CreateStandardConfig(DWORD dwType,DWORD dwPChannelCount,DWORD dwSampleRate);


// IUnknown
    STDMETHODIMP QueryInterface(const IID &iid, void **ppv);
    STDMETHODIMP_(ULONG) AddRef();
    STDMETHODIMP_(ULONG) Release();

// IPersist functions
    STDMETHODIMP GetClassID( CLSID* pClsId );
// IPersistStream functions
    STDMETHODIMP IsDirty();
    STDMETHODIMP Load( IStream* pIStream );
    STDMETHODIMP Save( IStream* pIStream, BOOL fClearDirty );
    STDMETHODIMP GetSizeMax( ULARGE_INTEGER FAR* pcbSize );

// IDirectMusicObject 
	STDMETHODIMP GetDescriptor(LPDMUS_OBJECTDESC pDesc);
	STDMETHODIMP SetDescriptor(LPDMUS_OBJECTDESC pDesc);
	STDMETHODIMP ParseDescriptor(LPSTREAM pStream, LPDMUS_OBJECTDESC pDesc);

    HRESULT Load( CRiffParser *pParser);

protected:
    CGraph *            m_pGraph;                   // Audio path tool graph.
    CPortConfigList     m_PortConfigList;           // List of port definitions.
    CBufferConfigList   m_BufferConfigList;         // List of global dsound buffer configurations.
    CRITICAL_SECTION    m_CriticalSection;
	long		        m_cRef;
// IDirectMusicObject variables
    CInfo               m_Info;
//	DWORD	            m_dwValidData;
//	GUID	            m_guidObject;
//	FILETIME	        m_ftDate;                       /* Last edited date of object. */
//	DMUS_VERSION	    m_vVersion;                 /* Version. */
//	WCHAR	            m_wszName[DMUS_MAX_NAME];			/* Name of object.       */
//	WCHAR	            m_wszCategory[DMUS_MAX_CATEGORY];	/* Category for object */
//	WCHAR               m_wszFileName[DMUS_MAX_FILENAME];	/* File path. */
    IUnknown *          m_pUnkDispatch; // holds the controlling unknown of the scripting object that implements IDispatch
};

#endif // __AUDIOPATH_H_
