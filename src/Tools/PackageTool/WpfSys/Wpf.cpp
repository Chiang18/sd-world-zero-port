#include "StdAfx.h"
#include "Wpf.h"
#include <algorithm>
#include <stdio.h>
#include  <io.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <ShlObj.h>
#include <vector>
//#include "Compr.h"


#pragma comment(lib,"User32.lib")
#pragma comment(lib,"shell32.lib")

#ifdef _DEBUG
//#pragma comment(lib,"CompressMTd.lib")
#else
#pragma comment(lib,"CompressMT.lib")
#endif


//ms-help://MS.VSCC.v80/MS.MSDN.v80/MS.VisualStudio.v80.chs/dv_vccrt/html/dc7874d3-a91b-456a-9015-4748bb358217.htm

#pragma warning(disable:4996)

DWORD CWpf::m_dwCryptTable[MAX_WPF_HASH_BUFFF_SIZE];

CWpf::CWpf(void)
{
	m_dwExpendLen = 10 * 1024 *1024;
	m_bSaveWhenExpended = true;
	m_bAlertError = false;
	m_bSaving = false;
	m_bSpeedPrior = true;
	m_bClearEmptyDirAndFile = false;
	m_dwAddAttributeToNewFile = 0;
	m_bMeargeNotNeedUnCopress = false;
	m_pBlankBlockList = NULL;
	for (int i = 0 ; i < EFT_NUM; i ++)
	{
		m_iFileHandle[i] = -1;
		m_pOpenFileFcb1[i] = NULL;
		m_iOpenFilePointPos[i] = NULL;
	}

	m_eWrLastError = EWR_UNKNOWN;
	m_bModifyed = false;
	m_iWpfStartPos = 0;
	m_dwOpenType = EWOT_READONLY;
	m_pFcb1List = NULL;
	m_pFcb2List = NULL;
	m_pFcbList = NULL;
	m_pWpfRoot = NULL;
	m_pDirList = NULL;
	m_CreateDirHeader.pList = NULL;
	m_CreateDirHeader.pNext = NULL;
	m_CreateFcbHeader.pfcb = NULL;
	m_CreateFcbHeader.pNext = NULL;

	m_bBuildTempFileHashEnabled = false;

	InitBuffer();
}

CWpf::~CWpf(void)
{
	WpfClose();
}

bool CWpf::WpfCreate(const char * strPathName,__int64 iSize,bool bFailExist)
{
	if(strPathName == NULL || strlen(strPathName) == 0 || iSize <= 0)
	{
		m_eWrLastError = EWR_PARA_ERROR;
		return false;
	}

	if(m_bSaving)
	{
		m_eWrLastError = EWR_FILE_SAVING;
		return false;
	}

	FILE *fp = fopen(strPathName,"rb");
	if(fp)
	{
		fclose(fp);
		if(bFailExist)
		{
			m_eWrLastError = EWR_FILE_EXIST;
			return false;
		}
	}	
	//创建目录
	m_strWpfDir = strPathName;
	replace(m_strWpfDir.begin(),m_strWpfDir.end(),'/','\\');
	size_t iPos = m_strWpfDir.find_last_of("\\");
	if(iPos != string::npos)
	{
		m_strWpfFileName = m_strWpfDir.substr(iPos + 1,m_strWpfDir.length() - iPos - 1);
		m_strWpfDir = m_strWpfDir.substr(0,iPos);
	}

	SHCreateDirectoryEx(NULL,m_strWpfDir.c_str(),NULL);
 
	//先关闭当前打开的wpf文件
	WpfClose();

	//打开或创建文件
	//m_handle = CreateFile(strPathName,GENERIC_WRITE,FILE_SHARE_READ,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
	errno_t err = _sopen_s(&(m_iFileHandle[EFT_MAIN]), strPathName, _O_BINARY | _O_RDWR | _O_CREAT | _O_TRUNC , _SH_DENYWR,  _S_IREAD | _S_IWRITE );
	if(err != 0)
	{
		m_eWrLastError = EWR_ERROR;
		return false;
	}

	//初始化文件头
	m_iWpfStartPos = 0;


	//凑成m_WpfHeader.wBytesPerBlock的倍数
	if((iSize % m_WpfHeader.wBytesPerBlock) != 0)
	{
		iSize += m_WpfHeader.wBytesPerBlock - (iSize % m_WpfHeader.wBytesPerBlock);
	}

	m_WpfHeader.iWpfSize = iSize;
	m_WpfHeader.iBlankBlockPos = m_WpfHeader.wHeaderSize + iSize;
	m_WpfHeader.dwTotalBlocks = DWORD(iSize/m_WpfHeader.wBytesPerBlock);
	m_WpfHeader.dwBlankBlockSize = 1;
	m_WpfHeader.iFatPos = m_WpfHeader.iBlankBlockPos + m_WpfHeader.dwBlankBlockSize * sizeof(BlankBlock);

	m_pBlankBlockList = new BlankBlockNode();
	m_pBlankBlockList->blankBlock.dwStart = 0;
	m_pBlankBlockList->blankBlock.dwSize = m_WpfHeader.dwTotalBlocks;

	//指定文件大小
	err = _chsize_s(m_iFileHandle[EFT_MAIN],m_iWpfStartPos + m_WpfHeader.wHeaderSize + m_WpfHeader.iWpfSize + m_WpfHeader.dwBlankBlockSize * sizeof(BlankBlock) + (m_WpfHeader.dwDirCount + m_WpfHeader.dwFileCount) * FCB_SIZE);
	if(err != 0)
	{
		m_eWrLastError = EWR_ERROR;
		return false;
	}

	for (int i = EFT_THREAD ; i < EFT_NUM; i ++)
	{
		err = 	_sopen_s(&(m_iFileHandle[i]), strPathName, _O_BINARY | _O_RDONLY , _SH_DENYNO,  _S_IREAD);

		if(err != 0)
		{
			m_eWrLastError = EWR_FILE_OPEN_FAIL;
			return false;
		}
	}

	m_dwOpenType = EWOT_RDWR|EWOT_LISTDIR;

	//格式化
	WpfFormat();

	m_WpfHeader_Bak = m_WpfHeader;//备份头


	//把相关信息保存到文件
	m_bModifyed = true;
	if(!WpfSave())
	{
		m_bModifyed = false;
		WpfClose();

		::DeleteFile(strPathName);


		m_eWrLastError = EWR_FILE_SAVE_FAIL;
		return false;
	}

	return true;
}

bool CWpf::WpfOpen(const char * strPathName,__int64 iOffset,DWORD dwOpenType)
{
	if(strPathName == NULL || dwOpenType == 0)
	{
		m_eWrLastError = EWR_PARA_ERROR;
		return false;
	}

	if(m_bSaving)
	{
		m_eWrLastError = EWR_FILE_SAVING;
		return false;
	}

	//先关闭当前打开的wpf文件
	WpfClose();

	m_strWpfDir = strPathName;
	replace(m_strWpfDir.begin(),m_strWpfDir.end(),'/','\\');
	size_t iPos = m_strWpfDir.find_last_of("\\");
	if (iPos != string::npos)
	{
		m_strWpfFileName = m_strWpfDir.substr(iPos + 1,m_strWpfDir.length() - iPos - 1);
		m_strWpfDir = m_strWpfDir.substr(0,iPos);
	}
	//打开或创建文件
	errno_t err = 0;
	if (dwOpenType & EWOT_RDWR)
	{
		err = 	_sopen_s(&(m_iFileHandle[EFT_MAIN]), strPathName, _O_BINARY | _O_RDWR , _SH_DENYWR,  _S_IREAD | _S_IWRITE );
		dwOpenType |= EWOT_LISTDIR;
	}
	else
	{
		err = 	_sopen_s(&(m_iFileHandle[EFT_MAIN]), strPathName, _O_BINARY | _O_RDONLY , /*_SH_DENYWR*/_SH_DENYNO,  _S_IREAD);
		if (dwOpenType & EWOT_READONLY)
		{
			dwOpenType |= EWOT_LISTHASH;
		}
	}

	if(err != 0)
	{
		m_eWrLastError = EWR_FILE_OPEN_FAIL;
		return false;
	}

	for (int i = EFT_THREAD ; i < EFT_NUM; i ++)
	{
		err = 	_sopen_s(&(m_iFileHandle[i]), strPathName, _O_BINARY | _O_RDONLY , _SH_DENYNO,  _S_IREAD);

		if(err != 0)
		{
			m_eWrLastError = EWR_FILE_OPEN_FAIL;
			return false;
		}
	}

	m_iWpfStartPos = iOffset;
	m_WpfHeader.dwMagic = 0;//防止有些文件长度不够sizeof(S_WpfHeader),取到默认值
	m_WpfHeader_Bak.dwMagic = 0;//防止有些文件长度不够sizeof(S_WpfHeader),取到默认值
	m_WpfHeader.bySavingHeader = TRUE;//防止有些文件长度不够sizeof(S_WpfHeader),取到默认值
	m_WpfHeader_Bak.bySavingHeader = TRUE;//防止有些文件长度不够sizeof(S_WpfHeader),取到默认值

	//读入文件头
	_lseeki64(m_iFileHandle[EFT_MAIN],m_iWpfStartPos,SEEK_SET);
	_read(m_iFileHandle[EFT_MAIN],&m_WpfHeader,sizeof(S_WpfHeader));

	//读入文件头备份
	_lseeki64(m_iFileHandle[EFT_MAIN],m_iWpfStartPos + sizeof(S_WpfHeader),SEEK_SET);
	_read(m_iFileHandle[EFT_MAIN],&m_WpfHeader_Bak,sizeof(S_WpfHeader));

	if(m_WpfHeader.bySavingHeader && m_WpfHeader_Bak.bySavingHeader)//该文件在创建的时候出现异常,如断点,程序崩溃等情况,没有正常写入头
	{
		return false;
	}
	
	//如果上次文件头没有正确保存,读取必份的文件
	if (m_WpfHeader.bySavingHeader)
	{
		m_WpfHeader = m_WpfHeader_Bak;
		m_bModifyed = true;//原来没有保存好,用了备份的,那么这次即使没有修改也要重新保存
	}

	m_WpfHeader_Disk = m_WpfHeader;

	if(m_WpfHeader.dwMagic != WPF_MAGIC)
	{
		m_eWrLastError = EWR_NOT_WPF_FILE;
		WpfClose();
		return false;
	}

	m_WpfHeader.bySavingHeader = FALSE;//理论上不可能出现文件头及备份文件头m_WpfHeader.bySavingHeader同时为TRUE,还是清一下标记
	m_dwOpenType = dwOpenType;

	//如果以写方式打开要求读入空闲块描述信息
	if ((m_dwOpenType & EWOT_RDWR) && m_WpfHeader.dwBlankBlockSize > 0)
	{
		PBlankBlock pBlock = new BlankBlock[m_WpfHeader.dwBlankBlockSize];
		_lseeki64(m_iFileHandle[EFT_MAIN],m_iWpfStartPos + m_WpfHeader.iBlankBlockPos,SEEK_SET);
		_read(m_iFileHandle[EFT_MAIN],pBlock,m_WpfHeader.dwBlankBlockSize * sizeof(BlankBlock));

		PBlankBlock p = pBlock;
		PBlankBlockNode pList = NULL,pList2 = NULL;

		pList = new BlankBlockNode();
		pList->blankBlock.dwStart = p->dwStart;
		pList->blankBlock.dwSize = p->dwSize;
		pList->pParent = NULL;
		pList->pNext = NULL;
		m_pBlankBlockList = pList;
		pList2 = pList;
		p ++;

		for (DWORD i = 1; i < m_WpfHeader.dwBlankBlockSize; i++,p++)
		{
			pList = new BlankBlockNode();
			pList->blankBlock.dwStart = p->dwStart;
			pList->blankBlock.dwSize = p->dwSize;
			pList->pParent = pList2;
			pList2->pNext = pList;
			pList2 = pList;
		}

		pList2->pNext = NULL;

		SAFE_DELETE_ARRAY(pBlock);
	}

	//载入文件分配表
	return WpfLoadFat();	
}

void CWpf::WpfClear()
{
	PBlankBlockNode pBlankBlockList = m_pBlankBlockList;
	while(m_pBlankBlockList)
	{
		pBlankBlockList = m_pBlankBlockList->pNext;
		SAFE_DELETE(m_pBlankBlockList);
		m_pBlankBlockList = pBlankBlockList;
	}

	if (m_bSpeedPrior)
	{
		SAFE_DELETE_ARRAY(m_pFcb1List);
		SAFE_DELETE_ARRAY(m_pFcb2List);
		SAFE_DELETE_ARRAY(m_pFcbList);
		SAFE_DELETE_ARRAY(m_pDirList);
		//清除新建目录及文件时申请的内存
		NEWDIR *p = m_CreateDirHeader.pNext,*p2 = NULL;
		while(p)
		{
			p2 = p->pNext;
			SAFE_DELETE(p->pList);
			SAFE_DELETE(p);
			p = p2;
		}

		m_pWpfRoot = NULL;
		//文件FCB
		NEWFCB *q = m_CreateFcbHeader.pNext,*q2 = NULL;
		while(q)
		{
			q2 = q->pNext;
			SAFE_DELETE(q->pfcb->pfcb1);
			SAFE_DELETE(q->pfcb->pfcb2);
			SAFE_DELETE(q->pfcb);
			SAFE_DELETE(q);
			q = q2;
		}
	}
	else
	{
		//释放目录结构
		vector<PWpfFileList> VDirs;
		VDirs.push_back(m_pWpfRoot);

		PWpfFileList pFront = NULL;
		PFCB pfcb = NULL;

		while(!VDirs.empty())
		{
			pFront = VDirs.front();
			if (!pFront)
			{
				VDirs.erase(VDirs.begin());
				continue;
			}

			MList &dirlist = pFront->MDirs;
			MSFCB &filelist = pFront->MFiles;

			//子目录
			for (MList::iterator itr = dirlist.begin();itr != dirlist.end(); itr++)
			{
				VDirs.push_back(itr->second);
			}

			//释放文件结构
			for(MSFCB::iterator itr = filelist.begin(); itr != filelist.end(); itr++)
			{
				pfcb = itr->second;
				SAFE_DELETE(pfcb->pfcb1);
				SAFE_DELETE(pfcb->pfcb2);
				SAFE_DELETE(pfcb);
			}

			SAFE_DELETE(pFront->pfcb->pfcb1);
			SAFE_DELETE(pFront->pfcb->pfcb2);
			SAFE_DELETE(pFront->pfcb);
			SAFE_DELETE(pFront);

			//VDirs.pop_back();
			VDirs.erase(VDirs.begin());
		}
	}


	m_MhashFcb1.clear();
	m_MhashFcb.clear();
	m_CreateDirHeader.pList = NULL;
	m_CreateDirHeader.pNext = NULL;
	m_CreateFcbHeader.pfcb = NULL;
	m_CreateFcbHeader.pNext = NULL;

	for (int i = 0 ; i < EFT_NUM; i ++)
	{
		m_pOpenFileFcb1[i] = NULL;
		m_iOpenFilePointPos[i] = 0;
	}

	m_bClearEmptyDirAndFile = false;
	m_dwAddAttributeToNewFile = 0;
	m_bMeargeNotNeedUnCopress = false;

}

