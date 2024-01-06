#include "DynamicBitset.hh"

#include <cmath>

namespace octopus
{

DynamicBitset::DynamicBitset(uint32_t nb_p) :
	_bitsetSize(nb_p),
	_size(sizeof(unsigned long)*8)
{
	for(uint32_t i = 0 ; i < std::ceil(double(_bitsetSize)/_size) ; ++i)
	{
		_vecBitset.emplace_back(0ul);
	}
}

void DynamicBitset::set(uint32_t idx_p, bool val_p)
{
	// get index in the vector
	uint32_t idxInVec_l = idx_p/_size;
	// get index in the underlying long
	uint32_t idxInLong_l = idx_p - idxInVec_l * _size;

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

uint32_t DynamicBitset::size() const
{
	return _bitsetSize;
}

void DynamicBitset::for_each(std::function<bool(int)> fn_p) const
{
	for (uint32_t k = 0; k < _vecBitset.size(); ++k) {
		unsigned long bitset_l = _vecBitset[k];
		#ifdef _MSC_VER
			int r = 0;
			while (bitset_l != 0)
			{
				if(bitset_l % 2 == 1)
				{
					if(fn_p(k * _size + r))
					{
						return;
					}
				}
				++r;
				bitset_l = bitset_l >> 1;
			}
		#else
			while (bitset_l != 0)
			{
				unsigned long t = bitset_l & -bitset_l;

				int r = __builtin_ctzl(bitset_l);

				if(fn_p(k * _size + r))
				{
					break;
				}

				bitset_l ^= t;
			}
		#endif
	}
}

bool DynamicBitset::empty() const
{
	for (uint32_t k = 0; k < _vecBitset.size(); ++k) {
		if(_vecBitset[k] > 0)
		{
			return false;
		}
	}
	return true;
}

void SetBitset::set(uint32_t idx_p, bool val_p)
{
	if(val_p)
	{
		_set.insert(idx_p);
	}
	else
	{
		_set.erase(idx_p);
	}
}

void SetBitset::reset()
{
	_set.clear();
}

void SetBitset::for_each(std::function<bool(int)> fn_p) const
{
	for(int i : _set)
	{
		if(fn_p(i))
		{
			break;
		}
	}
}

bool SetBitset::empty() const
{
	return _set.empty();
}


} // octopus

octopus::DynamicBitset & operator&=(octopus::DynamicBitset &a, octopus::DynamicBitset const &b)
{
	for(uint32_t i = 0 ; i < a._vecBitset.size() ; ++ i)
	{
		a._vecBitset[i] &= b._vecBitset[i];
	}
	return a;
}
octopus::DynamicBitset & operator|=(octopus::DynamicBitset &a, octopus::DynamicBitset const &b)
{
	for(uint32_t i = 0 ; i < a._vecBitset.size() ; ++ i)
	{
		a._vecBitset[i] |= b._vecBitset[i];
	}
	return a;
}
