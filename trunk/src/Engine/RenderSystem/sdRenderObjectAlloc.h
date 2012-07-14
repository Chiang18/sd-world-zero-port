//*************************************************************************************************
// ����:	��Ⱦ�����ڴ������
//---------------------------------------------------------
// ����:		��ǿ
// ����:		2012-06-30
// ����޸�:
//*************************************************************************************************
#pragma once
#ifndef _RENDER_OBJECT_ALLOC_H__
#define _RENDER_OBJECT_ALLOC_H__

namespace RenderSystem
{

// ��Ⱦ�����ڴ������(һ���򵥵��ڴ������)
//  1.Ԥ�ȷ������ڴ�,��ʹ��ʱֱ������Щ�ڴ���������,�������Լ����ڴ���Ƭ
//  2.��������Ⱦ���ڲ�ÿ֡֡��ʹ�õĶ���
//	3.����֧��˳������ȫ������,��֧���ض����������
//  4.֧���Զ�����,�������ӵ��ڴ���������ʽ��������
class sdRenderObjectAlloc : public NiRefObject
{
public:
	sdRenderObjectAlloc(unsigned int uiCapicaty, unsigned int uiIncreaseStep);
	~sdRenderObjectAlloc();

	void*	Alloc(unsigned int uiSize);
	void	DeallocAll();

	unsigned int Capacity() const;
	unsigned int Size() const;
	unsigned int NumObjects() const;

protected:
	char*					m_pcBuffer;
	unsigned int			m_uiCapacity;	// Buffer��С,byte
	unsigned int			m_uiSize;		// ��ʹ�ô�С,byte
	unsigned int			m_uiNumObject;	// �ѷ���Ķ�����Ŀ;

	unsigned int			m_uiIncreaseStep;
	sdRenderObjectAlloc*	m_pkIncreaseChild;
};

}

#endif