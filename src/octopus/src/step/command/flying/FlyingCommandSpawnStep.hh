#ifndef __FlyingCommandSpawnStep__
#define __FlyingCommandSpawnStep__

#include "step/Steppable.hh"

namespace octopus
{
class FlyingCommand;

class FlyingCommandSpawnStep : public Steppable
{
public:
	FlyingCommandSpawnStep(FlyingCommand *cmd_p) : _cmd(cmd_p) {}

	FlyingCommand * getCmd() { return _cmd; }
	FlyingCommand const * getCmd() const { return _cmd; }

	virtual void consolidate(State const &state_p, Step const &step_p) override;
	virtual void apply(State &state_p) const override;
	virtual void revert(State &state_p, SteppableData const *) const override;

	virtual bool isNoOp() const { return false; }

	virtual void visit(SteppableVisitor * visitor_p) const;
protected:
	FlyingCommand * const _cmd;
};

} // namespace octopus


#endif
