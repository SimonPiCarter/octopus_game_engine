#ifndef __STATS__
#define __STATS__

namespace octopus
{
	struct Stats
	{
		double _hp { 10. };
		double _damage { 2.5 };

		double _range { 3. };

		/// @brief reload time in steps
		unsigned long _reload { 1 };
	};
}

#endif
