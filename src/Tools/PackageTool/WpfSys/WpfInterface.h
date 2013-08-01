//*************************************************************************************************
// ����:	wpf�ļ�ϵͳʵ��,����wpf�ļ���ȡ��д��
//
// wpfinterface.h:wpf�ļ���ʽ�����ӿ�
// wpf�ļ���ʽ���:wpf��Dos\Windows�е�Fat32�ļ�ϵͳΪ����,����ʵ����Ҫ��һЩ�޸�.
// 1.���������ÿ��(����)512�ֽڸ�ʽ��,������ƽ��ÿ���ļ�Ѱַ���40��,��ʱ0.048ms,����һ���Զ�ȡ20K
//	(����ͼƬƽ����С)���ݵ�Ч��Ҫ�ȷ�40��ÿ�ζ�ȡ512�ֽڸ�1������.
//	��������÷������洢���������еĺܶ�ӿ�Ҫ��д,wpf�еĲ��������洢,���¼��ļ�ʱ�������������
//	��(����),����Ҳ�������EXPEND_LENΪ��λ��չ����ͼ����С,���ַ�ʽ��ȱ�������ײ�����Ƭ,��������
//	ɾ��������ļ�����,����������¼�,����ɾ���ļ����µĿռ���Ա������¼ӵ��ļ�����,�кܶఴť��
//	������С��512�ֽ�,�ܵ���˵��Ƭ����ܶ�.
//	���⻹�ṩ����Ƭ����ӿ�,�ڷ�����Ƭ̫���ʱ����Խ�����Ƭ����,����߿ռ�������,һ������´���
//	������Ϊ95%����,���ַ�ʽ��ȱ��������ײ�����Ƭ�Լ��洢ǰҪ֪���ļ��Ĵ�С,���������估��ʽ����
//	���������м���Ӱ��,�������Ǵ洢ǰһ�㶼֪���ļ���С,��������汾��ɾ�����޸Ĳ�������,����ѡ��
//	����������.
// 2.FCB(�ļ����ƿ�)�ṹ��ȥ��ʱ����ֶ�,���ļ�����չ��32�ֽ�(������չ��)
// 3.Ŀ¼����һ���ļ�,��������Ϊ����Ŀ¼�����ļ���FCB,20����ļ���Ŀ¼���Ҫ13.7M�Ŀռ�洢�����Ϣ
// 4.�²���ϵͳ�ڴ�������,������д��̿ռ�
//
// �ṹ����:
// -------------------------------             
//         S_WpfHeader�ļ�ͷ
// -------------------------------
//           �ļ�����
// -------------------------------
//           ���п�������Ϣ����λ��
// -------------------------------
//           FCB List  
//---------------------------------------------------------
//
// ����:	
// ����:		
// ����޸�:
//*************************************************************************************************
#pragma once

#ifndef _WINDOWS_
#define WIN32_LEAN_AND_MEAN		///< �� Windows ͷ���ų�����ʹ�õ�����
#include <windows.h>
#endif

#include <map>
#include <string>
#include <vector>

using namespace std;

