
#include "ModifierAoEBuff.hh"

#include "state/entity/Building.hh"
#include "state/entity/Entity.hh"
#include "state/player/Player.hh"
#include "state/State.hh"
#include <variant>

namespace octopus
{

bool ModifierAoEBuff::isApplying(State const &state_p, Entity const &ent_p) const
{
	return ent_p._model._isUnit;
}

bool ModifierAoEBuff::isApplying(State const &state_p, Entity const &source_p, Entity const &ent_p) const
{
	return isApplying(state_p, ent_p);
}

void ModifierAoEBuff::apply(Entity &ent_p) const
{
	if(std::holds_alternative<AoEModifier>(ent_p._attackMod))
	{
		AoEModifier & mod_l = std::get<AoEModifier>(ent_p._attackMod);
		mod_l._ratio += _deltaRatio;
		mod_l._range += _deltaRange;
	}
}

void ModifierAoEBuff::revert(Entity &ent_p) const
{
	if(std::holds_alternative<AoEModifier>(ent_p._attackMod))
	{
		AoEModifier & mod_l = std::get<AoEModifier>(ent_p._attackMod);
		mod_l._ratio -= _deltaRatio;
		mod_l._range -= _deltaRange;
	}
}

} // namespace octopus
