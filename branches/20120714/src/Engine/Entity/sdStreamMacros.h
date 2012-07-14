//*************************************************************************************************
// 内容:	
//---------------------------------------------------------
// 作者:		
// 创建:		2012-07-10
// 最后修改:
//*************************************************************************************************
#pragma once
#ifndef _STREAMING_MACRO_H__
#define _STREAMING_MACRO_H__

// 额,貌似是懒得实现GB的Streaming的那几个函数,直接调用父类来替代
#define sdImplementDefaultStream(classname)					\
	void classname::LoadBinary(NiStream& kStream)			\
	{														\
		__super::LoadBinary(kStream);						\
	}														\
															\
	void classname::LinkObject(NiStream& kStream)			\
	{														\
		__super::LinkObject(kStream);						\
	}														\
															\
	bool classname::RegisterStreamables(NiStream& kStream)	\
	{														\
		return __super::RegisterStreamables(kStream);		\
	}														\
															\
	void classname::SaveBinary(NiStream& kStream)			\
	{														\
		__super::SaveBinary(kStream);						\
	}														\
															\
	bool classname::IsEqual(NiObject* pkObject)				\
	{														\
		return __super::IsEqual(pkObject);							\
	}														\
															\
	NiObject* classname::CreateObject(const char** pcArguments, unsigned int uiArgCount)	\
	{														\
		classname* pkObject = NiNew classname;				\
		NIASSERT(pkObject);									\
		return pkObject;									\
	}														

// 额,这个貌似直接都是空实现
#define sdImplementAbstractStream(classname)				\
	void classname::LoadBinary(NiStream& kStream){}			\
	void classname::LinkObject(NiStream& kStream){}			\
	void classname::RegisterStreamables(NiStream& kStream)	\
	{														\
		return true;										\
	}														\
	void classname::SaveBinary(NiStream& kStream){}			\
	bool classname::IsEqual(NiObject* pkObject)				\
	{														\
		return __super::IsEqual(pkObject);							\
	}														\

#endif