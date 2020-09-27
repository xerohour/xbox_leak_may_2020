/***********************************************************************
* Microsoft Vulcan
*
* Microsoft Confidential.  Copyright 1997-1999 Microsoft Corporation.
*
* File: vulcanerr.h
*
* File Comments:
*
*
***********************************************************************/

#pragma once

#include <exception>

class VErr;

typedef void (VULCANCALL *ErrHandler)(VErr &);

class VULCANDLL VErr
{
public:

    // DoErrHandler() executes the error handler(s).
    // Since this is a virtual method, error classes that
    // inherited from this class may provide their own versions.
    // However, all these versions should eventually pass control
    // to the ErrHandler that is current set.
    //
    virtual void  DoErrHandler();

    // SetErrHandler() replaces the current error handler.
    // The previous error handler is returned for restoring
    // at a later time if desired.
    //
    static ErrHandler VULCANCALL SetErrHandler(ErrHandler); /* UNDONE - To be removed */
    static ErrHandler VULCANCALL SetErrHandler(const char *szAppName = NULL, ErrHandler = NULL);

    // Warnings allow static control over whether warnings are output
    // (for the default error handler!)
    static void VULCANCALL OutputWarnings(bool output);
    static bool VULCANCALL IsOutputWarnings();

    // Your error handler may call the following methods to retrieve
    // information about this object. You may also call the
    // what() method inherited from the C++ standard exception class.
    //
    const char *AppName()       {return (const char *) m_szAppName;}
    DWORD       Flag()          {return m_Flag;}
    bool        IsAssert()      {return !!(m_Flag & FLG_ASSERT     );}
    bool        IsContinuable() {return !!(m_Flag & FLG_CONTINUABLE);}
    bool        IsFatal()       {return !!(m_Flag & FLG_FATAL      );}
    bool        IsWarning()     {return !!(m_Flag & FLG_WARNING    );}

    // These methods change the flags.
    //

    void SetFatal()             {m_Flag = FLG_FATAL;}
    void SetWarning()           {m_Flag = FLG_WARNING;}
    void SetContinuable()       {m_Flag = FLG_CONTINUABLE;}

    // This method gets the what field.  Normally the field is not available till DoErrHandler is called.
    char*       GetWhat();

    // Continuable errors are "delayed fatal errors." These errors are
    // marked for continuation so that as many such errors are displayed,
    // etc. before any "real damage" is done. For example, just before
    // an application is about to write out a file, it calls the method
    // HadContinuableErrors(). If the returned value is true, the
    // application will skip the file write and terminate with the
    // appropriate error code.
    //
    static bool VULCANCALL HadContinuableErrors() {return m_HadContinuableError;}
    static void VULCANCALL ClearContinuableErrors() {m_HadContinuableError = false;}

    // Use this early at application initializing time to set the
    // application name.
    //
    static void SetAppName(const char *);

    // Use this constructor to deal with an error condition.
    //
    VErr(const char *msg, DWORD flags);

    // Copy constructor.
    //
    VErr(VErr &verr);

    virtual ~VErr();

    const char       *what()     {return m_msg;}


    // Use these flags to indicate desired action to the
    // error handler.
    //
    enum
    {
        FLG_ASSERT      = 0x01,
        FLG_FATAL       = 0x02,
        FLG_CONTINUABLE = 0x04,
        FLG_WARNING     = 0x08
    };

protected:
    // These methods should only be used by inheriting classes.
    //
    VErr();
    char             *m_msg;

private:
    DWORD             m_Flag;

    static char      *m_szAppName;
    static bool       m_HadContinuableError;
    static bool       m_OutputWarnings;

    // Inheriting classes should call VErr::DoErrHandler() instead of going
    // directly to this member.
    //
    static ErrHandler m_errHandler;
};


//=============================================================================
//
// Example of using the VErr object and
// catching Vulcan errors in your application.
//
//=============================================================================
//
// int main(int argc, char *argv)
// {
//     VEr::SetErrHandler("MyVulcanApplication.Exe", NULL);   // use default error handler.
//
//     TheRestOfMyInitialization();
//
//     try
//     {
//         MakeVulcanCallsEtc();
//         ...
//     }
//     catch (VErr &verr)           // MUST BE caught by REFERENCE!!
//     {                            // because VErr class may be inherited and
//         verr.DoErrHandler();     // the DoErrHandler() of the inherited class
//         return 1;                // needs to execute.
//     }
//
//     if (myerrobj.HadContinuableError())
//     {
//         return 1;   // indicate error.
//     }
//
//     FILE *fout = fopen("MyOutputFile, "wt");
//     fwrite(MyData, sizeof(MyData), 1, fout);
//     fclose(fout);
//
//     return 0;    // success.
// }


//=============================================================================
//
// This is the source code to the default exception handler.
//
// Notice the stdcall convention. Your error handler must use this
// convention as well.
//
//=============================================================================
//
// void VULCANCALL DefaultErrorHandler(VErr &verr)
// {
//     if (verr.IsAssert())
//     {
//         VulcanMessageBox(verr.what(), verr.IsWarning());  // User may terminate;
//         return;
//     }
//
//     fflush(stdout);
//
//     if (verr.AppName())
//     {
//         fprintf(stderr, "%s : %s\n", verr.AppName(), verr.what());
//     }
//     else
//     {
//         fprintf(stderr, "%s\n", verr.what());
//     }
//
//     fprintf(stderr, "%s : %s\n", verr.AppName(), verr.what());
//     fflush(stderr);
//
//     if (verr.IsFatal())
//     {
//         exit(1);
//     }
// }


//=============================================================================
//
// Example of a class that inherits from VErr.
//
//=============================================================================
//
// class MyFortuneCookieErrorClass : public VErr
// {
// public:
//     MyFortuneCookieErrorClass(COOKIEID id) {m_id = id;}
//
//     MyFortuneCookieErrorClass(MyFortuneCookieErrorClass &me) : VErr(me)
//     {
//         // Copy constructor must alway be present for future VErr versions.
//
//         m_id = (me.id + time(NULL)) % ::COUNT_FORTUNE;
//     }
//
//     void DoErrHandler()
//     {
//         char *szFortune = ::RgszFortune[m_id];
//
//         // The DoErrHandler() of the inheriting class must
//         // render all its information as a simple string
//         // for the base VErr class to deal with because that
//         // is about all it understands.
//         // Then, VErr::DoErrHandler() has to be called. To do it
//         // all, just create a VErr class.
//
//         VErr verr(szFortune, VErr::FLG_FATAL);
//     }
//
// private:
//     COOKIEID m_id;
// };
