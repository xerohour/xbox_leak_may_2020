//===================================================================
//
// Copyright Microsoft Corporation. All Rights Reserved.
//
//===================================================================
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "metadata.h"

#define MAX_DATA_SIZE		64000
#define MAX_BINTEXT_SIZE	8000

//
// Define an enumerated type for conputed fields. These must correspond to 
// entries in g_rgmdPropertyDescriptions below
//
typedef enum
{
	mciTitleId = 0,
	mciOfferId,
	mciRating,
	mciCancelable,
	mciLocationRank,
	mciOfferLocations,
	mciMax

} MD_COMPUTED_INDEX;

//
// Define all the supported properties. Please see MD_COMPUTED_INDEX above before 
// changing
//
MD_DESCRIPTION g_rgmdPropertyDescriptions[] =
{
	// Computed fields
	{
		"TitleID", "i_title_id", mdtInt32, 0, MD_DESCRIPTION_FLAG_COMPUTED, mqmTitleOffers,
		"Title ID (computed value)", NULL
	},
	{
		"OfferID", "i_offer_id", mdtInt32, 0, MD_DESCRIPTION_FLAG_COMPUTED, 
		mqmOffers | mqmTitleOffers | mqmOfferDescriptions | mqmOfferDescriptionDetails | mqmOfferLocations,
		"Offer ID (computed value)", NULL
	},
	{
		"Rating", "i_ESRB_id", mdtInt32, 0, MD_DESCRIPTION_FLAG_COMPUTED, mqmOffers,
		"Content Rating (computed value)", NULL
	},
	{
		"Cancelable", "b_cancelable", mdtBool, 0, MD_DESCRIPTION_FLAG_COMPUTED, mqmOffers,
		"Cancelable (computed value)", NULL
	},
	{
		"LocationRank", "i_location_rank", mdtInt32, 0, MD_DESCRIPTION_FLAG_COMPUTED, mqmOfferLocations,
		"Offer location Rank (computed value)", NULL
	},
	{
		"OfferLocations", "vc_XRL", mdtString, 4000, MD_DESCRIPTION_FLAG_COMPUTED, mqmOfferLocations,
		"Offer location XRL (computed value)", NULL
	},
	{
		"PackageSize", "i_package_size", mdtInt32, 0, 
		MD_DESCRIPTION_FLAG_COMPUTED | MD_DESCRIPTION_FLAG_FORCE_LOAD, mqmOffers,
		"Package Size (computed value)", NULL
	},
	{
		"InstallSize", "i_install_size", mdtInt32, 0, 
		MD_DESCRIPTION_FLAG_COMPUTED | MD_DESCRIPTION_FLAG_FORCE_LOAD, mqmOffers,
		"Install Size (computed value)", NULL
	},
	{
		"SymmetricKey", "vb_sym_key", mdtBinary, 16, 
		MD_DESCRIPTION_FLAG_COMPUTED | MD_DESCRIPTION_FLAG_FORCE_LOAD, mqmOffers,
		"Package Key (computed value)", NULL
	},

	// Publisher fields
	{	
		"BitFlags", "i_bitfilter", mdtInt32, 0, MD_DESCRIPTION_FLAG_REQUIRED, mqmOffers,
		"Bit flags (publisher-defined)", NULL
	},
	{	
		"OfferingType", "i_offer_type_id", mdtInt32, 0, 0, mqmOffers,
		"Type of offering (Bitmask: Content=1; subscription=2)", NULL
	},
	{	
		"OfferFrequency", "i_offer_frequency_id", mdtInt32, 0, 0, mqmOffers,
		"Offer frequency (non-terminating=0; monthly=1; yearly=2)", NULL
	},
	{	
		"ActivationDate", "sd_start_date", mdtDate, 0, 0, mqmOffers,
		"Offering Activation Date (mm/dd/yyyy)", NULL
	},
	{	
		"EndDate", "sd_end_date", mdtDate, 0, 0, mqmOffers,
		"Offering End/Expiry Date (mm/dd/yyyy)", NULL
	},
	{	
		"FriendlyName", "vc_friendly_name", mdtString, 150, 0, mqmOffers,
		"Offering End/Expiry Date (mm/dd/yyyy)", NULL
	},
	{	
		"EnumerationData", "vb_enum_blob", mdtBinaryFromFile, MAX_BINTEXT_SIZE, 
		MD_DESCRIPTION_FLAG_INDEXED | MD_DESCRIPTION_FLAG_REQUIRED, mqmOfferDescriptions,
		"Descriptive data returned by enumeration request (up to 8192 bytes)", NULL
	},
	{	
		"DetailsData", "vb_details_blob", mdtBinaryFromFile, MAX_DATA_SIZE, 
		MD_DESCRIPTION_FLAG_INDEXED | MD_DESCRIPTION_FLAG_MULTIPART | MD_DESCRIPTION_FLAG_REQUIRED, 
		mqmOfferDescriptionDetails,
		"Details returned by get details request (up to 65536 bytes)", NULL
	}	
};

