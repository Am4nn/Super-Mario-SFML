#include "Vec2.h"

Vec2::Vec2() {}

Vec2::Vec2(float xin, float yin)
	: x(xin), y(yin)
{}

bool Vec2::operator == (const Vec2& rhs) const
{
	return (x == rhs.x && y == rhs.y);
}

bool Vec2::operator != (const Vec2& rhs) const
{
	return (x != rhs.x || y != rhs.y);
}

Vec2 Vec2::operator + (const Vec2& rhs) const
{
	return Vec2(x + rhs.x, y + rhs.y);
}

Vec2 Vec2::operator - (const Vec2& rhs) const
{
	return Vec2(x - rhs.x, y - rhs.y);
}

Vec2 Vec2::operator * (const float val) const
{
	return Vec2(x * val, y * val);
}

Vec2 Vec2::operator / (const float val) const
{
	return Vec2(x / val, y / val);
}

void Vec2::operator += (const Vec2& rhs)
{
	x += rhs.x;
	y += rhs.y;
}

void Vec2::operator -= (const Vec2& rhs)
{
	x -= rhs.x;
	y -= rhs.y;
}

void Vec2::operator *= (const float val)
{
	x *= val;
	y *= val;
}

void Vec2::operator /= (const float val)
{
	x /= val;
	y /= val;
}

float Vec2::dist(const Vec2& rhs) const
{
	return sqrt((x - rhs.x) * (x - rhs.x) + (y - rhs.y) * (y - rhs.y));
}

float Vec2::distSq(const Vec2& rhs) const
{
	return ((x - rhs.x) * (x - rhs.x) + (y - rhs.y) * (y - rhs.y));
}

Vec2& Vec2::normalize()
{
	float mag = sqrt((x * x) + (y * y));
	if (mag != 0)
	{
		x /= mag;
		y /= mag;
	}
	return *this;
}

Vec2& Vec2::abs()
{
	x = std::abs(x);
	y = std::abs(y);
	return *this;
}