#ifndef __CommandDataLongStep__
#define __CommandDataLongStep__

#include "state/Handle.hh"
#include "step/Steppable.hh"

namespace octopus
{
class CommandDataLongDiffStep : public Steppable
{
public:
	CommandDataLongDiffStep(Handle const &handle_p, long diff_p)
		: _handle(handle_p) , _diff(diff_p){}

	virtual void apply(State &state_p) const override;
	virtual void revert(State &state_p) const override;

	virtual bool isNoOp() const override;

	Handle _handle {0};
	long _diff {0};
};
}

#endif
