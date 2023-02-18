#ifndef __StateTemplePositionRemoveStep__
#define __StateTemplePositionRemoveStep__

#include "step/Steppable.hh"
#include "utils/Vector.hh"

namespace octopus
{
class StateTemplePositionRemoveStep : public Steppable
{
public:
    StateTemplePositionRemoveStep(Vector const &pos_p) : _pos(pos_p) {}

	virtual void apply(State &state_p) const override;
	virtual void revert(State &state_p) const override;

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
