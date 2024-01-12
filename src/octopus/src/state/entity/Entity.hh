#ifndef __ENTITY__
#define __ENTITY__

#include <iostream>
#include <vector>
#include <unordered_map>

#include "buff/TimedBuff.hh"
#include "buff/ConditionalBuff.hh"
#include "command/Commandable.hh"
#include "utils/Vector.hh"
#include "state/Handle.hh"
#include "state/entity/attackModifier/AttackModifier.hh"
#include "state/model/entity/EntityModel.hh"

namespace octopus
{
	class Entity : public Commandable
	{
		public:
			Entity(Vector const &pos_p, bool frozen_p, EntityModel const &model_p);
			virtual ~Entity() {}

			virtual bool isActive() const override
			{
				return Commandable::isActive() && _alive;
			}

			bool isIgnoringCollision() const;

			Vector _pos;

			/// @brief cannot move
			bool _frozen {false};

			Handle _handle {0};

			/// @brief is alive in the state
			bool _alive {true};

			Fixed _hp { 10. };

			Fixed _aggroDistance = 8;

			/// @brief time since last attack in step
			unsigned long _reload { 10 };
			/// @brief time waiting since last command for entity
			/// used to auto attack nearby enemies
			unsigned long _waiting { 100000 };
			/// @brief time since last ability was started
			std::unordered_map<std::string, unsigned long> _reloadAbilities;

			/// @brief entity model
			EntityModel const &_model;

			unsigned long _player {0};

			AttackModifier  _attackMod {NoModifier()};

			///
			/// Buffing properties
			///
			Buff _buffSpeed;
			Buff _buffFullReload;
			Buff _buffDamage;
			Buff _buffHeal;
			Buff _buffArmor;
			Buff _buffHpMax;
			Buff _buffHpRegeneration;
			Buff _buffProduction;
			Buff _buffHarvest;

			///
			/// Conditinal buff properties
			///
			std::vector<ConditionalBuff> _condBuffSpeed;
			std::vector<ConditionalBuff> _condBuffFullReload;
			std::vector<ConditionalBuff> _condBuffDamage;
			std::vector<ConditionalBuff> _condBuffHeal;
			std::vector<ConditionalBuff> _condBuffArmor;
			std::vector<ConditionalBuff> _condBuffHpMax;
			std::vector<ConditionalBuff> _condBuffHpRegeneration;
			std::vector<ConditionalBuff> _condBuffProduction;
			std::vector<ConditionalBuff> _condBuffHarvest;

			/// @brief map of registered buff currently applied
			std::unordered_map<std::string, TimedBuff> _registeredBuff;
			/// @brief map of time since the buff has been applied
			std::unordered_map<std::string, unsigned long> _timeSinceBuff;

			///
			/// Getters for buffer properties
			///
			/// @brief get step speed updated using buffing info
			Fixed getStepSpeed() const;
			/// @brief get step speed updated using buffing info
			Fixed getFullReload() const;
			/// @brief get damage updated using buffing info and bonus from type
			Fixed getDamage(EntityModel const &target_p) const;
			/// @brief get damage updated using buffing info but not bonus from type
			Fixed getDamageNoBonus() const;
			/// @brief get heal using buff info
			Fixed getHeal() const;
			/// @brief get armor updated using buffing info
			Fixed getArmor() const;
			/// @brief get hp max updated using buffing info
			/// @param applyConditional_p if set to false the conditional buffs will not be computed
			/// this is necessary to avoid infinite loop when a condtional buff for max hp depends on hp ratio
			Fixed getHpMax(bool applyConditional_p=true) const;
			/// @brief get hp regenration with buffs
			Fixed getHpRegeneration() const;

			/// @brief get production updated using buffing info
			Fixed getProduction() const;
			/// @brief get harvest updated using buffing info
			Fixed getHarvest() const;

			/// @brief if frozen ou static
			bool isFrozen() const;

			/// @brief get the time since buff of the given buff
			/// @param idBuff_p the if od the buff to test
			/// @return the time since the buff (return limit max if not found)
			unsigned long getTimeSinceBuff(std::string const &idBuff_p) const;

			virtual void runCommands(StepShallow & step_p, State const &state_p, PathManager &pathManager_p) override;

	};

	/// @brief return reload time (time since last ability usage) for the entity and the given key
	/// if the key is not present the default value is used
	/// @param ent_p entity to get the info from
	/// @param key_p the reload key to use
	/// @param default_p the default value if the reload key is not found
	unsigned long getReloadAbilityTime(Entity const &ent_p, std::string const &key_p, unsigned long const &default_p);
}

std::ostream &operator<<(std::ostream &os_p, octopus::Entity const &ent_p);

std::ostream &streamCsvPos(std::ostream &os_p, octopus::Entity const &ent_p);

std::ostream &streamCsvEntity(std::ostream &os_p, std::vector<octopus::Entity*> const &vec_p);

#endif
