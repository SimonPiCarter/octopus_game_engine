#ifndef __CommandVar__
#define __CommandVar__

#include <variant>

#include "command/building/BuildingBlueprintCommand.hh"
#include "command/building/BuildingCancelCommand.hh"
#include "command/building/BuildingRallyPointCommand.hh"
#include "command/building/BuildingUnitCancelCommand.hh"
#include "command/building/BuildingUnitProductionCommand.hh"
#include "command/data/MoveData.hh"
#include "command/data/ProductionData.hh"
#include "command/data/TimerData.hh"
#include "command/entity/EntityAttackCommand.hh"
#include "command/entity/EntityAttackMoveCommand.hh"
#include "command/entity/EntityBuffCommand.hh"
#include "command/entity/EntityBuildingCommand.hh"
#include "command/entity/EntityMoveCommand.hh"
#include "command/entity/EntityRallyPointCommand.hh"
#include "command/entity/EntityWaitCommand.hh"
#include "flying/eot/DamageOverTime.hh"
#include "command/player/PlayerChoseDivinityCommand.hh"
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
    EntityAttackCommand,
    EntityMoveCommand,
    EntityAttackMoveCommand,
    EntityBuffCommand,
    EntityBuildingCommand,
    EntityRallyPointCommand,
    EntityWaitCommand,
    PlayerChoseDivinityCommand,
    PlayerChoseOptionCommand,
    AreaSpawnerCommand,
    UnitDropCommand,
    UnitHarvestCommand
>;
}

#endif