#define  WPF_MAGIC           'WPF\x01'
#define  MAX_FILE_NAME_LEN   256		///< svnԭ����ֵ��32��WZ��ֵ��256
//-------------------------------------------------------------------------------------------------
// �ļ�����
enum E_FCTATTR
{
	EFA_BLANK              = 0,			///< �հ�
	EFA_DIR                = 0x0001,	///< Ŀ¼
	EFA_FILE               = 0x0002,	///< �ļ�
	EFA_COMPRESS           = 0x0004,	///< ӵ�д����Ե��ļ�Ϊѹ���ļ�,ѹ���������ļ�ͷ�е�byCompressָ��
	EFA_CRYPT              = 0x0008,	///< ӵ�д����Ե��ļ�Ϊ�����ļ�,ѹ���������ļ�ͷ�е�byCryptָ��
	EFA_MEARGE_UNCOMPRESS  = 0x0010,	///< ӵ�д����Ե��ļ�,����Ѿ�ѹ����,���ϲ���Ŀ��wpf��ȥʱҪ�����н�ѹ,��ѹ���ٺϲ���Ŀ��wpf
};
//-------------------------------------------------------------------------------------------------
// ����һ���ļ�ָ����ļ�,һ��wpf�ļ�֧�ֶ���߳�ͬʱ����
// �����й�wpf�ı༭������ʹ��EFT_MAINָ��,ֻ�������������ͬʱʹ������ָ��
enum E_FPTYPE
{
	EFT_MAIN      = 0,		///< ���߳�
	EFT_THREAD    = 1,		///< �����߳�
	EFT_THREAD2   = 2,		///< �����߳�
	EFT_NUM,
};
//-------------------------------------------------------------------------------------------------
// wpf�������ش���
enum E_WPF_RTN
{
	EWR_OK	= 0,			///< �ɹ�
	EWR_ERROR,				///< ʧ��
	EWR_PARA_ERROR,			///< ��������
	EWR_FILE_NOT_OPEN,      ///< wpf�ļ�û�д�
	EWR_FILE_EXIST,         ///< �ļ��Ѿ�����
	EWR_FILE_NOT_EXIST,     ///< �ļ��Ѿ�����
	EWR_FILE_SAVE_FAIL,     ///< �ļ�����ʧ��
	EWR_FILE_OPEN_FAIL,     ///< �ļ���ʧ��
	EWR_NOT_WPF_FILE,       ///< �ļ���ʽ����
	EWR_DIR_EXIST,          ///< Ŀ¼�Ѿ�����
	EWR_DIR_NOT_EXIST,      ///< Ŀ¼������
	EWR_PATH_INVALID,       ///< �ļ�����·���Ƿ�
	EWR_FILE_SAVING,        ///< ���ڱ����ļ�,��ʱ��д�������ᱻ�ܾ�,Ϊ������,����ϲ�������߳�ͬʱ����д����

	EWR_UNKNOWN,			///< ����δ֪�������
};
//-------------------------------------------------------------------------------------------------
// wpf�Ĵ򿪷�ʽ
enum E_WPF_OPENTYPE
{
	EWOT_READONLY      = 0x0001,	///< ֻ��,����ģʽ���ṹ��Ŀ¼��,�����ļ��ǰ�·����hashֵ����,�ٶȿ�,ʡ�ڴ�
	EWOT_RDWR          = 0x0002,	///< ��дģʽ,Ҫ����ļ���Ŀ¼���,ɾ��,�޸ĵȲ���ʹ�ô�ģʽ,д����ʹ��EFT_MAIN�ļ�ָ��,�д�����ʱ���ļ������������̰߳�ȫ,���ⲿ��֤�̰߳�ȫ
	EWOT_LISTDIR       = 0x0004,	///< �г�Ŀ¼,EWOT_RDWR����EWOT_LISTDIR
	EWOT_LISTHASH      = 0x0008,	///< �г�hash��fcb1�б�,EWOT_READONLY����EWOT_LISTHASH
	EWOT_LISTFCBHASH   = 0x0010,	///< �г�hash��fcb�б�,��EWOT_LISTDIR����ʱ�����Բ���Ч,�д�����ʱ���ļ������������̰߳�ȫ,���ⲿ��֤�̵߳İ�ȫ
};

#pragma pack(4)
//-------------------------------------------------------------------------------------------------
//fcb��1����,������ʱ�õ�,Ϊ��ʡ�ڴ��fcb2�ֿ�
struct S_Fcb1
{
	S_Fcb1()
	{
		dwStart = 0;
		dwSize = 0;
		iHashKey = 0;
	}

	DWORD   dwStart;	///< ������ļ���ʾ���ļ�����ʼ���,�����Ŀ¼��ʾ��Ŀ¼���ļ�Ŀ¼����ʼFCB���
    DWORD   dwSize;		///< �����ļ���ʾ�ļ����ȣ����ֽ�Ϊ��λ,����Ŀ¼��ʾ��Ŀ¼�е�FCB����
	__int64 iHashKey;	///< ���ļ�ȫ·����hashֵ
};
//-------------------------------------------------------------------------------------------------
//fcb��2����,�༭����ʱ�õ�
struct S_Fcb2
{
	S_Fcb2()
	{
		memset(strName,0,MAX_FILE_NAME_LEN);
		memset(strMd5,0,16);
		dwAttribute = EFA_BLANK;
		dwRev = 0;
	}

