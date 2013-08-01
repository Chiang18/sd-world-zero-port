#pragma once

#include "wpfinterface.h"


#define  FCB1_SIZE           sizeof(S_Fcb1)
#define  FCB2_SIZE           sizeof(S_Fcb2)
#define  FCB_SIZE            (FCB1_SIZE + FCB2_SIZE)


#ifndef  SAFE_DELETE
#define  SAFE_DELETE(p)         {if(p){delete (p);(p)=NULL;}}
#endif
#ifndef  SAFE_DELETE_ARRAY
#define  SAFE_DELETE_ARRAY(p)   {if(p){delete [] (p);(p)=NULL;}}
#endif


#define  MAX_WPF_HASH_BUFFF_SIZE   0x500 //用来计算hash值
#define  BLOCK_SIZE 256*1024   //分块读写文件一块的大小




//新建文件夹信息,用于最后关闭时释放内存
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
//空闲块描述信息结构
typedef struct S_BlankBlock
{
	DWORD dwStart;//起始位置
	DWORD dwSize;//大小
}BlankBlock,*PBlankBlock;

//空闲块描述信息列表结构
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
}BlankBlockNode,*PBlankBlockNode;


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
	DWORD dwSize;//多少项
	DWORD dwRev;
}HashFileHeader,*PHashFileHeader;



class CWpf : public CWpfInterface
{
public:
	CWpf(void);
public:
	~CWpf(void);

public:
	//创建一个新的Wpf文件,strPathName:全路径名,bFailExist:文件存在是否返回失败,如果该值为false,文件存在就删除原来的,
	//创建后文件初始大小默认为1M(不包含文件头等),创建后会进行格式化
	virtual bool WpfCreate(const char * strPathName,__int64 iSize = 1*1024*1024,bool bFailExist = true);
    //打开一个Wpf文件,其中的目录及文件信息保存到一个MWpfFileList结构,可以通过GetRoot获得该
	//iOffset:wpf文件内容在strPathName这个文件中的起始位置,如果整个文件就是一个wpf文件,取0
	virtual bool WpfOpen(const char * strPathName,__int64 iOffset = 0,DWORD dwOpenType = EWOT_READONLY);
	//关闭文件,清除文件列表
	virtual void WpfClose();
	//获得最后一次错误代码
	virtual E_WPF_RTN GetLastError();
	//获得指定wpf文件的文件列表map,Open后有效
	virtual PWpfFileList GetRoot();
	//获得文件头
	virtual WpfHeader *GetHeader();
	//设置空间不够时每次扩展多少字节
	virtual bool SetExpendLength(DWORD dwLen = 10*1024*1024);
	//空间不足扩展之后是否马上保存目录结构,为了安全安装的时候应该保存,为了防止频繁保存,安装的时候ExpendLength应该设置得大一点,打包的时候为了速度及防止打出来的包太大,可以把ExpendLength设置得少一点
	virtual void SetSaveWhenExpended(bool bSave);
	//设置是否弹框提示错误,打包的时候要提示，安装的时候不提示
	virtual void SetAlertError(bool bAlert);
	//设置wpf的类型,比如多个游戏用到wpf文件格式,放升级包的时候防止放错了把其它游戏的文件包打进来了,就可以根据dwWpfType来区分,每个游戏的类型保证不一样,至少升级包保证不一样.
	virtual bool SetWpfType(DWORD dwType);
	//获得wpf的类型,比如多个游戏用到wpf文件格式,放升级包的时候防止放错了把其它游戏的文件包打进来了,就可以根据dwWpfType来区分,每个游戏的类型保证不一样,至少升级包保证不一样.失败返回-1,比如没有打开wpf文件
	virtual DWORD GetWpfType();
	//设置是否速度优先,速度优先的话读写fcb的时候都是所有文件的fcb一次性连续读写,否则为内存优先,单条读写
	virtual void SetSpeedPrior(bool bSpeedPrior);
	//是否速度优先
	virtual bool IsSpeedPrior();
	//设置是否清除空白文件及目录,默认不清除
	virtual void SetClearEmptyDirAndFile(bool bClearEmptyDirAndFile);
	//是否清除空白文件及目录,默认不清除
	virtual bool IsClearEmptyDirAndFile();
	//设置导入文件或目录时所有文件应该额外增加的属性,比如导入某个文件夹时要求添加EFA_MEARGE_UNCOMPRESS属性,而其它文件中的文件不需要这个属性
	virtual void SetAddAttributeToNewFile(DWORD dwAddAttribute = 0);
	//获得新加文件时额外添加的属性
	virtual DWORD GetAddAttributeToNewFile();
	//获取wpf文件路径
	virtual const char* GetWpfFileDir();
	//获取wpf文件名
	virtual const char* GetWpfFileName();
	//合并是否解压
	virtual void SetMeargeNotNeedUnCopress(bool val);
	//合并是否解压
	virtual bool GetMeargeNotNeedUnCopress();


