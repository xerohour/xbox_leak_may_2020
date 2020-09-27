/*
 *
 * emucore.h
 *
 * Core emulation stuff
 *
 */

#ifndef _EMUCORE_H
#define _EMUCORE_H

struct PLACEHOLDER {
    ULONG StartLSN;
    ULONG SectorCount;
};

struct PHYSICAL_GEOMETRY {
    ULONG Layer0StartPSN;
    ULONG Layer0SectorCount;
    ULONG Layer1SectorCount;
    BOOL IsX2Disc;
	BOOL SupportsMultiplePartitions;
};

class CEmulationFilesystem;

HRESULT StartEmulating(void);
HRESULT EmulationOpenTray(BOOL fWait);
HRESULT EmulationCloseTray(CEmulationFilesystem *, BOOL fWait);
HRESULT StopEmulating(void);
void AccumulateSeekDelay(ULONG PSN, PULARGE_INTEGER Delay);
BOOL DefaultChallengeResponse(UCHAR id, PULONG ChallengeValue,
    PULONG ResponseValue, PULARGE_INTEGER Delay);
BOOL DefaultControlData(PUCHAR, ULONG, PULARGE_INTEGER);

class CEmulationFilesystem
{
public:
    /* MountUnmount is called whenever the virtual disc is "physically"
     * inserted or removed from the device tray */
    virtual void MountUnmount(BOOL fMount) =0;

    /* GetPhysicalGeometry returns the physical geometry of the device */
    virtual void GetPhysicalGeometry(PHYSICAL_GEOMETRY *,
		BOOL fExtendedPartition) =0;

    /* NumberOfPlaceholders returns the number of placeholders on a
     * layer */
    virtual int NumberOfPlaceholders(int Layer) =0;

    /* GetPlaceholderInformation fills in an array of placeholder structures
     * for a given layer */
    virtual void GetPlaceholderInformation(int Layer,
        PLACEHOLDER *Placeholders) =0;
    
    /* GetSectorData fills in rgbData with (cblk * 2kb) data and returns 0 if
     * successful or one of the error codes if unsuccessful */
    virtual ULONG GetSectorData(ULONG lsn, ULONG cblk, PUCHAR rgbData) =0;

	/* GetControlData returns the DVDX2 control data for the disc */
	virtual BOOL GetControlData(BOOL fXControl, PUCHAR pb, ULONG cb,
		PULARGE_INTEGER Delay)
	{
		return DefaultControlData(pb, cb, Delay);
	}

	/* DiscChallengeResponse performs a DVDX2 authentication challenge */
    virtual BOOL DiscChallengeResponse(UCHAR id, PULONG ChallengeValue,
        PULONG ResponseValue, PULARGE_INTEGER Delay)
    {
        return DefaultChallengeResponse(id, ChallengeValue, ResponseValue,
            Delay);
    }
};

enum {
    EMU_ERR_SUCCESS = 0,
    EMU_ERR_PLACEHOLDER,
    EMU_ERR_INVALID_LSN,
    EMU_ERR_UNEXPECTED_ERROR
};

#endif // EMUCORE_H
