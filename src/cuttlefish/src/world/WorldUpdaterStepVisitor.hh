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
class World;
class Window;

/// @brief This class is used to maintain a representation of the world
/// based on state the engine sends
/// Each update checks for new Entity and remove non alive entities
class WorldUpdaterStepVisitor : public octopus::SteppableVisitor
{
public:
	WorldUpdaterStepVisitor(World &world_p, Window &window_p, octopus::State const *state_p)
		: _world(world_p), _window(window_p), _state(state_p) {}

	void clear(octopus::Handle const &handle_p);
	void spawn(octopus::Entity const &entity_p, octopus::Handle const &handle_p);

	virtual void visit(octopus::EntitySpawnStep const *step_p) override;
	virtual void visit(octopus::BuildingSpawnStep const *step_p) override;
	virtual void visit(octopus::ResourceSpawnStep const *step_p) override;
	virtual void visit(octopus::UnitSpawnStep const *step_p) override;
	virtual void visit(octopus::UnitHarvestQuantityStep const *step_p) override;
	virtual void visit(octopus::EntityHitPointChangeStep const *step_p) override;
	virtual void visit(octopus::EntityMoveStep const *step_p) override;

	virtual void visit(octopus::BuildingStep const *steppable_p) override {}
	virtual void visit(octopus::CommandAddSubAttackStep const *) override {}
	virtual void visit(octopus::CommandDataWaypointAddStep const *) override {}
	virtual void visit(octopus::CommandDataWaypointRemoveStep const *) override {}
	virtual void visit(octopus::CommandDataWaypointSetStep const *) override {}
	virtual void visit(octopus::CommandDelSubAttackStep const *) override {}
	virtual void visit(octopus::CommandDepositChangeStep const *) override {}
	virtual void visit(octopus::CommandHarvestingChangeStep const *) override {}
	virtual void visit(octopus::CommandIncrementNoProgressStep const *) override {}
	virtual void visit(octopus::CommandMoveUpdateStep const *) override {}
	virtual void visit(octopus::CommandNewTargetStep const *) override {}
	virtual void visit(octopus::CommandNextStep const *) override {}
	virtual void visit(octopus::CommandResourceChangeStep const *) override {}
	virtual void visit(octopus::CommandSetPositionFromStep const *) override {}
	virtual void visit(octopus::CommandSpawnStep const *) override {}
	virtual void visit(octopus::CommandUpdateLastPosStep const *) override {}
	virtual void visit(octopus::CommandWindUpDiffStep const *) override {}
	virtual void visit(octopus::EntityBuffStep const *) override {}
	virtual void visit(octopus::EntityAttackStep const *) override {}
	virtual void visit(octopus::PlayerSpawnStep const *) override {}
	virtual void visit(octopus::ProductionProgressionStep const *) override {}
	virtual void visit(octopus::TickingStep const *) override {}
	virtual void visit(octopus::UnitHarvestDropStep const *) override {}
	virtual void visit(octopus::UnitHarvestTypeStep const *) override {}

private:
	World &_world;
	Window &_window;
	octopus::State const * const _state;
};
} // namespace cuttlefish


#endif