const DWORD g_cmdPropertyDescriptions = sizeof(g_rgmdPropertyDescriptions) / sizeof(MD_DESCRIPTION);

//
// Error reporting string
//
static CHAR	g_szError[512];

//
// API to retrieve the last error string
//
PSTR MetadataGetErrorString()
{
	return(g_szError);
}

//
// Helper functions
//

//
// Validation routines. Strings MUST be NULL-terminated
//
BOOL ValidateInt(PSTR szInt, ULONGLONG qwLimit, ULONGLONG *pqwValue)
{
	DWORD		cbInt = strlen(szInt);
	ULONGLONG	qwValue = 0;
	BOOL		fHex = FALSE;
	DWORD		i;
	CHAR		ch;

	// Handle Hex values
	if ((cbInt > 2) && (*szInt == '0') && ((szInt[1] == 'x') || szInt[1] == 'X'))
	{
		fHex =  TRUE;
		cbInt -= 2;
		szInt += 2;
	}

	for (i = 0; i < cbInt; i++)
	{
		qwValue *= (fHex?0x10:10);
		ch = *szInt++;
		if ((ch < '0') || (ch > '9'))
		{
			if (!fHex)
				return(FALSE);

			if ((ch >= 'a') && (ch <= 'f'))
				ch = ch - 'a' + 10 + '0';
			else if ((ch >= 'A') && (ch <= 'F'))
				ch = ch - 'A' + 10 + '0';
			else
				return(FALSE);
		}
		
		qwValue += (ch - '0');

		if (qwValue >= qwLimit)
			return(FALSE);
	}

	if (pqwValue)
		*pqwValue = qwValue;

	return(TRUE);
}

BOOL ValidateBool(PSTR szBool)
{
	// Either True, false, or any 32-bit value (0=FALSE)
	if (!_stricmp(szBool, "true") || !_stricmp(szBool, "false") ||
		ValidateInt(szBool, 100000000, NULL))
		return(TRUE);
	return(FALSE);
}

BOOL ValidateDate(PSTR szDate)
{
	CHAR		szTemp[12];
	CHAR		*pS1, *pS2;
	ULONGLONG	qwMonth, qwDay, qwYear;

	if (strlen(szDate) > 10)
		return(FALSE);
	strcpy(szTemp, szDate);
	pS1 = strchr(szTemp, '/');
	if (!pS1)
		return(FALSE);
	*pS1++ = '\0';
	pS2 = strchr(pS1, '/');
	if (!pS2)
		return(FALSE);
	*pS2++ = '\0';
	if (!ValidateInt(szTemp, 13, &qwMonth) || !qwMonth)
		return(FALSE);
	if (!ValidateInt(pS1, 32, &qwDay) || !qwDay)
		return(FALSE);
	if (!ValidateInt(pS2, 10000, &qwYear) || !qwYear)
		return(FALSE);
	if ((qwMonth == 2) && (qwDay > 28))
	{
		if (((qwYear % 4) != 0) || (qwDay > 29))
			return(FALSE);
	}
	if (qwDay == 31)
	{
		switch (qwMonth)
		{
		case 4: case 6: case 9: case 11:
			return(FALSE);
		}
	}
	return(TRUE);
}

BOOL ValidateBinaryText(PSTR szBlob, DWORD dwMaxSize)
{
	CHAR ch;
	
	if ((dwMaxSize != 0) && ((strlen(szBlob)>>1) > dwMaxSize))
		return(FALSE);

	while (*szBlob)
	{
		ch = *szBlob++;
		if (((ch < '0') || (ch > '9')) &&
			((ch < 'a') || (ch > 'f')) &&
			((ch < 'A') || (ch > 'F')))
			return(FALSE);
	}

	return(TRUE);
}

//
// Some conversion routines
//
#define TO_HEX_DIGIT(x) (((x) >= 10)?(x)-10+'A':(x)+'0')

