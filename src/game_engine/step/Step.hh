#ifndef __STEP__
#define __STEP__

#include <vector>
#include <iostream>

namespace game_engine
{
	class EntityStep;

	class Step
	{
		public:
			std::vector<EntityStep *> _vecEntityStep;
	};

	void apply(Step &step_p);
}

std::ostream &streamCsvEntity(std::ostream &os_p, game_engine::Step const &step_p);

#endif
