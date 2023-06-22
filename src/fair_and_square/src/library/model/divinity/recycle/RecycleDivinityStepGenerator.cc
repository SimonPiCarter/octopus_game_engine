#include "RecycleDivinityStepGenerator.hh"

#include "controller/trigger/Listener.hh"
#include "controller/trigger/Trigger.hh"
#include "state/State.hh"
#include "state/player/upgrade/StepUpgradeGenerator.hh"
#include "step/Step.hh"
#include "step/player/PlayerSpendResourceStep.hh"
#include "step/player/PlayerBuffAllStep.hh"
#include "step/trigger/TriggerSpawn.hh"

using namespace octopus;

class RecycleDeathResTrigger : public OnEachTrigger
{
public:
	RecycleDeathResTrigger(unsigned long player_p, std::string const &res_p, Fixed const &qty_p) :
		OnEachTrigger(new ListenerEntityModelDied(nullptr, player_p)),
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

class RecycleDeathBuffTrigger : public OnEachTrigger
{
public:
	RecycleDeathBuffTrigger(std::string const &model_p, unsigned long player_p, Fixed trigger_p, std::vector<TimedBuff> buffs_p) :
		OnEachTrigger(new ListenerEntityModelDied(nullptr, player_p)),
		_player(player_p),
		_model(model_p),
		_trigger(trigger_p),
		_buffs(buffs_p)
	{}

	virtual void trigger(State const &state_p, Step &step_p, unsigned long, TriggerData const &) const override
	{
		// when trigger is exactly reached
		if(getResource(*state_p.getPlayer(_player), "RecycleDeathBuffTrigger") - step_p.getResourceSpent(_player, "RecycleDeathBuffTrigger") == _trigger)
		{
			for(TimedBuff const & buff_l : _buffs)
			{
				step_p.addSteppable(new PlayerBuffAllStep(_player, buff_l, _model));
			}
		}
	}

private:
	unsigned long const _player;
	std::string const _model;
	Fixed const _trigger;
	std::vector<TimedBuff> _buffs;
};

std::vector<Steppable *> recycleTierOneGenertor(unsigned long player_p)
{
	std::vector<Steppable *> steps_l;

	steps_l.push_back(new TriggerSpawn(new RecycleDeathResTrigger(player_p, "RecycleDeathBuffTrigger", 1)));
	steps_l.push_back(new TriggerSpawn(new RecycleDeathBuffTrigger("RecycleUnitTierOne", player_p, 5, {
		{0,0.5,TyppedBuff::Type::HpMax,0,false,"RecycleTier1Hp"},
		{0,0.5,TyppedBuff::Type::Damage,0,false,"RecycleTier1Hp"},
	})));

	return steps_l;
}

std::vector<Steppable *> recycleTierTwoGenertor(unsigned long player_p)
{
	std::vector<Steppable *> steps_l;

	steps_l.push_back(new TriggerSpawn(new RecycleDeathResTrigger(player_p, "bloc", 5)));
	steps_l.push_back(new TriggerSpawn(new RecycleDeathResTrigger(player_p, "ether", 2)));

	return steps_l;
}

std::vector<Steppable *> recycleTierThreeGenertor(unsigned long player_p)
{
	std::vector<Steppable *> steps_l;

	steps_l.push_back(new TriggerSpawn(new RecycleDeathBuffTrigger("RecycleUnitTierOne", player_p, 50, {
		{0,0.5,TyppedBuff::Type::HpMax,0,false,"RecycleTier3Hp"},
		{0,0.5,TyppedBuff::Type::Damage,0,false,"RecycleTier3Hp"},
	})));

	return steps_l;
}

class RecycleHarvestUpgrade : StepUpgradeGenerator
{
public:
    virtual RecycleHarvestUpgrade* newCopy() const
	{
		return new RecycleHarvestUpgrade();
	}

    virtual std::vector<Steppable *> getSteppables(unsigned long player_p) const
	{
		std::vector<Steppable *> steps_l;

		steps_l.push_back(new PlayerBuffAllStep(player_p, {0,0.1,TyppedBuff::Type::Harvest,0,false,"RecycleTier3Hp"}, "deposit"));

		return steps_l;
	}
};

class RecycleDeathResUpgrade : StepUpgradeGenerator
{
public:
    virtual RecycleDeathResUpgrade* newCopy() const
	{
		return new RecycleDeathResUpgrade();
	}

    virtual std::vector<Steppable *> getSteppables(unsigned long player_p) const
	{
		std::vector<Steppable *> steps_l;

		steps_l.push_back(new TriggerSpawn(new RecycleDeathResTrigger(player_p, "bloc", 5)));
		steps_l.push_back(new TriggerSpawn(new RecycleDeathResTrigger(player_p, "ether", 3)));

		return steps_l;
	}
};