	char  strName[MAX_FILE_NAME_LEN];    ///< ������ļ���ʾ�ļ�������չ��,�����Ŀ¼��ʾĿ¼��
	char  strMd5[16];                    ///< �ļ�MD5��
    DWORD dwAttribute;                   ///< ����E_FCTATTR����
	DWORD dwRev;                         ///< ��ӵ��EFA_COMPRESS����ʱ��ʾѹ��֮ǰ�Ĵ�С
};
//-------------------------------------------------------------------------------------------------
//wpfͷ�ṹ,64�ֽ�,2�ݹ�128�ֽ�
struct S_WpfHeader
{
	S_WpfHeader()
	{
		Clear();
		//memcpy(byRev2 + 64 - strlen("Design:�ι���,shandagames"),"Design:�ι���,shandagames",strlen("Design:�ι���,shandagames"));
	}

	void Clear()
	{
		dwMagic = WPF_MAGIC;
		wHeaderSize = sizeof(S_WpfHeader) * 2;	///< �ļ�ͷ���ļ�ͷ����
		iWpfSize = 0;
		iBlankBlockPos = wHeaderSize;
		dwBlankBlockSize = 0;
		dwDirCount = 1;							///< Ĭ����һ����Ŀ¼
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

	DWORD     dwMagic;              ///< ħ��,�̶�Ϊ'WPF\x01'
    WORD      wHeaderSize;          ///< ͷ�Ĵ�С,Ҳ�ǵ�һ���ļ���Ŀ¼����ʼλ��
	WORD      wBytesPerBlock;       ///< ÿ������ֽ���

	__int64   iBlankBlockPos;       ///< ���п�������Ϣ����λ��
	DWORD     dwBlankBlockSize;     ///< �ж��������п���Ϣ(��ʼλ��1,��С1,��ʼλ��2,��С2...)DWORD

	__int64   iFatPos;              ///< �ļ����������λ��
	DWORD     dwDirCount;           ///< Ŀ¼����
	DWORD     dwFileCount;          ///< �ļ�����

	DWORD     dwTotalBlocks;        ///< ���ֵ��ܿ���
	__int64   iWpfSize;             ///< ����wpf�ļ��Ĵ�С(�������ļ�ͷ,���п�������Ϣ���ļ������)
    BYTE      byRev1;               ///< ������ʷԭ��,���ֽ�����
	BYTE      byCrypt;              ///< ��������
	BYTE      byCompress;           ///< ѹ������
	BYTE      byRev2;               ///< ����
//	DWORD	  byRev[2];				///< ���ֶ���WZ�����������ֶ����(dwWpfType��dwRev)
	DWORD     dwWpfType;            ///< wpf������,��������Ϸ�õ�wpf�ļ���ʽ,����������ʱ���ֹ�Ŵ��˰�������Ϸ���ļ����������,�Ϳ��Ը���dwWpfType������,ÿ����Ϸ�����ͱ�֤��һ��,������������֤��һ��.
	DWORD     dwRev;                ///< ����
	BYTE      byRev3[3];            ///< ����
	BYTE      bySavingHeader;       ///< �Ƿ����ڱ����ļ�ͷ,ΪTRUE��ʾ�ϴ�û�б����ļ�ͷ�ɹ�,ʧ�ܺ��´ζ�ȡʱ���ȡ�ϴε��ļ�ͷ����,���ƿ��Ա�֤ʧ�ܺ�ԭ����fcblist��������,�ش�汾�����޸�,����ͷдʧ���´λ���д����ͷ,����������ͷ��bySavingHeader��ΪTRUE,���Ǵ�����ʱ���ʧ����,��ô�Ͳ���һ��wpf�ļ�,��������򿪵�ʱ��᷵��ʧ��
};
//-------------------------------------------------------------------------------------------------
#pragma pack ()

typedef struct S_WpfHeader WpfHeader;
typedef struct S_Fcb1   FCB1;
typedef struct S_Fcb1*  PFCB1;
typedef struct S_Fcb2   FCB2;
typedef struct S_Fcb2*  PFCB2;

// wpf�ļ��б�ṹ
struct S_WpfFileList;
typedef struct S_WpfFileList	WpfFileList;
typedef struct S_WpfFileList*	PWpfFileList;
typedef map<string,PWpfFileList> MList;

//-------------------------------------------------------------------------------------------------
// fcb��¼�ṹ
struct S_Fcb
{ 
	S_Fcb()
	{
		pfcb1 = NULL;
		pfcb2 = NULL;
		pParent = NULL;
	}

