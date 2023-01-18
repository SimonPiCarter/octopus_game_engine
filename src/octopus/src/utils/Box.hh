#ifndef __Box__
#define __Box__


namespace octopus
{
template<typename T>
struct Box
{
	Box() : _lowerX(0), _upperX(0), _lowerY(0), _upperY(0) {}
	Box(T const & lX, T const &uX, T const &lY, T const &uY)
		: _lowerX(lX), _upperX(uX), _lowerY(lY), _upperY(uY)
	{}
	T _lowerX {0};
	T _upperX {0};
	T _lowerY {0};
	T _upperY {0};
};
} // octopus

#endif
