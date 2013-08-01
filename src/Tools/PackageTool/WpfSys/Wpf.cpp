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
	//����Ŀ¼
	m_strWpfDir = strPathName;
	replace(m_strWpfDir.begin(),m_strWpfDir.end(),'/','\\');
	size_t iPos = m_strWpfDir.find_last_of("\\");
	if(iPos != string::npos)
	{
		m_strWpfFileName = m_strWpfDir.substr(iPos + 1,m_strWpfDir.length() - iPos - 1);
		m_strWpfDir = m_strWpfDir.substr(0,iPos);
	}

	SHCreateDirectoryEx(NULL,m_strWpfDir.c_str(),NULL);
 
	//�ȹرյ�ǰ�򿪵�wpf�ļ�
	WpfClose();

	//�򿪻򴴽��ļ�
	//m_handle = CreateFile(strPathName,GENERIC_WRITE,FILE_SHARE_READ,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
	errno_t err = _sopen_s(&(m_iFileHandle[EFT_MAIN]), strPathName, _O_BINARY | _O_RDWR | _O_CREAT | _O_TRUNC , _SH_DENYWR,  _S_IREAD | _S_IWRITE );
	if(err != 0)
	{
		m_eWrLastError = EWR_ERROR;
		return false;
	}

	//��ʼ���ļ�ͷ
	m_iWpfStartPos = 0;


	//�ճ�m_WpfHeader.wBytesPerBlock�ı���
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

	//ָ���ļ���С
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

	//��ʽ��
	WpfFormat();

	m_WpfHeader_Bak = m_WpfHeader;//����ͷ


	//�������Ϣ���浽�ļ�
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

	//�ȹرյ�ǰ�򿪵�wpf�ļ�
	WpfClose();

	m_strWpfDir = strPathName;
	replace(m_strWpfDir.begin(),m_strWpfDir.end(),'/','\\');
	size_t iPos = m_strWpfDir.find_last_of("\\");
	if (iPos != string::npos)
	{
		m_strWpfFileName = m_strWpfDir.substr(iPos + 1,m_strWpfDir.length() - iPos - 1);
		m_strWpfDir = m_strWpfDir.substr(0,iPos);
	}
	//�򿪻򴴽��ļ�
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
	m_WpfHeader.dwMagic = 0;//��ֹ��Щ�ļ����Ȳ���sizeof(S_WpfHeader),ȡ��Ĭ��ֵ
	m_WpfHeader_Bak.dwMagic = 0;//��ֹ��Щ�ļ����Ȳ���sizeof(S_WpfHeader),ȡ��Ĭ��ֵ
	m_WpfHeader.bySavingHeader = TRUE;//��ֹ��Щ�ļ����Ȳ���sizeof(S_WpfHeader),ȡ��Ĭ��ֵ
	m_WpfHeader_Bak.bySavingHeader = TRUE;//��ֹ��Щ�ļ����Ȳ���sizeof(S_WpfHeader),ȡ��Ĭ��ֵ

	//�����ļ�ͷ
	_lseeki64(m_iFileHandle[EFT_MAIN],m_iWpfStartPos,SEEK_SET);
	_read(m_iFileHandle[EFT_MAIN],&m_WpfHeader,sizeof(S_WpfHeader));

	//�����ļ�ͷ����
	_lseeki64(m_iFileHandle[EFT_MAIN],m_iWpfStartPos + sizeof(S_WpfHeader),SEEK_SET);
	_read(m_iFileHandle[EFT_MAIN],&m_WpfHeader_Bak,sizeof(S_WpfHeader));

	if(m_WpfHeader.bySavingHeader && m_WpfHeader_Bak.bySavingHeader)//���ļ��ڴ�����ʱ������쳣,��ϵ�,������������,û������д��ͷ
	{
		return false;
	}
	
	//����ϴ��ļ�ͷû����ȷ����,��ȡ�طݵ��ļ�
	if (m_WpfHeader.bySavingHeader)
	{
		m_WpfHeader = m_WpfHeader_Bak;
		m_bModifyed = true;//ԭ��û�б����,���˱��ݵ�,��ô��μ�ʹû���޸�ҲҪ���±���
	}

	m_WpfHeader_Disk = m_WpfHeader;

	if(m_WpfHeader.dwMagic != WPF_MAGIC)
	{
		m_eWrLastError = EWR_NOT_WPF_FILE;
		WpfClose();
		return false;
	}

	m_WpfHeader.bySavingHeader = FALSE;//�����ϲ����ܳ����ļ�ͷ�������ļ�ͷm_WpfHeader.bySavingHeaderͬʱΪTRUE,������һ�±��
	m_dwOpenType = dwOpenType;

	//�����д��ʽ��Ҫ�������п�������Ϣ
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

	//�����ļ������
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
		//����½�Ŀ¼���ļ�ʱ������ڴ�
		NEWDIR *p = m_CreateDirHeader.pNext,*p2 = NULL;
		while(p)
		{
			p2 = p->pNext;
			SAFE_DELETE(p->pList);
			SAFE_DELETE(p);
			p = p2;
		}

		m_pWpfRoot = NULL;
		//�ļ�FCB
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
		//�ͷ�Ŀ¼�ṹ
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

			//��Ŀ¼
			for (MList::iterator itr = dirlist.begin();itr != dirlist.end(); itr++)
			{
				VDirs.push_back(itr->second);
			}

			//�ͷ��ļ��ṹ
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
	//�ر��ļ�
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
	//ȥ������������"\\"
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
	else//��Ŀ¼������,�ݹ鴴��
	{
		//������Ŀ¼
		PWpfFileList parentlist = CreateDir(strParentPath.c_str());
		if(parentlist)//�ڸ�Ŀ¼�д�������Ŀ¼
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
	//ȥ������������"\\"
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
			sprintf(szTemp,"����Ŀ¼ʧ��,%s",strSubDir.c_str());
			MessageBox(NULL,szTemp,"��ʾ",MB_OK|MB_ICONERROR);
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

	//�ٶ�����ʱһ���Դ�ȡ����fcb,���fcb����һ�����ͷ�,���м��¼ӵ�fcb�ü�����,�Ա�����ͷ�
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
			sprintf(szTemp,"ɾ��Ŀ¼ʧ��,%s",strSubDir.c_str());
			MessageBox(NULL,szTemp,"��ʾ",MB_OK|MB_ICONERROR);
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

	//��ɾ��strSubDir����Ŀ¼�����ļ���ɾ��strSubDir
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

		//������ٶ�����Ҫdelete������ڴ�,�ٶ����������رջ򱣴�ʱͳһDelete,����ֻ���
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

	//����ļ��ɹ����ļ�ָ��ͣ������ļ�����ʼλ��
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
	static string strPath = "";//��Ϊ�п���Ҫ������̬��֮��,������static
	string strTemp = "";
	strPath.clear();

	while(pList && pList->pfcb && pList->pfcb->pParent && pList->pfcb->pfcb2)//root ��������ȫ·����
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
			sprintf(szTemp,"����ļ�ʧ��,%s\\%s",strParentDir.c_str(),strFileName.c_str());
			MessageBox(NULL,szTemp,"��ʾ",MB_OK|MB_ICONERROR);
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
	if (dwBlockNum == 0)//����ռ��һ��,���ļ�ռһ��
	{
		dwBlockNum = 1;
	}

	//�ҵ������Ŀհ׿ռ�
	PBlankBlockNode pBlankBlock = NULL;
	int iTimes = 0;
	while(NULL == pBlankBlock && iTimes < 1000)
	{
		pBlankBlock = FindBlankBlock(dwBlockNum);
		//����հ׵Ŀռ䲻������չ��
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


	//�������ļ���д��Ḳ�Ǳ��ݵ�FCB����һ��,Ҫ������������浽�����е�FCB��Ϊ���ݵ�FCB,��д��ǰ��FCB
	if (m_WpfHeader.wHeaderSize + pBlankBlock->blankBlock.dwStart * m_WpfHeader.wBytesPerBlock + dwLen >= m_WpfHeader_Bak.iBlankBlockPos)
	{
		//�����һ�α����û�б���ɹ�,��һ���õñ������Ǳ��ݵ���Ϣ,��ô��Ҫ����֮ǰ�ȱ���һ��,��Ȼ���п��ܻ����m_WpfHeader.bySavingHeader��m_WpfHeader_Bak.bySavingHeaderͬʱΪTRUE�����
		if (m_WpfHeader.bySavingHeader)
		{
			if (WpfSave() == false)
			{
				return false;
			}
		}

		if(!m_WpfHeader_Bak.bySavingHeader)
		{
			//д�뱸��ͷ������FCB��Ҫ���õı��
			_lseeki64(m_iFileHandle[EFT_MAIN],m_iWpfStartPos + sizeof(S_WpfHeader) * 2 - 1,SEEK_SET);
			m_WpfHeader_Bak.bySavingHeader = TRUE;//�����Ǿ���,����Ҫ����������һ��,�´�m_WpfHeader��Ҫ����ʱ,m_WpfHeader_Bak��ת����m_WpfHeader
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

	//�ٶ�����ʱһ���Դ�ȡ����fcb,���fcb����һ�����ͷ�,���м��¼ӵ�fcb�ü�����,�Ա�����ͷ�
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

	//���뵽��ʱhash��
	if (m_bBuildTempFileHashEnabled)
		m_kTempFileHash[pfcb->pfcb1->iHashKey] = 0;

	pList->MFiles[strFileName] = pfcb;
	pList->pfcb->pfcb1->dwSize ++;
	m_WpfHeader.dwFileCount  ++;

	m_MhashFcb[pfcb->pfcb1->iHashKey] = pfcb;


	//���������Ҫɾ��
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
		else//pParentһ������
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

		//���տռ�
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

					//��ǰ���ܲ��ܺϲ�С�Ŀ�ɴ�Ŀ�
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

					//����ܲ��ܺϲ�С�Ŀ�ɴ�Ŀ�
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

		//û������¼�һ��,��֤����,����ϲ��ĵط��ͻ�������
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

		//������ٶ�����Ҫdelete������ڴ�,�ٶ����������رջ򱣴�ʱͳһDelete,����ֻ���
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

	//�����չ��Ҫ�󱣴�,��֤һ����չ�ĳ��ȴ���ԭ��Ŀ¼�ṹ����Ϣ���ݵĳ���,����д��Ŀ¼�ṹ��Ϣʱ���Ḳ���ϵ�,��ʹ�µ�Ŀ¼�ṹ��Ϣ����ʧ��Ҳ���Ḳ��ԭ����Ŀ¼�ṹ����Ϣ
	if (m_bSaveWhenExpended)
	{
		DWORD dwFcbListSize_Disk = m_WpfHeader_Disk.dwBlankBlockSize * sizeof(BlankBlock) + (m_WpfHeader_Disk.dwDirCount + m_WpfHeader_Disk.dwFileCount) * FCB_SIZE;
		dwFcbListSize_Disk += 1024;//��һ��λ��
		
		if (m_WpfHeader_Disk.iBlankBlockPos + dwFcbListSize_Disk > m_WpfHeader.wHeaderSize + m_WpfHeader.iWpfSize + dwLen)
		{
			dwLen = (DWORD)(m_WpfHeader_Disk.iBlankBlockPos + dwFcbListSize_Disk - (m_WpfHeader.wHeaderSize + m_WpfHeader.iWpfSize));
		}		
	}

	//�ճ�m_WpfHeader.wBytesPerBlock�ı���
	if((dwLen % m_WpfHeader.wBytesPerBlock) != 0)
	{
		dwLen += m_WpfHeader.wBytesPerBlock - (dwLen % m_WpfHeader.wBytesPerBlock);
	}
	//ָ���ļ���С
	__int64 iNewWpfSize = m_WpfHeader.iWpfSize + dwLen;
	DWORD dwNewTotalBlocks = DWORD(iNewWpfSize/m_WpfHeader.wBytesPerBlock);
	DWORD dwOldTotalBlocks = m_WpfHeader.dwTotalBlocks;
	DWORD dwNewBlankBlockSize = m_WpfHeader.dwBlankBlockSize;


	//����ܲ��ܺϲ���
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


	//����ռ�
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


	//û�кϲ����¼�һ���ں���
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
		else//һ����m_pBlankBlockListΪ��
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


	//ÿ����չ֮�󶼱���һ��,������������ļ�ϵͳ����
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

	//�ȹرյ�ǰ���ʵ��ļ�
	FileClose();

	
	
	//�������FCB�Ѿ���������ļ�����(m_WpfHeader_Bak.bySavingHeaderΪTRUE),�����ϴ�Ҫ��д����ͷ,ȴû��д�ɹ�,��ô��ǰFCB��Ҫ����,��Ϊ����FCB,�µ�FCBд�ڱ��ݵ�FCB����
	if (m_WpfHeader_Bak.bySavingHeader)
	{
		//д�뱸��ͷ���ڱ���ı��,�����ʱдʧ��,�´λ�ʹ��ԭ�����ļ�ͷ��fcblist,�����´α���ʱ�����µ����������汸��ͷ
		_lseeki64(m_iFileHandle[EFT_MAIN],m_iWpfStartPos + sizeof(S_WpfHeader) * 2 - 1,SEEK_SET);
		m_WpfHeader_Bak.bySavingHeader = TRUE;
		if(_write(m_iFileHandle[EFT_MAIN],&(m_WpfHeader_Bak.bySavingHeader),sizeof(BYTE)) <= 0)
		{
			goto Save_Fail;
		}

		if (!m_WpfHeader_Disk.bySavingHeader)//m_WpfHeader_Disk.bySavingHeaderֻ�иտ�ʼ������ʱ��ΪTRUE,���ʱ��û��FCB���浽����,����û�б�����Ϣ
		{
			m_WpfHeader_Bak = m_WpfHeader_Disk;
			m_WpfHeader_Bak.bySavingHeader = FALSE;
		}
		//else//��һ��û�б�����Ϣ,���´��ٱ����ʱ�������,�ͻ��ٱ��汸����Ϣ,��Ϊ���������m_WpfHeader_Bak.bySavingHeader����TRUE
		//{
		//}

		//д�뱸��ͷ,�����ʱдʧ��,�´λ�ʹ��ԭ�����ļ�ͷ��fcblist,�����´α���ʱ�����µ����������汸��ͷ
		_lseeki64(m_iFileHandle[EFT_MAIN],m_iWpfStartPos + sizeof(S_WpfHeader),SEEK_SET);
		if(_write(m_iFileHandle[EFT_MAIN],&m_WpfHeader_Bak,sizeof(S_WpfHeader)) <= 0)
		{
			goto Save_Fail;
		}
	}

	//���m_WpfHeader_Bakָ���FCB����Ϣ����Ч��,Ӧ�ñ��⸲�ǵ�
	if(!m_WpfHeader_Bak.bySavingHeader)
	{
		//m_WpfHeader_Bak��Ӧ��FCB��С
		DWORD dwFcbListSize_Bak = m_WpfHeader_Bak.dwBlankBlockSize * sizeof(BlankBlock) + (m_WpfHeader_Bak.dwDirCount + m_WpfHeader_Bak.dwFileCount) * FCB_SIZE;
		//���ⱸ��FCB������
		if (m_WpfHeader_Bak.iBlankBlockPos + dwFcbListSize_Bak > m_WpfHeader.iBlankBlockPos)
		{
			m_WpfHeader.iBlankBlockPos = m_WpfHeader_Bak.iBlankBlockPos + dwFcbListSize_Bak;
		}
	}
	
	//���������ļ��ĳ���,�п����м�ɾ�����¼�Ŀ¼,fcblist�Ĵ�С�ͷ����˸ı�,�����ļ��Ĵ�СҲӦ�ñ�С
	DWORD dwFcbSize = m_WpfHeader.dwBlankBlockSize * sizeof(BlankBlock) + (m_WpfHeader.dwDirCount + m_WpfHeader.dwFileCount) * FCB_SIZE;
	//errno_t err = _chsize_s(m_iFileHandle[EFT_MAIN],m_iWpfStartPos + m_WpfHeader.wHeaderSize + m_WpfHeader.iWpfSize + dwFcbSize);
	errno_t err = _chsize_s(m_iFileHandle[EFT_MAIN],m_WpfHeader.iBlankBlockPos + dwFcbSize);
	if(err != 0)
	{
		goto Save_Fail;
	}



	//д�����ڱ���ı��,�����ʱдʧ��,�´λ�ʹ��ԭ�����ļ�ͷ��fcblist
	_lseeki64(m_iFileHandle[EFT_MAIN],m_iWpfStartPos + sizeof(S_WpfHeader) - 1,SEEK_SET);
	m_WpfHeader.bySavingHeader = TRUE;
	if(_write(m_iFileHandle[EFT_MAIN],&(m_WpfHeader.bySavingHeader),sizeof(BYTE)) <= 0)
	{
		goto Save_Fail;
	}


	//д��հ׿���Ϣ,�����ʱдʧ��,�´λ�ʹ�ñ��ݵ��ļ�ͷ��fcblist
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
	
	//д��FCB,�����ʱдʧ��,�´λ�ʹ�ñ��ݵ��ļ�ͷ��fcblist
	if(!WpfWriteFCB())
	{
		goto Save_Fail;
	}


	//д���ļ�ͷ,�����ʱдʧ��,m_WpfHeader.bySavingHeader�ض�ΪTRUE,��Ϊ����ͷ�����һ�ֽ�,Ϊ�˷�ֹͷ��һ��д���µ�,һ�����ϵ�,m_WpfHeader.bySavingHeaderΪTRUEʱ��ȡʱ�ñ��ݵ��ļ�ͷ�ͱ��ݵ�FCB
	m_WpfHeader.bySavingHeader = FALSE;
	_lseeki64(m_iFileHandle[EFT_MAIN],m_iWpfStartPos,SEEK_SET);
	if(_write(m_iFileHandle[EFT_MAIN],&m_WpfHeader,sizeof(S_WpfHeader)) <= 0)
	{
		goto Save_Fail;
	}



	//�ѻ������������������µ�����,�ĳ���ʹ�þ����򿪵��ļ���û��buff��,����Ҫfflush
	//    fflush(m_pFp[EFT_MAIN]);

	m_bSaving = false;
	m_WpfHeader_Disk = m_WpfHeader;//���¼�¼�޸�ǰ״̬

	return true;

	//����ʧ��ʱ,����ر��ļ���д����,���ͷ��FCB��Ϣ���ƻ�,��Ϊ�������չ��ʱ�򱣴�ʧ��,m_pBlankBlockList,m_WpfHeader,m_pWpfRoot����ϢҪ�ع�,��Ҫ�����´򿪲���ȡ��Щ��Ϣ
	//�����ܹر��������,����ⲿ�ĳ���������������ⲿ�����쳣,�������߳��������̵߳Ķ�����
Save_Fail:
	m_bSaving = false;
	m_bModifyed = false;
	m_dwOpenType &= (~EWOT_RDWR);//EWOT_LISTDIR��EWOT_LISTFCBHASH����,�Ա��ⲿ���ж�ȡ����
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
		if (itr->second->pfcb1->dwSize == 0)//dwSizeΪ0˵���ǿ��ļ�
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

	//���Ҫ�������Ŀ¼���ļ�
	if (m_bClearEmptyDirAndFile)
	{
		ClearEmptyDirAndFile(m_pWpfRoot);
	}


	//д��fcb
	DWORD dwCount = m_WpfHeader.dwDirCount + m_WpfHeader.dwFileCount;
	if (m_bSpeedPrior)
	{
		//Ϊ���ٶ�����������һ��ռ�,Ȼ�� һ��д�����
		PFCB1 fcb1 = new FCB1[dwCount];//fcb1�б�
		PFCB2 fcb2 = new FCB2[dwCount];//fcb2�б�


		vector<PWpfFileList> VDirs;
		VDirs.push_back(m_pWpfRoot);
		m_pWpfRoot->pfcb->pfcb1->dwStart = 1;
		m_pWpfRoot->pfcb->pfcb1->dwSize = (DWORD)(m_pWpfRoot->MDirs.size() + m_pWpfRoot->MFiles.size());
		m_pWpfRoot->pfcb->pfcb1->iHashKey = 0;

		memcpy(&fcb1[0],m_pWpfRoot->pfcb->pfcb1,FCB1_SIZE);
		memcpy(&fcb2[0],m_pWpfRoot->pfcb->pfcb2,FCB2_SIZE);


		PWpfFileList pFront = NULL;
		PFCB pfcb = NULL;
		DWORD dwNo = 0;//��Ŀ¼��Ӧfcb��λ��
		DWORD dwStart = 1;//��Ŀ¼��Ŀ¼fcb���ڵ�λ��
		dwStart += m_pWpfRoot->pfcb->pfcb1->dwSize;

		while(!VDirs.empty())
		{
			pFront = VDirs.front();

			MList &dirlist = pFront->MDirs;
			MSFCB &filelist = pFront->MFiles;
			dwNo = pFront->pfcb->pfcb1->dwStart;

			//��Ŀ¼�����ļ���fcb

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
		
		//д���Ŀ¼
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
		DWORD dwNo = 0;//��Ŀ¼��Ӧfcb��λ��
		DWORD dwStart = 1;//��Ŀ¼��Ŀ¼fcb���ڵ�λ��
		dwStart += m_pWpfRoot->pfcb->pfcb1->dwSize;

		while(!VDirs.empty())
		{
			pFront = VDirs.front();

			MList &dirlist = pFront->MDirs;
			MSFCB &filelist = pFront->MFiles;
			dwNo = pFront->pfcb->pfcb1->dwStart;

			//��Ŀ¼�����ļ���fcb
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


		if (m_dwOpenType & EWOT_LISTDIR)//Ҫ����Ŀ¼�ṹ
		{
			m_pFcb2List = new FCB2[dwCount];
			_read(m_iFileHandle[EFT_MAIN],m_pFcb2List,dwCount * FCB2_SIZE);

			m_pFcbList = new FCB[dwCount];
			memset(m_pFcbList,0,dwCount * sizeof(FCB));

			m_pDirList = new WpfFileList[m_WpfHeader.dwDirCount];

			//�ݹ鹹��Ŀ¼
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

		if (m_dwOpenType & EWOT_LISTDIR)//Ҫ����Ŀ¼�ṹ
		{
			//�ݹ鹹��Ŀ¼
			vector<PWpfFileList> VDirs;
			m_pWpfRoot = new WpfFileList();
			m_pWpfRoot->pfcb = new FCB();
			m_pWpfRoot->pfcb->pfcb1 = new FCB1();
			m_pWpfRoot->pfcb->pfcb2 = new FCB2();
			VDirs.push_back(m_pWpfRoot);

			//��ȡ��һ����Ŀ¼
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
					//��ȡ��һ��fcb2
					_lseeki64(m_iFileHandle[EFT_MAIN],m_iWpfStartPos + m_WpfHeader.iFatPos + dwCount * FCB1_SIZE + dwPos * FCB2_SIZE,SEEK_SET);
					_read(m_iFileHandle[EFT_MAIN],fcb2,FCB2_SIZE);

					if(fcb2->dwAttribute & EFA_DIR)
					{
						PFCB newfcb = new FCB();
						newfcb->pfcb1 = new FCB1();
					    //��ȡ��һ��fcb1
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
						//��ȡ��һ��fcb1
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

	//���
	WpfClear();

	m_WpfHeader.dwBlankBlockSize = 1;
	m_WpfHeader.iFatPos = m_WpfHeader.iBlankBlockPos + m_WpfHeader.dwBlankBlockSize * sizeof(BlankBlock);

	m_pBlankBlockList = new BlankBlockNode();
	m_pBlankBlockList->blankBlock.dwStart = 0;
	m_pBlankBlockList->blankBlock.dwSize = m_WpfHeader.dwTotalBlocks;
	//�����Ŀ¼,Ŀ¼��û������
	PFCB pfcb = new FCB();
	pfcb->pfcb1 = new FCB1();
	pfcb->pfcb2 = new FCB2();

	m_pWpfRoot = new WpfFileList;
	m_pWpfRoot->pfcb = pfcb;


	//�ٶ�����ʱһ���Դ�ȡ����fcb,���fcb����һ�����ͷ�,���м��¼ӵ�fcb�ü�����,�Ա�����ͷ�
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
	//����ļ��б�
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

	//ȥ������������"\\"
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

	//����Ŀ¼
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

	//����Ŀ¼
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
				else if (!IsOpenWpfFile() || (m_dwOpenType & EWOT_RDWR) == 0)//�п�����ImportFileʱ��չʧ�ܵ����ļ���д�������ر�
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
			else if (!IsOpenWpfFile() || (m_dwOpenType & EWOT_RDWR) == 0)//�п�����ImportFileʱ��չʧ�ܵ����ļ���д�������ر�
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

	//����Ŀ¼
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

	//�����ļ�
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

		////��Դ�ļ�
		//if (!pWpf->FileOpen(srcfcb->pfcb1))
		//{
		//	continue;
		//}

		////Ϊ���ٶ����ﲻ����ImportFileToDir
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
	//ȥ������������"\\"
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
	//ȥ������������"\\"
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

	//�ȵ�����Ŀ¼
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

	//��λ��fcb����λ��,��һ���Ǹ�Ŀ¼
	FCB1 fcb1;
	FCB2 fcb2;
	DWORD dwCount = m_WpfHeader.dwDirCount + m_WpfHeader.dwFileCount;
	//��ȡ��Ŀ¼
	_lseeki64(m_iFileHandle[EFT_MAIN],m_iWpfStartPos + m_WpfHeader.iFatPos,SEEK_SET);
	_read(m_iFileHandle[EFT_MAIN],&fcb1,FCB1_SIZE);	

	string strPath = strParnetDir;
	string strParentPath = strPath;
	//��ȡ���ļ�����Ŀ¼
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

		//��Ŀ¼�����ļ���fcb
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


