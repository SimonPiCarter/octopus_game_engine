#ifndef __EventEntityModelDied__
#define __EventEntityModelDied__

namespace octopus
{
struct EntityModel;

class EventEntityModelDied
{
public:
	EventEntityModelDied(EntityModel const &model_p, unsigned long player_p) : _model(model_p), _player(player_p) {}

	EntityModel const &_model;

	unsigned long const _player;
};

}

#endif
