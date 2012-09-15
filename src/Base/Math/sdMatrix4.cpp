#include "Stdafx.h"
#include "sdMatrix4.h"

namespace Base
{
	namespace Math
	{
		const sdMatrix4 sdMatrix4::ZERO(
			0, 0, 0, 0,
			0, 0, 0, 0,
			0, 0, 0, 0,
			0, 0, 0, 0);

		const sdMatrix4 sdMatrix4::ZEROAFFINE(
			0, 0, 0, 0,
			0, 0, 0, 0,
			0, 0, 0, 0,
			0, 0, 0, 1);

		const sdMatrix4 sdMatrix4::IDENTITY(
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1);
	}
}