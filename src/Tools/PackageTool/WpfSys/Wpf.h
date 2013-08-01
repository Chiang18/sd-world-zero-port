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


#define  MAX_WPF_HASH_BUFFF_SIZE   0x500 //��������hashֵ
#define  BLOCK_SIZE 256*1024   //�ֿ��д�ļ�һ��Ĵ�С




//�½��ļ�����Ϣ,�������ر�ʱ�ͷ��ڴ�
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
//���п�������Ϣ�ṹ
typedef struct S_BlankBlock
{
	DWORD dwStart;//��ʼλ��
	DWORD dwSize;//��С
}BlankBlock,*PBlankBlock;

//���п�������Ϣ�б�ṹ
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
	DWORD dwSize;//������
	DWORD dwRev;
}HashFileHeader,*PHashFileHeader;



class CWpf : public CWpfInterface
{
public:
	CWpf(void);
public:
	~CWpf(void);

public:
	//����һ���µ�Wpf�ļ�,strPathName:ȫ·����,bFailExist:�ļ������Ƿ񷵻�ʧ��,�����ֵΪfalse,�ļ����ھ�ɾ��ԭ����,
	//�������ļ���ʼ��СĬ��Ϊ1M(�������ļ�ͷ��),���������и�ʽ��
	virtual bool WpfCreate(const char * strPathName,__int64 iSize = 1*1024*1024,bool bFailExist = true);
    //��һ��Wpf�ļ�,���е�Ŀ¼���ļ���Ϣ���浽һ��MWpfFileList�ṹ,����ͨ��GetRoot��ø�
	//iOffset:wpf�ļ�������strPathName����ļ��е���ʼλ��,��������ļ�����һ��wpf�ļ�,ȡ0
	virtual bool WpfOpen(const char * strPathName,__int64 iOffset = 0,DWORD dwOpenType = EWOT_READONLY);
	//�ر��ļ�,����ļ��б�
	virtual void WpfClose();
	//������һ�δ������
	virtual E_WPF_RTN GetLastError();
	//���ָ��wpf�ļ����ļ��б�map,Open����Ч
	virtual PWpfFileList GetRoot();
	//����ļ�ͷ
	virtual WpfHeader *GetHeader();
	//���ÿռ䲻��ʱÿ����չ�����ֽ�
	virtual bool SetExpendLength(DWORD dwLen = 10*1024*1024);
	//�ռ䲻����չ֮���Ƿ����ϱ���Ŀ¼�ṹ,Ϊ�˰�ȫ��װ��ʱ��Ӧ�ñ���,Ϊ�˷�ֹƵ������,��װ��ʱ��ExpendLengthӦ�����õô�һ��,�����ʱ��Ϊ���ٶȼ���ֹ������İ�̫��,���԰�ExpendLength���õ���һ��
	virtual void SetSaveWhenExpended(bool bSave);
	//�����Ƿ񵯿���ʾ����,�����ʱ��Ҫ��ʾ����װ��ʱ����ʾ
	virtual void SetAlertError(bool bAlert);
	//����wpf������,��������Ϸ�õ�wpf�ļ���ʽ,����������ʱ���ֹ�Ŵ��˰�������Ϸ���ļ����������,�Ϳ��Ը���dwWpfType������,ÿ����Ϸ�����ͱ�֤��һ��,������������֤��һ��.
	virtual bool SetWpfType(DWORD dwType);
	//���wpf������,��������Ϸ�õ�wpf�ļ���ʽ,����������ʱ���ֹ�Ŵ��˰�������Ϸ���ļ����������,�Ϳ��Ը���dwWpfType������,ÿ����Ϸ�����ͱ�֤��һ��,������������֤��һ��.ʧ�ܷ���-1,����û�д�wpf�ļ�
	virtual DWORD GetWpfType();
	//�����Ƿ��ٶ�����,�ٶ����ȵĻ���дfcb��ʱ���������ļ���fcbһ����������д,����Ϊ�ڴ�����,������д
	virtual void SetSpeedPrior(bool bSpeedPrior);
	//�Ƿ��ٶ�����
	virtual bool IsSpeedPrior();
	//�����Ƿ�����հ��ļ���Ŀ¼,Ĭ�ϲ����
	virtual void SetClearEmptyDirAndFile(bool bClearEmptyDirAndFile);
	//�Ƿ�����հ��ļ���Ŀ¼,Ĭ�ϲ����
	virtual bool IsClearEmptyDirAndFile();
	//���õ����ļ���Ŀ¼ʱ�����ļ�Ӧ�ö������ӵ�����,���絼��ĳ���ļ���ʱҪ�����EFA_MEARGE_UNCOMPRESS����,�������ļ��е��ļ�����Ҫ�������
	virtual void SetAddAttributeToNewFile(DWORD dwAddAttribute = 0);
	//����¼��ļ�ʱ������ӵ�����
	virtual DWORD GetAddAttributeToNewFile();
	//��ȡwpf�ļ�·��
	virtual const char* GetWpfFileDir();
	//��ȡwpf�ļ���
	virtual const char* GetWpfFileName();
	//�ϲ��Ƿ��ѹ
	virtual void SetMeargeNotNeedUnCopress(bool val);
	//�ϲ��Ƿ��ѹ
	virtual bool GetMeargeNotNeedUnCopress();


