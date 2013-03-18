//*************************************************************************************************
// ����:	���߳���Ⱦ��
//---------------------------------------------------------
// ����:		
// ����:		2013-03-15
// ����޸�:	
//*************************************************************************************************
#ifndef MTRENDERTHREAD_HEAD_FILE
#define MTRENDERTHREAD_HEAD_FILE

#include "MTDoubleBuffer.h"
#include "MTThread.h"

//-------------------------------------------------------------------------------------------------
// ��ǰ��Ⱦ�߳�״̬
//-------------------------------------------------------------------------------------------------
enum enumRenderThreadState
{
	enRTS_UNKNOWN,
	enRTS_Waiting,				// �ȴ�,�߳���ͣ
	enRTS_Rendering,			// ��Ⱦ��
	enRTS_RenderComplated,		// ��Ⱦ���
	enRTS_EventResetComplated,	
};
//-------------------------------------------------------------------------------------------------
// ���߳���Ⱦ��
//-------------------------------------------------------------------------------------------------
class MT_RenderThread : public MT_Thread, public Singleton<MT_RenderThread>
{
public:
	MT_RenderThread(IDirect3DDevice9* pDevice = NULL);
	virtual	~MT_RenderThread();

	// ����
	HRESULT	Swap();

	// ͬ��,��Ҫ�������ݶ�ȡ
	void Sync();

	// �̺߳���
	virtual bool RepetitionRun();

	// ��ȡ��Ⱦ�����
	MT_DoubleBuffer& GetDBuffer() { return m_DBuffer;};

	// ��ȡ��ǰ��Ⱦ�߳�״̬
	enumRenderThreadState GetRenderThreadState(){ return m_State;}

	virtual bool StopThread();

	void SetDevice(IDirect3DDevice9* pDevice){ m_pDevice = pDevice;}

	void SetDeviceLost(bool	bDeviceLost){ m_bDeviceLost	= bDeviceLost;}
	bool IsDeviceLost() { return m_bDeviceLost;}
	
	void WaitRenderComplated();

	// ���߳�Present��ʱ����� �������߳�ִ��������Ⱦָ��
	void DebugRenderThread();

	// �ͷ���Ⱦ����
	void ReleaseAllObject();
	void ReleaseWriteObject();

	// ���̵߳���,������������(�̳���)
	MT_IDirect3DVertexDeclaration9*	CreateVD(CONST D3DVERTEXELEMENT9* pVertexElements);
protected:
	void RenderFrame();
	void DebugFrame(U8* pBuffer, U8* pEnd);

	void ReleaseRenderObject();
	void ReleaseObject(ReleaseObj& obj);

protected:
	IDirect3DDevice9* m_pDevice;

	// �߳�ͬ������
	Event m_evtRenderWaitMain;
	Event m_evtMainWaitRender;

	// ��Ⱦ�����
	MT_DoubleBuffer m_DBuffer;

	// ��Ⱦ�߳�״̬
	enumRenderThreadState m_State;
	bool m_bDeviceLost;

	// ��ǰ���еĸ���IDirect3DVertexDeclaration9�б�
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