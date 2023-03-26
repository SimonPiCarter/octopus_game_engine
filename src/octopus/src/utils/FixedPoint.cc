#include "FixedPoint.hh"

double sqr(double const &f) {
	return f*f;
}

bool is_zero(double const &f) {
	return std::abs(f) < 1e-10;
}
