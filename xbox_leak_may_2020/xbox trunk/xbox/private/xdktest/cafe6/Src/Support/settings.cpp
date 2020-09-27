/////////////////////////////////////////////////////////////////////////////
// settings.cpp
//
// email	date		change
// briancr	11/02/94	created
//
// copyright 1994 Microsoft

// Implementation of the CSettings class

#include "stdafx.h"
#include "settings.h"

#define new DEBUG_NEW

CString CSettings::strKeyClass = "CAFE";

///////////////////////////////////////////////////////////////////////////////
// CSettings

///////////////////////////////////////////////////////////////////////////////
// ctor/dtor

CSettings::~CSettings()
{
	CString strSetting;
	CString strValue;
	POSITION pos;

	// interate through the map and delete all the settings
	for (pos = m_mapSettings.GetStartPosition(); pos != NULL; ) {
		m_mapSettings.GetNextAssoc(pos, strSetting, strValue);
		Delete(strSetting);
	}
}

///////////////////////////////////////////////////////////////////////////////
// operations

BOOL CSettings::IsDefined(CString strSetting)
{
	CString strValue;

	// the setting must not be empty
	ASSERT(!strSetting.IsEmpty());

	// no case sensitivity
	strSetting.MakeUpper();

	// is the setting defined?
	return m_mapSettings.Lookup(strSetting, strValue);
}

BOOL CSettings::SetKey(CString strKey)
{
	// the read only flag must not be set
	ASSERT(!m_bReadOnly);

	if (m_bReadOnly) {
		TRACE("CSettings::SetKey: attempt to write to read-only CSettings object denied");
		return FALSE;
	}

	m_strKey = strKey;

	return TRUE;
}

BOOL CSettings::SetTextValue(CString strSetting, CString strValue, BOOL bOverride /*= TRUE*/)
{
	CString strOldValue;

	// the read only flag must not be set
	ASSERT(!m_bReadOnly);

	if (m_bReadOnly) {
		TRACE("CSettings::Set: attempt to write to read-only CSettings object denied");
		return FALSE;
	}

	// the setting string must not be empty
	ASSERT(!strSetting.IsEmpty());

	// no case sensitivity for switches
	strSetting.MakeUpper();

	// if the setting is already set and we're not to override, just return
	if (m_mapSettings.Lookup(strSetting, strOldValue) && !bOverride) {
		return FALSE;
	}

	// add switch/value pair to the map
	m_mapSettings.SetAt(strSetting, strValue);

	return TRUE;
}

CString CSettings::GetTextValue(CString strSetting, CString strDefault /*= ""*/)
{
	CString strValue;

	// the setting string must not be empty
	ASSERT(!strSetting.IsEmpty());

	// no case sensitivity for switches
	strSetting.MakeUpper();

	// look up the setting's value
	if (m_mapSettings.Lookup(strSetting, strValue)) {
		return strValue;
	}

	// the setting does not exist
	return strDefault;
}

BOOL CSettings::SetBooleanValue(CString strSetting, BOOL bValue, BOOL bOverride /*= TRUE*/)
{
	CString strValue;

	// convert the boolean to a string
	if (bValue) {
		strValue = "true";
	}
	else {
		strValue = "false";
	}
	return SetTextValue(strSetting, strValue, bOverride);
}

BOOL CSettings::GetBooleanValue(CString strSetting, BOOL bDefault /*= FALSE*/)
{
	CString strValue;

	// the setting string must not be empty
	ASSERT(!strSetting.IsEmpty());

	// no case sensitivity for switches
	strSetting.MakeUpper();

	// look up the setting's value
	if (m_mapSettings.Lookup(strSetting, strValue)) {
		return InterpretBooleanValue(strValue, bDefault);
	}

	// the setting does not exist
	return bDefault;
}

BOOL CSettings::SetIntValue(CString strSetting, int nValue, BOOL bOverride /*= TRUE*/)
{
	CString strValue;

	// convert the int to a string
	itoa(nValue, strValue.GetBuffer(255), 10);
	strValue.ReleaseBuffer();

	return SetTextValue(strSetting, strValue, bOverride);
}

