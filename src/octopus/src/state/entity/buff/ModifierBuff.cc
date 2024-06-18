
#include "ModifierBuff.hh"

#include "state/entity/Building.hh"
#include "state/entity/Entity.hh"
#include "state/player/Player.hh"
#include "state/State.hh"

namespace octopus
{

bool ModifierBuff::isApplying(State const &state_p, Entity const &ent_p) const
{
	return ent_p._model._isUnit;
}

bool ModifierBuff::isApplying(State const &state_p, Entity const &source_p, Entity const &ent_p) const
{
	return isApplying(state_p, ent_p);
}

void ModifierBuff::apply(Entity &ent_p) const
{
	ent_p._attackMod = _attackMod;
}

void ModifierBuff::revert(Entity &ent_p) const
{
	ent_p._attackMod = NoModifier();
}


} // namespace octopus
