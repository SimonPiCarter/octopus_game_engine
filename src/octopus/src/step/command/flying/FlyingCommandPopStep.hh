#ifndef __FlyingCommandPopStep__
#define __FlyingCommandPopStep__

#include "command/Command.hh"
#include "step/Steppable.hh"
#include <map>

namespace octopus
{
class FlyingCommand;

class FlyingCommandPopStep : public Steppable
{
public:
	FlyingCommandPopStep(FlyingCommand *cmd_p) : _cmd(cmd_p) {}
	~FlyingCommandPopStep();

	FlyingCommand * getCmd() { return _cmd; }
	FlyingCommand const * getCmd() const { return _cmd; }

	virtual void apply(State &state_p) const override;
	virtual void revert(State &state_p, SteppableData const *) const override;

	virtual bool isNoOp() const { return false; }

	virtual void visit(SteppableVisitor * visitor_p) const;
protected:
	FlyingCommand * const _cmd;

	/// @brief mutable map of data to be stored
	/// @note this has to be mutable sadly since it will be filled during apply to a state
	/// @warning on deletion all data here will be deleted
	mutable std::map<unsigned long, CommandData *> _mapData;
};

} // namespace octopus


#endif
