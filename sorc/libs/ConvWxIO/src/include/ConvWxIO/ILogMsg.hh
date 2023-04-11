// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// © University Corporation for Atmospheric Research (UCAR) 2009-2010. 
// All rights reserved.  The Government's right to use this data and/or 
// software (the "Work") is restricted, per the terms of Cooperative 
// Agreement (ATM (AGS)-0753581 10/1/08) between UCAR and the National 
// Science Foundation, to a "nonexclusive, nontransferable, irrevocable, 
// royalty-free license to exercise or have exercised for or on behalf of 
// the U.S. throughout the world all the exclusive rights provided by 
// copyrights.  Such license, however, does not include the right to sell 
// copies or phonorecords of the copyrighted works to the public."   The 
// Work is provided "AS IS" and without warranty of any kind.  UCAR 
// EXPRESSLY DISCLAIMS ALL OTHER WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
// ANY IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
// PURPOSE.  
//  
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
#include <toolsa/copyright.h>

/**
 * @file ILogMsg.hh
 * @brief  macros for logging.
 * @class ILogMsg
 * @brief  macros for logging.
 *
 * @note The macros in this class hide the implementation (not anymore)
 *
 *
 */
#ifndef INTERFACE_LOG_HH
#define INTERFACE_LOG_HH

#include <vector>
#include <string>
#include <toolsa/LogStream.hh>

// //------------------------------------------------------------------
// class ILogMsg          
// {
// public:

//   typedef enum
//   {
//     FORCE=0,              /**< Force (must display this one no matter what) */
//     PRINT=1,              /**< Force,but with no extra header information */
//     DEBUG=2,              /**< Debugging */
//     DEBUG_VERBOSE=3,      /**< Verbose debugging */
//     TRIGGER=4,            /**< Triggering specific messages */
//     THREAD=5,             /**< Threading specific messages */
//     SPECIAL=6,            /**< Special (for any customized use) */
//     WARNING=7,            /**< Warning message */
//     ERROR=8,              /**< Error message */
//     SEVERE=9,             /**< Severe error message */
//     FATAL=10              /**< Fatal error */
//   } Severity_t;

//   static LogStream::Log_t convertType(Severity_t s);
//   static void print(Severity_t s, const std::string &msg);
//   static void printv(Severity_t s, const std::string &fmt, ...);
//   static void enable(Severity_t s);
//   static void disable(Severity_t s);
//   static void accum(Severity_t s);

// private:
// };

// #define ILOG_CONVERT_TYPE(s) ILogMsg::convertType(s)

/**
 * Log a fixed string message if the severity type is enabled.
 *
 * @param[in] severity  Severity level
 * @param[in] msg  The fixed message to log
 *
 * @note '\n' is appended to the logged output
 */

//#define ILOG(severity, msg) (LOGPRINT((ILOG_CONVERT_TYPE(severity)), (msg)))
#define ILOG(severity, msg) (LOGPRINT(severity, msg))
#define ILOGV(severity, msg) (LOGPRINTV(severity, msg))

/**
 * Log a formatted formatted message if the severity type is enabled
 *
 * @param[in] severity  Severity level
 * @param[in] format  The printf style format string to use.
 *
 * The remaining arguments are those that get formatted.
 *
 * @note '\n' is appended to the logged output
 */
#define ILOGF(severity, format, ...) (LOGPRINT(severity, format, __VA_ARGS__))
#define ILOGFV(severity, format, ...) (LOGPRINTV(severity, format, __VA_ARGS__))

/**
 * Initialize state for commonly variable items
 *
 * @param[in] debug  True to enable DEBUG, false to disable.
 * @param[in] debugVerbose  True to enable DEBUG_VERBOSE, false to disable.
 * @param[in] realtime  True for showing real time in logged message
 * @param[in] showFile  True for showing file/line/method in logged messages
 *
 */
#define ILOG_INIT(debug,debugVerbose,realtime,showFile) (LOG_STREAM_INIT((debug),(debugVerbose),(realtime),(showFile)))


#define ILOG_LOGFILE_INIT(app,instance,logpath) (LOG_STREAM_TO_LOGFILE((app),(instance),(logpath)))

/**
 * Disable a particular severity type
 *
 * @param[in] s  Severity type
 */
#define ILOG_DISABLE_SEVERITY(s) LOG_STREAM_DISABLE(s)

/**
 * Enable a particular severity type
 *
 * @param[in] s  Severity type
 */
#define ILOG_ENABLE_SEVERITY(s)  LOG_STREAM_ENABLE(s)

