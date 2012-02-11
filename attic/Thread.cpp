/***************************************************/
/*! \class Thread
    \brief STK thread class.

    This class provides a uniform interface for cross-platform
    threads. On unix systems, the pthread library is used. Under
    Windows, the C runtime threadex functions are used.

    Each instance of the Thread class can be used to control a single
    thread process. Routines are provided to signal cancelation
    and/or joining with a thread, though it is not possible for this
    class to know the running status of a thread once it is started.

    For cross-platform compatability, thread functions should be
    declared as follows:

    THREAD_RETURN THREAD_TYPE thread_function(void *ptr)

    by Perry R. Cook and Gary P. Scavone, 1995 - 2010.
*/
/***************************************************/

#include "Thread.h"


Thread::Thread()
{
  m_thread = 0;
}


Thread::~Thread()
{
}


bool Thread::start( THREAD_FUNCTION routine, void * ptr )
{
  if ( m_thread ) {
    Exception::handle( "Thread:: a thread is already running!", Exception::WARNING );
    return false;
  }

#if( defined(__OS_LINUX__) || defined(__OS_MACOSX__) )

  if ( pthread_create(&m_thread, NULL, *routine, ptr) == 0 )
    return true;

#elif defined( __OS_WINDOWS__ )
  unsigned thread_id;
  m_thread = _beginthreadex(NULL, 0, routine, ptr, 0, &thread_id);
  if ( m_thread ) return true;

#endif
  return false;
}


bool Thread::cancel()
{
#if( defined(__OS_LINUX__) || defined(__OS_MACOSX__) )

  if( pthread_cancel( m_thread ) == 0 ) {
    return true;
  }

#elif defined( __OS_WINDOWS__ )

  TerminateThread( (HANDLE)m_thread, 0 );
  return true;

#endif
  return false;
}


bool Thread::wait()
{
#if( defined(__OS_LINUX__) || defined(__OS_MACOSX__) )

  if ( pthread_join( m_thread, NULL ) == 0 ) {
    m_thread = 0;
    return true;
  }

#elif defined( __OS_WINDOWS__ )

  long retval = WaitForSingleObject( (HANDLE)m_thread, INFINITE );
  if( retval == WAIT_OBJECT_0 ) {
    CloseHandle( (HANDLE)m_thread );
    m_thread = 0;
    return true;
  }

#endif
  return false;
}


void Thread::testCancel( void )
{
#if( defined(__OS_LINUX__) || defined(__OS_MACOSX__) )

  pthread_testcancel();

#endif
}

