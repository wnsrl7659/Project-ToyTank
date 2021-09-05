/******************************************************************************
  * Author : Cody Morgan
  * Class  : GAM 300
  * Brief  :

  (c) DigiPen Institute of Technology 2019
******************************************************************************/

#pragma once

namespace Util
{
	struct Vec2F
	{
		float x, y;
		float squareLength;
		float length;

		// constructor and destructor
		Vec2F() : x(0.0f), y(0.0f) {};
		Vec2F(float x, float y) : x(x), y(y) { squareLength = getSquareLength(); };
		~Vec2F() {};

		// get square length
		float getSquareLength();

		// get length
		float getLength();

		// normalize vector
		void normalize(float l = -1.0f);

		// overload operators
		Vec2F& operator*=(const float);
	};
}

struct vec3
{
  vec3(float x = 0.0f, float y = 0.0f, float z = 0.0f) :x(x), y(y), z(z)
  {}
  vec3& operator-=(vec3 const& rhs) {
    this->x -= rhs.x;
    this->y -= rhs.y;
    this->z -= rhs.z;
    return *this;
  }
  vec3& operator+=(vec3 const& rhs) {
    this->x += rhs.x;
    this->y += rhs.y;
    this->z += rhs.z;
    return *this;
  }
  vec3 operator-(vec3 const& rhs) {
    return vec3(this->x - rhs.x,
      this->y - rhs.y,
      this->z - rhs.z);
  }
  vec3 operator+(vec3 const& rhs) {
    return vec3(this->x + rhs.x,
      this->y + rhs.y,
      this->z + rhs.z);
  }

  vec3& operator +=(double scalar)
  {
    this->x += scalar;
    this->y += scalar;
    this->z += scalar;
    return *this;
  }
  vec3 operator * (double scale)
  {
    this->x *= scale;
    this->y *= scale;
    this->z *= scale;
    return *this;
  }


  void normalize();
  float getMagnitude();
  float getSquareLength() const;

  float x;
  float y;
  float z;
};