void CWpf::WpfClose()
{
	if(m_bSaving)
	{
		m_eWrLastError = EWR_FILE_SAVING;
		return;
	}

	WpfSave();

	WpfClear();

	m_WpfHeader.Clear();
	m_WpfHeader_Disk.Clear();
	m_WpfHeader_Bak.Clear();
	//关闭文件
	for (int i = 0 ; i < EFT_NUM; i ++)
	{
		if(m_iFileHandle[i] != -1)
		{
			_close(m_iFileHandle[i]);
			m_iFileHandle[i] = -1;
		}
	}

	m_iWpfStartPos = 0;
	m_bModifyed = false;
}

E_WPF_RTN CWpf::GetLastError()
{
	return m_eWrLastError;
}

PWpfFileList CWpf::GetRoot()
{
	return m_pWpfRoot;
}

WpfHeader* CWpf::GetHeader()
{
	if (!IsOpenWpfFile())
	{
		m_eWrLastError = EWR_FILE_NOT_OPEN;
		return NULL;
	}

	return &m_WpfHeader;
}


const char* CWpf::GetWpfFileDir()
{
	return m_strWpfDir.c_str();	
}

const char* CWpf::GetWpfFileName()
{
	return m_strWpfFileName.c_str();	
}

bool CWpf::SetExpendLength(DWORD dwLen)
{
	if (dwLen <= 0)
	{
		return false;
	}

	m_dwExpendLen = dwLen;

	return true;
}

void CWpf::SetSaveWhenExpended(bool bSave)
{
	m_bSaveWhenExpended = bSave;
}

void CWpf::SetAlertError(bool bAlert)
{
	m_bAlertError = bAlert;
}

bool CWpf::SetWpfType(DWORD dwType)
{
	if(!IsOpenWpfFile() || ((m_dwOpenType & EWOT_RDWR) == 0))
	{
		m_eWrLastError = EWR_FILE_NOT_OPEN;
		return false;
	}

	if(m_bSaving)
	{
		m_eWrLastError = EWR_FILE_SAVING;
		return false;
	}
	
	m_WpfHeader.dwWpfType = dwType;
	m_bModifyed = true;
	return true;
}

DWORD CWpf::GetWpfType()
{
	if (!IsOpenWpfFile())
	{
		m_eWrLastError = EWR_FILE_NOT_OPEN;
		return -1;
	}

	return m_WpfHeader.dwWpfType;
}

void CWpf::SetSpeedPrior(bool bSpeedPrior)
{
	m_bSpeedPrior = bSpeedPrior;
}

bool CWpf::IsSpeedPrior()
{
	return m_bSpeedPrior;
}

void CWpf::SetClearEmptyDirAndFile(bool bClearEmptyDirAndFile)
{
	m_bClearEmptyDirAndFile = bClearEmptyDirAndFile;
}

bool CWpf::IsClearEmptyDirAndFile()
{
	return m_bClearEmptyDirAndFile;
}

void CWpf::SetAddAttributeToNewFile(DWORD dwAddAttribute)
{
	m_dwAddAttributeToNewFile = dwAddAttribute;
}

DWORD CWpf::GetAddAttributeToNewFile()
{
	return m_dwAddAttributeToNewFile;
}

void CWpf::SetMeargeNotNeedUnCopress(bool val)
{
	m_bMeargeNotNeedUnCopress = val;
}

bool CWpf::GetMeargeNotNeedUnCopress()
{
	return m_bMeargeNotNeedUnCopress;
}

PWpfFileList CWpf::CreateDir(const char *strPathName)
{
	if(strPathName == NULL)
	{
		m_eWrLastError = EWR_PARA_ERROR;
		return NULL;
	}

	if(!IsOpenWpfFile() || ((m_dwOpenType & EWOT_RDWR) == 0))
	{
		m_eWrLastError = EWR_FILE_NOT_OPEN;
		return NULL;
	}

	if(m_bSaving)
	{
		m_eWrLastError = EWR_FILE_SAVING;
		return NULL;
	}

	string strPath = strPathName;
	replace(strPath.begin(),strPath.end(),'/','\\');
	//去掉后面连续的"\\"
	size_t iPos2 = strPath.find_last_not_of('\\');
	if(iPos2 == string::npos)
	{
		m_eWrLastError = EWR_PATH_INVALID;
		return NULL;
	}
	else if(iPos2 != strPath.length() - 1)
	{
		strPath = strPath.substr(0,iPos2 + 1);
	}

	if(strPath.empty())
	{
		m_eWrLastError = EWR_PATH_INVALID;
		return false;
	}

	size_t iPos = strPath.find_last_of('\\');
	PWpfFileList pList = NULL;

	string strSubDir = "";
	string strParentPath = strPath;
	if(iPos != string::npos)
	{
		strSubDir = strPath.substr(iPos + 1,strPath.length() - iPos);
		strParentPath = strPath.substr(0,iPos);
		pList = FindDirFromList(strParentPath,m_pWpfRoot);
	}
	else
	{
		pList = m_pWpfRoot;
		strSubDir = strPath;
	}

	PWpfFileList prtnlist = NULL;

	if(pList)
	{
		prtnlist = CreateDirToList(strSubDir,pList);
	}
	else//父目录不存在,递归创建
	{
		//创建父目录
		PWpfFileList parentlist = CreateDir(strParentPath.c_str());
		if(parentlist)//在父目录中创建本身目录
		{
			prtnlist = CreateDirToList(strSubDir,parentlist);
		}
		else
		{
			return NULL;
		}
	}

	if (prtnlist)
	{
		prtnlist->pfcb->pfcb1->iHashKey = HashFileName(strPath.c_str());
	}

	return prtnlist;
}

bool CWpf::DeleteDir(const char * strPathName)
{
	if(strPathName == NULL)
	{
		m_eWrLastError = EWR_PARA_ERROR;
		return false;
	}

	if(!IsOpenWpfFile() || ((m_dwOpenType & EWOT_RDWR) == 0))
	{
		m_eWrLastError = EWR_FILE_NOT_OPEN;
		return false;
	}

	if(m_bSaving)
	{
		m_eWrLastError = EWR_FILE_SAVING;
		return false;
	}

	string strPath = strPathName;
	replace(strPath.begin(),strPath.end(),'/','\\');
	//去掉后面连续的"\\"
	size_t iPos2 = strPath.find_last_not_of('\\');
	if(iPos2 == string::npos)
	{
		m_eWrLastError = EWR_PATH_INVALID;
		return NULL;
	}
	else if(iPos2 != strPath.length() - 1)
	{
		strPath = strPath.substr(0,iPos2 + 1);
	}

	if(strPath.empty())
	{
		m_eWrLastError = EWR_PATH_INVALID;
		return false;
	}

	size_t iPos = strPath.find_last_of("\\");
	PWpfFileList pList = NULL;

	string strSubDir = "";
	if(iPos != string::npos)
	{
		strSubDir = strPath.substr(iPos + 1,strPath.length() - iPos);
		string strDir = strPath.substr(0,iPos);
		pList = FindDirFromList(strDir.c_str(),m_pWpfRoot);
	}
	else
	{
		pList = m_pWpfRoot;
		strSubDir = strPath;
	}

	bool bRtn = false;

	if(pList)
	{
		bRtn = DelDirFromList(strSubDir,pList);
	}

	return bRtn;	
}

PWpfFileList CWpf::CreateDirToList(const string &strSubDir,PWpfFileList pList)
{
	if(!pList || strSubDir.length() > MAX_FILE_NAME_LEN || ((m_dwOpenType & EWOT_RDWR) == 0))
	{
		if (m_bAlertError)
		{
			char szTemp[512] = {0};
			sprintf(szTemp,"创建目录失败,%s",strSubDir.c_str());
			MessageBox(NULL,szTemp,"提示",MB_OK|MB_ICONERROR);
		}

		m_eWrLastError = EWR_PARA_ERROR;
		return NULL;
	}

	if(m_bSaving)
	{
		m_eWrLastError = EWR_FILE_SAVING;
		return NULL;
	}

	if(pList->MDirs.find(strSubDir) != pList->MDirs.end())
	{
		m_eWrLastError = EWR_DIR_EXIST;
		return NULL;
	}


	PFCB pfcb = new FCB();
	pfcb->pfcb1 = new FCB1();
	pfcb->pfcb2 = new FCB2();
	pfcb->pParent = pList;
	

	PWpfFileList sublist = new WpfFileList;
	sublist->pfcb = pfcb;

	//速度优先时一次性存取整个fcb,这个fcb可以一次性释放,但中间新加的fcb得记下来,以便最后释放
	if (m_bSpeedPrior)
	{
		NEWFCB *newfcb = new NEWFCB();
		newfcb->pfcb = pfcb;
		newfcb->pNext = m_CreateFcbHeader.pNext;
		m_CreateFcbHeader.pNext = newfcb;


		NEWDIR *newdir = new NEWDIR();
		newdir->pList = sublist;
		newdir->pNext = m_CreateDirHeader.pNext;
		m_CreateDirHeader.pNext = newdir;
	}


	memcpy(sublist->pfcb->pfcb2->strName,strSubDir.c_str(),strSubDir.length());
	sublist->pfcb->pfcb2->dwAttribute |= EFA_DIR;
	pList->MDirs[strSubDir] = sublist;
	pList->pfcb->pfcb1->dwSize ++;

	m_WpfHeader.dwDirCount++;

	m_bModifyed = true;

	return sublist;
}

bool CWpf::DelDirFromList(const string &strSubDir,PWpfFileList pList)
{
	if(!pList || strSubDir.length() > MAX_FILE_NAME_LEN || ((m_dwOpenType & EWOT_RDWR) == 0))
	{
		if (m_bAlertError)
		{
			char szTemp[512] = {0};
			sprintf(szTemp,"删除目录失败,%s",strSubDir.c_str());
			MessageBox(NULL,szTemp,"提示",MB_OK|MB_ICONERROR);
		}
		m_eWrLastError = EWR_PARA_ERROR;
		return false;
	}

	if(m_bSaving)
	{
		m_eWrLastError = EWR_FILE_SAVING;
		return false;
	}

	MList::iterator itr = pList->MDirs.find(strSubDir);
	if(itr == pList->MDirs.end())
	{
		m_eWrLastError = EWR_DIR_NOT_EXIST;
		return false;
	}

	//先删除strSubDir的子目录及子文件再删除strSubDir
	PWpfFileList pSubList = itr->second;
	if(pSubList)
	{
		MList::iterator itr2;
		while(!pSubList->MDirs.empty())
		{
			itr2 = pSubList->MDirs.begin();
			if(!DelDirFromList(itr2->first,pSubList))
			{
				pSubList->MDirs.erase(itr2);
			}
		}
		pSubList->MDirs.clear();

		MSFCB::iterator itr3;
		while(!pSubList->MFiles.empty())
		{
			itr3 = pSubList->MFiles.begin();
			if(!DelFileFromList(itr3->first,pSubList))
			{
				pSubList->MFiles.erase(itr3);
			}
		}

		pSubList->MFiles.clear();

		//如果非速度优先要delete申请的内存,速度优先在最后关闭或保存时统一Delete,这里只清除
		if (!m_bSpeedPrior)
		{
			SAFE_DELETE(pSubList->pfcb->pfcb1);
			SAFE_DELETE(pSubList->pfcb->pfcb2);
			SAFE_DELETE(pSubList->pfcb);
			SAFE_DELETE(pSubList);
		}
	}


	pList->MDirs.erase(itr);
	pList->pfcb->pfcb1->dwSize -= 1;
	m_WpfHeader.dwDirCount  --;

	m_bModifyed = true;

	return true;
}

PWpfFileList CWpf::FindDir(const char * strPathName)
{
	if(strPathName == NULL || (m_dwOpenType & EWOT_LISTDIR) == 0)
	{
		m_eWrLastError = EWR_PARA_ERROR;
		return NULL;
	}

	if(!IsOpenWpfFile())
	{
		m_eWrLastError = EWR_FILE_NOT_OPEN;
		return NULL;
	}

	string strDir = strPathName;
	replace(strDir.begin(),strDir.end(),'/','\\');
	if(strDir.empty())
	{
		return m_pWpfRoot;
	}

	return FindDirFromList(strDir.c_str(),m_pWpfRoot);
}

PWpfFileList CWpf::FindDirFromList(const string &strDir,PWpfFileList pList)
{
	if(!pList || strDir.empty() || (m_dwOpenType & EWOT_LISTDIR) == 0)
		return NULL;

	size_t iPos = strDir.find_first_of("\\");

	MList::iterator itr = pList->MDirs.find( iPos == string::npos?strDir:strDir.substr(0,iPos) );
	if(itr != pList->MDirs.end())
	{
		if(iPos != string::npos)
			return FindDirFromList(strDir.substr(iPos + 1,strDir.length() - iPos),itr->second);
		else
			return itr->second;
	}
	else
	{
		return NULL;
	}
}

