#include "Stdafx.h"
#include "sdResourceSystem.h"

namespace ResourceSystem
{
//-------------------------------------------------------------------------------------------------
sdResourceSystem::sdResourceSystem()
{

}
//-------------------------------------------------------------------------------------------------
sdResourceSystem::~sdResourceSystem()
{

}
//-------------------------------------------------------------------------------------------------
bool sdResourceSystem::Initialize()
{
	// ��ʼ�������ȡ
	NiDevImageConverter* pkDevImageConverter = NiNew NiDevImageConverter;
	NIASSERT(pkDevImageConverter);
	//pkDevImageConverter->AddReader(NiNew NiPNGReader);
	NiDevImageConverter::SetImageConverter(pkDevImageConverter);


	return true;
}
//-------------------------------------------------------------------------------------------------
void sdResourceSystem::Destroy()
{

}
//-------------------------------------------------------------------------------------------------
void sdResourceSystem::Update()
{

}
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
}