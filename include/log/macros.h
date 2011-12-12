#ifndef LOG_H_INCLUDED
#define LOG_H_INCLUDED

#include "log/log.h"

#ifdef DEBUG
    #define LOG_DEBUG(msg) FILE_LOG(logDEBUG) << __FILE__ << ":" << __LINE__ \
        << " " << __FUNCTION__ << "() : " << msg
    #define LOG_DEBUG1(msg) FILE_LOG(logDEBUG1) << __FILE__ << ":" << __LINE__ \
        << " " << __FUNCTION__ << "() : " << msg
    #define LOG_DEBUG2(msg) FILE_LOG(logDEBUG2) << __FILE__ << ":" << __LINE__ \
        << " " << __FUNCTION__ << "() : " << msg
#else
    #define LOG_DEBUG(msg)
    #define LOG_DEBUG1(msg)
    #define LOG_DEBUG2(msg)
#endif

#define LOG_INFO(msg) FILE_LOG(logINFO) << msg
#define LOG_WARN(msg) FILE_LOG(logWARNING) << msg
#define LOG_ERROR(msg) FILE_LOG(logERROR) << msg

#define IS_LOG_DEBUG logDEBUG <= FILELog::ReportingLevel()
#define IS_LOG_DEBUG1 logDEBUG1 <= FILELog::ReportingLevel()
#define IS_LOG_DEBUG2 logDEBUG2 <= FILELog::ReportingLevel()
#define IS_LOG_INFO logINFO <= FILELog::ReportingLevel()
#define IS_LOG_WARN logWARNING <= FILELog::ReportingLevel()
#define IS_LOG_ERROR logERROR <= FILELog::ReportingLevel()

#endif // LOG_H_INCLUDED
