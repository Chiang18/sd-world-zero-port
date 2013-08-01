//*************************************************************************************************
// ����:	wpf�ļ�ϵͳʵ��,����wpf�ļ���ȡ��д��
//---------------------------------------------------------
// ����:	
// ����:		
// ����޸�:
//*************************************************************************************************
#pragma once
#include "wpfinterface.h"

#define  FCB1_SIZE	sizeof(S_Fcb1)
#define  FCB2_SIZE	sizeof(S_Fcb2)
#define  FCB_SIZE	(FCB1_SIZE + FCB2_SIZE)

#ifndef  SAFE_DELETE
#define  SAFE_DELETE(p)         {if(p){delete (p);(p)=NULL;}}
#endif

#ifndef  SAFE_DELETE_ARRAY
#define  SAFE_DELETE_ARRAY(p)   {if(p){delete [] (p);(p)=NULL;}}
#endif

#define  MAX_WPF_HASH_BUFFF_SIZE   0x500	///< ��������hashֵ
#define  BLOCK_SIZE 256*1024				///< �ֿ��д�ļ�һ��Ĵ�С

//-------------------------------------------------------------------------------------------------
// �½��ļ�����Ϣ,�������ر�ʱ�ͷ��ڴ�
typedef struct NEW_DIR
{
	NEW_DIR()
	{
		pList = NULL;
		pNext = NULL;
	}

	PWpfFileList pList;
	NEW_DIR * pNext;
}NEWDIR;
//-------------------------------------------------------------------------------------------------
typedef struct NEW_FCB
{
	NEW_FCB()
	{
		pfcb = NULL;
		pNext = NULL;
	}

	PFCB pfcb;
	NEW_FCB * pNext;
}NEWFCB;
//-------------------------------------------------------------------------------------------------
// ���п�������Ϣ�ṹ
typedef struct S_BlankBlock
{
	DWORD dwStart;	///< ��ʼλ��
	DWORD dwSize;	///< ��С(�������п�����)
}BlankBlock, *PBlankBlock;
//-------------------------------------------------------------------------------------------------
// ���п�������Ϣ�б�ṹ
typedef struct S_BlankBlockNode
{
	S_BlankBlockNode()
	{
		blankBlock.dwStart = 0;
		blankBlock.dwSize = 0;
		pParent = NULL;
		pNext = NULL;
	}

	S_BlankBlock  blankBlock;
	S_BlankBlockNode* pParent;
	S_BlankBlockNode* pNext;
}BlankBlockNode, *PBlankBlockNode;
//-------------------------------------------------------------------------------------------------
typedef struct S_HashFileHeader
{
	S_HashFileHeader()
	{
		dwMagic = 'hash';
		dwVerson = 0x01;
		dwSize = 0;
		dwRev = 0;
	}

	DWORD dwMagic;
	DWORD dwVerson;
	DWORD dwSize;		///< ������
	DWORD dwRev;
}HashFileHeader, *PHashFileHeader;
//-------------------------------------------------------------------------------------------------
// wpf�ļ�����ʵ��
//-------------------------------------------------------------------------------------------------
class CWpf : public CWpfInterface
{
public:
	CWpf(void);
	~CWpf(void);

	// �麯���̳�(�̳���CWpfInterface)
	// @{
public:
	virtual bool WpfCreate(const char* strPathName, __int64 iSize = 1*1024*1024, bool bFailExist = true);
	virtual bool WpfOpen(const char* strPathName, __int64 iOffset = 0, DWORD dwOpenType = EWOT_READONLY);
	virtual void WpfClose();

	virtual E_WPF_RTN GetLastError();
	virtual PWpfFileList GetRoot();
	virtual WpfHeader *GetHeader();
	virtual bool SetExpendLength(DWORD dwLen = 10*1024*1024);
	virtual void SetSaveWhenExpended(bool bSave);
	virtual void SetAlertError(bool bAlert);

	virtual bool SetWpfType(DWORD dwType);
	virtual DWORD GetWpfType();

	virtual void SetSpeedPrior(bool bSpeedPrior);
	virtual bool IsSpeedPrior();

