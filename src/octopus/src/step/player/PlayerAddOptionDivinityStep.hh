#ifndef __PlayerAddOptionDivinityStep__
#define __PlayerAddOptionDivinityStep__

#include <set>

#include "state/Handle.hh"
#include "step/Steppable.hh"
#include "state/DivinityType.hh"

namespace octopus
{

/// @brief add options for the given divinity types for the player
class PlayerAddOptionDivinityStep : public Steppable
{
public:
	PlayerAddOptionDivinityStep(unsigned long player_p, std::set<DivinityType> types_p, bool lvlUp_p)
		: _player(player_p), _types(types_p), _lvlUp(lvlUp_p) {}

	virtual void apply(State &state_p) const override;
	virtual void revert(State &state_p) const override;

	virtual bool isNoOp() const override { return _types.empty(); }
	virtual void visit(SteppableVisitor * visitor_p) const override
	{
		visitor_p->visit(this);
	}

	unsigned long const _player {0};
	std::set<DivinityType> const _types;
	bool const _lvlUp;
};

} // namespace octopus


#endif
