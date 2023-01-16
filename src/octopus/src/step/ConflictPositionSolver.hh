#ifndef __CONFLICT_POSITION_SOLVER__
#define __CONFLICT_POSITION_SOLVER__


namespace octopus
{
	class Step;
	class State;

	/// @brief will update all EntityMoveStep position to try to resolve conflict
	/// may require multiple steps to fix
	/// @return true if a correction has been made
	bool updateStepFromConflictPosition(Step &step_p, State const &state_p);
}

#endif
