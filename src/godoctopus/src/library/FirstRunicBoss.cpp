#include "FirstRunicBoss.h"

#include "library/Library.hh"
#include "state/State.hh"
#include "state/entity/Unit.hh"
#include "step/Step.hh"
#include "step/entity/EntityAttackStep.hh"
#include "step/entity/EntityHitPointChangeStep.hh"
#include "step/entity/spawn/UnitSpawnStep.hh"
#include "step/unit/UnitDataStep.hh"
#include "step/custom/implem/ImpactStep.hh"

#include "controller/step/library/FirstRunicBossStep.h"

#include "logger/Logger.hh"

namespace godot
{

struct FirstRunicBossStaticData : public octopus::StaticUnitData
{
	/// @brief Create a new UnitData that is a copy of this one
	/// @note a new allocation must be performed
	virtual octopus::StaticUnitData* clone() const
	{
		FirstRunicBossStaticData * new_l = new FirstRunicBossStaticData();
		*new_l = *this;
		return new_l;
	}

	// aoe params
	unsigned long long aoe_first_spawn = 500;
	unsigned long long aoe_reload_spawn = 1500;
	unsigned long long aoe_spawn_time = 250;
	unsigned long long aoe_count = 2;
	unsigned long long aoe_spawn_time_range = 100;
	octopus::Fixed aoe_aggro_range = 8;
	octopus::Fixed aoe_range = 2;
	octopus::Fixed aoe_damage = 75;

	// adds params
	unsigned long long nb_adds = 10;
	unsigned long long adds_reload_time = 6000;
	unsigned long long adds_first_time = 200;
	octopus::UnitModel const *add_model = nullptr;

	unsigned long seed = 42;
};

/// @brief Contain info for an aoe zone
/// - pos
/// - idx
/// - spawn_time
/// - triggered
struct AoEInfo
{
	octopus::Vector pos;
	unsigned int idx = 0;
	unsigned long long spawn_time = 0;
	bool triggered = false;
};

struct FirstRunicBossData : public octopus::UnitData
{
	/// @brief Create a new UnitData that is a copy of this one
	/// @note a new allocation must be performed
	virtual octopus::UnitData* clone() const
	{
		FirstRunicBossData * new_l = new FirstRunicBossData();
		new_l->copyFrom(this);
		return new_l;
	}
	/// @brief copy everything from another UnitData
	virtual void copyFrom(octopus::UnitData const *other_p)
	{
		FirstRunicBossData const *data_l = dynamic_cast<FirstRunicBossData const *>(other_p);
		*this = *data_l;
	}

	bool enabled = false;
	unsigned long long last_check = 0;
	unsigned long long enable_time = 0;


	/// @brief step id of last aoe spawn
	unsigned long long last_aoe = 0;
	std::vector<AoEInfo> aoe_info;