	//����Ŀ¼
	virtual PWpfFileList CreateDir(const char *strPathName);
	//�ݹ�ɾ��ָ��Ŀ¼�������ļ�����Ŀ¼
	virtual bool DeleteDir(const char * strPathName);
	//����Ŀ¼
	virtual PWpfFileList FindDir(const char * strPathName);
	//����һ���ļ�,dwLen:�ļ�����,bFailExist:�ļ������Ƿ񷵻�ʧ��,�����ֵΪfalse,�ļ����ھ�ɾ��ԭ����,��ӳɹ�����Զ��򿪸��ļ������ļ�ָ��ͣ���ļ���ʼλ��
	virtual PFCB CreateFile(const char * strPathName,DWORD dwLen,bool bFailExist = true);
	//��buf�����һ���ļ�
	virtual PFCB CreateFileFromBuff(const char * strPathName,const char * buff,DWORD dwLen,bool bFailExist = true,bool bCompress = false,bool bUnCompress = false);//bCompress:����ǰ�Ƿ�Ҫ��ѹ��,bUnCompress:����ǰ�Ƿ��Ƚ�ѹ
	//�����ļ�,Ŀ¼�ָ���ֻ����"\",������,"/",��ΪЧ������,�ڲ�����ת��,EWOT_LISTDIR��ʽ����Ч
	virtual PFCB FindFile_PFCB(const char * strPathName);
	//�����ļ�,Ŀ¼�ָ���ֻ����"\",������,"/",��ΪЧ������,�ڲ�����ת��
	virtual PFCB1 FindFile_PFCB1(const char * strPathName);
	//�����ļ�,����EWOT_LISTHASH��(EWOT_LISTDIR & EWOT_LISTFCBHASH)������ʱ��Ч
	virtual PFCB1 FindFile_PFCB1(__int64 iHash);
	//���ļ�,��EWOT_RDWR��ʽ��wfpʱ��Ч
	virtual PFCB FileOpenEx(const char * strPathName,E_FPTYPE fpType = EFT_MAIN);
	//ɾ���ļ�
	virtual bool DeleteFile(const char * strPathName);
	//�����ļ�����,��Ϊ���������ԭ��,��ʵ������ɾ�������
	virtual bool UpdateFile(const char * strPathName,const char *buff,DWORD dwLen,bool bCompress = false,bool bUnCompress = false);//bCompress:����ǰ�Ƿ�Ҫ��ѹ��,bUnCompress:����ǰ�Ƿ��Ƚ�ѹ


