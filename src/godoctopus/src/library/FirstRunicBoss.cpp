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
#include "controller/step/DialogStep.h"

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
	bool aoe_enabled = true;
	unsigned long long aoe_first_spawn = 500;
	unsigned long long aoe_reload_spawn = 1500;
	unsigned long long aoe_spawn_time = 250;
	unsigned long long aoe_count = 2;
	unsigned long long aoe_spawn_time_range = 100;
	octopus::Fixed aoe_aggro_range = 8;
	octopus::Fixed aoe_range = 2;
	octopus::Fixed aoe_damage = 75;
	// phase 2
	unsigned long long aoe_reload_spawn_p2 = 1500;
	unsigned long long aoe_count_p2 = 3;
	octopus::Fixed aoe_damage_p2 = 150;

	// adds params
	bool adds_enabled = true;
	unsigned long long nb_adds = 10;
	unsigned long long adds_reload_time = 6000;
	unsigned long long adds_first_time = 200;
	octopus::UnitModel const *add_model = nullptr;

	// pilar params
	bool pillars_enabled = true;
	unsigned long long nb_pillars = 2;
	unsigned long long pillars_reload_time = 1500;
	unsigned long long pillars_expiration_time = 1000;
	octopus::UnitModel const *pillar_model = nullptr;

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
	unsigned long long last_pillars = 0;
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

	bool phase1_l = unit_l._hp * 2 > unit_l.getHpMax();
	bool phase2_l = !phase1_l;

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
				if(sData_l->adds_enabled
				&& &ent_l->_model == sData_l->add_model
				&& ent_l->_player == unit_l._player
				&& ent_l->_alive)
				{
					octopus::Fixed curHp_l = ent_l->_hp + step_p.getHpChange(ent_l->_handle);
					octopus::Fixed maxHp_l = ent_l->getHpMax();
					step_p.addSteppable(new octopus::EntityHitPointChangeStep(ent_l->_handle, -maxHp_l * 0.2, curHp_l, maxHp_l));
				}
				if(sData_l->pillars_enabled
				&& &ent_l->_model == sData_l->pillar_model
				&& ent_l->_player == unit_l._player
				&& ent_l->_alive)
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
		bool aoe_spawn_reload = (data_l->last_aoe + sData_l->aoe_reload_spawn <= step_p.getId() && phase1_l)
							||  (data_l->last_aoe + sData_l->aoe_reload_spawn_p2 <= step_p.getId() && phase2_l);
		// check if we spawn aoe info
		if(sData_l->aoe_enabled
		&& data_l->aoe_info.empty()
		&& (aoe_spawn_reload || data_l->last_aoe == 0)
		&& data_l->enable_time + sData_l->aoe_first_spawn <= step_p.getId())
		{
			octopus::TargetPanel panel_l = octopus::lookUpNewTargets(state_p, unit_l._handle, sData_l->aoe_aggro_range, true);
			std::list<octopus::Entity const *> &units_l = panel_l.units;
			unsigned long long aoe_count = phase1_l ? sData_l->aoe_count : sData_l->aoe_count_p2;
			for(unsigned int c = 0 ; c < aoe_count && !units_l.empty(); ++ c)
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
					step_p.addSteppable(new FirstRunicBossStep(ent_p._handle.index, info_l.idx, false, info_l.pos.x.to_double(), info_l.pos.y.to_double(), sData_l->aoe_range.to_double()));
				}

				// trigger aoe
				if(step_p.getId() >= info_l.spawn_time + sData_l->aoe_spawn_time && !info_l.triggered)
				{
					octopus::Logger::getDebug()<<"firstRunicBossRoutine :: damaging aoe"<<std::endl;
					// trigger aoe
					octopus::TargetPanel panel_l = octopus::lookUpNewTargets(state_p, unit_l._handle.index, info_l.pos, team_l, sData_l->aoe_range, true);
					octopus::Fixed dmg_l = phase1_l ? sData_l->aoe_damage : sData_l->aoe_damage_p2;
					for(octopus::Entity const *ent_l : panel_l.units)
					{
						octopus::Logger::getDebug()<<"firstRunicBossRoutine :: \t"<<ent_l->_handle.index<<" dmg = "<<dmg_l.to_double()<<std::endl;
						octopus::Fixed curHp_l = ent_l->_hp + step_p.getHpChange(ent_l->_handle);
						octopus::Fixed maxHp_l = ent_l->getHpMax();
						step_p.addSteppable(new octopus::EntityHitPointChangeStep(ent_l->_handle, -dmg_l, curHp_l, maxHp_l));
					}

					step_p.addSteppable(new FirstRunicBossStep(ent_p._handle.index, info_l.idx, true, info_l.pos.x.to_double(), info_l.pos.y.to_double(), sData_l->aoe_range.to_double()));

					// update triggered and changed done
					info_l.triggered = true;
					change_done_l = true;
				}
				non_triggered_aoe |= !info_l.triggered;
			}

			// if all aoe are triggered
			if(!non_triggered_aoe)
			{
				octopus::Logger::getDebug()<<"firstRunicBossRoutine :: resetting aoe"<<std::endl;
				// empty aoe info
				new_data_l->aoe_info = {};
				change_done_l = true;
			}
		}


		// check if spawn adds
		if(sData_l->adds_enabled
		&& sData_l->add_model
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

		// check if spawn pillars
		if(sData_l->pillars_enabled
		&& phase2_l
		&& sData_l->pillar_model
		&& (data_l->last_pillars + sData_l->pillars_reload_time <= step_p.getId() || data_l->last_pillars == 0))
		{
			for(unsigned int c = 0 ; c < sData_l->nb_pillars; ++ c)
			{
				int x = rand_l.roll(2, 5);
				int y = rand_l.roll(2, 5);
				if(rand_l.roll(0, 1) > 0) { x = -x; }
				if(rand_l.roll(0, 1) > 0) { y = -y; }
				octopus::Unit model_l(unit_l._pos + octopus::Vector(x, y), false, *sData_l->pillar_model);
				model_l._player = unit_l._player;
				step_p.addSteppable(new octopus::UnitSpawnStep(getNextHandle(step_p, state_p), model_l));
			}
			step_p.addSteppable(new FirstRunicBossPillar(ent_p._handle.index, true, data_l->last_pillars == 0));
			if(data_l->last_pillars == 0)
			{
				step_p.addSteppable(new DialogStep("first_runic_boss_first_pillar"));
			}
			new_data_l->last_pillars = step_p.getId();
			change_done_l = true;
		}

		// check if we trigger pillar explosion
		if(sData_l->pillars_enabled
		&& phase2_l
		&& sData_l->pillar_model
		&& (data_l->last_pillars + sData_l->pillars_expiration_time == step_p.getId()))
		{
			bool any_pillar_alive_l = false;
			// if any pillar is still alive
			for(octopus::Entity const *ent_l : state_p.getEntities())
			{
				if(&ent_l->_model == sData_l->pillar_model
				&& ent_l->_player == unit_l._player
				&& ent_l->_alive)
				{
					octopus::Fixed curHp_l = ent_l->_hp + step_p.getHpChange(ent_l->_handle);
					octopus::Fixed maxHp_l = ent_l->getHpMax();
					step_p.addSteppable(new octopus::EntityHitPointChangeStep(ent_l->_handle, -maxHp_l * 10, curHp_l, maxHp_l));
					any_pillar_alive_l = true;
				}
			}
			if(any_pillar_alive_l)
			{
				octopus::TargetPanel panel_l = octopus::lookUpNewTargets(state_p, ent_p._handle, 10, true);
				for(octopus::Entity const *ent_l : panel_l.units)
				{
					octopus::Fixed curHp_l = ent_l->_hp + step_p.getHpChange(ent_l->_handle);
					octopus::Fixed maxHp_l = ent_l->getHpMax();
					step_p.addSteppable(new octopus::EntityHitPointChangeStep(ent_l->_handle, -maxHp_l * 0.9, curHp_l, maxHp_l));
				}
				step_p.addSteppable(new FirstRunicBossPillar(ent_p._handle.index, false, false));
			}
		}

		// check for updating pillar explosion
		if(sData_l->pillars_enabled
		&& phase2_l
		&& sData_l->pillar_model)
		{
			bool step1_l = data_l->last_pillars + 1*sData_l->pillars_expiration_time/4 == step_p.getId();
			bool step2_l = data_l->last_pillars + 2*sData_l->pillars_expiration_time/4 == step_p.getId();
			bool step3_l = data_l->last_pillars + 3*sData_l->pillars_expiration_time/4 == step_p.getId();

			// only if one step need to be updated
			if(step1_l
			|| step2_l
			|| step3_l)
			{
				int step_l = 1;
				if(step2_l)
				{
					step_l = 2;
				}
				else if(step3_l)
				{
					step_l = 3;
				}
				// if any pillar is still alive
				for(octopus::Entity const *ent_l : state_p.getEntities())
				{
					if(&ent_l->_model == sData_l->pillar_model
					&& ent_l->_player == unit_l._player
					&& ent_l->_alive)
					{
						octopus::Fixed curHp_l = ent_l->_hp + step_p.getHpChange(ent_l->_handle);
						octopus::Fixed maxHp_l = ent_l->getHpMax();
						step_p.addSteppable(new FirstRunicBossPillarUp(ent_p._handle.index, ent_l->_handle.index, step_l));
					}
				}
			}
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
				unit_l.getDamageNoBonus() * (phase1_l?1:2),
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
				projectile_l._baseDamage = unit_l.getDamageNoBonus() * (phase1_l?1:2);
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
    lib_p.registerUnitModel("firstRunicBoss_wave", unitModel_l);

    octopus::UnitModel pillarModel_l { false, 0.5, 0, 150. };
    pillarModel_l._isUnit = true;
	pillarModel_l._projectile = true;
    pillarModel_l._damage = 2;
    pillarModel_l._armor = 2;
    pillarModel_l._range = 4;
    pillarModel_l._lineOfSight = 5;
    pillarModel_l._fullReload = 300;
    pillarModel_l._windup = 20;

    lib_p.registerUnitModel("firstRunicBoss_pillar", pillarModel_l);

	octopus::UnitModel firstRunicBossModel_l { false, 1.9, 0, 7500 };
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
	sData_l->pillar_model = &lib_p.getUnitModel("firstRunicBoss_pillar");
	firstRunicBossModel_l._staticUnitData = sData_l;

	lib_p.registerUnitModel("firstRunicBoss", firstRunicBossModel_l);

	octopus::UnitModel firstRunicAnomaly_l { false, 1.9, 0, 2000 };
	firstRunicAnomaly_l._projectile = true;
	firstRunicAnomaly_l._isUnit = true;
	firstRunicAnomaly_l._damage = 10;
	firstRunicAnomaly_l._armor = 2;
	firstRunicAnomaly_l._range = 10.;
	firstRunicAnomaly_l._lineOfSight = 50;
	firstRunicAnomaly_l._fullReload = 80;
	firstRunicAnomaly_l._windup = 20;
	firstRunicAnomaly_l._idleFunc = firstRunicBossRoutine;
	firstRunicAnomaly_l._unitData = new FirstRunicBossData();
	sData_l = new FirstRunicBossStaticData();
	sData_l->add_model = &lib_p.getUnitModel("firstRunicBoss_add");
	sData_l->pillar_model = &lib_p.getUnitModel("firstRunicBoss_pillar");
	sData_l->adds_enabled = false;
	sData_l->pillars_enabled = false;
	firstRunicAnomaly_l._staticUnitData = sData_l;

	lib_p.registerUnitModel("firstRunicAnomaly", firstRunicAnomaly_l);
}

} // namespace godot