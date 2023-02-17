#ifndef __STEP__
#define __STEP__

#include <list>
#include <iostream>
#include <map>

#include "state/ResourceType.hh"

namespace octopus
{
	class EntityMoveStep;
	class State;
	class Steppable;
	class SteppableVisitor;

	class Step
	{
		public:
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
		private:
			std::list<EntityMoveStep *> _listEntityMoveStep;

			std::list<Steppable *> _listSteppable;

			/// @brief a map of map of resource spent (player, resource) -> value spent this step
			std::map<unsigned long, std::map<ResourceType, double> > _spent;
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
}

#endif
