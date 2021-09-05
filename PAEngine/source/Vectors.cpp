#include "pch.h"
#include "Vectors.h"

#include <math.h>

namespace Util
{	
	// square length
	float Vec2F::getSquareLength()
	{
		squareLength = x * x + y * y;
		return squareLength;
	}

	// length
	float Vec2F::getLength()
	{
		length = sqrtf(x * x + y * y);
		return length;
	}

	void Vec2F::normalize(float l)
	{
		if (l == -1)
		{
			// no length provided -> compute it
			l = getLength();
		}

		// normalize vector
		x = x / l;
		y = y / l;
	}

	Vec2F& Vec2F::operator*=(const float a)
	{
		x *= a;
		y *= a;

		return *this;
	}

}

void vec3::normalize()
{
	float size = sqrt(x * x + y * y + z * z);
	x /= size;
	y /= size;
	z /= size;

}

float vec3::getMagnitude()
{
	return sqrt(x * x + y * y + z * z);
}

float vec3::getSquareLength() const
{
  return x*x + y*y + z*z;
}
