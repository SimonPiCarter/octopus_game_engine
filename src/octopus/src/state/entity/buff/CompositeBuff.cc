#include "CompositeBuff.hh"

namespace octopus
{

bool CompositeBuff::isApplying(State const &state_p, Entity const &ent_p) const
{
	bool is_applying_l = true;
	for(CompositableBuff const &buff_l : _buffs)
	{
    	std::visit([&](auto && arg) { is_applying_l &= arg.isApplying(state_p, ent_p); }, buff_l);
	}
	return is_applying_l;
}

bool CompositeBuff::isApplying(State const &state_p, Entity const &source_p, Entity const &ent_p) const
{
	bool is_applying_l = true;
	for(CompositableBuff const &buff_l : _buffs)
	{
    	std::visit([&](auto && arg) { is_applying_l &= arg.isApplying(state_p, source_p, ent_p); }, buff_l);
	}
	return is_applying_l;
}

void CompositeBuff::apply(Entity &ent_p) const
{
	for(CompositableBuff const &buff_l : _buffs)
	{
    	std::visit([&](auto && arg) { arg.apply(ent_p); }, buff_l);
	}
}

void CompositeBuff::revert(Entity &ent_p) const
{
	for(auto it_l = _buffs.rbegin(); it_l != _buffs.rend() ; ++it_l)
	{
		CompositableBuff const &buff_l = *it_l;
    	std::visit([&](auto && arg) { arg.revert(ent_p); }, buff_l);
	}
}

} // octopus
