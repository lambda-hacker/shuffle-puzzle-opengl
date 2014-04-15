#ifndef _VECTOR2_H_
#define _VECTOR2_H_

namespace smadani {
	class Vector2 {
	public:
		GLfloat x, y;

		Vector2() : x(0.0f), y(0.0f) { }
		Vector2(GLfloat x, GLfloat y) : x(x), y(y) { }
		Vector2(Vector2 const& rhs);
		Vector2& operator= (Vector2 const& rhs);
		Vector2& operator+= (Vector2 const& rhs);
		Vector2& operator-= (Vector2 const& rhs);
	};



	Vector2::Vector2(Vector2 const& rhs)
	{
		x = rhs.x;
		y = rhs.y;
	}
	Vector2& Vector2::operator= (Vector2 const& rhs)
	{
		if (&rhs != this) {
			x = rhs.x;
			y = rhs.y;
		}
		return *this;
	}
	Vector2& Vector2::operator+= (Vector2 const& rhs)
	{
		x += rhs.x;
		y += rhs.y;
		return *this;
	}
	Vector2& Vector2::operator-= (Vector2 const& rhs)
	{
		x -= rhs.x;
		y -= rhs.y;
		return *this;
	}
}

#endif