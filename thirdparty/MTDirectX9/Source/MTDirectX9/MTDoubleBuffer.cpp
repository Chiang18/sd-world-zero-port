#include "MTDoubleBuffer.h"
#include "MTRenderThread.h"

// 命令缓存大小
#define	DEFAULT_BUFFER_SIZE	4 * 1024 * 1024

// 主线程
extern DWORD g_uiMainThreadId;

//-------------------------------------------------------------------------------------------------
MT_DoubleBuffer::MT_DoubleBuffer()
{
	m_BufferSize[0]	= DEFAULT_BUFFER_SIZE;
	m_BufferSize[1]	= DEFAULT_BUFFER_SIZE;

	m_Buffer[0].resize(DEFAULT_BUFFER_SIZE);
	m_Buffer[1].resize(DEFAULT_BUFFER_SIZE);

	m_BufferOffset[0] =	0;
	m_BufferOffset[1] =	0;

	m_uiWriteIndex = 0;
};
//-------------------------------------------------------------------------------------------------
MT_DoubleBuffer::~MT_DoubleBuffer()
{

};
//-------------------------------------------------------------------------------------------------
void MT_DoubleBuffer::EmptyWriteBufer()
{
	U32& uiOffset = m_BufferOffset[m_uiWriteIndex];
	uiOffset = 0;
}
//-------------------------------------------------------------------------------------------------
void* MT_DoubleBuffer::Request(U32 dwSize)
{
	//if(GetCurrentThreadId()!=g_uiMainThreadId)
	//{
	//	::OutputDebugStringA("More than One Thread Push Command!\n");
	//	//MessageBoxA(NULL, "More than One Thread Push Command!", NULL, NULL);
	//}

	U32& uiOffset =	m_BufferOffset[m_uiWriteIndex];

	// 检查剩余缓存是否足够,不够则重新分配
	if(uiOffset	+ dwSize > m_BufferSize[m_uiWriteIndex])
		ReAlloc(uiOffset + dwSize);

	// 当前缓存
	void* p	= &m_Buffer[m_uiWriteIndex][uiOffset];
	uiOffset +=	dwSize;

	return p;
};
//-------------------------------------------------------------------------------------------------
void* MT_DoubleBuffer::Read(U32 dwSize)
{
	U32 uiReadIndex	= (m_uiWriteIndex+1)&1;
	U32& uiReadOffset =	m_BufferOffset[uiReadIndex];	///< 当前读取偏移
	Buffer&	uiReadBuffer = m_Buffer[uiReadIndex];		///< 当前读取缓存
	
	void* p = &uiReadBuffer[uiReadOffset];
	uiReadOffset += dwSize;
	return p;
};
//-------------------------------------------------------------------------------------------------
void MT_DoubleBuffer::Sync()
{
	GetReadReleaseObjVector().clear();	///< 清理待释放对象

	m_uiWriteIndex = (m_uiWriteIndex+1)&1;
	m_BufferOffset[0] =	0;
	m_BufferOffset[1] =	0;
};
//-------------------------------------------------------------------------------------------------
void MT_DoubleBuffer::ReAlloc(U32 uiSize)
{
	U32	uiOldSize =	m_Buffer[m_uiWriteIndex].size();
	U32	uiNewSize =	uiOldSize<<1;
	while (uiNewSize <= uiSize)
		uiNewSize = uiNewSize<<1;
	m_Buffer[m_uiWriteIndex].resize(uiNewSize);
	m_BufferSize[m_uiWriteIndex] = uiNewSize;
}
//-------------------------------------------------------------------------------------------------
void MT_DoubleBuffer::PushReleaseObj(const ReleaseObj&	pObj)
{
	//if (GetCurrentThreadId() != g_uiMainThreadId)
	//{
	//	::OutputDebugStringA("More than One Thread PushReleaseObj\n");
	//	//MessageBoxA(NULL,"More than One Thread PushReleaseObjn", NULL, NULL);
	//}

	//if (MT_RenderThread::GetSingleton().IsDeviceLost())
	//{
	//	::OutputDebugStringA("Device Lost PushReleaseObj\n");
	//}

	//ReleaseObj obj;
	//obj.func	= func;
	//obj.pObj	= pObj;
	//obj.pFunc = NULL;
	//switch(func)
	//{
	//	case enCF_Surface_Release:
	//	{
	//							 
	//	}break;
	//}

	GetWriteReleaseObjVector().push_back(pObj);
}
//-------------------------------------------------------------------------------------------------
ReleaseObjVector& MT_DoubleBuffer::GetWriteReleaseObjVector()
{
	return m_kReleaseObjVec[m_uiWriteIndex];
};
//-------------------------------------------------------------------------------------------------
ReleaseObjVector& MT_DoubleBuffer::GetReadReleaseObjVector()
{
	U32 uiReadIndex	= (m_uiWriteIndex+1)&1;
	return	m_kReleaseObjVec[uiReadIndex];
};
//-------------------------------------------------------------------------------------------------