

BOOL CALLBACK EnumDevicesCallback(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef);
/*
class CXidDevice
{
	friend class CXidDeviceCollection;
	public:
		CXidDevice(PCINPUT_DEVICE_INFORMATION pDeviceInfo);
		~CXidDevice();
		void	Open();
		void	Close();
		BOOL	IsDevice(PCINPUT_DEVICE_INFORMATION pDeviceInfo);
		void	PollDevice();
		BOOL	ThresholdCheck();
	private:
		INPUT_DEVICE_INFORMATION	m_DeviceInfo;
		DWORD						m_dwDeviceHandle;
		CXidDevice					*m_pPrevDevice;
		CXidDevice					*m_pNextDevice;
		INPUT_FORMAT_GAME			m_InputBuffer;
		INPUT_FORMAT_GAME			m_InputBufferLast;
		DWORD						m_dwPacketNumber;
		DWORD						m_dwIdlePollCount;
};


class CXidDeviceCollection
{
	public:
		CXidDeviceCollection();
		~CXidDeviceCollection();
		void	Add(PCINPUT_DEVICE_INFORMATION pDeviceInfo);
		void	Remove(PCINPUT_DEVICE_INFORMATION pDeviceInfo);
		void	PollDevices();
		//void	PrintDeviceCount();
		//void	PrintDeviceList();
	private:
		int			m_iDeviceCount;
		CXidDevice	*m_pDevices;
};

void DeviceChanges(
	PCINPUT_DEVICE_INFORMATION DeviceInformation,
	BOOL fAdd,
	BOOL fEnum,
	CXidDeviceCollection *pDeviceCollection
	);

void DumpDeviceInfo(PCINPUT_DEVICE_INFORMATION pDeviceInfo);

void DumpDeviceState(PINPUT_FORMAT_GAME pGamePacket);
*/