//*************************************************************************************************
// ����:	
//---------------------------------------------------------
// ����:		
// ����:		2012-07-10
// ����޸�:
//*************************************************************************************************
#pragma once
#ifndef _STREAMING_MACRO_H__
#define _STREAMING_MACRO_H__

// ��,ò��������ʵ��GB��Streaming���Ǽ�������,ֱ�ӵ��ø��������
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

// ��,���ò��ֱ�Ӷ��ǿ�ʵ��
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