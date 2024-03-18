#include "ControllerStepVisitor.h"

#include "Controller.h"
#include "step/CameraStep.h"
#include "step/DialogStep.h"
#include "step/WaveStep.h"
#include "step/ObjectiveStep.h"
#include "step/library/FirstRunicBossStep.h"

// octopus
#include "command/data/AttackMoveData.hh"
#include "state/entity/Building.hh"
#include "state/entity/Entity.hh"
#include "state/entity/Resource.hh"
#include "state/entity/Unit.hh"
#include "state/State.hh"
#include "step/building/BuildingCancelStep.hh"
#include "step/building/BuildingRemoveRallyPointStep.hh"
#include "step/building/BuildingSetRallyPointStep.hh"
#include "step/building/BuildingStep.hh"
#include "step/command/harvest/CommandHarvestStep.hh"
#include "step/command/CommandWindUpDiffStep.hh"
#include "step/command/MissingResourceStep.hh"
#include "step/custom/implem/ImpactStep.hh"
#include "step/custom/implem/WindUpStartStep.hh"
#include "step/entity/EntityHitPointChangeStep.hh"
#include "step/entity/EntityMoveStep.hh"
#include "step/entity/EntityUpdateReloadAbilityStep.hh"
#include "step/entity/buff/EntityBuffStep.hh"
#include "step/entity/spawn/BuildingSpawnStep.hh"
#include "step/entity/spawn/EntitySpawnStep.hh"
#include "step/entity/spawn/ResourceSpawnStep.hh"
#include "step/entity/spawn/UnitSpawnStep.hh"
#include "step/player/PlayerAddOptionStep.hh"
#include "step/player/PlayerPopOptionStep.hh"
#include "step/player/PlayerBuffAllStep.hh"
#include "step/projectile/ProjectileMoveStep.hh"
#include "step/projectile/ProjectileSpawnStep.hh"
#include "step/state/StateAddConstraintPositionStep.hh"
#include "step/state/StateRemoveConstraintPositionStep.hh"
#include "step/state/StateWinStep.hh"
#include "step/unit/UnitHarvestStep.hh"

#include "controller/step/RuneWellPopStep.h"

using octopus::to_double;

