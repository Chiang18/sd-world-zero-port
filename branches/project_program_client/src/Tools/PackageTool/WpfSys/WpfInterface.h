//*************************************************************************************************
// 内容:	wpf文件系统实现,用于wpf文件读取与写入
//
// wpfinterface.h:wpf文件格式操作接口
// wpf文件格式简介:wpf以Dos\Windows中的Fat32文件系统为参照,根据实际需要有一些修改.
// 1.经测试如果每块(扇区)512字节格式化,传世中平均每个文件寻址多达40次,耗时0.048ms,此外一次性读取20K
//	(传世图片平均大小)内容的效率要比分40次每次读取512字节高1倍以上.
//	另外如果用非连续存储传世中现有的很多接口要重写,wpf中的采用连续存储,在新加文件时必须分配连续的
//	块(扇区),如果找不到则以EXPEND_LEN为单位扩展整个图包大小,这种方式的缺点是容易产生碎片,但传世中
//	删除或更改文件很少,大多数都是新加,再者删除文件留下的空间可以被后面新加的文件利用,有很多按钮文
//	件本身都小于512字节,总的来说碎片不会很多.
//	此外还提供了碎片整理接口,在发现碎片太多的时候可以进行碎片整理,以提高空间利用率,一般情况下磁盘
//	利用率为95%左右,这种方式的缺点就是容易产生碎片以及存储前要知道文件的大小,但索引分配及链式分配
//	都对性能有极大影响,鉴于我们存储前一般都知道文件大小,并且外面版本的删除及修改操作很少,所以选用
//	了连续分配.
// 2.FCB(文件控制块)结构中去掉时间等字段,把文件名扩展到32字节(包含扩展名)
// 3.目录看成一个文件,其中内容为其子目录及子文件的FCB,20万个文件及目录大概要13.7M的空间存储相关信息
// 4.仿操作系统内存管理机制,管理空闲磁盘空间
//
// 结构如下:
// -------------------------------             
//         S_WpfHeader文件头
// -------------------------------
//           文件数据
// -------------------------------
//           空闲块描述信息所在位置
// -------------------------------
//           FCB List  
//---------------------------------------------------------
//
// 作者:	
// 创建:		
// 最后修改:
//*************************************************************************************************
#pragma once

#ifndef _WINDOWS_
#define WIN32_LEAN_AND_MEAN		///< 从 Windows 头中排除极少使用的资料
#include <windows.h>
#endif

#include <map>
#include <string>
#include <vector>

using namespace std;

#define  WPF_MAGIC           'WPF\x01'
#define  MAX_FILE_NAME_LEN   256		///< svn原来的值是32，WZ的值是256
//-------------------------------------------------------------------------------------------------
// 文件属性
enum E_FCTATTR
{
	EFA_BLANK              = 0,			///< 空白
	EFA_DIR                = 0x0001,	///< 目录
	EFA_FILE               = 0x0002,	///< 文件
	EFA_COMPRESS           = 0x0004,	///< 拥有此属性的文件为压缩文件,压缩类型由文件头中的byCompress指定
	EFA_CRYPT              = 0x0008,	///< 拥有此属性的文件为加密文件,压缩类型由文件头中的byCrypt指定
	EFA_MEARGE_UNCOMPRESS  = 0x0010,	///< 拥有此属性的文件,如果已经压缩了,当合并到目标wpf里去时要求自行解压,解压后再合并到目标wpf
};
//-------------------------------------------------------------------------------------------------
// 用哪一个文件指针打开文件,一个wpf文件支持多个线程同时操作
// 所有有关wpf的编辑操作都使用EFT_MAIN指针,只读操作可以随便同时使用所有指针
enum E_FPTYPE
{
	EFT_MAIN      = 0,		///< 主线程
	EFT_THREAD    = 1,		///< 其它线程
	EFT_THREAD2   = 2,		///< 其它线程
	EFT_NUM,
};
//-------------------------------------------------------------------------------------------------
// wpf操作返回代码
enum E_WPF_RTN
{
	EWR_OK	= 0,			///< 成功
	EWR_ERROR,				///< 失败
	EWR_PARA_ERROR,			///< 参数错误
	EWR_FILE_NOT_OPEN,      ///< wpf文件没有打开
	EWR_FILE_EXIST,         ///< 文件已经存在
	EWR_FILE_NOT_EXIST,     ///< 文件已经存在
	EWR_FILE_SAVE_FAIL,     ///< 文件保存失败
	EWR_FILE_OPEN_FAIL,     ///< 文件打开失败
	EWR_NOT_WPF_FILE,       ///< 文件格式错误
	EWR_DIR_EXIST,          ///< 目录已经存在
	EWR_DIR_NOT_EXIST,      ///< 目录不存在
	EWR_PATH_INVALID,       ///< 文件名或路径非法
	EWR_FILE_SAVING,        ///< 正在保存文件,此时的写操作将会被拒绝,为了性能,设计上不允许多线程同时进行写操作

