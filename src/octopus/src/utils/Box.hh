#ifndef __Box__
#define __Box__


namespace octopus
{
template<typename T>
struct Box
{
	T _lowerX {0};
	T _upperX {0};
	T _lowerY {0};
	T _upperY {0};
};
} // octopus

#endif