	//����һ���ļ�֮ǰҪ�ȴ�����ļ�,��Seek,Read��,�����������е�һ���ļ�һ��,���Ժ��ļ�ָ��ͣ������ļ�����ʼλ��
	//ͬһʱ��ֻ�ܲ���һ���ļ�,���Ҫ����̲߳���ͬһ��wpf�ļ�����ͨ����ͬ���ļ�ָ��,��E_FPTYPE,һ��������ͬʱ������wpfͼ���е��ļ�,Ҫ����������ʹ�ö������,���ⲿ����
	//FileWrite���������Զ������ļ�,�ļ��Ĵ�С�ڴ���ʱ�Ͷ���,��������ļ�����󳤶�д���ļ�����Ϊֹ
	//��һ���ļ�,���֮ǰ���������ļ����Զ��ر�ԭ�����ļ�,��һ�����ļ�֮ǰ���Բ�����FileClose(),Ŀ¼�ָ���ֻ����"\",������,"/",��ΪЧ������,�ڲ�����ת��
	virtual PFCB1 FileOpen(const char * strPathName,E_FPTYPE fpType = EFT_MAIN);
	//���ļ�,����hashֵ����,����PFCB1,EWOT_LISTHASH��ʽ����Ч
	virtual PFCB1 FileOpen_Hash2FCB1(__int64 iHash,E_FPTYPE fpType = EFT_MAIN);
	//���ļ�,����hashֵ����,����PFCB,EWOT_LISTDIR�Լ�EWOT_LISTFCBHASH��ʽ����Ч
	virtual PFCB FileOpen_Hash2FCB(__int64 iHash,E_FPTYPE fpType = EFT_MAIN);
	//���ļ�
	virtual bool FileOpen(PFCB1 pfcb1,E_FPTYPE fpType = EFT_MAIN);
	//�رյ�ǰ�򿪵��ļ�
	virtual bool FileClose(E_FPTYPE fpType = EFT_MAIN);
	//�ڵ�ǰ�򿪵��ļ���Ѱַ,SEEK_ENDʱ,dwOffsetҲȡ��ֵ,��ʾ��λ��ΪFileLength - dwOffset
	virtual bool FileSeek(DWORD dwOffset,int iOrigin = SEEK_SET,E_FPTYPE fpType = EFT_MAIN);
	//��ȡ�ļ� ���� <= 0 ��ʾʧ��
	virtual DWORD FileRead(void* pBbuff,DWORD dwSize,E_FPTYPE fpType = EFT_MAIN);
	//д�ļ� ���� <= 0 ��ʾʧ��
	virtual DWORD FileWrite(const void* pBbuff,DWORD dwSize,E_FPTYPE fpType = EFT_MAIN);
	//����ļ�����
	virtual DWORD FileLength(E_FPTYPE fpType = EFT_MAIN);
	//��ô��ļ�����ļ�ͷ��ƫ��,���û�д��ļ�����-1
	virtual __int64 GetFileOffset(E_FPTYPE fpType = EFT_MAIN);
	//�ȴ��ļ�,����򿪳ɹ�,��ô��ļ�����ļ�ͷ��ƫ��,���򷵻�-1
	virtual __int64 GetFileOffset(const char * strPathName,E_FPTYPE fpType = EFT_MAIN);


	//��չ����,WpfSeek,WpfRead,WpfWrite���ᵼ��FileClose(),�رյ�ǰ�����ĺ���
	virtual int GetFileHandle(E_FPTYPE fpType);//��������dll��exe֮�䴫��FILE*,���Ƕ���MTD,���Ҫ������ļ�ָ��,�������ⲿ��,Ȼ��ͨ����ؽӿڻ��offset�����ⲿ����
	virtual __int64 WpfLength();
	virtual __int64 WpfSeek(__int64 dwOffset = 0,int iOrigin = SEEK_SET);
	virtual DWORD WpfRead(void* pBbuff,DWORD dwSize);// ���� <= 0 ��ʾʧ��
	virtual DWORD WpfWrite(const void* pBbuff,DWORD dwSize);// ���� <= 0 ��ʾʧ��

	virtual PFCB ImportFile(const char *strParnetDir,const char *strFilePath,bool bCompress,bool bFailExist = true);//����һ���ļ�,���strParnetDirΪNULL��""��������Ŀ¼
	virtual PFCB ImportFileToDir(CWpfInterface * pWpf,PFCB pfcb,PWpfFileList pList,bool bFailExist = true);//��pWpf�е��ļ�pfcb���Ƶ�Щwpf��ָ��Ŀ¼plist��
	virtual PWpfFileList ImportDir(const char *strParnetDir,const char *strDirPath,bool bCompress,bool bFailExist = true,LPWpfCallBack pCallBack = NULL);//�ݹ鵼��һ��Ŀ¼�������ļ���paretntpatah,����strDirPath����,
	virtual PWpfFileList ImportSubDir(const char *strParnetDir,const char *strDirPath,bool bCompress,bool bFailExist = true,LPWpfCallBack pCallBack = NULL);//�ݹ鵼��һ��Ŀ¼�������ļ�paretntpatah,������strDirPath����,
	virtual bool ImportDirFromList(CWpfInterface * pWpf,PWpfFileList pSrcList,PWpfFileList pDestList,bool bFailExist = true,LPWpfCallBack pCallBack = NULL);//����һ��wpf�ļ��е�ָ��Ŀ¼���뵽���wpf��,����pSrcList���Ŀ¼����
	virtual bool ImportSubDirFromList(CWpfInterface * pWpf,PWpfFileList pSrcList,PWpfFileList pDestList,bool bFailExist = true,LPWpfCallBack pCallBack = NULL);//����һ��wpf�ļ��е�ָ��Ŀ¼���뵽���wpf��,������pSrcList���Ŀ¼����
	virtual bool ExportFile(const char *strSrcPath,const char *strDestPath);//����ָ���ļ���Ŀ¼�ļ�,����Ϊȫ·����,�����ļ�������չ��
	virtual bool ExportFile(PFCB pfcb,const char *strDestPath);//����ָ���ļ���Ŀ¼�ļ�,����Ϊȫ·����,�����ļ�������չ��
	virtual bool ExportDir(const char *strSrcPath,const char *strDestPath,LPWpfCallBack pCallBack = NULL);//����Ŀ¼,����strSrcPath����
	virtual bool ExportSubDir(const char *strSrcPath,const char *strDestPath,LPWpfCallBack pCallBack = NULL);//����Ŀ¼,������strSrcPath����
	virtual void ExportDirFromList(const string &strDestPath,PWpfFileList pList,LPWpfCallBack pCallBack = NULL);//����Ŀ¼,����strSrcPath����
	virtual void ExportSubDirFromList(const string &strDestPath,PWpfFileList pList,LPWpfCallBack pCallBack = NULL);//����Ŀ¼,������strSrcPath����
	virtual bool ExportAllFileFromFCB1(const char *strDestPath);

