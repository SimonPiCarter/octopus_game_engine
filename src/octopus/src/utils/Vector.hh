#ifndef __VECTOR__
#define __VECTOR__

#include <iostream>

#include "Fixed.hh"

namespace octopus
{
	struct Vector
	{
		Vector();
		Vector(Fixed x_p, Fixed y_p);

		Fixed x {0.};
		Fixed y {0.};

		bool operator==(octopus::Vector const & other_p) const;
		bool operator!=(octopus::Vector const & other_p) const;
	};

	Fixed length(Vector const &vec_p);
	Fixed square_length(Vector const &vec_p);
	Fixed dot_product(Vector const &a, Vector const b);

	bool same_direction(Vector const & a, Vector const & b);

	bool is_zero(Vector const &v);
}

octopus::Vector operator+(octopus::Vector const & a, octopus::Vector const & b);
octopus::Vector & operator+=(octopus::Vector & a, octopus::Vector const & b);

octopus::Vector operator-(octopus::Vector const & a, octopus::Vector const & b);
octopus::Vector & operator-=(octopus::Vector & a, octopus::Vector const & b);

octopus::Vector operator/(octopus::Vector const & a, octopus::Fixed const & b);
octopus::Vector & operator/=(octopus::Vector & a, octopus::Fixed const & b);

octopus::Vector operator*(octopus::Vector const & a, octopus::Fixed const & b);
octopus::Vector & operator*=(octopus::Vector & a, octopus::Fixed const & b);


std::ostream &operator<<(std::ostream &os_p, octopus::Vector const &vec_p);

template<>
struct std::hash<octopus::Vector>
{
	uint32_t operator()(octopus::Vector const &vector_p) const
	{
		return vector_p.x.data() + vector_p.y.data();
	}
};

#endif
