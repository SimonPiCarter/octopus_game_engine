#ifndef __CommandEffectOverTime__
#define __CommandEffectOverTime__

#include "command/Command.hh"
#include "command/data/TimerData.hh"

namespace octopus
{

/// @brief this FlyingCommand is to be used to add periodic effects
/// to the controller
/// - applyEffect should be overriden to apply the desirted effects (damage, healing, resources gain)
class CommandEffectOverTime : public FlyingCommand
{
public:
	CommandEffectOverTime(Handle handle_p, unsigned long tickRate_p, unsigned long nbOfTicks_p, bool applyOnStart_p=false)
		: FlyingCommand(handle_p), _tickRate(tickRate_p), _nbOfTicks(nbOfTicks_p), _applyOnStart(applyOnStart_p)
	{}

	/// @brief compile command or info into the step
	/// @return true if command is over
	virtual bool applyCommand(Step & step_p, State const &state_p, CommandData const * data_p, PathManager &pathManager_p) const;

	/// @brief create data supporting the command actions
	virtual CommandData * newData() const { return new TimerData(); }

	/// @brief virtual pure method to apply the effect
	/// @note add steppables here
	virtual void applyEffect(Step & step_p, State const &state_p, CommandData const * data_p, PathManager &pathManager_p) const = 0;

private:
	/// @brief number of step required for effect to occur
	unsigned long const _tickRate;

	/// @brief number of times the effect should occur
	/// @note can be set to 0 for infinite
	unsigned long const _nbOfTicks;

	/// @brief true if should trigger effect on start
	/// false if we should wait first tickrate
	bool const _applyOnStart;
};
} // namespace octopus

#endif