	PFCB1 pfcb1;          ///< fcb��һ����
	PFCB2 pfcb2;          ///< fcb�ڶ�����
	PWpfFileList pParent; ///< ��Ŀ¼	
};
//-------------------------------------------------------------------------------------------------

typedef struct S_Fcb  FCB;
typedef struct S_Fcb* PFCB;
typedef map<string,PFCB> MSFCB;
typedef map<__int64,PFCB1> MIFCB1;
typedef map<__int64,PFCB> MIFCB;

//-------------------------------------------------------------------------------------------------
//Ŀ¼�ṹ
struct S_WpfFileList
{
	S_WpfFileList()
	{
		pfcb = NULL;
	}

	PFCB        pfcb;   ///< ��Ŀ¼���ļ���fcb
	MList       MDirs;  ///< ��Ŀ¼�µ���Ŀ¼�б�
	MSFCB       MFiles; ///< ��Ŀ¼�µ����ļ��б�
};
//-------------------------------------------------------------------------------------------------
// �ڵ��뵼���ļ�ʱ�Ļص�����,ÿ����򵼳�һ���ļ�,����һ���������.
// szOutPutMsgһ����ָ���ոմ�������ļ�·����
typedef void (*LPWpfCallBack)(const char *szOutPutMsg);

//-------------------------------------------------------------------------------------------------
// wpf�ļ������ӿ�,��Ҫ���ĳ�������г��ֵĴ������,���Ե���GetLastError()
// Ϊ��Ч������,����·�����ִ�Сд
//-------------------------------------------------------------------------------------------------
class CWpfInterface
{
public:
	virtual ~CWpfInterface(){} 

public:
	// wpf�ļ�����
	// @{
	// ����һ���µ�Wpf�ļ�,�������ļ���ʼ��СĬ��Ϊ1M(�������ļ�ͷ,���п�������Ϣ���ļ������),
	// ���������и�ʽ��
	//	@param[in]	strPathName		ȫ·����
	//	@param[in]	iSize			�ļ��ߴ�(�������ļ�ͷ��)
	//	@param[in]	bFailExist		�ļ������Ƿ񷵻�ʧ��,�����ֵΪfalse,�ļ����ھ�ɾ��ԭ����,
	virtual bool WpfCreate(const char* strPathName,
		__int64 iSize = 1*1024*1024,
		bool bFailExist = true) = 0;

    // ��һ��Wpf�ļ�,���е�Ŀ¼���ļ���Ϣ���浽һ��MWpfFileList�ṹ,����ͨ��GetRoot��ø�
	//	@param[in]	strPathName		ȫ·����
	//	@param[in]	iOffset			wpf�ļ�������strPathName����ļ��е���ʼλ��,��������ļ�����һ��wpf�ļ�,ȡ0
	//	@param[in]	dwOpenType
	virtual bool WpfOpen(const char * strPathName,
		__int64 iOffset = 0,
		DWORD dwOpenType = EWOT_READONLY) = 0;
	
	// �ر��ļ�,����ļ��б�
	virtual void WpfClose() = 0;

	// ������һ�δ������
	virtual E_WPF_RTN GetLastError() = 0;

	// ���ָ��wpf�ļ����ļ��б�map,Open����Ч
	virtual PWpfFileList GetRoot() = 0;

	// ����ļ�ͷ
	virtual WpfHeader *GetHeader() = 0;

	// ���ÿռ䲻��ʱÿ����չ�����ֽ�
	virtual bool SetExpendLength(DWORD dwLen = 10*1024*1024) = 0;

	// �ռ䲻����չ֮���Ƿ����ϱ���Ŀ¼�ṹ,Ϊ�˰�ȫ��װ��ʱ��Ӧ�ñ���,Ϊ�˷�ֹƵ������, 
	// ��װ��ʱ��ExpendLengthӦ�����õô�һ��,�����ʱ��Ϊ���ٶȼ���ֹ������İ�̫��,��
	// �԰�ExpendLength���õ���һ��
	virtual void SetSaveWhenExpended(bool bSave) = 0;

