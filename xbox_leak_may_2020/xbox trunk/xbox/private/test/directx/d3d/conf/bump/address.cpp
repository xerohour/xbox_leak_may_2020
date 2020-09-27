// FILE:        address.cpp
// DESC:        texture address mode tests
// AUTHOR:      Todd M. Frost

#include "d3dlocus.h"
#include "cd3dtest.h"
#include "3dmath.h"
#include "cshapes.h"
#include "cshapesfvf.h"
#include "bump.h"

// NAME:        CBumpAddress()
// DESC:        bump address class constructor
// INPUT:       none
// OUTPUT:      none

CBumpAddress::CBumpAddress()
{
    m_szTestName = TEXT("BUMPENV address");
    m_szCommandKey = TEXT("Address");
}

// NAME:        ~CBumpAddress()
// DESC:        bump address class destructor
// INPUT:       none
// OUTPUT:      none

CBumpAddress::~CBumpAddress()
{
}

// NAME:        CommandLineHelp()
// DESC:        command line help
// INPUT:       none
// OUTPUT:      none

void CBumpAddress::CommandLineHelp(void)
{
    CBump::CommandLineHelp();
}

// NAME:        TestInitialize()
// DESC:        initialize test
// INPUT:       none
// OUTPUT:      CBump::TestInitialize()

UINT CBumpAddress::TestInitialize(VOID)
{
    UINT uResult = CBump::TestInitialize();

    if (D3DTESTINIT_RUN != uResult)
        return uResult;

    SetTestRange((UINT) 1, (UINT) (m_uCommonTextureFormats*NMODES*NMODES));
    return D3DTESTINIT_RUN;
}

// NAME:        ExecuteTest()
// DESC:        execute test
// INPUT:       uTest........test number
// OUTPUT:      CBump::ExecuteTests()....if test executed
//              false....................otherwise

bool CBumpAddress::ExecuteTest(UINT uTest)
{
    DWORD dwFormat, dwWidth = (DWORD) 256, dwHeight = (DWORD) 256;
    UINT i, j, uTemp;

    for (uTemp = 1, dwFormat = 0; dwFormat < m_uCommonTextureFormats; dwFormat++)
        for (i = 0; i < NMODES; i++)
            for (j = 0; j < NMODES; j++, uTemp++)
                if (uTemp == uTest)
                {
                    if (((i == j) || m_Flags.bCheck(INDEPENDENT)) &&
                        (m_dwTexAddressCaps & m_dwModeCapsVals[j][0]) &&
                        (m_dwTexAddressCaps & m_dwModeCapsVals[i][0]) &&
                        bSetBump(dwFormat, dwWidth, dwHeight))
                    {
                        m_dwModes[0] = (D3DTEXTUREADDRESS) m_dwModeCapsVals[i][1];
                        m_dwModes[1] = (D3DTEXTUREADDRESS) m_dwModeCapsVals[j][1];
                        return CBump::ExecuteTest(uTest);
                    }
                    else
                    {
                        SkipTests((UINT) 1);
                        return false;
                    }
                }

    return false;
}
