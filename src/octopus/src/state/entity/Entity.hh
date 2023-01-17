#ifndef __ENTITY__
#define __ENTITY__

#include <iostream>
#include <vector>
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

			Vector _pos;

			/// @brief cannot move
			bool _frozen {false};

			Handle _handle {0};

			/// @brief is alive in the state
			bool _alive {false};

			double _hp { 10. };
			/// @brief time since last attack in step
			unsigned long _reload { 10 };

			/// @brief entity model
			EntityModel const &_model;
	};
}

std::ostream &operator<<(std::ostream &os_p, octopus::Entity const &ent_p);

std::ostream &streamCsvPos(std::ostream &os_p, octopus::Entity const &ent_p);

std::ostream &streamCsvEntity(std::ostream &os_p, std::vector<octopus::Entity*> const &vec_p);

#endif
