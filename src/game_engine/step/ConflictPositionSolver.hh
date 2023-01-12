#ifndef __CONFLICT_POSITION_SOLVER__
#define __CONFLICT_POSITION_SOLVER__


namespace game_engine
{
	class Step;

	/// @brief will update all EntityStep position to try to resolve conflict
	/// may require multiple steps to fix
	void updateStepFromConflictPosition(Step &step_p);
}

#endif
