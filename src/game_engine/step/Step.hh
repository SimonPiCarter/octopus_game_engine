#ifndef __STEP__
#define __STEP__

#include <list>
#include <iostream>

namespace game_engine
{
	class EntityStep;

	class Step
	{
		public:
			~Step();

			/// @brief add step and keep ownership
			void addEntityStep(EntityStep * step_p);

			std::list<EntityStep *> &getEntityStep();
			std::list<EntityStep *> const &getEntityStep() const;
		private:
			std::list<EntityStep *> _listEntityStep;
	};

	/// @brief apply the step and reverse it
	/// two consecutive apply of the same step should
	/// do nothing!
	void apply(Step &step_p);

	/// @brief compact the step to avoid useles steps
	/// for example : remove all EntityStep with no noticeable differance
	/// between pos and new pos
	void compact(Step &step_p);
}

#endif
