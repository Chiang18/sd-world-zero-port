//*************************************************************************************************
// ����:	����༭���й������
//---------------------------------------------------------
// ����:		
// ����:		2012-08-25
// ����޸�:
//*************************************************************************************************
#pragma once
using namespace System::Windows::Forms;

//
class sdWorldEditor;

namespace WorldEditorFramework 
{
	public ref class MWorldEditor
	{
	public:
		MWorldEditor();
		~MWorldEditor();

		// 
		bool	Initialize(System::IntPtr hWndPtr);
		void	Destroy();
		void	Update();

		// ������Ϣ
		void	WndProc(Message& msg);

	protected:
		sdWorldEditor*	m_pkImpl;
	};
}