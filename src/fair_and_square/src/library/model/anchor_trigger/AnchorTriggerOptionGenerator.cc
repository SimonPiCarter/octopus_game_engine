#include "AnchorTriggerOptionGenerator.hh"

Option no_op(octopus::State const &, unsigned long, octopus::RandomGenerator &) { return Option(); }

Option no_op(octopus::State const &, unsigned long, octopus::RandomGenerator &, std::string const &) { return Option(); }

struct SquareGen 	{ constexpr static char const* gen() { return "square"; } };
struct CircleGen 	{ constexpr static char const* gen() { return "circle"; } };
struct TriangleGen 	{ constexpr static char const* gen() { return "triangle"; } };

template < class type_name_gen, octopus::TyppedBuff::Type type_t, int min_value, int max_value >
Option flat_basic(octopus::State const &, unsigned long player_p, octopus::RandomGenerator &rand_p, std::string const &id_p)
{
	Option opt_l;

	octopus::TimedBuff tBuff_l;
	tBuff_l._id = "flat_basic."+id_p;
	tBuff_l._type = type_t;
	tBuff_l._offset = rand_p.roll(min_value, max_value);

	BuffOption buff_l {
		player_p,
		tBuff_l,
		type_name_gen::gen()
	};

	opt_l._playerOption = buff_l;

	return opt_l;
}

std::list<AnchorOptionGenerator> generateOptionGenerators(octopus::State const &, unsigned long)
{
	std::list<AnchorOptionGenerator> generators_l {
		// Common
			// Flat basic
			{ 10, flat_basic<SquareGen, octopus::TyppedBuff::Type::Damage, 2, 2>, false },
			{ 10, flat_basic<CircleGen, octopus::TyppedBuff::Type::Damage, 2, 2>, false },
			{ 10, flat_basic<TriangleGen, octopus::TyppedBuff::Type::Damage, 2, 2>, false },
			{ 10, flat_basic<SquareGen, octopus::TyppedBuff::Type::Armor, 1, 1>, false },
			{ 10, flat_basic<CircleGen, octopus::TyppedBuff::Type::Armor, 1, 1>, false },
			{ 10, flat_basic<TriangleGen, octopus::TyppedBuff::Type::Armor, 1, 1>, false },
			{ 10, flat_basic<SquareGen, octopus::TyppedBuff::Type::HpMax, 10, 10>, false },
			{ 10, flat_basic<CircleGen, octopus::TyppedBuff::Type::HpMax, 10, 10>, false },
			{ 10, flat_basic<TriangleGen, octopus::TyppedBuff::Type::HpMax, 10, 10>, false },
		// Uncommon
			// Flat basic
			{ 5, flat_basic<SquareGen, octopus::TyppedBuff::Type::Damage, 5, 5>, false },
			{ 5, flat_basic<CircleGen, octopus::TyppedBuff::Type::Damage, 5, 5>, false },
			{ 5, flat_basic<TriangleGen, octopus::TyppedBuff::Type::Damage, 5, 5>, false },
			{ 5, flat_basic<SquareGen, octopus::TyppedBuff::Type::Armor, 2, 2>, false },
			{ 5, flat_basic<CircleGen, octopus::TyppedBuff::Type::Armor, 2, 2>, false },
			{ 5, flat_basic<TriangleGen, octopus::TyppedBuff::Type::Armor, 2, 2>, false },
			{ 5, flat_basic<SquareGen, octopus::TyppedBuff::Type::HpMax, 40, 40>, false },
			{ 5, flat_basic<CircleGen, octopus::TyppedBuff::Type::HpMax, 40, 40>, false },
			{ 5, flat_basic<TriangleGen, octopus::TyppedBuff::Type::HpMax, 40, 40>, false },
		// Rare
			// Flat basic
			{ 1, flat_basic<SquareGen, octopus::TyppedBuff::Type::Damage, 15, 15>, false },
			{ 1, flat_basic<CircleGen, octopus::TyppedBuff::Type::Damage, 15, 15>, false },
			{ 1, flat_basic<TriangleGen, octopus::TyppedBuff::Type::Damage, 15, 15>, false },
			{ 1, flat_basic<SquareGen, octopus::TyppedBuff::Type::Armor, 4, 4>, false },
			{ 1, flat_basic<CircleGen, octopus::TyppedBuff::Type::Armor, 4, 4>, false },
			{ 1, flat_basic<TriangleGen, octopus::TyppedBuff::Type::Armor, 4, 4>, false },
			{ 1, flat_basic<SquareGen, octopus::TyppedBuff::Type::HpMax, 100, 100>, false },
			{ 1, flat_basic<CircleGen, octopus::TyppedBuff::Type::HpMax, 100, 100>, false },
			{ 1, flat_basic<TriangleGen, octopus::TyppedBuff::Type::HpMax, 100, 100>, false },
	};

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