	//创建目录
	virtual PWpfFileList CreateDir(const char *strPathName);
	//递归删除指定目录下所有文件及子目录
	virtual bool DeleteDir(const char * strPathName);
	//查找目录
	virtual PWpfFileList FindDir(const char * strPathName);
	//创建一个文件,dwLen:文件长度,bFailExist:文件存在是否返回失败,如果该值为false,文件存在就删除原来的,添加成功后会自动打开该文件并把文件指针停在文件开始位置
	virtual PFCB CreateFile(const char * strPathName,DWORD dwLen,bool bFailExist = true);
	//从buf中添加一个文件
	virtual PFCB CreateFileFromBuff(const char * strPathName,const char * buff,DWORD dwLen,bool bFailExist = true,bool bCompress = false,bool bUnCompress = false);//bCompress:导入前是否要先压缩,bUnCompress:导入前是否先解压
	//查找文件,目录分隔符只能用"\",不能用,"/",因为效率问题,内部不做转换,EWOT_LISTDIR方式下有效
	virtual PFCB FindFile_PFCB(const char * strPathName);
	//查找文件,目录分隔符只能用"\",不能用,"/",因为效率问题,内部不做转换
	virtual PFCB1 FindFile_PFCB1(const char * strPathName);
	//查找文件,包含EWOT_LISTHASH或(EWOT_LISTDIR & EWOT_LISTFCBHASH)打开属性时有效
	virtual PFCB1 FindFile_PFCB1(__int64 iHash);
	//打开文件,以EWOT_RDWR方式打开wfp时有效
	virtual PFCB FileOpenEx(const char * strPathName,E_FPTYPE fpType = EFT_MAIN);
	//删除文件
	virtual bool DeleteFile(const char * strPathName);
	//更新文件内容,因为连续分配的原因,其实就是先删除再添加
	virtual bool UpdateFile(const char * strPathName,const char *buff,DWORD dwLen,bool bCompress = false,bool bUnCompress = false);//bCompress:导入前是否要先压缩,bUnCompress:导入前是否先解压


