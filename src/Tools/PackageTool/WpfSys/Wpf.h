//*************************************************************************************************
// 内容:	wpf文件系统实现,用于wpf文件读取与写入
//---------------------------------------------------------
// 作者:	
// 创建:		
// 最后修改:
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

#define  MAX_WPF_HASH_BUFFF_SIZE   0x500	///< 用来计算hash值
#define  BLOCK_SIZE 256*1024				///< 分块读写文件一块的大小

//-------------------------------------------------------------------------------------------------
// 新建文件夹信息,用于最后关闭时释放内存
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
// 空闲块描述信息结构
typedef struct S_BlankBlock
{
	DWORD dwStart;	///< 起始位置
	DWORD dwSize;	///< 大小(连续空闲块数量)
}BlankBlock, *PBlankBlock;
//-------------------------------------------------------------------------------------------------
// 空闲块描述信息列表结构
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
	DWORD dwSize;		///< 多少项
	DWORD dwRev;
}HashFileHeader, *PHashFileHeader;
//-------------------------------------------------------------------------------------------------
// wpf文件操作实现
//-------------------------------------------------------------------------------------------------
class CWpf : public CWpfInterface
{
public:
	CWpf(void);
	~CWpf(void);

	// 虚函数继承(继承自CWpfInterface)
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


	// 内部用到的函数,为了效率内部不检查参数合法性,由调用处保证其合法性
	// @{
protected:
	// 初始化buff,用于计算hash值
	bool InitBuffer();

	// 是否打开了wpf文件
	bool IsOpenWpfFile();

	// 格式化,必须先Open(),或Create之后调用,调用后将会清除所有数据
	bool WpfFormat();

	// 无法再分配连续空间时进行空间扩展,以字节为单位
	bool WpfExpend(DWORD dwLen);

	// 查找指定长度的连续空白块,返回块号,失败返回-1(查找方式类似于内存分配的首次适应算法)
	PBlankBlockNode FindBlankBlock(DWORD dwNum);

	// 写入一个目录下所有子文件及子目录的信息
	bool WpfWriteFCB();

	// 载入Fat到m_MWpfFileList,调用时要在打开wpf后,函数内部不再判断
	bool WpfLoadFat();

	// 从列表查找目录
	PWpfFileList FindDirFromList(const string &strDir, PWpfFileList pList);

	// 在指定PWpfFileList中添加子目录
	PWpfFileList CreateDirToList(const string &strSubDir, PWpfFileList pList);

	// 在指定PWpfFileList中递归删除目录及其中的子目录和所有子文件
	bool DelDirFromList(const string &strSubDir, PWpfFileList pList);

	// 在指定PWpfFileList中删除指定文件
	bool DelFileFromList(const string &strFileName, PWpfFileList pList);

	// 把一个文件加入到指定目录
	//	@param[in]	strParentDir	父目录全路径,最后没有"\"
	//	@param[in]	strFileName		文件名
	//	@param[in]	dwLen
	//	@param[in]	pList
	//	@param[in]	bFailExist
	PFCB AddFileToDir(const string &strParentDir, const string &strFileName, DWORD dwLen, PWpfFileList pList, bool bFailExist);
	
	// 计算string的hash值
	DWORD HashString(const char * strFileName,DWORD type);

	// 计算路径的hash值
	__int64 HashFileName(const char * pbKey);

	// 清除申请的内存及状态
	void WpfClear();

	// 清除空白文件和目录
	void ClearEmptyDirAndFile(PWpfFileList pList);
	// @}

protected:
	string m_strWpfFileName;			///< wpf文件名
	string m_strWpfDir;					///< wpf文件目录路径

	NEWDIR m_CreateDirHeader;			///< 速度优先时的临时数据
	NEWFCB m_CreateFcbHeader;			///< 速度优先时的临时数据

	DWORD m_dwExpendLen;				///< 没有空白空间时扩展的长度
    bool m_bSaveWhenExpended;			///< 扩展之后是否立即保存
	bool m_bAlertError;					///< 出错之后是否弹框警告
	bool m_bSpeedPrior;					///< 是否速度优先,否则内存优先
	bool m_bClearEmptyDirAndFile;		///< 是否清除空目录以及空文件
	DWORD m_dwAddAttributeToNewFile;	///< 设置导入文件或目录时所有文件应该额外增加的属性,比如导入某个文件夹时要求添加EFA_MEARGE_UNCOMPRESS属性,而其它文件中的文件不需要这个属性

	E_WPF_RTN m_eWrLastError;			///< 最后一次错误代码
	WpfHeader m_WpfHeader;				///< 文件头
	WpfHeader m_WpfHeader_Bak;			///< 文件头备份
	WpfHeader m_WpfHeader_Disk;			///< 已经保存到磁盘中的文件头,m_WpfHeader随时会发生改变,如果备份头指向的FCB被文件覆盖,m_WpfHeader_Bak应该变成m_WpfHeader_Disk,这时如果写m_WpfHeader_Bak失败,m_WpfHeader_Disk始终是有效的,下次保存时会要求重写m_WpfHeader_Bak
	PWpfFileList m_pWpfRoot;			///< 根目录
	PWpfFileList m_pDirList;			///< 目录列表
	int m_iFileHandle[EFT_NUM];			///< 文件句柄
	PBlankBlockNode m_pBlankBlockList;	///< 空闲块描述列表

	PFCB1 m_pOpenFileFcb1[EFT_NUM];		///< 当前访问文件的FCB1
	DWORD m_iOpenFilePointPos[EFT_NUM];	///< 当前访问文件的文件指针位置,相对于当前打开文件的起始位置

	bool m_bModifyed;					///< 是否对wpf文件进行了修改操作,如果有在最后要自动保存
	bool m_bSaving;						///< 是否正在保存,如果正在保存,不允许其它地方进行写操作,为了速度设计上不允许多个线程同时进行写操作,否则可能破坏目录结构
	__int64 m_iWpfStartPos;				///< wpf文件内容在整个打开文件中的起始位置
	DWORD m_dwOpenType;					///< 打开文件的类型
	MIFCB1 m_MhashFcb1;					///< 各文件对应的hash值map,只读操作时用到,编辑操作用m_pDirList
	MIFCB m_MhashFcb;					///< 各文件对应的hash值到fcb的map,EWOT_LISTDIR及EWOT_LISTFCBHASH时用到
	PFCB1 m_pFcb1List;					///< fcb1列表
	PFCB2 m_pFcb2List;					///< fcb1列表
	PFCB m_pFcbList;					///< fcb列表
	bool m_bMeargeNotNeedUnCopress;		///< 合并是否要解压,true不解压

	bool m_bBuildTempFileHashEnabled;	///< 是否生成临时hash表
	map<__int64, BYTE> m_kTempFileHash;	///< 临时hash表

	static DWORD m_dwCryptTable[MAX_WPF_HASH_BUFFF_SIZE];	///< 用来计算hash值
};
//-------------------------------------------------------------------------------------------------