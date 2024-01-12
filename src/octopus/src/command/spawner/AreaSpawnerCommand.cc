
#include "AreaSpawnerCommand.hh"

#include "logger/Logger.hh"
#include "utils/Vector.hh"
#include "step/Step.hh"
#include "state/State.hh"
#include "step/command/CommandQueueStep.hh"
#include "step/entity/spawn/BuildingSpawnStep.hh"
#include "step/entity/spawn/EntitySpawnStep.hh"
#include "step/entity/spawn/ResourceSpawnStep.hh"
#include "step/entity/spawn/UnitSpawnStep.hh"
#include "step/state/StateTemplePositionAddStep.hh"

namespace octopus
{

AreaSpawnerCommand::AreaSpawnerCommand(RandomGenerator &rand_p, std::list<AreaSpawn> const &spawns_p)
	: Command(Handle(0))
	, _spawns(spawns_p)
	, _rand(&rand_p)
{}

AreaSpawnerCommand::~AreaSpawnerCommand()
{
	for(AreaSpawn const &spawn_l : _spawns)
	{
		for(std::pair<Entity *, unsigned long> const &pair_l : spawn_l.entities)
		{
			delete pair_l.first;
		}
	}
}

void spawn(State const &state_p, Step & step_p, Entity const *model_p, Option const &option_p)
{
	Vector pos_l {option_p.x + numeric::ceil(2*model_p->_model._ray)/2., option_p.y + numeric::ceil(2*model_p->_model._ray)/2. };
	if(model_p->_model._isUnit)
	{
		Logger::getDebug()<<"AreaSpawnerCommand :: spawn unit at " << pos_l<<std::endl;
		Unit unit_l(*dynamic_cast<Unit const *>(model_p));
		unit_l._pos = pos_l;

		step_p.addSteppable(state_p, new UnitSpawnStep(getNextHandle(step_p, state_p), unit_l));
	}
	else if(model_p->_model._isBuilding)
	{
		Logger::getDebug()<<"AreaSpawnerCommand :: spawn building at " << pos_l<<std::endl;
		Building building_l(*dynamic_cast<Building const *>(model_p));
		building_l._pos = pos_l;

		step_p.addSteppable(state_p, new BuildingSpawnStep(getNextHandle(step_p, state_p), building_l, true));

		// special case for abandonned temple
		if(model_p->_model._isAbandonedTemple)
		{
			Logger::getDebug()<<"AreaSpawnerCommand :: spawn abandonned temple at " << pos_l<<std::endl;
			step_p.addSteppable(state_p, new StateTemplePositionAddStep(pos_l));
		}
	}
	else if(model_p->_model._isResource)
	{
		Logger::getDebug()<<"AreaSpawnerCommand :: spawn resource at " << pos_l<<std::endl;
		Resource res_l(*dynamic_cast<Resource const *>(model_p));
		res_l._pos = pos_l;

		step_p.addSteppable(state_p, new ResourceSpawnStep(getNextHandle(step_p, state_p), res_l));
	}
	else
	{
		Logger::getDebug()<<"AreaSpawnerCommand :: spawn entity at " << pos_l<<std::endl;
		Entity entity_l(*model_p);
		entity_l._pos = pos_l;
		step_p.addSteppable(state_p, new EntitySpawnStep(getNextHandle(step_p, state_p), entity_l));
	}
}

void update(SpawningGrid &grid_p, EntityModel const &model_p, Option const &option_p)
{
	grid_p.fillGrid(option_p.x, option_p.y, numeric::ceil(2.*model_p._ray).to_uint());
}

void AreaSpawnerCommand::registerCommand(Step & step_p, State const &state_p)
{
	Logger::getDebug() << "AreaSpawnerCommand:: register Command "<<std::endl;

	/// init grid from state
	SpawningGrid grid_l(state_p.getPathGrid().getSizeX());
	initGrid(0, 0, grid_l, state_p.getPathGrid());

	/// update from step
	for(Steppable const *steppable_l : step_p.getSteppable())
	{
		AbstractSpawnModelStep const * spawner_l = dynamic_cast<AbstractSpawnModelStep const *>(steppable_l);

		if(!spawner_l)
		{
			continue;
		}

		Entity const *ent_l = spawner_l->getEntityTemplate();

		// if static
		if(ent_l->_model._isStatic)
		{
			grid_l.fillGrid(
				numeric::floor(ent_l->_pos.x-ent_l->_model._ray).to_uint(),
				numeric::floor(ent_l->_pos.y-ent_l->_model._ray).to_uint(),
				numeric::ceil(2.*ent_l->_model._ray).to_uint());
		}
	}

	/// iterate on avery area spawn to spawn static first
	for(AreaSpawn const &spawn_l : _spawns)
	{
		// on every models
		for(std::pair<Entity *, unsigned long> pair_l : spawn_l.entities)
		{
			Entity * model_l = pair_l.first;
			// skip if not static
			if(!model_l->_model._isStatic)
			{
				continue;
			}

			// iterate on number of entity to spawn
			for(unsigned long i = 0 ; i < pair_l.second ; ++ i)
			{
				// size is 2*ray that we ceil
				std::vector<Option> options_l = getOptions(spawn_l.x, spawn_l.y, spawn_l.width, spawn_l.height, grid_l, numeric::ceil(2*model_l->_model._ray).to_uint());

				// no room to spawn
				if(options_l.empty())
				{
					continue;
				}

				// randomize an option
				unsigned long optionIdx_l = random(0,options_l.size());

				// add spawn to step
				spawn(state_p, step_p, model_l, options_l[optionIdx_l]);
				if(_replicatorSetUp)
				{
					spawn(state_p, step_p, model_l, _replicator(options_l[optionIdx_l]));
				}
				/// update grid from spawn
				update(grid_l, model_l->_model, options_l[optionIdx_l]);
			}
		}
	}

	/// iterate on avery area spawn to spawn non static
	for(AreaSpawn const &spawn_l : _spawns)
	{
		// on every models
		for(std::pair<Entity *, unsigned long> pair_l : spawn_l.entities)
		{
			Entity * model_l = pair_l.first;
			// skip if static
			if(model_l->_model._isStatic)
			{
				continue;
			}

			// iterate on number of entity to spawn
			for(unsigned long i = 0 ; i < pair_l.second ; ++ i)
			{
				// size is 2*ray that we ceil
				std::vector<Option> options_l = getOptions(spawn_l.x, spawn_l.y, spawn_l.width, spawn_l.height, grid_l, numeric::ceil(2*model_l->_model._ray).to_uint());

				// no room to spawn
				if(options_l.empty())
				{
					continue;
				}

				// randomize an option
				unsigned long optionIdx_l = random(0,options_l.size());

				// add spawn to step
				spawn(state_p, step_p, model_l, options_l[optionIdx_l]);
				if(_replicatorSetUp)
				{
					spawn(state_p, step_p, model_l, _replicator(options_l[optionIdx_l]));
				}
				// we do not update grid for non static
			}
		}
	}

	step_p.addSteppable(state_p, new CommandStorageStep(this));
}

bool AreaSpawnerCommand::applyCommand(StepShallow &, State const &, CommandData const *, PathManager &) const
{
	// NA
	return true;
}

unsigned long AreaSpawnerCommand::random(unsigned long min_p, unsigned long max_p)
{
	unsigned long rdm_l = min_p;
	if(!_nonRandom)
	{
		rdm_l = _rand->roll(min_p, max_p-1);
	}
	return rdm_l;
}

void AreaSpawnerCommand::setReplicationOption(std::function<Option(Option const &)> const &replicator_p)
{
	_replicatorSetUp = true;
	_replicator = replicator_p;
}


} // namespace octopus

