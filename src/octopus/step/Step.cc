#include "Step.hh"

#include "step/entity/EntityStep.hh"
#include "utils/Vector.hh"

namespace octopus
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

std::list<EntityStep *> &Step::getEntityStep()
{
	return _listEntityStep;
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

void compact(Step & step_p)
{
	// update all position
	for(auto it_l = step_p.getEntityStep().begin() ; it_l != step_p.getEntityStep().end() ;)
	{
		// remove if no op
		if(isEntityStepNoOp(**it_l))
		{
			it_l = step_p.getEntityStep().erase(it_l);
		}
		else
		{
			++it_l;
		}
	}
}

} // namespace octopus