PFCB CWpf::CreateFile(const char * strPathName,DWORD dwLen,bool bFailExist)
{
	if(!IsOpenWpfFile() || ((m_dwOpenType & EWOT_RDWR) ==0))
	{
		m_eWrLastError = EWR_FILE_NOT_OPEN;
		return NULL;
	}

	if(m_bSaving)
	{
		m_eWrLastError = EWR_FILE_SAVING;
		return NULL;
	}

	//if(dwLen <= 0)
	//{
	//	m_eWrLastError = EWR_PARA_ERROR;
	//	return NULL;
	//}


	PWpfFileList pList = NULL;

	string strPath = strPathName;
	replace(strPath.begin(),strPath.end(),'/','\\');
	size_t iPos = strPath.find_last_of("\\");
	string strFileName = strPath;
	string strDir = "";

	if(iPos != string::npos)
	{
		strDir = strPath.substr(0,iPos);
		strFileName = strPath.substr(iPos + 1,strPath.length() - iPos);
		pList = FindDirFromList(strDir.c_str(),m_pWpfRoot);

		if(NULL == pList)
		{
			pList = CreateDir(strDir.c_str());
		}
	}
	else
	{
		pList = m_pWpfRoot;
	}

	if(strFileName.empty())
	{
		m_eWrLastError = EWR_PARA_ERROR;
		return NULL;
	}

	if(NULL == pList)
	{
		return NULL;
	}

	//if (strFileName.find("W001") != string::npos)
	//{
	//	int  i = 3;
	//}

	PFCB pfcb = AddFileToDir(strDir,strFileName,dwLen,pList,bFailExist);

	if(NULL == pfcb)
	{
		return NULL;
	}

	FileClose();

	//添加文件成功把文件指针停在这个文件的起始位置
	FileOpen(pfcb->pfcb1);

	m_bModifyed = true;

	return pfcb;
}

PFCB CWpf::CreateFileFromBuff(const char * strPathName,const char * buff,DWORD dwLen,bool bFailExist,bool bCompress,bool bUnCompress)
{
	
		return NULL;
	
}

const char *  CWpf::GetDirFullPath(PWpfFileList pList)
{
	static string strPath = "";//因为有可能要传到动态库之外,所以用static
	string strTemp = "";
	strPath.clear();

	while(pList && pList->pfcb && pList->pfcb->pParent && pList->pfcb->pfcb2)//root 不包含在全路径中
	{
		strTemp = pList->pfcb->pfcb2->strName;
		if (strPath.empty())
			strPath = strTemp;
		else
			strPath = strTemp + "\\" + strPath;

		pList = pList->pfcb->pParent;
	}

	return strPath.c_str();
}

PFCB CWpf::ImportFileToDir(CWpfInterface * pWpf,PFCB pfcb,PWpfFileList pList,bool bFailExist)
{
	
		return NULL;
	
}

PFCB CWpf::AddFileToDir(const string &strParentDir,const string &strFileName,DWORD dwLen,PWpfFileList pList,bool bFailExist)
{
	if (!IsOpenWpfFile() || (m_dwOpenType & EWOT_RDWR) == 0)
	{
		m_eWrLastError = EWR_FILE_NOT_OPEN;
		return NULL;
	}

	if(!pList || strFileName.length() > MAX_FILE_NAME_LEN)
	{
		if (m_bAlertError)
		{
			char szTemp[512] = {0};
			sprintf(szTemp,"添加文件失败,%s\\%s",strParentDir.c_str(),strFileName.c_str());
			MessageBox(NULL,szTemp,"提示",MB_OK|MB_ICONERROR);
		}

		m_eWrLastError = EWR_PARA_ERROR;
		return NULL;
	}

	if(m_bSaving)
	{
		m_eWrLastError = EWR_FILE_SAVING;
		return NULL;
	}

	if(pList->MFiles.find(strFileName) != pList->MFiles.end())
	{
		if (bFailExist)
		{
			m_eWrLastError = EWR_FILE_EXIST;
			return NULL;
		}
		else
		{
			if (!DelFileFromList(strFileName,pList))
			{
				return NULL;
			}
		}
	}

	DWORD dwBlockNum = dwLen / m_WpfHeader.wBytesPerBlock + ( (dwLen % m_WpfHeader.wBytesPerBlock) == 0?0:1 );
	if (dwBlockNum == 0)//最少占用一块,空文件占一块
	{
		dwBlockNum = 1;
	}

	//找到连续的空白空间
	PBlankBlockNode pBlankBlock = NULL;
	int iTimes = 0;
	while(NULL == pBlankBlock && iTimes < 1000)
	{
		pBlankBlock = FindBlankBlock(dwBlockNum);
		//如果空白的空间不够就扩展它
		if( NULL == pBlankBlock)
		{
			if(!WpfExpend(m_dwExpendLen))
			{
				return NULL;
			}
		}

		iTimes ++;
	}

	if( NULL == pBlankBlock)
	{
		return false;
	}


	//如果这个文件的写入会覆盖备份的FCB保存一下,要把最后正常保存到磁盘中的FCB作为备份的FCB,重写当前的FCB
	if (m_WpfHeader.wHeaderSize + pBlankBlock->blankBlock.dwStart * m_WpfHeader.wBytesPerBlock + dwLen >= m_WpfHeader_Bak.iBlankBlockPos)
	{
		//如果上一次本身就没有保存成功,这一次用得本来就是备份的信息,那么在要覆盖之前先保存一下,不然就有可能会出现m_WpfHeader.bySavingHeader和m_WpfHeader_Bak.bySavingHeader同时为TRUE的情况
		if (m_WpfHeader.bySavingHeader)
		{
			if (WpfSave() == false)
			{
				return false;
			}
		}

		if(!m_WpfHeader_Bak.bySavingHeader)
		{
			//写入备份头及备份FCB需要重置的标记
			_lseeki64(m_iFileHandle[EFT_MAIN],m_iWpfStartPos + sizeof(S_WpfHeader) * 2 - 1,SEEK_SET);
			m_WpfHeader_Bak.bySavingHeader = TRUE;//打个标记就行,不需要再立即保存一次,下次m_WpfHeader需要保存时,m_WpfHeader_Bak会转换成m_WpfHeader
			if(_write(m_iFileHandle[EFT_MAIN],&(m_WpfHeader_Bak.bySavingHeader),sizeof(BYTE)) <= 0)
			{
				return false;
			}
		}

	}


	PFCB pfcb = new FCB();
	pfcb->pfcb1 = new FCB1();
	pfcb->pfcb2 = new FCB2();
	pfcb->pParent = pList;

	//速度优先时一次性存取整个fcb,这个fcb可以一次性释放,但中间新加的fcb得记下来,以便最后释放
	if (m_bSpeedPrior)
	{
		NEWFCB * newfcb = new NEWFCB();
		newfcb->pfcb = pfcb;
		newfcb->pNext = m_CreateFcbHeader.pNext;
		m_CreateFcbHeader.pNext = newfcb;
	}

	memcpy(pfcb->pfcb2->strName,strFileName.c_str(),strFileName.length());
	pfcb->pfcb1->dwStart = pBlankBlock->blankBlock.dwStart;
	pfcb->pfcb1->dwSize = dwLen;
	pfcb->pfcb2->dwAttribute |= EFA_FILE;

	string strPath = strFileName;
	if (!strParentDir.empty())
	{
		strPath = strParentDir + "\\" + strFileName;
	}
	pfcb->pfcb1->iHashKey = HashFileName(strPath.c_str());

	//加入到临时hash表
	if (m_bBuildTempFileHashEnabled)
		m_kTempFileHash[pfcb->pfcb1->iHashKey] = 0;

	pList->MFiles[strFileName] = pfcb;
	pList->pfcb->pfcb1->dwSize ++;
	m_WpfHeader.dwFileCount  ++;

	m_MhashFcb[pfcb->pfcb1->iHashKey] = pfcb;


	//这块用完了要删除
	if (dwBlockNum >= pBlankBlock->blankBlock.dwSize)
	{
		if (pBlankBlock == m_pBlankBlockList)
		{
			m_pBlankBlockList = pBlankBlock->pNext;
			if (m_pBlankBlockList)
			{
				m_pBlankBlockList->pParent = NULL;
			}
		}
		else//pParent一定存在
		{
			pBlankBlock->pParent->pNext = pBlankBlock->pNext;
			if (pBlankBlock->pNext)
			{
				pBlankBlock->pNext->pParent = pBlankBlock->pParent;
			}
		}

		SAFE_DELETE(pBlankBlock);
		m_WpfHeader.dwBlankBlockSize --;
	}
	else
	{
		pBlankBlock->blankBlock.dwStart += dwBlockNum;
		pBlankBlock->blankBlock.dwSize -= dwBlockNum;
	}

	pfcb->pfcb2->dwAttribute |= m_dwAddAttributeToNewFile;

	m_bModifyed = true;
	FileOpen(pfcb->pfcb1);

	return pfcb;
}

PFCB CWpf::FindFile_PFCB(const char * strPathName)
{
	if(!IsOpenWpfFile() || !strPathName || (m_dwOpenType & EWOT_LISTDIR) == 0)
	{
		m_eWrLastError = EWR_FILE_NOT_OPEN;
		return NULL;
	}

	if (m_dwOpenType & EWOT_LISTFCBHASH)
	{
		MIFCB::iterator itr = m_MhashFcb.find(HashFileName(strPathName));
		if (itr != m_MhashFcb.end())
		{
			return itr->second;
		}
	}
	else
	{
		string strPath = strPathName;
		//replace(strPath.begin(),strPath.end(),'/','\\');
		size_t iPos = strPath.find_last_of("\\");

		PWpfFileList pList = NULL;
		string strFileName = strPathName;

		if(iPos != string::npos)
		{
			string strDir = strPath.substr(0,iPos);
			strFileName = strPath.substr(iPos + 1,strPath.length() - iPos);
			pList = FindDirFromList(strDir,m_pWpfRoot);
		}
		else
		{
			pList = m_pWpfRoot;
		}

		if(pList)
		{
			MSFCB::iterator itr = pList->MFiles.find(strFileName);
			if(itr != pList->MFiles.end())
			{
				return itr->second;
			}
		}
	}

	return NULL;
}

PFCB1 CWpf::FindFile_PFCB1(const char * strPathName)
{
	if(!strPathName)
	{
		return NULL;
	}

	if((m_dwOpenType & EWOT_LISTDIR) && !(m_dwOpenType & EWOT_LISTFCBHASH))
	{
		PFCB pfcb = FindFile_PFCB(strPathName);
		if(pfcb)
		{
			return pfcb->pfcb1;
		}
	}
	else
	{
		return FindFile_PFCB1(HashFileName(strPathName));
	}

	return NULL;
}

PFCB1 CWpf::FindFile_PFCB1(__int64 iHash)
{
	if(!IsOpenWpfFile())
	{
		m_eWrLastError = EWR_FILE_NOT_OPEN;
		return NULL;
	}

	if ((m_dwOpenType & EWOT_LISTDIR) && (m_dwOpenType & EWOT_LISTFCBHASH))
	{
		MIFCB::iterator itr = m_MhashFcb.find(iHash);
		if (itr != m_MhashFcb.end())
		{
			if (itr->second)
			{
				return itr->second->pfcb1;
			}
		}
	}
	else if (m_dwOpenType & EWOT_LISTHASH)
	{
		MIFCB1::iterator itr = m_MhashFcb1.find(iHash);
		if (itr != m_MhashFcb1.end())
		{
			return itr->second;
		}
	}

	return NULL;
}

bool CWpf::DeleteFile(const char * strPathName)
{
	if(!IsOpenWpfFile() || ((m_dwOpenType & EWOT_RDWR) == 0 ))
	{
		m_eWrLastError = EWR_FILE_NOT_OPEN;
		return false;
	}

	if(m_bSaving)
	{
		m_eWrLastError = EWR_FILE_SAVING;
		return false;
	}

	string strPath = strPathName;
	replace(strPath.begin(),strPath.end(),'/','\\');
	size_t iPos = strPath.find_last_of("\\");

	bool bRtn = false;
	PWpfFileList pList = NULL;
	string strFileName = strPathName;

	if(iPos != string::npos)
	{
		string strDir = strPath.substr(0,iPos);
		strFileName = strPath.substr(iPos + 1,strPath.length() - iPos);

		if(strFileName.empty())
		{
			m_eWrLastError = EWR_PARA_ERROR;
			return false;
		}

		pList = FindDirFromList(strDir.c_str(),m_pWpfRoot);
	}
	else
	{
		pList = m_pWpfRoot;
	}

	if(pList)
	{
		bRtn =  DelFileFromList(strFileName,pList);
	}

	return bRtn;
}

