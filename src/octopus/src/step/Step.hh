#ifndef __STEP__
#define __STEP__

#include <list>
#include <iostream>

namespace octopus
{
	class EntityStep;
	class State;
	class Steppable;

	class Step
	{
		public:
			~Step();

			/// @brief add step and keep ownership
			void addEntityStep(EntityStep * step_p);

			std::list<EntityStep *> &getEntityStep();
			std::list<EntityStep *> const &getEntityStep() const;

			std::list<Steppable *> &getSteppable();
			std::list<Steppable *> const &getSteppable() const;
		private:
			std::list<EntityStep *> _listEntityStep;

			std::list<Steppable *> _listSteppable;
	};

	/// @brief apply the step
	void apply(Step const &step_p, State &state_p);

	/// @brief apply the step
	void revert(Step const &step_p, State &state_p);

	/// @brief compact the step to avoid useles steps
	/// for example : remove all EntityStep with no noticeable difference
	/// between pos and new pos
	void compact(Step &step_p);
}

#endif