HRESULT ConvertToBinaryText(PBYTE pbBlob, DWORD cbBlob, PSTR szBinText, DWORD *pchBinText)
{
	DWORD	cbLen = *pchBinText;

	*pchBinText = (cbBlob << 1) + 3;
	if (cbLen < *pchBinText)
		return(HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER));

	*szBinText++ = '0';
	*szBinText++ = 'x';
	while (cbBlob--)
	{
		*szBinText++ = TO_HEX_DIGIT((*pbBlob >> 4) & 0xf);
		*szBinText++ = TO_HEX_DIGIT(*pbBlob & 0xf);
		pbBlob++;
	}
	*szBinText = '\0';
	return(S_OK);
}

// ================================================================
// Part 1: Read publisher metadata file and convert to a buffer
//

//
// Publisher metadata file format:
// 
// The publisher metadata file format is a standard .ini file. 
// The file will contain two main sections: [All] and [Indexed]
// The [All] section contains properties that are common for all
// versions (i.e. all languages) of the offered content. Properties
// are expressed in a property=value pair under the section.
//

//
// API to read and parse a publisher-supplied metadata file, and 
// build a consolidated, packed metadata file for submission. The
// packed metadata file will be written out to the specified file
// name
//
HRESULT BuildPackedFileFromMetadataFile(
			PSTR	szMetadataFileName,
			PSTR	szOutputFileName
			)
{
	HRESULT	hr = S_OK;
	HANDLE	hFrom = INVALID_HANDLE_VALUE;
	BYTE	rgbData[MAX_DATA_SIZE];
	BYTE	rgbProps[MAX_DATA_SIZE];
	BYTE	rgbFile[MAX_DATA_SIZE];
	CHAR	szKey[MAX_PATH];
	DWORD	cbData;
	DWORD	cbProps;
	DWORD	cbFile;
	DWORD	cbText;
	PBYTE	pbData = rgbData;
	PBYTE	pbProps;
	PBYTE	pbEnd;
	PBYTE	pbPropEnd;
	PBYTE	pbValueEnd;
	BOOL	fIsDecInt;
	BOOL	fError;

	DWORD	dwCurSection = 0;
	DWORD	dwOrder;
	DWORD	i, j;

	PMD_DESCRIPTION	pmd;
	LARGE_INTEGER	liSize;

	// Load all sections of the metadata file into memory
	cbData = GetPrivateProfileSectionNamesA(
				rgbData, MAX_DATA_SIZE, szMetadataFileName);
	if (cbData <= 2)
	{
		// Well, we read nothing, return error
		sprintf(g_szError, "Unable to read metadata file %s (or file is empty)\n", 
					szMetadataFileName);
		goto InvalidData;
	}

	// Delete the target file, it it exists
	DeleteFile(szOutputFileName);

	// Now parse each section and process the information
	while (cbData)
	{
		// leading NULL means end of names
		if (*pbData == '\0')
			break;

		// Parse the next section name
		fIsDecInt = TRUE;
		for (pbEnd = pbData;
				(cbData > 0) && (*pbEnd != '\0');
				pbEnd++, cbData--)
		{				
			if ((*pbEnd < '0') || (*pbEnd > '9'))
				fIsDecInt = FALSE;
		}
		if (!cbData)
		{
			// String is not terminated!
			sprintf(g_szError, "Invalid data\n");
			goto InvalidData;
		}

		// Process this section
		if (dwCurSection == 0)
		{
			// The first section must be [All]
			if (_stricmp(pbData, MD_COMMON_SECTION_NAME) != 0)
			{
				sprintf(g_szError, "The first section of the metadata file must be [%s]\n",
							MD_COMMON_SECTION_NAME);
				goto InvalidData;
			}
		}
		else
		{
			// Subsequent section names must be a valid decimal integer
			// (maps to description index)
			if (!fIsDecInt)
			{
				sprintf(g_szError, "Section names must be decimal integers (e.g. 12)\n");
				goto InvalidData;
			}

			// Clear the required fields
			for (i = 0; i < g_cmdPropertyDescriptions; i++)
				if (g_rgmdPropertyDescriptions[i].dwFlags & MD_DESCRIPTION_FLAG_INDEXED)
				{
					MD_CLEAR_VALUE_SET(g_rgmdPropertyDescriptions + i);
				}
		}

		printf("Processing section %s ...\n", pbData);

		// Now process the property-value pairs in this section
		pbProps = rgbProps;
		cbProps = GetPrivateProfileSectionA(
					pbData, pbProps, MAX_DATA_SIZE, szMetadataFileName);
		if (cbProps <= 2)
		{
			// Well, we read nothing, return error
			sprintf(g_szError, "Unable to read metadata in section %s\n", pbData);
			goto InvalidData;
		}

		// Parse out the property name and value
		while (cbProps)
		{
			// leading NULL means end of names
			if (*pbProps == '\0')
				break;

			// Parse the next section name
			for (pbPropEnd = pbProps;
					(cbProps > 0) && (*pbPropEnd != '=');
					pbPropEnd++, cbProps--)
				;
			if (cbProps < 3)
			{
				// String is not terminated!
				sprintf(g_szError, "Invalid data\n");
				goto InvalidData;
			}
			*pbPropEnd = '\0';

			// Parse the value to make sure it terminates
			for (cbProps--, pbValueEnd = pbPropEnd + 1;
					(cbProps > 0) && (*pbValueEnd != '\0');
					pbValueEnd++, cbProps--)
				;
			if (!cbProps)
			{
				// String is not terminated!
				sprintf(g_szError, "Invalid data\n");
				goto InvalidData;
			}

			// Process this property
			for (i = 0, pmd = g_rgmdPropertyDescriptions; 
					i < g_cmdPropertyDescriptions; i++, pmd++)
			{
				if (!_stricmp(pbProps, pmd->szPropertyName))
				{
					// Match! Make sure the property is in the right section
					if (dwCurSection == 0)
					{
						// Make sure the property is not indexed
						if (pmd->dwFlags & MD_DESCRIPTION_FLAG_INDEXED)
						{
							sprintf(g_szError, "Indexed property %s cannot be specified in [All] section.\n",
									pmd->szPropertyName);
							goto InvalidData;
						}
					}
					else
					{
						// Make sure the property is indexed
						if ((pmd->dwFlags & MD_DESCRIPTION_FLAG_INDEXED) == 0)
						{
							sprintf(g_szError, "Non-indexed property %s cannot be specified in Indexed section.\n",
									pmd->szPropertyName);
							goto InvalidData;
						}
					}

					// Make sure the property is not computed
					if (pmd->dwFlags & MD_DESCRIPTION_FLAG_COMPUTED)
					{
						sprintf(g_szError, "Unsupported property %s\n", pmd->szPropertyName);
						goto InvalidData;
					}

					printf("    %s = %s\n", pbProps, pbPropEnd + 1);

					// Validate the data
					switch (pmd->mdt)
					{
					case mdtBool:
						if (!ValidateBool(pbPropEnd + 1))
						{
							sprintf(g_szError, "Invalid boolean value for property %s (%s)\n",
									pbProps, pbPropEnd + 1);
							goto InvalidData;
						}
						break;

					case mdtInt8:
						if (!ValidateInt(pbPropEnd + 1, 0x100, NULL))
						{
							sprintf(g_szError, "Invalid 8-bit value for property %s (%s)\n",
									pbProps, pbPropEnd + 1);
							goto InvalidData;
						}
						break;
						
					case mdtInt16:
						if (!ValidateInt(pbPropEnd + 1, 0x10000, NULL))
						{
							sprintf(g_szError, "Invalid 16-bit value for property %s (%s)\n",
									pbProps, pbPropEnd + 1);
							goto InvalidData;
						}
						break;

					case mdtInt32:
						if (!ValidateInt(pbPropEnd + 1, 0x100000000, NULL))
						{
							sprintf(g_szError, "Invalid 32-bit value for property %s (%s)\n",
									pbProps, pbPropEnd + 1);
							goto InvalidData;
						}
						break;

					case mdtDate:
						if (!ValidateDate(pbPropEnd + 1))
						{
							sprintf(g_szError, "Invalid Date value for property %s (%s)\n",
									pbProps, pbPropEnd + 1);
							goto InvalidData;
						}
						break;

					case mdtString:
						if ((pmd->cbMaxSize > 0) && 
							(strlen(pbPropEnd + 1) > pmd->cbMaxSize))
						{
							sprintf(g_szError, "String value too long for property %s (Max is %u characters)\n",
									pbProps, pmd->cbMaxSize);
							goto InvalidData;
						}
						break;

					case mdtBinary:
						if (!ValidateBinaryText(pbPropEnd + 1, pmd->cbMaxSize))
						{
							sprintf(g_szError, "Invalid Hex binary value for property %s\n", pbProps);
							goto InvalidData;
						}
						break;

					case mdtBinaryFromFile:
						// The value is actually the file name
						hFrom = CreateFile(
									pbPropEnd + 1, 
									GENERIC_READ, 
									FILE_SHARE_READ, 
									NULL, 
									OPEN_EXISTING, 
									FILE_ATTRIBUTE_NORMAL, 
									NULL);
						if (hFrom == INVALID_HANDLE_VALUE)
						{
							sprintf(g_szError, "Unable to open binary data file %s for property %s\n",
										pbPropEnd + 1, pbProps);
							hr = HRESULT_FROM_WIN32(GetLastError());
							goto Error;
						}

						if (!GetFileSizeEx(hFrom, &liSize))
						{
							sprintf(g_szError, "Unable to get size of binary data file %s for property %s\n",
										pbPropEnd + 1, pbProps);
							hr = HRESULT_FROM_WIN32(GetLastError());
							goto Error;
						}

						if (liSize.QuadPart > pmd->cbMaxSize)
						{
							sprintf(g_szError, "Binary value too long for property %s (Max is %u characters)\n",
									pbProps, pmd->cbMaxSize);
							goto InvalidData;
						}
						dwOrder = 0;

						do
						{
							// Read in 8k chunks
							if (!ReadFile(hFrom, rgbFile, MAX_BINTEXT_SIZE, &cbFile, NULL))
							{
								sprintf(g_szError, "Unable to load binary data from file %s for property %s\n",
											pbPropEnd + 1, pbProps);
								hr = HRESULT_FROM_WIN32(GetLastError());
								goto Error;
							}

							if (cbFile)
							{
								// Convert to binary text
								cbText = (MAX_BINTEXT_SIZE << 1) + 3;
								ConvertToBinaryText(rgbFile, cbFile, rgbFile + MAX_BINTEXT_SIZE, &cbText);
								
								// Write it out
								if (pmd->dwFlags & MD_DESCRIPTION_FLAG_MULTIPART)
									sprintf(szKey, "%s.%u", pbProps, dwOrder);
								else
								{
									if (dwOrder > 0)
									{
										sprintf(g_szError, "Internal error: multipart required on simple property %s\n", pbProps);
										goto InvalidData;
									}
									sprintf(szKey, "%s", pbProps);
								}
								if (!WritePrivateProfileStringA(
											pbData, szKey, rgbFile + MAX_BINTEXT_SIZE, szOutputFileName))
								{
									sprintf(g_szError, "Unable to write binary data for property %s\n", pbProps);
									hr = HRESULT_FROM_WIN32(GetLastError());
									goto Error;
								}

								dwOrder++;
							}
							
						} while (cbFile);

						CloseHandle(hFrom);
						hFrom = INVALID_HANDLE_VALUE;
						
						break;
					}

					// Write out the data (except for mdtBinaryFromFile)
					if (pmd->mdt != mdtBinaryFromFile)
					{
						if (!WritePrivateProfileStringA(
									pbData, pbProps, pbPropEnd + 1, szOutputFileName))
						{
							sprintf(g_szError, "Unable to write data for property %s\n", pbProps);
							hr = HRESULT_FROM_WIN32(GetLastError());
							goto Error;
						}
					}

					// Mark the property as "set"
					if (MD_IS_VALUE_SET(pmd))
					{
						printf("Warning: property %s already set, overwriting with newer value\n", pbProps);
					}
					MD_MARK_VALUE_SET(pmd);

					break;
				}
			}

			if (i == g_cmdPropertyDescriptions)
			{
				sprintf(g_szError, "Unsupported property %s\n", pbProps);
				goto InvalidData;
			}

			// Skip to next property
			pbProps = pbValueEnd + 1;
		}

		// Validation: make sure all required fields are provided
		fError = FALSE;
		for (i = 0; i < g_cmdPropertyDescriptions; i++)
			if ((g_rgmdPropertyDescriptions[i].dwFlags & MD_DESCRIPTION_FLAG_REQUIRED) &&
				!(g_rgmdPropertyDescriptions[i].dwFlags & MD_DESCRIPTION_FLAG_VALUE_SET))
			{
				if ((!dwCurSection && !(g_rgmdPropertyDescriptions[i].dwFlags & MD_DESCRIPTION_FLAG_INDEXED)) ||
					(dwCurSection && (g_rgmdPropertyDescriptions[i].dwFlags & MD_DESCRIPTION_FLAG_INDEXED)))
				{
					if (!fError)
						printf("ERROR: The following required properties were not specified for [%s]:\n", pbData);
					fError = TRUE;
					printf("    %s\n", g_rgmdPropertyDescriptions[i].szPropertyName);
				}
			}
		if (fError)
			goto InvalidData;

		// Process the next section
		dwCurSection++;
		pbData = pbEnd + 1;		
	}

	// There needs to be at least 2 sections
	if (dwCurSection < 2)
	{
		sprintf(g_szError, "ERROR: There needs to be at least two sections\n");
		goto InvalidData;
	}

Exit:

	if (hFrom != INVALID_HANDLE_VALUE)
		CloseHandle(hFrom);

	// Delete the intermediate file if failed
	if (FAILED(hr))
		DeleteFile(szOutputFileName);

	return(hr);

Error:
	goto Exit;

InvalidData:
	hr = HRESULT_FROM_WIN32(ERROR_INVALID_DATA);
	goto Error;
}