int CSettings::GetIntValue(CString strSetting, int nDefault /*= 0*/)
{
	CString strValue;

	// the setting string must not be empty
	ASSERT(!strSetting.IsEmpty());

	// no case sensitivity for switches
	strSetting.MakeUpper();

	// look up the setting's value
	if (m_mapSettings.Lookup(strSetting, strValue)) {
		return atoi(strValue);
	}

	// the setting does not exist
	return nDefault;
}

BOOL CSettings::Delete(CString strSetting)
{
	CString strValue;

	// the read only flag must not be set
	ASSERT(!m_bReadOnly);

	if (m_bReadOnly) {
		TRACE("CSettings::Delete: attempt to write to read-only CSettings object denied");
		return FALSE;
	}

	// the setting string must not be empty
	ASSERT(!strSetting.IsEmpty());

	// no case sensitivity for switches
	strSetting.MakeUpper();

	// is this setting in the map?
	if (m_mapSettings.Lookup(strSetting, strValue)) {
		// remove the entry from the map
		m_mapSettings.RemoveKey(strSetting);

		return TRUE;
	}
	return FALSE;
}

BOOL CSettings::ReadRegistry(void)
{
	DWORD dwDisposition;
	HKEY hKey;
	DWORD dwIndex;
	CString strSetting;
	DWORD dwSettingSize;
	CString strValue;
	DWORD dwValueSize;

	// the key name must not be empty
	ASSERT(!m_strKey.IsEmpty());

	if (m_strKey.IsEmpty()) {
		return FALSE;
	}

	// open the registry subkey
	if (RegCreateKeyEx(HKEY_CURRENT_USER, cstrKeyRoot + m_strKey, 0, strKeyClass.GetBuffer(0), REG_OPTION_NON_VOLATILE, KEY_READ, NULL, &hKey, &dwDisposition) == ERROR_SUCCESS) {

		// iterate through all values in the current registry subkey
		for (dwIndex = 0, dwSettingSize = 1023, dwValueSize = 1023; RegEnumValue(hKey, dwIndex, strSetting.GetBuffer(dwSettingSize), &dwSettingSize, 0, NULL, (unsigned char *)strValue.GetBuffer(dwValueSize), &dwValueSize) != ERROR_NO_MORE_ITEMS; dwIndex++) {
			strSetting.ReleaseBuffer(dwSettingSize);
			strValue.ReleaseBuffer(dwValueSize);

			// add these entries to the map
			SetTextValue(strSetting, strValue);

			dwSettingSize = 1023;
			dwValueSize = 1023;
		}

		RegCloseKey(hKey);
		return TRUE;
	}

	return FALSE;
}

BOOL CSettings::WriteRegistry(void)
{
	CString strSetting;
	CString strValue;
	POSITION pos;
	BOOL bResult = TRUE;
	
	// the key name must not be empty
	ASSERT(!m_strKey.IsEmpty());

	if (m_strKey.IsEmpty()) {
		return FALSE;
	}

	// iterate through all entries in the map and write them to the registry
	for (pos = m_mapSettings.GetStartPosition(); pos != NULL; ) {
		m_mapSettings.GetNextAssoc(pos, strSetting, strValue);

		bResult &= WriteRegistryValue(strSetting, strValue);
	}

	return bResult;
}



///////////////////////////////////////////////////////////////////////////////
// operations (internal)

// InterpretBooleanValue
//
// Description: This function interprets the given value into a boolean value.
//   No value is interpreted as true
//   FALSE, NO, OFF, and 0 are interpreted as FALSE.
//   TRUE, YES, ON, 1, and no value are interpreted as TRUE.
//
// Return value: boolean value of the switch (or nDefault if switch not defined)
BOOL CSettings::InterpretBooleanValue(CString &strValue, BOOL bDefault /*=FALSE*/)
{
	CString strFalse[] = { "0", "NO", "FALSE", "OFF", "" };
	CString strTrue[] = { "1", "YES", "TRUE", "ON", "" };

	// if there is no value, the value is TRUE
	if (strValue == "") {
		return TRUE;
	}

	// remove case sensitivity for boolean checks
	strValue.MakeUpper();

	int i;
	// check for FALSE interpretation
	for (i = 0; strFalse[i] != ""; i++) {
		if (strValue == strFalse[i]) {
			return FALSE;
		}
	}

	// check for TRUE interpretation
	for (i = 0; strTrue[i] != ""; i++) {
		if (strValue == strTrue[i]) {
			return TRUE;
		}
	}

	return bDefault;
}

