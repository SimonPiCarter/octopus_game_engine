#ifndef __AttackMoveData__
#define __AttackMoveData__

#include "AttackData.hh"
#include "command/entity/EntityAttackCommand.hh"
#include "command/entity/EntityBuffCommand.hh"

namespace octopus
{

/// @brief This struct is aimed at supporting
/// attack, move, attack move and patrol commands
struct AttackMoveData : public AttackData
{
	AttackMoveData() {}
	AttackMoveData(Handle const &target_p, Vector const &finalPoint_p, unsigned long gridStatus_p, std::list<Vector> const &waypoints_p)
		: AttackData(target_p, finalPoint_p, gridStatus_p, waypoints_p) {}
	// used in attackmove
	bool _hasSubAttackCommand {false};
	EntityAttackCommand _subAttackCommand;
	// used for buff command
	bool _hasSubBuffCommand {false};
	EntityBuffCommand _subBuffCommand;

	Vector _positionFromAttack;
};

} // namespace octopus


#endif
