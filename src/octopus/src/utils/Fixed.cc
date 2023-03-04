#include "Fixed.hh"

#include <cmath>

namespace numeric
{

double square_root(double v)
{
	double res_l(1.);
	double epsilon_l(0.000001);

	int i = 0;
	double delta_l = res_l * res_l - v;
	while(i < 1000 && (delta_l < -epsilon_l || delta_l > epsilon_l))
	{
		res_l = (res_l + v / res_l) * 0.5;
		delta_l = res_l * res_l - v;
		++i;
	}
	return res_l;
}

double abs(double d)
{
	return std::abs(d);
}

} // namespace numeric


namespace octopus
{

long long to_int(double const &f) {
	return static_cast<long long>(f);
}

unsigned long long to_uint(double const &f) {
	return static_cast<unsigned long long>(f);
}

double to_double(double const &f) {
	return f;
}

}
