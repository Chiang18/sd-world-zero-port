#include "Stdafx.h"
#include "sdEditBrushShape.h"

namespace GameEditEx
{
//-------------------------------------------------------------------------------------------------
sdBrushDoubalCircle::sdBrushDoubalCircle()
{
	
}
//-------------------------------------------------------------------------------------------------
sdBrushDoubalCircle::~sdBrushDoubalCircle()
{

}
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
void sdBrushDoubalCircle::CreateInnerCircle()
{
	m_spInnerCircle = NiNew NiMesh;
	NIASSERT(m_spInnerCircle);

	m_spInnerCircle->SetPrimitiveType(NiPrimitiveType::PRIMITIVE_LINES);
	m_spInnerCircle->SetName("BrushInnerCircle");


}
//-------------------------------------------------------------------------------------------------
}