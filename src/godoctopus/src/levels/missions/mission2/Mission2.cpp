#include "Mission2.h"

#include <iostream>
#include <fstream>
#include <random>

// fas
#include "library/model/ModelLoader.hh"

// octopus
#include "controller/trigger/Listener.hh"
#include "controller/trigger/Trigger.hh"
#include "library/Library.hh"
#include "state/entity/buff/TimedBuff.hh"
#include "step/entity/buff/EntityBuffStep.hh"
#include "step/trigger/TriggerSpawn.hh"

// godot
#include "controller/step/CameraStep.h"
#include "controller/step/DialogStep.h"

// missions
#include "levels/missions/helpers/DialogTrigger.h"
#include "levels/missions/helpers/SpawnerTrigger.h"

using namespace octopus;

namespace godot
{
namespace mission
{

std::list<Steppable *> Mission2Steps(Library &lib_p, RandomGenerator &rand_p, unsigned long nbPlayers_p, std::vector<GodotEntityInfo> const &entityInfo_p)
{
	loadMinimalModels(lib_p);

	std::list<Steppable *> spawners_l;

	Vector boxPos_l(10, 10);
	Vector boxSize_l(10, 10);

	for(unsigned long player_l = 0 ; player_l < 3 ; ++ player_l)
	{
		std::vector<unsigned long> set_l = getHandles(entityInfo_p, player_l, "chosen_square");

		for(unsigned long handleIndex_l : set_l)
		{
			Handle handle_l(handleIndex_l);

			Trigger * triggerBuff_l = new OneShotFunctionTrigger({new ListenerEntityInBox({handle_l}, boxPos_l, boxSize_l)},
			[handle_l](State const &state_p, Step &step_p, unsigned long, TriggerData const &)
			{
				TimedBuff buff_l;
				buff_l._id = "AttackSpeedDivintiy_BuffDamageConditionalTierThree";
				buff_l._duration = 3000;
				buff_l._attackMod = AoEModifier(0.5, 5);
				// buff_l._type = TyppedBuff::Type::Damage;
				// buff_l._offset = 10;
				step_p.addSteppable(new EntityBuffStep(handle_l, buff_l));
			});

			spawners_l.push_back(new TriggerSpawn(triggerBuff_l));
		}

	}

	return spawners_l;
}

std::list<Command *> Mission2Commands(Library &lib_p, RandomGenerator &rand_p, unsigned long nbPlayers_p)
{
	std::list<Command *> commands_l {
	};

	return commands_l;
}

/// @brief write header for classic arena level
void writeMission2Header(std::ofstream &file_p, Mission2Header const &header_p)
{
    file_p.write((char*)&header_p.seed, sizeof(header_p.seed));
    file_p.write((char*)&header_p.nb_players, sizeof(header_p.nb_players));
}

/// @brief read header for classic arena level and return a pair of steppable and command
std::pair<std::list<octopus::Steppable *>, std::list<octopus::Command *> > readMission2Header(octopus::Library &lib_p, std::ifstream &file_p,
	octopus::RandomGenerator * &rand_p, Mission2Header &header_r)
{
    file_p.read((char*)&header_r.seed, sizeof(header_r.seed));
    file_p.read((char*)&header_r.nb_players, sizeof(header_r.nb_players));

	delete rand_p;
	rand_p = new octopus::RandomGenerator(header_r.seed);

	std::pair<std::list<octopus::Steppable *>, std::list<octopus::Command *> > pair_l;
	pair_l.first = Mission2Steps(lib_p, *rand_p, header_r.nb_players, {});
	pair_l.second = Mission2Commands(lib_p, *rand_p, header_r.nb_players);
	return pair_l;
}


} // namespace Mission2
} // namespace godot
