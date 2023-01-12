#ifndef __Command__
#define __Command__

namespace game_engine
{
class Step;

class Command
{
public:
	/// @brief register new commands or info into the step
	/// @return true if command is over
	virtual bool registerCommand(Step & step_p) = 0;
};

} // namespace game_engine


#endif
