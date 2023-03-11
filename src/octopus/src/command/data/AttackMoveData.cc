#include "AttackMoveData.hh"

#include "command/entity/EntityAttackCommand.hh"

namespace octopus
{
AttackMoveData::~AttackMoveData()
{
	delete _subAttackCommand;
}
} // namespace octopus
