#ifndef __PlayerAddOptionStep__
#define __PlayerAddOptionStep__

#include <string>
#include "step/Steppable.hh"
#include "state/player/StepOptionsGenerator.hh"

namespace octopus
{

/// @brief add options for the given divinity types for the player
class PlayerAddOptionStep : public Steppable
{
public:
	PlayerAddOptionStep(unsigned long player_p, std::string const &key_p, StepOptionsGenerator *generator_p)
		: _player(player_p), _key(key_p), _generator(generator_p) {}

	virtual void apply(State &state_p) const override;
	virtual void revert(State &state_p, SteppableData const *) const override;

	virtual bool isNoOp() const override { return _generator == nullptr; }
	virtual void visit(SteppableVisitor * visitor_p) const override
	{
		visitor_p->visit(this);
	}

	unsigned long const _player;
	std::string const _key;
	StepOptionsGenerator const * const _generator;
};

} // namespace octopus


#endif