	virtual void SetClearEmptyDirAndFile(bool bClearEmptyDirAndFile);
	virtual bool IsClearEmptyDirAndFile();

	virtual void SetAddAttributeToNewFile(DWORD dwAddAttribute = 0);
	virtual DWORD GetAddAttributeToNewFile();
	
	virtual const char* GetWpfFileDir();
	virtual const char* GetWpfFileName();

	virtual void SetMeargeNotNeedUnCopress(bool val);
	virtual bool GetMeargeNotNeedUnCopress();

	virtual PWpfFileList CreateDir(const char *strPathName);
	virtual bool DeleteDir(const char* strPathName);
	virtual PWpfFileList FindDir(const char* strPathName);

	virtual PFCB CreateFile(const char* strPathName, DWORD dwLen, bool bFailExist = true);
	virtual PFCB CreateFileFromBuff(const char* strPathName, const char* buff, DWORD dwLen, bool bFailExist = true, bool bCompress = false, bool bUnCompress = false);
	virtual PFCB FindFile_PFCB(const char* strPathName);
	virtual PFCB1 FindFile_PFCB1(const char* strPathName);
	virtual PFCB1 FindFile_PFCB1(__int64 iHash);
	virtual PFCB FileOpenEx(const char* strPathName, E_FPTYPE fpType = EFT_MAIN);
	virtual bool DeleteFile(const char* strPathName);
	virtual bool UpdateFile(const char* strPathName, const char* buff, DWORD dwLen, bool bCompress = false, bool bUnCompress = false);
	virtual PFCB1 FileOpen(const char* strPathName, E_FPTYPE fpType = EFT_MAIN);
	virtual PFCB1 FileOpen_Hash2FCB1(__int64 iHash, E_FPTYPE fpType = EFT_MAIN);
	virtual PFCB FileOpen_Hash2FCB(__int64 iHash, E_FPTYPE fpType = EFT_MAIN);
	virtual bool FileOpen(PFCB1 pfcb1, E_FPTYPE fpType = EFT_MAIN);
	virtual bool FileClose(E_FPTYPE fpType = EFT_MAIN);
	virtual bool FileSeek(DWORD dwOffset, int iOrigin = SEEK_SET, E_FPTYPE fpType = EFT_MAIN);
	virtual DWORD FileRead(void* pBbuff, DWORD dwSize, E_FPTYPE fpType = EFT_MAIN);
	virtual DWORD FileWrite(const void* pBbuff, DWORD dwSize, E_FPTYPE fpType = EFT_MAIN);
	virtual DWORD FileLength(E_FPTYPE fpType = EFT_MAIN);
	virtual __int64 GetFileOffset(E_FPTYPE fpType = EFT_MAIN);
	virtual __int64 GetFileOffset(const char* strPathName, E_FPTYPE fpType = EFT_MAIN);

	virtual int GetFileHandle(E_FPTYPE fpType);

	virtual __int64 WpfLength();
	virtual __int64 WpfSeek(__int64 dwOffset = 0, int iOrigin = SEEK_SET);
	virtual DWORD WpfRead(void* pBbuff, DWORD dwSize);
	virtual DWORD WpfWrite(const void* pBbuff, DWORD dwSize);
	virtual bool WpfSave();
	virtual bool MeargeWpf(const char* strSrcPath,__int64 iOffset = 0,LPWpfCallBack pCallBack = NULL);

	virtual PFCB ImportFile(const char* strParnetDir, const char* strFilePath, bool bCompress, bool bFailExist = true);
	virtual PFCB ImportFileToDir(CWpfInterface* pWpf, PFCB pfcb, PWpfFileList pList, bool bFailExist = true);
	virtual PWpfFileList ImportDir(const char* strParnetDir, const char* strDirPath, bool bCompress ,bool bFailExist = true, LPWpfCallBack pCallBack = NULL);
	virtual PWpfFileList ImportSubDir(const char* strParnetDir, const char* strDirPath, bool bCompress, bool bFailExist = true, LPWpfCallBack pCallBack = NULL);
	virtual bool ImportDirFromList(CWpfInterface* pWpf, PWpfFileList pSrcList, PWpfFileList pDestList, bool bFailExist = true, LPWpfCallBack pCallBack = NULL);
	virtual bool ImportSubDirFromList(CWpfInterface * pWpf, PWpfFileList pSrcList, PWpfFileList pDestList, bool bFailExist = true, LPWpfCallBack pCallBack = NULL);
	
