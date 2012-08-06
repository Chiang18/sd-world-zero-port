#include "Stdafx.h"
#include "sdLogger.h"
#include "sdFileUtil.h"
#include "sdStrUtil.h"

// sdk
#include <strsafe.h>

namespace Base
{

// 内部临时缓存,用于格式化字符串之类的
static char buff[1024];

#define SD_CHECK_MSG(message)	\
	if (!message)				\
	{							\
		return;			\
	}

#define SD_GET_FMT_MSG				\
	{								\
		va_list args;				\
		va_start(args, message);	\
		::StringCchVPrintf(buff, 1024, message, args);	\
		buff[1023] = '\0';			\
		va_end(args);				\
	}

//-------------------------------------------------------------------------------------------------
sdLogger::sdLogger()
{
	m_eLoggingLevel = E_INFORMATIVE;
	m_bLogCache = true;

	std::string szName = sdFileUtil::GetModuleName();
	char szData[16] = { 0 };
	char szTime[16] = { 0 };
	_strdate_s(szData);
	_strtime_s(szTime);
	szData[2] = '-';
	szData[5] = '-';
	szTime[2] = '-';
	szTime[5] = '-';

	SetLogFileName(sdStrUtil::Format("log\\%s-%s-%s.log", szName.c_str(), szData, szTime).c_str());
}
//-------------------------------------------------------------------------------------------------
sdLogger::~sdLogger()
{
	if (m_kLogStream.is_open())
	{
		LogEvent("Logger Singleton Destroyed!");
		m_kLogStream.close();
	}
}
//-------------------------------------------------------------------------------------------------
void sdLogger::OutputDebugString(const char* message, ...)
{
	SD_GET_FMT_MSG(message);

	// 输出到调试控制台
	::OutputDebugString(buff);
}
//-------------------------------------------------------------------------------------------------
void sdLogger::Log(const char* message, ...)
{
	m_kCriticalSection.Lock();
	SD_CHECK_MSG(message);
	SD_GET_FMT_MSG(message);

	// 输出到调试控制台
	::OutputDebugString(buff);
	::OutputDebugString("\n");

	// 输出到日志
	LogEvent(buff, E_INFORMATIVE);

	m_kCriticalSection.Unlock();
}
//-------------------------------------------------------------------------------------------------
void sdLogger::LogWarning(const char* message, ...)
{
	m_kCriticalSection.Lock();
	SD_CHECK_MSG(message);
	SD_GET_FMT_MSG(message);

	// 输出到调试控制台
	::OutputDebugString(buff);
	::OutputDebugString("\n");

	// 输出到日志
	LogEvent(buff, E_WARNNING);

	m_kCriticalSection.Unlock();
}
//-------------------------------------------------------------------------------------------------
void sdLogger::LogError(const char* message, ...)
{
	m_kCriticalSection.Lock();
	SD_CHECK_MSG(message);
	SD_GET_FMT_MSG(message);

	// 输出到调试控制台
	::OutputDebugString(buff);
	::OutputDebugString("\n");

	// 输出到日志
	LogEvent(buff, E_ERROR);

	m_kCriticalSection.Unlock();
}
//-------------------------------------------------------------------------------------------------
void sdLogger::LogEvent(LoggingLevel level, const char* message, ...)
{
	// 内部函数,调用前已经加锁
	SD_CHECK_MSG(message);
	SD_GET_FMT_MSG(message);

	LogEvent(buff, level);
}
//-------------------------------------------------------------------------------------------------
void sdLogger::LogEvent(const std::string& message, LoggingLevel level /* = E_INFORMATIVE */)
{
	using namespace std;

	// 抛弃掉低级别日志
	if (level > m_eLoggingLevel)
		return;

	// 调用日志回调
	if (m_pfnLogHandler)
	{
		m_pfnLogHandler(message, level);
	}

	//
	time_t et;
	time(&et);
	tm* etm = localtime(&et);
	if (etm)
	{
		// 清除数据流
		m_kLogWorkStream.str("");

		// 写入日期
		m_kLogWorkStream << setfill('0') << setw(2) << etm->tm_mday << '/'
						 << setfill('0') << setw(2) << 1 + etm->tm_mon << '/'
						 << setw(4) << (1900 + etm + etm->tm_year) << ' ';

		// 写入时间
		m_kLogWorkStream << setfill('0') << setw(2) << etm->tm_hour << ':'
						 << setfill('0') << setw(2) << etm->tm_min  << ':'
						 << setfill('0') << setw(2) << etm->tm_sec  << ' ';

		// 写入事件类型
		switch(level)
		{
		case E_ERROR:
			m_kLogWorkStream << "(Error\t\t)";
			break;

		case E_WARNNING:
			m_kLogWorkStream << "(Warnning)\t";
			break;

		case E_INFORMATIVE:
			m_kLogWorkStream << "(Informative)";
			break;

		default:
			m_kLogWorkStream << "(Unkown)\t";
		}

		// 写入消息
		m_kLogWorkStream << message << endl;

		// 处理工作流
		if (m_bLogCache)
		{
			m_kLogCache.push_back(std::make_pair(m_kLogWorkStream.str(), level));
		}
		else
		{
			m_kLogStream << m_kLogWorkStream.str();
			m_kLogStream.flush();
		}
	}
}
//-------------------------------------------------------------------------------------------------
void sdLogger::SetLogFileName(const char* filename, bool append)
{
	using namespace std;

	// 设置能够访问中文路径
	std::locale loc = std::locale::global(std::locale(""));

	// 创建日志目录
	string full_filename = sdFileUtil::GetModulePath() + "\\" + filename;
	sdFileUtil::ConfimDir(sdFileUtil::ExtractPath(full_filename));

	//
	if (m_kLogStream.is_open())
		m_kLogStream.close();

	//
	if (sdFileUtil::IsFileExist(full_filename))
		sdFileUtil::RemoveReadOnly(full_filename.c_str());

	//
	m_kLogStream.open(
		full_filename.c_str(),
		std::ios_base::out | (append ? std::ios_base::app : std::ios_base::trunc));
	if (!m_kLogStream.is_open())
	{
		OutputDebugString("Logger::SetLogFileName - Failed to open file. %s\n", full_filename);
		return;
	}

	if (m_bLogCache)
	{
		m_bLogCache = false;

		LogCacheItr itr = m_kLogCache.begin();
		LogCacheItr itr_end = m_kLogCache.end();
		for (; itr!=itr_end; ++itr)
		{
			if (itr->second <= m_eLoggingLevel)
			{
				m_kLogStream << itr->first;
				m_kLogStream.flush();
			}
		}
		m_kLogCache.clear();
	}

	// 恢复设置
	std::locale::global(loc);
}
//-------------------------------------------------------------------------------------------------
}