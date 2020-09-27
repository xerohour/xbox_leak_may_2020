// FILE:        matrix.cpp
// DESC:        bump matrix tests
// AUTHOR:      Todd M. Frost

#include "d3dlocus.h"
#include "cd3dtest.h"
#include "3dmath.h"
#include "cshapes.h"
#include "cshapesfvf.h"
#include "bump.h"

const FLOAT MAGNITUDE = 0.1f; // matrix magnitude

// NAME:        CBumpMatrix()
// DESC:        bump matrix class constructor
// INPUT:       none
// OUTPUT:      none

CBumpMatrix::CBumpMatrix()
{
    m_szTestName = TEXT("BUMPENV matrix");
    m_szCommandKey = TEXT("Matrix");
}

// NAME:        ~CBumpMatrix()
// DESC:        bump matrix class destructor
// INPUT:       none
// OUTPUT:      none

CBumpMatrix::~CBumpMatrix()
{
}

// NAME:        CommandLineHelp()
// DESC:        command line help
// INPUT:       none
// OUTPUT:      none

void CBumpMatrix::CommandLineHelp(void)
{
    CBump::CommandLineHelp();
//    WriteCommandLineHelp("$yvariations: $wvariations per test $c(default=8)");
}

// NAME:        TestInitialize()
// DESC:        initialize test
// INPUT:       none
// OUTPUT:      CBump::TestInitialize()

UINT CBumpMatrix::TestInitialize(VOID)
{
    UINT uResult = CBump::TestInitialize();

    if (D3DTESTINIT_RUN != uResult)
        return uResult;

    SetTestRange((UINT) 1, (UINT) (m_uCommonTextureFormats*NMATRICES*m_uVariations));
    return D3DTESTINIT_RUN;
}

// NAME:        ExecuteTest()
// DESC:        execute test
// INPUT:       uTest........test number
// OUTPUT:      CBump::ExecuteTests()....if test executed
//              false....................otherwise
// COMMENTS:    D3DTADDRESS_MIRROR most visually pleasing, substitute for wrap if supported

bool CBumpMatrix::ExecuteTest(UINT uTest)
{
    DWORD dwFormat, dwWidth = (DWORD) 256, dwHeight = (DWORD) 256;
    FLOAT fDu, fDv, fTemp;
    UINT i, j, uTemp;

    if (m_dwTexAddressCaps & D3DPTADDRESSCAPS_MIRROR)
        m_dwModes[1] = m_dwModes[0] = D3DTADDRESS_MIRROR;

    for (uTemp = 1, dwFormat = 0; dwFormat < m_uCommonTextureFormats; dwFormat++)
        for (i = 0; i < NMATRICES; i++)
            for (j = 1; j <= m_uVariations; j++, uTemp++)
                if (uTemp == uTest)
                {
                    if (bSetBump(dwFormat, dwWidth, dwHeight))
                    {
                        fTemp = 1.0f - 2.0f*fNormalize(j, 1, m_uVariations);
                        fDu = fDv = 2.0f*MAGNITUDE*fTemp;
                        vSetMatrix(i, fDu, fDv);
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