	// �����Ƿ񵯿���ʾ����,�����ʱ��Ҫ��ʾ����װ��ʱ����ʾ
	virtual void SetAlertError(bool bAlert) = 0;

	// ����wpf������,��������Ϸ�õ�wpf�ļ���ʽ,����������ʱ���ֹ�Ŵ��˰�������Ϸ���ļ���
	// �������,�Ϳ��Ը���dwWpfType������,ÿ����Ϸ�����ͱ�֤��һ��,������������֤��һ��.
	virtual bool SetWpfType(DWORD dwType) = 0;

	// ���wpf������,��������Ϸ�õ�wpf�ļ���ʽ,����������ʱ���ֹ�Ŵ��˰�������Ϸ���ļ���
	// �������,�Ϳ��Ը���dwWpfType������,ÿ����Ϸ�����ͱ�֤��һ��,������������֤��һ��.
	// ʧ�ܷ���-1,����û�д�wpf�ļ�
	virtual DWORD GetWpfType() = 0;

	// �����Ƿ��ٶ�����,�ٶ����ȵĻ���дfcb��ʱ���������ļ���fcbһ����������д,����Ϊ
	// �ڴ�����,������д.�����ܵ�fcb��ռ�ڴ治�������½���ʹ���ٶ�����,Ĭ���ٶ�����
	virtual void SetSpeedPrior(bool bSpeedPrior) = 0;

	// �Ƿ��ٶ�����
	virtual bool IsSpeedPrior() = 0;

	// �����Ƿ�����հ��ļ���Ŀ¼,Ĭ�ϲ����
	virtual void SetClearEmptyDirAndFile(bool bClearEmptyDirAndFile) = 0;

	// �Ƿ�����հ��ļ���Ŀ¼,Ĭ�ϲ����
	virtual bool IsClearEmptyDirAndFile() = 0;
	
	// ���õ����ļ���Ŀ¼ʱ�����ļ�Ӧ�ö������ӵ�����,���絼��ĳ���ļ���ʱҪ�����
	// EFA_MEARGE_UNCOMPRESS����,�������ļ��е��ļ�����Ҫ�������
	virtual void SetAddAttributeToNewFile(DWORD dwAddAttribute = 0) = 0;

	// ����¼��ļ�ʱ������ӵ�����
	virtual DWORD GetAddAttributeToNewFile() = 0;

	// ��ȡwpf�ļ�·��
	virtual const char* GetWpfFileDir() = 0;

	// ��ȡwpf�ļ���
	virtual const char* GetWpfFileName() = 0;

	// ���úϲ��Ƿ��ѹ
	virtual void SetMeargeNotNeedUnCopress(bool val) = 0;

	// �ϲ��Ƿ��ѹ
	virtual bool GetMeargeNotNeedUnCopress() = 0;
	// @}


	//
	// Wpf���ļ���Ŀ¼����
	//
	// 1.����һ���ļ�֮ǰҪ�ȴ�����ļ�,��Seek,Read��,�����������е�һ���ļ�һ��,
	//	 ���Ժ��ļ�ָ��ͣ������ļ�����ʼλ��.
	// 2.ͬһʱ��ֻ�ܲ���һ���ļ�,���Ҫ����̲߳���ͬһ��wpf�ļ�����ͨ����ͬ���ļ�ָ��,
	//	 ��E_FPTYPE,һ��������ͬʱ������wpfͼ���е��ļ�,Ҫ����������ʹ�ö������,
	//	 ���ⲿ����
	//
	// FileWrite���������Զ������ļ�,�ļ��Ĵ�С�ڴ���ʱ�Ͷ���,��������ļ�����󳤶�д���ļ�����Ϊֹ
	// ��һ���ļ�,���֮ǰ���������ļ����Զ��ر�ԭ�����ļ�,��һ�����ļ�֮ǰ���Բ�����
	// FileClose(),Ŀ¼�ָ���ֻ����"\",������,"/",��ΪЧ������,�ڲ�����ת��.
	//
	// @{
	//
	// ����Ŀ¼
	virtual PWpfFileList CreateDir(const char *strPathName) = 0;

