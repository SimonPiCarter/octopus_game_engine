#include "AnchorTriggerOptionGenerator.hh"

#include "library/model/divinity/generators/UnitModelIds.hh"
#include "library/model/survival/SurvivalModelLoader.hh"
#include "library/model/survival/SurvivalNameGenerator.hh"
#include "state/State.hh"
#include "state/entity/buff/ModifierAoEBuff.hh"
#include "state/entity/buff/TimedBuff.hh"
#include "state/player/Player.hh"
#include "utils/Fixed.hh"

#include "DemoConstants.h"


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

template < class type_name_gen, int min_value, int max_value, bool range >
Option aoe_buff_type(octopus::State const &, unsigned long player_p, octopus::RandomGenerator &rand_p, std::string const &id_p)
{
	Option opt_l;

	octopus::ModifierAoEBuff tBuff_l;
	tBuff_l._id = "aoe_buff."+id_p;
	if(range)
	{
		tBuff_l._deltaRange = octopus::Fixed(rand_p.roll(min_value, max_value)) / octopus::Fixed(100);
	}
	else
	{
		tBuff_l._deltaRatio = octopus::Fixed(rand_p.roll(min_value, max_value)) / octopus::Fixed(100);
	}

	BuffOption buff_l {
		player_p,
		tBuff_l,
		type_name_gen::gen()
	};

	opt_l._playerOption = buff_l;

	return opt_l;
}

template< class upgrade_t, class unit_t >
Option upgrade_option(octopus::State const &, unsigned long player_p, octopus::RandomGenerator &, std::string const &)
{
	Option opt_l;

	opt_l._playerOption = UpgradeOption { player_p, UpgradeUnitGenerator<upgrade_t, unit_t>::gen() };

	return opt_l;
}

template< class unit_t >
Option spawn_option(octopus::State const &, unsigned long player_p, octopus::RandomGenerator &, std::string const &)
{
	Option opt_l;

	opt_l._playerOption = SpawnUnitOption { player_p, unit_t::gen(), octopus::Vector(120, 120) };

	return opt_l;
}

template< class upgrade_t, class unit_t >
void add_upgrade(octopus::Player const &player_p, std::list<AnchorOptionGenerator> &generators_p)
{
	if(octopus::getUpgradeLvl(player_p, UpgradeUnitGenerator<upgrade_t, unit_t>::gen()) == 0)
	{
		generators_p.push_back({ 1, upgrade_option<upgrade_t, unit_t>, false });
	}
}

template< class unit_t >
void add_upgrades_ability(octopus::Player const &player_p, std::list<AnchorOptionGenerator> &generators_p)
{
	add_upgrade<AttackSpeedAbility, unit_t>(player_p, generators_p);
	add_upgrade<AttackSpeedAbilityHp, unit_t>(player_p, generators_p);
	add_upgrade<ArmorAbility, unit_t>(player_p, generators_p);
	add_upgrade<ArmorAbilityHp, unit_t>(player_p, generators_p);
	add_upgrade<ArmorHpLowDamage, unit_t>(player_p, generators_p);
	add_upgrade<AegenBurst, unit_t>(player_p, generators_p);
	add_upgrade<AoubleDamage, unit_t>(player_p, generators_p);
}