	EWR_UNKNOWN,			///< 其它未知错误代码
};
//-------------------------------------------------------------------------------------------------
// wpf的打开方式
enum E_WPF_OPENTYPE
{
	EWOT_READONLY      = 0x0001,	///< 只读,这种模式不会构造目录树,查找文件是按路径的hash值查找,速度快,省内存
	EWOT_RDWR          = 0x0002,	///< 读写模式,要求对文件或目录添加,删除,修改等操作使用此模式,写操作使用EFT_MAIN文件指针,有此属性时各文件操作函数非线程安全,由外部保证线程安全
	EWOT_LISTDIR       = 0x0004,	///< 列出目录,EWOT_RDWR包含EWOT_LISTDIR
	EWOT_LISTHASH      = 0x0008,	///< 列出hash和fcb1列表,EWOT_READONLY包含EWOT_LISTHASH
	EWOT_LISTFCBHASH   = 0x0010,	///< 列出hash和fcb列表,有EWOT_LISTDIR属性时该属性才有效,有此属性时各文件操作函数非线程安全,由外部保证线程的安全
};

#pragma pack(4)
//-------------------------------------------------------------------------------------------------
//fcb第1部分,读操作时用到,为了省内存和fcb2分开
struct S_Fcb1
{
	S_Fcb1()
	{
		dwStart = 0;
		dwSize = 0;
		iHashKey = 0;
	}

	DWORD   dwStart;	///< 如果是文件表示该文件的起始块号,如果是目录表示该目录的文件目录的起始FCB编号
    DWORD   dwSize;		///< 对于文件表示文件长度，以字节为单位,对于目录表示该目录中的FCB项数
	__int64 iHashKey;	///< 该文件全路径的hash值
};
//-------------------------------------------------------------------------------------------------
//fcb第2部分,编辑操作时用到
struct S_Fcb2
{
	S_Fcb2()
	{
		memset(strName,0,MAX_FILE_NAME_LEN);
		memset(strMd5,0,16);
		dwAttribute = EFA_BLANK;
		dwRev = 0;
	}

	char  strName[MAX_FILE_NAME_LEN];    ///< 如果是文件表示文件名加扩展名,如果是目录表示目录名
	char  strMd5[16];                    ///< 文件MD5码
    DWORD dwAttribute;                   ///< 属性E_FCTATTR类型
	DWORD dwRev;                         ///< 当拥有EFA_COMPRESS属性时表示压缩之前的大小
};
//-------------------------------------------------------------------------------------------------
//wpf头结构,64字节,2份共128字节
struct S_WpfHeader
{
	S_WpfHeader()
	{
		Clear();
		//memcpy(byRev2 + 64 - strlen("Design:何国辉,shandagames"),"Design:何国辉,shandagames",strlen("Design:何国辉,shandagames"));
	}

	void Clear()
	{
		dwMagic = WPF_MAGIC;
		wHeaderSize = sizeof(S_WpfHeader) * 2;	///< 文件头及文件头备份
		iWpfSize = 0;
		iBlankBlockPos = wHeaderSize;
		dwBlankBlockSize = 0;
		dwDirCount = 1;							///< 默认有一个根目录
		dwFileCount = 0;
		dwTotalBlocks = 0;
		wBytesPerBlock = 128;
		byRev1 = 0;
		byCompress = 0;
		byCrypt = 0;
		byRev2 = 0;	
		dwWpfType = 0;
		dwRev = 0;

		memset(byRev3,0,sizeof(BYTE) * 3);
		bySavingHeader = TRUE;
	}

