//*************************************************************************************************
// 内容:	Win32 File的一些功能
//---------------------------------------------------------
// 作者:		
// 创建:		2012-07-13
// 最后修改:
//*************************************************************************************************
#pragma once
#ifndef _SD_BASE_FILE_UTIL_H__
#define _SD_BASE_FILE_UTIL_H__
#include "sdBase.h"

namespace Base
{

	// 用来处理文件目录路径等一些与操作系统相关的功能
	class sdFileUtil
	{
	public:
		// 判断文件是否存在
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

		// 判断目录是否存在
		static bool IsDirExist(const std::string& szDirName)
		{
			DWORD uiAttribute = ::GetFileAttributes(szDirName.c_str());
			return (uiAttribute != -1) && (uiAttribute & FILE_ATTRIBUTE_DIRECTORY);
		}

		// 确定目录是否存在,不存在则创建之
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

		// 文件只读属性
		static void RemoveReadOnly(const char* szFileFullPath)
		{
			DWORD uiAttribute = ::GetFileAttributes(szFileFullPath);
			if ((uiAttribute != -1) && (uiAttribute & FILE_ATTRIBUTE_READONLY))
			{
				DWORD uiWriteableAttribute = uiAttribute & ~FILE_ATTRIBUTE_READONLY;
				::SetFileAttributes(szFileFullPath, uiWriteableAttribute);
			}
		}

		// 确认路径的最后以\结束,不存在则添加之
		static void ConfimSlash(std::string& szPath)
		{
			 if (!szPath.empty())
			 {
				if (szPath[szPath.length() - 1] != '\\')
					szPath += '\\';
			 }
		}

		// 将路径转换为DOS样式的左斜杠
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

		// 从绝对路径文件名中解析路径(末尾不带斜杠)
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

		// 从绝对路径文件名中解析文件名
		static std::string ExtractName(const std::string& szPathFileName)
		{
			char szDosName[MAX_PATH + 1];
			ConvertToDOSName(szDosName, szPathFileName.c_str());
			return szPathFileName.substr(std::string(szDosName).rfind("\\") + 1);
		};

		// 取当前exe文件路径(末尾不带斜杠)
		static std::string GetModulePath()
		{
			char szPath[MAX_PATH + 1];
			::GetModuleFileName(NULL, szPath, MAX_PATH);
			return ExtractPath(szPath);
		};

		// 取当前exe/dll文件名(不带路径)
		static std::string GetModuleName()
		{
			char szPath[MAX_PATH + 1];
			::GetModuleFileName(NULL, szPath, MAX_PATH);
			return ExtractName(szPath);
		};

	};
}
#endif