/**
 * Disable putting of real time stamps into logged messages
 *
 */
#define ILOG_DISABLE_TIME_STAMP() LOG_STREAM_DISABLE_TIME_STAMP()

/**
 * Log the accumulated information that was build up using ILOG_ACCUMULATE
 * or ILOG_ACCUMULATEF macros, if the severity level is enabled.
 *
 * @param[in] severity  Severity level
 *
 * All the accumulated information is cleared out by this macro
 */
#define ILOG_ACCUM(severity) (LOG_STREAM_ACCUM(severity))
#define ILOG_ACCUMV(severity) (LOG_STREAM_ACCUMV(severity))

/**
 * Store a string internally for later logging.
 *
 * Put in an empty string to get it to work
 *
 * @param[in] msg  Fixed message
 */
#define ILOG_ACCUMULATE(format) (LOG_STREAM_ACCUMULATE((format)))

/**
 * Store a formatted string internally for later logging.
 *
 * @param[in] format  The printf style format string
 *
 * The remaining args are those that get formatted to produce the stored string.
 */
#define ILOG_ACCUMULATEF(format, ...) (LOG_STREAM_ACCUMULATE((format),__VA_ARGS__))

/**
 * Free up memory used for logging
 */
#define ILOG_FINISH() (LOG_STREAM_FINISH())

// // #ifdef NOTDEF
// //   /**
// //    * Log a message, fixed format
// //    * @param[in] fname File name within source code
// //    * @param[in] line  Line number within source code
// //    * @param[in] method  Method name within source code
// //    * @param[in] t   The severity enumerated value
// //    * @param[in] msg  The message to log
// //    */
// //   static void ilog(const std::string &fname, const int line, 
// // 		   const std::string &method,
// // 		   Severity_t t, const std::string &msg);

// //   /**
// //    * Log a formatted message, with arguments
// //    * @param[in] fname  File name within source code
// //    * @param[in] line  Line number within source code
// //    * @param[in] method  Method name within source code
// //    * @param[in] t   The severity enumerated value
// //    * @param[in] format  The c++ format string
// //    * remaining arguments are expected as needed
// //    */
// //   static void ilogf(const std::string &fname, const int line, 
// // 		    const std::string &method,
// // 		    Severity_t t, const std::string &format, ...);

// //   /**
// //    * Initialize logging state
// //    * @param[in] debug  True to output DEBUG messages
// //    * @param[in] debugVerbose  True to output DEBUG_VERBOSE messages
// //    * @param[in] realtime  True to show the real time in logged messages
// //    * @param[in] showFile  True to put the file/line/method in logged messages
// //    */
// //   static void ilog_init(bool debug, bool debugVerbose, bool realtime,
// // 			bool showFile);

// //   /**
// //    * Initialize to log to logfiles
//    * @param[in] app  App name for logfile names
//    * @param[in] instance App instance for logfile names
//    * @param[in] logPath  Top directory for logfiles,if empty default is $LOG_DIR
//    */
//   static void ilog_logfile_init(const std::string &app,
// 				const std::string &instance,
// 				const std::string &logPath = "");

//   /**
//    * Disable a particular severity so message of that type are not logged
//    * @param[in] t  The severity
//    */
//   static void ilog_disable(Severity_t t);

//   /**
//    * Enable a particular severity so message of that type are logged
//    * @param[in] t  The severity
//    */
//   static void ilog_enable(Severity_t t);
  
//   /**
//    * Log the message that was accumulated by calls to ilog_accumulate and
//    * ilog_accumulatef
//    * @param[in] fname File name within source code
//    * @param[in] line  Line number within source code
//    * @param[in] method  Method name within source code
//    * @param[in] t   The severity enumerated value
//    */
//   static void ilog_accum(const std::string &fname, const int line, 
// 			 const std::string &method, Severity_t t);

//   // /**
//   //  * Store a string for later logging
//   //  * @param[in] msg  String to store
//   //  */
//   // static void ilog_accumulate(const std::string &msg);

//   // /**
//   //  * Store a formatted string for later logging
//   //  * @param[in] format  c++ format string
//   //  * Additional args as needed
//   //  */
//   // static void ilog_accumulatef(const std::string &format, ...);

//   /**
//    * Free up memory used for logging
//    */
//   static void ilog_destroy();

//   /**
//    * disable the writing of real time to logged messages
//    */
//   static void ilog_disable_time_stamp();
// #endif

//   // static LogStream::Log_t pSeverity(ILogMsg::Severity_t s);

// protected:
// private:  

// };

# endif
