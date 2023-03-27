#ifndef __FixedPoint__
#define __FixedPoint__

#include <iostream>
#include <limits>

namespace octopus
{
	/// @brief store a real x e as an integer
	template<long long e>
	struct FixedPoint
	{
	public:
		template<class Number, class = typename std::enable_if<std::is_arithmetic<Number>::value>::type>
		FixedPoint(Number const &val, bool internal_p=false) : _data(internal_p?val:val*e) {}
		FixedPoint(FixedPoint<e> const &f) : _data(f._data) {}
		FixedPoint() : _data(0) {}

		FixedPoint<e> operator*(FixedPoint<e> const &f) const
		{
			long long tmp_l = _data * f._data;
			return FixedPoint<e>(tmp_l/e, true);
		}
		FixedPoint<e> operator/(FixedPoint<e> const &f) const
		{
			long long tmp_l = e * _data;
			return FixedPoint<e>(tmp_l/f._data, true);
		}
		FixedPoint<e> operator+(FixedPoint<e> const &f) const
		{
			return FixedPoint<e>(_data + f._data, true);
		}
		FixedPoint<e> operator-(FixedPoint<e> const &f) const
		{
			return FixedPoint<e>(_data - f._data, true);
		}
		FixedPoint<e> operator-() const
		{
			return FixedPoint<e>(-_data, true);
		}

		FixedPoint<e> &operator*=(FixedPoint<e> const &f)
		{
			_data *= f._data;
			_data /= e;
			return *this;
		}
		FixedPoint<e> &operator/=(FixedPoint<e> const &f)
		{
			_data *= e;
			_data /= f._data;
			return *this;
		}
		FixedPoint<e> &operator+=(FixedPoint<e> const &f)
		{
			_data += f._data;
			return *this;
		}
		FixedPoint<e> &operator-=(FixedPoint<e> const &f)
		{
			_data -= f._data;
			return *this;
		}

		bool operator>(FixedPoint<e> const &f) const
		{
			return _data > f._data;
		}

		bool operator<(FixedPoint<e> const &f) const
		{
			return _data < f._data;
		}

		bool operator>=(FixedPoint<e> const &f) const
		{
			return _data >= f._data;
		}

		bool operator<=(FixedPoint<e> const &f) const
		{
			return _data <= f._data;
		}

		bool operator==(FixedPoint<e> const &f) const
		{
			return _data == f._data;
		}

		bool operator!=(FixedPoint<e> const &f) const
		{
			return _data != f._data;
		}

		template <class Number, class = typename std::enable_if<std::is_arithmetic<Number>::value>::type>
		FixedPoint<e> operator*(Number const &f) const
		{
			long long tmp_l = _data * f;
			return FixedPoint<e>(tmp_l, true);
		}
		template <class Number, class = typename std::enable_if<std::is_arithmetic<Number>::value>::type>
		FixedPoint<e> operator/(Number const &f) const
		{
			return FixedPoint<e>(_data/f, true);
		}
		template <class Number, class = typename std::enable_if<std::is_arithmetic<Number>::value>::type>
		FixedPoint<e> operator+(Number const &f) const
		{
			return FixedPoint<e>(_data + f*e, true);
		}
		template <class Number, class = typename std::enable_if<std::is_arithmetic<Number>::value>::type>
		FixedPoint<e> operator-(Number const &f) const
		{
			return FixedPoint<e>(_data - f*e, true);
		}

		template <class Number, class = typename std::enable_if<std::is_arithmetic<Number>::value>::type>
		FixedPoint<e> &operator*=(Number const &f)
		{
			_data = static_cast<long long>(_data * f);
			return *this;
		}
		template <class Number, class = typename std::enable_if<std::is_arithmetic<Number>::value>::type>
		FixedPoint<e> &operator/=(Number const &f)
		{
			_data = static_cast<long long>(_data / f);
			return *this;
		}
		template <class Number, class = typename std::enable_if<std::is_arithmetic<Number>::value>::type>
		FixedPoint<e> &operator+=(Number const &f)
		{
			_data += static_cast<long long>(f*e);
			return *this;
		}
		template <class Number, class = typename std::enable_if<std::is_arithmetic<Number>::value>::type>
		FixedPoint<e> &operator-=(Number const &f)
		{
			_data -= static_cast<long long>(f*e);
			return *this;
		}

		template <class Number, class = typename std::enable_if<std::is_arithmetic<Number>::value>::type>
		bool operator>(Number const &f) const
		{
			return _data > static_cast<long long>(f*e);
		}

		template <class Number, class = typename std::enable_if<std::is_arithmetic<Number>::value>::type>
		bool operator<(Number const &f) const
		{
			return _data < static_cast<long long>(f*e);
		}

		template <class Number, class = typename std::enable_if<std::is_arithmetic<Number>::value>::type>
		bool operator>=(Number const &f) const
		{
			return _data >= static_cast<long long>(f*e);
		}

