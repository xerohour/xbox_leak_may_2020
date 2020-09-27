#ifndef __IIV5_H__
#define __IIV5_H__

/*
 *      IIV5.H
 *
 *  This file defines the COM interface IInfoViewer5.  This interface
 *  is used to access functionality of an individual InfoViewer title
 *  other than that available by binding to particular items.
 *
 *  This interface should be accessed by creating a moniker for the
 *  desired InfoViewer title (@ivt://id) and calling BindToObject.
 *
 *  Subfiles may be accessed by creating monikers for those
 *  files (@ivt://id/file) and calling BindToStorage to allocate
 *  an IStream.
 *
 *  Both IInfoViewer5's and IStream's are closed by calling Release.
 *
 *  IInfoViewer5Callback is used by controls to communicate
 *  with the InfoViewer5 application itself.
 *
 *  IInfoViewer5TitleCallback is used by the moniker to communicate
 *  with InfoViewer.
 */

typedef struct tagIVPROMPTINFO
{
    DWORD cbSize;
    DWORD dwFlags;
    LPOLESTR pwzTitleId;
    LPOLESTR pwzTitleName;
    LPOLESTR pwzTitleFile;
    LPOLESTR pwzVolumeId;
    LPOLESTR pwzVolumeName;
    LPOLESTR pwzLocation;
    DWORD cchMaxLocation;
} IVPROMPTINFO;

#define IIV5PROMPT_CONTENT          0x00000001
#define IIV5PROMPT_STRUCTURE        0x00000002
#define IIV5PROMPT_FTS              0x00000004
#define IIV5PROMPT_SINGLEFILE       0x00000008
#define IIV5PROMPT_ALREADYOPEN      0x00000100
#define IIV5PROMPT_LOCATIONCHANGED  0x00001000
#define IIV5PROMPT_NOPROMPT         0x00002000
#define IIV5PROMPT_NOCHECK          0x00004000

interface IInfoViewer5Callback : public IUnknown
{
public:
    STDMETHOD(ExecuteKLink)(LPOLESTR lpwzKeyword, LPOLESTR lpwzDefault) = 0;
    STDMETHOD(ExecuteALink)(LPOLESTR lpwzId, LPOLESTR lpwzDefault) = 0;
    STDMETHOD(GetSampleDir)(LPOLESTR lpwzSampleDir) = 0;
    STDMETHOD(SetSampleDir)(LPOLESTR lpwzSampleDir) = 0;
    STDMETHOD(ViewSample)(LPOLESTR lpwzFileName) = 0;
};

interface IInfoViewer5TitleCallback : public IUnknown
{
    STDMETHOD(Prompt)(IVPROMPTINFO * ppi) = 0;
    STDMETHOD(PollQueryContinue)() = 0;
};

interface IInfoViewer5 : public IUnknown
{
public:
    STDMETHOD(OpenSubfile)(LPOLESTR lpwzItem, IStream ** ppstm) = 0;
    STDMETHOD(LoadSubfile)(LPOLESTR lpwzItem, void ** ppdata, DWORD * pcb) = 0;
    STDMETHOD(FullTextSearch)(LPOLESTR lpwzQuery, int iOptions, int iNear, DWORD ** ppResults) = 0;
    STDMETHOD(SetTitleCallback)(IInfoViewer5TitleCallback * pcb) = 0;
    STDMETHOD(ConfirmFile)(DWORD dwFlags) = 0;
};

#endif

// { DCBF8E3x-9A4F-11CF-928E-00AA0057AD67 }
//
DEFINE_GUID( CLSID_IVT,
             0xdcbf8e30, 0x9a4f, 0x11cf,
             0x92, 0x8e, 0x00, 0xaa, 0x00, 0x57, 0xad, 0x67 );

DEFINE_GUID( CLSID_IVTMON,
             0xDCBF8E31, 0x9A4F, 0x11CF,
             0x92, 0x8e, 0x00, 0xaa, 0x00, 0x57, 0xad, 0x67 );

DEFINE_GUID( IID_IInfoViewer5Callback,
             0xDCBF8E32, 0x9A4F, 0x11CF,
             0x92, 0x8e, 0x00, 0xaa, 0x00, 0x57, 0xad, 0x67 );

DEFINE_GUID( IID_IInfoViewer5,
             0xDCBF8E33, 0x9A4F, 0x11CF,
             0x92, 0x8e, 0x00, 0xaa, 0x00, 0x57, 0xad, 0x67 );

DEFINE_GUID( IID_IInfoViewer5TitleCallback,
             0xDCBF8E34, 0x9A4F, 0x11CF,
             0x92, 0x8e, 0x00, 0xaa, 0x00, 0x57, 0xad, 0x67 );

// special error code from FullTextSearch for invalid query syntax
//
#define IV_ERR_SYNTAX   _HRESULT_TYPEDEF_(0x8CAC0001)
#define IV_ERR_NOINDEX  _HRESULT_TYPEDEF_(0x8CAC0002)
