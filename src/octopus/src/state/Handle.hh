#ifndef __HANDLE__
#define __HANDLE__

namespace octopus
{

/// @brief Handle is used to get an instance of the object in a State
/// handles are stored in step to access the data to be modified
typedef unsigned long Handle;

template<typename T>
struct HandleSorter
{
	bool operator()(T const &a, T const &b) const
	{
		return a.getHandle() < b.getHandle();
	}
};

template<typename T>
struct HandleSorterPtr
{
	bool operator()(T const *a, T const *b) const
	{
		return a->getHandle() < b->getHandle();
	}
};

}

#endif