	virtual bool ExportFile(const char* strSrcPath, const char* strDestPath);
	virtual bool ExportFile(PFCB pfcb, const char *strDestPath);
	virtual bool ExportDir(const char* strSrcPath, const char* strDestPath, LPWpfCallBack pCallBack = NULL);
	virtual bool ExportSubDir(const char* strSrcPath, const char* strDestPath, LPWpfCallBack pCallBack = NULL);
	virtual void ExportDirFromList(const string& strDestPath, PWpfFileList pList, LPWpfCallBack pCallBack = NULL);
	virtual void ExportSubDirFromList(const string& strDestPath, PWpfFileList pList, LPWpfCallBack pCallBack = NULL);
	virtual bool ExportAllFileFromFCB1(const char* strDestPath);

	virtual bool CleanUpFragment();
	virtual bool GetSubFileName(const char* strParnetDir,LPWpfCallBack pCallBack,bool bSubDir = false);
	virtual bool CreateHashFile(const char* strFilePath, const char* strDirPath = "");
	virtual const char* GetDirFullPath(PWpfFileList pList);
	virtual __int64 GetHashKey(const char* str);

	virtual bool IsBuildTempFileHashEnabled() const;
	virtual void SetBuildTempFileHashEnabled(bool val);
	virtual const map<__int64,BYTE>& GetTempFileHash() const;
	virtual void ClearTempFileHash();
	// @}


	// �ڲ��õ��ĺ���,Ϊ��Ч���ڲ����������Ϸ���,�ɵ��ô���֤��Ϸ���
	// @{
protected:
	// ��ʼ��buff,���ڼ���hashֵ
	bool InitBuffer();

	// �Ƿ����wpf�ļ�
	bool IsOpenWpfFile();

	// ��ʽ��,������Open(),��Create֮�����,���ú󽫻������������
	bool WpfFormat();

	// �޷��ٷ��������ռ�ʱ���пռ���չ,���ֽ�Ϊ��λ
	bool WpfExpend(DWORD dwLen);

	// ����ָ�����ȵ������հ׿�,���ؿ��,ʧ�ܷ���-1(���ҷ�ʽ�������ڴ������״���Ӧ�㷨)
	PBlankBlockNode FindBlankBlock(DWORD dwNum);

	// д��һ��Ŀ¼���������ļ�����Ŀ¼����Ϣ
	bool WpfWriteFCB();

	// ����Fat��m_MWpfFileList,����ʱҪ�ڴ�wpf��,�����ڲ������ж�
	bool WpfLoadFat();

	// ���б����Ŀ¼
	PWpfFileList FindDirFromList(const string &strDir, PWpfFileList pList);

	// ��ָ��PWpfFileList�������Ŀ¼
	PWpfFileList CreateDirToList(const string &strSubDir, PWpfFileList pList);

	// ��ָ��PWpfFileList�еݹ�ɾ��Ŀ¼�����е���Ŀ¼���������ļ�
	bool DelDirFromList(const string &strSubDir, PWpfFileList pList);

	// ��ָ��PWpfFileList��ɾ��ָ���ļ�
	bool DelFileFromList(const string &strFileName, PWpfFileList pList);

	// ��һ���ļ����뵽ָ��Ŀ¼
	//	@param[in]	strParentDir	��Ŀ¼ȫ·��,���û��"\"
	//	@param[in]	strFileName		�ļ���
	//	@param[in]	dwLen
	//	@param[in]	pList
	//	@param[in]	bFailExist
	PFCB AddFileToDir(const string &strParentDir, const string &strFileName, DWORD dwLen, PWpfFileList pList, bool bFailExist);
	
	// ����string��hashֵ
	DWORD HashString(const char * strFileName,DWORD type);

	// ����·����hashֵ
	__int64 HashFileName(const char * pbKey);

	// ���������ڴ漰״̬
	void WpfClear();