bool CWpf::DelFileFromList(const string &strFileName,PWpfFileList pList)
{
	if(!IsOpenWpfFile() || ((m_dwOpenType & EWOT_RDWR) == 0 ))
	{
		m_eWrLastError = EWR_FILE_NOT_OPEN;
		return false;
	}

	if(m_bSaving)
	{
		m_eWrLastError = EWR_FILE_SAVING;
		return false;
	}

	MSFCB::iterator itr = pList->MFiles.find(strFileName);
	if(itr != pList->MFiles.end())
	{
		PFCB pfcb = itr->second;

		m_WpfHeader.dwFileCount  --;
		DWORD dwBlockNum = pfcb->pfcb1->dwSize / m_WpfHeader.wBytesPerBlock + ( (pfcb->pfcb1->dwSize % m_WpfHeader.wBytesPerBlock) == 0?0:1 );
		if (dwBlockNum == 0)
		{
			dwBlockNum = 1;
		}

		//回收空间
		bool bFind = false;
		PBlankBlockNode pBlankBlock = m_pBlankBlockList,pSmallBlock = NULL;
		DWORD dwStartBlock = pfcb->pfcb1->dwStart,dwEndBlcokAdd1 = dwStartBlock + dwBlockNum;
		while(pBlankBlock)
		{
			if (!bFind)
			{
				if (pBlankBlock->blankBlock.dwStart == dwEndBlcokAdd1)
				{
					pBlankBlock->blankBlock.dwStart = dwStartBlock;
					pBlankBlock->blankBlock.dwSize += dwBlockNum;

					//向前看能不能合并小的块成大的块
					PBlankBlockNode pNode = pBlankBlock;
					PBlankBlockNode pParent = pNode->pParent;
					while(pNode && pParent)
					{
						if (pParent->blankBlock.dwStart + pParent->blankBlock.dwSize == pNode->blankBlock.dwStart)
						{
							pParent->blankBlock.dwSize += pNode->blankBlock.dwSize;
							pParent->pNext = pNode->pNext;
							if (pNode->pNext)
							{
								pNode->pNext->pParent = pParent;
							}

							SAFE_DELETE(pNode);
							pNode = pParent;
							pParent = pNode->pParent;
							m_WpfHeader.dwBlankBlockSize --;
						}
						else
						{
							pNode = pParent;
							pParent = pParent->pParent;
						}
					}

					bFind = true;
					break;
				}
				else if (pBlankBlock->blankBlock.dwStart + pBlankBlock->blankBlock.dwSize == dwStartBlock)
				{
					pBlankBlock->blankBlock.dwSize += dwBlockNum;

					//向后看能不能合并小的块成大的块
					PBlankBlockNode pNode = pBlankBlock;
					PBlankBlockNode pNext = pNode->pNext;
					while(pNode && pNext)
					{
						if (pNode->blankBlock.dwStart + pNode->blankBlock.dwSize == pNext->blankBlock.dwStart)
						{
							pNode->blankBlock.dwSize += pNext->blankBlock.dwSize;
							pNode->pNext = pNext->pNext;
							if (pNext->pNext)
							{
								pNext->pNext->pParent = pNode;
							}

							SAFE_DELETE(pNext);
							pNext = pNode->pNext;
							m_WpfHeader.dwBlankBlockSize --;
						}
						else
						{
							pNode = pNext;
							pNext = pNext->pNext;
						}
					}

					bFind = true;
					break;
				}
			}

			if (pBlankBlock->blankBlock.dwStart < dwStartBlock)
			{
				pSmallBlock = pBlankBlock;
			}

			pBlankBlock = pBlankBlock->pNext;
		}

		//没插入就新加一块,保证有序,否则合并的地方就会有问题
		if (!bFind)
		{
			pBlankBlock = new BlankBlockNode();
			pBlankBlock->blankBlock.dwStart = dwStartBlock;
			pBlankBlock->blankBlock.dwSize = dwBlockNum;

			if (pSmallBlock)
			{
				pBlankBlock->pNext = pSmallBlock->pNext;
				pBlankBlock->pParent = pSmallBlock;
				if (pSmallBlock->pNext)
				{
					pSmallBlock->pNext->pParent = pBlankBlock;
				}

				pSmallBlock->pNext = pBlankBlock;
			}
			else
			{
				pBlankBlock->pParent = NULL;
				pBlankBlock->pNext = m_pBlankBlockList;
				if (m_pBlankBlockList)
				{
					m_pBlankBlockList->pParent = pBlankBlock;
				}
				m_pBlankBlockList = pBlankBlock;
			}


			m_WpfHeader.dwBlankBlockSize ++;
		}


		m_MhashFcb.erase(pfcb->pfcb1->iHashKey);

		if(pfcb->pfcb1 == m_pOpenFileFcb1[EFT_MAIN])
		{
			FileClose(EFT_MAIN);
		}

		//如果非速度优先要delete申请的内存,速度优先在最后关闭或保存时统一Delete,这里只清除
		if (!m_bSpeedPrior)
		{
			SAFE_DELETE(pfcb->pfcb1);
			SAFE_DELETE(pfcb->pfcb2);
			SAFE_DELETE(pfcb);
		}

		pList->MFiles.erase(itr);
		pList->pfcb->pfcb1->dwSize --;


		m_bModifyed = true;

		return true;
	}

	m_eWrLastError = EWR_FILE_NOT_EXIST;
	return false;
}


bool CWpf::UpdateFile(const char * strPathName,const char *buff,DWORD dwLen,bool bCompress,bool bUnCompress)
{
	if(!IsOpenWpfFile() || ((m_dwOpenType & EWOT_RDWR) == 0))
	{
		m_eWrLastError = EWR_FILE_NOT_OPEN;
		return false;
	}

	if(m_bSaving)
	{
		m_eWrLastError = EWR_FILE_SAVING;
		return false;
	}

	if(!this->CreateFileFromBuff(strPathName,buff,dwLen,false,bCompress,bUnCompress))
	{
		return false;
	}

	return true;
}

PFCB CWpf::FileOpenEx(const char * strPathName,E_FPTYPE fpType)
{
	if (!strPathName || !IsOpenWpfFile() || (m_dwOpenType & EWOT_LISTDIR) == 0)
	{
		return NULL;
	}

	if(m_bSaving)
	{
		m_eWrLastError = EWR_FILE_SAVING;
		return NULL;
	}

	PFCB pfcb = FindFile_PFCB(strPathName);
	if (!pfcb)
	{
		m_pOpenFileFcb1[fpType] = NULL;
		return NULL;
	}

	m_pOpenFileFcb1[fpType] = pfcb->pfcb1;

	_lseeki64(m_iFileHandle[fpType],m_iWpfStartPos + m_WpfHeader.wHeaderSize + pfcb->pfcb1->dwStart * m_WpfHeader.wBytesPerBlock,SEEK_SET);	
	m_iOpenFilePointPos[fpType] = 0;

	return pfcb;
}

PFCB1 CWpf::FileOpen(const char * strPathName,E_FPTYPE fpType)
{
	if (!strPathName || !IsOpenWpfFile())
	{
		return NULL;
	}

	if(m_bSaving)
	{
		m_eWrLastError = EWR_FILE_SAVING;
		return NULL;
	}

	m_pOpenFileFcb1[fpType] = FindFile_PFCB1(strPathName);

	if(NULL == m_pOpenFileFcb1[fpType])
	{
		return NULL;
	}

	_lseeki64(m_iFileHandle[fpType],m_iWpfStartPos + m_WpfHeader.wHeaderSize + m_pOpenFileFcb1[fpType]->dwStart * m_WpfHeader.wBytesPerBlock,SEEK_SET);	
	m_iOpenFilePointPos[fpType] = 0;

	return m_pOpenFileFcb1[fpType];
}

bool CWpf::FileOpen(PFCB1 pfcb1,E_FPTYPE fpType)
{
	if(m_bSaving)
	{
		m_eWrLastError = EWR_FILE_SAVING;
		return false;
	}

	bool bRtn = false;
	if (pfcb1)
	{
		if (-1 != _lseeki64(m_iFileHandle[fpType],m_iWpfStartPos + m_WpfHeader.wHeaderSize + pfcb1->dwStart * m_WpfHeader.wBytesPerBlock,SEEK_SET))
		{
			m_pOpenFileFcb1[fpType] = pfcb1;
			m_iOpenFilePointPos[fpType] = 0;
			bRtn = true;
		}
	}

	return bRtn;
}

PFCB1 CWpf::FileOpen_Hash2FCB1(__int64 iHash,E_FPTYPE fpType)
{
	MIFCB1::iterator itr = m_MhashFcb1.find(iHash);
	if (itr != m_MhashFcb1.end())
	{
		FileOpen(itr->second,fpType);
		return itr->second;
	}

	return NULL;
}

PFCB CWpf::FileOpen_Hash2FCB(__int64 iHash,E_FPTYPE fpType)
{
	MIFCB::iterator itr = m_MhashFcb.find(iHash);
	if (itr != m_MhashFcb.end())
	{
		FileOpen(itr->second->pfcb1,fpType);
		return itr->second;
	}

	return NULL;
}

bool CWpf::FileClose(E_FPTYPE fpType)
{
	m_pOpenFileFcb1[fpType] = NULL;
	m_iOpenFilePointPos[fpType] = 0;

	return true;
}

bool CWpf::FileSeek(DWORD dwOffset,int iOrigin,E_FPTYPE fpType)
{
	if(NULL == m_pOpenFileFcb1[fpType])
	{
		return false;
	}

	if(m_bSaving)
	{
		m_eWrLastError = EWR_FILE_SAVING;
		return false;
	}

	switch(iOrigin)
	{
	case SEEK_SET:
		if(dwOffset >= m_pOpenFileFcb1[fpType]->dwSize)
		{
			return false;
		}

		_lseeki64(m_iFileHandle[fpType],m_iWpfStartPos + m_WpfHeader.wHeaderSize + m_pOpenFileFcb1[fpType]->dwStart * m_WpfHeader.wBytesPerBlock + dwOffset,SEEK_SET);	
		break;
	case SEEK_CUR:
		if(dwOffset + m_iOpenFilePointPos[fpType] >= m_pOpenFileFcb1[fpType]->dwSize)
		{
			return false;
		}

		m_iOpenFilePointPos[fpType] += dwOffset;

		_lseeki64(m_iFileHandle[fpType],m_iWpfStartPos + m_WpfHeader.wHeaderSize + m_pOpenFileFcb1[fpType]->dwStart * m_WpfHeader.wBytesPerBlock + m_iOpenFilePointPos[fpType],SEEK_SET);	
		break;
	case SEEK_END:
		if(dwOffset >= m_pOpenFileFcb1[fpType]->dwSize)
		{
			return false;
		}

		m_iOpenFilePointPos[fpType] = m_pOpenFileFcb1[fpType]->dwSize - dwOffset;

		_lseeki64(m_iFileHandle[fpType],m_iWpfStartPos + m_WpfHeader.wHeaderSize + m_pOpenFileFcb1[fpType]->dwStart * m_WpfHeader.wBytesPerBlock + m_iOpenFilePointPos[fpType],SEEK_SET);	
		break;

	default:
		break;
	}


	return true;
}

DWORD CWpf::FileRead(void* pBbuff,DWORD dwSize,E_FPTYPE fpType)
{
	if(m_bSaving)
	{
		m_eWrLastError = EWR_FILE_SAVING;
		return 0;
	}

	if(NULL == m_pOpenFileFcb1[fpType] || dwSize <= 0 || m_iOpenFilePointPos[fpType] >= m_pOpenFileFcb1[fpType]->dwSize)
	{
		return 0;
	}

	if(m_iOpenFilePointPos[fpType] + dwSize >= m_pOpenFileFcb1[fpType]->dwSize)
	{
		dwSize = m_pOpenFileFcb1[fpType]->dwSize - m_iOpenFilePointPos[fpType];
	}

	DWORD dwRtn = _read(m_iFileHandle[fpType],pBbuff,dwSize);
	if (dwRtn > 0)
	{
		m_iOpenFilePointPos[fpType] += dwRtn;
	}

	return dwRtn;
}

DWORD CWpf::FileWrite(const void* pBbuff,DWORD dwSize,E_FPTYPE fpType)
{
	if(m_bSaving)
	{
		m_eWrLastError = EWR_FILE_SAVING;
		return 0;
	}

	if(NULL == m_pOpenFileFcb1[fpType] || ((m_dwOpenType & EWOT_RDWR) == 0 ))
	{
		m_eWrLastError = EWR_FILE_NOT_OPEN;
		return false;
	}

	if( dwSize <= 0 || m_iOpenFilePointPos[fpType] >= m_pOpenFileFcb1[fpType]->dwSize)
	{
		m_eWrLastError = EWR_PARA_ERROR;
		return false;
	}

	if(m_iOpenFilePointPos[fpType] + dwSize >= m_pOpenFileFcb1[fpType]->dwSize)
	{
		dwSize = m_pOpenFileFcb1[fpType]->dwSize - m_iOpenFilePointPos[fpType];
	}

	DWORD dwRtn = _write(m_iFileHandle[fpType],pBbuff,dwSize);
	if (dwRtn > 0)
	{
		m_iOpenFilePointPos[fpType] += dwRtn;
	}

	return dwRtn;
}

DWORD CWpf::FileLength(E_FPTYPE fpType)
{
	if(NULL == m_pOpenFileFcb1[fpType])
	{
		return 0;
	}

	return m_pOpenFileFcb1[fpType]->dwSize;
}

__int64 CWpf::GetFileOffset(E_FPTYPE fpType)
{
	if(NULL == m_pOpenFileFcb1[fpType])
	{
		return -1;
	}

	return m_iWpfStartPos + m_WpfHeader.wHeaderSize + m_pOpenFileFcb1[fpType]->dwStart * m_WpfHeader.wBytesPerBlock;
}

__int64 CWpf::GetFileOffset(const char * strPathName,E_FPTYPE fpType)
{
	if (!FileOpen(strPathName,fpType))
	{
		return -1;
	}

	if(NULL == m_pOpenFileFcb1[fpType])
	{
		return -1;
	}

	return m_iWpfStartPos + m_WpfHeader.wHeaderSize + m_pOpenFileFcb1[fpType]->dwStart * m_WpfHeader.wBytesPerBlock;
}

int CWpf::GetFileHandle(E_FPTYPE fpType)
{
	return m_iFileHandle[fpType];
}

__int64 CWpf::WpfSeek(__int64 dwOffset,int iOrigin)
{
	if(!IsOpenWpfFile())
	{
		m_eWrLastError = EWR_FILE_NOT_OPEN;
		return -1;
	}

	if(m_bSaving)
	{
		m_eWrLastError = EWR_FILE_SAVING;
		return -1;
	}

	FileClose();

	return _lseeki64(m_iFileHandle[EFT_MAIN],m_iWpfStartPos + dwOffset,iOrigin);
}

DWORD CWpf::WpfRead(void* pBbuff,DWORD dwSize)
{
	if(!IsOpenWpfFile())
	{
		m_eWrLastError = EWR_FILE_NOT_OPEN;
		return 0;
	}

	if(m_bSaving)
	{
		m_eWrLastError = EWR_FILE_SAVING;
		return 0;
	}


	FileClose();

	return _read(m_iFileHandle[EFT_MAIN],pBbuff,dwSize);
}