	// �ݹ�ɾ��ָ��Ŀ¼�������ļ�����Ŀ¼
	virtual bool DeleteDir(const char * strPathName) = 0;
	
	// ����Ŀ¼
	virtual PWpfFileList FindDir(const char * strPathName) = 0;

	// ����һ���ļ�
	//	@param[in]	strPathName
	//	@param[in]	dwLen		�ļ�����
	//	@param[in]	bFailExist	�ļ������Ƿ񷵻�ʧ��,�����ֵΪfalse,�ļ����ھ�ɾ��ԭ����, ��ӳɹ�����Զ��򿪸��ļ������ļ�ָ��ͣ���ļ���ʼλ�á�
	virtual PFCB CreateFile(const char * strPathName, DWORD dwLen, bool bFailExist = true) = 0;
	
	// ��buf�����һ���ļ�
	//	@param[in]	strPathName
	//	@param[in]	buff
	//	@param[in]	dwLen
	//	@param[in]	bFailExist	
	//	@param[in]	bCompress	����ǰ�Ƿ�Ҫ��ѹ��
	//	@param[in]	bUnCompress	����ǰ�Ƿ��Ƚ�ѹ
	virtual PFCB CreateFileFromBuff(const char * strPathName, 
		const char * buff, 
		DWORD dwLen, 
		bool bFailExist = true,
		bool bCompress = false,
		bool bUnCompress = false) = 0;

	// �����ļ�,��EWOT_RDWR��ʽ��wfpʱ��Ч
	//virtual FindFile(const char* strPathName) = 0;
	
	// �����ļ�,Ŀ¼�ָ���ֻ����"\\",������"/",��ΪЧ������,�ڲ�����ת��,EWOT_LISTDIR��ʽ����Ч
	virtual PFCB FindFile_PFCB(const char* strPathName) = 0;
	
	// �����ļ�,Ŀ¼�ָ���ֻ����"\\",������"/",��ΪЧ������,�ڲ�����ת��
	virtual PFCB1 FindFile_PFCB1(const char* strPathName) = 0;
	 
	// �����ļ�,����EWOT_LISTHASH��EWOT_LISTDIR������ʱ��Ч
	virtual PFCB1 FindFile_PFCB1(__int64 iHash) = 0;
	
	// ���ļ�,��EWOT_RDWR��ʽ��wfpʱ��Ч
	virtual PFCB FileOpenEx(const char* strPathName, E_FPTYPE fpType = EFT_MAIN) = 0;
	
	// ɾ���ļ�
	virtual bool DeleteFile(const char* strPathName) = 0;
	
	// �����ļ�����,��Ϊ���������ԭ��,��ʵ������ɾ�������
	//	@param[in]	strPathName
	//	@param[in]	buff
	//	@param[in]	dwLen
	//	@param[in]	bCompress	����ǰ�Ƿ�Ҫ��ѹ��
	//	@param[in]	bUnCompress	����ǰ�Ƿ��Ƚ�ѹ
	virtual bool UpdateFile(const char* strPathName,
		const char* buff,
		DWORD dwLen,
		bool bCompress = false,
		bool bUnCompress = false) = 0;

	// ���ļ�(Hashģʽ�²����ִ�Сд)
	virtual PFCB1 FileOpen(const char* strPathName, E_FPTYPE fpType = EFT_MAIN) = 0;
	
	// ���ļ�,����hashֵ����,����PFCB1,EWOT_LISTHASH��ʽ����Ч
	virtual PFCB1 FileOpen_Hash2FCB1(__int64 iHash, E_FPTYPE fpType = EFT_MAIN) = 0;

	// ���ļ�,����hashֵ����,����PFCB,EWOT_LISTDIR�Լ�EWOT_LISTFCBHASH��ʽ����Ч
	virtual PFCB FileOpen_Hash2FCB(__int64 iHash, E_FPTYPE fpType = EFT_MAIN) = 0;
	
	// ���ļ�
	virtual bool FileOpen(PFCB1 pfcb1, E_FPTYPE fpType = EFT_MAIN) = 0;

