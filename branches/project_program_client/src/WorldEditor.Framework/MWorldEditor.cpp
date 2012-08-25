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

	void MWorldEditor::WndProc(Message& msg)
	{
		int cc = 0;
	}
}