DWORD CWpf::WpfWrite(const void* pBbuff,DWORD dwSize)
{
	if(!IsOpenWpfFile() || ((m_dwOpenType & EWOT_RDWR) == 0 ))
	{
		m_eWrLastError = EWR_FILE_NOT_OPEN;
		return 0;
	}

	if(m_bSaving)
	{
		m_eWrLastError = EWR_FILE_SAVING;
		return 0;
	}

	FileClose();

	return _write(m_iFileHandle[EFT_MAIN],pBbuff,dwSize);
}

__int64 CWpf::WpfLength()
{
	if(!IsOpenWpfFile())
	{
		m_eWrLastError = EWR_FILE_NOT_OPEN;
		return 0;
	}

	return m_WpfHeader.iWpfSize;
}


bool CWpf::WpfExpend(DWORD dwLen)
{
	if(!IsOpenWpfFile() || ((m_dwOpenType & EWOT_RDWR) == 0))
	{
		m_eWrLastError = EWR_FILE_NOT_OPEN;
		return false;
	}

	if(m_bSaving)
	{
		m_eWrLastError = EWR_FILE_SAVING;
		return false;
	}

	//如果扩展后要求保存,保证一次扩展的长度大于原来目录结构等信息内容的长度,这样写新目录结构信息时不会覆盖老的,即使新的目录结构信息保存失败也不会覆盖原来的目录结构等信息
	if (m_bSaveWhenExpended)
	{
		DWORD dwFcbListSize_Disk = m_WpfHeader_Disk.dwBlankBlockSize * sizeof(BlankBlock) + (m_WpfHeader_Disk.dwDirCount + m_WpfHeader_Disk.dwFileCount) * FCB_SIZE;
		dwFcbListSize_Disk += 1024;//留一点位置
		
		if (m_WpfHeader_Disk.iBlankBlockPos + dwFcbListSize_Disk > m_WpfHeader.wHeaderSize + m_WpfHeader.iWpfSize + dwLen)
		{
			dwLen = (DWORD)(m_WpfHeader_Disk.iBlankBlockPos + dwFcbListSize_Disk - (m_WpfHeader.wHeaderSize + m_WpfHeader.iWpfSize));
		}		
	}

	//凑成m_WpfHeader.wBytesPerBlock的倍数
	if((dwLen % m_WpfHeader.wBytesPerBlock) != 0)
	{
		dwLen += m_WpfHeader.wBytesPerBlock - (dwLen % m_WpfHeader.wBytesPerBlock);
	}
	//指定文件大小
	__int64 iNewWpfSize = m_WpfHeader.iWpfSize + dwLen;
	DWORD dwNewTotalBlocks = DWORD(iNewWpfSize/m_WpfHeader.wBytesPerBlock);
	DWORD dwOldTotalBlocks = m_WpfHeader.dwTotalBlocks;
	DWORD dwNewBlankBlockSize = m_WpfHeader.dwBlankBlockSize;


	//检查能不能合并块
	bool bFind = false;
	PBlankBlockNode pBlankBlock = m_pBlankBlockList,pLast = m_pBlankBlockList,pFindBlock = NULL;

	while(pBlankBlock)
	{
		if (!bFind)
		{
			if (pBlankBlock->blankBlock.dwStart + pBlankBlock->blankBlock.dwSize == dwOldTotalBlocks)
			{
				//pBlankBlock->blankBlock.dwSize += (dwNewTotalBlocks - dwOldTotalBlocks);
				pFindBlock = pBlankBlock;
				bFind = true;
			}
		}

		pLast = pBlankBlock;
		pBlankBlock = pBlankBlock->pNext;
	}

	if (!bFind)
	{
		dwNewBlankBlockSize ++;
	}


	//分配空间
	errno_t err = _chsize_s(m_iFileHandle[EFT_MAIN],m_iWpfStartPos + m_WpfHeader.wHeaderSize + iNewWpfSize + dwNewBlankBlockSize * sizeof(BlankBlock) + (m_WpfHeader.dwDirCount + m_WpfHeader.dwFileCount) * FCB_SIZE);
	if(err != 0)
	{
		m_eWrLastError = EWR_ERROR;
		return false;
	}


	m_WpfHeader.iWpfSize = iNewWpfSize;
	m_WpfHeader.dwTotalBlocks = dwNewTotalBlocks;
	m_WpfHeader.iBlankBlockPos = m_WpfHeader.wHeaderSize + iNewWpfSize;
	m_WpfHeader.dwBlankBlockSize  = dwNewBlankBlockSize;
	m_WpfHeader.iFatPos = m_WpfHeader.iBlankBlockPos + dwNewBlankBlockSize * sizeof(BlankBlock);


	//没有合并就新加一块在后面
	if (!bFind)
	{
		pBlankBlock = new BlankBlockNode();
		pBlankBlock->blankBlock.dwStart = dwOldTotalBlocks;
		pBlankBlock->blankBlock.dwSize = dwNewTotalBlocks - dwOldTotalBlocks;
		pBlankBlock->pParent = pLast;
		pBlankBlock->pNext = NULL;
		if (pLast)
		{
			pLast->pNext = pBlankBlock;
		}
		else//一定是m_pBlankBlockList为空
		{
			m_pBlankBlockList = pBlankBlock;
			m_pBlankBlockList->pParent = NULL;
		}
	}
	else if (pFindBlock)
	{
		pFindBlock->blankBlock.dwSize += (dwNewTotalBlocks - dwOldTotalBlocks);
	}
	else
	{
		int i = 3;
	}


	//每次扩展之后都保存一下,否则出错整个文件系统崩溃
	m_bModifyed = true;
	if (m_bSaveWhenExpended)
	{
		if(!WpfSave())
		{
			return false;
		}
	}

	return true;
}

PBlankBlockNode CWpf::FindBlankBlock(DWORD dwNum)
{
	if(!IsOpenWpfFile() || ((m_dwOpenType & EWOT_RDWR) == 0))
	{
		m_eWrLastError = EWR_FILE_NOT_OPEN;
		return NULL;
	}

	if(dwNum <= 0)
	{
		dwNum = 1;
	}

	PBlankBlockNode pBlankBlockList = m_pBlankBlockList;
	while(pBlankBlockList)
	{
		if (pBlankBlockList->blankBlock.dwSize >= dwNum)
		{
			return pBlankBlockList;
		}

		pBlankBlockList = pBlankBlockList->pNext;
	}

	return NULL;
}

bool CWpf::WpfSave()
{
	if (!m_bModifyed)
	{
		return false;
	}

	m_bModifyed = false;

	if(!IsOpenWpfFile() || ((m_dwOpenType & EWOT_RDWR) == 0 ))
	{
		m_eWrLastError = EWR_FILE_NOT_OPEN;
		return false;
	}

	if (m_bSaving)
	{
		m_eWrLastError = EWR_FILE_SAVING;
		return false;
	}

	m_bSaving = true;

	//先关闭当前访问的文件
	FileClose();

	
	
	//如果备份FCB已经被后面的文件覆盖(m_WpfHeader_Bak.bySavingHeader为TRUE),或者上次要求写备份头,却没有写成功,那么当前FCB不要覆盖,成为备份FCB,新的FCB写在备份的FCB后面
	if (m_WpfHeader_Bak.bySavingHeader)
	{
		//写入备份头正在保存的标记,如果此时写失败,下次会使用原来的文件头及fcblist,并且下次保存时会重新到这里来保存备份头
		_lseeki64(m_iFileHandle[EFT_MAIN],m_iWpfStartPos + sizeof(S_WpfHeader) * 2 - 1,SEEK_SET);
		m_WpfHeader_Bak.bySavingHeader = TRUE;
		if(_write(m_iFileHandle[EFT_MAIN],&(m_WpfHeader_Bak.bySavingHeader),sizeof(BYTE)) <= 0)
		{
			goto Save_Fail;
		}

		if (!m_WpfHeader_Disk.bySavingHeader)//m_WpfHeader_Disk.bySavingHeader只有刚开始创建的时候为TRUE,这个时候还没有FCB保存到磁盘,所以没有备份信息
		{
			m_WpfHeader_Bak = m_WpfHeader_Disk;
			m_WpfHeader_Bak.bySavingHeader = FALSE;
		}
		//else//第一次没有备份信息,等下次再保存的时候就有了,就会再保存备份信息,因为这种情况下m_WpfHeader_Bak.bySavingHeader还是TRUE
		//{
		//}

		//写入备份头,如果此时写失败,下次会使用原来的文件头及fcblist,并且下次保存时会重新到这里来保存备份头
		_lseeki64(m_iFileHandle[EFT_MAIN],m_iWpfStartPos + sizeof(S_WpfHeader),SEEK_SET);
		if(_write(m_iFileHandle[EFT_MAIN],&m_WpfHeader_Bak,sizeof(S_WpfHeader)) <= 0)
		{
			goto Save_Fail;
		}
	}

	//如果m_WpfHeader_Bak指向的FCB等信息是有效的,应该避免覆盖掉
	if(!m_WpfHeader_Bak.bySavingHeader)
	{
		//m_WpfHeader_Bak对应的FCB大小
		DWORD dwFcbListSize_Bak = m_WpfHeader_Bak.dwBlankBlockSize * sizeof(BlankBlock) + (m_WpfHeader_Bak.dwDirCount + m_WpfHeader_Bak.dwFileCount) * FCB_SIZE;
		//避免备份FCB被覆盖
		if (m_WpfHeader_Bak.iBlankBlockPos + dwFcbListSize_Bak > m_WpfHeader.iBlankBlockPos)
		{
			m_WpfHeader.iBlankBlockPos = m_WpfHeader_Bak.iBlankBlockPos + dwFcbListSize_Bak;
		}
	}
	
	//重新设置文件的长度,有可能中间删除或新加目录,fcblist的大小就发生了改变,整个文件的大小也应该变小
	DWORD dwFcbSize = m_WpfHeader.dwBlankBlockSize * sizeof(BlankBlock) + (m_WpfHeader.dwDirCount + m_WpfHeader.dwFileCount) * FCB_SIZE;
	//errno_t err = _chsize_s(m_iFileHandle[EFT_MAIN],m_iWpfStartPos + m_WpfHeader.wHeaderSize + m_WpfHeader.iWpfSize + dwFcbSize);
	errno_t err = _chsize_s(m_iFileHandle[EFT_MAIN],m_WpfHeader.iBlankBlockPos + dwFcbSize);
	if(err != 0)
	{
		goto Save_Fail;
	}



	//写入正在保存的标记,如果此时写失败,下次会使用原来的文件头及fcblist
	_lseeki64(m_iFileHandle[EFT_MAIN],m_iWpfStartPos + sizeof(S_WpfHeader) - 1,SEEK_SET);
	m_WpfHeader.bySavingHeader = TRUE;
	if(_write(m_iFileHandle[EFT_MAIN],&(m_WpfHeader.bySavingHeader),sizeof(BYTE)) <= 0)
	{
		goto Save_Fail;
	}


	//写入空白块信息,如果此时写失败,下次会使用备份的文件头及fcblist
	if (m_WpfHeader.dwBlankBlockSize > 0)
	{
		PBlankBlock pBlock = new BlankBlock[m_WpfHeader.dwBlankBlockSize];

		PBlankBlock p = pBlock;
		PBlankBlockNode pNode = m_pBlankBlockList;
		while(pNode)
		{
			p->dwStart = pNode->blankBlock.dwStart;
			p->dwSize = pNode->blankBlock.dwSize;
			pNode = pNode->pNext;
			p ++;
		}

		_lseeki64(m_iFileHandle[EFT_MAIN],m_iWpfStartPos + m_WpfHeader.iBlankBlockPos,SEEK_SET);
		if(_write(m_iFileHandle[EFT_MAIN],pBlock,m_WpfHeader.dwBlankBlockSize * sizeof(BlankBlock)) <= 0)
		{
			goto Save_Fail;
		}

		SAFE_DELETE_ARRAY(pBlock)
	}

	m_WpfHeader.iFatPos = m_WpfHeader.iBlankBlockPos + m_WpfHeader.dwBlankBlockSize * sizeof(BlankBlock);
	
	//写入FCB,如果此时写失败,下次会使用备份的文件头及fcblist
	if(!WpfWriteFCB())
	{
		goto Save_Fail;
	}


	//写入文件头,如果此时写失败,m_WpfHeader.bySavingHeader必定为TRUE,因为这是头的最后一字节,为了防止头部一分写入新的,一部分老的,m_WpfHeader.bySavingHeader为TRUE时读取时用备份的文件头和备份的FCB
	m_WpfHeader.bySavingHeader = FALSE;
	_lseeki64(m_iFileHandle[EFT_MAIN],m_iWpfStartPos,SEEK_SET);
	if(_write(m_iFileHandle[EFT_MAIN],&m_WpfHeader,sizeof(S_WpfHeader)) <= 0)
	{
		goto Save_Fail;
	}



	//把缓冲区的内容立即更新到磁盘,改成了使用句柄后打开的文件是没有buff的,不需要fflush
	//    fflush(m_pFp[EFT_MAIN]);

	m_bSaving = false;
	m_WpfHeader_Disk = m_WpfHeader;//重新记录修改前状态

	return true;

	//保存失败时,必须关闭文件的写操作,免得头及FCB信息被破坏,因为如果是扩展的时候保存失败,m_pBlankBlockList,m_WpfHeader,m_pWpfRoot等信息要回滚,就要求重新打开并读取这些信息
	//但不能关闭其读操作,免得外部的程序读操作错误导致外部程序异常,包括主线程与其它线程的读操作
Save_Fail:
	m_bSaving = false;
	m_bModifyed = false;
	m_dwOpenType &= (~EWOT_RDWR);//EWOT_LISTDIR和EWOT_LISTFCBHASH留着,以便外部进行读取操作
	return false;
}

