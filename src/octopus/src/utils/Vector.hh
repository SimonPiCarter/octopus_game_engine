#ifndef __VECTOR__
#define __VECTOR__

#include <iostream>

namespace octopus
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

octopus::Vector operator+(octopus::Vector const & a, octopus::Vector const & b);
octopus::Vector & operator+=(octopus::Vector & a, octopus::Vector const & b);

octopus::Vector operator-(octopus::Vector const & a, octopus::Vector const & b);
octopus::Vector & operator-=(octopus::Vector & a, octopus::Vector const & b);

octopus::Vector operator/(octopus::Vector const & a, double const & b);
octopus::Vector & operator/=(octopus::Vector & a, double const & b);

octopus::Vector operator*(octopus::Vector const & a, double const & b);
octopus::Vector & operator*=(octopus::Vector & a, double const & b);

bool operator==(octopus::Vector const & a, octopus::Vector const & b);
bool operator!=(octopus::Vector const & a, octopus::Vector const & b);

std::ostream &operator<<(std::ostream &os_p, octopus::Vector const &vec_p);

#endif
