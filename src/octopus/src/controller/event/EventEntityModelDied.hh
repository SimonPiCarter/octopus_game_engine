#ifndef __EventEntityModelDied__
#define __EventEntityModelDied__

namespace octopus
{
class Entity;
struct EntityModel;

class EventEntityModelDied
{
public:
	EventEntityModelDied(Entity const &entity_p, EntityModel const &model_p, unsigned long player_p) : _entity(entity_p), _model(model_p), _player(player_p) {}

	Entity const &_entity;
	EntityModel const &_model;

	unsigned long const _player;
};

}

#endif
