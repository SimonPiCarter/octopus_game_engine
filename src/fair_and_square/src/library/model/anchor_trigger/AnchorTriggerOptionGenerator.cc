#include "AnchorTriggerOptionGenerator.hh"

#include "library/model/divinity/generators/UnitModelIds.hh"
#include "library/model/survival/SurvivalModelLoader.hh"
#include "library/model/survival/SurvivalNameGenerator.hh"
#include "state/State.hh"
#include "state/player/Player.hh"
#include "utils/Fixed.hh"


Option no_op(octopus::State const &, unsigned long, octopus::RandomGenerator &, std::string const &) { return Option(); }


template < class type_name_gen, octopus::TyppedBuff::Type type_t, int min_value, int max_value >
Option flat_basic(octopus::State const &, unsigned long player_p, octopus::RandomGenerator &rand_p, std::string const &id_p)
{
	Option opt_l;

	octopus::TimedBuff tBuff_l;
	tBuff_l._id = "flat_basic."+id_p;
	tBuff_l._type = type_t;
	tBuff_l._offset = octopus::Fixed(rand_p.roll(min_value, max_value)) / octopus::Fixed(100);

	BuffOption buff_l {
		player_p,
		tBuff_l,
		type_name_gen::gen()
	};

	opt_l._playerOption = buff_l;

	return opt_l;
}

template < class type_name_gen, octopus::TyppedBuff::Type type_t, int min_value, int max_value >
Option coef_basic(octopus::State const &, unsigned long player_p, octopus::RandomGenerator &rand_p, std::string const &id_p)
{
	Option opt_l;

	octopus::TimedBuff tBuff_l;
	tBuff_l._id = "coef_basic."+id_p;
	tBuff_l._type = type_t;
	tBuff_l._coef = - octopus::Fixed(rand_p.roll(min_value, max_value)) / octopus::Fixed(100);

	BuffOption buff_l {
		player_p,
		tBuff_l,
		type_name_gen::gen()
	};

	opt_l._playerOption = buff_l;

	return opt_l;
}

template<fas::SurvivalSpecialType type_t>
Option special_type(octopus::State const &, unsigned long player_p, octopus::RandomGenerator &, std::string const &)
{
	Option opt_l;

	SurvivalOption survival_opt_l {player_p, type_t};
	opt_l._playerOption = survival_opt_l;

	return opt_l;
}

std::list<AnchorOptionGenerator> generateOptionGenerators(octopus::State const &state_p, unsigned long player_p)
{
	std::list<AnchorOptionGenerator> generators_l {
		// Common
			// Flat basic
			{ 10, flat_basic<CircleGen, octopus::TyppedBuff::Type::Damage, 20, 20>, false },
			{ 10, flat_basic<TriangleGen, octopus::TyppedBuff::Type::Damage, 20, 20>, false },
			{ 10, flat_basic<CircleGen, octopus::TyppedBuff::Type::HpMax, 100, 100>, false },
			{ 10, flat_basic<TriangleGen, octopus::TyppedBuff::Type::HpMax, 100, 100>, false },
		// Uncommon
			// Flat basic
			{ 5, flat_basic<CircleGen, octopus::TyppedBuff::Type::Damage, 50, 50>, false },
			{ 5, flat_basic<TriangleGen, octopus::TyppedBuff::Type::Damage, 50, 50>, false },
			{ 5, flat_basic<CircleGen, octopus::TyppedBuff::Type::HpMax, 400, 400>, false },
			{ 5, flat_basic<TriangleGen, octopus::TyppedBuff::Type::HpMax, 400, 400>, false },
	};

	octopus::Player const &player_l = *state_p.getPlayer(player_p);

	if(octopus::getUpgradeLvl(player_l, fas::survivalSpecialTypeName(fas::SurvivalSpecialType::AttackSpeed)) > 0)
	{
		// Damage buffs
		generators_l.push_back({ 10, flat_basic<AttackSpeedDivGen, octopus::TyppedBuff::Type::Damage, 20, 20>, false });
		generators_l.push_back({ 5, flat_basic<AttackSpeedDivGen, octopus::TyppedBuff::Type::Damage, 50, 50>, false });
		// Full Reload
		generators_l.push_back({ 10, coef_basic<AttackSpeedDivGen, octopus::TyppedBuff::Type::FullReload, 5, 5>, false });
		generators_l.push_back({ 5, coef_basic<AttackSpeedDivGen, octopus::TyppedBuff::Type::FullReload, 10, 10>, false });
	}
	else
	{
		generators_l.push_back({ 1, special_type<fas::SurvivalSpecialType::AttackSpeed>, false });
	}

	if(octopus::getUpgradeLvl(player_l, fas::survivalSpecialTypeName(fas::SurvivalSpecialType::Heal)) > 0)
	{
		// Heal buffs
		generators_l.push_back({ 10, flat_basic<HealDivGen, octopus::TyppedBuff::Type::Heal, 20, 20>, false });
		generators_l.push_back({ 5, flat_basic<HealDivGen, octopus::TyppedBuff::Type::Heal, 40, 40>, false });
		// Full Reload
		generators_l.push_back({ 10, coef_basic<HealDivGen, octopus::TyppedBuff::Type::FullReload, 5, 5>, false });
		generators_l.push_back({ 5, coef_basic<HealDivGen, octopus::TyppedBuff::Type::FullReload, 10, 10>, false });
	}
	else
	{
		generators_l.push_back({ 1, special_type<fas::SurvivalSpecialType::Heal>, false });
	}

	if(octopus::getUpgradeLvl(player_l, fas::survivalSpecialTypeName(fas::SurvivalSpecialType::AreaOfDamage)) > 0)
	{
		// Damage buffs
		generators_l.push_back({ 10, flat_basic<LifestealDivGen, octopus::TyppedBuff::Type::Damage, 50, 50>, false });
		generators_l.push_back({ 5, flat_basic<LifestealDivGen, octopus::TyppedBuff::Type::Damage, 100, 100>, false });
		// Aoe Radius
		// TODO
	}
	else
	{
		generators_l.push_back({ 1, special_type<fas::SurvivalSpecialType::AreaOfDamage>, false });
	}

	if(octopus::getUpgradeLvl(player_l, fas::survivalSpecialTypeName(fas::SurvivalSpecialType::Rebound)) > 0)
	{
		// Damage buffs
		generators_l.push_back({ 10, flat_basic<RecycleDivGen, octopus::TyppedBuff::Type::DamageReturn, 5, 5>, false });
		generators_l.push_back({ 5, flat_basic<RecycleDivGen, octopus::TyppedBuff::Type::DamageReturn, 10, 10>, false });
		// Hp buffs
		generators_l.push_back({ 10, flat_basic<RecycleDivGen, octopus::TyppedBuff::Type::HpMax, 100, 100>, false });
		generators_l.push_back({ 5, flat_basic<RecycleDivGen, octopus::TyppedBuff::Type::HpMax, 400, 400>, false });
	}
	else
	{
		generators_l.push_back({ 1, special_type<fas::SurvivalSpecialType::Rebound>, false });
	}

	return generators_l;
}

unsigned long getSumOfProba(std::list<AnchorOptionGenerator> const &generators_p)
{
	unsigned long proba_l = 0;
	for(AnchorOptionGenerator const &gen_l : generators_p)
	{
		proba_l += gen_l.proba_coef;
	}
	return proba_l;
}
