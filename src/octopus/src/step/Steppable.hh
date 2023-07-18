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
			virtual void apply(State &state_p) const = 0;
			/// @brief revert this Steppable to the given state
			virtual void revert(State &state_p, SteppableData const *) const = 0;

			/// @brief return true if this Steppable does no operation on the State it would be applied
			virtual bool isNoOp() const = 0;

			/// @brief visitor method
			virtual void visit(SteppableVisitor * visitor_p) const = 0;

			/// @brief create an optional new SteppableData
			/// @return nullptr if not required
			virtual SteppableData * newData(State const &) const { return nullptr; }
	};

	class AttackModifierStep;
	class BuildingCancelStep;
	class BuildingRemoveRallyPointStep;
	class BuildingSetRallyPointStep;
	class BuildingSpawnStep;
	class BuildingStep;
	class CancelUnitProductionStep;
	class CommandAddSubCommandStep;
	class CommandDataWaypointAddStep;
	class CommandDataWaypointRemoveStep;
	class CommandDataWaypointSetStep;
	class CommandDelSubCommandStep;
	class CommandDepositChangeStep;
	class CommandHarvestingChangeStep;
	class CommandHarvestTimeSinceHarvestStep;
	class CommandIncrementNoProgressStep;
	class CommandMoveLosStep;
	class CommandUpdateFlockingReached;
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
	class EntityConditionalBuffStep;
	class EntityFrozenStep;
	class EntityHitPointChangeStep;
	class EntityMoveStep;
	class EntitySpawnStep;
	class EntityUpdateReloadAbilityStep;
	class EntityUpdateWaitingStep;
	class FlyingCommandPopStep;
	class FlyingCommandSpawnStep;
	class MissingResourceStep;
	class PlayerAddBuildingModel;
	class PlayerAddCostBonusStep;
	class PlayerAddOptionStep;
	class PlayerAddTimeProdBonusStep;
	class PlayerAttackModAllStep;
	class PlayerBuffAllStep;
	class PlayerConditionalBuffAllStep;
	class PlayerLevelUpUpgradeStep;
	class PlayerPopOptionStep;
	class PlayerProducedUpgradeStep;
	class PlayerSpawnStep;
	class PlayerSpendResourceStep;
	class PlayerUpdateBuildingCountStep;
	class ProductionPaidStep;
	class ProductionProgressionStep;
	class ResourceSpawnStep;
	class StateAddConstraintPositionStep;
	class StateDrawStep;
	class StateFreeHandleStep;
	class StateRemoveConstraintPositionStep;
	class StateTemplePositionAddStep;
	class StateTemplePositionRemoveStep;
	class StateWinStep;
	class TeamVisionStep;
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

			virtual void visit(AttackModifierStep const *) = 0;
			virtual void visit(BuildingCancelStep const *) = 0;
			virtual void visit(BuildingRemoveRallyPointStep const *) = 0;
			virtual void visit(BuildingSetRallyPointStep const *) = 0;
			virtual void visit(BuildingSpawnStep const *) = 0;
			virtual void visit(BuildingStep const *) = 0;
    		virtual void visit(CancelUnitProductionStep const *) = 0;
			virtual void visit(CommandAddSubCommandStep const *) = 0;
			virtual void visit(CommandDataWaypointAddStep const *) = 0;
			virtual void visit(CommandDataWaypointRemoveStep const *) = 0;
			virtual void visit(CommandDataWaypointSetStep const *) = 0;
			virtual void visit(CommandDelSubCommandStep const *) = 0;
			virtual void visit(CommandDepositChangeStep const *) = 0;
			virtual void visit(CommandHarvestingChangeStep const *) = 0;
			virtual void visit(CommandHarvestTimeSinceHarvestStep const *) = 0;
			virtual void visit(CommandIncrementNoProgressStep const *) = 0;
			virtual void visit(CommandMoveLosStep const *) = 0;
			virtual void visit(CommandUpdateFlockingReached const *) = 0;
			virtual void visit(CommandMoveStepSinceUpdateIncrementStep const *) = 0;
			virtual void visit(CommandMoveUpdateStep const *) = 0;
			virtual void visit(CommandNewTargetStep const *) = 0;
			virtual void visit(CommandNextStep const *) = 0;
			virtual void visit(CommandResourceChangeStep const *) = 0;
			virtual void visit(CommandSetPositionFromStep const *) = 0;
			virtual void visit(CommandSpawnStep const *) = 0;
			virtual void visit(CommandStorageStep const *) = 0;
			virtual void visit(CommandUpdateLastIdStep const *) = 0;
			virtual void visit(CommandUpdateLastPosStep const *) = 0;
			virtual void visit(CommandWindUpDiffStep const *) = 0;
			virtual void visit(CustomStep const *steppable_p) {}
			virtual void visit(EntityAttackStep const *) = 0;
			virtual void visit(EntityBuffStep const *) = 0;
			virtual void visit(EntityConditionalBuffStep const *) = 0;
			virtual void visit(EntityFrozenStep const *) = 0;
			virtual void visit(EntityHitPointChangeStep const *) = 0;
			virtual void visit(EntityMoveStep const *) = 0;
			virtual void visit(EntitySpawnStep const *) = 0;
			virtual void visit(FlyingCommandSpawnStep const *) = 0;
			virtual void visit(FlyingCommandPopStep const *) = 0;
			virtual void visit(EntityUpdateReloadAbilityStep const *) = 0;
			virtual void visit(EntityUpdateWaitingStep const *) = 0;
			virtual void visit(MissingResourceStep const *) = 0;
			virtual void visit(PlayerAddBuildingModel const *) = 0;
			virtual void visit(PlayerAddCostBonusStep const *) = 0;
			virtual void visit(PlayerAddOptionStep const *) = 0;
			virtual void visit(PlayerAddTimeProdBonusStep const *) = 0;
			virtual void visit(PlayerAttackModAllStep const *) = 0;
			virtual void visit(PlayerBuffAllStep const *) = 0;
			virtual void visit(PlayerConditionalBuffAllStep const *) = 0;
			virtual void visit(PlayerLevelUpUpgradeStep const *) = 0;
			virtual void visit(PlayerPopOptionStep const *) = 0;
			virtual void visit(PlayerProducedUpgradeStep const *) = 0;
			virtual void visit(PlayerSpawnStep const *) = 0;
			virtual void visit(PlayerSpendResourceStep const *) = 0;
			virtual void visit(PlayerUpdateBuildingCountStep const *) = 0;
			virtual void visit(ProductionPaidStep const *) = 0;
			virtual void visit(ProductionProgressionStep const *) = 0;
			virtual void visit(ResourceSpawnStep const *) = 0;
			virtual void visit(StateAddConstraintPositionStep const *) = 0;
			virtual void visit(StateDrawStep const *) = 0;
			virtual void visit(StateFreeHandleStep const *) = 0;
			virtual void visit(StateRemoveConstraintPositionStep const *) = 0;
			virtual void visit(StateTemplePositionAddStep const *) = 0;
			virtual void visit(StateTemplePositionRemoveStep const *) = 0;
			virtual void visit(StateWinStep const *) = 0;
			virtual void visit(TeamVisionStep const *) = 0;
			virtual void visit(TickingStep const *) = 0;
			virtual void visit(TimerDataUpdateStep const *) = 0;
			virtual void visit(TriggerCountChange const *) = 0;
			virtual void visit(TriggerEnableChange const *) = 0;
			virtual void visit(TriggerEntityAddStep const *) = 0;
			virtual void visit(TriggerEntityResetStep const *) = 0;
			virtual void visit(TriggerSpawn const *) = 0;
			virtual void visit(TriggerStepCountChange const *) = 0;
			virtual void visit(UnitHarvestDropStep const *) = 0;
			virtual void visit(UnitHarvestQuantityStep const *) = 0;
			virtual void visit(UnitHarvestTypeStep const *) = 0;
			virtual void visit(UnitSpawnStep const *) = 0;
			virtual void visit(VisionChangeStep const *) = 0;
	};
}

#endif
