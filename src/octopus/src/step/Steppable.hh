#ifndef __STEPPABLE__
#define __STEPPABLE__

namespace octopus
{
	class State;
	class SteppableVisitor;

	/// @brief empty class to define a steppable data
	struct SteppableData {
		virtual ~SteppableData() {}
	};

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
			virtual void apply(State &state_p, SteppableData *data_p) const = 0;
			/// @brief revert this Steppable to the given state
			virtual void revert(State &state_p, SteppableData *data_p) const = 0;

			/// @brief return true if this Steppable does no operation on the State it would be applied
			virtual bool isNoOp() const = 0;

			/// @brief visitor method
			virtual void visit(SteppableVisitor * visitor_p) const = 0;

			/// @brief create an optional new SteppableData
			/// @return nullptr if not required
			virtual SteppableData * newData() const { return nullptr; }
	};

	class BuildingCancelStep;
	class BuildingSpawnStep;
	class BuildingStep;
	class CancelUnitProductionStep;
	class CommandAddSubAttackStep;
	class CommandDataWaypointAddStep;
	class CommandDataWaypointRemoveStep;
	class CommandDataWaypointSetStep;
	class CommandDelSubAttackStep;
	class CommandDepositChangeStep;
	class CommandHarvestingChangeStep;
	class CommandHarvestTimeSinceHarvestStep;
	class CommandIncrementNoProgressStep;
	class CommandMoveStepSinceUpdateIncrementStep;
	class CommandMoveUpdateStep;
	class CommandNewTargetStep;
	class CommandNextStep;
	class CommandResourceChangeStep;
	class CommandSetPositionFromStep;
	class CommandSpawnStep;
	class CommandStorageStep;
	class CommandUpdateLastIdStep;
	class CommandUpdateLastPosStep;
	class CommandWindUpDiffStep;
	class CustomStep;
	class EntityAttackStep;
	class EntityBuffStep;
	class EntityFrozenStep;
	class EntityHitPointChangeStep;
	class EntityMoveStep;
	class EntitySpawnStep;
	class EntityUpdateWaitingStep;
	class FlyingCommandPopStep;
	class FlyingCommandSpawnStep;
	class MissingResourceStep;
	class PlayerAddBuildingModel;
	class PlayerAddOptionDivinityStep;
	class PlayerAddOptionStep;
	class PlayerAnchorDivinityStep;
	class PlayerBuffAllStep;
	class PlayerLevelUpDivinityStep;
	class PlayerPopOptionStep;
	class PlayerResetOptionDivinityStep;
	class PlayerSpawnStep;
	class PlayerSpendResourceStep;
	class ProductionPaidStep;
	class ProductionProgressionStep;
	class ResourceSpawnStep;
	class StateDrawStep;
	class StateTemplePositionAddStep;
	class StateTemplePositionRemoveStep;
	class StateWinStep;
	class TickingStep;
	class TimerDataUpdateStep;
	class TriggerCountChange;
	class TriggerEnableChange;
	class TriggerEntityAddStep;
	class TriggerEntityResetStep;
	class TriggerSpawn;
	class TriggerStepCountChange;
	class UnitHarvestDropStep;
	class UnitHarvestQuantityStep;
	class UnitHarvestTypeStep;
	class UnitSpawnStep;
	class VisionChangeStep;

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

			virtual void visit(BuildingCancelStep const *steppable_p) = 0;
			virtual void visit(BuildingSpawnStep const *steppable_p) = 0;
			virtual void visit(BuildingStep const *steppable_p) = 0;
    		virtual void visit(CancelUnitProductionStep const *) = 0;
			virtual void visit(CommandAddSubAttackStep const *steppable_p) = 0;
			virtual void visit(CommandDataWaypointAddStep const *steppable_p) = 0;
			virtual void visit(CommandDataWaypointRemoveStep const *steppable_p) = 0;
			virtual void visit(CommandDataWaypointSetStep const *steppable_p) = 0;
			virtual void visit(CommandDelSubAttackStep const *steppable_p) = 0;
			virtual void visit(CommandDepositChangeStep const *steppable_p) = 0;
			virtual void visit(CommandHarvestingChangeStep const *steppable_p) = 0;
			virtual void visit(CommandHarvestTimeSinceHarvestStep const *steppable_p) = 0;
			virtual void visit(CommandIncrementNoProgressStep const *steppable_p) = 0;
			virtual void visit(CommandMoveStepSinceUpdateIncrementStep const *steppable_p) = 0;
			virtual void visit(CommandMoveUpdateStep const *steppable_p) = 0;
			virtual void visit(CommandNewTargetStep const *steppable_p) = 0;
			virtual void visit(CommandNextStep const *steppable_p) = 0;
			virtual void visit(CommandResourceChangeStep const *steppable_p) = 0;
			virtual void visit(CommandSetPositionFromStep const *steppable_p) = 0;
			virtual void visit(CommandSpawnStep const *steppable_p) = 0;
			virtual void visit(CommandStorageStep const *steppable_p) = 0;
			virtual void visit(CommandUpdateLastIdStep const *) = 0;
			virtual void visit(CommandUpdateLastPosStep const *steppable_p) = 0;
			virtual void visit(CommandWindUpDiffStep const *steppable_p) = 0;
			virtual void visit(CustomStep const *steppable_p) {}
			virtual void visit(EntityAttackStep const *steppable_p) = 0;
			virtual void visit(EntityBuffStep const *steppable_p) = 0;
			virtual void visit(EntityFrozenStep const *steppable_p) = 0;
			virtual void visit(EntityHitPointChangeStep const *steppable_p) = 0;
			virtual void visit(EntityMoveStep const *steppable_p) = 0;
			virtual void visit(EntitySpawnStep const *steppable_p) = 0;
			virtual void visit(FlyingCommandSpawnStep const *steppable_p) = 0;
			virtual void visit(FlyingCommandPopStep const *steppable_p) = 0;
			virtual void visit(EntityUpdateWaitingStep const *steppable_p) = 0;
			virtual void visit(MissingResourceStep const *steppable_p) = 0;
			virtual void visit(PlayerAddBuildingModel const *steppable_p) = 0;
			virtual void visit(PlayerAddOptionDivinityStep const *steppable_p) = 0;
			virtual void visit(PlayerAddOptionStep const *steppable_p) = 0;
			virtual void visit(PlayerAnchorDivinityStep const *steppable_p) = 0;
			virtual void visit(PlayerBuffAllStep const *steppable_p) = 0;
			virtual void visit(PlayerLevelUpDivinityStep const *steppable_p) = 0;
			virtual void visit(PlayerPopOptionStep const *steppable_p) = 0;
			virtual void visit(PlayerResetOptionDivinityStep const *steppable_p) = 0;
			virtual void visit(PlayerSpawnStep const *steppable_p) = 0;
			virtual void visit(PlayerSpendResourceStep const *steppable_p) = 0;
			virtual void visit(ProductionPaidStep const *steppable_p) = 0;
			virtual void visit(ProductionProgressionStep const *steppable_p) = 0;
			virtual void visit(ResourceSpawnStep const *steppable_p) = 0;
			virtual void visit(StateDrawStep const *steppable_p) = 0;
			virtual void visit(StateTemplePositionAddStep const *steppable_p) = 0;
			virtual void visit(StateTemplePositionRemoveStep const *steppable_p) = 0;
			virtual void visit(StateWinStep const *steppable_p) = 0;
			virtual void visit(TickingStep const *steppable_p) = 0;
			virtual void visit(TimerDataUpdateStep const *steppable_p) = 0;
			virtual void visit(TriggerCountChange const *steppable_p) = 0;
			virtual void visit(TriggerEnableChange const *steppable_p) = 0;
			virtual void visit(TriggerEntityAddStep const *steppable_p) = 0;
			virtual void visit(TriggerEntityResetStep const *steppable_p) = 0;
			virtual void visit(TriggerSpawn const *steppable_p) = 0;
			virtual void visit(TriggerStepCountChange const *steppable_p) = 0;
			virtual void visit(UnitHarvestDropStep const *steppable_p) = 0;
			virtual void visit(UnitHarvestQuantityStep const *steppable_p) = 0;
			virtual void visit(UnitHarvestTypeStep const *steppable_p) = 0;
			virtual void visit(UnitSpawnStep const *steppable_p) = 0;
			virtual void visit(VisionChangeStep const *steppable_p) = 0;
	};
}

#endif
