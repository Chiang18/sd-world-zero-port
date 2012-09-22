//*************************************************************************************************
// ����:	�¼�����,��Ҫ����C#/CLR֮��ͨ��
//---------------------------------------------------------
// ����:		
// ����:		2012-08-28
// ����޸�:
//*************************************************************************************************
#pragma once

namespace WorldEditorFramework 
{
	
	public ref class MEventManager
	{
	public:
		__event void AppOpening();
		__event void AppClosing();

		__event void WorldOpening();
		__event void WorldClosing();

		__event void SceneNew();
		__event void SceneOpening();
		__event void SceneSaving();
		__event void SceneClosing();

		void RaiseAppOpening();
		void RaiseAppClosing();

		void RaiseWorldOpening();
		void RaiseWorldClosing();

		void RaisSceneNew();
		void RaisSceneOpening();
		void RaisSceneSaving();
		void RaisSceneClosing();
	};
}