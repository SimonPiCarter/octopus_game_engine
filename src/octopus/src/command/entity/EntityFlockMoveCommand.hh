#ifndef __EntityFlockMoveCommand__
#define __EntityFlockMoveCommand__

#include "command/Command.hh"

#include "command/data/MoveData.hh"
#include "state/Handle.hh"
#include "utils/Vector.hh"
#include <list>


namespace octopus
{

/// @brief Basic class that ensure move command for a flock of entities
/// simplified
class EntityFlockMoveCommand : public Command
{
public:
	EntityFlockMoveCommand(std::list<Handle> const &commandHandles_p, Vector const &finalPoint_p, bool attackMove_p=false, bool neverStop_p=false);

	/// @brief register the command into the step
	/// This method will add move command to every commands
	virtual void registerCommand(Step & step_p, State const &state_p) override;

	/// @brief NA (should never be called)
	virtual bool applyCommand(Step &, State const &, CommandData const *, CommandContext &) const override { return true; }

	std::list<Handle>  const &getHandles() const { return _handles; }
	Vector const &getFinalPoint() const { return _finalPoint; }
	bool isAttackMove() const { return _attackMove; }
	bool isNeverStop() const { return _neverStop; }
private:
	std::list<Handle> _handles {};
	Vector _finalPoint {1,1};
	/// @brief if set to true the command will spawn attack move commands
	bool _attackMove {false};
	/// @brief if set to true the command will never stop when not making progress
	bool _neverStop {false};

	FlockInformation _flockInfo;
};

} // namespace octopus


#endif
