//*************************************************************************************************
// 内容:	渲染对象内存分配器
//---------------------------------------------------------
// 作者:		喻强
// 创建:		2012-06-30
// 最后修改:
//*************************************************************************************************
#pragma once
#ifndef _RENDER_OBJECT_ALLOC_H__
#define _RENDER_OBJECT_ALLOC_H__

namespace RenderSystem
{

// 渲染对象内存分配器(一个简单的内存分配器)
//  1.预先分配大块内存,在使用时直接在这些内存上来分配,这样可以减少内存碎片
//  2.仅仅供渲染器内部每帧帧内使用的对象
//	3.仅仅支持顺序分配和全部销毁,不支持特定对象的销毁
//  4.支持自动增加,这里增加的内存以链表形式链接起来
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
	unsigned int			m_uiCapacity;	// Buffer大小,byte
	unsigned int			m_uiSize;		// 已使用大小,byte
	unsigned int			m_uiNumObject;	// 已分配的对象数目;

	unsigned int			m_uiIncreaseStep;
	sdRenderObjectAlloc*	m_pkIncreaseChild;
};

}

#endif