	DWORD     dwMagic;              ///< 魔数,固定为'WPF\x01'
    WORD      wHeaderSize;          ///< 头的大小,也是第一个文件或目录的起始位置
	WORD      wBytesPerBlock;       ///< 每个块的字节数

	__int64   iBlankBlockPos;       ///< 空闲块描述信息所在位置
	DWORD     dwBlankBlockSize;     ///< 有多少条空闲块信息(起始位置1,大小1,起始位置2,大小2...)DWORD

	__int64   iFatPos;              ///< 文件分配表所在位置
	DWORD     dwDirCount;           ///< 目录总数
	DWORD     dwFileCount;          ///< 文件个数

	DWORD     dwTotalBlocks;        ///< 划分的总块数
	__int64   iWpfSize;             ///< 整个wpf文件的大小(不包含文件头,空闲块描述信息及文件分配表)
    BYTE      byRev1;               ///< 由于历史原因,此字节作废
	BYTE      byCrypt;              ///< 加密类型
	BYTE      byCompress;           ///< 压缩类型
	BYTE      byRev2;               ///< 保留
//	DWORD	  byRev[2];				///< 此字段在WZ被以下两个字段替代(dwWpfType和dwRev)
	DWORD     dwWpfType;            ///< wpf的类型,比如多个游戏用到wpf文件格式,放升级包的时候防止放错了把其它游戏的文件包打进来了,就可以根据dwWpfType来区分,每个游戏的类型保证不一样,至少升级包保证不一样.
	DWORD     dwRev;                ///< 保留
	BYTE      byRev3[3];            ///< 保留
	BYTE      bySavingHeader;       ///< 是否正在保存文件头,为TRUE表示上次没有保存文件头成功,失败后下次读取时会读取上次的文件头备份,机制可以保证失败后原来的fcblist不被覆盖,重打版本可以修复,备份头写失败下次会重写备份头,不可能两份头的bySavingHeader都为TRUE,除非创建的时候就失败了,那么就不是一个wpf文件,这种情况打开的时候会返回失败
};
//-------------------------------------------------------------------------------------------------
#pragma pack ()

typedef struct S_WpfHeader WpfHeader;
typedef struct S_Fcb1   FCB1;
typedef struct S_Fcb1*  PFCB1;
typedef struct S_Fcb2   FCB2;
typedef struct S_Fcb2*  PFCB2;

// wpf文件列表结构
struct S_WpfFileList;
typedef struct S_WpfFileList	WpfFileList;
typedef struct S_WpfFileList*	PWpfFileList;
typedef map<string,PWpfFileList> MList;

//-------------------------------------------------------------------------------------------------
// fcb记录结构
struct S_Fcb
{ 
	S_Fcb()
	{
		pfcb1 = NULL;
		pfcb2 = NULL;
		pParent = NULL;
	}

	PFCB1 pfcb1;          ///< fcb第一部分
	PFCB2 pfcb2;          ///< fcb第二部分
	PWpfFileList pParent; ///< 父目录	
};
//-------------------------------------------------------------------------------------------------

typedef struct S_Fcb  FCB;
typedef struct S_Fcb* PFCB;
typedef map<string,PFCB> MSFCB;
typedef map<__int64,PFCB1> MIFCB1;
typedef map<__int64,PFCB> MIFCB;

//-------------------------------------------------------------------------------------------------
//目录结构
struct S_WpfFileList
{
	S_WpfFileList()
	{
		pfcb = NULL;
	}

	PFCB        pfcb;   ///< 该目录或文件的fcb
	MList       MDirs;  ///< 该目录下的子目录列表
	MSFCB       MFiles; ///< 该目录下的子文件列表
};
//-------------------------------------------------------------------------------------------------
// 在导入导出文件时的回调函数,每导入或导出一个文件,调用一次这个函数.
// szOutPutMsg一般是指传刚刚处理完的文件路径名
typedef void (*LPWpfCallBack)(const char *szOutPutMsg);