	//操作一个文件之前要先打开这个文件,像Seek,Read等,就想打个磁盘中的一个文件一样,打开以后文件指针停在这个文件的起始位置
	//同一时刻只能操作一个文件,如果要多个线程操作同一个wpf文件可以通过不同的文件指针,见E_FPTYPE,一个对象不能同时处理多个wpf图包中的文件,要处理多个自行使用多个对象,在外部管理
	//FileWrite函数不会自动增大文件,文件的大小在创建时就定了,如果超过文件的最大长度写到文件结束为止
	//打开一个文件,如果之前打开了其它文件会自动关闭原来的文件,打开一个新文件之前可以不调用FileClose(),目录分隔符只能用"\",不能用,"/",因为效率问题,内部不做转换
	virtual PFCB1 FileOpen(const char * strPathName,E_FPTYPE fpType = EFT_MAIN);
	//打开文件,根据hash值访问,返回PFCB1,EWOT_LISTHASH方式下有效
	virtual PFCB1 FileOpen_Hash2FCB1(__int64 iHash,E_FPTYPE fpType = EFT_MAIN);
	//打开文件,根据hash值访问,返回PFCB,EWOT_LISTDIR以及EWOT_LISTFCBHASH方式下有效
	virtual PFCB FileOpen_Hash2FCB(__int64 iHash,E_FPTYPE fpType = EFT_MAIN);
	//打开文件
	virtual bool FileOpen(PFCB1 pfcb1,E_FPTYPE fpType = EFT_MAIN);
	//关闭当前打开的文件
	virtual bool FileClose(E_FPTYPE fpType = EFT_MAIN);
	//在当前打开的文件内寻址,SEEK_END时,dwOffset也取正值,表示的位置为FileLength - dwOffset
	virtual bool FileSeek(DWORD dwOffset,int iOrigin = SEEK_SET,E_FPTYPE fpType = EFT_MAIN);
	//读取文件 返回 <= 0 表示失败
	virtual DWORD FileRead(void* pBbuff,DWORD dwSize,E_FPTYPE fpType = EFT_MAIN);
	//写文件 返回 <= 0 表示失败
	virtual DWORD FileWrite(const void* pBbuff,DWORD dwSize,E_FPTYPE fpType = EFT_MAIN);
	//获得文件长度
	virtual DWORD FileLength(E_FPTYPE fpType = EFT_MAIN);
	//获得打开文件相对文件头的偏移,如果没有打开文件返回-1
	virtual __int64 GetFileOffset(E_FPTYPE fpType = EFT_MAIN);
	//先打开文件,如果打开成功,获得打开文件相对文件头的偏移,否则返回-1
	virtual __int64 GetFileOffset(const char * strPathName,E_FPTYPE fpType = EFT_MAIN);


	//扩展函数,WpfSeek,WpfRead,WpfWrite都会导致FileClose(),关闭当前操作的函数
	virtual int GetFileHandle(E_FPTYPE fpType);//不可以在dll和exe之间传递FILE*,除非都是MTD,如果要外管理文件指针,可以在外部打开,然后通过相关接口获得offset再在外部处理
	virtual __int64 WpfLength();
	virtual __int64 WpfSeek(__int64 dwOffset = 0,int iOrigin = SEEK_SET);
	virtual DWORD WpfRead(void* pBbuff,DWORD dwSize);// 返回 <= 0 表示失败
	virtual DWORD WpfWrite(const void* pBbuff,DWORD dwSize);// 返回 <= 0 表示失败

	virtual PFCB ImportFile(const char *strParnetDir,const char *strFilePath,bool bCompress,bool bFailExist = true);//导入一个文件,如果strParnetDir为NULL或""都导到根目录
	virtual PFCB ImportFileToDir(CWpfInterface * pWpf,PFCB pfcb,PWpfFileList pList,bool bFailExist = true);//把pWpf中的文件pfcb复制到些wpf的指定目录plist中
	virtual PWpfFileList ImportDir(const char *strParnetDir,const char *strDirPath,bool bCompress,bool bFailExist = true,LPWpfCallBack pCallBack = NULL);//递归导入一个目录及其子文件到paretntpatah,包含strDirPath本身,
	virtual PWpfFileList ImportSubDir(const char *strParnetDir,const char *strDirPath,bool bCompress,bool bFailExist = true,LPWpfCallBack pCallBack = NULL);//递归导入一个目录及其子文件paretntpatah,不包含strDirPath本身,
	virtual bool ImportDirFromList(CWpfInterface * pWpf,PWpfFileList pSrcList,PWpfFileList pDestList,bool bFailExist = true,LPWpfCallBack pCallBack = NULL);//把另一个wpf文件中的指定目录导入到这个wpf中,包含pSrcList这个目录本身
	virtual bool ImportSubDirFromList(CWpfInterface * pWpf,PWpfFileList pSrcList,PWpfFileList pDestList,bool bFailExist = true,LPWpfCallBack pCallBack = NULL);//把另一个wpf文件中的指定目录导入到这个wpf中,不包含pSrcList这个目录本身
	virtual bool ExportFile(const char *strSrcPath,const char *strDestPath);//导出指定文件到目录文件,参数为全路径名,包括文件名及扩展名
	virtual bool ExportFile(PFCB pfcb,const char *strDestPath);//导出指定文件到目录文件,参数为全路径名,包括文件名及扩展名
	virtual bool ExportDir(const char *strSrcPath,const char *strDestPath,LPWpfCallBack pCallBack = NULL);//导出目录,包括strSrcPath本身
	virtual bool ExportSubDir(const char *strSrcPath,const char *strDestPath,LPWpfCallBack pCallBack = NULL);//导出目录,不包括strSrcPath本身
	virtual void ExportDirFromList(const string &strDestPath,PWpfFileList pList,LPWpfCallBack pCallBack = NULL);//导出目录,包括strSrcPath本身
	virtual void ExportSubDirFromList(const string &strDestPath,PWpfFileList pList,LPWpfCallBack pCallBack = NULL);//导出目录,不包括strSrcPath本身
	virtual bool ExportAllFileFromFCB1(const char *strDestPath);

