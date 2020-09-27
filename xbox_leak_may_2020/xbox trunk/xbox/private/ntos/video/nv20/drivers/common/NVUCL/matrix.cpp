/*
 * (C) Copyright NVIDIA Corporation Inc., 1996,1997,1998. All rights reserved.
 */
/********************************* Direct 3D *******************************\
*                                                                           *
* Module: MATRIX.CPP                                                        *
*   Matrix private implementation                                           *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Ben de Waal                 07/07/98 - Created                      *
*                                                                           *
\***************************************************************************/
#include "precomp.h"
#include "matrix.h"

/*
 * attach
 *
 * bump reference count up
 */
NVUCL_RESULT NVUCL_STDCALL UCL_MATRIX::attach
(
    void
)
{
    m_dwRefCount++;
    return NVUCL_OK;
}

/*
 * detach
 *
 * bump reference count down & delete object if it reaches zero
 */
NVUCL_RESULT NVUCL_STDCALL UCL_MATRIX::detach
(
    void
)
{
    if (--m_dwRefCount)
    {
        return NVUCL_OK;
    }
    /*
     * kill self
     */
    delete this;
    return NVUCL_OK;
}

/*
 * setStore
 *
 * redirect matrix element storage
 */
NVUCL_RESULT NVUCL_STDCALL UCL_MATRIX::setStore
(
    float *pfAddress
)
{
    m_fElement = pfAddress;
    classify();
    return NVUCL_OK;
}


/*
 * setUnit
 *
 * sets the matrix to unity
 */
NVUCL_RESULT NVUCL_STDCALL UCL_MATRIX::setUnit
(
    void
)
{
    /*
     * set to unit
     */
    memset (m_fElement,0,sizeof(float) * 16);
    m_fElement[0] = m_fElement[5] = m_fElement[10] = m_fElement[15] = 1.0f;
    m_eClass = unit;
    /*
     * done
     */
    return NVUCL_OK;
}

/*
 * setElement
 * 
 * set a specific element
 */
NVUCL_RESULT NVUCL_STDCALL UCL_MATRIX::setElement
(
    DWORD dwIndex,
    float fValue
)
{
    /*
     * check parameters
     */
    if (dwIndex > 15)
    {
        return NVUCL_ERROR_INVALIDPARAM;
    }
    /*
     * assign
     */
    m_fElement[dwIndex] = fValue;

    /*
     * reclassify matrix
     */
    classify();

    /*
     * done
     */
    return NVUCL_OK;
}

/*
 * setRow
 *
 * set an entire row
 */
NVUCL_RESULT NVUCL_STDCALL UCL_MATRIX::setRow
(
    DWORD dwRow,
    float *pfValue
)
{
    /*
     * check parameters
     */
    if (dwRow > 3)
    {
        return NVUCL_ERROR_INVALIDPARAM;
    }
    /*
     * assign
     */
    memcpy (m_fElement + (dwRow << 2),pfValue,sizeof(float) * 4);

    /*
     * reclassify matrix
     */
    classify();

    /*
     * done
     */
    return NVUCL_OK;
}

/*
 * setAll
 *
 * set all elements in matrix
 */
NVUCL_RESULT NVUCL_STDCALL UCL_MATRIX::setAll
(
    float *pfValue
)
{
    return NVUCL_ERROR_NOTIMLEMENTED;
}

/*
 * getElement
 *
 * get a specific element
 */
NVUCL_RESULT NVUCL_STDCALL UCL_MATRIX::getElement
(
    DWORD dwIndex,
    float *pfValue
)
{
    return NVUCL_ERROR_NOTIMLEMENTED;
}

/*
 * getRow
 *
 * get a whole row of elements
 */
NVUCL_RESULT NVUCL_STDCALL UCL_MATRIX::getRow
(
    DWORD dwRow,
    float *pfValue
)
{
    return NVUCL_ERROR_NOTIMLEMENTED;
}


/*
 * getAll
 *
 * get all the elements
 */
NVUCL_RESULT NVUCL_STDCALL UCL_MATRIX::getAll
(
    float *pfValue
)
{
    return NVUCL_ERROR_NOTIMLEMENTED;
}

/*
 * multiply
 *
 * multiply tow matrices
 */
NVUCL_RESULT NVUCL_STDCALL UCL_MATRIX::multiply
(
    void *pMatrix
)
{
    return NVUCL_ERROR_NOTIMLEMENTED;
}

/*
 * invert
 *
 * invert matrix
 */
NVUCL_RESULT NVUCL_STDCALL UCL_MATRIX::invert
(
    void
)
{
    return NVUCL_ERROR_NOTIMLEMENTED;
}

/*
 * classify
 *
 * look at elements in matrix and classify acorrdingly
 */
void UCL_MATRIX::classify
(
    void
)
{
    /*
     * test for general
     */
    if (m_fElement[3] || m_fElement[7] || m_fElement[11] || (m_fElement[15] != 1.0f))
    {
        m_eClass = general;
        return;
    }

    /*
     * test for translation
     */
    if (m_fElement[12] || m_fElement[13] || m_fElement[14])
    {
        m_eClass = translation;
    }
    else
    {
        m_eClass = unit;
    }

    /*
     * test for rotation
     */
    if ((m_fElement[0] != 1.0f) || (m_fElement[5] != 1.0f) || (m_fElement[10] != 1.0f)
     || m_fElement[1] || m_fElement[2] 
     || m_fElement[4] || m_fElement[6]
     || m_fElement[8] || m_fElement[9])
    {
        m_eClass = (m_eClass == unit) ? rotation : threebyfour;
    }
}

/*
 * construction
 */
UCL_MATRIX::UCL_MATRIX (void)
{
    m_dwRefCount = 1;
    m_fElement   = m_fInternalStorage;
    setUnit();
}