void CWpf::ClearEmptyDirAndFile(PWpfFileList pList)
{
	MList &dirlist = pList->MDirs;
	MSFCB &filelist = pList->MFiles;

	PFCB pfcb = NULL;
	PWpfFileList pDelList = NULL;

	for (MList::iterator itr = dirlist.begin();itr != dirlist.end(); )
	{
		ClearEmptyDirAndFile(itr->second);

		if (itr->second->MDirs.empty() && itr->second->MFiles.empty())
		{
			if (!m_bSpeedPrior)
			{
				pDelList = itr->second;
				SAFE_DELETE(pDelList->pfcb->pfcb1);
				SAFE_DELETE(pDelList->pfcb->pfcb2);
				SAFE_DELETE(pDelList->pfcb);
				SAFE_DELETE(pDelList);
			}

			itr = dirlist.erase(itr);
		}
		else
		{
			itr++;
		}
	}

	MSFCB::iterator itr = filelist.begin();
	while(itr != filelist.end())
	{
		if (itr->second->pfcb1->dwSize == 0)//dwSize为0说明是空文件
		{
			pfcb = itr->second;
			if (!m_bSpeedPrior)
			{
				SAFE_DELETE(pfcb->pfcb1);
				SAFE_DELETE(pfcb->pfcb2);
				SAFE_DELETE(pfcb);
			}
			itr = filelist.erase(itr);
		}
		else
		{
			itr ++;
		}
	}

}

bool CWpf::WpfWriteFCB()
{

	//如果要求清除空目录及文件
	if (m_bClearEmptyDirAndFile)
	{
		ClearEmptyDirAndFile(m_pWpfRoot);
	}


	//写入fcb
	DWORD dwCount = m_WpfHeader.dwDirCount + m_WpfHeader.dwFileCount;
	if (m_bSpeedPrior)
	{
		//为了速度这里先申请一块空间,然后 一并写入磁盘
		PFCB1 fcb1 = new FCB1[dwCount];//fcb1列表
		PFCB2 fcb2 = new FCB2[dwCount];//fcb2列表


		vector<PWpfFileList> VDirs;
		VDirs.push_back(m_pWpfRoot);
		m_pWpfRoot->pfcb->pfcb1->dwStart = 1;
		m_pWpfRoot->pfcb->pfcb1->dwSize = (DWORD)(m_pWpfRoot->MDirs.size() + m_pWpfRoot->MFiles.size());
		m_pWpfRoot->pfcb->pfcb1->iHashKey = 0;

		memcpy(&fcb1[0],m_pWpfRoot->pfcb->pfcb1,FCB1_SIZE);
		memcpy(&fcb2[0],m_pWpfRoot->pfcb->pfcb2,FCB2_SIZE);


		PWpfFileList pFront = NULL;
		PFCB pfcb = NULL;
		DWORD dwNo = 0;//该目录对应fcb的位置
		DWORD dwStart = 1;//该目录子目录fcb所在的位置
		dwStart += m_pWpfRoot->pfcb->pfcb1->dwSize;

		while(!VDirs.empty())
		{
			pFront = VDirs.front();

			MList &dirlist = pFront->MDirs;
			MSFCB &filelist = pFront->MFiles;
			dwNo = pFront->pfcb->pfcb1->dwStart;

			//子目录及子文件的fcb

			for (MList::iterator itr = dirlist.begin();itr != dirlist.end(); itr++,dwNo++)
			{
				pfcb = itr->second->pfcb;
				pfcb->pfcb1->dwStart = dwStart;
				pfcb->pfcb1->dwSize = (DWORD)(itr->second->MDirs.size() + itr->second->MFiles.size());
				dwStart += pfcb->pfcb1->dwSize;

				pfcb->pfcb1->iHashKey = 0;

				memcpy(&fcb1[dwNo],pfcb->pfcb1,FCB1_SIZE);
				memcpy(&fcb2[dwNo],pfcb->pfcb2,FCB2_SIZE);
				VDirs.push_back(itr->second);
			}

			for(MSFCB::iterator itr = filelist.begin(); itr != filelist.end(); itr++,dwNo++)
			{
				memcpy(&fcb1[dwNo],itr->second->pfcb1,FCB1_SIZE);
				memcpy(&fcb2[dwNo],itr->second->pfcb2,FCB2_SIZE);
			}

			//VDirs.pop_back();
			VDirs.erase(VDirs.begin());
		}

		bool bRtn = true;

		if(_write(m_iFileHandle[EFT_MAIN],fcb1,(m_WpfHeader.dwDirCount + m_WpfHeader.dwFileCount)*FCB1_SIZE) <= 0)
		{
			bRtn = false;
		}
		SAFE_DELETE_ARRAY(fcb1);

		if(bRtn && _write(m_iFileHandle[EFT_MAIN],fcb2,(m_WpfHeader.dwDirCount + m_WpfHeader.dwFileCount)*FCB2_SIZE) <= 0)
		{
			bRtn = false;
		}
		SAFE_DELETE_ARRAY(fcb2);

		return bRtn;
	}
	else
	{
		vector<PWpfFileList> VDirs;
		VDirs.push_back(m_pWpfRoot);
		m_pWpfRoot->pfcb->pfcb1->dwStart = 1;
		m_pWpfRoot->pfcb->pfcb1->dwSize = (DWORD)(m_pWpfRoot->MDirs.size() + m_pWpfRoot->MFiles.size());
		
		//写入根目录
		_lseeki64(m_iFileHandle[EFT_MAIN],m_iWpfStartPos + m_WpfHeader.iFatPos,SEEK_SET);
		if(_write(m_iFileHandle[EFT_MAIN],m_pWpfRoot->pfcb->pfcb1,FCB1_SIZE) <= 0)
		{
			return false;
		}
	
		_lseeki64(m_iFileHandle[EFT_MAIN],m_iWpfStartPos + m_WpfHeader.iFatPos + dwCount * FCB1_SIZE,SEEK_SET);
		if(_write(m_iFileHandle[EFT_MAIN],m_pWpfRoot->pfcb->pfcb2,FCB2_SIZE) <= 0)
		{
			return false;
		}


		PWpfFileList pFront = NULL;
		PFCB pfcb = NULL;
		DWORD dwNo = 0;//该目录对应fcb的位置
		DWORD dwStart = 1;//该目录子目录fcb所在的位置
		dwStart += m_pWpfRoot->pfcb->pfcb1->dwSize;

		while(!VDirs.empty())
		{
			pFront = VDirs.front();

			MList &dirlist = pFront->MDirs;
			MSFCB &filelist = pFront->MFiles;
			dwNo = pFront->pfcb->pfcb1->dwStart;

			//子目录及子文件的fcb
			for (MList::iterator itr = dirlist.begin();itr != dirlist.end(); itr++,dwNo++)
			{
				pfcb = itr->second->pfcb;
				pfcb->pfcb1->dwStart = dwStart;
				pfcb->pfcb1->dwSize = (DWORD)(itr->second->MDirs.size() + itr->second->MFiles.size());
				dwStart += pfcb->pfcb1->dwSize;


				_lseeki64(m_iFileHandle[EFT_MAIN],m_iWpfStartPos + m_WpfHeader.iFatPos + dwNo * FCB1_SIZE,SEEK_SET);
				if(_write(m_iFileHandle[EFT_MAIN],pfcb->pfcb1,FCB1_SIZE) <= 0)
				{
					return false;
				}

				_lseeki64(m_iFileHandle[EFT_MAIN],m_iWpfStartPos + m_WpfHeader.iFatPos + dwCount * FCB1_SIZE + dwNo * FCB2_SIZE,SEEK_SET);
				if(_write(m_iFileHandle[EFT_MAIN],pfcb->pfcb2,FCB2_SIZE) <= 0)
				{
					return false;
				}


				VDirs.push_back(itr->second);
			}

			for(MSFCB::iterator itr = filelist.begin(); itr != filelist.end(); itr++,dwNo++)
			{
				_lseeki64(m_iFileHandle[EFT_MAIN],m_iWpfStartPos + m_WpfHeader.iFatPos + dwNo * FCB1_SIZE,SEEK_SET);
				if(_write(m_iFileHandle[EFT_MAIN],itr->second->pfcb1,FCB1_SIZE) <= 0)
				{
					return false;
				}

				_lseeki64(m_iFileHandle[EFT_MAIN],m_iWpfStartPos + m_WpfHeader.iFatPos + dwCount * FCB1_SIZE + dwNo * FCB2_SIZE,SEEK_SET);
				if(_write(m_iFileHandle[EFT_MAIN],itr->second->pfcb2,FCB2_SIZE) <= 0)
				{
					return false;
				}
			}

			//VDirs.pop_back();
			VDirs.erase(VDirs.begin());
		}
	}

	return true;
}

bool CWpf::WpfLoadFat()
{
	if(!IsOpenWpfFile())
	{
		m_eWrLastError = EWR_FILE_NOT_OPEN;
		return false;
	}

	if(m_WpfHeader.dwDirCount < 1)
	{
		m_eWrLastError = EWR_NOT_WPF_FILE;
		return false;
	}

	_lseeki64(m_iFileHandle[EFT_MAIN],m_iWpfStartPos + m_WpfHeader.iFatPos,SEEK_SET);

	DWORD dwCount = m_WpfHeader.dwDirCount + m_WpfHeader.dwFileCount;

	if (m_bSpeedPrior)
	{
		m_pFcb1List = new FCB1[dwCount];
		_read(m_iFileHandle[EFT_MAIN],m_pFcb1List,dwCount * FCB1_SIZE);

		if (m_dwOpenType & EWOT_LISTHASH)
		{
			//hash map
			PFCB1 pfcb1 = m_pFcb1List;
			for (DWORD i = 0; i < dwCount; i++,pfcb1++)
			{
#ifdef _DEBUG
				if (pfcb1->iHashKey != 0 && m_MhashFcb1.find(pfcb1->iHashKey) != m_MhashFcb1.end())
				{
					int i = 3;
				}
#endif
				if (pfcb1->iHashKey != 0)
				{
					m_MhashFcb1[pfcb1->iHashKey] = pfcb1;
				}
			}
		}	


		if (m_dwOpenType & EWOT_LISTDIR)//要求构造目录结构
		{
			m_pFcb2List = new FCB2[dwCount];
			_read(m_iFileHandle[EFT_MAIN],m_pFcb2List,dwCount * FCB2_SIZE);

			m_pFcbList = new FCB[dwCount];
			memset(m_pFcbList,0,dwCount * sizeof(FCB));

			m_pDirList = new WpfFileList[m_WpfHeader.dwDirCount];

			//递归构造目录
			vector<PWpfFileList> VDirs;
			m_pWpfRoot = &(m_pDirList[0]);
			m_pWpfRoot->pfcb = &(m_pFcbList[0]);
			m_pWpfRoot->pfcb->pfcb1 = &(m_pFcb1List[0]);
			m_pWpfRoot->pfcb->pfcb2 = &(m_pFcb2List[0]);
			VDirs.push_back(m_pWpfRoot);

			DWORD dwDirNo = 1,dwPos = 0;
			PWpfFileList pFront = NULL;
			while(!VDirs.empty())
			{
				pFront = VDirs.front();

				dwPos = pFront->pfcb->pfcb1->dwStart;
				for (DWORD i = 0; i < pFront->pfcb->pfcb1->dwSize; i++,dwPos++)
				{
					PFCB2 fcb2 = &(m_pFcb2List[dwPos]);
					if(fcb2->dwAttribute & EFA_DIR)
					{
						PWpfFileList sublist = &(m_pDirList[dwDirNo]);
						dwDirNo ++;
						PFCB newfcb = &(m_pFcbList[dwPos]);
						newfcb->pfcb1 = &(m_pFcb1List[dwPos]);
						newfcb->pfcb2 = &(m_pFcb2List[dwPos]);
						newfcb->pParent = pFront;
						sublist->pfcb = newfcb;

						VDirs.push_back(sublist);
						pFront->MDirs[fcb2->strName] = sublist;

					}
					else if (fcb2->dwAttribute & EFA_FILE)
					{
						PFCB newfcb = &(m_pFcbList[dwPos]);
						newfcb->pfcb1 = &(m_pFcb1List[dwPos]);
						newfcb->pfcb2 = &(m_pFcb2List[dwPos]);
						newfcb->pParent = pFront;

						pFront->MFiles[fcb2->strName] = newfcb;

						if (m_dwOpenType & EWOT_LISTFCBHASH)
						{
							m_MhashFcb[newfcb->pfcb1->iHashKey] = newfcb;
						}
					}
				}

				VDirs.erase(VDirs.begin());
			}
		}

	}
	else
	{
		m_pFcb1List = NULL;
		m_pFcb2List = NULL;
		m_pFcbList = NULL;
		m_pDirList = NULL;

		
		if (m_dwOpenType & EWOT_LISTHASH)
		{
			for (DWORD index = 0; index < dwCount; index ++)
			{
				PFCB1 pfcb1 = new FCB1();
				_read(m_iFileHandle[EFT_MAIN],pfcb1,FCB1_SIZE);

				//hash map
#ifdef _DEBUG
				if (pfcb1->iHashKey != 0 && m_MhashFcb1.find(pfcb1->iHashKey) != m_MhashFcb1.end())
				{
					int i = 3;
				}
#endif
				if (pfcb1->iHashKey != 0)
				{
					m_MhashFcb1[pfcb1->iHashKey] = pfcb1;
				}
			}
		}

		if (m_dwOpenType & EWOT_LISTDIR)//要求构造目录结构
		{
			//递归构造目录
			vector<PWpfFileList> VDirs;
			m_pWpfRoot = new WpfFileList();
			m_pWpfRoot->pfcb = new FCB();
			m_pWpfRoot->pfcb->pfcb1 = new FCB1();
			m_pWpfRoot->pfcb->pfcb2 = new FCB2();
			VDirs.push_back(m_pWpfRoot);

			//读取第一个根目录
			_lseeki64(m_iFileHandle[EFT_MAIN],m_iWpfStartPos + m_WpfHeader.iFatPos,SEEK_SET);
			_read(m_iFileHandle[EFT_MAIN],m_pWpfRoot->pfcb->pfcb1,FCB1_SIZE);

			_lseeki64(m_iFileHandle[EFT_MAIN],m_iWpfStartPos + m_WpfHeader.iFatPos + dwCount * FCB1_SIZE,SEEK_SET);
			_read(m_iFileHandle[EFT_MAIN],m_pWpfRoot->pfcb->pfcb2,FCB2_SIZE);

		
			PWpfFileList pFront = NULL;
			while(!VDirs.empty())
			{
				pFront = VDirs.front();

				for (DWORD dwPos = pFront->pfcb->pfcb1->dwStart; dwPos < pFront->pfcb->pfcb1->dwStart + pFront->pfcb->pfcb1->dwSize; dwPos++)
				{
					PFCB2 fcb2 = new FCB2();
					//读取这一条fcb2
					_lseeki64(m_iFileHandle[EFT_MAIN],m_iWpfStartPos + m_WpfHeader.iFatPos + dwCount * FCB1_SIZE + dwPos * FCB2_SIZE,SEEK_SET);
					_read(m_iFileHandle[EFT_MAIN],fcb2,FCB2_SIZE);

					if(fcb2->dwAttribute & EFA_DIR)
					{
						PFCB newfcb = new FCB();
						newfcb->pfcb1 = new FCB1();
					    //读取这一条fcb1
						_lseeki64(m_iFileHandle[EFT_MAIN],m_iWpfStartPos + m_WpfHeader.iFatPos + dwPos * FCB1_SIZE,SEEK_SET);
						_read(m_iFileHandle[EFT_MAIN],newfcb->pfcb1,FCB1_SIZE);

						PWpfFileList sublist = new WpfFileList();

						newfcb->pfcb2 = fcb2;
						newfcb->pParent = pFront;
						sublist->pfcb = newfcb;

						VDirs.push_back(sublist);
						pFront->MDirs[fcb2->strName] = sublist;
					}
					else if (fcb2->dwAttribute & EFA_FILE)
					{
						PFCB newfcb = new FCB();
						newfcb->pfcb1 = new FCB1();
						//读取这一条fcb1
						_lseeki64(m_iFileHandle[EFT_MAIN],m_iWpfStartPos + m_WpfHeader.iFatPos + dwPos * FCB1_SIZE,SEEK_SET);
						_read(m_iFileHandle[EFT_MAIN],newfcb->pfcb1,FCB1_SIZE);

						newfcb->pfcb2 = fcb2;
						newfcb->pParent = pFront;

						pFront->MFiles[fcb2->strName] = newfcb;

						if (m_dwOpenType & EWOT_LISTFCBHASH)
						{
							m_MhashFcb[newfcb->pfcb1->iHashKey] = newfcb;
						}
					}
				}

				VDirs.erase(VDirs.begin());
			}
		}

	}



	return true;
}

