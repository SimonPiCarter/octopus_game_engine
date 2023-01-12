#include "Step.hh"

#include "entity/EntityStep.hh"
#include "entity/Vector.hh"

void game_engine::apply(game_engine::Step & step_p)
{
	// update all position
	for(game_engine::EntityStep *ent_l: step_p._vecEntityStep)
	{
		std::swap(ent_l->_ent->_pos, ent_l->_newPosition);
	}

}

std::ostream &streamCsvEntity(std::ostream &os_p, game_engine::Step const &step_p)
{
	os_p<<"x,y,r"<<std::endl;
	for(game_engine::EntityStep *ent_l: step_p._vecEntityStep)
	{
		streamCsvPos(os_p, *ent_l->_ent) << std::endl;
	}
	return os_p;
}
