//===================================================================
//
// Copyright Microsoft Corporation. All Rights Reserved.
//
//===================================================================

#ifndef __METADATA_H__
#define __METADATA_H__

// ==================================================
// Metadata manager
//

//
// Define the section name for the common section
//
#define MD_COMMON_SECTION_NAME		"all"

//
// Define the supported metadata types
//
typedef enum
{
	mdtBool = 0,
	mdtInt8,
	mdtInt16,
	mdtInt32,
	mdtString,
	mdtDate,
	mdtBinary,
	mdtBinaryFromFile,
	mdtMax

} MD_DATA_TYPE;

//
// Define a union for metadata values
//
typedef union
{
	BOOL		f;		// Bool
	BYTE		b;		// Int8
	WORD		w;		// Int16
	DWORD		dw;		// Int32
	struct
	{
		PSTR	sz;		// String
		DWORD	cch;	// Length
	};
	FILETIME	ft;		// Date
	struct 				// Binary data
	{
		PBYTE	pb;		// Data
		DWORD	cb;		// Length
	};

} MD_DATA, *PMD_DATA;

//
// Define a structure for property-schema-description mapping
//
typedef struct
{
	
	PSTR			szPropertyName;	// Name
	PSTR			szColumnName;	// SQL column name
	MD_DATA_TYPE	mdt;			// Data type
	DWORD			cbMaxSize;		// Max size (variable types only)
	DWORD			dwFlags;		// Flags
	DWORD			dwQuerySet;		// Set of queries containing this prop
	PSTR			szDescription;	// Descriptive text
	PMD_DATA		pmddDefault;	// Optional default value

} MD_DESCRIPTION, *PMD_DESCRIPTION;

//
// Define the supported flags for metadat entries
//
// MD_DESCRIPTION_FLAG_INDEXED - The property supports Description
// 		index, which means multiple entries of this property can
//		be found in the metadata file, one for each description index
//
// MD_DESCRIPTION_FLAG_REQUIRED - This property is required
//
// MD_DESCRIPTION_FLAG_FORCE_LOAD - The value must be loaded from the
//		metadata file.
//
// MD_DESCRIPTION_FLAG_MULTIPART - The value contains binary informaiton
//		that has to be broken into 8k chunks.
//
// MD_DESCRIPTION_FLAG_VALUE_SET - Run-time flag indicating that this
//		value was provided by the user data
//
// MD_DESCRIPTION_FLAG_COMPUTED - The property is computed and cannot
//		be set by the publisher metadata
//
#define MD_DESCRIPTION_FLAG_INDEXED			((DWORD)0x00000001)
#define MD_DESCRIPTION_FLAG_REQUIRED		((DWORD)0x00000002)
#define MD_DESCRIPTION_FLAG_FORCE_LOAD		((DWORD)0x00000004)
#define MD_DESCRIPTION_FLAG_MULTIPART		((DWORD)0x00000008)
#define MD_DESCRIPTION_FLAG_VALUE_SET		((DWORD)0x40000000)
#define MD_DESCRIPTION_FLAG_COMPUTED		((DWORD)0x80000000)

#define MD_MARK_VALUE_SET(pmd)	((pmd)->dwFlags |= MD_DESCRIPTION_FLAG_VALUE_SET)
#define MD_CLEAR_VALUE_SET(pmd)	((pmd)->dwFlags &= ~MD_DESCRIPTION_FLAG_VALUE_SET)
#define MD_IS_VALUE_SET(pmd)	(((pmd)->dwFlags & MD_DESCRIPTION_FLAG_VALUE_SET) != 0)

//
// Define what a property index looks like. This value indices back into
// the rgmdPropertyDescriptions array above
//
typedef DWORD MD_PROPERTY_DESCRIPTION_INDEX;

//
// Define what a description index is like
//
typedef DWORD MD_DESCRIPTION_INDEX;

//
// Define a structure for a metadata item
//
typedef struct
{
	MD_PROPERTY_DESCRIPTION_INDEX	mpdi;	// Property
	MD_DESCRIPTION_INDEX			mdi;	// Index (language, etc) 
	MD_DATA							mdd;	// Data

} MD_PROPERTY, *PMD_PROPERTY;

// ==================================================
// Query Generator
//

//
// Define the list of SQL queries that need to be generated
//
typedef enum
{
	mqmOffers = 0x1,
	mqmTitleOffers = 0x2,
	mqmOfferDescriptions = 0x4,
	mqmOfferDescriptionDetails = 0x8,
	mqmOfferLocations = 0x10

} MD_QUERY_MASK;

//
// Define a function prototype to build a query
//
typedef HRESULT (*PFN_BUILD_QUERY)(PVOID, DWORD, PSTR, FILE *);

//
// Define the structure representing a SQL Query
//
typedef struct
{
	MD_QUERY_MASK		mqm;
	PSTR				szTableName;
	BOOL				fIndexed;
	PFN_BUILD_QUERY		pfnBuildQuery;

} MD_QUERY, *PMD_QUERY;

// ====================================================================
// Metadata APIs
//

//
// API to retrieve the last error string
//
PSTR MetadataGetErrorString();

//
// API to read and parse a publisher-supplied metadata file, and 
// build a consolidated, packed metadata file for submission. The
// packed metadata file will be written out to the specified file
// name
//
HRESULT BuildPackedFileFromMetadataFile(
			PSTR	szMetadataFileName,
			PSTR	szOutputFileName
			);

//
// API to generate SQL queries from the metadata file
//
HRESULT GenerateQueriesFromMetadataFile(
			DWORD	dwTitleId,
			DWORD	dwOfferId,
			PSTR	szMetadataFileName,
			PSTR	szQueryFileName
			);

#endif