bool CWpf::WpfFormat()
{
	if(!IsOpenWpfFile() || ((m_dwOpenType & EWOT_RDWR) == 0))
	{
		m_eWrLastError = EWR_FILE_NOT_OPEN;
		return false;
	}

	if(m_bSaving)
	{
		m_eWrLastError = EWR_FILE_SAVING;
		return false;
	}

	//清空
	WpfClear();

	m_WpfHeader.dwBlankBlockSize = 1;
	m_WpfHeader.iFatPos = m_WpfHeader.iBlankBlockPos + m_WpfHeader.dwBlankBlockSize * sizeof(BlankBlock);

	m_pBlankBlockList = new BlankBlockNode();
	m_pBlankBlockList->blankBlock.dwStart = 0;
	m_pBlankBlockList->blankBlock.dwSize = m_WpfHeader.dwTotalBlocks;
	//加入根目录,目录下没有内容
	PFCB pfcb = new FCB();
	pfcb->pfcb1 = new FCB1();
	pfcb->pfcb2 = new FCB2();

	m_pWpfRoot = new WpfFileList;
	m_pWpfRoot->pfcb = pfcb;


	//速度优先时一次性存取整个fcb,这个fcb可以一次性释放,但中间新加的fcb得记下来,以便最后释放
	if (m_bSpeedPrior)
	{
		NEWFCB *newfcb = new NEWFCB();
		newfcb->pfcb = pfcb;
		newfcb->pNext = m_CreateFcbHeader.pNext;
		m_CreateFcbHeader.pNext = newfcb;

		NEWDIR *newdir = new NEWDIR();
		newdir->pList = m_pWpfRoot;
		newdir->pNext = m_CreateDirHeader.pNext;
		m_CreateDirHeader.pNext = newdir;
	}


	memcpy(m_pWpfRoot->pfcb->pfcb2->strName,"Root",sizeof("Root"));
	m_pWpfRoot->pfcb->pfcb2->dwAttribute = EFA_DIR;
	m_pWpfRoot->pfcb->pfcb1->dwStart = 0;
	m_pWpfRoot->pfcb->pfcb1->dwSize = 0;
	m_WpfHeader.dwDirCount = 1;
	//清空文件列表
	m_pWpfRoot->MFiles.clear();
	m_pWpfRoot->MDirs.clear();

	return true;
}

bool CWpf::IsOpenWpfFile()
{
	return (m_iFileHandle[EFT_MAIN] != -1);
}

PFCB CWpf::ImportFile(const char *strParnetDir,const char *strFilePath,bool bCompress,bool bFailExist)
{
	
		return NULL;
	
}

PWpfFileList CWpf::ImportDir(const char *strParnetDir,const char *strDirPath,bool bCompress,bool bFailExist,LPWpfCallBack pCallBack)
{
	if (!IsOpenWpfFile() || ((m_dwOpenType & EWOT_RDWR) == 0))
	{
		m_eWrLastError = EWR_FILE_NOT_OPEN;
		return NULL;
	}

	if(m_bSaving)
	{
		m_eWrLastError = EWR_FILE_SAVING;
		return NULL;
	}

	string strSrcDir = strDirPath;
	if(strSrcDir.empty())
	{
		return NULL;
	}

	string strDestDir = "";
	if (strParnetDir)
		strDestDir = strDestDir + strParnetDir;

	//去掉后面连续的"\\"
	size_t iPos = strSrcDir.find_last_not_of('\\');
	if(iPos == string::npos)
	{
		m_eWrLastError = EWR_PATH_INVALID;
		return NULL;
	}
	else if(iPos != strSrcDir.length() - 1)
	{
		strSrcDir = strSrcDir.substr(0,iPos + 1);
	}

	if (!strDestDir.empty())
	{
		iPos = strDestDir.find_last_not_of('\\');
		if(iPos == string::npos)
		{
			m_eWrLastError = EWR_PATH_INVALID;
			return NULL;
		}
		else if(iPos != strDestDir.length() - 1)
		{
			strDestDir = strDestDir.substr(0,iPos + 1);
		}
	}

	iPos = strSrcDir.find_last_of('\\');
	if(iPos != string::npos)
	{
		string strTemp = strSrcDir.substr(iPos + 1,strSrcDir.length() - iPos - 1);
		if (strDestDir.empty())
			strDestDir = strTemp;
		else
			strDestDir = strDestDir + "\\" + strTemp;
	}

	//创建目录
	PWpfFileList pList = this->FindDir(strDestDir.c_str());
	if (!pList)
	{
		pList = this->CreateDir(strDestDir.c_str());
	}
	if (!pList)
	{
		m_eWrLastError = EWR_PATH_INVALID;
		return NULL;
	}

	ImportSubDir(strDestDir.c_str(),strDirPath,bCompress,bFailExist,pCallBack);

	return pList;
}

bool CWpf::ImportDirFromList(CWpfInterface * pWpf,PWpfFileList pSrcList,PWpfFileList pDestList,bool bFailExist,LPWpfCallBack pCallBack)
{
	if (!pWpf || !pSrcList || !pDestList)
	{
		return false;
	}

	if (!IsOpenWpfFile() || ((m_dwOpenType & EWOT_RDWR) == 0))
	{
		m_eWrLastError = EWR_FILE_NOT_OPEN;
		return NULL;
	}

	if(m_bSaving)
	{
		m_eWrLastError = EWR_FILE_SAVING;
		return false;
	}

	PWpfFileList pList = NULL;

	MList::iterator itr = pDestList->MDirs.find(pSrcList->pfcb->pfcb2->strName);
	if(itr != pDestList->MDirs.end())
	{
		pList = itr->second;
	}

	if (!pList)
	{
		pList = this->CreateDirToList(pSrcList->pfcb->pfcb2->strName,pDestList);
	}

	if (!pList)
	{
		return NULL;
	}

	ImportSubDirFromList(pWpf,pSrcList,pList,bFailExist,pCallBack);
	return true;
}

PWpfFileList CWpf::ImportSubDir(const char *strParnetDir,const char *strDirPath,bool bCompress,bool bFailExist,LPWpfCallBack pCallBack)
{
	if (!IsOpenWpfFile() || ((m_dwOpenType & EWOT_RDWR) == 0) || !strParnetDir || !strDirPath)
	{
		m_eWrLastError = EWR_FILE_NOT_OPEN;
		return NULL;
	}

	if(m_bSaving)
	{
		m_eWrLastError = EWR_FILE_SAVING;
		return NULL;
	}

	struct _finddata_t file;
	string strSrcDir = strDirPath;
	if (strSrcDir.empty())
	{
		return NULL;
	}	
	string strSrc = strSrcDir + "\\*.*";
	string strDestDir = strParnetDir;

	intptr_t fhandle = _findfirst(strSrc.c_str(), &file);
	if(fhandle == -1)
	{
		return NULL;
	}

	//创建目录
	PWpfFileList list = this->FindDir(strParnetDir);
	if(!list)
	{
		list = this->CreateDir(strParnetDir);
	}

	if(!list)
	{
		return NULL;
	}


	do
	{
		if(file.attrib & _A_SUBDIR)
		{
			//if(strcmp(file.name,".") != 0 && strcmp(file.name,"..") != 0 && strcmp(file.name,".svn") != 0)
			if(file.name[0] != '.')
			{        
				string strd;
				if(strDestDir.empty())
					strd = file.name; 
				else
					strd = strDestDir + "\\" + file.name;

				string strs;
				if(strSrcDir.empty())
					strs = file.name;
				else
					strs = strSrcDir + "\\" + file.name;

				PWpfFileList sublist = ImportSubDir(strd.c_str() , strs.c_str(),bCompress,bFailExist,pCallBack);
				if(sublist)
				{
					list->MDirs[file.name] = sublist;
				}
				else if (!IsOpenWpfFile() || (m_dwOpenType & EWOT_RDWR) == 0)//有可能在ImportFile时扩展失败导致文件的写操作被关闭
				{
					m_eWrLastError = EWR_FILE_NOT_OPEN;
					return NULL;
				}

			}
		}
		else
		{ 
			string strd = strDestDir;
			string strs;
			if(strSrcDir.empty())
				strs = file.name;
			else
				strs = strSrcDir + "\\" + file.name;

			PFCB pfcb = ImportFile(strd.c_str() , strs.c_str(),bCompress,bFailExist);
			if(pfcb)
			{
				list->MFiles[file.name] = pfcb;
			}
			else if (!IsOpenWpfFile() || (m_dwOpenType & EWOT_RDWR) == 0)//有可能在ImportFile时扩展失败导致文件的写操作被关闭
			{
				m_eWrLastError = EWR_FILE_NOT_OPEN;
				return NULL;
			}


			if (pCallBack)
			{
				pCallBack(strs.c_str());
			}
		}
	} while(!_findnext(fhandle, &file));

	_findclose(fhandle);


	return list;
}

bool CWpf::ImportSubDirFromList(CWpfInterface * pWpf,PWpfFileList pSrcList,PWpfFileList pDestList,bool bFailExist,LPWpfCallBack pCallBack)
{
	if (!pWpf || !pSrcList || !pDestList)
	{
		return false;
	}

	if (!IsOpenWpfFile() || ((m_dwOpenType & EWOT_RDWR) == 0))
	{
		m_eWrLastError = EWR_FILE_NOT_OPEN;
		return NULL;
	}

	if(m_bSaving)
	{
		m_eWrLastError = EWR_FILE_SAVING;
		return false;
	}

	//导入目录
	MList &subdirs = pSrcList->MDirs;
	PWpfFileList sublist = NULL;
	for (MList::iterator itr = subdirs.begin(); itr != subdirs.end(); itr++)
	{
		sublist = NULL;
		MList::iterator itr2 = pDestList->MDirs.find(itr->first);
		if(itr2 != pDestList->MDirs.end())
		{
			sublist = itr2->second;
		}

		if (!sublist)
		{
			sublist = this->CreateDirToList(itr->first,pDestList);
		}

		if (sublist)
		{
			ImportSubDirFromList(pWpf,itr->second,sublist,bFailExist,pCallBack);
		}
	}

	//导入文件
	MSFCB &srcfiles = pSrcList->MFiles;
	PFCB subfile = NULL,srcfcb = NULL;

	char *buff = new char[BLOCK_SIZE];
	DWORD dwLeftBytes = 0;
	string strParentPath = GetDirFullPath(pDestList);

	for (MSFCB::iterator itr = srcfiles.begin(); itr != srcfiles.end(); itr ++)
	{
		srcfcb = itr->second;
		if (!srcfcb || !srcfcb->pfcb1)
		{
			continue;
		}

		////打开源文件
		//if (!pWpf->FileOpen(srcfcb->pfcb1))
		//{
		//	continue;
		//}

		////为了速度这里不调用ImportFileToDir
		//subfile = AddFileToDir(strParentPath,itr->first,srcfcb->pfcb1->dwSize,pDestList,bFailExist);

		//if (subfile)
		//{
		//	dwLeftBytes = srcfcb->pfcb1->dwSize;
		//	while(dwLeftBytes > 0)
		//	{
		//		DWORD dwReadBytes = min(BLOCK_SIZE,dwLeftBytes);
		//		if (pWpf->FileRead(buff,dwReadBytes) <= 0)
		//		{
		//			break;
		//		}

		//		FileWrite(buff,dwReadBytes);
		//		dwLeftBytes -= dwReadBytes;
		//	}
		//	subfile->pfcb2->dwAttribute |= srcfcb->pfcb2->dwAttribute;		
		//}

		if (!ImportFileToDir(pWpf,srcfcb,pDestList,bFailExist))
		{
			continue;
		}

		if (pCallBack)
		{
			pCallBack((strParentPath + "\\" + itr->first).c_str());
		}

		pWpf->FileClose();
	}	

	SAFE_DELETE_ARRAY(buff);

	return true;
}