// ==========================================================
// Query builder functions
//
CHAR	g_szData[MAX_DATA_SIZE << 1];
CHAR	g_szValues[MAX_DATA_SIZE << 1];
CHAR	g_szQuery[MAX_DATA_SIZE << 1];

HRESULT BuildQueryHandler(
			PVOID	pvmq,
			DWORD	dwDescriptionIndex,
			PSTR	szMetadataFile,
			FILE	*fOut
			)
{
	HRESULT	hr = S_OK;
	CHAR	szColumns[MAX_BINTEXT_SIZE];
	CHAR	szPropertyName[MAX_PATH];
	DWORD	cbData = 0;
	DWORD	cbColumns = 0;
	DWORD	cbValues = 0;
	DWORD	cbQuery = 0;
	DWORD	cColumns = 0;
	DWORD	cOrder = 0;
	DWORD	i;
	CHAR	chDefault = '\0';
	CHAR	szQuote[2] = { '\0', '\0' };
	BOOL	fDone = TRUE;
	BOOL	fSet;
	BOOL	fMultipart;
	BOOL	fContainsMultipart = FALSE;

	PMD_DESCRIPTION	pmd;
	PMD_QUERY		pmq;

	pmq = (PMD_QUERY)pvmq;

	do
	{
		// Initialize
		cColumns = 0;
		cbColumns = 0;
		cbValues = 0;
		fContainsMultipart = FALSE;

		for (i = 0, pmd = g_rgmdPropertyDescriptions; 
				i < g_cmdPropertyDescriptions; 
				i++, pmd++)
		{
			// Determine if it is a multipart property
			fMultipart = ((pmd->dwFlags & MD_DESCRIPTION_FLAG_MULTIPART) != 0);
		
			// Only select properties that are marked for this query
			if (pmd->dwQuerySet & pmq->mqm)
			{
				// If the field is computed, then look up the default value
				fSet = FALSE;
				if ((pmd->dwFlags & MD_DESCRIPTION_FLAG_COMPUTED) &&
					!(pmd->dwFlags & MD_DESCRIPTION_FLAG_FORCE_LOAD))
				{
					if (!pmd->pmddDefault)
					{
						sprintf(g_szError, "Internal error: Computed value %s without default\n", 
									pmd->szPropertyName);
						goto InvalidData;
					}

					// Use the default value (must be a string)
					strcpy(g_szData, pmd->pmddDefault->sz);
					cbData = pmd->pmddDefault->cch;
				}
				else
				{
					// Figure out the property name
					if (pmd->dwFlags & MD_DESCRIPTION_FLAG_INDEXED)
					{
						sprintf(g_szQuery, "%u", dwDescriptionIndex);

						if (fMultipart)
							sprintf(szPropertyName, "%s.%u", pmd->szPropertyName, cOrder);
						else
							strcpy(szPropertyName, pmd->szPropertyName);
					}
					else
					{
						strcpy(g_szQuery, MD_COMMON_SECTION_NAME);
						strcpy(szPropertyName, pmd->szPropertyName);
					}

					// Read the property from the metadata file
					*g_szData = '\0';
					cbData = GetPrivateProfileStringA(
								g_szQuery,
								szPropertyName,
								&chDefault,
								g_szData,
								MAX_DATA_SIZE,
								szMetadataFile);
					if (!cbData || (*g_szData == '\0'))
					{
						// Field is not present, we will ignore this if the
						// field is not required
						if (pmd->dwFlags & MD_DESCRIPTION_FLAG_REQUIRED)
						{
							// If the query is multipart and if the order is greater than 0,
							// then we got all the data
							if (fMultipart && cOrder)
							{
								// We are done!
								fDone = TRUE;
								cColumns = 0;
								break;
							}
							
							sprintf(g_szError, "Required field %s invalid or not found\n", 
										pmd->szPropertyName);
							goto InvalidData;
						}

						continue;
					}
				}

				// Insert the column-value pair in the buffer
				if (fMultipart)
					fContainsMultipart = TRUE;
				if (((cbColumns + strlen(pmd->szColumnName) + 6) > MAX_DATA_SIZE) ||
					((cbValues + cbData + 6) > MAX_DATA_SIZE))
				{
					sprintf(g_szError, "Internal error: Column/value data too large\n");
					goto InvalidData;
				}

				// Certain data types require single quotes, add if appropriate
				if ((pmd->mdt == mdtDate) || (pmd->mdt == mdtString))
					szQuote[0] = '\'';
				else
					szQuote[0] = '\0';

				cbColumns += sprintf(szColumns + cbColumns, "\n    %s,", pmd->szColumnName);
				cbValues += sprintf(g_szValues + cbValues, "\n    %s%s%s,", 
							szQuote, g_szData, szQuote);
				cColumns++;
			}
		} // for

		// Generate the query
		if (cColumns)
		{
			if (pmq->fIndexed)
			{
				// Need to indicate the description index
				cbColumns += sprintf(szColumns + cbColumns, "\n    i_description_index,");
				cbValues += sprintf(g_szValues + cbValues, "\n    %u,", dwDescriptionIndex);
				cColumns++;
			}
			
			if (fContainsMultipart)
			{
				// Tack on the order field
				cbColumns += sprintf(szColumns + cbColumns, "\n    i_order");
				cbValues += sprintf(g_szValues + cbValues, "\n    %u", cOrder++);
				cColumns++;
				fDone = FALSE;
			}
			else
			{
				// Remove annoying trailing comma
				szColumns[--cbColumns] = '\0';
				g_szValues[--cbValues] = '\0';
			}

			// Format and write out the query
			sprintf(g_szQuery, "insert into %s (%s)\nvalues (%s)\n\n", 
						pmq->szTableName, szColumns, g_szValues);
			if (fprintf(fOut, g_szQuery) <= 0)
			{
				sprintf(g_szError, "Error writing query for %s to file\n", pmq->szTableName);
				hr = HRESULT_FROM_WIN32(GetLastError());
				goto Error;
			}
		}
		else if (!fDone)
		{
			printf("No columns found, skipping query for %s\n", pmq->szTableName);
		}

	} while (!fDone);

Exit:
	return(hr);

Error:
	goto Exit;

InvalidData:
	hr = HRESULT_FROM_WIN32(ERROR_INVALID_DATA);
	goto Error;
}