//-------------------------------------------------------------------------------------------------
// wpf文件操作接口,如要获得某个操作中出现的错误代码,可以调用GetLastError()
// 为了效率问题,所以路径区分大小写
//-------------------------------------------------------------------------------------------------
class CWpfInterface
{
public:
	virtual ~CWpfInterface(){} 

public:
	// wpf文件操作
	// @{
	// 创建一个新的Wpf文件,创建后文件初始大小默认为1M(不包含文件头,空闲块描述信息及文件分配表),
	// 创建后会进行格式化
	//	@param[in]	strPathName		全路径名
	//	@param[in]	iSize			文件尺寸(不包含文件头等)
	//	@param[in]	bFailExist		文件存在是否返回失败,如果该值为false,文件存在就删除原来的,
	virtual bool WpfCreate(const char* strPathName,
		__int64 iSize = 1*1024*1024,
		bool bFailExist = true) = 0;

    // 打开一个Wpf文件,其中的目录及文件信息保存到一个MWpfFileList结构,可以通过GetRoot获得该
	//	@param[in]	strPathName		全路径名
	//	@param[in]	iOffset			wpf文件内容在strPathName这个文件中的起始位置,如果整个文件就是一个wpf文件,取0
	//	@param[in]	dwOpenType
	virtual bool WpfOpen(const char * strPathName,
		__int64 iOffset = 0,
		DWORD dwOpenType = EWOT_READONLY) = 0;
	
	// 关闭文件,清除文件列表
	virtual void WpfClose() = 0;

	// 获得最后一次错误代码
	virtual E_WPF_RTN GetLastError() = 0;

	// 获得指定wpf文件的文件列表map,Open后有效
	virtual PWpfFileList GetRoot() = 0;

	// 获得文件头
	virtual WpfHeader *GetHeader() = 0;

	// 设置空间不够时每次扩展多少字节
	virtual bool SetExpendLength(DWORD dwLen = 10*1024*1024) = 0;

	// 空间不足扩展之后是否马上保存目录结构,为了安全安装的时候应该保存,为了防止频繁保存, 
	// 安装的时候ExpendLength应该设置得大一点,打包的时候为了速度及防止打出来的包太大,可
	// 以把ExpendLength设置得少一点
	virtual void SetSaveWhenExpended(bool bSave) = 0;

	// 设置是否弹框提示错误,打包的时候要提示，安装的时候不提示
	virtual void SetAlertError(bool bAlert) = 0;

	// 设置wpf的类型,比如多个游戏用到wpf文件格式,放升级包的时候防止放错了把其它游戏的文件包
	// 打进来了,就可以根据dwWpfType来区分,每个游戏的类型保证不一样,至少升级包保证不一样.
	virtual bool SetWpfType(DWORD dwType) = 0;

	// 获得wpf的类型,比如多个游戏用到wpf文件格式,放升级包的时候防止放错了把其它游戏的文件包
	// 打进来了,就可以根据dwWpfType来区分,每个游戏的类型保证不一样,至少升级包保证不一样.
	// 失败返回-1,比如没有打开wpf文件
	virtual DWORD GetWpfType() = 0;

	// 设置是否速度优先,速度优先的话读写fcb的时候都是所有文件的fcb一次性连续读写,否则为
	// 内存优先,单条读写.对于总的fcb所占内存不大的情况下建议使用速度优先,默认速度优先
	virtual void SetSpeedPrior(bool bSpeedPrior) = 0;

	// 是否速度优先
	virtual bool IsSpeedPrior() = 0;

	// 设置是否清除空白文件及目录,默认不清除
	virtual void SetClearEmptyDirAndFile(bool bClearEmptyDirAndFile) = 0;

	// 是否清除空白文件及目录,默认不清除
	virtual bool IsClearEmptyDirAndFile() = 0;
	
	// 设置导入文件或目录时所有文件应该额外增加的属性,比如导入某个文件夹时要求添加
	// EFA_MEARGE_UNCOMPRESS属性,而其它文件中的文件不需要这个属性
	virtual void SetAddAttributeToNewFile(DWORD dwAddAttribute = 0) = 0;

	// 获得新加文件时额外添加的属性
	virtual DWORD GetAddAttributeToNewFile() = 0;

