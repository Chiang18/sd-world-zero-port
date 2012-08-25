#include "Stdafx.h"
#include "sdTextureUtility.h"

namespace RenderSystem
{
//-------------------------------------------------------------------------------------------------
bool sdTextureUtility::IsDDSFormat(D3DFORMAT eFormat)
{
	static uint32 uiKey		= MAKEFOURCC('D', 'X', 'T', 0);
	static uint32 uiMask	= MAKEFOURCC(0xff, 0xff, 0xff, 0x00);

	return (eFormat & uiMask) == uiKey;
}
//-------------------------------------------------------------------------------------------------
bool sdTextureUtility::IsZStencilFormat(D3DFORMAT eFormat)
{
	// Depth-Stencil缓存格式列表(来自d3dtypes.h)
	static D3DFORMAT eZStencilFormatTable[] = {
		D3DFMT_D16_LOCKABLE,
		D3DFMT_D32,
		D3DFMT_D15S1,
		D3DFMT_D24S8,
		D3DFMT_D24X8,
		D3DFMT_D24X4S4,
		D3DFMT_D16,
		D3DFMT_D32F_LOCKABLE,
		D3DFMT_D24FS8 ,
	#if !defined(D3D_DISABLE_9EX)
		D3DFMT_D32_LOCKABLE,
		D3DFMT_S8_LOCKABLE,
	#endif
		};
	
	static uint uiNumZStencilFormat = sizeof(eZStencilFormatTable) / sizeof (eZStencilFormatTable[0]);

	for (uint i = 0; i < uiNumZStencilFormat; ++i)
	{
		if (eFormat == eZStencilFormatTable[i])
			return true;
	}

	return false;
}
//-------------------------------------------------------------------------------------------------
}