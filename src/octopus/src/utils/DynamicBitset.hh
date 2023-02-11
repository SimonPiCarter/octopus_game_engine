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

/// @brief define the interface for fast access on sparse bitset
class AbstractBitset
{
public:
	virtual ~AbstractBitset() {}

	/// @brief set a bit to the given value
	virtual void set(size_t idx_p, bool val_p) = 0;
	/// @brief reset the bitset
	virtual void reset() = 0;

	virtual void for_each(std::function<void(int)> fn_p) const = 0;
};

/// @brief this class encapsulate vector of unsigned long to represent bitsets
/// it allows dynamic size and fast bitwise operations
/// @warning underlying data will always have a size of 64, all non set bits
/// will be set to 0, beware of this when manipilation the object
class DynamicBitset : public AbstractBitset
{
public:
	DynamicBitset(size_t nb_p);

	void addBits(size_t nb_p);
	/// @brief return the logical size of the bitset
	size_t size() const;

	void set(size_t idx_p, bool val_p) override;

	void reset() override;

	virtual void for_each(std::function<void(int)> fn_p) const override;

private:
	/// @brief underlying data structure
	std::vector<unsigned long> _vecBitset;
	/// @brief this is the logical size of the bitset (takes into account constructor nb and all addBits call)
	/// This can be different from the underlying data
	size_t _bitsetSize;

	/// @brief size of the underlying type (eg 64 for unsigned long usually)
	size_t const _size;

	friend octopus::DynamicBitset & ::operator&=(octopus::DynamicBitset &a, octopus::DynamicBitset const &b);
	friend octopus::DynamicBitset & ::operator|=(octopus::DynamicBitset &a, octopus::DynamicBitset const &b);
};

}


#endif
