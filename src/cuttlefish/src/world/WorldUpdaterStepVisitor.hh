#ifndef __WorldUpdaterStepVisitor__
#define __WorldUpdaterStepVisitor__

// octopus
#include "step/Steppable.hh"
#include "state/Handle.hh"

namespace octopus
{
	class Entity;
	class State;
} // namespace octopus


namespace cuttlefish
{
class DivinityPanel;
class Panel;
class SpriteLibrary;
class StatsPanel;
class Window;
class World;

/// @brief This class is used to maintain a representation of the world
/// based on state the engine sends
/// Each update checks for new Entity and remove non alive entities
class WorldUpdaterStepVisitor : public octopus::SteppableVisitor
{
public:
	WorldUpdaterStepVisitor(World &world_p, Window &window_p, Panel &panel_p, StatsPanel &statsPanel_p, DivinityPanel &divPanel_p,
		octopus::State const *state_p, SpriteLibrary const &lib_p)
		: _world(world_p), _window(window_p), _panel(panel_p), _statsPanel(statsPanel_p), _divPanel(divPanel_p), _state(state_p), _lib(lib_p) {}

	void clear(octopus::Handle const &handle_p);
	void spawn(octopus::Handle const &handle_p);

	virtual void visit(octopus::BuildingCancelStep const *) override;
	virtual void visit(octopus::BuildingSpawnStep const *step_p) override;
	virtual void visit(octopus::BuildingStep const *steppable_p) override;
	virtual void visit(octopus::CommandAddSubAttackStep const *) override {}
	virtual void visit(octopus::CommandDataWaypointAddStep const *) override {}
	virtual void visit(octopus::CommandDataWaypointRemoveStep const *) override {}
	virtual void visit(octopus::CommandDataWaypointSetStep const *) override {}
	virtual void visit(octopus::CommandDelSubAttackStep const *) override {}
	virtual void visit(octopus::CommandDepositChangeStep const *) override {}
	virtual void visit(octopus::CommandHarvestingChangeStep const *) override {}
	virtual void visit(octopus::CommandHarvestTimeSinceHarvestStep const *) override;
	virtual void visit(octopus::CommandIncrementNoProgressStep const *) override {}
	virtual void visit(octopus::CommandMoveStepSinceUpdateIncrementStep const *) {}
	virtual void visit(octopus::CommandMoveUpdateStep const *) override {}
	virtual void visit(octopus::CommandNewTargetStep const *) override {}
	virtual void visit(octopus::CommandNextStep const *) override {}
	virtual void visit(octopus::CommandResourceChangeStep const *) override {}
	virtual void visit(octopus::CommandSetPositionFromStep const *) override {}
	virtual void visit(octopus::CommandSpawnStep const *) override {}
	virtual void visit(octopus::CommandStorageStep const *) override {}
	virtual void visit(octopus::CommandUpdateLastIdStep const *) override {}
	virtual void visit(octopus::CommandUpdateLastPosStep const *) override {}
	virtual void visit(octopus::CommandWindUpDiffStep const *) override;
	virtual void visit(octopus::EntityAttackStep const *) override {}
	virtual void visit(octopus::EntityBuffStep const *) override {}
	virtual void visit(octopus::EntityFrozenStep const *) {}
	virtual void visit(octopus::EntityHitPointChangeStep const *step_p) override;
	virtual void visit(octopus::EntityMoveStep const *step_p) override;
	virtual void visit(octopus::EntitySpawnStep const *step_p) override;
	virtual void visit(octopus::EntityUpdateWaitingStep const *) override {}
	virtual void visit(octopus::MissingResourceStep const *) override {}
	virtual void visit(octopus::PlayerAddBuildingModel const *) override {}
	virtual void visit(octopus::PlayerAddOptionDivinityStep const *) override;
	virtual void visit(octopus::PlayerAddOptionStep const *) {}
	virtual void visit(octopus::PlayerAnchorDivinityStep const *) override {}
	virtual void visit(octopus::PlayerBuffAllStep const *) {}
	virtual void visit(octopus::PlayerLevelUpDivinityStep const *) override {}
	virtual void visit(octopus::PlayerPopOptionStep const *) {}
	virtual void visit(octopus::PlayerResetOptionDivinityStep const *) override {}
	virtual void visit(octopus::PlayerSpawnStep const *) override {}
	virtual void visit(octopus::PlayerSpendResourceStep const *) override {}
	virtual void visit(octopus::ProductionPaidStep const *) override {}
	virtual void visit(octopus::ProductionProgressionStep const *) override {}
	virtual void visit(octopus::ResourceSpawnStep const *step_p) override;
	virtual void visit(octopus::StateDrawStep const *) override {}
	virtual void visit(octopus::StateTemplePositionAddStep const *) override {}
	virtual void visit(octopus::StateTemplePositionRemoveStep const *) override {}
	virtual void visit(octopus::StateWinStep const *) override {}
	virtual void visit(octopus::TickingStep const *) override {}
	virtual void visit(octopus::TriggerCountChange const *) override {}
	virtual void visit(octopus::TriggerEnableChange const *) override {}
	virtual void visit(octopus::TriggerEntityAddStep const *) override {}
	virtual void visit(octopus::TriggerEntityResetStep const *) override {}
	virtual void visit(octopus::TriggerSpawn const *) override {}
	virtual void visit(octopus::TriggerStepCountChange const *) override {}
	virtual void visit(octopus::UnitHarvestDropStep const *) override {}
	virtual void visit(octopus::UnitHarvestQuantityStep const *step_p) override;
	virtual void visit(octopus::UnitHarvestTypeStep const *) override {}
	virtual void visit(octopus::UnitSpawnStep const *step_p) override;

private:
	World &_world;
	Window &_window;
	Panel &_panel;
	StatsPanel &_statsPanel;
	DivinityPanel &_divPanel;
	octopus::State const * const _state;
	SpriteLibrary const &_lib;
};
} // namespace cuttlefish


#endif
