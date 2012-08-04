#include "Stdafx.h"
#include "sdLogger.h"
#include "sdFileUtil.h"
#include "sdStrUtil.h"

// sdk
#include <strsafe.h>

namespace Base
{

// �ڲ���ʱ����,���ڸ�ʽ���ַ���֮���
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

	// ��������Կ���̨
	::OutputDebugString(buff);
}
//-------------------------------------------------------------------------------------------------
void sdLogger::Log(const char* message, ...)
{
	m_kCriticalSection.Lock();
	SD_CHECK_MSG(message);
	SD_GET_FMT_MSG(message);

	// ��������Կ���̨
	::OutputDebugString(buff);
	::OutputDebugString("\n");

	// �������־
	LogEvent(buff, E_INFORMATIVE);

	m_kCriticalSection.Unlock();
}
//-------------------------------------------------------------------------------------------------
void sdLogger::LogWarning(const char* message, ...)
{
	m_kCriticalSection.Lock();
	SD_CHECK_MSG(message);
	SD_GET_FMT_MSG(message);

	// ��������Կ���̨
	::OutputDebugString(buff);
	::OutputDebugString("\n");

	// �������־
	LogEvent(buff, E_WARNNING);

	m_kCriticalSection.Unlock();
}
//-------------------------------------------------------------------------------------------------
void sdLogger::LogError(const char* message, ...)
{
	m_kCriticalSection.Lock();
	SD_CHECK_MSG(message);
	SD_GET_FMT_MSG(message);

	// ��������Կ���̨
	::OutputDebugString(buff);
	::OutputDebugString("\n");

	// �������־
	LogEvent(buff, E_ERROR);

	m_kCriticalSection.Unlock();
}
//-------------------------------------------------------------------------------------------------
void sdLogger::LogEvent(LoggingLevel level, const char* message, ...)
{
	// �ڲ�����,����ǰ�Ѿ�����
	SD_CHECK_MSG(message);
	SD_GET_FMT_MSG(message);

	LogEvent(buff, level);
}
//-------------------------------------------------------------------------------------------------
void sdLogger::LogEvent(const std::string& message, LoggingLevel level /* = E_INFORMATIVE */)
{
	using namespace std;

	// �������ͼ�����־
	if (level > m_eLoggingLevel)
		return;

	// ������־�ص�
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
		// ���������
		m_kLogWorkStream.str("");

		// д������
		m_kLogWorkStream << setfill('0') << setw(2) << etm->tm_mday << '/'
						 << setfill('0') << setw(2) << 1 + etm->tm_mon << '/'
						 << setw(4) << (1900 + etm + etm->tm_year) << ' ';

		// д��ʱ��
		m_kLogWorkStream << setfill('0') << setw(2) << etm->tm_hour << ':'
						 << setfill('0') << setw(2) << etm->tm_min  << ':'
						 << setfill('0') << setw(2) << etm->tm_sec  << ' ';

		// д���¼�����
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

		// д����Ϣ
		m_kLogWorkStream << message << endl;

		// ��������
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

	// �����ܹ���������·��
	std::locale loc = std::locale::global(std::locale(""));

	// ������־Ŀ¼
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

	// �ָ�����
	std::locale::global(loc);
}
//-------------------------------------------------------------------------------------------------
}