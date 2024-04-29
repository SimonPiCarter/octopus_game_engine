#ifndef __SurvivalNameGenerator__
#define __SurvivalNameGenerator__

///
/// naming generator for units
///

// common/enemies
struct CircleGen 	{ constexpr static char const* gen() { return "circle"; } };
struct TriangleGen 	{ constexpr static char const* gen() { return "triangle"; } };
struct USquareGen 	{ constexpr static char const* gen() { return "U_square_survival"; } };
struct QuadSquareGen 	{ constexpr static char const* gen() { return "quad_square_survival"; } };
struct DoubleSquareGen 	{ constexpr static char const* gen() { return "double_square_survival"; } };
// specials
struct AttackSpeedDivGen 	{ constexpr static char const* gen() { return "survival_attackspeed"; } };
struct HealDivGen 	{ constexpr static char const* gen() { return "survival_heal"; } };
struct LifestealDivGen 	{ constexpr static char const* gen() { return "survival_aoe"; } };
struct RecycleDivGen 	{ constexpr static char const* gen() { return "survival_rebound"; } };
// supers
struct BufferAttackSpeedGen 	{ constexpr static char const* gen() { return "survival_buffer_attackspeed"; } };
struct BufferArmorGen 	{ constexpr static char const* gen() { return "survival_buffer_armor"; } };
struct BufferDamageGen 	{ constexpr static char const* gen() { return "survival_buffer_damage"; } };
struct SuperAoeGen 	{ constexpr static char const* gen() { return "survival_super_aoe"; } };
struct SuperChainingGen 	{ constexpr static char const* gen() { return "survival_super_chaining"; } };
struct SuperSniperGen 	{ constexpr static char const* gen() { return "survival_super_sniper"; } };

///
/// naming generator for upgrades
///

struct AttackSpeedAbility { constexpr static char const * gen() { return "survival_attack_speed_ability"; } };
struct AttackSpeedAbilityHp { constexpr static char const * gen() { return "survival_attack_speed_ability_hp"; } };
struct ArmorAbility { constexpr static char const * gen() { return "survival_armor_ability"; } };
struct ArmorAbilityHp { constexpr static char const * gen() { return "survival_armor_ability_hp"; } };
struct ArmorHpLowDamage { constexpr static char const * gen() { return "survival_armor_hp_low_damage"; } };
struct AegenBurst { constexpr static char const * gen() { return "survival_regen_burst"; } };
struct AoubleDamage { constexpr static char const * gen() { return "survival_double_damage"; } };

/// joined naming generator for upgrades and unit

template< typename upgrade_t, typename unit_t >
struct UpgradeUnitGenerator { static std::string gen() { return std::string("upgrade_unit.")+std::string(upgrade_t::gen()) + "." + std::string(unit_t::gen()); } };

#endif