std::list<AnchorOptionGenerator> generateOptionGenerators(octopus::State const &state_p, unsigned long player_p)
{
	std::list<AnchorOptionGenerator> generators_l {
		// Common
			// Flat basic
			{ 10, flat_basic<CircleGen, octopus::TyppedBuff::Type::Damage, 200, 200>, false },
			{ 10, flat_basic<TriangleGen, octopus::TyppedBuff::Type::Damage, 200, 200>, false },
			{ 10, flat_basic<CircleGen, octopus::TyppedBuff::Type::HpMax, 1000, 1000>, false },
			{ 10, flat_basic<TriangleGen, octopus::TyppedBuff::Type::HpMax, 1000, 1000>, false },
		// Uncommon
			// Flat basic
			{ 5, flat_basic<CircleGen, octopus::TyppedBuff::Type::Damage, 500, 500>, false },
			{ 5, flat_basic<TriangleGen, octopus::TyppedBuff::Type::Damage, 500, 500>, false },
			{ 5, flat_basic<CircleGen, octopus::TyppedBuff::Type::HpMax, 4000, 4000>, false },
			{ 5, flat_basic<TriangleGen, octopus::TyppedBuff::Type::HpMax, 4000, 4000>, false },
		// Spawn supers
			{ 3, spawn_option<BufferAttackSpeedGen>, false},
			{ 3, spawn_option<SuperAoeGen>, false},
	};
	if(!IS_DEMO)
	{
		generators_l.push_back({ 3, spawn_option<BufferArmorGen>, false});
		generators_l.push_back({ 3, spawn_option<BufferDamageGen>, false});
		generators_l.push_back({ 3, spawn_option<SuperChainingGen>, false});
		generators_l.push_back({ 3, spawn_option<SuperSniperGen>, false});
	}

	octopus::Player const &player_l = *state_p.getPlayer(player_p);

	if(octopus::getUpgradeLvl(player_l, fas::survivalSpecialTypeName(fas::SurvivalSpecialType::AttackSpeed)) > 0)
	{
		// Damage buffs
		generators_l.push_back({ 10, flat_basic<AttackSpeedDivGen, octopus::TyppedBuff::Type::Damage, 200, 200>, false });
		generators_l.push_back({ 5, flat_basic<AttackSpeedDivGen, octopus::TyppedBuff::Type::Damage, 400, 400>, false });
		generators_l.push_back({ 1, flat_basic<AttackSpeedDivGen, octopus::TyppedBuff::Type::Damage, 1000, 1000>, false });
		// Full Reload
		generators_l.push_back({ 10, coef_basic<AttackSpeedDivGen, octopus::TyppedBuff::Type::FullReload, 5, 5>, false });
		generators_l.push_back({ 5, coef_basic<AttackSpeedDivGen, octopus::TyppedBuff::Type::FullReload, 10, 10>, false });
		generators_l.push_back({ 1, coef_basic<AttackSpeedDivGen, octopus::TyppedBuff::Type::FullReload, 25, 25>, false });
		if(!IS_DEMO)
		{
			// abilities
			add_upgrades_ability<AttackSpeedDivGen>(player_l, generators_l);
		}
	}
	else if(!IS_DEMO)
	{
		generators_l.push_back({ 1, special_type<fas::SurvivalSpecialType::AttackSpeed>, false });
	}

	if(octopus::getUpgradeLvl(player_l, fas::survivalSpecialTypeName(fas::SurvivalSpecialType::Heal)) > 0)
	{
		// Heal buffs
		generators_l.push_back({ 10, flat_basic<HealDivGen, octopus::TyppedBuff::Type::Heal, 200, 200>, false });
		generators_l.push_back({ 5, flat_basic<HealDivGen, octopus::TyppedBuff::Type::Heal, 400, 400>, false });
		generators_l.push_back({ 1, flat_basic<HealDivGen, octopus::TyppedBuff::Type::Damage, 1000, 1000>, false });
		// Full Reload
		generators_l.push_back({ 10, coef_basic<HealDivGen, octopus::TyppedBuff::Type::FullReload, 5, 5>, false });
		generators_l.push_back({ 5, coef_basic<HealDivGen, octopus::TyppedBuff::Type::FullReload, 10, 10>, false });
		generators_l.push_back({ 1, coef_basic<HealDivGen, octopus::TyppedBuff::Type::FullReload, 25, 25>, false });
		if(!IS_DEMO)
		{
			// abilities
			add_upgrades_ability<HealDivGen>(player_l, generators_l);
		}
	}
	else
	{
		generators_l.push_back({ 1, special_type<fas::SurvivalSpecialType::Heal>, false });
	}

	if(octopus::getUpgradeLvl(player_l, fas::survivalSpecialTypeName(fas::SurvivalSpecialType::AreaOfDamage)) > 0)
	{
		// Damage buffs
		generators_l.push_back({ 10, flat_basic<LifestealDivGen, octopus::TyppedBuff::Type::Damage, 300, 300>, false });
		// Aoe Radius
		generators_l.push_back({ 10, aoe_buff_type<LifestealDivGen, 100, 100, true>, false });
		generators_l.push_back({ 5, aoe_buff_type<LifestealDivGen, 200, 200, true>, false });
		generators_l.push_back({ 1, aoe_buff_type<LifestealDivGen, 500, 500, true>, false });
		generators_l.push_back({ 5, aoe_buff_type<LifestealDivGen, 10, 10, false>, false });
		generators_l.push_back({ 1, aoe_buff_type<LifestealDivGen, 40, 40, false>, false });
		if(!IS_DEMO)
		{
			// abilities
			add_upgrades_ability<LifestealDivGen>(player_l, generators_l);
		}
	}
	else if(!IS_DEMO)
	{
		generators_l.push_back({ 1, special_type<fas::SurvivalSpecialType::AreaOfDamage>, false });
	}

	if(octopus::getUpgradeLvl(player_l, fas::survivalSpecialTypeName(fas::SurvivalSpecialType::Rebound)) > 0)
	{
		// Damage buffs
		generators_l.push_back({ 10, flat_basic<RecycleDivGen, octopus::TyppedBuff::Type::DamageReturn, 5, 5>, false });
		generators_l.push_back({ 5, flat_basic<RecycleDivGen, octopus::TyppedBuff::Type::DamageReturn, 10, 10>, false });
		generators_l.push_back({ 1, flat_basic<RecycleDivGen, octopus::TyppedBuff::Type::DamageReturn, 40, 40>, false });
		// Hp buffs
		generators_l.push_back({ 10, flat_basic<RecycleDivGen, octopus::TyppedBuff::Type::HpMax, 1000, 1000>, false });
		generators_l.push_back({ 5, flat_basic<RecycleDivGen, octopus::TyppedBuff::Type::HpMax, 4000, 4000>, false });
		generators_l.push_back({ 1, flat_basic<RecycleDivGen, octopus::TyppedBuff::Type::HpMax, 10000, 10000>, false });
		if(!IS_DEMO)
		{
			// abilities
			add_upgrades_ability<RecycleDivGen>(player_l, generators_l);
		}
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
