//
// Debug API - Socket Functions
//

#include "tests.h"
#include "logging.h"

static void testDmIsSecurityEnabled();
static void testDmAddUser();
static void testDmRemoveUser();
static void testUserAccess();
static void testDmWalkUserList();
static void testDmCloseUserList();
static void testDmSetAdminPassword(); // Win32 only

unsigned int hlpNumberOfUsers()
{
	PDM_WALK_USERS	walk = NULL;
	DM_USER			user;
	unsigned int	count = 0;
	char			buf[256];
	HRESULT			hr;

	OutputDebugString("*** begin users list ***\n");
	VERIFY_HRESULT_ALT(DmWalkUserList(&walk, &user), XBDM_NOERR, XBDM_ENDOFLIST);
	while (SUCCEEDED(hr) && (hr != XBDM_ENDOFLIST))
	{
		count++;
		wsprintf(buf, "user: %-12s access: [%s]read [%s]write [%s]ctrl [%s]cfg [%s]manage\n", 
				user.UserName,
				user.AccessPrivileges & DMPL_PRIV_READ		? "+" : "-",
				user.AccessPrivileges & DMPL_PRIV_WRITE		? "+" : "-",
				user.AccessPrivileges & DMPL_PRIV_CONTROL	? "+" : "-",
				user.AccessPrivileges & DMPL_PRIV_CONFIGURE	? "+" : "-",
				user.AccessPrivileges & DMPL_PRIV_MANAGE	? "+" : "-"
				);
		OutputDebugString(buf);
		VERIFY_HRESULT_ALT(DmWalkUserList(&walk, &user), XBDM_NOERR, XBDM_ENDOFLIST);
	}
	
	DmCloseUserList(walk);
	OutputDebugString("*** end users list ***\n");
	return count;
}

void testSecurity()
{
	testDmIsSecurityEnabled();
	testDmAddUser();
	testDmRemoveUser();
	testUserAccess();
}

void testDmIsSecurityEnabled()
{
	HRESULT			hr;
	BOOL			fTemp, fSetting;

	// very important!  why is that?
	VERIFY_HRESULT(DmUseSharedConnection(TRUE), XBDM_NOERR);

	VERIFY_HRESULT(DmIsSecurityEnabled(NULL), E_INVALIDARG);
	VERIFY_HRESULT(DmIsSecurityEnabled(&fTemp /* valid ptr */), XBDM_NOERR);

	VERIFY_HRESULT(DmEnableSecurity(TRUE), XBDM_NOERR);
	VERIFY_HRESULT(DmIsSecurityEnabled(&fSetting), XBDM_NOERR);
	VERIFY_TRUE(fSetting == TRUE);

	VERIFY_HRESULT(DmEnableSecurity(FALSE), XBDM_NOERR);
	VERIFY_HRESULT(DmIsSecurityEnabled(&fSetting), XBDM_NOERR);
	VERIFY_TRUE(fSetting == FALSE);
}

