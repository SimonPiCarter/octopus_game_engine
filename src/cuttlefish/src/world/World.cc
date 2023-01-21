#include "World.hh"
#include "WorldUpdaterStepVisitor.hh"
#include "sprite/Sprite.hh"

// octopus
#include "controller/Controller.hh"
#include "step/Step.hh"

namespace cuttlefish
{

void World::handleStep(Window &window_p, octopus::StateAndSteps const &steps_p)
{
	WorldUpdaterStepVisitor vis_l(*this, window_p, steps_p._state);

	if(_first)
	{
		std::cout<<"first"<<std::endl;
		_lastIt = steps_p._steps.begin();
		_first = false;
	}

	// Every step missing
	for(auto it_l = _lastIt ; it_l != steps_p._stepIt ; ++it_l)
	{
		std::cout<<"iterating step"<<std::endl;
		// Visit every stepapble in the step
		for(octopus::Steppable const * steppable_l : (*it_l)->getSteppable())
		{
			vis_l(steppable_l);
		}
		_lastIt = it_l;
	}
}

void World::display(Window &window_p, double elapsed_p)
{
	for(Sprite * sprite_l : _listSprite)
	{
		sprite_l->update(elapsed_p);
		sprite_l->render(window_p);
	}
}


}