BOOL CSettings::WriteRegistryValue(CString strSetting, CString strValue)
{
	DWORD dwDisposition;
	HKEY hKey;
	BOOL bResult = FALSE;

	// the key name must not be empty
	ASSERT(!m_strKey.IsEmpty());

	if (m_strKey.IsEmpty()) {
		return FALSE;
	}

	// the setting string must not be empty
	ASSERT(!strSetting.IsEmpty());

	// open the registry subkey
	if (RegCreateKeyEx(HKEY_CURRENT_USER, cstrKeyRoot + m_strKey, 0, strKeyClass.GetBuffer(0), REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, &dwDisposition) == ERROR_SUCCESS) {

		// write the value to the registry
		bResult = (RegSetValueEx(hKey, strSetting, 0, REG_SZ, (const unsigned char*)(LPCTSTR)strValue, strValue.GetLength()) == ERROR_SUCCESS);

		// close the registry
		RegCloseKey(hKey);
	}

	if (!bResult) {
		TRACE("CSettings::WriteRegistryValue: unable to write setting, value (%s, %s) to registry\n", strSetting, strValue);
	}
	return bResult;
}

CString CSettings::ReadRegistryValue(CString strSetting)
{
	DWORD dwDisposition;
	HKEY hKey;
	BOOL bResult = FALSE;
	CString strValue;
	DWORD dwValueSize = 1023;

	// the key name must not be empty
	ASSERT(!m_strKey.IsEmpty());

	if (m_strKey.IsEmpty()) {
		strValue.Empty();
		return strValue;
	}

	// the setting string must not be empty
	ASSERT(!strSetting.IsEmpty());

	// open the registry subkey
	if (RegCreateKeyEx(HKEY_CURRENT_USER, cstrKeyRoot + m_strKey, 0, strKeyClass.GetBuffer(0), REG_OPTION_NON_VOLATILE, KEY_READ, NULL, &hKey, &dwDisposition) == ERROR_SUCCESS) {

		// read the value from the registry
		bResult = (RegQueryValueEx(hKey, strSetting.GetBuffer(0), 0, NULL, (unsigned char*)strValue.GetBuffer(dwValueSize), &dwValueSize) == ERROR_SUCCESS);
		strValue.ReleaseBuffer();

		// close the registry
		RegCloseKey(hKey);
	}

	if (!bResult) {
		TRACE("CSettings::ReadRegistryValue: unable to read setting (%s) from registry\n", strSetting);
		strValue.Empty();
	}

	return strValue;
}

BOOL CSettings::DeleteRegistryValue(CString strSetting)
{
	DWORD dwDisposition;
	HKEY hKey;
	BOOL bResult = FALSE;

	// the key name must not be empty
	ASSERT(!m_strKey.IsEmpty());

	if (m_strKey.IsEmpty()) {
		return FALSE;
	}

	// the setting string must not be empty
	ASSERT(!strSetting.IsEmpty());

	// open the registry subkey
	if (RegCreateKeyEx(HKEY_CURRENT_USER, cstrKeyRoot + m_strKey, 0, strKeyClass.GetBuffer(0), REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, &dwDisposition) == ERROR_SUCCESS) {

		// delete the setting from the registry
		bResult = (RegDeleteValue(hKey, strSetting.GetBuffer(0)) == ERROR_SUCCESS);

		// close the registry
		RegCloseKey(hKey);
	}

	if (!bResult) {
		TRACE("CSettings::DeleteRegistryValue: unable to delete setting (%s) from registry\n", strSetting);
	}

	return bResult;
}

#ifdef _DEBUG
void CSettings::DumpSettings(CString strDesc)
{
	CString strSetting;
	CString strValue;
	POSITION pos;

	TRACE("Dumping %s:\n", strDesc);

	// interate through the map
	for (pos = m_mapSettings.GetStartPosition(); pos != NULL; ) {
		m_mapSettings.GetNextAssoc(pos, strSetting, strValue);
		TRACE("  %s = %s\n", (LPCTSTR)strSetting, (LPCTSTR)strValue);
	}
	TRACE("End of dump\n");
}
#endif // _DEBUG


TARGET_DATA BOOL gbLastSubSuite = FALSE;
TARGET_DATA BOOL gbPersistTarget = FALSE;