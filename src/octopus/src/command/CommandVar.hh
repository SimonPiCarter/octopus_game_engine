#ifndef __CommandVar__
#define __CommandVar__

#include <variant>

#include "command/building/BuildingBlueprintCommand.hh"
#include "command/building/BuildingCancelCommand.hh"
#include "command/building/BuildingRallyPointCommand.hh"
#include "command/building/BuildingUnitCancelCommand.hh"
#include "command/building/BuildingUnitProductionCommand.hh"
#include "command/building/BuildingUpgradeProductionCommand.hh"
#include "command/data/MoveData.hh"
#include "command/data/ProductionData.hh"
#include "command/data/TimerData.hh"
#include "command/debug/DebugCommand.hh"
#include "command/entity/EntityAbilityCommand.hh"
#include "command/entity/EntityAttackCommand.hh"
#include "command/entity/EntityAttackMoveCommand.hh"
#include "command/entity/EntityBuffCommand.hh"
#include "command/entity/EntityBuildingCommand.hh"
#include "command/entity/EntityMoveCommand.hh"
#include "command/entity/EntityRallyPointCommand.hh"
#include "command/entity/EntityWaitCommand.hh"
#include "command/entity/EntityFlockMoveCommand.hh"
#include "flying/eot/DamageOverTime.hh"
#include "command/player/PlayerChoseOptionCommand.hh"
#include "command/spawner/AreaSpawnerCommand.hh"
#include "command/unit/UnitDropCommand.hh"
#include "command/unit/UnitHarvestCommand.hh"

namespace octopus
{
using CommandVar = std::variant<
    BuildingBlueprintCommand,
    BuildingCancelCommand,
    BuildingRallyPointCommand,
    BuildingUnitCancelCommand,
    BuildingUnitProductionCommand,
    BuildingUpgradeProductionCommand,
	DebugCommand,
    EntityAbilityCommand,
    EntityAttackCommand,
    EntityMoveCommand,
    EntityAttackMoveCommand,
    EntityBuffCommand,
    EntityBuildingCommand,
    EntityRallyPointCommand,
    EntityWaitCommand,
    PlayerChoseOptionCommand,
    AreaSpawnerCommand,
    UnitDropCommand,
    UnitHarvestCommand,
    EntityFlockMoveCommand
>;

/// @brief register the command into the step
/// This method is responsible for
/// handling cost of command and spawning command in step
/// and checking legality
void registerCommand(CommandVar const &var_p, Step & step_p, State const &state_p);

/// @brief compile command or info into the step
/// @return true if command is over
bool applyCommand(CommandVar const &var_p, Step & step_p, State const &state_p, CommandData const * data_p, PathManager &pathManager_p);

/// @brief create data supporting the command actions
CommandData * newData(CommandVar const &var_p);

/// @brief get data supporting the command actions
CommandData const * getData(CommandVar const &var_p);

/// @brief get data supporting the command actions
CommandData * getData(CommandVar &var_p);

/// @brief compile clean up for command (may reset some state values for example)
/// is useful when command are interupted
/// @param step_p push actions in this step
/// @param state_p the state to use to get information
/// @param data_p date of the command
/// @note this will only be used on started commands when going to next command
void cleanUp(CommandVar const &var_p, Step & step_p, State const &state_p, CommandData const * data_p);

/// @brief Check if command is valid for given player
/// @param state_p the state in case we need to get some info
/// @param player_p the player supposed to commit the command
/// @return true if the command can be commited for said player
bool checkPlayer(CommandVar const &var_p, State const &state_p, unsigned long player_p);

void setQueued(CommandVar const &var_p, bool queued_p);
bool isQueued(CommandVar const &var_p);
Handle const &getHandleCommand(CommandVar const &var_p);

CommandVar getVarFromCommand(Command *cmd_p);

Command const *getCommandFromVar(CommandVar const &var_p);

} // octopus

#endif