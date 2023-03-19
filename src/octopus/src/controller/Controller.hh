#ifndef __CONTROLLER__
#define __CONTROLLER__

#include <mutex>
#include <list>
#include <vector>
#include <unordered_map>

#include "Metrics.hh"
#include "trigger/Trigger.hh"
#include "step/Step.hh"
#include "graph/PathManager.hh"

namespace octopus
{

class AbstractCommand;
class Command;
class OrcaManager;
class Steppable;
class State;

/// @brief simple structure aggregating usefull info for triple buffering
struct BufferedState
{
	BufferedState(unsigned long stepHandled_p, std::list<Step *>::iterator it_p, State *state_p);
	~BufferedState();

	unsigned long long getNextStepToApplyId() const;

	unsigned long _stepHandled {0};
	/// @brief iterator after the last handled step
	std::list<Step *>::iterator _stepIt;
	State * _state {nullptr};
};

/// @brief simple structure used to return in one go a state
/// its last step applied and the list of all steps
/// this is necessary to process all steps between last
/// state returned and the new state (especially for display purpose)
struct StateAndSteps
{
	/// @brief iterator after the last handled step
	std::list<Step *>::iterator _stepIt;
	std::list<Step *> const &_steps;
	State const *_state;
	Step const &_initialStep;
};

/// @brief This class aims at controlling a triple state buffer
/// It works this way
/// - Three State are kept in memory
///   - Back State :  We increase the step count of this state and apply new steps
///   - Buffer State : This state is stationnary
///   - Front State : This state is stationnary and is accessible from the outside
///
/// during the main loop :
/// - ongoing step counter is updated on a regular time basis
/// - no more step can be played than the ongoing step counter - 1
///
/// While no outside event are triggered :
/// - Back State is increasing its step count
/// - As soon as Back State step count is greater than buffer State step count we swap them
/// - Goes on and on
///
/// State Query : A new state is asked from outside
/// - iif Buffer State step count > Front State step count
///   - Buffer State and Front State are swapped
///
/// Command commit : A command is added in ongoing step
///
class Controller
{

public:
	Controller(std::list<Steppable *> const &initSteppables_p, double timePerStep_p,
		std::list<Command *> const &initCommands_p = {}, unsigned long gridPointSize_p=1, unsigned long gridSize_p=50);
	~Controller();

	/// @brief returns true once buffer and back states are up to date
	bool loop_body();

	/// @brief increment ongoing step if necessary
	/// increment back buffer for as much as required
	/// perform internal swaps if necessary
	unsigned long update(double elapsedTime_p);

	/// @brief update and return the front state
	/// only swap the buffer and front state if buffer state is more advanced than front state
	State const * queryState();
	/// @brief update and return the front state with steps information
	/// only swap the buffer and front state if buffer state is more advanced than front state
	StateAndSteps queryStateAndSteps();

	/// @brief add a Command to the ongoing step
	void commitCommand(Command * cmd_p);

	/// @brief add a Command to the ongoing step
	/// @note only accepts command matching the player (entity needs to match player)
	void commitCommandAsPlayer(Command * cmd_p, unsigned long player_p);

	/// @brief add a trigger on the ongoing step
	void commitTrigger(Trigger * trigger_p);

	State const * getBackState() const;
	State const * getBufferState() const;
	State const * getFrontState() const;

	Metrics const &getMetrics() const;

	unsigned long getOngoingStep() const;

	void enableORCA() { _orcaCollision = true; }

	double getTimePerStep() const { return _timePerStep; }

	std::vector<std::list<Command *> *> const & getCommitedCommands() const { return _commitedCommands; }

	///
	/// Serialization part
	///
	/// @brief set up all internal containers to get data from a given number of steps
	void setOngoingStep(unsigned long step_p);

	/// @brief set replay mode : no command can be commited in this mode
	void setReplayMode(bool replayMode_p);

private:
	/// @brief set to true to enable orca Collision
	/// way better performance but less predictible (should not be used in tests)
	bool _orcaCollision {false};

	BufferedState * _backState {nullptr};
	BufferedState * _bufferState {nullptr};
	BufferedState * _frontState {nullptr};

	/// @brief path manager
	PathManager _pathManager;

	/// @brief required time per step
	double const _timePerStep {0.1};
	/// @brief time since last update of step
	double _overTime {0.};
	/// @brief ongoing step, all command received are aggregated into this step
	unsigned long _ongoingStep {0};
	/// @brief last handled step, all commands for steps after this one have not been processed
	unsigned long _lastHandledStep {0};
	/// @brief list of commit commands for every step
	std::vector<std::list<Command *> *> _commitedCommands;
	/// @brief memory handling list (per step of adding in case of rewind)
	std::vector<std::list<AbstractCommand const *>> _commands;
	/// @brief list of all triggers (for memory management)
	std::vector<std::list<Trigger const *>> _triggers;
	/// @brief initial step
	Step _initialStep;
	/// @brief list of compiled steps
	std::list<Step *> _compiledSteps;

	/// @brief the metrics of this controller
	Metrics _metrics;

	/// @brief mutex for multi threading handling
	std::mutex _mutex;

	void updateCommitedCommand();

	/// @brief one shot trigger lists
	std::unordered_map<unsigned long, std::list<Trigger *> > _queuedTriggers;

	void handleTriggers(State const &state_p, Step &step_p, Step const &prevStep_p);

	Step const &getStepBeforeLastCompiledStep() const;

	OrcaManager *_orcaManager {nullptr};

	/// @brief in replay mode all commuited command wont affect the controller
	bool _replayMode {false};
};

} // namespace octopus

#endif
