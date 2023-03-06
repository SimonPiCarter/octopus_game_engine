#ifndef __STEP__
#define __STEP__

#include <list>
#include <iostream>
#include <map>

#include "state/ResourceType.hh"
#include "state/Handle.hh"

namespace octopus
{
	class EntityMoveStep;
	class State;
	class Steppable;
	class SteppableVisitor;

	class Step
	{
		public:
			Step(Step const *prev_p) : _prev(prev_p) {}
			~Step();

			/// @brief add step and keep ownership
			void addEntityMoveStep(EntityMoveStep * step_p);
			/// @brief add step and keep ownership
			void addSteppable(Steppable * step_p);

			std::list<EntityMoveStep *> &getEntityMoveStep();
			std::list<EntityMoveStep *> const &getEntityMoveStep() const;

			std::list<Steppable *> &getSteppable();
			std::list<Steppable *> const &getSteppable() const;

			double & getResourceSpent(unsigned long player_p, ResourceType res_p);
			double getResourceSpent(unsigned long player_p, ResourceType res_p) const;

			const std::map<ResourceType, double> & getResourceSpent(unsigned long player_p) const;

			unsigned long & getDivOptionSpent(unsigned long player_p);
			unsigned long getDivOptionSpent(unsigned long player_p) const;

			/// @brief add an entity spawned
			void addEntitySpawned();
			/// @brief get the number of entity spawned in this step
			unsigned long getEntitySpawned() const;

			/// @brief get previous step
			/// @return previous step
			Step const * getPrev() const { return _prev; }
		private:
			std::list<EntityMoveStep *> _listEntityMoveStep;

			std::list<Steppable *> _listSteppable;

			/// @brief a map of map of resource spent (player, resource) -> value spent this step
			std::map<unsigned long, std::map<ResourceType, double> > _spent;

			/// @brief a map of number of div option spent in this step (player) -> number of div option spent this step
			std::map<unsigned long, unsigned long> _divOptionsSpent;

			/// @brief a map of entity spawned in this step
			unsigned long _entitySpawned {0};

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