		template <class Number, class = typename std::enable_if<std::is_arithmetic<Number>::value>::type>
		bool operator<=(Number const &f) const
		{
			return _data <= static_cast<long long>(f*e);
		}

		template <class Number, class = typename std::enable_if<std::is_arithmetic<Number>::value>::type>
		bool operator==(Number const &f) const
		{
			return _data == static_cast<long long>(f*e);
		}

		template <class Number, class = typename std::enable_if<std::is_arithmetic<Number>::value>::type>
		bool operator!=(Number const &f) const
		{
			return _data != static_cast<long long>(f*e);
		}

		long long to_int() const
		{
			return _data/e;
		}

		unsigned long long to_uint() const
		{
			return std::abs(_data/e);
		}

		double to_double() const
		{
			return _data/double(e);
		}

		long long data() const
		{
			return _data;
		}

	private:
		long long _data;
	};

} // octopus

template <long long e, class Number, class = typename std::enable_if<std::is_arithmetic<Number>::value>::type>
octopus::FixedPoint<e> operator*(Number const &f, octopus::FixedPoint<e> const &fp)
{
	return fp * f;
}
template <long long e, class Number, class = typename std::enable_if<std::is_arithmetic<Number>::value>::type>
octopus::FixedPoint<e> operator/(Number const &f, octopus::FixedPoint<e> const &fp)
{
	return octopus::FixedPoint<e>(f) / fp;
}
template <long long e, class Number, class = typename std::enable_if<std::is_arithmetic<Number>::value>::type>
octopus::FixedPoint<e> operator+(Number const &f, octopus::FixedPoint<e> const &fp)
{
	return fp + f;
}
template <long long e, class Number, class = typename std::enable_if<std::is_arithmetic<Number>::value>::type>
octopus::FixedPoint<e> operator-(Number const &f, octopus::FixedPoint<e> const &fp)
{
	return -fp + f;
}

template <long long e, class Number, class = typename std::enable_if<std::is_arithmetic<Number>::value>::type>
bool operator>(Number const &f, octopus::FixedPoint<e> const &fp)
{
	return fp <= f;
}

template <long long e, class Number, class = typename std::enable_if<std::is_arithmetic<Number>::value>::type>
bool operator<(Number const &f, octopus::FixedPoint<e> const &fp)
{
	return fp >= f;
}

template <long long e, class Number, class = typename std::enable_if<std::is_arithmetic<Number>::value>::type>
bool operator>=(Number const &f, octopus::FixedPoint<e> const &fp)
{
	return fp < f;
}

template <long long e, class Number, class = typename std::enable_if<std::is_arithmetic<Number>::value>::type>
bool operator<=(Number const &f, octopus::FixedPoint<e> const &fp)
{
	return fp > f;
}

template <long long e, class Number, class = typename std::enable_if<std::is_arithmetic<Number>::value>::type>
bool operator==(Number const &f, octopus::FixedPoint<e> const &fp)
{
	return fp == f;
}

template <long long e, class Number, class = typename std::enable_if<std::is_arithmetic<Number>::value>::type>
bool operator!=(Number const &f, octopus::FixedPoint<e> const &fp)
{
	return fp != f;
}

template <long long e>
std::ostream &operator<<(std::ostream &os, octopus::FixedPoint<e> f) {
	os << f.to_double();
	return os;
}

template <long long e>
octopus::FixedPoint<e> sqr(octopus::FixedPoint<e> const &f) {
	return f*f;
}

double sqr(double const &f);

template <long long e>
bool is_zero(octopus::FixedPoint<e> const &f) {
	return std::abs(f.data()) < 1;
}

bool is_zero(double const &f);

namespace numeric
{

template <long long e>
octopus::FixedPoint<e> square_root(octopus::FixedPoint<e> const &v)
{
	// quick win for exact match
	if(v == 0)
	{
		return 0;
	}
	octopus::FixedPoint<e> res_l(e, true);
	octopus::FixedPoint<e> epsilon_l(10, true);
	octopus::FixedPoint<e> mepsilon_l(-10, true);

	int i = 0;
	octopus::FixedPoint<e> delta_l = res_l * res_l - v;
	/// @todo mettre 200?
	while(i < 1000 && (delta_l < mepsilon_l || delta_l > epsilon_l))
	{
		res_l = (res_l + v / res_l) * octopus::FixedPoint<e>(e/2, true);
		delta_l = res_l * res_l - v;
		++i;
	}
	return res_l;
}

template <long long e>
octopus::FixedPoint<e> abs(octopus::FixedPoint<e> const &f)
{
	return octopus::FixedPoint<e>(std::abs(f.data()), true);
}


template<class Number, class = typename std::enable_if<std::is_arithmetic<Number>::value>::type>
Number infinity()
{
	return std::numeric_limits<Number>::infinity();
}

template<class Fixed>
Fixed infinity()
{
	return Fixed(std::numeric_limits<long long>::max(), true);
}

} // namespace numeric


#endif
