#ifndef __ENTITY__
#define __ENTITY__

#include <iostream>
#include <vector>
#include "Vector.hh"

namespace game_engine
{
	class Entity
	{
		public:
			Vector _pos;
			double _ray {1.};

			/// @brief cannot move
			bool _frozen {false};

			double _stepSpeed {0.25};
	};
}

std::ostream &operator<<(std::ostream &os_p, game_engine::Entity const &ent_p);

std::ostream &streamCsvPos(std::ostream &os_p, game_engine::Entity const &ent_p);

std::ostream &streamCsvEntity(std::ostream &os_p, std::vector<game_engine::Entity> const &vec_p);

#endif
