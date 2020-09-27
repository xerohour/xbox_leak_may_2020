#if !defined(__MEM_CHECK_H__)
#define __MEM_CHECK_H__

#if defined(_DEBUG)
#include <crtdbg.h>

#if !defined(QA_DBG_OUTPUT_DEBUGWIN)

#define  QA_SEND_TO_STDOUT() \
    _CrtSetReportMode( _CRT_WARN, _CRTDBG_MODE_FILE ); \
    _CrtSetReportFile( _CRT_WARN, _CRTDBG_FILE_STDOUT ); \
    _CrtSetReportMode( _CRT_ERROR, _CRTDBG_MODE_FILE ); \
    _CrtSetReportFile( _CRT_ERROR, _CRTDBG_FILE_STDOUT ); \
    _CrtSetReportMode( _CRT_ASSERT, _CRTDBG_MODE_FILE ); \
    _CrtSetReportFile( _CRT_ASSERT, _CRTDBG_FILE_STDOUT )
#else
#define  QA_SEND_TO_STDOUT()
#endif

#define  SET_CRT_DEBUG_FIELD(a) \
            _CrtSetDbgFlag((a) | _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG))
#define  CLEAR_CRT_DEBUG_FIELD(a) \
            _CrtSetDbgFlag(~(a) & _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG))

#define QA_DISPLAY_MEM_LEAKS_STDOUT() \
	QA_SEND_TO_STDOUT(); \
    SET_CRT_DEBUG_FIELD(_CRTDBG_LEAK_CHECK_DF)

class CMemCheck
{
public:
  CMemCheck(int line)
  : m_line(line)
  {
    //!: how much of this is necessary?
	QA_DISPLAY_MEM_LEAKS_STDOUT();
    _CrtMemCheckpoint(&m_memState);
  }
  ~CMemCheck()
  {
    _CrtMemState  cur;
    _CrtMemState  result;
    _CrtMemCheckpoint(&cur);

    if (_CrtMemDifference(&result, &m_memState, &cur ))
    {
      complain(m_line);
      _CrtMemDumpAllObjectsSince(&m_memState);
    }
  }

private:
  int       m_line;
  _CrtMemState  m_memState;
};

#define MEM_CHECK_OBJECT CMemCheck      memCheck(__LINE__)
#define MEM_CHECK_OBJECT_GLOBAL CMemCheck g_memCheck(__LINE__)

#else

#define MEM_CHECK_OBJECT
#define MEM_CHECK_OBJECT_GLOBAL

#define  SET_CRT_DEBUG_FIELD(a)
#define  CLEAR_CRT_DEBUG_FIELD(a)

#define  QA_SEND_TO_STDOUT()

#define QA_DISPLAY_MEM_LEAKS_STDOUT()

#endif

#endif // __MEM_CHECK_H__
