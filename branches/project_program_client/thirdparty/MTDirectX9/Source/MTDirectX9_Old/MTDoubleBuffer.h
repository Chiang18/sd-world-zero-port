#ifndef MTDOUBLEBUFFER_HEAD_FILE
#define	MTDOUBLEBUFFER_HEAD_FILE

#include "MTHeader.h"


// 渲染命令双缓存,用于保存主线程渲染命令
class MT_DoubleBuffer
{
public:
	MT_DoubleBuffer();
	virtual	~MT_DoubleBuffer();

	// 主线程调用
	// @{
	// 清理当前缓存,并切换到下一个缓存
	virtual	void	Sync();

	// 清理写入缓存
	void	EmptyWriteBufer();

	// 分配指定尺寸缓存
	virtual	void*	Request(U32 dwSize);

	// 分配指定渲染命令
	void	Request(enumCommandFunc	func)
	{
		enumCommandFunc*	pFunc	=	(enumCommandFunc*)Request(sizeof(enumCommandFunc));
		*pFunc	=	func;
	}

	template<typename T>
	T*	Request(enumCommandFunc	func)
	{
		U8*	pStart					=	(U8*)Request(sizeof(T)+sizeof(enumCommandFunc));
		enumCommandFunc*	pFunc	=	(enumCommandFunc*)pStart;
		*pFunc	=	func;
		return	(T*)(pStart+sizeof(enumCommandFunc));
	};

	template<typename T>
	T*	Request(enumCommandFunc	func,U32 uiBufferSize,void*&	pOutBuffer){
		U8*	pStart					=	(U8*)Request(sizeof(T)+sizeof(enumCommandFunc) + uiBufferSize);
		enumCommandFunc*	pFunc	=	(enumCommandFunc*)pStart;
		*pFunc	=	func;
		pStart+=sizeof(enumCommandFunc);
		T*	pRet	=	(T*)pStart;
		pStart+=sizeof(T);
		pOutBuffer	=	pStart;
		return	pRet;
	};
	// @}

	// 渲染线程调用
	// @{
	virtual	void*	Read(U32 dwSize);
	// @}

	void				PushReleaseObj(const ReleaseObj&	pObj);
	ReleaseObjVector&	GetWriteReleaseObjVector();
	ReleaseObjVector&	GetReadReleaseObjVector();

protected:
	// 用于缓存不够时,重新分配双倍缓存
	void ReAlloc(U32 uiSize);

public:
	// 命令队列
	typedef	std::vector<U8>	Buffer;
	Buffer	m_Buffer[2];
	U32	m_BufferOffset[2];
	U32	m_BufferSize[2];
	U32	m_uiWriteIndex;

	// 对象队列
	ReleaseObjVector	m_vecReleaseObj[2];
};

#endif