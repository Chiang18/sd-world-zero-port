#include "Stdafx.h"
#include "sdMath.h"

namespace Base
{
	namespace Math
	{
		const float sdMath::POS_INFINITY	= std::numeric_limits<float>::infinity();
		const float sdMath::NEG_INFINITY	= -std::numeric_limits<float>::infinity();
		const float sdMath::PI				= float(4.0f * atan(1.0f));
		const float sdMath::TWO_PI			= float(2.0f * PI);
		const float sdMath::HALF_PI			= float(0.5f * PI);
		const float sdMath::Deg2Rad			= PI / float(180.0f);
		const float sdMath::Rad2Deg			= float(180.0f) / PI;
	}
}