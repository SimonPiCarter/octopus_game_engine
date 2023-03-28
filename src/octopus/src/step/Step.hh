#ifndef __STEP__
#define __STEP__

#include <list>
#include <iostream>
#include <map>
#include <unordered_map>
#include <set>

#include "state/ResourceType.hh"
#include "state/Handle.hh"

namespace octopus
{
	class CommandData;
	class EntityMoveStep;
	class FlyingCommand;
	class State;
	class Steppable;
	class SteppableData;
	class SteppableVisitor;

	/// @brief simple class to store a steppable and its associated data
	struct SteppableBundle
	{
		Steppable const *_steppable {nullptr};
		SteppableData *_data {nullptr};
	};

	class Step
	{
		public:
			Step(Step const *prev_p) : _prev(prev_p), _id(prev_p?prev_p->_id+1:0) {}
			~Step();

			/// @brief add step and keep ownership
			void addEntityMoveStep(EntityMoveStep * step_p);
			/// @brief add step and keep ownership
			void addSteppable(Steppable * step_p);

			std::list<EntityMoveStep *> &getEntityMoveStep();
			std::list<EntityMoveStep *> const &getEntityMoveStep() const;

			std::list<SteppableBundle> &getSteppable();
			std::list<SteppableBundle> const &getSteppable() const;

			double & getResourceSpent(unsigned long player_p, ResourceType res_p);
			double getResourceSpent(unsigned long player_p, ResourceType res_p) const;

			const std::map<ResourceType, double> & getResourceSpent(unsigned long player_p) const;

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

			/// @brief add a canceled building
			void addCanceledBuilding(Handle const &handle_p);
			/// @brief check if the building has been canceled already
			bool isCanceled(Handle const &handle_p) const;

			void addCmdCanceled(CommandData const *data_p);
			bool isCmdCanceled(CommandData const *data_p) const;

			/// @brief add an hp change
			void addHpChange(Handle const &handle_p, double delta_p);
			/// @brief get all cumulative hp change
			std::unordered_map<Handle, double> const& getHpChange() const;

			/// @brief get previous step
			/// @return previous step
			Step const * getPrev() const { return _prev; }

			unsigned long long getId() const { return _id; }
		private:
			std::list<EntityMoveStep *> _listEntityMoveStep;

			std::list<SteppableBundle> _listSteppable;

			/// @brief the id of the step (previous step + 1)
			unsigned long long const _id {0};

			/// @brief a map of map of resource spent (player, resource) -> value spent this step
			std::map<unsigned long, std::map<ResourceType, double> > _spent;

			/// @brief a map of number of div option spent in this step (player) -> number of div option spent this step
			std::map<unsigned long, unsigned long> _divOptionsSpent;

			/// @brief the number of entity spawned in this step
			unsigned long _entitySpawned {0};

			/// @brief the number of flying command spawned in this step
			unsigned long _flyingCommandSpawned {0};

			/// @brief set of cancelled buildings to avoid refunding them multiple times
			std::set<Handle> _canceledBuildings;

			/// @brief of commands data canceled in this step (to avoid refunding twice)
			std::set<CommandData const *> _canceledCmd;

			/// @brief vector of hp change for entities
			std::unordered_map<Handle, double> _hpChange;

			Step const * const _prev {nullptr};

	};

	/// @brief apply the step
	void apply(Step const &step_p, State &state_p);

	/// @brief apply the step
	void revert(Step const &step_p, State &state_p);

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
