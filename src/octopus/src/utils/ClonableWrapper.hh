#ifndef __ClonableWrapper__
#define __ClonableWrapper__

/// @brief This class aims at wrapping safely a pointer that will be cloned
/// on copy of the wrapper
/// @tparam T must implement a clone method that returns a new T
template<typename T>
struct ClonableWrapper
{
	ClonableWrapper() {}
	ClonableWrapper(T * ptr_p) : ptr(ptr_p) {}
	ClonableWrapper(ClonableWrapper const &other_p) : ptr(other_p.ptr?other_p.ptr->clone():nullptr) {}

	~ClonableWrapper() { delete ptr; }

	ClonableWrapper & operator=(ClonableWrapper const &other_p)
	{
		delete ptr;
		ptr = other_p.ptr?other_p.ptr->clone():nullptr;
		return *this;
	}

	T * operator->() { return ptr; }
	T const * operator->() const { return ptr; }

	T * get() { return ptr; }
	T const * get() const { return ptr; }
private:
	T * ptr = nullptr;
};

#endif
