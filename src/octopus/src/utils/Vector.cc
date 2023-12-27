#include "Vector.hh"

#include <cmath>

octopus::Vector::Vector()
	: x(0.) , y(0.)
{}
octopus::Vector::Vector(Fixed x_p, Fixed y_p)
	: x(x_p) , y(y_p)
{}

octopus::Fixed octopus::length(Vector const &vec_p)
{
	return numeric::square_root(octopus::square_length(vec_p));
}

octopus::Fixed octopus::square_length(Vector const &vec_p)
{
	return vec_p.x * vec_p.x + vec_p.y * vec_p.y;
}

octopus::Fixed octopus::dot_product(Vector const &a, Vector const b)
{
	return a.x*b.x + a.y*b.y;
}

bool octopus::same_direction(Vector const & a, Vector const & b)
{
	if((a.x > -1e-3 && a.x < 1e-3) || (b.x > -1e-3 && b.x < 1e-3))
	{
		return (a.x > -1e-3 && a.x < 1e-3) && (b.x > -1e-3 && b.x < 1e-3);
	}
	if((a.y > -1e-3 && a.y < 1e-3) || (b.y > -1e-3 && b.y < 1e-3))
	{
		return (a.y > -1e-3 && a.y < 1e-3) && (b.y > -1e-3 && b.y < 1e-3);
	}

	if(a.x > 0. && b.x < 0.)
	{
		return false;
	}
	if(a.y > 0. && b.y < 0.)
	{
		return false;
	}

	Fixed diff_l = a.x/a.y - b.x/b.y;
	return diff_l < 1e-3 && diff_l > -1e-3;
}

bool octopus::is_zero(Vector const &v)
{
	return ::is_zero(v.x) && ::is_zero(v.y);
}

octopus::Vector operator+(octopus::Vector const & a, octopus::Vector const & b)
{
	return octopus::Vector { a.x + b.x, a.y + b.y} ;
}
octopus::Vector & operator+=(octopus::Vector & a, octopus::Vector const & b)
{
	a.x += b.x;
	a.y += b.y;
	return a;
}

octopus::Vector operator-(octopus::Vector const & a, octopus::Vector const & b)
{
	return octopus::Vector { a.x - b.x, a.y - b.y} ;
}
octopus::Vector & operator-=(octopus::Vector & a, octopus::Vector const & b)
{
	a.x -= b.x;
	a.y -= b.y;
	return a;
}

octopus::Vector operator/(octopus::Vector const & a, octopus::Fixed const & b)
{
	return octopus::Vector { a.x / b, a.y / b} ;
}
octopus::Vector & operator/=(octopus::Vector & a, octopus::Fixed const & b)
{
	a.x /= b;
	a.y /= b;
	return a;
}

octopus::Vector operator*(octopus::Vector const & a, octopus::Fixed const & b)
{
	return octopus::Vector { a.x * b, a.y * b} ;
}
octopus::Vector & operator*=(octopus::Vector & a, octopus::Fixed const & b)
{
	a.x *= b;
	a.y *= b;
	return a;
}

bool octopus::Vector::operator==(octopus::Vector const & b) const
{
	return square_length((*this) - b) < 1e-3;
}

bool octopus::Vector::operator!=(octopus::Vector const & b) const
{
	return !((*this) == b);
}

std::ostream &operator<<(std::ostream &os_p, octopus::Vector const &vec_p)
{
	return os_p<<"Vector[x="<<vec_p.x<<", y="<<vec_p.y<<"]";
}

