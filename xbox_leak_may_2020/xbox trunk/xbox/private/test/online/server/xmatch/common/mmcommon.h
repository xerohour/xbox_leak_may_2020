// mmcommon.h : Defines various global defaults for the mmcommon program
//
#define MMTEST_AGENT_NAME		TEXT("XboxMMTest")
#define CONTENT_LENGTH_HEADER	TEXT("Content-Length:")

#define SAMPLE_ATTRIB1_ID		0xAAAA
#define SAMPLE_ATTRIB1_STRING	"Attrib string"
#define SAMPLE_ATTRIB1_INT		12345

#define SAMPLE_PARAM1_STRING	"Parameter string 1.."

#define SAMPLE_PUID1			0x0000000000000001
#define SAMPLE_PUID2			0x0000000000000002
#define SAMPLE_PUID3			0x0000000000000003
#define SAMPLE_PUID4			0x0000000000000004

typedef struct _ATTRIB_LIST
{
	BYTE				*pAttribBuffer;
	DWORD				dwAttribLen;
	struct _ATTRIB_LIST	*pNext;
} ATTRIB_LIST, *PATTRIB_LIST;