#include "stdafx.h"
#include "sdRenderObjectAlloc.h"

namespace RenderSystem
{

//-------------------------------------------------------------------------------------------------
sdRenderObjectAlloc::sdRenderObjectAlloc(unsigned int uiCapicaty, unsigned int uiIncreaseStep)
: m_uiCapacity(uiCapicaty)
, m_uiSize(0)
, m_uiNumObject(0)
, m_uiIncreaseStep(uiIncreaseStep)
, m_pkIncreaseChild(NULL)
{
	m_pcBuffer = new char[uiCapicaty];
	NIASSERT(m_pcBuffer);
}
//-------------------------------------------------------------------------------------------------
sdRenderObjectAlloc::~sdRenderObjectAlloc()
{
	delete m_pkIncreaseChild;
	m_pkIncreaseChild = NULL;

	delete [] m_pcBuffer;
	m_pcBuffer = NULL;

	m_uiCapacity = 0;
	m_uiSize = 0;
	m_uiNumObject = 0;
	m_uiIncreaseStep = 0;
}
//-------------------------------------------------------------------------------------------------
void* sdRenderObjectAlloc::Alloc(unsigned int uiSize)
{
	if (m_uiSize + uiSize > m_uiCapacity)
	{
		if (NULL == m_pkIncreaseChild)
		{
			unsigned int uiCapacity = m_uiIncreaseStep;
			if (uiCapacity < uiSize)
			{
				// 所要求分配对象过大,超出内存增加步长,则增大到所需求的大小
				NIASSERT(0);
				uiCapacity = uiSize;
			}

			m_pkIncreaseChild = NiNew sdRenderObjectAlloc(uiCapacity, m_uiIncreaseStep);
		}

		return m_pkIncreaseChild->Alloc(uiSize);
	}

	void* pResult = &m_pcBuffer[m_uiSize];
	m_uiSize += uiSize;
	++m_uiNumObject;

	return pResult;
}
//-------------------------------------------------------------------------------------------------
void sdRenderObjectAlloc::DeallocAll()
{
	delete m_pkIncreaseChild;
	m_pkIncreaseChild = NULL;

	m_uiSize = 0;
	m_uiNumObject = 0;
}
//-------------------------------------------------------------------------------------------------
unsigned int sdRenderObjectAlloc::Capacity() const
{
	unsigned int uiCapacity = m_uiCapacity;
	if (m_pkIncreaseChild)
		uiCapacity += m_pkIncreaseChild->Capacity();
	return uiCapacity;
}
//-------------------------------------------------------------------------------------------------
unsigned int sdRenderObjectAlloc::Size() const
{
	unsigned int uiSize = m_uiSize;
	if (m_pkIncreaseChild)
		uiSize += m_pkIncreaseChild->Size();
	return uiSize;
}
//-------------------------------------------------------------------------------------------------
unsigned int sdRenderObjectAlloc::NumObjects() const
{
	unsigned int uiNumObject = m_uiNumObject;
	if (m_pkIncreaseChild)
		uiNumObject += m_pkIncreaseChild->NumObjects();
	return uiNumObject;
}
//-------------------------------------------------------------------------------------------------
}