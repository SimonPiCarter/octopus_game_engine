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
class OptionPanel;
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
	WorldUpdaterStepVisitor(World &world_p, Window &window_p, Panel &panel_p, StatsPanel &statsPanel_p, OptionPanel &optionPanel_p,
		octopus::State const *state_p, SpriteLibrary const &lib_p, double &x, double &y)
		: _world(world_p), _window(window_p), _panel(panel_p), _statsPanel(statsPanel_p), _optPanel(optionPanel_p), _state(state_p), _lib(lib_p), _x(x), _y(y) {}

	void clear(octopus::Handle const &handle_p);
	void spawn(octopus::Handle const &handle_p);

	virtual void visit(octopus::AttackModifierStep const *) override {}
	virtual void visit(octopus::BuildingCancelStep const *) override;
	virtual void visit(octopus::BuildingRemoveRallyPointStep const *) override {}
	virtual void visit(octopus::BuildingSetRallyPointStep const *) override {}
	virtual void visit(octopus::BuildingSpawnStep const *step_p) override;
	virtual void visit(octopus::BuildingStep const *steppable_p) override;
	virtual void visit(octopus::CancelUnitProductionStep const *) override {}
	virtual void visit(octopus::CommandAddSubCommandStep const *) override {}
	virtual void visit(octopus::CommandDataWaypointAddStep const *) override {}
	virtual void visit(octopus::CommandDataWaypointRemoveStep const *) override {}
	virtual void visit(octopus::CommandDataWaypointSetStep const *) override {}
	virtual void visit(octopus::CommandDelSubCommandStep const *) override {}
	virtual void visit(octopus::CommandDepositChangeStep const *) override {}
	virtual void visit(octopus::CommandHarvestingChangeStep const *) override {}
	virtual void visit(octopus::CommandHarvestTimeSinceHarvestStep const *) override;
	virtual void visit(octopus::CommandIncrementNoProgressStep const *) override {}
	virtual void visit(octopus::CommandMoveLosStep const *) {}
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
	virtual void visit(octopus::CustomStep const *) override;
	virtual void visit(octopus::EntityAttackStep const *) override {}
	virtual void visit(octopus::EntityBuffStep const *) override {}
	virtual void visit(octopus::EntityConditionalBuffStep const *) override {}
	virtual void visit(octopus::EntityFrozenStep const *) {}
	virtual void visit(octopus::EntityHitPointChangeStep const *step_p) override;
	virtual void visit(octopus::EntityMoveStep const *step_p) override;
	virtual void visit(octopus::EntitySpawnStep const *step_p) override;
	virtual void visit(octopus::EntityUpdateWaitingStep const *) override {}
	virtual void visit(octopus::FlyingCommandSpawnStep const *) override {}
	virtual void visit(octopus::FlyingCommandPopStep const *) override {}
	virtual void visit(octopus::MissingResourceStep const *) override {}
	virtual void visit(octopus::PlayerAddBuildingModel const *) override {}
	virtual void visit(octopus::PlayerAddOptionDivinityStep const *) override;
	virtual void visit(octopus::PlayerAddCostBonusStep const *) override {}
	virtual void visit(octopus::PlayerAddOptionStep const *) override {}
	virtual void visit(octopus::PlayerAddTimeProdBonusStep const *) override {}
    virtual void visit(octopus::PlayerAttackModAllStep const *) override {}
	virtual void visit(octopus::PlayerAnchorDivinityStep const *) override {}
	virtual void visit(octopus::PlayerBuffAllStep const *) {}
	virtual void visit(octopus::PlayerConditionalBuffAllStep const *) {}
	virtual void visit(octopus::PlayerLevelUpDivinityStep const *) override {}
	virtual void visit(octopus::PlayerPopOptionStep const *);
	virtual void visit(octopus::PlayerResetOptionDivinityStep const *) override {}
	virtual void visit(octopus::PlayerSpawnStep const *) override {}
	virtual void visit(octopus::PlayerSpendResourceStep const *) override {}
	virtual void visit(octopus::ProductionPaidStep const *) override {}
	virtual void visit(octopus::ProductionProgressionStep const *) override {}
	virtual void visit(octopus::ResourceSpawnStep const *step_p) override;
	virtual void visit(octopus::StateAddConstraintPositionStep const *) override {}
	virtual void visit(octopus::StateDrawStep const *) override {}
	virtual void visit(octopus::StateFreeHandleStep const *) override {}
	virtual void visit(octopus::StateRemoveConstraintPositionStep const *) override {}
	virtual void visit(octopus::StateTemplePositionAddStep const *) override {}
	virtual void visit(octopus::StateTemplePositionRemoveStep const *) override {}
	virtual void visit(octopus::StateWinStep const *) override {}
	virtual void visit(octopus::TeamVisionStep const *) {}
	virtual void visit(octopus::TickingStep const *) override {}
	virtual void visit(octopus::TimerDataUpdateStep const *) override {}
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
	virtual void visit(octopus::VisionChangeStep const *) override {}

private:
	World &_world;
	Window &_window;
	Panel &_panel;
	StatsPanel &_statsPanel;
	OptionPanel &_optPanel;
	octopus::State const * const _state;
	SpriteLibrary const &_lib;
	/// @brief camera position
	double &_x;
	double &_y;
};
} // namespace cuttlefish


#endif
