#include "Stdafx.h"
#include "sdMatrix3.h"

namespace Base
{
	namespace Math
	{
		const sdMatrix3 sdMatrix3::ZERO(
			0, 0, 0,
			0, 0, 0,
			0, 0, 0);

		const sdMatrix3 sdMatrix3::IDENTITY(
			1, 0, 0,
			0, 1, 0,
			0, 0, 1);
	}
}