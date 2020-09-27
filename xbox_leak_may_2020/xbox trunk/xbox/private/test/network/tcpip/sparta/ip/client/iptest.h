#ifndef __IPTEST_H__
#define __IPTEST_H__

//==================================================================================
// Includes
//==================================================================================
#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>
#include <xtl.h>
#include <xnetref.h>
#include <xlog.h>
#include <xtestlib.h>
#include <netsync.h>
#include <xlog.h>
#include <xboxp.h>
#include <stdio.h>

#include "ipmsg.h"

//==================================================================================
// Defines
//==================================================================================
#define IP_SERVER_DLL_W L"ipserv.dll"
#define IP_SERVER_MAIN_FUNCTION_A "IpServerCallbackFunction"
#define IP_SERVER_START_FUNCTION_A "IpServerStartFunction"
#define IP_SERVER_STOP_FUNCTION_A "IpServerStopFunction"

#define MAX_REQUEST_RETRIES 5
#define RETRY_TIMEOUT 3000
#define SIM_REASM_MAX 4
#define TIME_INFINITY 10000

#define MAX_SIMUL_PORT_OFFSET 5
#define ABOVE_MAX_SIMUL_PORT_OFFSET 5
//==================================================================================
// Typedefs
//==================================================================================

// IP test function
typedef BOOL (*PIP_TEST_FUNCTION)(WORD LowPort, WORD HighPort);

// States that sockets can be monitored for
typedef enum _SOCKET_STATE
{
	READABLE,
	WRITEABLE,
	EXCEPTION
} SOCKET_STATE;

//==================================================================================
// Prototypes 
//==================================================================================
// Harness functions
BOOL WINAPI IpTestDllMain(IN HINSTANCE hInstance, IN DWORD dwReason, IN LPVOID lpContext);
VOID WINAPI IpTestStartTest(IN HANDLE hLog);
VOID WINAPI IpTestEndTest();

// Test case functions
BOOL IPVersionSixTest(WORD LowPort, WORD HighPort);
BOOL IPVersionBadTest(WORD LowPort, WORD HighPort);
BOOL IPHeaderLenBelowMinTest(WORD LowPort, WORD HighPort);
BOOL IPHeaderLenMaxTest(WORD LowPort, WORD HighPort);
BOOL IPHeaderLenAboveTotalLenTest(WORD LowPort, WORD HighPort);
BOOL IPHeaderLenAboveAvailTest(WORD LowPort, WORD HighPort);
BOOL IPTosNormalTest(WORD LowPort, WORD HighPort);
BOOL IPTosNetControlTest(WORD LowPort, WORD HighPort);
BOOL IPTosLowDelayTest(WORD LowPort, WORD HighPort);
BOOL IPTosHighThroughputTest(WORD LowPort, WORD HighPort);
BOOL IPTosHighReliabilityTest(WORD LowPort, WORD HighPort);
BOOL IPTosReservedTest(WORD LowPort, WORD HighPort);
BOOL IPLengthBelowMinTest(WORD LowPort, WORD HighPort);
BOOL IPLengthAtMinTest(WORD LowPort, WORD HighPort);
BOOL IPLengthAboveMinTest(WORD LowPort, WORD HighPort);
BOOL IPLengthAboveTotalTest(WORD LowPort, WORD HighPort);
BOOL IPLengthBelowTotalTest(WORD LowPort, WORD HighPort);
BOOL IPLengthMaxTest(WORD LowPort, WORD HighPort);
BOOL IPIDMinTest(WORD LowPort, WORD HighPort);
BOOL IPIDMaxTest(WORD LowPort, WORD HighPort);
BOOL IPFlagReservedSetTest(WORD LowPort, WORD HighPort);
BOOL IPFlagDontFragAndMoreFragsTest(WORD LowPort, WORD HighPort);
BOOL IPTtlMinTest(WORD LowPort, WORD HighPort);
BOOL IPTtlMaxTest(WORD LowPort, WORD HighPort);
BOOL IPProtocolUnsupportedTest(WORD LowPort, WORD HighPort);
BOOL IPProtocolICMPTooSmallTest(WORD LowPort, WORD HighPort);
BOOL IPProtocolIGMPTooSmallTest(WORD LowPort, WORD HighPort);
BOOL IPProtocolTCPTooSmallTest(WORD LowPort, WORD HighPort);
BOOL IPProtocolUDPTooSmallTest(WORD LowPort, WORD HighPort);
BOOL IPProtocolUnsupportedTooSmallTest(WORD LowPort, WORD HighPort);
BOOL IPChecksumBadTest(WORD LowPort, WORD HighPort);
BOOL IPOptionsAboveAvailTest(WORD LowPort, WORD HighPort);
BOOL IPOptionsExactSizeTest(WORD LowPort, WORD HighPort);
BOOL IPOptionsEndOfOpsLastTest(WORD LowPort, WORD HighPort);
BOOL IPOptionsEndOfOpsFirstTest(WORD LowPort, WORD HighPort);
BOOL IPFragFullReverseTest(WORD LowPort, WORD HighPort);
BOOL IPFragMidReverseTest(WORD LowPort, WORD HighPort);
BOOL IPFragLastInSecondTest(WORD LowPort, WORD HighPort);
BOOL IPFragMixedSizeTest(WORD LowPort, WORD HighPort);
BOOL IPFragOneHundredTest(WORD LowPort, WORD HighPort);
BOOL IPFragMultipleLastFragmentTest(WORD LowPort, WORD HighPort);
BOOL IPFragOverlappingFragmentsTest(WORD LowPort, WORD HighPort);
BOOL IPFragMaxDatagramSizeTest(WORD LowPort, WORD HighPort);
BOOL IPFragMaxReassemblySizeTest(WORD LowPort, WORD HighPort);
BOOL IPFragAboveMaxReassemblySizeTest(WORD LowPort, WORD HighPort);
BOOL IPFragMaxSimulReassemblyTest(WORD LowPort, WORD HighPort);
BOOL IPFragAboveMaxSimulReassemblyTest(WORD LowPort, WORD HighPort);
BOOL IPFragOversizedFragmentsTest(WORD LowPort, WORD HighPort);
BOOL IPAttackTeardropTest(WORD LowPort, WORD HighPort);
BOOL IPAttackNewTeardropTest(WORD LowPort, WORD HighPort);
BOOL IPAttackImpTeardropTest(WORD LowPort, WORD HighPort);
BOOL IPAttackSynDropTest(WORD LowPort, WORD HighPort);
BOOL IPAttackBonkTest(WORD LowPort, WORD HighPort);
BOOL IPAttackZeroLenOptTest(WORD LowPort, WORD HighPort);
BOOL IPAttackNesteaTest(WORD LowPort, WORD HighPort);
#endif // __IPTEST_H__
