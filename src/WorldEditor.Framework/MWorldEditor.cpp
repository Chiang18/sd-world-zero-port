#include "Stdafx.h"
#include "MWorldEditor.h"
#include "sdWorldEditor.h"


namespace WorldEditorFramework 
{
	
	MWorldEditor::MWorldEditor()
	{

	}
	
	MWorldEditor::~MWorldEditor()
	{

	}

	bool MWorldEditor::Initialize(System::IntPtr hWndPtr)
	{
		m_pkImpl = NiNew sdWorldEditor;
		NIASSERT(m_pkImpl);
		m_pkImpl->Initialize((HWND)hWndPtr.ToPointer());

		return true;
	}

	void MWorldEditor::Destroy()
	{
		
	}

	void MWorldEditor::Update()
	{
		m_pkImpl->Update();
	}

	bool MWorldEditor::SaveScene()
	{
		return m_pkImpl->SaveScene();
	}

	void MWorldEditor::WndProc(System::Windows::Forms::Message% msg)
	{
		m_pkImpl->WndProc(msg.Msg, msg.WParam.ToInt32(), msg.LParam.ToInt32());
	}
}