	// 获取wpf文件路径
	virtual const char* GetWpfFileDir() = 0;

	// 获取wpf文件名
	virtual const char* GetWpfFileName() = 0;

	// 设置合并是否解压
	virtual void SetMeargeNotNeedUnCopress(bool val) = 0;

	// 合并是否解压
	virtual bool GetMeargeNotNeedUnCopress() = 0;
	// @}


	//
	// Wpf的文件与目录操作
	//
	// 1.操作一个文件之前要先打开这个文件,像Seek,Read等,就想打个磁盘中的一个文件一样,
	//	 打开以后文件指针停在这个文件的起始位置.
	// 2.同一时刻只能操作一个文件,如果要多个线程操作同一个wpf文件可以通过不同的文件指针,
	//	 见E_FPTYPE,一个对象不能同时处理多个wpf图包中的文件,要处理多个自行使用多个对象,
	//	 在外部管理
	//
	// FileWrite函数不会自动增大文件,文件的大小在创建时就定了,如果超过文件的最大长度写到文件结束为止
	// 打开一个文件,如果之前打开了其它文件会自动关闭原来的文件,打开一个新文件之前可以不调用
	// FileClose(),目录分隔符只能用"\",不能用,"/",因为效率问题,内部不做转换.
	//
	// @{
	//
	// 创建目录
	virtual PWpfFileList CreateDir(const char *strPathName) = 0;

	// 递归删除指定目录下所有文件及子目录
	virtual bool DeleteDir(const char * strPathName) = 0;
	
	// 查找目录
	virtual PWpfFileList FindDir(const char * strPathName) = 0;

	// 创建一个文件
	//	@param[in]	strPathName
	//	@param[in]	dwLen		文件长度
	//	@param[in]	bFailExist	文件存在是否返回失败,如果该值为false,文件存在就删除原来的, 添加成功后会自动打开该文件并把文件指针停在文件开始位置。
	virtual PFCB CreateFile(const char * strPathName, DWORD dwLen, bool bFailExist = true) = 0;
	
	// 从buf中添加一个文件
	//	@param[in]	strPathName
	//	@param[in]	buff
	//	@param[in]	dwLen
	//	@param[in]	bFailExist	
	//	@param[in]	bCompress	导入前是否要先压缩
	//	@param[in]	bUnCompress	导入前是否先解压
	virtual PFCB CreateFileFromBuff(const char * strPathName, 
		const char * buff, 
		DWORD dwLen, 
		bool bFailExist = true,
		bool bCompress = false,
		bool bUnCompress = false) = 0;

	// 查找文件,以EWOT_RDWR方式打开wfp时有效
	//virtual FindFile(const char* strPathName) = 0;
	
	// 查找文件,目录分隔符只能用"\\",不能用"/",因为效率问题,内部不做转换,EWOT_LISTDIR方式下有效
	virtual PFCB FindFile_PFCB(const char* strPathName) = 0;
	
	// 查找文件,目录分隔符只能用"\\",不能用"/",因为效率问题,内部不做转换
	virtual PFCB1 FindFile_PFCB1(const char* strPathName) = 0;
	 
	// 查找文件,包含EWOT_LISTHASH或EWOT_LISTDIR打开属性时有效
	virtual PFCB1 FindFile_PFCB1(__int64 iHash) = 0;
	
	// 打开文件,以EWOT_RDWR方式打开wfp时有效
	virtual PFCB FileOpenEx(const char* strPathName, E_FPTYPE fpType = EFT_MAIN) = 0;
	
	// 删除文件
	virtual bool DeleteFile(const char* strPathName) = 0;
	
	// 更新文件内容,因为连续分配的原因,其实就是先删除再添加
	//	@param[in]	strPathName
	//	@param[in]	buff
	//	@param[in]	dwLen
	//	@param[in]	bCompress	导入前是否要先压缩
	//	@param[in]	bUnCompress	导入前是否先解压
	virtual bool UpdateFile(const char* strPathName,
		const char* buff,
		DWORD dwLen,
		bool bCompress = false,
		bool bUnCompress = false) = 0;

