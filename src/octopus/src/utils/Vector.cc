#include "Vector.hh"

#include <cmath>

double octopus::length(Vector const &vec_p)
{
	return std::sqrt(octopus::square_length(vec_p));
}

double octopus::square_length(Vector const &vec_p)
{
	return vec_p.x * vec_p.x + vec_p.y * vec_p.y;
}

bool octopus::same_direction(Vector const & a, Vector const & b)
{
	if(std::abs(a.x) < 1e-3 || std::abs(b.x) < 1e-3)
	{
		return std::abs(a.x) < 1e-3 && std::abs(b.x) < 1e-3;
	}
	if(std::abs(a.y) < 1e-3 || std::abs(b.y) < 1e-3)
	{
		return std::abs(a.y) < 1e-3 && std::abs(b.y) < 1e-3;
	}

	if(a.x > 0. && b.x < 0.)
	{
		return false;
	}
	if(a.y > 0. && b.y < 0.)
	{
		return false;
	}

	return std::abs(a.x/a.y - b.x/b.y) < 1e-3;
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

octopus::Vector operator/(octopus::Vector const & a, double const & b)
{
	return octopus::Vector { a.x / b, a.y / b} ;
}
octopus::Vector & operator/=(octopus::Vector & a, double const & b)
{
	a.x /= b;
	a.y /= b;
	return a;
}

octopus::Vector operator*(octopus::Vector const & a, double const & b)
{
	return octopus::Vector { a.x * b, a.y * b} ;
}
octopus::Vector & operator*=(octopus::Vector & a, double const & b)
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

