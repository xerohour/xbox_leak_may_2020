//DOSRM_AU.H contains all the header information for both the
//MPU401 VxD and the ResMan VxD.

// NoteOff priority
//
#define URGENT  0
#define NORMAL 1

#define MAX_NUMVOICES MAX_AUPATCH_INPUT		//max. number of notes for the DOS VxD.

#define	DOSRM_USERNAME	0x6969		//just a random number.

//************ Cut from NV32.H ******************************
	/***** Notification Status Values *****/
	/*
	 * NV sets NvNotification.status to NV_STATUS_DONE_OK if the operation
	 * succeeded.
	 */
	#define NV_STATUS_DONE_OK          0
	/*
	 * Client sets NvNotification.status to NV_STATUS_IN_PROGRESS, NV 
	 * sets to a different value when it notifies.
	 */
	#define NV_STATUS_IN_PROGRESS      255
	/***** NvNotification Structure *****/
	/*
	 * NV objects return information about method completion to clients 
	 * via notification structures in main memory.
	 *
	 * The client sets the status field to NV_STATUS_IN_PROGRESS.
	 * NV sets it to something different when it notifies, after setting 
	 * other fields.  NV fills in the timeStamp; the contents and 
	 * interpretation of the statusInfo, otherInfo32, and OtherInfo16 
	 * fields depend on the status value.  See NV_STATUS_ elsewhere.
	 */
	typedef volatile struct {
	 struct tagNvTimeStamp {
	  U032 nanoseconds[2];           /* Nanoseconds since Jan. 1, 1970 */
	 } timeStamp;                  /*                                 0000-0007*/
	 V032 otherInfo32;             /* info reported depends on method 0008-000b*/
	 V016 otherInfo16;             /* info reported depends on method 000c-000d*/
	 V008 statusInfo;              /* ERROR1 code if (status==ERROR1) 000e-000e*/
	 V008 status;                  /* client sets, NV changes         000f-000f*/
	} NvNotification;

//******************************************

typedef struct {
	NvNotification BufNotify;
} DOSRM_NOTE_STRUC;

//List of possible parameter types accepted by NVChangeParam_HW()
#define VOLUME				0
#define SAMPLE_RATE			1
#define VIBRATO				3
#define TREMOLO				4
#define EFFECTS				5

#define VOICE_UNUSED	(0x0FF)

