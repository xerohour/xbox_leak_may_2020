#include <windows.h>
#include <atlbase.h>
#include <xsum.h>
#include <tape.h>
#include <dx2ml.h>
#include <crypt.h>
#include <xctrl.h>
#include <emucore.h>

class CTapeDevice2 : public CTapeDevice
{
    friend class CBasedFile;
};

class CBasedFile : public CDiskFile
{
public:
    CBasedFile(CTapeDevice2 *ptap);
    CBasedFile(CDiskFile *pstm);
    virtual BOOL FSeek(LARGE_INTEGER li);
    virtual BOOL FSeek(LONG l);
private:
    LARGE_INTEGER m_liBase;
};

class CTapeEmulation : public CEmulationFilesystem
{
public:
    virtual void MountUnmount(BOOL fMount);
    virtual void GetPhysicalGeometry(PHYSICAL_GEOMETRY *, BOOL);
    virtual int NumberOfPlaceholders(int Layer);
    virtual void GetPlaceholderInformation(int Layer,
        PLACEHOLDER *Placeholders);
    virtual ULONG GetSectorData(ULONG lsn, ULONG cblk, PUCHAR rgbData);
};

class CTapeEmulationDVDX2 : public CTapeEmulation
{
public:
	virtual BOOL GetControlData(BOOL fXControl, PUCHAR pb, ULONG cb,
		PULARGE_INTEGER Delay);
    virtual BOOL DiscChallengeResponse(UCHAR id, PULONG ChallengeValue,
        PULONG ResponseValue, PULARGE_INTEGER Delay);
};