	// 打开文件(Hash模式下不区分大小写)
	virtual PFCB1 FileOpen(const char* strPathName, E_FPTYPE fpType = EFT_MAIN) = 0;
	
	// 打开文件,根据hash值访问,返回PFCB1,EWOT_LISTHASH方式下有效
	virtual PFCB1 FileOpen_Hash2FCB1(__int64 iHash, E_FPTYPE fpType = EFT_MAIN) = 0;

	// 打开文件,根据hash值访问,返回PFCB,EWOT_LISTDIR以及EWOT_LISTFCBHASH方式下有效
	virtual PFCB FileOpen_Hash2FCB(__int64 iHash, E_FPTYPE fpType = EFT_MAIN) = 0;
	
	// 打开文件
	virtual bool FileOpen(PFCB1 pfcb1, E_FPTYPE fpType = EFT_MAIN) = 0;

	// 关闭当前打开的文件
	virtual bool FileClose(E_FPTYPE fpType = EFT_MAIN) = 0;

	// 在当前打开的文件内寻址,SEEK_END时,dwOffset也取正值,表示的位置为FileLength - dwOffset
	virtual bool FileSeek(DWORD dwOffset, int iOrigin = SEEK_SET, E_FPTYPE fpType = EFT_MAIN) = 0;
	
	// 读取文件,返回 <= 0 表示失败
	virtual DWORD FileRead(void* pBbuff, DWORD dwSize, E_FPTYPE fpType = EFT_MAIN) = 0;
	
	// 写文件,返回 <= 0 表示失败
	virtual DWORD FileWrite(const void* pBbuff,DWORD dwSize,E_FPTYPE fpType = EFT_MAIN) = 0;
	
	// 获得文件长度
	virtual DWORD FileLength(E_FPTYPE fpType = EFT_MAIN) = 0;
	
	// 获得打开文件相对文件头的偏移,如果没有打开文件返回-1
	virtual __int64 GetFileOffset(E_FPTYPE fpType = EFT_MAIN) = 0;
	
	// 先打开文件,如果打开成功,获得打开文件相对文件头的偏移,否则返回-1
	virtual __int64 GetFileOffset(const char * strPathName,E_FPTYPE fpType = EFT_MAIN) = 0;
	// @}

	
	//
	// 扩展函数,WpfSeek,WpfRead,WpfWrite都会导致FileClose(),关闭当前操作的函数
	//
	// 不可以在dll和exe之间传递FILE*,除非都是MTD,如果要外管理文件指针,可以在外部打开,
	// 然后通过相关接口获得offset再在外部处理
	//
	// @{
	virtual int GetFileHandle(E_FPTYPE fpType) = 0;

	//
	virtual __int64 WpfLength() = 0;

	//
	virtual __int64 WpfSeek(__int64 dwOffset = 0, int iOrigin = SEEK_SET) = 0;

	// Wpf文件数据读取，返回 <= 0 表示失败
	virtual DWORD WpfRead(void* pBbuff, DWORD dwSize) = 0;

	// Wpf文件数据写入，返回 <= 0 表示失败
	virtual DWORD WpfWrite(const void* pBbuff, DWORD dwSize) = 0;

	// 保存wpf,保存文件头,字位影像图,文件分配表,对打开的wpf文件有效,
	// 如果不保存,到最后关闭的时候才会保存,会导致FileClose()操作,
	virtual bool WpfSave() = 0;

	// 合并wpf,里面如果有同名文件则会覆盖本图包中的文件
	//	@param[in]	strSrcPath
	//	@param[in]	iOffset		源wpf在strSrcPath这个文件中的起始位置,如果这个文件是一个纯粹的wpf,为0
	//	@param[in]	pCallBack
	virtual bool MeargeWpf(const char* strSrcPath, __int64 iOffset = 0, LPWpfCallBack pCallBack = NULL) = 0;

	// 导入一个文件
	//	@param[in]	strParnetDir	导入到wpf的路径,不一定要以"\\"结束,如果strParnetDir为NULL或""都导到根目录
	//	@param[in]	strFilePath		待导入的文件全路径
	//	@param[in]	bCompress
	//	@param[in]	bFailExist
	virtual PFCB ImportFile(const char* strParnetDir, const char* strFilePath, bool bCompress, bool bFailExist = true) = 0;