	// �رյ�ǰ�򿪵��ļ�
	virtual bool FileClose(E_FPTYPE fpType = EFT_MAIN) = 0;

	// �ڵ�ǰ�򿪵��ļ���Ѱַ,SEEK_ENDʱ,dwOffsetҲȡ��ֵ,��ʾ��λ��ΪFileLength - dwOffset
	virtual bool FileSeek(DWORD dwOffset, int iOrigin = SEEK_SET, E_FPTYPE fpType = EFT_MAIN) = 0;
	
	// ��ȡ�ļ�,���� <= 0 ��ʾʧ��
	virtual DWORD FileRead(void* pBbuff, DWORD dwSize, E_FPTYPE fpType = EFT_MAIN) = 0;
	
	// д�ļ�,���� <= 0 ��ʾʧ��
	virtual DWORD FileWrite(const void* pBbuff,DWORD dwSize,E_FPTYPE fpType = EFT_MAIN) = 0;
	
	// ����ļ�����
	virtual DWORD FileLength(E_FPTYPE fpType = EFT_MAIN) = 0;
	
	// ��ô��ļ�����ļ�ͷ��ƫ��,���û�д��ļ�����-1
	virtual __int64 GetFileOffset(E_FPTYPE fpType = EFT_MAIN) = 0;
	
	// �ȴ��ļ�,����򿪳ɹ�,��ô��ļ�����ļ�ͷ��ƫ��,���򷵻�-1
	virtual __int64 GetFileOffset(const char * strPathName,E_FPTYPE fpType = EFT_MAIN) = 0;
	// @}

	
	//
	// ��չ����,WpfSeek,WpfRead,WpfWrite���ᵼ��FileClose(),�رյ�ǰ�����ĺ���
	//
	// ��������dll��exe֮�䴫��FILE*,���Ƕ���MTD,���Ҫ������ļ�ָ��,�������ⲿ��,
	// Ȼ��ͨ����ؽӿڻ��offset�����ⲿ����
	//
	// @{
	virtual int GetFileHandle(E_FPTYPE fpType) = 0;

	//
	virtual __int64 WpfLength() = 0;

	//
	virtual __int64 WpfSeek(__int64 dwOffset = 0, int iOrigin = SEEK_SET) = 0;

	// Wpf�ļ����ݶ�ȡ������ <= 0 ��ʾʧ��
	virtual DWORD WpfRead(void* pBbuff, DWORD dwSize) = 0;

	// Wpf�ļ�����д�룬���� <= 0 ��ʾʧ��
	virtual DWORD WpfWrite(const void* pBbuff, DWORD dwSize) = 0;

	// ����wpf,�����ļ�ͷ,��λӰ��ͼ,�ļ������,�Դ򿪵�wpf�ļ���Ч,
	// ���������,�����رյ�ʱ��Żᱣ��,�ᵼ��FileClose()����,
	virtual bool WpfSave() = 0;

	// �ϲ�wpf,���������ͬ���ļ���Ḳ�Ǳ�ͼ���е��ļ�
	//	@param[in]	strSrcPath
	//	@param[in]	iOffset		Դwpf��strSrcPath����ļ��е���ʼλ��,�������ļ���һ�������wpf,Ϊ0
	//	@param[in]	pCallBack
	virtual bool MeargeWpf(const char* strSrcPath, __int64 iOffset = 0, LPWpfCallBack pCallBack = NULL) = 0;

	// ����һ���ļ�
	//	@param[in]	strParnetDir	���뵽wpf��·��,��һ��Ҫ��"\\"����,���strParnetDirΪNULL��""��������Ŀ¼
	//	@param[in]	strFilePath		��������ļ�ȫ·��
	//	@param[in]	bCompress
	//	@param[in]	bFailExist
	virtual PFCB ImportFile(const char* strParnetDir, const char* strFilePath, bool bCompress, bool bFailExist = true) = 0;

	// ��pWpf�е��ļ�pfcb���Ƶ�Щwpf��ָ��Ŀ¼plist��
	virtual PFCB ImportFileToDir(CWpfInterface* pWpf, PFCB pfcb, PWpfFileList pList, bool bFailExist = true) = 0;
	
