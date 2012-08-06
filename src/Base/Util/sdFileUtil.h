//*************************************************************************************************
// ����:	Win32 File��һЩ����
//---------------------------------------------------------
// ����:		
// ����:		2012-07-13
// ����޸�:
//*************************************************************************************************
#pragma once
#ifndef _SD_BASE_FILE_UTIL_H__
#define _SD_BASE_FILE_UTIL_H__
#include "sdBase.h"

namespace Base
{

	// ���������ļ�Ŀ¼·����һЩ�����ϵͳ��صĹ���
	class sdFileUtil
	{
	public:
		// �ж��ļ��Ƿ����
		static bool IsFileExist(const std::string& szFileName)
		{
			WIN32_FIND_DATA kFindData = { 0 };
			HANDLE hFile = ::FindFirstFile(szFileName.c_str(), &kFindData);
			if (hFile != INVALID_HANDLE_VALUE && kFindData.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY)
			{
				::FindClose(hFile);
				return true;
			}

			return false;
		}

		// �ж�Ŀ¼�Ƿ����
		static bool IsDirExist(const std::string& szDirName)
		{
			DWORD uiAttribute = ::GetFileAttributes(szDirName.c_str());
			return (uiAttribute != -1) && (uiAttribute & FILE_ATTRIBUTE_DIRECTORY);
		}

		// ȷ��Ŀ¼�Ƿ����,�������򴴽�֮
		static void ConfimDir(const std::string& szDir)
		{
			using namespace std;

			char szDOSName[MAX_PATH];
			ConvertToDOSName(szDOSName, szDir.c_str());

			string szFullPath(szDOSName);
			ConfimSlash(szFullPath);

			if (IsDirExist(szFullPath))
				return;

			string::size_type stPos = szFullPath.find("\\");
			if (stPos == string::npos) return;
			for (;;)
			{
				stPos = szFullPath.find("\\");
				if (stPos == string::npos) 
					break;

				::CreateDirectory(szFullPath.substr(0, stPos).c_str(), NULL);
				if (stPos == szFullPath.size())
					break;
			}
		}

		// �ļ�ֻ������
		static void RemoveReadOnly(const char* szFileFullPath)
		{
			DWORD uiAttribute = ::GetFileAttributes(szFileFullPath);
			if ((uiAttribute != -1) && (uiAttribute & FILE_ATTRIBUTE_READONLY))
			{
				DWORD uiWriteableAttribute = uiAttribute & ~FILE_ATTRIBUTE_READONLY;
				::SetFileAttributes(szFileFullPath, uiWriteableAttribute);
			}
		}

		// ȷ��·���������\����,�����������֮
		static void ConfimSlash(std::string& szPath)
		{
			 if (!szPath.empty())
			 {
				if (szPath[szPath.length() - 1] != '\\')
					szPath += '\\';
			 }
		}

		// ��·��ת��ΪDOS��ʽ����б��
		static void ConvertToDOSName(char* pszDst, const char* pszSrc)
		{
			while (*pszSrc)
			{
				if (*pszSrc == '/')
					*pszDst = '\\';
				else
					*pszDst = *pszSrc;

				++pszSrc;
				++pszDst;
			}
			*pszDst = 0;
		}

		// �Ӿ���·���ļ����н���·��(ĩβ����б��)
		static std::string ExtractPath(const std::string& szPathFileName)
		{
			int iNum = (int)szPathFileName.length() - 1;
			while (iNum > 0)
			{
				if (szPathFileName[iNum] == '\\' || szPathFileName[iNum] == '/')
					return szPathFileName.substr(0, iNum);
				
				--iNum;
			}

			return "";
		};

		// �Ӿ���·���ļ����н����ļ���
		static std::string ExtractName(const std::string& szPathFileName)
		{
			char szDosName[MAX_PATH + 1];
			ConvertToDOSName(szDosName, szPathFileName.c_str());
			return szPathFileName.substr(std::string(szDosName).rfind("\\") + 1);
		};

		// ȡ��ǰexe�ļ�·��(ĩβ����б��)
		static std::string GetModulePath()
		{
			char szPath[MAX_PATH + 1];
			::GetModuleFileName(NULL, szPath, MAX_PATH);
			return ExtractPath(szPath);
		};

		// ȡ��ǰexe/dll�ļ���(����·��)
		static std::string GetModuleName()
		{
			char szPath[MAX_PATH + 1];
			::GetModuleFileName(NULL, szPath, MAX_PATH);
			return ExtractName(szPath);
		};

	};
}
#endif