//
// Define the list of queries
//
MD_QUERY g_rgmqQueries[] =
{
	{ 
		mqmOffers, 
		"t_offers", 
		FALSE, 
		BuildQueryHandler 
	},
	{ 
		mqmTitleOffers, 
		"t_title_offers", 
		FALSE, 
		BuildQueryHandler 
	},
	{
		mqmOfferDescriptions, 
		"t_offer_descriptions", 
		TRUE, 
		BuildQueryHandler 
	},
	{
		mqmOfferDescriptionDetails, 
		"t_offer_description_details", 
		TRUE, 
		BuildQueryHandler 
	},
	{
		mqmOfferLocations, 
		"t_offer_locations", 
		FALSE, 
		BuildQueryHandler 
	}
};

const DWORD g_cmqQueries = sizeof(g_rgmqQueries) / sizeof(MD_QUERY);

//
// API to generate SQL queries from the metadata file
//
#define MAX_INDICES		256

HRESULT GenerateQueriesFromMetadataFile(
			DWORD	dwTitleId,
			DWORD	dwOfferId,
			PSTR	szMetadataFileName,
			PSTR	szQueryFileName
			)
{
	HRESULT	hr = S_OK;
	MD_DATA	rgData[mciMax];
	CHAR	szData[MAX_DATA_SIZE];
	CHAR	szTitleId[10];
	CHAR	szOfferId[10];
	CHAR	szOfferLocations[MAX_PATH];
	DWORD	rgdwDescriptionIndices[MAX_INDICES];
	DWORD	cbData;
	DWORD	dwCurSection = 0;
	DWORD	dwIndex;
	CHAR	*pbData;
	CHAR	*pbEnd;
	DWORD	i, j;
	BOOL	fDupe;
	FILE	*fOut = NULL;

	ULONGLONG	qwValue;
	PMD_QUERY	pmq;

	const PSTR	szRating = "0";
	const PSTR	szCancellable = "0";
	const PSTR	szRank = "0";

	const PSTR	szDownloadIP = "131.107.228.145:80";

	// Set up the default values
	rgData[mciTitleId].sz = szTitleId;
	rgData[mciTitleId].cch = sprintf(szTitleId, "0x%08x", dwTitleId);
	rgData[mciOfferId].sz = szOfferId;
	rgData[mciOfferId].cch = sprintf(szOfferId, "0x%08x", dwOfferId);
	rgData[mciRating].sz = szRating;
	rgData[mciRating].cch = strlen(szRating);
	rgData[mciCancelable].sz = szCancellable;
	rgData[mciCancelable].cch = strlen(szCancellable);
	rgData[mciLocationRank].sz = szRank;
	rgData[mciLocationRank].cch = strlen(szRank);
	rgData[mciOfferLocations].sz = szOfferLocations;
	rgData[mciOfferLocations].cch = 
			sprintf(szOfferLocations, "%s/content/%08x/%08x.xcp", 
						szDownloadIP, dwTitleId, dwOfferId);

	// set the default values
	for (i = 0 ; i < mciMax; i++)
	{
		assert(g_rgmdPropertyDescriptions[i].dwFlags & MD_DESCRIPTION_FLAG_COMPUTED);
		g_rgmdPropertyDescriptions[i].pmddDefault = rgData + i;
	}

	// Load all the section names form the metadata file
	cbData = GetPrivateProfileSectionNamesA(
				szData, MAX_DATA_SIZE, szMetadataFileName);
	if (cbData <= 2)
	{
		// Well, we read nothing, return error
		sprintf(g_szError, "Unable to read metadata file %s (or file is empty)\n", 
					szMetadataFileName);
		goto InvalidData;
	}

	// Delete the target file, it it exists
	DeleteFile(szQueryFileName);

	// Create the query file
	fOut = fopen(szQueryFileName, "w");
	if (!fOut)
	{
		hr = HRESULT_FROM_WIN32(GetLastError());
		sprintf(g_szError, "Error creating query file %s (%08x)\n",
					szQueryFileName, hr);
		goto Error;
	}

	// Now parse each section and process the information
	pbData = szData;
	while (cbData)
	{
		// leading NULL means end of names
		if (*pbData == '\0')
			break;

		// Parse the next section name
		for (pbEnd = pbData;
				(cbData > 0) && (*pbEnd != '\0');
				pbEnd++, cbData--)
			;
		if (!cbData)
		{
			// String is not terminated!
			sprintf(g_szError, "Invalid data\n");
			goto InvalidData;
		}

		// Process this section
		if (dwCurSection == 0)
		{
			// The first section must be [All]
			if (_stricmp(pbData, MD_COMMON_SECTION_NAME) != 0)
			{
				sprintf(g_szError, "The first section of the metadata file must be [%s]\n", 
							MD_COMMON_SECTION_NAME);
				goto InvalidData;
			}
			dwCurSection++;
		}
		else
		{
			// Subsequent section names must be a valid decimal integer
			// (maps to description index)
			if (!ValidateInt(pbData, 0x100000000, &qwValue))
			{
				sprintf(g_szError, "Section names must be decimal integers (e.g. 12)\n");
				goto InvalidData;
			}

			dwIndex = (DWORD)qwValue;
			fDupe = FALSE;
			if (dwCurSection > 1)
			{
				for (i = 0; i < dwCurSection - 1; i++)
					if (rgdwDescriptionIndices[i] == dwIndex)
					{
						printf("Warning: duplicate description index %u, ignoring second definition\n", dwIndex);
						fDupe = TRUE;
						break;
					}
			}
			if (!fDupe)
			{
				rgdwDescriptionIndices[dwCurSection - 1] = dwIndex;
				dwCurSection++;
			}
		}

		// Next section
		pbData = pbEnd + 1;
	}

	// There needs to be at least 2 sections
	if (dwCurSection < 2)
	{
		sprintf(g_szError, "ERROR: There needs to be at least two sections\n");
		goto InvalidData;
	}
	
	// Loop through our list of queries and generate them
	for (i = 0, pmq = g_rgmqQueries; i < g_cmqQueries; i++, pmq++)
	{
		// Handle it differently depending on whether the query
		// is indexed
		if (pmq->fIndexed)
		{
			// Loop through the description indices
			for (j = 1; j < dwCurSection; j++)
			{
				printf("  * Building indexed Query for %s, description index %u\n",
							pmq->szTableName, rgdwDescriptionIndices[j-1]);
				hr = pmq->pfnBuildQuery((PVOID)pmq, 
						rgdwDescriptionIndices[j-1], szMetadataFileName, fOut);
				if (FAILED(hr))
					break;
			}
		}
		else
		{
			printf("Building Query for %s\n", pmq->szTableName);
			hr = pmq->pfnBuildQuery((PVOID)pmq, 0, szMetadataFileName, fOut);
		}

		if (FAILED(hr))
			goto Error;
	}

Exit:
	if (fOut != NULL);
		fclose(fOut);

	if (FAILED(hr))
		DeleteFile(szQueryFileName);

	return(hr);

Error:
	goto Exit;

InvalidData:
	hr = HRESULT_FROM_WIN32(ERROR_INVALID_DATA);
	goto Error;
}


