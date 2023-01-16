#ifndef __DynamicBitset__
#define __DynamicBitset__

#include <bitset>
#include <iostream>
#include <vector>
#include <functional>

namespace octopus
{
	class DynamicBitset;
}
/// Declare basic operators (only the one that avoid creating new bitsets for performance reason)

octopus::DynamicBitset & operator&=(octopus::DynamicBitset &a, octopus::DynamicBitset const &b);
octopus::DynamicBitset & operator|=(octopus::DynamicBitset &a, octopus::DynamicBitset const &b);

namespace octopus
{

/// @brief execute the given function with one argument being the index of every bit set to true
void for_each_bit(DynamicBitset const &bitset_p, std::function<void(int)> fn_p);

/// @brief this class encapsulate vector of unsigned long to represent bitsets
/// it allows dynamic size and fast bitwise operations
/// @warning underlying data will always have a size of 64, all non set bits
/// will be set to 0, beware of this when manipilation the object
class DynamicBitset
{
public:
	DynamicBitset(size_t nb_p);

	void addBits(size_t nb_p);

	void set(size_t idx_p, bool val_p);

	void reset();

	/// @brief return the logical size of the bitset
	size_t size() const;

private:
	/// @brief underlying data structure
	std::vector<unsigned long> _vecBitset;
	/// @brief this is the logical size of the bitset (takes into account constructor nb and all addBits call)
	/// This can be different from the underlying data
	size_t _bitsetSize;

	/// @brief size of the underlying type (eg 64 for unsigned long usually)
	size_t const _size;

	/// @brief to avoid making some accessor public
	friend void for_each_bit(DynamicBitset const &bitset_p, std::function<void(int)> fn_p);
	friend octopus::DynamicBitset & ::operator&=(octopus::DynamicBitset &a, octopus::DynamicBitset const &b);
	friend octopus::DynamicBitset & ::operator|=(octopus::DynamicBitset &a, octopus::DynamicBitset const &b);
};

}


#endif