	// �ݹ鵼��һ��Ŀ¼�������ļ���paretntpatah,����strDirPath����
	virtual PWpfFileList ImportDir(const char* strParnetDir, const char* strDirPath, bool bCompress, bool bFailExist = true, LPWpfCallBack pCallBack = NULL) = 0;
	
	// �ݹ鵼��һ��Ŀ¼�������ļ�paretntpatah,������strDirPath����
	virtual PWpfFileList ImportSubDir(const char* strParnetDir, const char* strDirPath, bool bCompress, bool bFailExist = true, LPWpfCallBack pCallBack = NULL) = 0;
	
	// ����һ��wpf�ļ��е�ָ��Ŀ¼���뵽���wpf��,����pSrcList���Ŀ¼����
	virtual bool ImportDirFromList(CWpfInterface* pWpf, PWpfFileList pSrcList ,PWpfFileList pDestList, bool bFailExist = true, LPWpfCallBack pCallBack = NULL) = 0;
	
	// ����һ��wpf�ļ��е�ָ��Ŀ¼���뵽���wpf��,������pSrcList���Ŀ¼����
	virtual bool ImportSubDirFromList(CWpfInterface* pWpf, PWpfFileList pSrcList, PWpfFileList pDestList, bool bFailExist = true, LPWpfCallBack pCallBack = NULL) = 0;
	
	// ����ָ���ļ���Ŀ¼�ļ�,����Ϊȫ·����,�����ļ�������չ��
	virtual bool ExportFile(const char* strSrcPath, const char *strDestPath) = 0;
	
	// ����ָ���ļ���Ŀ¼�ļ�,����Ϊȫ·����,�����ļ�������չ��
	virtual bool ExportFile(PFCB pfcb, const char* strDestPath) = 0;
	
	// ����Ŀ¼,����strSrcPath����
	virtual bool ExportDir(const char* strSrcPath,const char* strDestPath, LPWpfCallBack pCallBack = NULL) = 0;

	// ����Ŀ¼,������strSrcPath����
	virtual bool ExportSubDir(const char* strSrcPath,const char* strDestPath, LPWpfCallBack pCallBack = NULL) = 0;

	// ����Ŀ¼,����strSrcPath����
	virtual void ExportDirFromList(const string& strDestPath,PWpfFileList pList, LPWpfCallBack pCallBack = NULL) = 0;
	
	// ����Ŀ¼,������strSrcPath����
	virtual void ExportSubDirFromList(const string& strDestPath,PWpfFileList pList, LPWpfCallBack pCallBack = NULL) = 0;
	
	//
	virtual bool ExportAllFileFromFCB1(const char* strDestPath) = 0;

	// ��Ƭ����(ò��ľ��ʵ��)
	virtual bool CleanUpFragment() = 0;

	// ���ָ��·���µ����ļ�����Ŀ¼,ֻ����ʽ��ʹ��,��ֻ����ʽ����ʹ��FindDir�ӿ�,
	//	@param[in]	strParnetDir
	//	@param[in]	pCallBack		Ϊ�ص�����,ÿ�ҵ�һ���ļ�����Ŀ¼�����ļ���Ŀ¼Ϊ�������øú���,��̬����Ӧ�ó���֮ǰ����ֱ�Ӵ���vector֮�����,
	//	@param[in]	bSubDir			falseΪȡ���ļ�,trueΪȡ��Ŀ¼
	virtual bool GetSubFileName(const char* strParnetDir, LPWpfCallBack pCallBack, bool bSubDir = false) = 0;
	
	// ����hash�ļ�,������hash���浽һ���ļ�,ͬʱ���ļ�ͷ(ò��ľ��ʵ��)
	virtual bool CreateHashFile(const char* strFilePath, const char* strDirPath = "") = 0;

	// ���Ŀ¼��ȫ·����
	virtual const char* GetDirFullPath(PWpfFileList pList) = 0;

	// �õ�str��hashֵ
	virtual __int64 GetHashKey(const char* str) = 0;
	// @}


	// ��ʱhash��
	// @{
	virtual bool IsBuildTempFileHashEnabled() const = 0;
	virtual void SetBuildTempFileHashEnabled(bool val)  = 0;
	virtual const map<__int64,BYTE>& GetTempFileHash() const = 0;
	virtual void ClearTempFileHash() = 0;
	// @}
};