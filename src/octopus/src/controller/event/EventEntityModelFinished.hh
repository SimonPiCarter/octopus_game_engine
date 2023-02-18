#ifndef __EventEntityModelFinished__
#define __EventEntityModelFinished__


namespace octopus
{
class Entity;
struct EntityModel;

class EventEntityModelFinished
{
public:
	EventEntityModelFinished(Entity const &entity_p, EntityModel const &model_p, unsigned long player_p) : _entity(entity_p), _model(model_p), _player(player_p) {}

	Entity const &_entity;
	EntityModel const &_model;

	unsigned long const _player;
};

}

#endif