	virtual bool MeargeWpf(const char *strSrcPath,__int64 iOffset = 0,LPWpfCallBack pCallBack = NULL);//�ϲ�wpf,���������ͬ���ļ���Ḳ�Ǳ�ͼ���е��ļ�,iOffsetԴwpf��strSrcPath����ļ��е���ʼλ��,�������ļ���һ�������wpf,Ϊ0
	virtual bool WpfSave();//����wpf,�����ļ�ͷ,��λӰ��ͼ,�ļ������,�Դ򿪵�wpf�ļ���Ч,���������,�����رյ�ʱ��Żᱣ��,�ᵼ��FileClose()����
	virtual bool CleanUpFragment();//��Ƭ����
	virtual bool GetSubFileName(const char *strParnetDir,LPWpfCallBack pCallBack,bool bSubDir = false);//���ָ��·���µ����ļ�����Ŀ¼,ֻ����ʽ��ʹ��,��ֻ����ʽ����ʹ��FindDir�ӿ�,pCallBackΪ�ص�����,ÿ�ҵ�һ���ļ�����Ŀ¼�����ļ���Ŀ¼Ϊ�������øú���,��̬����Ӧ�ó���֮ǰ����ֱ�Ӵ���vector֮�����,bSubDir: falseΪȡ���ļ�,trueΪȡ��Ŀ¼
	virtual bool CreateHashFile(const char *strFilePath, const char *strDirPath = "");//����hash�ļ�,������hash���浽һ���ļ�,ͬʱ���ļ�ͷ
	virtual const char * GetDirFullPath(PWpfFileList pList);//���Ŀ¼��ȫ·����
	virtual __int64 GetHashKey(const char *str);//�õ�str��hashֵ

