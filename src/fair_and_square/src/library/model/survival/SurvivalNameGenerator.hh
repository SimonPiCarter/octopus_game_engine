#ifndef __SurvivalNameGenerator__
#define __SurvivalNameGenerator__

struct CircleGen 	{ constexpr static char const* gen() { return "circle"; } };
struct TriangleGen 	{ constexpr static char const* gen() { return "triangle"; } };
struct AttackSpeedDivGen 	{ static char const* gen() { return "survival_attackspeed"; } };
struct HealDivGen 	{ static char const* gen() { return "survival_heal"; } };
struct LifestealDivGen 	{ static char const* gen() { return "survival_aoe"; } };
struct RecycleDivGen 	{ static char const* gen() { return "survival_rebound"; } };

#endif
