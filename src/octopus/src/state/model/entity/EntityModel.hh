#ifndef __EntityModel__
#define __EntityModel__

namespace octopus
{
	struct EntityModel
	{
		EntityModel(bool isStatic_p, double ray_p, double stepSpeed_p, double hpMax_p)
			: _isStatic(isStatic_p), _ray(ray_p), _stepSpeed(stepSpeed_p), _hpMax(hpMax_p)
		{}

		virtual ~EntityModel() {}

		/// @brief if true is a static (in that case _ray is the width if the building)
		/// buildings are squared
		bool _isStatic { false };

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

		bool _isBuilding { false };
		bool _isUnit { false };
		bool _isResource { false };
	};
} // namespace octopus


#endif
