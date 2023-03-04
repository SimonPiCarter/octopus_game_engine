#ifndef __EntityModel__
#define __EntityModel__

#include <map>
#include <unordered_map>
#include "state/model/requirements/Requirements.hh"
#include "state/ResourceType.hh"

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

		std::unordered_map<std::string, double> _bonusDamage;

		/// @brief time to wind up an attack
		unsigned long _windup { 3 };

		bool _isBuilding { false };
		bool _isUnit { false };
		bool _isResource { false };

		bool _isBuilder { false };

		/// @brief those buildings are ignored when testing grid for TempleModel
		bool _isAbandonedTemple {false};

		std::map<ResourceType, double> _cost;
		/// @brief requirements
		Requirements _requirements;

		/// @brief distance of line of sight
		double _lineOfSight { 5 };

		/// @brief id to be set up when library loads a model
		std::string _id;
	};

	std::map<ResourceType, double> getReverseCostMap(std::map<ResourceType, double> const &cost_p);

	double getBonus(std::string const &id_p, EntityModel const &model_p);
} // namespace octopus


#endif