bool CWpf::ExportFile(const char *strSrcPath,const char *strDestPath)
{
	if(m_bSaving)
	{
		m_eWrLastError = EWR_FILE_SAVING;
		return false;
	}

	PFCB pfcb = FindFile_PFCB(strSrcPath);

	if (ExportFile(pfcb,strDestPath))
	{
		return true;
	}

	return false;
}

bool CWpf::ExportAllFileFromFCB1(const char *strDestPath)
{
	if (!IsOpenWpfFile() || ((m_dwOpenType & EWOT_LISTHASH) == 0) || !strDestPath || strDestPath[0] == 0)
	{
		m_eWrLastError = EWR_FILE_NOT_OPEN;
		return false;
	}

	if(m_bSaving)
	{
		m_eWrLastError = EWR_FILE_SAVING;
		return false;
	}


	BYTE *pReadBuf = new BYTE[BLOCK_SIZE + 1024];
	char szFileName[32] = {0};
	for(MIFCB1::iterator itr = m_MhashFcb1.begin(); itr != m_MhashFcb1.end(); itr ++)
	{
		if (itr->second && itr->second->iHashKey != 0 && itr->second->dwSize > 0 && itr->second->dwStart > 0)
		{
			string szFilePath = strDestPath;
			sprintf_s(szFileName,32,"%I64d",itr->second->iHashKey);

			//if (-7889083853190628685 == itr->second->iHashKey)
			//{
			//	int i = 3;
			//}
			
			szFilePath = szFilePath + "\\" + szFileName;
			FILE* fpDest = fopen(szFilePath.c_str(),"wb");
			if (!fpDest)
			{
				continue;
			}

			if (!FileOpen(itr->second))
			{
				fclose(fpDest);
				continue;
			}

			DWORD dwReadBytes = 0;
			DWORD dwLeftBytes = itr->second->dwSize;

			while(dwLeftBytes > 0)
			{
				dwReadBytes = min(dwLeftBytes,BLOCK_SIZE);
				if (FileRead(pReadBuf,dwReadBytes) <= 0)
				{
					break;
				}

				fwrite(pReadBuf,dwReadBytes,1,fpDest);

				dwLeftBytes -= dwReadBytes;
			}

			fclose(fpDest);
		}
	}

	SAFE_DELETE_ARRAY(pReadBuf);
	return true;
}

bool CWpf::ExportFile(PFCB pfcb,const char *strDestPath)
{
	
		return false;
	
}

bool CWpf::ExportDir(const char *strSrcPath,const char *strDestPath,LPWpfCallBack pCallBack)
{
	if(m_bSaving)
	{
		m_eWrLastError = EWR_FILE_SAVING;
		return false;
	}

	PWpfFileList pList = FindDir(strSrcPath);

	if (!pList || !strDestPath)
	{
		m_eWrLastError = EWR_DIR_NOT_EXIST;
		return false;
	}

	string strPath = strDestPath;
	//去掉后面连续的"\\"
	size_t iPos = strPath.find_last_not_of('\\');
	if(iPos == string::npos)
	{
		m_eWrLastError = EWR_PATH_INVALID;
		return NULL;
	}
	else if(iPos != strPath.length() - 1)
	{
		strPath = strPath.substr(0,iPos + 1);
	}

	if(strPath.empty())
	{
		m_eWrLastError = EWR_PATH_INVALID;
		return false;
	}

	ExportDirFromList(strPath,pList,pCallBack);

	return true;
}

bool CWpf::ExportSubDir(const char *strSrcPath,const char *strDestPath,LPWpfCallBack pCallBack)
{
	if(m_bSaving)
	{
		m_eWrLastError = EWR_FILE_SAVING;
		return false;
	}

	PWpfFileList pList = FindDir(strSrcPath);

	if (!pList || !strDestPath)
	{
		m_eWrLastError = EWR_DIR_NOT_EXIST;
		return false;
	}

	string strPath = strDestPath;
	//去掉后面连续的"\\"
	size_t iPos = strPath.find_last_not_of('\\');
	if(iPos == string::npos)
	{
		m_eWrLastError = EWR_PATH_INVALID;
		return NULL;
	}
	else if(iPos != strPath.length() - 1)
	{
		strPath = strPath.substr(0,iPos + 1);
	}

	if(strPath.empty())
	{
		m_eWrLastError = EWR_PATH_INVALID;
		return false;
	}

	ExportSubDirFromList(strPath,pList,pCallBack);

	return true;
}

void CWpf::ExportDirFromList(const string &strDestPath,PWpfFileList pList,LPWpfCallBack pCallBack)
{
	if (!pList || !pList->pfcb || !pList->pfcb->pfcb2)
		return;

	if(m_bSaving)
	{
		m_eWrLastError = EWR_FILE_SAVING;
		return;
	}

	string strPath = strDestPath + "\\" + pList->pfcb->pfcb2->strName;
	ExportSubDirFromList(strPath,pList,pCallBack);
}

void CWpf::ExportSubDirFromList(const string &strDestPath,PWpfFileList pList,LPWpfCallBack pCallBack)
{
	if (!pList || !pList->pfcb || !pList->pfcb->pfcb2)
		return;

	if(m_bSaving)
	{
		m_eWrLastError = EWR_FILE_SAVING;
		return;
	}

	SHCreateDirectoryEx(NULL,strDestPath.c_str(),NULL);

	//先导出子目录
	for (MList::iterator itr = pList->MDirs.begin(); itr != pList->MDirs.end(); itr++)
	{
		ExportSubDirFromList(strDestPath + "\\" + itr->first,itr->second,pCallBack);
	}

	string strFilePath = "";
	for (MSFCB::iterator itr = pList->MFiles.begin(); itr != pList->MFiles.end(); itr++)
	{
		strFilePath = strDestPath + "\\" + itr->first;
		ExportFile(itr->second,strFilePath.c_str());

		if (pCallBack)
		{
			pCallBack(strFilePath.c_str());
		}
	}
}

bool CWpf::MeargeWpf(const char *strSrcPath,__int64 iOffset,LPWpfCallBack pCallBack)
{
	if(!IsOpenWpfFile() || !m_pWpfRoot)
	{
		m_eWrLastError = EWR_FILE_NOT_OPEN;
		return false;
	}

	if(m_bSaving)
	{
		m_eWrLastError = EWR_FILE_SAVING;
		return false;
	}

	CWpfInterface *pWpf = new CWpf();
	if (!pWpf->WpfOpen(strSrcPath,iOffset,EWOT_RDWR))
	{
		SAFE_DELETE(pWpf);
		return false;
	}

	PWpfFileList pRoot = pWpf->GetRoot();
	if (!pRoot)
	{
		SAFE_DELETE(pWpf);
		return false;
	}

	bool bRtn = ImportSubDirFromList(pWpf,pRoot,m_pWpfRoot,false,pCallBack);

	SAFE_DELETE(pWpf);

	return bRtn;
}

bool CWpf::InitBuffer()
{
	DWORD seed = 0x00100001;
	DWORD index1 = 0;
	DWORD index2 = 0;
	DWORD temp1 = 0,temp2 = 0;
	int i = 0;

	memset(m_dwCryptTable,0,sizeof(m_dwCryptTable));
	for(index1 = 0; index1 < 0x100; index1++)
	{
		for (index2 = index1,i = 0; i < 5; i++,index2 += 0x100)
		{
			seed = (seed * 125 + 3)  % 0x2AAAAB;
			temp1 = (seed & 0xFFFF) << 0x10;
			seed = (seed * 125 + 3)  % 0x2AAAAB;
			temp1 = (seed & 0xFFFF);
			m_dwCryptTable[index2] = (temp1 | temp2);			
		}
	}

	return true;
}

inline DWORD CWpf::HashString(const char * pStr,DWORD type)
{
	DWORD seed1 = 0x7FED7FED;
	DWORD seed2 = 0xEEEEEEEE;
	DWORD ch;			/* One key character */

	while(*pStr != 0)
	{
		ch = toupper(*pStr++);
		seed1 = m_dwCryptTable[(type << 8) + ch] ^ (seed1 + seed2);
		seed2 = ch + seed1 + seed2 + (seed2 << 5) + 3;
	}

	return seed1;	
}

__int64 CWpf::GetHashKey(const char *str)
{
	if(!str)
	{
		return 0;
	}

	return HashFileName(str);
}

inline __int64 CWpf::HashFileName(const char * strFileName)
{
	DWORD o1 = HashString(strFileName,1);
	DWORD o2 = HashString(strFileName,2);

	return ( (((__int64)(o1)) << 32) + o2);
}

bool CWpf::CleanUpFragment()
{
	return false;
}

bool CWpf::GetSubFileName(const char *strParnetDir,LPWpfCallBack pCallBack,bool bSubDir)
{
	if(!IsOpenWpfFile())
	{
		m_eWrLastError = EWR_FILE_NOT_OPEN;
		return false;
	}

	if (m_bSaving || !strParnetDir || !pCallBack)
	{
		m_eWrLastError = EWR_ERROR;
		return false;
	}

	//定位到fcb所在位置,第一个是根目录
	FCB1 fcb1;
	FCB2 fcb2;
	DWORD dwCount = m_WpfHeader.dwDirCount + m_WpfHeader.dwFileCount;
	//读取根目录
	_lseeki64(m_iFileHandle[EFT_MAIN],m_iWpfStartPos + m_WpfHeader.iFatPos,SEEK_SET);
	_read(m_iFileHandle[EFT_MAIN],&fcb1,FCB1_SIZE);	

	string strPath = strParnetDir;
	string strParentPath = strPath;
	//读取子文件或子目录
	while(true)
	{
		size_t iPos = strPath.find_first_of("\\");
		if (iPos != string::npos)
		{
			strParentPath = strPath.substr(0,iPos);
			strPath = strPath.substr(iPos + 1,strPath.length() - iPos);
		}
		else
		{
			strParentPath = strPath;
			strPath = "";
		}

		if (strParentPath.empty())
		{
			for (size_t i = 0; i < fcb1.dwSize; i++)
			{
				_lseeki64(m_iFileHandle[EFT_MAIN],m_iWpfStartPos + m_WpfHeader.iFatPos + dwCount * FCB1_SIZE + (fcb1.dwStart + i) * FCB2_SIZE,SEEK_SET);
				_read(m_iFileHandle[EFT_MAIN],&fcb2,FCB2_SIZE);
				if (( bSubDir && (fcb2.dwAttribute & EFA_DIR) ) ||
					(!bSubDir && (fcb2.dwAttribute & EFA_FILE)) )
				{
					pCallBack(fcb2.strName);
				}
			}
			break;
		}
		
		int iFcbPos = -1;
		for (size_t i = 0; i < fcb1.dwSize; i++)
		{
			_lseeki64(m_iFileHandle[EFT_MAIN],m_iWpfStartPos + m_WpfHeader.iFatPos + dwCount * FCB1_SIZE + (fcb1.dwStart + i) * FCB2_SIZE,SEEK_SET);
			_read(m_iFileHandle[EFT_MAIN],&fcb2,FCB2_SIZE);
			if ((fcb2.dwAttribute & EFA_DIR) && stricmp(strParentPath.c_str(),fcb2.strName) == 0)
			{
				iFcbPos = (int)(i + fcb1.dwStart);
				_lseeki64(m_iFileHandle[EFT_MAIN],m_iWpfStartPos + m_WpfHeader.iFatPos + iFcbPos * FCB1_SIZE,SEEK_SET);
				_read(m_iFileHandle[EFT_MAIN],&fcb1,FCB1_SIZE);
				break;
			}
		}

		if (iFcbPos == -1)
		{
			m_eWrLastError = EWR_PATH_INVALID;
			return false;
		}
	}

	return true;
}

bool CWpf::CreateHashFile(const char *strFilePath, const char *strDirPath)
{
	if(!IsOpenWpfFile())
	{
		m_eWrLastError = EWR_FILE_NOT_OPEN;
		return false;
	}

	if (m_bSaving || !strFilePath || !strDirPath)
	{
		m_eWrLastError = EWR_ERROR;
		return false;
	}

	PWpfFileList pList = FindDir(strDirPath);
	if (pList == NULL)
		return false;

	FILE *fp = fopen(strFilePath,"wb");
	if (!fp)
	{
		m_eWrLastError = EWR_PATH_INVALID;
		return false;
	}

	DWORD dwCount = 0;
	
	HashFileHeader header;
	header.dwSize = dwCount;
	fseek(fp,0,SEEK_SET);
	fwrite(&header,sizeof(HashFileHeader),1,fp);	

	vector<PWpfFileList> VDirs;
	VDirs.push_back(pList);

	PWpfFileList pFront = NULL;
	
	while(!VDirs.empty())
	{
		pFront = VDirs.front();

		MList &dirlist = pFront->MDirs;
		MSFCB &filelist = pFront->MFiles;

		//子目录及子文件的fcb
		for (MList::iterator itr = dirlist.begin();itr != dirlist.end(); itr++)
		{		
			//fwrite(&(itr->second->pfcb->pfcb1->iHashKey),sizeof(__int64),1,fp);
			//++dwCount;

			VDirs.push_back(itr->second);
		}

		for(MSFCB::iterator itr = filelist.begin(); itr != filelist.end(); itr++)
		{
			fwrite(&(itr->second->pfcb1->iHashKey),sizeof(__int64),1,fp);
			++dwCount;
		}

		//VDirs.pop_back();
		VDirs.erase(VDirs.begin());
	}


	header.dwSize = dwCount;
	fseek(fp,0,SEEK_SET);
	fwrite(&header,sizeof(HashFileHeader),1,fp);	

	fclose(fp);
	
	return true;

}

bool CWpf::IsBuildTempFileHashEnabled() const
{
	return m_bBuildTempFileHashEnabled;
}

void CWpf::SetBuildTempFileHashEnabled(bool val)
{
	m_bBuildTempFileHashEnabled = val; 
}

const map<__int64,BYTE>& CWpf::GetTempFileHash() const 
{
	return m_kTempFileHash;
}

void CWpf::ClearTempFileHash()
{
	m_kTempFileHash.clear();
}


