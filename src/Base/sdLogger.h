//*************************************************************************************************
// ����:	��־ϵͳ
//---------------------------------------------------------
// ����:		
// ����:		2012-07-16
// ����޸�:
//*************************************************************************************************
#pragma once
#ifndef _LOGGER_H__
#define _LOGGER_H__
#include "sdSingleton.h"
#include "sdCriticalSection.h"

namespace Base
{
	// һ���򵥵ĵ���־�ļ�����־ϵͳ
	class sdLogger : public sdTSingleton<sdLogger>
	{
	public:
		// ��־�¼�����,���ڹ�����־
		enum LoggingLevel
		{	
			E_ERROR	= 0,
			E_WARNNING,
			E_INFORMATIVE,
		};

		// ��־�¼��ص�����
		typedef void (*LogEventHandler)(const std::string& message, LoggingLevel level);

	public:
		sdLogger();
		virtual ~sdLogger();

		// ��־
		void	Log(const char* message, ...);
		void	LogWarning(const char* message, ...);
		void	LogError(const char* message, ...);

		// �����ַ���
		void	OutputDebugString(const char* message, ...);

		// ������־�ļ���
		void	SetLogFileName(const char* filename, bool append = false);

		// ������־�¼��ص�����
		LogEventHandler SetLogHandle(LogEventHandler pfn_logger);

	protected:
		void	LogEvent(LoggingLevel level, const char* message, ...);
		void	LogEvent(const std::string& message, LoggingLevel level = E_INFORMATIVE);

	protected:
		// ��־����
		LoggingLevel		m_eLoggingLevel;

		// ��־������
		std::ofstream		m_kLogStream;		// ��־������
		std::ostringstream	m_kLogWorkStream;	// ��־��ʽ��������		

		// ��־����
		typedef std::vector<std::pair<std::string, LoggingLevel> > LogCache;
		typedef std::vector<std::pair<std::string, LoggingLevel> >::iterator LogCacheItr;
		LogCache	m_kLogCache;	// ��־����
		bool		m_bLogCache;

		// ��־�ص�����
		LogEventHandler		m_pfnLogHandler;

		// �ٽ���
		sdCriticalSection	m_kCriticalSection;	
	};

#define LOG_TEXT		Logger::Instance().Log
#define LOG_WARNING		Logger::Instance().LogWarning
#define Log_ERROR		Logger::Instance().LogError

}
#endif