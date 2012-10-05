//*************************************************************************************************
// 内容:	内建的Streaming与RTTI基类
//---------------------------------------------------------
// 作者:		
// 创建:		2012-07-10
// 最后修改:
//*************************************************************************************************
#pragma once
#ifndef _SD_ENGINE_SCENE_GAME_OBJECT_H__
#define _SD_ENGINE_SCENE_GAME_OBJECT_H__

// 目前暂时继承自NiNode, 后期整理内存结构时去掉
class sdGameObject : public NiNode
{
	NiDeclareRTTI;
	NiDeclareStream;

public:
	sdGameObject();
	virtual ~sdGameObject();

protected:


};

#endif