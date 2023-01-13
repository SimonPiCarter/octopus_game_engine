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
	};
}

#endif
