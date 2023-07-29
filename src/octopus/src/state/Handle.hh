#ifndef __HANDLE__
#define __HANDLE__

#include <unordered_set>
#include <iostream>

namespace octopus
{

/// @brief Handle is used to get an instance of the object in a State
/// handles are stored in step to access the data to be modified
struct Handle
{
	Handle() {}
	Handle(unsigned long idx_p, unsigned char rev_p) : index(idx_p), revision(rev_p) {}
	explicit Handle(unsigned long idx_p) : index(idx_p) {}

	unsigned long index {0};
	unsigned char revision {0};

	operator unsigned long() const { return index; }

	bool operator<(Handle const &other_p) const
	{
		return index < other_p.index;
	}

	bool operator==(Handle const &other_p) const
	{
		return index == other_p.index && revision == other_p.revision;
	}

	bool operator!=(Handle const &other_p) const
	{
		return !(*this == other_p);
	}

	Handle const & operator++()
	{
		++index;
		return *this;
	}

	Handle operator++(int)
	{
		Handle old_l(*this);
		++index;
		return old_l;
	}
};

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

template<>
struct std::hash<octopus::Handle>
{
	size_t operator()(octopus::Handle const &handle_p) const
	{
		return handle_p.revision * 10000000 + handle_p.index;
	}
};

std::ostream & operator<<(std::ostream & os_p, octopus::Handle const &handle_p);


#endif