	// 把pWpf中的文件pfcb复制到些wpf的指定目录plist中
	virtual PFCB ImportFileToDir(CWpfInterface* pWpf, PFCB pfcb, PWpfFileList pList, bool bFailExist = true) = 0;
	
	// 递归导入一个目录及其子文件到paretntpatah,包含strDirPath本身
	virtual PWpfFileList ImportDir(const char* strParnetDir, const char* strDirPath, bool bCompress, bool bFailExist = true, LPWpfCallBack pCallBack = NULL) = 0;
	
	// 递归导入一个目录及其子文件paretntpatah,不包含strDirPath本身
	virtual PWpfFileList ImportSubDir(const char* strParnetDir, const char* strDirPath, bool bCompress, bool bFailExist = true, LPWpfCallBack pCallBack = NULL) = 0;
	
	// 把另一个wpf文件中的指定目录导入到这个wpf中,包含pSrcList这个目录本身
	virtual bool ImportDirFromList(CWpfInterface* pWpf, PWpfFileList pSrcList ,PWpfFileList pDestList, bool bFailExist = true, LPWpfCallBack pCallBack = NULL) = 0;
	
	// 把另一个wpf文件中的指定目录导入到这个wpf中,不包含pSrcList这个目录本身
	virtual bool ImportSubDirFromList(CWpfInterface* pWpf, PWpfFileList pSrcList, PWpfFileList pDestList, bool bFailExist = true, LPWpfCallBack pCallBack = NULL) = 0;
	
	// 导出指定文件到目录文件,参数为全路径名,包括文件名及扩展名
	virtual bool ExportFile(const char* strSrcPath, const char *strDestPath) = 0;
	
	// 导出指定文件到目录文件,参数为全路径名,包括文件名及扩展名
	virtual bool ExportFile(PFCB pfcb, const char* strDestPath) = 0;
	
	// 导出目录,包括strSrcPath本身
	virtual bool ExportDir(const char* strSrcPath,const char* strDestPath, LPWpfCallBack pCallBack = NULL) = 0;

	// 导出目录,不包括strSrcPath本身
	virtual bool ExportSubDir(const char* strSrcPath,const char* strDestPath, LPWpfCallBack pCallBack = NULL) = 0;

	// 导出目录,包括strSrcPath本身
	virtual void ExportDirFromList(const string& strDestPath,PWpfFileList pList, LPWpfCallBack pCallBack = NULL) = 0;
	
	// 导出目录,不包括strSrcPath本身
	virtual void ExportSubDirFromList(const string& strDestPath,PWpfFileList pList, LPWpfCallBack pCallBack = NULL) = 0;
	
	//
	virtual bool ExportAllFileFromFCB1(const char* strDestPath) = 0;

	// 碎片整理(貌似木有实现)
	virtual bool CleanUpFragment() = 0;

	// 获得指定路径下的子文件或子目录,只读方式下使用,非只读方式下请使用FindDir接口,
	//	@param[in]	strParnetDir
	//	@param[in]	pCallBack		为回调函数,每找到一个文件或子目录会以文件或目录为参数调用该函数,动态库与应用程序之前不能直接传递vector之类参数,
	//	@param[in]	bSubDir			false为取子文件,true为取子目录
	virtual bool GetSubFileName(const char* strParnetDir, LPWpfCallBack pCallBack, bool bSubDir = false) = 0;
	
	// 生成hash文件,把整个hash表保存到一个文件,同时有文件头(貌似木有实现)
	virtual bool CreateHashFile(const char* strFilePath, const char* strDirPath = "") = 0;

	// 获得目录的全路径名
	virtual const char* GetDirFullPath(PWpfFileList pList) = 0;

	// 得到str的hash值
	virtual __int64 GetHashKey(const char* str) = 0;
	// @}


	// 临时hash表
	// @{
	virtual bool IsBuildTempFileHashEnabled() const = 0;
	virtual void SetBuildTempFileHashEnabled(bool val)  = 0;
	virtual const map<__int64,BYTE>& GetTempFileHash() const = 0;
	virtual void ClearTempFileHash() = 0;
	// @}
};