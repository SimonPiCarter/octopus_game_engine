#include "FASStepVisitor.hh"

#include "logger/Logger.hh"
#include "state/State.hh"
#include "state/entity/Entity.hh"
#include "step/entity/EntityHitPointChangeStep.hh"

#include "window/Window.hh"

using octopus::to_double;

namespace fas
{

void FASStepVisitor::visit(octopus::PlayerPopOptionStep const *)
{
	for(octopus::Vector worldPos_l : _anchorEffectPos)
	{
		octopus::Vector pos_l = _window.getPixelVector(octopus::to_double(worldPos_l.x), octopus::to_double(worldPos_l.y));

		cuttlefish::Picture *pic_l = new cuttlefish::Picture(_window.loadTexture("resources/fair_and_square/effect_anchor.png"), 128, 128, {8}, {0.1}, false);
		pic_l->setDestination(octopus::to_int(pos_l.x)-64, octopus::to_int(pos_l.y)-64, 128, 128);

		pic_l->setEndAfterLastFrame(true);
		_world.getDyingPictures().push_back(pic_l);
	}
	_anchorEffectPos.clear();
}

void FASStepVisitor::visit(octopus::EntityHitPointChangeStep const *step_p)
{
	octopus::Entity const *ent_l = _state->getEntity(step_p->_handle);
	if(step_p->_delta > 0)
	{
		octopus::Vector pos_l = _window.getPixelVector(octopus::to_double(ent_l->_pos.x), octopus::to_double(ent_l->_pos.y));

		cuttlefish::Picture *pic_l = new cuttlefish::Picture(_window.loadTexture("resources/fair_and_square/healing.png"), 96, 96, {9}, {0.1}, false);
		pic_l->setDestination(octopus::to_int(pos_l.x)-48, octopus::to_int(pos_l.y)-48, 96, 96);

		pic_l->setEndAfterLastFrame(true);
		_world.getDyingPictures().push_back(pic_l);
	}
}


}
