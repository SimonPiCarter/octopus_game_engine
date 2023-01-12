#include "Vector.hh"

#include <cmath>

double game_engine::length(Vector const &vec_p)
{
	return std::sqrt(game_engine::square_length(vec_p));
}

double game_engine::square_length(Vector const &vec_p)
{
	return vec_p.x * vec_p.x + vec_p.y * vec_p.y;
}

bool game_engine::same_direction(Vector const & a, Vector const & b)
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

game_engine::Vector operator+(game_engine::Vector const & a, game_engine::Vector const & b)
{
	return game_engine::Vector { a.x + b.x, a.y + b.y} ;
}
game_engine::Vector & operator+=(game_engine::Vector & a, game_engine::Vector const & b)
{
	a.x += b.x;
	a.y += b.y;
	return a;
}

game_engine::Vector operator-(game_engine::Vector const & a, game_engine::Vector const & b)
{
	return game_engine::Vector { a.x - b.x, a.y - b.y} ;
}
game_engine::Vector & operator-=(game_engine::Vector & a, game_engine::Vector const & b)
{
	a.x -= b.x;
	a.y -= b.y;
	return a;
}

game_engine::Vector operator/(game_engine::Vector const & a, double const & b)
{
	return game_engine::Vector { a.x / b, a.y / b} ;
}
game_engine::Vector & operator/=(game_engine::Vector & a, double const & b)
{
	a.x /= b;
	a.y /= b;
	return a;
}

game_engine::Vector operator*(game_engine::Vector const & a, double const & b)
{
	return game_engine::Vector { a.x * b, a.y * b} ;
}
game_engine::Vector & operator*=(game_engine::Vector & a, double const & b)
{
	a.x *= b;
	a.y *= b;
	return a;
}

bool operator==(game_engine::Vector const & a, game_engine::Vector const & b)
{
	return square_length(a - b) < 1e-3;
}

std::ostream &operator<<(std::ostream &os_p, game_engine::Vector const &vec_p)
{
	return os_p<<"Vector[x="<<vec_p.x<<", y="<<vec_p.y<<"]";
}

