#ifndef __AreaSpawnerCommand__
#define __AreaSpawnerCommand__

#include <list>
#include <random>
#include <functional>

#include "command/Command.hh"
#include "utils/RandomGenerator.hh"

#include "SpawningGrid.hh"
#include "Area.hh"

namespace octopus
{
class Entity;

/// @brief This command will spawn models randomly in the given areas
/// @warning if multiple area spawner share the same area during the same
/// step they may spawn static on top of eachoter
class AreaSpawnerCommand : public Command
{
public:
	AreaSpawnerCommand() {}
	AreaSpawnerCommand(RandomGenerator &rand_p, std::list<AreaSpawn> const &spawns_p);
	~AreaSpawnerCommand();

	virtual void registerCommand(Step & step_p, State const &state_p) override;

	/// @brief
	virtual bool applyCommand(Step & step_p, State const &state_p, CommandData const *data_p, PathManager &pathManager_p) const override;

	/// @brief for testing purpose mainly spawner can be set up to be non random
	/// @note in that case the first option (first left, top) will be taken
	void setNonRandom() { _nonRandom = true; }

	/// @brief cannot be commited by a player
	virtual bool checkPlayer(State const &, unsigned long ) const override { return false; }

	void setReplicationOption(std::function<Option(Option const &)> const &replicator_p);

private:
	/// @brief randomize between min_p and max_p - 1
	/// @note if _nonRandom is true then always return min_p
	unsigned long random(unsigned long min_p, unsigned long max_p);

	/// @brief list of spawns
	std::list<AreaSpawn> _spawns;

	/// @brief random generator
	RandomGenerator *_rand {nullptr};

	bool _nonRandom {false};

	bool _replicatorSetUp {false};
	std::function<Option(Option const &)> _replicator;
};

} // namespace octopus


#endif
