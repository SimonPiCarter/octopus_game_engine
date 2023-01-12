#ifndef __VECTOR__
#define __VECTOR__

#include <iostream>

namespace game_engine
{
	struct Vector
	{
		double x {0.};
		double y {0.};
	};

	double length(Vector const &vec_p);
	double square_length(Vector const &vec_p);

	bool same_direction(Vector const & a, Vector const & b);
}

game_engine::Vector operator+(game_engine::Vector const & a, game_engine::Vector const & b);
game_engine::Vector & operator+=(game_engine::Vector & a, game_engine::Vector const & b);

game_engine::Vector operator-(game_engine::Vector const & a, game_engine::Vector const & b);
game_engine::Vector & operator-=(game_engine::Vector & a, game_engine::Vector const & b);

game_engine::Vector operator/(game_engine::Vector const & a, double const & b);
game_engine::Vector & operator/=(game_engine::Vector & a, double const & b);

game_engine::Vector operator*(game_engine::Vector const & a, double const & b);
game_engine::Vector & operator*=(game_engine::Vector & a, double const & b);

bool operator==(game_engine::Vector const & a, game_engine::Vector const & b);

std::ostream &operator<<(std::ostream &os_p, game_engine::Vector const &vec_p);

#endif
