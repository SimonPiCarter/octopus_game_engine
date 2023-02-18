#ifndef __STEPPABLE__
#define __STEPPABLE__

namespace octopus
{
	class State;
	class SteppableVisitor;

	/// @brief define an interface for
	/// apply and revert on a Steppable, they
	/// are contained in a Step object that
	/// represent a step from one state to another
	/// It is mandatory that Stappable apply and
	/// revert does the exact opposite actions
	/// must be serializable otherwise wont be
	/// replayable easily
	class Steppable
	{
		public:
			virtual ~Steppable() {}
			/// @brief apply this Steppable to the given state
			virtual void apply(State &state_p) const = 0;
			/// @brief revert this Steppable to the given state
			virtual void revert(State &state_p) const = 0;

			/// @brief return true if this Steppable does no operation on the State it would be applied
			virtual bool isNoOp() const = 0;

			/// @brief visitor method
			virtual void visit(SteppableVisitor * visitor_p) const = 0;
	};

	class BuildingSpawnStep;
	class BuildingStep;
	class BuildingCancelStep;
	class CommandAddSubAttackStep;
	class CommandDataWaypointAddStep;
	class CommandDataWaypointRemoveStep;
	class CommandDataWaypointSetStep;
	class CommandDelSubAttackStep;
	class CommandDepositChangeStep;
	class CommandHarvestingChangeStep;
	class CommandHarvestTimeSinceHarvestStep;
	class CommandIncrementNoProgressStep;
	class CommandMoveUpdateStep;
	class CommandMoveStepSinceUpdateIncrementStep;
	class CommandNewTargetStep;
	class CommandNextStep;
	class CommandResourceChangeStep;
	class CommandSetPositionFromStep;
	class CommandSpawnStep;
	class CommandStorageStep;
	class CommandUpdateLastPosStep;
	class CommandWindUpDiffStep;
	class EntityAttackStep;
	class EntityBuffStep;
	class EntityHitPointChangeStep;
	class EntityMoveStep;
	class EntityUpdateWaitingStep;
	class EntitySpawnStep;
	class MissingResourceStep;
	class PlayerAddBuildingModel;
	class PlayerAddOptionDivinityStep;
	class PlayerAnchorDivinityStep;
	class PlayerLevelUpDivinityStep;
	class PlayerResetOptionDivinityStep;
	class PlayerSpawnStep;
	class PlayerSpendResourceStep;
	class ProductionPaidStep;
	class ProductionProgressionStep;
	class ResourceSpawnStep;
	class TickingStep;
	class TriggerCountChange;
	class TriggerEnableChange;
	class TriggerSpawn;
	class TriggerStepCountChange;
	class UnitHarvestDropStep;
	class UnitHarvestQuantityStep;
	class UnitHarvestTypeStep;
	class UnitSpawnStep;

	/// @brief This is used to enable specific step comportement in
	/// libs using Octopus
	/// especially for diplay or saving purpose
	class SteppableVisitor
	{
		public:
			virtual ~SteppableVisitor() {}

			/// @brief untypped visiting method
			void operator()(Steppable const *steppable_p)
			{
				steppable_p->visit(this);
			}

			virtual void visit(BuildingSpawnStep const *steppable_p) = 0;
			virtual void visit(BuildingStep const *steppable_p) = 0;
			virtual void visit(BuildingCancelStep const *steppable_p) = 0;
			virtual void visit(CommandAddSubAttackStep const *steppable_p) = 0;
			virtual void visit(CommandDataWaypointAddStep const *steppable_p) = 0;
			virtual void visit(CommandDataWaypointRemoveStep const *steppable_p) = 0;
			virtual void visit(CommandDataWaypointSetStep const *steppable_p) = 0;
			virtual void visit(CommandDelSubAttackStep const *steppable_p) = 0;
			virtual void visit(CommandDepositChangeStep const *steppable_p) = 0;
			virtual void visit(CommandHarvestingChangeStep const *steppable_p) = 0;
			virtual void visit(CommandHarvestTimeSinceHarvestStep const *steppable_p) = 0;
			virtual void visit(CommandIncrementNoProgressStep const *steppable_p) = 0;
			virtual void visit(CommandMoveUpdateStep const *steppable_p) = 0;
			virtual void visit(CommandMoveStepSinceUpdateIncrementStep const *steppable_p) = 0;
			virtual void visit(CommandNewTargetStep const *steppable_p) = 0;
			virtual void visit(CommandNextStep const *steppable_p) = 0;
			virtual void visit(CommandResourceChangeStep const *steppable_p) = 0;
			virtual void visit(CommandSetPositionFromStep const *steppable_p) = 0;
			virtual void visit(CommandSpawnStep const *steppable_p) = 0;
			virtual void visit(CommandStorageStep const *steppable_p) = 0;
			virtual void visit(CommandUpdateLastPosStep const *steppable_p) = 0;
			virtual void visit(CommandWindUpDiffStep const *steppable_p) = 0;
			virtual void visit(EntityAttackStep const *steppable_p) = 0;
			virtual void visit(EntityBuffStep const *steppable_p) = 0;
			virtual void visit(EntityHitPointChangeStep const *steppable_p) = 0;
			virtual void visit(EntityMoveStep const *steppable_p) = 0;
			virtual void visit(EntityUpdateWaitingStep const *steppable_p) = 0;
			virtual void visit(EntitySpawnStep const *steppable_p) = 0;
			virtual void visit(MissingResourceStep const *steppable_p) = 0;
			virtual void visit(PlayerAddBuildingModel const *steppable_p) = 0;
			virtual void visit(PlayerAddOptionDivinityStep const *steppable_p) = 0;
			virtual void visit(PlayerAnchorDivinityStep const *steppable_p) = 0;
			virtual void visit(PlayerLevelUpDivinityStep const *steppable_p) = 0;
			virtual void visit(PlayerResetOptionDivinityStep const *steppable_p) = 0;
			virtual void visit(PlayerSpawnStep const *steppable_p) = 0;
			virtual void visit(PlayerSpendResourceStep const *steppable_p) = 0;
			virtual void visit(ProductionPaidStep const *steppable_p) = 0;
			virtual void visit(ProductionProgressionStep const *steppable_p) = 0;
			virtual void visit(ResourceSpawnStep const *steppable_p) = 0;
			virtual void visit(TickingStep const *steppable_p) = 0;
			virtual void visit(TriggerCountChange const *steppable_p) = 0;
			virtual void visit(TriggerEnableChange const *steppable_p) = 0;
			virtual void visit(TriggerSpawn const *steppable_p) = 0;
			virtual void visit(TriggerStepCountChange const *steppable_p) = 0;
			virtual void visit(UnitHarvestDropStep const *steppable_p) = 0;
			virtual void visit(UnitHarvestQuantityStep const *steppable_p) = 0;
			virtual void visit(UnitHarvestTypeStep const *steppable_p) = 0;
			virtual void visit(UnitSpawnStep const *steppable_p) = 0;
	};
}

#endif
