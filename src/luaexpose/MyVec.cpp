#include "MyVec.h"

namespace Vectors
{
	void Test( )
	{
		Vec2f t1( Vec2fZero );
		Vec2f t2( Vec2fZero );

		t1 += t2;
		t1 -= t2;
		t1 *= t2;

		t1 = t1 + t2;
		t1 = t1 - t2;
		t1 = t1 * t2;

		t1 = t2;
		t2.x = 1.0f;
		t2.y = 1.0f;

		t1 /= t2;
		t1 = t1 / t2;
	}
}
