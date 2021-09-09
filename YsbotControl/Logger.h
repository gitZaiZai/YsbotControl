 

#ifndef _LOGGER_
#define _LOGGER_

#include <iostream>   // needed for ostream (logging to output stream)
#include <fstream>    // needed for fstream (logging to file)
#include <string>     // needed for string
#include <iomanip>    // needed for setw
#include <set>        // needed for set

 
#include <windows.h>  // needed for DWORD and GetTickCount() function
#include <time.h>     // needed for time_t
 

using namespace std;

#define MAX_LOG_LINE 3072 /*!< maximum size of a log message */
#define MAX_HEADER   128  /*!< maximum size of the header    */

/*****************************************************************************/
/*********************** CLASS TIMING ****************************************/
/*****************************************************************************/

/*!This class holds a timer. This timer can be set (restartTime) and text can
   be printed with the elapsed time since the timer was restarted.. */
class Timing
{
 
  DWORD  time1;                  /*! < the number of milliseconds that have
                                       elapsed since the Windows was started */
 
  
public:
  // methods to restart the timer, get the elapsed time and print messages
 
  static double getTimeDifference    ( DWORD tv1, DWORD tv2                  );
 
  void          printTimeDiffWithText( ostream& os,
                                       char     *str,
                                       int      iFactor = 1000               );
  double        getElapsedTime       ( int      iFactor = 1                  );
  void          restartTime          (                                       );
} ;


/*****************************************************************************/
/**************************** LOGGER *****************************************/
/*****************************************************************************/

/*! This class makes it possible to log information on different abstraction
    levels. All messages are passed to the log method 'log' with a level
    indication. When it has been specified that this level should be logged
    using either the 'addLogLevel' or 'addLogRange' method
    the message is logged, otherwise it is ignored. This makes it
    possible to print only  the information you are interested in.
    There is one global Log class which is used by all classes that use the
    Logger. This instantiation of the Logger is located in the file Logger.C and
    is called 'Log'. All classes that want use this Logger should make a
    reference to it using the line 'extern Logger Log;' and can then use
    this Logger with the Log.log( ... ) methods. Furthermore the Logger also
    contains a timer with makes it possible to print the time since the timer
    has been restarted. */
class Logger
{
  Timing   m_timing;               /*!< timer to print timing information    */
  char     m_buf[MAX_LOG_LINE];    /*!< buffer needed by different methods   */
  set<int> m_setLogLevels;         /*!< set that contains all log levels     */

  char     m_strHeader[MAX_HEADER];/*!< header string printed before msg     */
  ostream* m_os;                   /*!< output stream to print messages to   */
  string   m_strSignal;            /*!< temporary string for other messages  */

public:
  Logger( ostream& os=cout, int iMinLogLevel=0, int iMaxLogLevel = 0);

  // different methods associated with logging messages
  bool     log              ( int         iLevel, string str              );
  bool     log              ( int         i,      char   *str, ...        );
  bool     logWithTime      ( int         iLevel, char   *str, ...        );
  bool     logFromSignal    ( int         iLevel, char   *str, ...        );
  bool     logSignal        (                                             );
  
  void     restartTimer     (                                             );
  Timing   getTiming        (                                             );
  bool     isInLogLevel     ( int         iLevel                          );

  bool     addLogLevel      ( int         iLevel                          );
  bool     addLogRange      ( int         iMin,   int iMax                );

  char*    getHeader        (                                             );
  bool     setHeader        ( char        *str                            );
  bool     setHeader        ( int         i                               );
  bool     setHeader        ( int         i1,     int    i2               );

  bool     setOutputStream  ( ostream&    os                              );
  ostream& getOutputStream  (                                             );
  void     showLogLevels    ( ostream&    os                              );
};


#endif
