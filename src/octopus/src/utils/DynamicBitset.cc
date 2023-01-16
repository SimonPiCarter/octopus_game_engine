#include "DynamicBitset.hh"

#include <cmath>

namespace octopus
{

DynamicBitset::DynamicBitset(size_t nb_p) :
	_bitsetSize(nb_p),
	_size(sizeof(unsigned long)*8)
{
	for(size_t i = 0 ; i < std::ceil(double(_bitsetSize)/_size) ; ++i)
	{
		_vecBitset.emplace_back(0ul);
	}
}

void DynamicBitset::addBits(size_t nb_p)
{
	// increase vector size if necessary
	while(_vecBitset.size() * _size < _bitsetSize + nb_p)
	{
		_vecBitset.emplace_back(0ul);
	}

	// update size
	_bitsetSize += nb_p;
}

void DynamicBitset::set(size_t idx_p, bool val_p)
{
	// get index in the vector
	size_t idxInVec_l = idx_p/_size;
	// get index in the underlying long
	size_t idxInLong_l = idx_p - idxInVec_l * _size;

	unsigned long &long_l = _vecBitset[idxInVec_l];

	if(val_p)
	{
		long_l |= 1UL << idxInLong_l;
	}
	else
	{
		long_l &= ~(1UL << idxInLong_l);
	}
}

void DynamicBitset::reset()
{
	for(unsigned long &long_l : _vecBitset)
	{
		long_l = 0ul;
	}
}

size_t DynamicBitset::size() const
{
	return _bitsetSize;
}

void for_each_bit(DynamicBitset const &bitset_p, std::function<void(int)> fn_p)
{
	for (size_t k = 0; k < bitset_p._vecBitset.size(); ++k) {
		unsigned long bitset_l = bitset_p._vecBitset[k];

		while (bitset_l != 0) {
			unsigned long t = bitset_l & -bitset_l;

			int r = __builtin_ctzl(bitset_l);

			fn_p(k * bitset_p._size + r);

			bitset_l ^= t;
		}
	}
}

} // octopus

octopus::DynamicBitset & operator&=(octopus::DynamicBitset &a, octopus::DynamicBitset const &b)
{
	for(size_t i = 0 ; i < a._vecBitset.size() ; ++ i)
	{
		a._vecBitset[i] &= b._vecBitset[i];
	}
	return a;
}
octopus::DynamicBitset & operator|=(octopus::DynamicBitset &a, octopus::DynamicBitset const &b)
{
	for(size_t i = 0 ; i < a._vecBitset.size() ; ++ i)
	{
		a._vecBitset[i] |= b._vecBitset[i];
	}
	return a;
}