	//��ʱhash��
	virtual bool IsBuildTempFileHashEnabled() const;
	virtual void SetBuildTempFileHashEnabled(bool val);
	virtual const map<__int64,BYTE>& GetTempFileHash() const;
	virtual void ClearTempFileHash();


//�ڲ��õ��ĺ���,Ϊ��Ч���ڲ����������Ϸ���,�ɵ��ô���֤��Ϸ���
protected:
	//��ʼ��buff,���ڼ���hashֵ
	bool InitBuffer();
	//�Ƿ����wpf�ļ�
	bool IsOpenWpfFile();
	//��ʽ��,������Open(),��Create֮�����,���ú󽫻������������
	bool WpfFormat();
	//�޷��ٷ��������ռ�ʱ���пռ���չ,���ֽ�Ϊ��λ
	bool WpfExpend(DWORD dwLen);
	//����ָ�����ȵ������հ׿�,���ؿ��,ʧ�ܷ���-1;
	PBlankBlockNode FindBlankBlock(DWORD dwNum);
	//д��һ��Ŀ¼���������ļ�����Ŀ¼����Ϣ
	bool WpfWriteFCB();
	//����Fat��m_MWpfFileList,����ʱҪ�ڴ�wpf��,�����ڲ������ж�
	bool WpfLoadFat();
	//���б����Ŀ¼
	PWpfFileList FindDirFromList(const string &strDir,PWpfFileList pList);
	//��ָ��PWpfFileList�������Ŀ¼
	PWpfFileList CreateDirToList(const string &strSubDir,PWpfFileList pList);
	//��ָ��PWpfFileList�еݹ�ɾ��Ŀ¼�����е���Ŀ¼���������ļ�
	bool DelDirFromList(const string &strSubDir,PWpfFileList pList);
	//��ָ��PWpfFileList��ɾ��ָ���ļ�
	bool DelFileFromList(const string &strFileName,PWpfFileList pList);
	//��һ���ļ����뵽ָ��Ŀ¼,strParentDir:��Ŀ¼ȫ·��,���û��"\",
	PFCB AddFileToDir(const string &strParentDir,const string &strFileName,DWORD dwLen,PWpfFileList pList,bool bFailExist);
	//����string��hashֵ
	DWORD HashString(const char * strFileName,DWORD type);
	//����·����hashֵ
	__int64 HashFileName(const char * pbKey);
	//���������ڴ漰״̬
	void WpfClear();
	//����հ��ļ���Ŀ¼
	void ClearEmptyDirAndFile(PWpfFileList pList);



protected:
	static DWORD  m_dwCryptTable[MAX_WPF_HASH_BUFFF_SIZE];//��������hashֵ
	string        m_strWpfDir,m_strWpfFileName;//wpf·����Ϣ
	NEWDIR        m_CreateDirHeader;
	NEWFCB        m_CreateFcbHeader;
	DWORD         m_dwExpendLen;//û�пհ׿ռ�ʱ��չ�ĳ���
    bool          m_bSaveWhenExpended;//��չ֮���Ƿ���������
	bool          m_bAlertError;//����֮���Ƿ񵯿򾯸�
	bool          m_bSpeedPrior;//�Ƿ��ٶ�����,�����ڴ�����
	bool          m_bClearEmptyDirAndFile;//�Ƿ������Ŀ¼�Լ����ļ�
	DWORD         m_dwAddAttributeToNewFile;//���õ����ļ���Ŀ¼ʱ�����ļ�Ӧ�ö������ӵ�����,���絼��ĳ���ļ���ʱҪ�����EFA_MEARGE_UNCOMPRESS����,�������ļ��е��ļ�����Ҫ�������

	E_WPF_RTN     m_eWrLastError;//���һ�δ������
	WpfHeader     m_WpfHeader;//�ļ�ͷ
	WpfHeader     m_WpfHeader_Bak;//�ļ�ͷ����
	WpfHeader     m_WpfHeader_Disk;//�Ѿ����浽�����е��ļ�ͷ,m_WpfHeader��ʱ�ᷢ���ı�,�������ͷָ���FCB���ļ�����,m_WpfHeader_BakӦ�ñ��m_WpfHeader_Disk,��ʱ���дm_WpfHeader_Bakʧ��,m_WpfHeader_Diskʼ������Ч��,�´α���ʱ��Ҫ����дm_WpfHeader_Bak
	PWpfFileList  m_pWpfRoot;//��Ŀ¼
	PWpfFileList  m_pDirList;//Ŀ¼�б�
	int           m_iFileHandle[EFT_NUM];//�ļ����
	PBlankBlockNode   m_pBlankBlockList;//���п������б�
	PFCB1         m_pOpenFileFcb1[EFT_NUM];//��ǰ�����ļ���FCB1
	DWORD         m_iOpenFilePointPos[EFT_NUM];//��ǰ�����ļ����ļ�ָ��λ��,����ڵ�ǰ���ļ�����ʼλ��
	bool          m_bModifyed;//�Ƿ��wpf�ļ��������޸Ĳ���,����������Ҫ�Զ�����
	bool          m_bSaving;//�Ƿ����ڱ���,������ڱ���,�����������ط�����д����,Ϊ���ٶ�����ϲ��������߳�ͬʱ����д����,��������ƻ�Ŀ¼�ṹ
	__int64       m_iWpfStartPos;//wpf�ļ��������������ļ��е���ʼλ��
	DWORD         m_dwOpenType;//���ļ�������
	MIFCB1        m_MhashFcb1;//���ļ���Ӧ��hashֵmap,ֻ������ʱ�õ�,�༭������m_pDirList
	MIFCB         m_MhashFcb;//���ļ���Ӧ��hashֵ��fcb��map,EWOT_LISTDIR��EWOT_LISTFCBHASHʱ�õ�
	PFCB1         m_pFcb1List;//fcb1�б�
	PFCB2         m_pFcb2List;//fcb1�б�
	PFCB          m_pFcbList;//fcb�б�
	bool		  m_bMeargeNotNeedUnCopress;//�ϲ��Ƿ�Ҫ��ѹ,true����ѹ

	bool			  m_bBuildTempFileHashEnabled;//�Ƿ�������ʱhash��
	map<__int64,BYTE> m_kTempFileHash;//��ʱhash��
};
