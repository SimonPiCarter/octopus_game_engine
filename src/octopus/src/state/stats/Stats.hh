#ifndef __STATS__
#define __STATS__

namespace octopus
{
	struct Stats
	{
		double _hp { 10. };
		double _damage { 4. };
		double _armor { 1. };

		double _range { 3. };

		/// @brief time since last attack in step
		unsigned long _reload { 10 };
		/// @brief reload time in steps
		unsigned long _fullReload { 10 };

		/// @brief time to wind up an attack
		unsigned long _windup { 3 };
	};
}

#endif
