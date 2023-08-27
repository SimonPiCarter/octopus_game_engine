#ifndef __STEP__
#define __STEP__

#include <list>
#include <iostream>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <vector>

#include "state/Handle.hh"
#include "utils/Fixed.hh"

namespace octopus
{
	class CommandData;
	class EntityMoveStep;
	class FlyingCommand;
	class State;
	class Steppable;
	struct SteppableData;
	class SteppableVisitor;

	struct CommandIdx
	{
		CommandIdx(Handle const &cmdable_p, unsigned long idx) : cmdable(cmdable_p), cmdidx(idx) {}
		Handle const &cmdable;
		unsigned long const &cmdidx;

		bool operator<(CommandIdx const &other_p) const
		{
			if(cmdable == other_p.cmdable)
			{
				return cmdidx < other_p.cmdidx;
			}
			return cmdable < other_p.cmdable;
		}
	};

	class Step
	{
		public:
			Step(Step const *prev_p) : _id(prev_p?prev_p->_id+1:0), _prev(prev_p) {}
			~Step();

			/// @brief add step and keep ownership
			void addEntityMoveStep(EntityMoveStep * step_p);
			/// @brief add step and keep ownership
			void addSteppable(Steppable * step_p);

			std::list<EntityMoveStep *> &getEntityMoveStep();
			std::list<EntityMoveStep *> const &getEntityMoveStep() const;

			std::list<Steppable const *> &getSteppable();
			std::list<Steppable const *> const &getSteppable() const;

			Fixed & getResourceSpent(unsigned long player_p, std::string res_p);
			Fixed getResourceSpent(unsigned long player_p, std::string res_p) const;

			const std::map<std::string, Fixed> & getResourceSpent(unsigned long player_p) const;

			unsigned long & getDivOptionSpent(unsigned long player_p);
			unsigned long getDivOptionSpent(unsigned long player_p) const;

			/// @brief add an entity spawned
			void addEntitySpawned();
			/// @brief get the number of entity spawned in this step
			unsigned long getEntitySpawned() const;

			/// @brief add an flying command spawned
			void addFlyingCommnandSpawned();
			/// @brief get the number of flying command spawned in this step
			unsigned long getFlyingCommandSpawned() const;

			/// @brief set the given slot of the given resource as taken in this step
			void setSlotTaken(Handle const &handle_p, int idx_p);
			/// @brief check if the given slot of the given resource is taken in this step
			bool isSlotTaken(Handle const &handle_p, int idx_p) const;

			/// @brief update produced upgrade
			void updateProducedUpgrade(unsigned long const &player_p, std::string const &upgrade_p, bool add_p);
			/// @brief check if the upgrade has been started during this step
			bool isUpgradeProduced(unsigned long const &player_p, std::string const &upgrade_p) const;

			/// @brief add a canceled building
			void addCanceledBuilding(Handle const &handle_p);
			/// @brief check if the building has been canceled already
			bool isCanceled(Handle const &handle_p) const;

			void addCmdCanceled(CommandIdx cmdidx_p);
			bool isCmdCanceled(CommandIdx cmdidx_p) const;

			void addAbilityRegistered(Handle const &handle_p, std::string const &key_p);
			bool checkAbilityNotAlreadyRegistered(Handle const &handle_p, std::string const &key_p) const;

			/// @brief add an hp change
			void addHpChange(Handle const &handle_p, Fixed delta_p);
			/// @brief get all cumulative hp change
			std::unordered_map<Handle, Fixed> const& getHpChange() const;
			/// @brief get hp change for given handle
			Fixed getHpChange(Handle const &handle_p) const;

			/// @brief get previous step
			/// @return previous step
			Step const * getPrev() const { return _prev; }

			unsigned long long getId() const { return _id; }
		private:
			std::list<EntityMoveStep *> _listEntityMoveStep;

			std::list<Steppable const *> _listSteppable;

			/// @brief the id of the step (previous step + 1)
			unsigned long long const _id {0};

			/// @brief a map of map of resource spent (player, resource) -> value spent this step
			std::map<unsigned long, std::map<std::string, Fixed> > _spent;

			/// @brief the number of entity spawned in this step
			unsigned long _entitySpawned {0};

			/// @brief the number of flying command spawned in this step
			unsigned long _flyingCommandSpawned {0};

			/// @brief set of cancelled buildings to avoid refunding them multiple times
			std::set<Handle> _canceledBuildings;

			/// @brief map of produced upgrade in this step per player and per upgrade id
			std::unordered_map<unsigned long, std::unordered_map<std::string, long> > _producedUpgrade;

			/// @brief map of all comand registered for every entity in this step (using reload key in the set)
			std::unordered_map<Handle, std::unordered_set<std::string> > _commandRegistered;

			/// @brief map of all slots taken for every resources
			std::unordered_map<Handle, std::unordered_set<int> > _slotTaken;

			/// @brief of commands data canceled in this step (to avoid refunding twice)
			std::set<CommandIdx> _canceledCmd;

			/// @brief vector of hp change for entities
			std::unordered_map<Handle, Fixed> _hpChange;

			Step const * const _prev {nullptr};

	};

	/// @brief This class handles data for steppables based on a Step
	/// each state will have its own set of data
	class StepData
	{
		public:
			~StepData();
			std::vector<SteppableData *> _listSteppableData;
	};

	/// @brief apply the step
	void apply(Step const &step_p, State &state_p, StepData &stepData_p);

	/// @brief apply the step
	void revert(Step const &step_p, State &state_p, StepData &stepData_p);

	/// @brief compact the step to avoid useles steps
	/// for example : remove all EntityMoveStep with no noticeable difference
	/// between pos and new pos
	void compact(Step &step_p);

	/// @brief visit all steppable in the step
	void visitAll(Step const &step_p, SteppableVisitor &visitor_p);

	/// @brief get the next handle to spawn an entity/commandable
	/// @param step_p the step to be applied
	/// @param state_p the state on which the step is gonna be applied
	/// @return the next handle
	Handle getNextHandle(Step const &step_p, State const &state_p);
}

#endif