	// ����հ��ļ���Ŀ¼
	void ClearEmptyDirAndFile(PWpfFileList pList);
	// @}

protected:
	string m_strWpfFileName;			///< wpf�ļ���
	string m_strWpfDir;					///< wpf�ļ�Ŀ¼·��

	NEWDIR m_CreateDirHeader;			///< �ٶ�����ʱ����ʱ����
	NEWFCB m_CreateFcbHeader;			///< �ٶ�����ʱ����ʱ����

	DWORD m_dwExpendLen;				///< û�пհ׿ռ�ʱ��չ�ĳ���
    bool m_bSaveWhenExpended;			///< ��չ֮���Ƿ���������
	bool m_bAlertError;					///< ����֮���Ƿ񵯿򾯸�
	bool m_bSpeedPrior;					///< �Ƿ��ٶ�����,�����ڴ�����
	bool m_bClearEmptyDirAndFile;		///< �Ƿ������Ŀ¼�Լ����ļ�
	DWORD m_dwAddAttributeToNewFile;	///< ���õ����ļ���Ŀ¼ʱ�����ļ�Ӧ�ö������ӵ�����,���絼��ĳ���ļ���ʱҪ�����EFA_MEARGE_UNCOMPRESS����,�������ļ��е��ļ�����Ҫ�������

	E_WPF_RTN m_eWrLastError;			///< ���һ�δ������
	WpfHeader m_WpfHeader;				///< �ļ�ͷ
	WpfHeader m_WpfHeader_Bak;			///< �ļ�ͷ����
	WpfHeader m_WpfHeader_Disk;			///< �Ѿ����浽�����е��ļ�ͷ,m_WpfHeader��ʱ�ᷢ���ı�,�������ͷָ���FCB���ļ�����,m_WpfHeader_BakӦ�ñ��m_WpfHeader_Disk,��ʱ���дm_WpfHeader_Bakʧ��,m_WpfHeader_Diskʼ������Ч��,�´α���ʱ��Ҫ����дm_WpfHeader_Bak
	PWpfFileList m_pWpfRoot;			///< ��Ŀ¼
	PWpfFileList m_pDirList;			///< Ŀ¼�б�
	int m_iFileHandle[EFT_NUM];			///< �ļ����
	PBlankBlockNode m_pBlankBlockList;	///< ���п������б�

	PFCB1 m_pOpenFileFcb1[EFT_NUM];		///< ��ǰ�����ļ���FCB1
	DWORD m_iOpenFilePointPos[EFT_NUM];	///< ��ǰ�����ļ����ļ�ָ��λ��,����ڵ�ǰ���ļ�����ʼλ��

	bool m_bModifyed;					///< �Ƿ��wpf�ļ��������޸Ĳ���,����������Ҫ�Զ�����
	bool m_bSaving;						///< �Ƿ����ڱ���,������ڱ���,�����������ط�����д����,Ϊ���ٶ�����ϲ��������߳�ͬʱ����д����,��������ƻ�Ŀ¼�ṹ
	__int64 m_iWpfStartPos;				///< wpf�ļ��������������ļ��е���ʼλ��
	DWORD m_dwOpenType;					///< ���ļ�������
	MIFCB1 m_MhashFcb1;					///< ���ļ���Ӧ��hashֵmap,ֻ������ʱ�õ�,�༭������m_pDirList
	MIFCB m_MhashFcb;					///< ���ļ���Ӧ��hashֵ��fcb��map,EWOT_LISTDIR��EWOT_LISTFCBHASHʱ�õ�
	PFCB1 m_pFcb1List;					///< fcb1�б�
	PFCB2 m_pFcb2List;					///< fcb1�б�
	PFCB m_pFcbList;					///< fcb�б�
	bool m_bMeargeNotNeedUnCopress;		///< �ϲ��Ƿ�Ҫ��ѹ,true����ѹ

	bool m_bBuildTempFileHashEnabled;	///< �Ƿ�������ʱhash��
	map<__int64, BYTE> m_kTempFileHash;	///< ��ʱhash��

	static DWORD m_dwCryptTable[MAX_WPF_HASH_BUFFF_SIZE];	///< ��������hashֵ
};
//-------------------------------------------------------------------------------------------------