void testDmAddUser()
{
	HRESULT			hr;
	BOOL			fSecure;
	char			szLocalhost[256];
	DWORD			dwSize = sizeof szLocalhost;

	// get name of this box, for later use
	// TODO: DmAddUser - figure out what to do when running natively on xbox

	GetComputerName(szLocalhost, &dwSize);

	VERIFY_HRESULT(DmUseSharedConnection(TRUE), XBDM_NOERR);
	VERIFY_HRESULT(DmIsSecurityEnabled(&fSecure), XBDM_NOERR);

	// unlock box for start of tests, and abort if it fails

	if (fSecure)
		VERIFY_HRESULT(DmEnableSecurity(FALSE), XBDM_NOERR);
	if (FAILED(hr))
		return;

	// try adding users while the box isn't locked

	VERIFY_HRESULT(DmAddUser("BOGUS-NOTLOCKED", -1), XBDM_NOTLOCKED);
	VERIFY_TRUE(hlpNumberOfUsers() == 0);

	// now lock box

	VERIFY_HRESULT(DmEnableSecurity(TRUE), XBDM_NOERR);

	// fire up some users

	VERIFY_TRUE(hlpNumberOfUsers() == 0);
	VERIFY_HRESULT(DmAddUser(szLocalhost, -1), XBDM_NOERR);
	VERIFY_TRUE(hlpNumberOfUsers() == 1);
	VERIFY_HRESULT(DmAddUser(szLocalhost /* dupe */, -1), XBDM_ALREADYEXISTS);
	VERIFY_TRUE(hlpNumberOfUsers() == 1);

	// reverse case of username	

	for (int i = strlen(szLocalhost); i; i--)
	{
		if (isupper(szLocalhost[i-1]))
			szLocalhost[i-1] = tolower(szLocalhost[i-1]);
		else
			szLocalhost[i-1] = toupper(szLocalhost[i-1]);
	}

	VERIFY_HRESULT(DmAddUser(szLocalhost /* dupe in diff case */, -1), XBDM_ALREADYEXISTS);
	VERIFY_TRUE(hlpNumberOfUsers() == 1);

	VERIFY_HRESULT(DmAddUser("DmAddUser-NONE", 0), E_INVALIDARG);
	VERIFY_HRESULT(DmAddUser(NULL, DMPL_PRIV_READ), E_INVALIDARG);
	// woot: bug
	VERIFY_HRESULT(DmAddUser("", DMPL_PRIV_READ), E_INVALIDARG);

	VERIFY_HRESULT(DmAddUser("DmAddUser-READ", DMPL_PRIV_READ), XBDM_NOERR);
	VERIFY_HRESULT(DmAddUser("DmAddUser-WRITE", DMPL_PRIV_WRITE), XBDM_NOERR);
	VERIFY_HRESULT(DmAddUser("DmAddUser-CTRL", DMPL_PRIV_CONTROL), XBDM_NOERR);
	VERIFY_HRESULT(DmAddUser("DmAddUser-CFG", DMPL_PRIV_CONFIGURE), XBDM_NOERR);
	VERIFY_HRESULT(DmAddUser("DmAddUser-MANAGE", DMPL_PRIV_MANAGE), XBDM_NOERR);
	VERIFY_HRESULT(DmAddUser("DmAddUser-MIXED", DMPL_PRIV_READ|DMPL_PRIV_WRITE|DMPL_PRIV_MANAGE), XBDM_NOERR);
	VERIFY_TRUE(hlpNumberOfUsers() == 7);

	// TODO: DmAddUser - mbcs user name
	// unlock on exit

	VERIFY_HRESULT(DmEnableSecurity(FALSE), XBDM_NOERR);
}

void testDmRemoveUser()
{
	HRESULT			hr;
	BOOL			fSecure;
	char			szLocalhost[256];
	DWORD			dwSize = sizeof szLocalhost;

	// get name of this box, for later use
	// TODO: DmRemoveUser - figure out what to do when running natively on xbox

	GetComputerName(szLocalhost, &dwSize);

	VERIFY_HRESULT(DmUseSharedConnection(TRUE), XBDM_NOERR);
	VERIFY_HRESULT(DmIsSecurityEnabled(&fSecure), XBDM_NOERR);

	// unlock box for start of tests, and abort if it fails

	if (fSecure)
		VERIFY_HRESULT(DmEnableSecurity(FALSE), XBDM_NOERR);
	if (FAILED(hr))
		return;

	// try removing users while the box isn't locked

	VERIFY_HRESULT(DmRemoveUser("BOGUS-NOTLOCKED"), XBDM_NOTLOCKED);

	// now lock box

	VERIFY_HRESULT(DmEnableSecurity(TRUE), XBDM_NOERR);

	// fire up some users for the test to delete

	VERIFY_HRESULT(DmAddUser(szLocalhost, -1), XBDM_NOERR);
	VERIFY_HRESULT(DmAddUser("DmRemoveUser-OK", -1), XBDM_NOERR);
	VERIFY_HRESULT(DmAddUser("DmRemoveUser-READ", DMPL_PRIV_READ), XBDM_NOERR);
	VERIFY_HRESULT(DmAddUser("DmRemoveUser-WRITE", DMPL_PRIV_WRITE), XBDM_NOERR);
	VERIFY_HRESULT(DmAddUser("DmRemoveUser-CTRL", DMPL_PRIV_CONTROL), XBDM_NOERR);
	VERIFY_HRESULT(DmAddUser("DmRemoveUser-CFG", DMPL_PRIV_CONFIGURE), XBDM_NOERR);
	VERIFY_HRESULT(DmAddUser("DmRemoveUser-MANAGE", DMPL_PRIV_MANAGE), XBDM_NOERR);
	VERIFY_HRESULT(DmAddUser("DmRemoveUser-MIXED", DMPL_PRIV_READ|DMPL_PRIV_WRITE|DMPL_PRIV_MANAGE), XBDM_NOERR);

	// start deleting...

	VERIFY_HRESULT(DmRemoveUser(NULL), E_INVALIDARG);
	VERIFY_HRESULT(DmRemoveUser(""), XBDM_NOSUCHFILE);
	VERIFY_HRESULT(DmRemoveUser("DmRemoveUser-OK"), XBDM_NOERR);
	VERIFY_HRESULT(DmRemoveUser("DmRemoveUser-NOTTHERE"), XBDM_NOSUCHFILE);
	VERIFY_HRESULT(DmRemoveUser("DmRemoveUser-mixed" /* diff case than add */), XBDM_NOERR);

	// throw in an add, and continue deleting

	VERIFY_HRESULT(DmAddUser("DmRemoveUser-OK", -1), XBDM_NOERR);
	VERIFY_HRESULT(DmRemoveUser("DmRemoveUser-OK"), XBDM_NOERR);
	VERIFY_HRESULT(DmRemoveUser("DmRemoveUser-READ"), XBDM_NOERR);
	VERIFY_HRESULT(DmRemoveUser("DmRemoveUser-WRITE"), XBDM_NOERR);
	VERIFY_HRESULT(DmRemoveUser("DmRemoveUser-CTRL"), XBDM_NOERR);
	VERIFY_HRESULT(DmRemoveUser("DmRemoveUser-CFG"), XBDM_NOERR);
	VERIFY_HRESULT(DmRemoveUser("DmRemoveUser-MANAGE"), XBDM_NOERR);
	VERIFY_TRUE(hlpNumberOfUsers() == 1);

	// unlock on exit

	VERIFY_HRESULT(DmEnableSecurity(FALSE), XBDM_NOERR);

}

