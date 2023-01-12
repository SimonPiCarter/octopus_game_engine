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

			std::list<EntityStep *> const &getEntityStep() const;
		private:
			std::list<EntityStep *> _listEntityStep;
	};

	/// @brief apply the step and reverse it
	/// two consecutive apply of the same step should
	/// do nothing!
	void apply(Step &step_p);
}

std::ostream &streamCsvEntity(std::ostream &os_p, game_engine::Step const &step_p);

#endif
