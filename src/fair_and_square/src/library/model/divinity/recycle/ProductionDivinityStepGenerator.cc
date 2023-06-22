#include "ProductionDivinityStepGenerator.hh"

#include "controller/trigger/Listener.hh"
#include "controller/trigger/Trigger.hh"
#include "library/Library.hh"
#include "state/State.hh"
#include "state/entity/Entity.hh"
#include "state/entity/Unit.hh"
#include "state/player/upgrade/StepUpgradeGenerator.hh"
#include "step/Step.hh"
#include "step/entity/spawn/UnitSpawnStep.hh"
#include "step/player/PlayerSpendResourceStep.hh"
#include "step/player/PlayerBuffAllStep.hh"
#include "step/player/bonus/PlayerAddTimeProdBonusStep.hh"
#include "step/player/bonus/PlayerAddCostBonusStep.hh"
#include "step/trigger/TriggerSpawn.hh"

using namespace octopus;

class ProductionDeathResTrigger : public OnEachTrigger
{
public:
	ProductionDeathResTrigger(Library const &lib_p, unsigned long player_p, std::string const &res_p, Fixed const &qty_p) :
		OnEachTrigger(new ListenerEntityModelDied(&lib_p.getEntityModel("ProductionBuffer"), player_p)),
		_player(player_p),
		_res(res_p),
		_qty(qty_p)
	{}

	virtual void trigger(State const &state_p, Step &step_p, unsigned long, TriggerData const &) const override
	{
		// add resource for the player
		std::map<std::string, Fixed> map_l;
		map_l[_res] = -_qty;
		step_p.addSteppable(new PlayerSpendResourceStep(_player, map_l));
	}

private:
	unsigned long const _player;
	std::string const _res;
	Fixed const _qty;
};

class ProductionDeathRespawnTrigger : public OnEachTrigger
{
public:
	ProductionDeathRespawnTrigger(Library const &lib_p, unsigned long player_p) :
		OnEachTrigger(new ListenerEntityModelDied(&lib_p.getEntityModel("ProductionUnitOne"), player_p)),
		_player(player_p),
		_lib(lib_p)
	{}

	virtual void trigger(State const &state_p, Step &step_p, unsigned long count_p, TriggerData const &data_p) const override
	{
		ListenerEntityData const * entData_l = dynamic_cast<ListenerEntityData const * >(data_p._listenerData[0]);
		Entity const * ent_l = entData_l->_entities[count_p];
		step_p.addSteppable(new UnitSpawnStep(getNextHandle(step_p, state_p), {ent_l->_pos, false, _lib.getUnitModel("ProductionUnitOneRespawn")}));
	}

private:
	unsigned long const _player;
	Library const &_lib;
};

std::vector<Steppable *> productionTierOneGenertor(unsigned long player_p)
{
	std::vector<Steppable *> steps_l;

	// NA
	// just unlocking a unit

	return steps_l;
}

std::vector<Steppable *> productionTierTwoGenertor(Library const &lib_p, unsigned long player_p)
{
	std::vector<Steppable *> steps_l;

	// add starting number of resource for buffer
	std::map<std::string, Fixed> map_l;
	map_l["ProductionBufferResource"] = -4;
	steps_l.push_back(new PlayerSpendResourceStep(player_p, map_l));
	// regain resource on death
	steps_l.push_back(new TriggerSpawn(new ProductionDeathResTrigger(lib_p, player_p, "ProductionBufferResource", 1)));

	return steps_l;
}

std::vector<Steppable *> productionTierThreeGenertor(Library const &lib_p, unsigned long player_p)
{
	std::vector<Steppable *> steps_l;

	steps_l.push_back(new TriggerSpawn(new ProductionDeathRespawnTrigger(lib_p, player_p)));

	return steps_l;
}

class ProductionBuffUpgrade : StepUpgradeGenerator
{
public:
	ProductionBuffUpgrade(std::vector<std::string> const &models_p) : _models(models_p) {}

	virtual ProductionBuffUpgrade* newCopy() const
	{
		return new ProductionBuffUpgrade(_models);
	}

	virtual std::vector<Steppable *> getSteppables(unsigned long player_p) const
	{
		std::vector<Steppable *> steps_l;

		for(std::string const &model_l : _models)
		{
			steps_l.push_back(new PlayerAddTimeProdBonusStep(player_p, model_l, {0, -0.1}));
		}
		return steps_l;
	}

private:
	std::vector<std::string> const _models;
};

class ProductionResourceBuffUpgrade : StepUpgradeGenerator
{
public:
	virtual ProductionResourceBuffUpgrade* newCopy() const
	{
		return new ProductionResourceBuffUpgrade();
	}

	virtual std::vector<Steppable *> getSteppables(unsigned long player_p) const
	{
		std::vector<Steppable *> steps_l;

		steps_l.push_back(new PlayerAddCostBonusStep(player_p, "ProductionUnitOne", "bloc", {0, -0.25}));
		steps_l.push_back(new PlayerAddCostBonusStep(player_p, "ProductionUnitOne", "ether", {0, -0.25}));

		return steps_l;
	}
};

class ProductionMoreBufferUpgrade : StepUpgradeGenerator
{
public:
	virtual ProductionMoreBufferUpgrade* newCopy() const
	{
		return new ProductionMoreBufferUpgrade();
	}

	virtual std::vector<Steppable *> getSteppables(unsigned long player_p) const
	{
		std::vector<Steppable *> steps_l;

		// add starting number of resource for buffer
		std::map<std::string, Fixed> map_l;
		map_l["ProductionBufferResource"] = -4;
		steps_l.push_back(new PlayerSpendResourceStep(player_p, map_l));

		return steps_l;
	}
};
