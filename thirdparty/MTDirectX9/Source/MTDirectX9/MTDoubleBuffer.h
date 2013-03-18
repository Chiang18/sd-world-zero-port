//*************************************************************************************************
// ����:	��Ⱦ�����,ֻ���𱣴�����(������Ⱦ�����ʹ��ͷŶ����б�)
//---------------------------------------------------------
// ����:		
// ����:		2013-03-07
// ����޸�:	2013-03-15
//*************************************************************************************************
#ifndef MTDOUBLEBUFFER_HEAD_FILE
#define	MTDOUBLEBUFFER_HEAD_FILE

#include "MTHeader.h"
//-------------------------------------------------------------------------------------------------
class MT_DoubleBuffer
{
public:
	MT_DoubleBuffer();
	virtual	~MT_DoubleBuffer();

	// ���̵߳���
	// @{
	// ����ǰ����,���л�����һ������
	virtual	void Sync();

	// ����д�뻺��
	void EmptyWriteBufer();

	// ����ָ���ߴ绺��
	virtual	void* Request(U32 dwSize);

	// ����ָ����Ⱦ����
	// void Request(enumCommandFunc func)						��������
	// template<typename T> T* Request(enumCommandFunc func)	��������+���뻺��
	// template<typename T> T* Request(enumCommandFunc func, U32 uiBufferSize, void*& pOutBuffer) ��������+���뻺��+�������
	void Request(enumCommandFunc func)
	{
		enumCommandFunc* pFunc = (enumCommandFunc*)Request(sizeof(enumCommandFunc));
		*pFunc = func;
	}

	template<typename T>
	T*	Request(enumCommandFunc	func)
	{
		U8*	pStart = (U8*)Request(sizeof(T) + sizeof(enumCommandFunc));
		enumCommandFunc* pFunc = (enumCommandFunc*)pStart;
		*pFunc = func;
		return (T*)(pStart + sizeof(enumCommandFunc));
	};

	template<typename T>
	T*	Request(enumCommandFunc	func, U32 uiBufferSize, void*& pOutBuffer)
	{
		U8*	pStart = (U8*)Request(sizeof(T) + sizeof(enumCommandFunc) + uiBufferSize);
		enumCommandFunc* pFunc = (enumCommandFunc*)pStart;
		*pFunc = func;
		pStart += sizeof(enumCommandFunc);
		T* pRet	= (T*)pStart;
		pStart += sizeof(T);
		pOutBuffer = pStart;
		return	pRet;
	};
	// @}

	// ��Ⱦ�̵߳���
	// @{
	virtual	void* Read(U32 dwSize);
	// @}

	void PushReleaseObj(const ReleaseObj& kObj);	///< ���̵߳���,д��Ҫ�ͷŵ���Դ
	ReleaseObjVector& GetWriteReleaseObjVector();	///< ���̵߳���
	ReleaseObjVector& GetReadReleaseObjVector();	///< ��Ⱦ�̵߳���

protected:
	// ���ڻ��治��ʱ,���·���˫������
	void ReAlloc(U32 uiSize);

public:
	// ��Ⱦ�������
	typedef	std::vector<U8>	Buffer;
	Buffer	m_Buffer[2];	///< �����
	U32	m_BufferOffset[2];	///< ������ȡ/д������
	U32	m_BufferSize[2];	///< �����ߴ�
	U32	m_uiWriteIndex;		///< ��ǰ���̻߳���

	// ���ͷŶ������
	ReleaseObjVector m_kReleaseObjVec[2];
};
//-------------------------------------------------------------------------------------------------
#endif