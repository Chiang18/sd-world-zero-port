//*************************************************************************************************
// 内容:	世界编辑器托管类入口
//---------------------------------------------------------
// 作者:		
// 创建:		2012-08-25
// 最后修改:
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

		// 输入消息
		void	WndProc(Message& msg);

	protected:
		sdWorldEditor*	m_pkImpl;
	};
}