	virtual bool MeargeWpf(const char *strSrcPath,__int64 iOffset = 0,LPWpfCallBack pCallBack = NULL);//合并wpf,里面如果有同名文件则会覆盖本图包中的文件,iOffset源wpf在strSrcPath这个文件中的起始位置,如果这个文件是一个纯粹的wpf,为0
	virtual bool WpfSave();//保存wpf,保存文件头,字位影像图,文件分配表,对打开的wpf文件有效,如果不保存,到最后关闭的时候才会保存,会导致FileClose()操作
	virtual bool CleanUpFragment();//碎片整理
	virtual bool GetSubFileName(const char *strParnetDir,LPWpfCallBack pCallBack,bool bSubDir = false);//获得指定路径下的子文件或子目录,只读方式下使用,非只读方式下请使用FindDir接口,pCallBack为回调函数,每找到一个文件或子目录会以文件或目录为参数调用该函数,动态库与应用程序之前不能直接传递vector之类参数,bSubDir: false为取子文件,true为取子目录
	virtual bool CreateHashFile(const char *strFilePath, const char *strDirPath = "");//生成hash文件,把整个hash表保存到一个文件,同时有文件头
	virtual const char * GetDirFullPath(PWpfFileList pList);//获得目录的全路径名
	virtual __int64 GetHashKey(const char *str);//得到str的hash值

