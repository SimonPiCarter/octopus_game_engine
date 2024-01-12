#include "EntityModel.hh"

namespace octopus
{

void idleFunctioNoOp(Entity const &, StepShallow &, State const &) {}

bool alwaysOkChecker(Step const &, State const &, Handle const &, Handle const &, Vector const &) { return true; }

std::map<std::string, Fixed> getReverseCostMap(std::map<std::string, Fixed> const &cost_p)
{
	std::map<std::string, Fixed> reversedCost_l;
	for(auto && pair_l : cost_p)
	{
		reversedCost_l[pair_l.first] = -pair_l.second;
	}
	return reversedCost_l;
}

Fixed getBonus(std::string const &id_p, EntityModel const &model_p)
{
	auto &&it_l = model_p._bonusDamage.find(id_p);
	if(it_l != model_p._bonusDamage.end())
	{
		return it_l->second;
	}
	return 0.;
}

void abilityNoOp(StepShallow &, State const &, Handle const &, Handle const &, Vector const &) {}

Ability const & getAbility(EntityModel const &model_p, std::string const &id_p)
{
	static Ability noAbility_s {"noOp", "noOp", abilityNoOp};
	auto &&it_l = model_p._abilities.find(id_p);
	if(it_l == model_p._abilities.end())
	{
		return noAbility_s;
	}
	return it_l->second;
}

unsigned long getMinReloadTime(EntityModel const &model_p, std::string const &key_p)
{
	auto &&it_l = model_p._abilityReloadTime.find(key_p);
	if(it_l == model_p._abilityReloadTime.end())
	{
		return 0;
	}
	return it_l->second;
}

} // namespace octopus

