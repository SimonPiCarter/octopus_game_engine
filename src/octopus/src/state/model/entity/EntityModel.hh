#ifndef __EntityModel__
#define __EntityModel__

#include <map>
#include <unordered_map>
#include "state/entity/attackModifier/AttackModifier.hh"
#include "state/model/requirements/Requirements.hh"

namespace octopus
{
	struct EntityModel
	{
		EntityModel(bool isStatic_p, Fixed ray_p, Fixed stepSpeed_p, Fixed hpMax_p)
			: _isStatic(isStatic_p), _ray(ray_p), _stepSpeed(stepSpeed_p), _hpMax(hpMax_p)
		{}

		virtual ~EntityModel() {}

		/// @brief default modifier
		AttackModifier  _defaultAttackMod {NoModifier()};

		/// @brief if true is a static (in that case _ray is the width if the building)
		/// buildings are squared
		bool _isStatic { false };

		Fixed _ray {1.};

		Fixed _stepSpeed {0.25};
		////////////////////
		/// Static stats
		////////////////////
		Fixed _hpMax { 10. };
		Fixed _damage { 4. };
		Fixed _armor { 1. };
		Fixed _range { 3. };
		/// @brief hp regeneration every 100 steps
		Fixed _hpRegeneration { 0. };
		/// @brief reload time in steps
		unsigned long _fullReload { 10 };

		std::unordered_map<std::string, Fixed> _bonusDamage;

		/// @brief time to wind up an attack
		unsigned long _windup { 3 };

		bool _isBuilding { false };
		bool _isUnit { false };
		bool _isResource { false };

		bool _isBuilder { false };

		/// @brief is invulnerable
		bool _invulnerable {false};

		/// @brief those buildings are ignored when testing grid for TempleModel
		bool _isAbandonedTemple {false};

		std::map<std::string, Fixed> _cost;
		/// @brief requirements
		Requirements _requirements;

		/// @brief distance of line of sight
		Fixed _lineOfSight { 5 };

		/// @brief id to be set up when library loads a model
		std::string _id;
	};

	std::map<std::string, Fixed> getReverseCostMap(std::map<std::string, Fixed> const &cost_p);

	Fixed getBonus(std::string const &id_p, EntityModel const &model_p);
} // namespace octopus


#endif
