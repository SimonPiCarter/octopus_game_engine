#ifndef __DynamicBitset__
#define __DynamicBitset__

#include <bitset>
#include <iostream>
#include <set>
#include <vector>
#include <functional>
#include <boost/unordered/unordered_flat_set.hpp>

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
	virtual void set(uint32_t idx_p, bool val_p) = 0;
	/// @brief reset the bitset
	virtual void reset() = 0;

	virtual void for_each(std::function<bool(int)> fn_p) const = 0;

	virtual bool empty() const = 0;
};

/// @brief this class encapsulate vector of unsigned long to represent bitsets
/// it allows dynamic size and fast bitwise operations
/// @warning underlying data will always have a size of 64, all non set bits
/// will be set to 0, beware of this when manipilation the object
class DynamicBitset : public AbstractBitset
{
public:
	DynamicBitset(uint32_t nb_p);

	/// @brief return the logical size of the bitset
	uint32_t size() const;

	void set(uint32_t idx_p, bool val_p) override;

	void reset() override;

	void for_each(std::function<bool(int)> fn_p) const override;

	bool empty() const override;

private:
	/// @brief underlying data structure
	std::vector<unsigned long> _vecBitset;
	/// @brief this is the logical size of the bitset
	/// This can be different from the underlying data
	uint32_t _bitsetSize;

	/// @brief size of the underlying type (eg 64 for unsigned long usually)
	uint32_t const _size;

	friend octopus::DynamicBitset & ::operator&=(octopus::DynamicBitset &a, octopus::DynamicBitset const &b);
	friend octopus::DynamicBitset & ::operator|=(octopus::DynamicBitset &a, octopus::DynamicBitset const &b);
};

/// @brief this class enacpsulate a set to modelize a sparse bitset
class SetBitset : public AbstractBitset
{
public:
	void set(uint32_t idx_p, bool val_p) override;

	void reset() override;

	void for_each(std::function<bool(int)> fn_p) const override;

	bool empty() const override;

private:
	boost::unordered_flat_set<int> _set;
};

}


#endif
