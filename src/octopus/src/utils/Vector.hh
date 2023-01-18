#ifndef __VECTOR__
#define __VECTOR__

#include <iostream>

namespace octopus
{
	struct Vector
	{
		Vector();
		Vector(double x_p, double y_p);

		double x {0.};
		double y {0.};

		bool operator==(octopus::Vector const & other_p) const;
		bool operator!=(octopus::Vector const & other_p) const;
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


std::ostream &operator<<(std::ostream &os_p, octopus::Vector const &vec_p);

#endif