	unsigned long long last_adds = 0;

};

bool shouldEnable(octopus::Unit const &unit_p, octopus::State const &state_p)
{
	octopus::TargetPanel panel_l = octopus::lookUpNewTargets(state_p, unit_p._handle, 10, true);
	return !panel_l.units.empty();
}

void firstRunicBossRoutine(octopus::Entity const &ent_p, octopus::Step & step_p, octopus::State const &state_p)
{
	octopus::Unit const &unit_l = static_cast<octopus::Unit const &>(ent_p);
	FirstRunicBossData const * data_l = static_cast<FirstRunicBossData const *>(unit_l._unitData.get());
	FirstRunicBossStaticData const * sData_l = static_cast<FirstRunicBossStaticData const *>(unit_l._staticUnitData);
	FirstRunicBossData * new_data_l = static_cast<FirstRunicBossData *>(data_l->clone());
	bool change_done_l = false;

	// random generator used locally
	octopus::RandomGenerator rand_l(sData_l->seed + step_p.getId());

	// if not enabled we check if we need to enable
	if(step_p.getId() >= data_l->last_check + 50)
	{
		// by default keep same enabled status
		new_data_l->enabled = shouldEnable(unit_l, state_p);
		// if enabling
		if(new_data_l->enabled && !data_l->enabled)
		{
			octopus::Logger::getDebug()<<"firstRunicBossRoutine :: enabling"<<std::endl;
			new_data_l->enable_time = step_p.getId();
			change_done_l = true;
		}

		// if disabling
		if(!new_data_l->enabled)
		{
			octopus::Logger::getDebug()<<"firstRunicBossRoutine :: disabling"<<std::endl;
			// destroy every adds
			for(octopus::Entity const *ent_l : state_p.getEntities())
			{
				if(&ent_l->_model == sData_l->add_model
				&& ent_l->_player == unit_l._player)
				{
					octopus::Fixed curHp_l = ent_l->_hp + step_p.getHpChange(ent_l->_handle);
					octopus::Fixed maxHp_l = ent_l->getHpMax();
					step_p.addSteppable(new octopus::EntityHitPointChangeStep(ent_l->_handle, -maxHp_l * 0.2, curHp_l, maxHp_l));
				}
			}

			// heal
			octopus::Fixed curHp_l = unit_l._hp + step_p.getHpChange(unit_l._handle);
			octopus::Fixed maxHp_l = unit_l.getHpMax();
			step_p.addSteppable(new octopus::EntityHitPointChangeStep(unit_l._handle, maxHp_l * 0.05, curHp_l, maxHp_l));
		}

		// change last check in data
		new_data_l->last_check = step_p.getId();
		change_done_l = true;
	}

	if(data_l->enabled)
	{
		// check if we spawn aoe info
		if(data_l->aoe_info.empty()
		&& (data_l->last_aoe + sData_l->aoe_reload_spawn <= step_p.getId() || data_l->last_aoe == 0)
		&& data_l->enable_time + sData_l->aoe_first_spawn <= step_p.getId())
		{
			octopus::TargetPanel panel_l = octopus::lookUpNewTargets(state_p, unit_l._handle, sData_l->aoe_aggro_range, true);
			std::list<octopus::Entity const *> &units_l = panel_l.units;
			for(unsigned int c = 0 ; c < sData_l->aoe_count && !units_l.empty(); ++ c)
			{
				int index_l = rand_l.roll(0, units_l.size()-1);
				auto it_l = std::next(units_l.begin(), index_l);
				octopus::Entity const * target_l = *it_l;
				units_l.erase(it_l);

				// create aoe info
				AoEInfo info_l {
					target_l->_pos,
					c,
					step_p.getId() + rand_l.roll(1, sData_l->aoe_spawn_time_range),
					false
				};
				octopus::Logger::getDebug()<<"firstRunicBossRoutine :: spawning aoe "<<target_l->_pos<<std::endl;
				new_data_l->aoe_info.push_back(info_l);
			}
			new_data_l->last_aoe = step_p.getId();
			change_done_l = true;
		}
		// if aoe are spawned
		else if(!data_l->aoe_info.empty())
		{
			unsigned long team_l = state_p.getPlayer(unit_l._player)->_team;
			bool non_triggered_aoe = false;
			for(AoEInfo &info_l : new_data_l->aoe_info)
			{
				if(step_p.getId() == info_l.spawn_time)
				{
					octopus::Logger::getDebug()<<"firstRunicBossRoutine :: spawning aoe for real"<<std::endl;
					step_p.addSteppable(new FirstRunicBossStep(info_l.idx, false, info_l.pos.x.to_double(), info_l.pos.y.to_double(), sData_l->aoe_range.to_double()));
				}

				// trigger aoe
				if(step_p.getId() >= info_l.spawn_time + sData_l->aoe_spawn_time && !info_l.triggered)
				{
					octopus::Logger::getDebug()<<"firstRunicBossRoutine :: damaging aoe"<<std::endl;
					// trigger aoe
					octopus::TargetPanel panel_l = octopus::lookUpNewTargets(state_p, unit_l._handle.index, info_l.pos, team_l, sData_l->aoe_range, true);
					for(octopus::Entity const *ent_l : panel_l.units)
					{
						octopus::Logger::getDebug()<<"firstRunicBossRoutine :: \t"<<ent_l->_handle.index<<" dmg = "<<sData_l->aoe_damage.to_double()<<std::endl;
						octopus::Fixed curHp_l = ent_l->_hp + step_p.getHpChange(ent_l->_handle);
						octopus::Fixed maxHp_l = ent_l->getHpMax();
						step_p.addSteppable(new octopus::EntityHitPointChangeStep(ent_l->_handle, -sData_l->aoe_damage, curHp_l, maxHp_l));
					}

					step_p.addSteppable(new FirstRunicBossStep(info_l.idx, true, info_l.pos.x.to_double(), info_l.pos.y.to_double(), sData_l->aoe_range.to_double()));

					// update triggered and changed done
					info_l.triggered = true;
					change_done_l = true;
				}
				non_triggered_aoe |= !info_l.triggered;
			}

			// if all aoe are triggered
			if(!non_triggered_aoe)
			{
				octopus::Logger::getDebug()<<"firstRunicBossRoutine :: reseting aoe"<<std::endl;
				// empty aoe info
				new_data_l->aoe_info = {};
				change_done_l = true;
			}
		}


		// check if spawn adds
		if(sData_l->add_model
		&& (data_l->last_adds + sData_l->adds_reload_time <= step_p.getId() || data_l->last_adds == 0)
		&& data_l->enable_time + sData_l->adds_first_time <= step_p.getId())
		{
			for(unsigned int c = 0 ; c < sData_l->nb_adds; ++ c)
			{
				int x = rand_l.roll(1, 5);
				int y = rand_l.roll(1, 5);
				if(rand_l.roll(0, 1) > 0) { x = -x; }
				if(rand_l.roll(0, 1) > 0) { y = -y; }
				octopus::Unit model_l(unit_l._pos + octopus::Vector(x, y), false, *sData_l->add_model);
				model_l._player = unit_l._player;
				step_p.addSteppable(new octopus::UnitSpawnStep(getNextHandle(step_p, state_p), model_l));
			}

			new_data_l->last_adds = step_p.getId();
			change_done_l = true;
		}
	}

	// auto attack
	if(unit_l._reload >= unit_l.getFullReload())
	{
		octopus::Entity const * entTarget_l = lookUpNewTarget(state_p, unit_l._handle, unit_l._model._range, false);
		if(entTarget_l)
		{
			octopus::Player const * playerSource_l = state_p.getPlayer(unit_l._player);
			octopus::AttackModifierData attackModData_l {
				unit_l._handle,
				entTarget_l->_handle,
				entTarget_l->_pos,
				playerSource_l->_team,
				unit_l.getDamageNoBonus(),
				unit_l.getDamage(entTarget_l->_model)
			};
			if(unit_l._model._projectile)
			{
				octopus::Projectile projectile_l;
				projectile_l._pos = unit_l._pos;
				projectile_l._posTarget = entTarget_l->_pos;
				projectile_l._target = entTarget_l->_handle;
				projectile_l._source = unit_l._handle;
				projectile_l._sourceModel = &unit_l._model;
				projectile_l._sourceTeam = playerSource_l->_team;
				projectile_l._speed = 0.1;
				projectile_l._baseDamage = unit_l.getDamageNoBonus();
				projectile_l._bonusDamage = unit_l.getDamage(entTarget_l->_model);
				projectile_l._generator = unit_l._attackMod;
				step_p.getProjectileSpawnStep().addProjectile(state_p.getProjectileContainer(), std::move(projectile_l));
			}
			else
			{
				step_p.addSteppable(new octopus::ImpactStep(unit_l._model._id, entTarget_l->_pos));
				newAttackSteppable(unit_l._attackMod, step_p, attackModData_l, state_p);
			}
			step_p.addSteppable(new octopus::EntityAttackStep(unit_l._handle, unit_l._reload));
		}
	}

	if(change_done_l)
	{
		octopus::Logger::getDebug()<<"firstRunicBossRoutine :: adding change"<<std::endl;
		step_p.addSteppable(new octopus::UnitDataStep(unit_l._handle, new_data_l));
	}
	else
	{
		delete new_data_l;
	}
}

void addFirstRunicBossToLibrary(octopus::Library &lib_p)
{

    octopus::UnitModel unitModel_l { false, 0.5, 0.05, 70. };
    unitModel_l._isUnit = true;
    unitModel_l._damage = 10;
    unitModel_l._armor = 0;
    unitModel_l._range = 0.5;
    unitModel_l._lineOfSight = 20;
    unitModel_l._fullReload = 300;
    unitModel_l._windup = 20;

    lib_p.registerUnitModel("firstRunicBoss_add", unitModel_l);

	octopus::UnitModel firstRunicBossModel_l { true, 1.9, 0., 7500 };
	firstRunicBossModel_l._projectile = true;
	firstRunicBossModel_l._isUnit = true;
	firstRunicBossModel_l._damage = 10;
	firstRunicBossModel_l._armor = 3;
	firstRunicBossModel_l._range = 10.;
	firstRunicBossModel_l._lineOfSight = 50;
	firstRunicBossModel_l._fullReload = 80;
	firstRunicBossModel_l._windup = 20;
	firstRunicBossModel_l._idleFunc = firstRunicBossRoutine;
	firstRunicBossModel_l._unitData = new FirstRunicBossData();
	FirstRunicBossStaticData *sData_l = new FirstRunicBossStaticData();
	sData_l->add_model = &lib_p.getUnitModel("firstRunicBoss_add");
	firstRunicBossModel_l._staticUnitData = sData_l;

	lib_p.registerUnitModel("firstRunicBoss", firstRunicBossModel_l);
}

} // namespace godot