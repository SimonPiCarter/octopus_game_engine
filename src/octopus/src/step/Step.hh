#ifndef __STEP__
#define __STEP__

#include <list>
#include <iostream>

namespace octopus
{
	class EntityMoveStep;
	class State;
	class Steppable;

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
		private:
			std::list<EntityMoveStep *> _listEntityMoveStep;

			std::list<Steppable *> _listSteppable;
	};

	/// @brief apply the step
	void apply(Step const &step_p, State &state_p);

	/// @brief apply the step
	void revert(Step const &step_p, State &state_p);

	/// @brief compact the step to avoid useles steps
	/// for example : remove all EntityMoveStep with no noticeable difference
	/// between pos and new pos
	void compact(Step &step_p);
}

#endif
