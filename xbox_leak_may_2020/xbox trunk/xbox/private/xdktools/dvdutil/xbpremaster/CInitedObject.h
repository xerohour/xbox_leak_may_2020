// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:     CInitedObject.h
// Contents: Contains the declaration of the CInitedObject object class.
//
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ CLASSES +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Class:   CInitedObject
// Purpose: Tracks initialization state of an object.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CInitedObject
{
public:

    // ++++ CONSTRUCTION-RELATED FUNCTIONS ++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    // CInitedObject        -- Standard constructor
    CInitedObject()
    {
        m_hrInit = E_FAIL;
    }


    // ++++ ACCESSOR FUNCTIONS ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    // GetInitCode          -- Returns the result of initialization
    HRESULT GetInitCode()
    {
        try
        {
            if (this == NULL)
                return E_FAIL;
            return m_hrInit;
        }
        catch(...)
        {
            // Something unexpected happened - most likely we were passed an invalid 'this' pointer
            return E_FAIL;
        }
    }

    // IsInited             -- Returns 'true' if the object was successfully initialized
    bool IsInited()
    {
        return GetInitCode() == S_OK;
    }


protected:

    // ++++ ACCESSOR FUNCTIONS ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    // SetInited            -- Allows a derived class to clear or set the initialized state to track
    //                         the derived class' initialization success.
    void SetInited(HRESULT hrInit)
    {
        m_hrInit = hrInit;
    }

private:

    // m_hrInit             -- Tracks the result of object initialization.
    HRESULT m_hrInit;
};
