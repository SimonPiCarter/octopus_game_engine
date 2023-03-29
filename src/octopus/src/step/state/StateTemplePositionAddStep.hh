#ifndef __StateTemplePositionAddStep__
#define __StateTemplePositionAddStep__

#include "step/Steppable.hh"
#include "utils/Vector.hh"

namespace octopus
{
class StateTemplePositionAddStep : public Steppable
{
public:
    StateTemplePositionAddStep(Vector const &pos_p) : _pos(pos_p) {}

	virtual void apply(State &state_p) const override;
	virtual void revert(State &state_p, SteppableData const *) const override;

	virtual bool isNoOp() const override
	{
		return false;
	}
	virtual void visit(SteppableVisitor * visitor_p) const override
	{
		visitor_p->visit(this);
	}
private:
    Vector const _pos;
};
}

#endif
