#ifndef __EntityModel__
#define __EntityModel__

#include <map>
#include <functional>
#include <unordered_map>
#include "state/entity/attackModifier/AttackModifier.hh"
#include "state/model/requirements/Requirements.hh"
#include "utils/Vector.hh"
#include "state/Handle.hh"

namespace octopus
{
	class Entity;
	class Step;
	class State;
	void idleFunctioNoOp(Entity const &, Step &, State const &);

	bool alwaysOkChecker(Step const &, State const &, Handle const &, Handle const &, Vector const &);

	struct Ability
	{
		std::string _id;
		std::string _reloadKey;
		std::function<void(Step &, State const &, Handle const &, Handle const &, Vector const &)> _runnable;
		std::function<bool(Step const &, State const &, Handle const &, Handle const &, Vector const &)> _checker = alwaysOkChecker;
	};

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
		Fixed _heal { 0. };
		Fixed _armor { 1. };
		Fixed _range { 3. };
		/// @brief hp regeneration every 100 steps
		Fixed _hpRegeneration { 0. };
		/// @brief reload time in steps
		unsigned long _fullReload { 10 };

		std::unordered_map<std::string, Fixed> _bonusDamage;

		/// @brief map of abilities indexed on unique id of abilities
		std::unordered_map<std::string, Ability > _abilities;
		/// @brief map indexed on the reload times
		std::unordered_map<std::string, unsigned long > _abilityReloadTime;

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

		std::function<void(Entity const &, Step &, State const &)> _idleFunc = idleFunctioNoOp;
	};

	std::map<std::string, Fixed> getReverseCostMap(std::map<std::string, Fixed> const &cost_p);

	Fixed getBonus(std::string const &id_p, EntityModel const &model_p);

	Ability const & getAbility(EntityModel const &model_p, std::string const &id_p);

	unsigned long getMinReloadTime(EntityModel const &model_p, std::string const &key_p);
} // namespace octopus


#endif
