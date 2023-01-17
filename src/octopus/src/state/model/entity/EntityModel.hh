#ifndef __EntityModel__
#define __EntityModel__

namespace octopus
{
	struct EntityModel
	{
		/// @brief if true is a building (in that case _ray is the width if the building)
		/// buildings are squared
		bool _isBuilding { false };

		double _ray {1.};

		double _stepSpeed {0.25};
		////////////////////
		/// Static stats
		////////////////////
		double _hpMax { 10. };
		double _damage { 4. };
		double _armor { 1. };
		double _range { 3. };
		/// @brief reload time in steps
		unsigned long _fullReload { 10 };

		/// @brief time to wind up an attack
		unsigned long _windup { 3 };
	};
} // namespace octopus


#endif
