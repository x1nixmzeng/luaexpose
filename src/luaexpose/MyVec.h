/*
	MyVec
	Written by x1nixmzeng

	Portable 2D/3D vector template with overloaded operators for common operations
*/
#ifndef _LEVEC_H_
#define _LEVEC_H_

namespace Vectors
{
	// -- 2D Vector
	template<class T>
	union Vec2
	{
		struct
		{
			T x, y;
		};

		T xy[2];

		#include "Vec2.inl"
	};

	// Some default instances with common types
	typedef Vec2<float>		Vec2f;
	typedef Vec2<int>		Vec2i;

	static Vec2f Vec2fZero( 0.0f );
	static Vec2i Vec2iZero( 0 );

	// -- 3D Vector
	template<class T>
	union Vec3
	{
		struct
		{
			T x, y, z;//, _padding;
		};

		T xyz[3];

		#include "Vec3.inl"
	};

	// Some default instances with common types
	typedef Vec3<float>		Vec3f;
	typedef Vec3<int>		Vec3i;

	static Vec3f Vec3fZero( 0.0f );
	static Vec3i Vec3iZero( 0 );

	void Test( );
}

#endif