	//临时hash表
	virtual bool IsBuildTempFileHashEnabled() const;
	virtual void SetBuildTempFileHashEnabled(bool val);
	virtual const map<__int64,BYTE>& GetTempFileHash() const;
	virtual void ClearTempFileHash();


//内部用到的函数,为了效率内部不检查参数合法性,由调用处保证其合法性
protected:
	//初始化buff,用于计算hash值
	bool InitBuffer();
	//是否打开了wpf文件
	bool IsOpenWpfFile();
	//格式化,必须先Open(),或Create之后调用,调用后将会清除所有数据
	bool WpfFormat();
	//无法再分配连续空间时进行空间扩展,以字节为单位
	bool WpfExpend(DWORD dwLen);
	//查找指定长度的连续空白块,返回块号,失败返回-1;
	PBlankBlockNode FindBlankBlock(DWORD dwNum);
	//写入一个目录下所有子文件及子目录的信息
	bool WpfWriteFCB();
	//载入Fat到m_MWpfFileList,调用时要在打开wpf后,函数内部不再判断
	bool WpfLoadFat();
	//从列表查找目录
	PWpfFileList FindDirFromList(const string &strDir,PWpfFileList pList);
	//在指定PWpfFileList中添加子目录
	PWpfFileList CreateDirToList(const string &strSubDir,PWpfFileList pList);
	//在指定PWpfFileList中递归删除目录及其中的子目录和所有子文件
	bool DelDirFromList(const string &strSubDir,PWpfFileList pList);
	//在指定PWpfFileList中删除指定文件
	bool DelFileFromList(const string &strFileName,PWpfFileList pList);
	//把一个文件加入到指定目录,strParentDir:父目录全路径,最后没有"\",
	PFCB AddFileToDir(const string &strParentDir,const string &strFileName,DWORD dwLen,PWpfFileList pList,bool bFailExist);
	//计算string的hash值
	DWORD HashString(const char * strFileName,DWORD type);
	//计算路径的hash值
	__int64 HashFileName(const char * pbKey);
	//清除申请的内存及状态
	void WpfClear();
	//清除空白文件和目录
	void ClearEmptyDirAndFile(PWpfFileList pList);



protected:
	static DWORD  m_dwCryptTable[MAX_WPF_HASH_BUFFF_SIZE];//用来计算hash值
	string        m_strWpfDir,m_strWpfFileName;//wpf路径信息
	NEWDIR        m_CreateDirHeader;
	NEWFCB        m_CreateFcbHeader;
	DWORD         m_dwExpendLen;//没有空白空间时扩展的长度
    bool          m_bSaveWhenExpended;//扩展之后是否立即保存
	bool          m_bAlertError;//出错之后是否弹框警告
	bool          m_bSpeedPrior;//是否速度优先,否则内存优先
	bool          m_bClearEmptyDirAndFile;//是否清除空目录以及空文件
	DWORD         m_dwAddAttributeToNewFile;//设置导入文件或目录时所有文件应该额外增加的属性,比如导入某个文件夹时要求添加EFA_MEARGE_UNCOMPRESS属性,而其它文件中的文件不需要这个属性

	E_WPF_RTN     m_eWrLastError;//最后一次错误代码
	WpfHeader     m_WpfHeader;//文件头
	WpfHeader     m_WpfHeader_Bak;//文件头备份
	WpfHeader     m_WpfHeader_Disk;//已经保存到磁盘中的文件头,m_WpfHeader随时会发生改变,如果备份头指向的FCB被文件覆盖,m_WpfHeader_Bak应该变成m_WpfHeader_Disk,这时如果写m_WpfHeader_Bak失败,m_WpfHeader_Disk始终是有效的,下次保存时会要求重写m_WpfHeader_Bak
	PWpfFileList  m_pWpfRoot;//根目录
	PWpfFileList  m_pDirList;//目录列表
	int           m_iFileHandle[EFT_NUM];//文件句柄
	PBlankBlockNode   m_pBlankBlockList;//空闲块描述列表
	PFCB1         m_pOpenFileFcb1[EFT_NUM];//当前访问文件的FCB1
	DWORD         m_iOpenFilePointPos[EFT_NUM];//当前访问文件的文件指针位置,相对于当前打开文件的起始位置
	bool          m_bModifyed;//是否对wpf文件进行了修改操作,如果有在最后要自动保存
	bool          m_bSaving;//是否正在保存,如果正在保存,不允许其它地方进行写操作,为了速度设计上不允许多个线程同时进行写操作,否则可能破坏目录结构
	__int64       m_iWpfStartPos;//wpf文件内容在整个打开文件中的起始位置
	DWORD         m_dwOpenType;//打开文件的类型
	MIFCB1        m_MhashFcb1;//各文件对应的hash值map,只读操作时用到,编辑操作用m_pDirList
	MIFCB         m_MhashFcb;//各文件对应的hash值到fcb的map,EWOT_LISTDIR及EWOT_LISTFCBHASH时用到
	PFCB1         m_pFcb1List;//fcb1列表
	PFCB2         m_pFcb2List;//fcb1列表
	PFCB          m_pFcbList;//fcb列表
	bool		  m_bMeargeNotNeedUnCopress;//合并是否要解压,true不解压

	bool			  m_bBuildTempFileHashEnabled;//是否生成临时hash表
	map<__int64,BYTE> m_kTempFileHash;//临时hash表
};
