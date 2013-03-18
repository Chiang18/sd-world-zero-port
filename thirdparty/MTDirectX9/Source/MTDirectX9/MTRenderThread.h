//*************************************************************************************************
// 内容:	多线程渲染类
//---------------------------------------------------------
// 作者:		
// 创建:		2013-03-15
// 最后修改:	
//*************************************************************************************************
#ifndef MTRENDERTHREAD_HEAD_FILE
#define MTRENDERTHREAD_HEAD_FILE

#include "MTDoubleBuffer.h"
#include "MTThread.h"

//-------------------------------------------------------------------------------------------------
// 当前渲染线程状态
//-------------------------------------------------------------------------------------------------
enum enumRenderThreadState
{
	enRTS_UNKNOWN,
	enRTS_Waiting,				// 等待,线程暂停
	enRTS_Rendering,			// 渲染中
	enRTS_RenderComplated,		// 渲染完毕
	enRTS_EventResetComplated,	
};
//-------------------------------------------------------------------------------------------------
// 多线程渲染类
//-------------------------------------------------------------------------------------------------
class MT_RenderThread : public MT_Thread, public Singleton<MT_RenderThread>
{
public:
	MT_RenderThread(IDirect3DDevice9* pDevice = NULL);
	virtual	~MT_RenderThread();

	// 交换
	HRESULT	Swap();

	// 同步,主要用于数据读取
	void Sync();

	// 线程函数
	virtual bool RepetitionRun();

	// 获取渲染命令缓存
	MT_DoubleBuffer& GetDBuffer() { return m_DBuffer;};

	// 获取当前渲染线程状态
	enumRenderThreadState GetRenderThreadState(){ return m_State;}

	virtual bool StopThread();

	void SetDevice(IDirect3DDevice9* pDevice){ m_pDevice = pDevice;}

	void SetDeviceLost(bool	bDeviceLost){ m_bDeviceLost	= bDeviceLost;}
	bool IsDeviceLost() { return m_bDeviceLost;}
	
	void WaitRenderComplated();

	// 主线程Present的时候调用 用于主线程执行所有渲染指令
	void DebugRenderThread();

	// 释放渲染对象
	void ReleaseAllObject();
	void ReleaseWriteObject();

	// 主线程调用,创建顶点声明(继承类)
	MT_IDirect3DVertexDeclaration9*	CreateVD(CONST D3DVERTEXELEMENT9* pVertexElements);
protected:
	void RenderFrame();
	void DebugFrame(U8* pBuffer, U8* pEnd);

	void ReleaseRenderObject();
	void ReleaseObject(ReleaseObj& obj);

protected:
	IDirect3DDevice9* m_pDevice;

	// 线程同步对象
	Event m_evtRenderWaitMain;
	Event m_evtMainWaitRender;

	// 渲染命令缓存
	MT_DoubleBuffer m_DBuffer;

	// 渲染线程状态
	enumRenderThreadState m_State;
	bool m_bDeviceLost;

	// 当前已有的各种IDirect3DVertexDeclaration9列表
	typedef std::vector<MT_IDirect3DVertexDeclaration9*> MTD3DVD9Vec;
	typedef std::vector<MT_IDirect3DVertexDeclaration9*>::iterator MTD3DVD9VecItr;
	MTD3DVD9Vec m_kMTD3DVD9Vec;
};
//-------------------------------------------------------------------------------------------------
// Hook IDirect3DDevice9
//-------------------------------------------------------------------------------------------------
void Hook_IDirect3DDevice9(IDirect3DDevice9* pDevice);
void UnHook_IDirect3DDevice9();
//-------------------------------------------------------------------------------------------------
#endif