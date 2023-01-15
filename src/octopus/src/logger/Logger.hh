#ifndef __LOGGER__
#define __LOGGER__

#include <iostream>

namespace octopus
{
	class Logger
	{
	public:
		static std::ostream &getNormal();
		static std::ostream &getDebug();

		static void enable_debug() { _debug = true; }
		static void disable_debug() { _debug = false; }

	private:
		static bool _debug;
	};
}

#endif
