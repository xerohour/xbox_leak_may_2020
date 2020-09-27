#ifndef __IOJAZZDOC_H__
#define __IOJAZZDOC_H__

// DirectMusic Producer Project structures


#define FOURCC_PROJECT_FORM				mmioFOURCC('J','A','Z','P')
#define FOURCC_PROJECT_LIST				mmioFOURCC('p','r','o','j')
#define FOURCC_PROJECT_CHUNK			mmioFOURCC('p','j','c','t')
#define FOURCC_PROJECT_PCHANNEL_NAMES	mmioFOURCC('p','j','p','n')
#define FOURCC_OPEN_EDITORS_LIST		mmioFOURCC('o','p','e','n')
#define FOURCC_BOOKMARK_LIST			mmioFOURCC('b','k','m','k')
#define FOURCC_BOOKMARK_EDITOR_LIST		mmioFOURCC('e','d','t','r')
#define FOURCC_BOOKMARK_EDITOR_WP_CHUNK	mmioFOURCC('e','d','t','w')
#define FOURCC_BOOKMARK_COMPONENT_LIST	mmioFOURCC('c','o','m','p')
#define FOURCC_BOOKMARK_COMPONENT_CHUNK	mmioFOURCC('c','o','m','h')
#define FOURCC_FILE_LIST				mmioFOURCC('f','i','l','e')
#define FOURCC_FILE_CHUNK				mmioFOURCC('f','i','l','h')
#define FOURCC_NOTIFY_CHUNK				mmioFOURCC('n','t','f','y')
#define FOURCC_NODE_LIST				mmioFOURCC('n','o','d','e')
#define FOURCC_NAME_CHUNK				mmioFOURCC('n','a','m','e')
#define FOURCC_EDITOR_WP_CHUNK			mmioFOURCC('e','d','w','p')
#define FOURCC_RUNTIME_FOLDERS_LIST		mmioFOURCC('r','f','l','d')
#define FOURCC_RUNTIME_FOLDER_LIST		mmioFOURCC('f','l','d','r')
#define FOURCC_PATH_CHUNK				mmioFOURCC('p','a','t','h')
#define FOURCC_FILTER_CHUNK				mmioFOURCC('f','l','t','r')
#define FOURCC_GUID_CHUNK				mmioFOURCC('g','u','i','d')


#define FOURCC_INFO_LIST				mmioFOURCC('I','N','F','O')
#define FOURCC_UNFO_LIST				mmioFOURCC('U','N','F','O')
#define FOURCC_UNAM_CHUNK				mmioFOURCC('U','N','A','M')
#define FOURCC_UNFO_COMMENT				mmioFOURCC('c','m','n','t')
#define FOURCC_UNFO_NODE_NAME			mmioFOURCC('n','n','a','m')
#define FOURCC_UNFO_NODE_DESC			mmioFOURCC('n','d','s','c')
#define FOURCC_UNFO_RUNTIME_FILE		mmioFOURCC('r','f','i','l')
#define FOURCC_UNFO_RUNTIME_FOLDER		mmioFOURCC('r','d','i','r')
#define FOURCC_UNFO_RUNTIME_NAME		mmioFOURCC('r','n','a','m')

#pragma pack(2)

struct ioJzProject
{
	GUID				m_guidProject;		// Every Project has a GUID
};

struct ioJzFile
{
	GUID				m_guidFile;				// Every file has a GUID
	FILETIME			m_ftModified;			// Modified time of file
	DWORD				m_dwSize;				// Size of file
	GUID				m_guidListInfoObject;	// GUID for file's DMUSProdListInfo info 
};

struct ioJzNode
{
	GUID				m_guidNodeId;		// Every node has an ID; i.e. GUID_StyleNode
	long				m_lTreePos;			// Position of this node in this file
	WINDOWPLACEMENT		m_wpEditor;			// Window placement info for this node's editor
};

struct ioPChannelName
{
	DWORD				m_dwPChannel;		// PChannel number
	DWORD				m_dwStrCount;		// Number of WCHARs in PChannel name, including NULL
	WCHAR*				m_pwstrName;		// Null-terminated PChannel name
};

#pragma pack()


/*
RIFF
(
	'JAZP'				// DirectMusic Producer Project File
	<proj-list>			// Project information 
	[<file-list>]		// Files in Project
)

	// <proj-list>
	proj
	(
		<pjct-ck>			// Project chunk 
		[<INFO-list>]		// Comments
		[<pjpn-ck>]			// PChannel name
	)

		// <pjct-ck>
		pjct
		(
			WORD sizeof(ioJzProject)
			<ioJzProject>
			Username (unicode)
		)

		// [<UNFO-list>]
		LIST
		(
			'UNFO'
			[<cmmt>]		// Comments (FOURCC_UNFO_COMMENT)
		)

		// [<pjpn-ck]
		pjpn
		(
			<ioPChannelName>	// Any number of ioPChannelName structures
		)

	// [<file-list>]
	LIST
	(
		'file'
		<name-ck>		// Filename (relative path)
		<filh-ck>		// File header chunk
		[<ntfy-ck>]		// List of GUIDs for files in this file's notify list
		[<INFO-list>]	// NodeName, NodeDescriptor
		[<node-list>]	// List of WINDOWPLACEMENT info for individual node editors
	)

		// <name-ck>
		name
		(
			Filename - relative path (unicode)
		)

		// <filh-ck>
		filh
		(
			<ioJzFile>
		)

		// [<ntfy-ck>]
		'ntfy'
		(
			// GUID's for all files in this file's notify list
		)

		// [<INFO-list>]
		LIST
		(
			'INFO'
			[<nnam>]	// Node Name
			[<ndsc>]	// Node Descriptor
		)

		// [<node-list>]
		LIST
		(
			'node'
			<name-ck>		// Node name
			<edwp-ck>		// Associated editor's WINDOWPLACEMENT information
		)

			// <name-ck>
			name
			(
				Node name (unicode)
			)

			// <edwp-ck>
			edwp
			(
				<ioJzNode>
			)
*/

#endif // __IOJAZZDOC_H__
