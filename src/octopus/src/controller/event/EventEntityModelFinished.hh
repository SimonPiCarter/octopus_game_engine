#ifndef __EventEntityModelFinished__
#define __EventEntityModelFinished__


namespace octopus
{
class EntityModel;

class EventEntityModelFinished
{
public:
	EventEntityModelFinished(EntityModel const &model_p, unsigned long player_p) : _model(model_p), _player(player_p) {}

	EntityModel const &_model;

	unsigned long const _player;
};

}

#endif
