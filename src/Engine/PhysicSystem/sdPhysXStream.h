//*************************************************************************************************
// 内容:	PhysX的数据流
//---------------------------------------------------------
// 作者:		
// 创建:		2012-08-17
// 最后修改:
//*************************************************************************************************
#pragma once
#ifndef _SD_ENGINE_PHYSX_STREAM_H__
#define _SD_ENGINE_PHYSX_STREAM_H__
#include <sdEngine.h>

namespace PhysicsSystem
{

	// PhysX的数据流
	class sdPhysXStream : public NxStream, public NiMemObject
	{
	public:
	

	protected:


	};

	class sdPhysXMemoryWriteStream : public NxStream, public NiMemObject
	{
	public:


	protected:


	};

	class sdPhysXMemoryReadStream : public NxStream, public NiMemObject
	{
	public:


	protected:


	};
}
#endif