void testUserAccess()
{
	HRESULT			hr;
	BOOL			fSecure;
	char			szLocalhost[256];
	DWORD			dwSize = sizeof szLocalhost;
	DWORD			dwAccess;

	// get name of this box, for later use
	// TODO: testUserAccess - figure out what to do when running natively on xbox

	GetComputerName(szLocalhost, &dwSize);

	VERIFY_HRESULT(DmUseSharedConnection(TRUE), XBDM_NOERR);
	VERIFY_HRESULT(DmIsSecurityEnabled(&fSecure), XBDM_NOERR);

	// unlock box for start of tests, and abort if it fails

	if (fSecure)
		VERIFY_HRESULT(DmEnableSecurity(FALSE), XBDM_NOERR);
	if (FAILED(hr))
		return;

	// now all user settings have been cleared...					  

	VERIFY_HRESULT(DmGetUserAccess("BOGUS-NOTLOCKED", &dwAccess), XBDM_NOTLOCKED);
	VERIFY_HRESULT(DmSetUserAccess("BOGUS-NOTLOCKED", -1), XBDM_NOTLOCKED);

	// now lock box

	VERIFY_HRESULT(DmEnableSecurity(TRUE), XBDM_NOERR);

	VERIFY_HRESULT(DmAddUser(szLocalhost, -1), XBDM_NOERR);
	VERIFY_HRESULT(DmAddUser("UserAccess-NONE", 0), XBDM_NOERR);
	VERIFY_HRESULT(DmAddUser("UserAccess-ALL", -1), XBDM_NOERR);

	// test cases

	VERIFY_HRESULT(DmGetUserAccess("UserAccess-NONE", &dwAccess), XBDM_NOERR);
	VERIFY_TRUE(dwAccess == 0);
	VERIFY_HRESULT(DmGetUserAccess("UserAccess-ALL", &dwAccess), XBDM_NOERR);
	VERIFY_TRUE(dwAccess == -1);
	VERIFY_HRESULT(DmSetUserAccess("UserAccess-NONE", DMPL_PRIV_CONTROL), XBDM_NOERR);
	VERIFY_HRESULT(DmGetUserAccess("UserAccess-NONE", &dwAccess), XBDM_NOERR);
	VERIFY_TRUE(dwAccess == DMPL_PRIV_CONTROL);

	VERIFY_HRESULT(DmGetUserAccess("BOGUS", &dwAccess), XBDM_NOSUCHFILE);
	VERIFY_HRESULT(DmSetUserAccess("BOGUS", -1), XBDM_NOSUCHFILE);
	// woot: bug
	VERIFY_HRESULT(DmGetUserAccess(NULL, &dwAccess), E_INVALIDARG);
	VERIFY_HRESULT(DmSetUserAccess(NULL, -1), E_INVALIDARG);
	VERIFY_HRESULT(DmGetUserAccess("", &dwAccess), XBDM_NOSUCHFILE);
	VERIFY_HRESULT(DmSetUserAccess("", -1), XBDM_NOSUCHFILE);

	// unlock on exit

	VERIFY_HRESULT(DmEnableSecurity(FALSE), XBDM_NOERR);
}
