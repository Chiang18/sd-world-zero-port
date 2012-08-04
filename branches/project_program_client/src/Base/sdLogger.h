//*************************************************************************************************
// 内容:	日志系统
//---------------------------------------------------------
// 作者:		
// 创建:		2012-07-16
// 最后修改:
//*************************************************************************************************
#pragma once
#ifndef _LOGGER_H__
#define _LOGGER_H__
#include "sdSingleton.h"
#include "sdCriticalSection.h"

namespace Base
{
	// 一个简单的单日志文件的日志系统
	class sdLogger : public sdTSingleton<sdLogger>
	{
	public:
		// 日志事件级别,用于过滤日志
		enum LoggingLevel
		{	
			E_ERROR	= 0,
			E_WARNNING,
			E_INFORMATIVE,
		};

		// 日志事件回调函数
		typedef void (*LogEventHandler)(const std::string& message, LoggingLevel level);

	public:
		sdLogger();
		virtual ~sdLogger();

		// 日志
		void	Log(const char* message, ...);
		void	LogWarning(const char* message, ...);
		void	LogError(const char* message, ...);

		// 调试字符串
		void	OutputDebugString(const char* message, ...);

		// 设置日志文件名
		void	SetLogFileName(const char* filename, bool append = false);

		// 设置日志事件回调函数
		LogEventHandler SetLogHandle(LogEventHandler pfn_logger);

	protected:
		void	LogEvent(LoggingLevel level, const char* message, ...);
		void	LogEvent(const std::string& message, LoggingLevel level = E_INFORMATIVE);

	protected:
		// 日志级别
		LoggingLevel		m_eLoggingLevel;

		// 日志数据流
		std::ofstream		m_kLogStream;		// 日志数据流
		std::ostringstream	m_kLogWorkStream;	// 日志格式化数据流		

		// 日志缓存
		typedef std::vector<std::pair<std::string, LoggingLevel> > LogCache;
		typedef std::vector<std::pair<std::string, LoggingLevel> >::iterator LogCacheItr;
		LogCache	m_kLogCache;	// 日志缓存
		bool		m_bLogCache;

		// 日志回调函数
		LogEventHandler		m_pfnLogHandler;

		// 临界区
		sdCriticalSection	m_kCriticalSection;	
	};

#define LOG_TEXT		Logger::Instance().Log
#define LOG_WARNING		Logger::Instance().LogWarning
#define Log_ERROR		Logger::Instance().LogError

}
#endif