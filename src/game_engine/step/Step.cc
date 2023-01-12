#include "Step.hh"

#include "entity/EntityStep.hh"
#include "entity/Vector.hh"

namespace game_engine
{


Step::~Step()
{
	for(EntityStep * step_l : _listEntityStep)
	{
		delete step_l;
	}
}

void Step::addEntityStep(EntityStep * step_p)
{
	_listEntityStep.push_back(step_p);
}

std::list<EntityStep *> const &Step::getEntityStep() const
{
	return _listEntityStep;
}

void apply(Step & step_p)
{
	// update all position
	for(EntityStep *ent_l: step_p.getEntityStep())
	{
		std::swap(ent_l->_ent->_pos, ent_l->_newPosition);
	}

	// now all steps are reversed !
	// this is important in case want to undo a step
}

} // namespace game_engine

std::ostream &streamCsvEntity(std::ostream &os_p, game_engine::Step const &step_p)
{
	os_p<<"x,y,r"<<std::endl;
	for(game_engine::EntityStep *ent_l: step_p.getEntityStep())
	{
		streamCsvPos(os_p, *ent_l->_ent) << std::endl;
	}
	return os_p;
}

