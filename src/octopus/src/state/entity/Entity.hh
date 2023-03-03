#ifndef __ENTITY__
#define __ENTITY__

#include <iostream>
#include <vector>
#include <unordered_map>

#include "Buff.hh"
#include "command/Commandable.hh"
#include "utils/Vector.hh"
#include "state/Handle.hh"
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

			double _hp { 10. };
			/// @brief time since last attack in step
			unsigned long _reload { 10 };
			/// @brief time waiting since last command for entity
			/// used to auto attack nearby enemies
			unsigned long _waiting { 100000 };

			/// @brief entity model
			EntityModel const &_model;

			unsigned long _player {0};

			///
			/// Buffing properties
			///
			Buff _buffSpeed;
			Buff _buffFullReload;
			Buff _buffDamage;
			Buff _buffArmor;
			Buff _buffProduction;
			Buff _buffHarvest;

			/// @brief map of registered buff currently applied
			std::unordered_map<std::string, TyppedBuff> _registeredBuff;
			/// @brief map of time since the buff has been applied
			std::unordered_map<std::string, unsigned long> _timeSinceBuff;

			///
			/// Getters for buffer properties
			///
			/// @brief get step speed updated using buffing info
			double getStepSpeed() const;
			/// @brief get step speed updated using buffing info
			double getFullReload() const;
			/// @brief get damage updated using buffing info and bonus from type
			double getDamage(EntityModel const &target_p) const;
			/// @brief get damage updated using buffing info but not bonus from type
			double getDamageNoBonus() const;
			/// @brief get armor updated using buffing info
			double getArmor() const;
			/// @brief get production updated using buffing info
			double getProduction() const;
			/// @brief get harvest updated using buffing info
			double getHarvest() const;

			/// @brief if frozen ou static
			bool isFrozen() const;

			/// @brief get the time since buff of the given buff
			/// @param idBuff_p the if od the buff to test
			/// @return the time since the buff (return limit max if not found)
			unsigned long getTimeSinceBuff(std::string const &idBuff_p) const;
	};
}

std::ostream &operator<<(std::ostream &os_p, octopus::Entity const &ent_p);

std::ostream &streamCsvPos(std::ostream &os_p, octopus::Entity const &ent_p);

std::ostream &streamCsvEntity(std::ostream &os_p, std::vector<octopus::Entity*> const &vec_p);

#endif
