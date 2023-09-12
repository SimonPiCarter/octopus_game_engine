#ifndef __WaveDuelLevel__
#define __WaveDuelLevel__

#include <list>
#include <vector>
#include <functional>
#include "controller/trigger/Trigger.hh"
#include "utils/RandomGenerator.hh"

// fas
#include "library/model/divinity/DivinityModelLoader.hh"

#define LEVEL_ID_DUEL 1004
#define LEVEL_DUEL_SIZE 250

namespace octopus
{
	class Command;
	class Library;
	class State;
	class Step;
	class Steppable;
} // namespace octopus

namespace godot
{
namespace duellevel
{

std::list<octopus::Steppable *> LevelSteps(octopus::Library &lib_p, octopus::RandomGenerator &rand_p, int stepCount_p, bool buffProd_p,
	std::vector<fas::DivinityType> const &divinitiesPlayer1_p, std::vector<fas::DivinityType> const &divinitiesPlayer2_p);

std::list<octopus::Command *> LevelCommands(octopus::Library &lib_p, octopus::RandomGenerator &rand_p);

struct DuelLevelHeader
{
	// seed for random generation
	int seed = 0;
	// step count to end
	int step_count = 0;

	bool buff_prod {false};

	// divinity types of player 1
	std::vector<fas::DivinityType> div_player_1;
	// divinity types of player 2
	std::vector<fas::DivinityType> div_player_2;
};

/// @brief write header for classic arena level
void writeLevelHeader(std::ofstream &file_p, DuelLevelHeader const &header_p);
/// @brief read header for classic arena level and return a pair of steppable and command (warning delete and reset rand_p using the written seed)
std::pair<std::list<octopus::Steppable *>, std::list<octopus::Command *> > readLevelHeader(octopus::Library &lib_p, std::ifstream &file_p,
	octopus::RandomGenerator * &rand_p, DuelLevelHeader &header_r);

class LoseTrigger : public octopus::OneShotTrigger
{
public:
	LoseTrigger(octopus::Listener * listener_p, unsigned long player_p);

	virtual void trigger(octopus::State const &state_p, octopus::Step &step_p, unsigned long, octopus::TriggerData const &) const override;
private:
    unsigned long const _player;
};

} // namespace duellevel
} // namespace godot

#endif
