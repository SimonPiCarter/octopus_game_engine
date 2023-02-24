#ifndef __StateWinStep__
#define __StateWinStep__

#include "step/Steppable.hh"
#include "utils/Vector.hh"

namespace octopus
{
class StateWinStep : public Steppable
{
public:
    StateWinStep(bool oldIsOver_p, bool oldHasWinningTeam_p, unsigned long oldWinningTeam_p, unsigned long winningTeam_p)
		: _oldIsOver(oldIsOver_p), _oldhasWinningTeam(oldHasWinningTeam_p), _oldWinningTeam(oldWinningTeam_p), _winningTeam(winningTeam_p) {}

	virtual void apply(State &state_p) const override;
	virtual void revert(State &state_p) const override;

	virtual bool isNoOp() const override
	{
		return _oldIsOver == true && _oldhasWinningTeam == false;
	}
	virtual void visit(SteppableVisitor * visitor_p) const override
	{
		visitor_p->visit(this);
	}
private:
    bool const _oldIsOver;
    bool const _oldhasWinningTeam;
	unsigned long const _oldWinningTeam;
	unsigned long const _winningTeam;
};
}

#endif
