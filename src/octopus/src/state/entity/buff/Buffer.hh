#ifndef __Buffer__
#define __Buffer__

#include "TimedBuff.hh"

namespace octopus
{

struct Buffer
{
	/// @brief true if the buffer is active (false if no buff to apply)
	bool _active {false};
	/// @brief reload time to apply buff
	unsigned long _reload {0};
	/// @brief range to apply buff
	Fixed _range {3.};
	/// @brief the buff than can be applied
	TimedBuff _buff;
};

} // namespace octopus

#endif
