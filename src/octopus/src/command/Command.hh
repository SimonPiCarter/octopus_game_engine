#ifndef __Command__
#define __Command__

#include "CommandData.hh"
#include "state/Handle.hh"

namespace octopus
{
class KdTree;
class PathManager;
class Step;
class State;
class PathManager;

struct CommandContext
{
	PathManager &pathManager;
	KdTree const *kdTree;
	KdTree const *kdTreeStatic;
};

/// @brief represent a command that can be registered in a step and
/// applied to the state
/// It supports creating data to support the command
class AbstractCommand
{
public:
	AbstractCommand() {}
	virtual ~AbstractCommand() {}

	/// @brief register the command into the step
	/// This method is responsible for
	/// handling cost of command and spawning command in step
	/// and checking legality
	virtual void registerCommand(Step & step_p, State const &state_p) = 0;

	/// @brief compile command or info into the step
	/// @return true if command is over
	virtual bool applyCommand(Step & step_p, State const &state_p, CommandData const * data_p, CommandContext &commandContext_p) const = 0;

	/// @brief create data supporting the command actions
	virtual CommandData * newData() const { return nullptr; }

	virtual CommandData * getData() { return nullptr; }
	virtual CommandData const * getData() const { return nullptr; }
};

/// @brief symbolise a command given (by player; by ai)
/// those commands must be serializable otherwise replay from file will not be possible
class Command : public AbstractCommand
{
public:
	Command() : _handleCommand(0) {}
	Command(Handle const &handle_p) : _handleCommand(handle_p) {}
	virtual ~Command() {}

	/// @brief register the command into the step
	/// This method is responsible for
	/// handling cost of command and spawning command in step
	/// and checking legality
	virtual void registerCommand(Step & step_p, State const &state_p) override;

	/// @brief compile clean up for command (may reset some state values for example)
	/// is useful when command are interupted
	/// @param step_p push actions in this step
	/// @param state_p the state to use to get information
	/// @param data_p date of the command
	/// @note this will only be used on started commands when going to next command
	virtual void cleanUp(Step & step_p, State const &state_p, CommandData const * data_p) const;

	/// @brief Check if command is valid for given player
	/// @param state_p the state in case we need to get some info
	/// @param player_p the player supposed to commit the command
	/// @return true if the command can be commited for said player
	virtual bool checkPlayer(State const &state_p, unsigned long player_p) const;

	void setQueued(bool queued_p) { _queued = queued_p; }
	bool isQueued() const { return _queued; }
	Handle const &getHandleCommand() const { return _handleCommand; }

	unsigned long long const & getStepOfRegistration() const { return _stepOfRegistration; }
	void setStepOfRegristration(unsigned long long const &stepIdx_p) { _stepOfRegistration = stepIdx_p; }

protected:
	/// @brief the commandable handle for the executor of this command
	Handle _handleCommand {0};

	/// @brief if set to true the command will be queued up
	bool _queued { false };

	/// @brief the idx of the step this command was registered at
	unsigned long long _stepOfRegistration {0};
};

/// @brief those commands are not tied to an Entity
/// they can be used to represent events or effect like
/// - area of effect (damage/heal)
/// - chaining effects (damage/heal)
/// - on death effects
/// - effects over time (damage/heal)
/// all registered commands execute on every compilation
/// each state stores a instance of data (should be synced on same step count)
///
/// @warning FlyingCommand must have a unique handle to be inserted in the same order in case of reverting
/// removal
class FlyingCommand : public AbstractCommand
{
public:
	FlyingCommand(Handle handle_p) : _handle(handle_p) {}

	virtual void registerCommand(Step & step_p, State const &state_p);

	Handle const &getHandle() const { return _handle; }
private:
	Handle const _handle;
};

} // namespace octopus


#endif
