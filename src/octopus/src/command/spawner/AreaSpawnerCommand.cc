
#include "AreaSpawnerCommand.hh"

#include "logger/Logger.hh"
#include "step/Step.hh"
#include "state/State.hh"
#include "step/command/CommandQueueStep.hh"
#include "step/entity/spawn/BuildingSpawnStep.hh"
#include "step/entity/spawn/EntitySpawnStep.hh"
#include "step/entity/spawn/UnitSpawnStep.hh"

namespace octopus
{

AreaSpawnerCommand::AreaSpawnerCommand(std::list<AreaSpawn> const &spawns_p)
	: Command(0)
	, _spawns(spawns_p)
	, _gen(42)
{}

void spawn(State const &state_p, Step & step_p, EntityModel const *model_p, Option const &option_p, unsigned long player_p)
{
	Vector pos_l {option_p.x + std::ceil(2*model_p->_ray)/2., option_p.y + std::ceil(2*model_p->_ray)/2. };
	if(model_p->_isUnit)
	{
		Logger::getDebug()<<"AreaSpawnerCommand :: spawn unit at " << pos_l<<std::endl;
		Unit unit_l(pos_l, false, *dynamic_cast<UnitModel const *>(model_p));
		unit_l._player = player_p;

		step_p.addSteppable(new UnitSpawnStep(getNextHandle(step_p, state_p), unit_l));
	}
	else if(model_p->_isBuilding)
	{
		Logger::getDebug()<<"AreaSpawnerCommand :: spawn building at " << pos_l<<std::endl;
		Building building_l(pos_l, model_p->_isStatic, *dynamic_cast<BuildingModel const *>(model_p));
		building_l._player = player_p;

		step_p.addSteppable(new BuildingSpawnStep(getNextHandle(step_p, state_p), building_l, true));
	}
	else
	{
		Logger::getDebug()<<"AreaSpawnerCommand :: spawn entity  at " << pos_l<<std::endl;
		Entity entity_l(pos_l, model_p->_isStatic, *model_p);
		entity_l._player = player_p;

		step_p.addSteppable(new EntitySpawnStep(getNextHandle(step_p, state_p), entity_l));
	}
}

void update(SpawningGrid &grid_p, EntityModel const *model_p, Option const &option_p)
{
	grid_p.fillGrid(option_p.x, option_p.y, std::ceil(2*model_p->_ray));
}

void AreaSpawnerCommand::registerCommand(Step & step_p, State const &state_p)
{
	Logger::getDebug() << "AreaSpawnerCommand:: register Command "<<std::endl;

	/// init grid from state
	SpawningGrid grid_l(state_p.getPathGrid().getSize());
	initGrid(0, 0, grid_l, state_p.getPathGrid());

	/// @todo maybe update from step?

	/// iterate on avery area spawn to spawn static first
	for(AreaSpawn const &spawn_l : _spawns)
	{
		// on every models
		for(std::pair<EntityModel *, unsigned long> pair_l : spawn_l.models)
		{
			EntityModel * model_l = pair_l.first;
			// skip if not static
			if(!model_l->_isStatic)
			{
				continue;
			}

			// iterate on number of entity to spawn
			for(unsigned long i = 0 ; i < pair_l.second ; ++ i)
			{
				// size is 2*ray that we ceil
				std::vector<Option> options_l = getOptions(spawn_l.x, spawn_l.y, spawn_l.size, spawn_l.size, grid_l, std::ceil(2*model_l->_ray));

				// no room to spawn
				if(options_l.empty())
				{
					continue;
				}

				// randomize an option
				unsigned long optionIdx_l = random(0,options_l.size());

				// add spawn to step
				spawn(state_p, step_p, model_l, options_l[optionIdx_l], spawn_l.player);
				/// update grid from spawn
				update(grid_l, model_l, options_l[optionIdx_l]);
			}
		}
	}

	/// iterate on avery area spawn to spawn non static
	for(AreaSpawn const &spawn_l : _spawns)
	{
		// on every models
		for(std::pair<EntityModel *, unsigned long> pair_l : spawn_l.models)
		{
			EntityModel * model_l = pair_l.first;
			// skip if static
			if(model_l->_isStatic)
			{
				continue;
			}

			// iterate on number of entity to spawn
			for(unsigned long i = 0 ; i < pair_l.second ; ++ i)
			{
				// size is 2*ray that we ceil
				std::vector<Option> options_l = getOptions(spawn_l.x, spawn_l.y, spawn_l.size, spawn_l.size, grid_l, std::ceil(2*model_l->_ray));

				// no room to spawn
				if(options_l.empty())
				{
					continue;
				}

				// randomize an option
				unsigned long optionIdx_l = random(0,options_l.size());

				// add spawn to step
				spawn(state_p, step_p, model_l, options_l[optionIdx_l], spawn_l.player);
				// we do not update grid for non static
			}
		}
	}

	step_p.addSteppable(new CommandStorageStep(this));
}

bool AreaSpawnerCommand::applyCommand(Step &, State const &, CommandData const *) const
{
	// NA
	return true;
}

unsigned long AreaSpawnerCommand::random(unsigned long min_p, unsigned long max_p)
{
	std::uniform_int_distribution<unsigned long> distribution_l(min_p, max_p-1);
	unsigned long rdm_l = min_p;
	if(!_nonRandom)
	{
		rdm_l = distribution_l(_gen);
	}
	return rdm_l;
}

} // namespace octopus