namespace godot
{

void ControllerStepVisitor::visit(octopus::EntitySpawnStep const *steppable_p)
{
	_controller.spawn(steppable_p->getHandle());
}

void ControllerStepVisitor::visit(octopus::BuildingSpawnStep const *steppable_p)
{
	_controller.spawn(steppable_p->getHandle());
}

void ControllerStepVisitor::visit(octopus::ResourceSpawnStep const *steppable_p)
{
	_controller.spawn(steppable_p->getHandle());
}

void ControllerStepVisitor::visit(octopus::StateAddConstraintPositionStep const *steppable_p)
{
	_controller.emit_signal("spawn_blockers",
		octopus::to_double(steppable_p->_val),
		octopus::to_double(steppable_p->_min),
		octopus::to_double(steppable_p->_max),
		steppable_p->_less,
		steppable_p->_x
	);
}

void ControllerStepVisitor::visit(octopus::StateRemoveConstraintPositionStep const *steppable_p)
{
	_controller.emit_signal("clear_blockers",
		octopus::to_double(steppable_p->_val),
		octopus::to_double(steppable_p->_min),
		octopus::to_double(steppable_p->_max),
		steppable_p->_less,
		steppable_p->_x
	);
}

void ControllerStepVisitor::visit(octopus::StateWinStep const *steppable_p)
{
	_controller.emit_signal("over", int(steppable_p->_winningTeam));
}

void ControllerStepVisitor::visit(octopus::UnitSpawnStep const *steppable_p)
{
	_controller.spawn(steppable_p->getHandle());
}

void ControllerStepVisitor::visit(octopus::BuildingCancelStep const *steppable_p)
{
	_controller.emit_signal("clear_entity", int(steppable_p->_handle.index));
}

void ControllerStepVisitor::visit(octopus::BuildingRemoveRallyPointStep const *steppable_p)
{
	_controller.emit_signal("remove_rally_point", int(steppable_p->_handle.index));
}

void ControllerStepVisitor::visit(octopus::BuildingSetRallyPointStep const *steppable_p)
{
	if(!steppable_p->_rallyPointEntityActive)
	{
		_controller.emit_signal("set_rally_point", int(steppable_p->_handle.index), Vector2(octopus::to_double(steppable_p->_rallyPoint.x), octopus::to_double(steppable_p->_rallyPoint.y)));
	}
	else
	{
		_controller.emit_signal("set_rally_point_target", int(steppable_p->_handle.index), int(steppable_p->_rallyPointEntity.index));
	}
}

void ControllerStepVisitor::visit(octopus::UnitHarvestDropStep const *steppable_p)
{
	octopus::Entity const * ent_l = _state->getEntity(steppable_p->_handle);
	octopus::Unit const * unit_l = dynamic_cast<octopus::Unit const *>(ent_l);

	_controller.emit_signal("harvest_drop",
		int(ent_l->_handle.index),
		int(unit_l->_player),
		octopus::to_double(steppable_p->_dropped),
		String(unit_l->_typeOfResource.c_str()));
}

void ControllerStepVisitor::visit(octopus::UnitHarvestQuantityStep const *steppable_p)
{
	_controller.emit_signal("harvest_unit", int(steppable_p->_handle.index));
	if(static_cast<octopus::Resource const *>(_state->getEntity(steppable_p->_res))->_resource <= 0.)
	{
		_controller.emit_signal("clear_entity", int(steppable_p->_res.index));
	}
}


void ControllerStepVisitor::visit(octopus::CommandHarvestTimeSinceHarvestStep const *steppable_p)
{
	// NA (ok)
}

void ControllerStepVisitor::visit(octopus::EntityHitPointChangeStep const *steppable_p)
{
	octopus::Entity const * ent_l = _state->getEntity(steppable_p->_handle);
	double hp_l = octopus::to_double(ent_l->_hp);
	// warning when we have deleted it already (can happens when multiple change step in one step)
	_controller.hp_change(steppable_p->_handle.index, octopus::to_double(ent_l->_hp/ent_l->getHpMax()), octopus::to_double(steppable_p->_delta));
	if(!ent_l->_alive)
	{
		_controller.kill(steppable_p->_handle);
		if(ent_l->_model._isBuilding)
		{
			_controller.emit_signal("updated_requirements");
		}
	}
}

void ControllerStepVisitor::visit(octopus::PlayerLevelUpUpgradeStep const *)
{
	_controller.emit_signal("updated_requirements");
}

void ControllerStepVisitor::visit(octopus::ProjectileMoveStep const *steppable_p)
{
	for(uint32_t i = 0 ; i < steppable_p->getMove().size() ; ++ i)
	{
		if(!octopus::is_zero(steppable_p->getMove()[i]))
		{
			octopus::Projectile const & proj_l = _state->getProjectileContainer().getProjectiles()[i];
			_controller.emit_signal("move_projectile", int(i), Vector2(octopus::to_double(proj_l._pos.x), octopus::to_double(proj_l._pos.y)));
		}
	}

	for(uint32_t i = 0 ; i < steppable_p->getOver().size() ; ++ i)
	{
		if(steppable_p->getOver()[i])
		{
			_controller.emit_signal("end_projectile", int(i));
		}
	}
}

void ControllerStepVisitor::visit(octopus::ProjectileSpawnStep const *steppable_p)
{
	for(octopus::Projectile const &proj_l : steppable_p->getToBeSpawned())
	{
		if(proj_l._sourceModel)
		{
			_controller.emit_signal("spawn_projectile", int(proj_l._index), String(proj_l._sourceModel->_id.c_str()),
				Vector2(octopus::to_double(proj_l._pos.x), octopus::to_double(proj_l._pos.y)),
				Vector2(octopus::to_double(proj_l._posTarget.x), octopus::to_double(proj_l._posTarget.y)),
				int(proj_l._target.index));
		}
	}
}

void ControllerStepVisitor::visit(octopus::EntityMoveStep const *steppable_p)
{
	if(!_skipMoveSteps)
	{
		_controller.move(steppable_p->_handle);
	}
}

void ControllerStepVisitor::visit(octopus::BuildingStep const *steppable_p)
{
	octopus::Entity const * ent_l = _state->getEntity(steppable_p->_handle);
	octopus::Building const * building_l = static_cast<octopus::Building const *>(ent_l);
	_controller.emit_signal("build", int(steppable_p->_source.index), int(steppable_p->_handle.index),
		octopus::to_double(building_l->_buildingProgress/building_l->_buildingModel._buildingTime));
	if(building_l->isBuilt())
	{
		_controller.emit_signal("updated_requirements");
	}
}

void ControllerStepVisitor::visit(octopus::PlayerAddOptionStep const *steppable_p)
{
	_controller.getOptionManagers().at(steppable_p->_player)->addOptionLayer(*_state, steppable_p);
	_controller.emit_signal("option_update");
}

void ControllerStepVisitor::visit(octopus::EntityUpdateReloadAbilityStep const *steppable_p)
{
	if(steppable_p->_new == 0)
	{
		_controller.emit_signal("ability_used", int(steppable_p->_handle.index), String(steppable_p->_key.c_str()));
	}
}

void ControllerStepVisitor::visit(octopus::MissingResourceStep const *steppable_p)
{
	_controller.emit_signal("missing_resource", int(steppable_p->_player), String(steppable_p->_resource.c_str()));
}

void ControllerStepVisitor::visit(octopus::PlayerBuffAllStep const *steppable_p)
{
	_controller.emit_signal("buff_all", int(steppable_p->_player), String(steppable_p->_buff._id.c_str()), String(steppable_p->_model.c_str()));
}

void ControllerStepVisitor::visit(octopus::PlayerPopOptionStep const *steppable_p)
{
	_controller.getOptionManagers().at(steppable_p->_player)->popOptionLayer(steppable_p);
	_controller.emit_signal("option_update");
	_controller.emit_signal("pop_option");
}

void ControllerStepVisitor::visit(octopus::CustomStep const *steppable_p)
{
	octopus::ImpactStep const *impact_l = dynamic_cast<octopus::ImpactStep const *>(steppable_p);
	octopus::WindUpStartStep const *windupStart_l = dynamic_cast<octopus::WindUpStartStep const *>(steppable_p);
	DialogStep const *dialog_l = dynamic_cast<DialogStep const *>(steppable_p);
	CameraStep const *camera_l = dynamic_cast<CameraStep const *>(steppable_p);
	WaveStep const *wave_l = dynamic_cast<WaveStep const *>(steppable_p);
	WaveSpawPointStep const *waveSpawnPoint_l = dynamic_cast<WaveSpawPointStep const *>(steppable_p);
	RuneWellPopStep const *runeWellPopStep_l = dynamic_cast<RuneWellPopStep const *>(steppable_p);
	FirstRunicBossStep const *firstRunicBossStep_l = dynamic_cast<FirstRunicBossStep const *>(steppable_p);
	FirstRunicBossPillar const *firstRunicBossPillar_l = dynamic_cast<FirstRunicBossPillar const *>(steppable_p);
	FirstRunicBossPillarUp const *firstRunicBossPillarUp_l = dynamic_cast<FirstRunicBossPillarUp const *>(steppable_p);


	// objective handling
	AbstractObjectiveStep const * objectiveStep_l = dynamic_cast<AbstractObjectiveStep const *>(steppable_p);

	if(impact_l)
	{
		_controller.emit_signal("impact", String(impact_l->_model.c_str()), octopus::to_double(impact_l->_pos.x), octopus::to_double(impact_l->_pos.y));
	}
	else if(windupStart_l)
	{
		_controller.windup(windupStart_l->_handle.index);
	}
	else if(camera_l)
	{
		_controller.emit_signal("set_camera", camera_l->_x, camera_l->_y, int(camera_l->_player));
	}
	else if(dialog_l)
	{
		_controller.emit_signal("spawn_dialog", String(dialog_l->_dialog.c_str()), dialog_l->_over);
	}
	else if(wave_l)
	{
		_controller.emit_signal("wave");
	}
	else if(waveSpawnPoint_l)
	{
		for(octopus::Vector const &point_l : waveSpawnPoint_l->getPoints())
		{
			_controller.emit_signal("wave_spawn_point", (int)point_l.x.to_double(), (int)point_l.y.to_double());
		}
	}
	else if(runeWellPopStep_l)
	{
		_controller.emit_signal("rune_well_pop_step", (int)runeWellPopStep_l->_wellIdx, (int)runeWellPopStep_l->_player);
	}
	else if(objectiveStep_l)
	{
		objectiveStep_l->add_signals(_controller);
	}
	else if(firstRunicBossStep_l)
	{
		if(firstRunicBossStep_l->_trigger)
		{
			_controller.emit_signal("first_runic_boss_trigger_aoe", firstRunicBossStep_l->_id, firstRunicBossStep_l->_idAoe);
		}
		else
		{
			_controller.emit_signal("first_runic_boss_spawn_aoe",
				firstRunicBossStep_l->_id, firstRunicBossStep_l->_idAoe,
				firstRunicBossStep_l->_x, firstRunicBossStep_l->_y, firstRunicBossStep_l->_range);
		}
	}
	else if(firstRunicBossPillar_l)
	{
		_controller.emit_signal("first_runic_boss_pillar",
				firstRunicBossPillar_l->_id, firstRunicBossPillar_l->_spawn, firstRunicBossPillar_l->_firstSpawn);
	}
	else if(firstRunicBossPillarUp_l)
	{
		_controller.emit_signal("first_runic_boss_pillar_up",
				firstRunicBossPillarUp_l->_id, firstRunicBossPillarUp_l->_idPillar, firstRunicBossPillarUp_l->_step);
	}
}

void ControllerStepVisitor::visit(octopus::EntityBuffStep const *steppable_p)
{
		_controller.emit_signal("buff", int(steppable_p->_target.index), String(steppable_p->_buff._id.c_str()), int(steppable_p->_buff._duration));
}

void applyControllerStepVisitor(Controller &controller_p, octopus::State const &state_p, octopus::Step const &step_p)
{
	ControllerStepVisitor vis_l(controller_p, &state_p, step_p.getId()+1 != state_p.getStepApplied());
	// visit intial steps
	for(octopus::Steppable const * steppable_l : step_p.getSteppable())
	{
		vis_l(steppable_l);
	}
	vis_l(&step_p.getProjectileSpawnStep());
	vis_l(&step_p.getProjectileMoveStep());
}


} // godot
