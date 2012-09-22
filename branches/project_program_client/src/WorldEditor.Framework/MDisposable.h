//*************************************************************************************************
// ����:	�ֶ��ͷ���Դ�ӿ�
//---------------------------------------------------------
// ����:		
// ����:		2012-09-21
// ����޸�:
//*************************************************************************************************
#pragma once

namespace WorldEditorFramework 
{
	// ��.NET�Ķ�����ʵ���������������ͷ���Դ�ĺ�����Dispose��Finalize��
	//		Finalize��Ŀ���������ͷŷ��йܵ���Դ��
	//		Dispose�������ͷ�������Դ�������йܵĺͷ��йܵġ�
	public ref class MDisposable abstract : public System::IDisposable
	{
	public:
		MDisposable();
		!MDisposable();
		virtual ~MDisposable();	

	protected:
		// �ͷ���Դ������ִ�к���
		virtual void Do_Dispose(bool bDisposing){};	
		
	private:
		bool m_bDisposed;
	};
}