#ifndef _BEXCEPTION_H
#define _BEXCEPTION_H

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// exception
//
class BException
{
public:
    //
    // codes
    //
    enum EXCEPTIONCODE
    {
        outOfMemory = 0x80000000,
    };

protected:
    EXCEPTIONCODE m_ecErrorCode;

public:
    BException (EXCEPTIONCODE ecErrorCode) { m_ecErrorCode = ecErrorCode; }
};

#endif