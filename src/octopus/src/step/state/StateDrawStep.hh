#ifndef __StateDrawStep__
#define __StateDrawStep__

#include "step/Steppable.hh"

namespace octopus
{
class StateDrawStep : public Steppable
{
public:
    StateDrawStep(bool oldIsOver_p, bool oldHasWinningTeam_p) : _oldIsOver(oldIsOver_p), _oldhasWinningTeam(oldHasWinningTeam_p) {}

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
};
}

#endif
