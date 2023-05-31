#include "CommandVar.hh"

namespace octopus
{

void registerCommand(CommandVar &var_p, Step & step_p, State const &state_p)
{
    std::visit([&](auto && arg) { arg.registerCommand(step_p, state_p); }, var_p);
}
bool applyCommand(CommandVar const &var_p, Step & step_p, State const &state_p, CommandData const * data_p, PathManager &pathManager_p)
{
    bool isDone_l = false;
    std::visit([&](auto && arg) { isDone_l = arg.applyCommand(step_p, state_p, data_p, pathManager_p); }, var_p);
    return isDone_l;
}
CommandData * newData(CommandVar const &var_p)
{
    CommandData * data_l = nullptr;
    std::visit([&](auto && arg) { data_l = arg.newData(); }, var_p);
    return data_l;
}
CommandData const * getData(CommandVar const &var_p)
{
    CommandData const * data_l = nullptr;
    std::visit([&](auto && arg) { data_l = arg.getData(); }, var_p);
    return data_l;
}
CommandData * getData(CommandVar &var_p)
{
    CommandData * data_l = nullptr;
    std::visit([&](auto && arg) { data_l = arg.getData(); }, var_p);
    return data_l;
}
void cleanUp(CommandVar const &var_p, Step & step_p, State const &state_p, CommandData const * data_p)
{
    std::visit([&](auto && arg) { arg.cleanUp(step_p, state_p, data_p); }, var_p);
}
bool checkPlayer(CommandVar const &var_p, State const &state_p, unsigned long player_p)
{
    bool check_l = false;
    std::visit([&](auto && arg) { check_l = arg.checkPlayer(state_p, player_p); }, var_p);
    return check_l;
}
void setQueued(CommandVar &var_p, bool queued_p)
{
    std::visit([&](auto && arg) { arg.setQueued(queued_p); }, var_p);
}
bool isQueued(CommandVar const &var_p)
{
    bool check_l = false;
    std::visit([&](auto && arg) { check_l = arg.isQueued(); }, var_p);
    return check_l;
}
const Handle &getHandleCommand(CommandVar const &var_p)
{
    Handle const *handle_l = nullptr;
    std::visit([&](auto && arg) { handle_l = &arg.getHandleCommand(); }, var_p);
    return *handle_l;
}

CommandVar getVarFromCommand(Command *cmd_p)
{
    {
        BuildingBlueprintCommand * typped_l = dynamic_cast<BuildingBlueprintCommand *>(cmd_p);
        if(typped_l)
        {
            return *typped_l;
        }
    }
    {
        BuildingCancelCommand * typped_l = dynamic_cast<BuildingCancelCommand *>(cmd_p);
        if(typped_l)
        {
            return *typped_l;
        }
    }
    {
        BuildingRallyPointCommand * typped_l = dynamic_cast<BuildingRallyPointCommand *>(cmd_p);
        if(typped_l)
        {
            return *typped_l;
        }
    }
    {
        BuildingUnitCancelCommand * typped_l = dynamic_cast<BuildingUnitCancelCommand *>(cmd_p);
        if(typped_l)
        {
            return *typped_l;
        }
    }
    {
        BuildingUnitProductionCommand * typped_l = dynamic_cast<BuildingUnitProductionCommand *>(cmd_p);
        if(typped_l)
        {
            return *typped_l;
        }
    }
    {
        BuildingUpgradeProductionCommand * typped_l = dynamic_cast<BuildingUpgradeProductionCommand *>(cmd_p);
        if(typped_l)
        {
            return *typped_l;
        }
    }
    {
        EntityAttackCommand * typped_l = dynamic_cast<EntityAttackCommand *>(cmd_p);
        if(typped_l)
        {
            return *typped_l;
        }
    }
    {
        EntityMoveCommand * typped_l = dynamic_cast<EntityMoveCommand *>(cmd_p);
        if(typped_l)
        {
            return *typped_l;
        }
    }
    {
        EntityAttackMoveCommand * typped_l = dynamic_cast<EntityAttackMoveCommand *>(cmd_p);
        if(typped_l)
        {
            return *typped_l;
        }
    }
    {
        EntityBuffCommand * typped_l = dynamic_cast<EntityBuffCommand *>(cmd_p);
        if(typped_l)
        {
            return *typped_l;
        }
    }
    {
        EntityBuildingCommand * typped_l = dynamic_cast<EntityBuildingCommand *>(cmd_p);
        if(typped_l)
        {
            return *typped_l;
        }
    }
    {
        EntityRallyPointCommand * typped_l = dynamic_cast<EntityRallyPointCommand *>(cmd_p);
        if(typped_l)
        {
            return *typped_l;
        }
    }
    {
        EntityWaitCommand * typped_l = dynamic_cast<EntityWaitCommand *>(cmd_p);
        if(typped_l)
        {
            return *typped_l;
        }
    }
    {
        PlayerChoseOptionCommand * typped_l = dynamic_cast<PlayerChoseOptionCommand *>(cmd_p);
        if(typped_l)
        {
            return *typped_l;
        }
    }
    {
        AreaSpawnerCommand * typped_l = dynamic_cast<AreaSpawnerCommand *>(cmd_p);
        if(typped_l)
        {
            return *typped_l;
        }
    }
    {
        UnitDropCommand * typped_l = dynamic_cast<UnitDropCommand *>(cmd_p);
        if(typped_l)
        {
            return *typped_l;
        }
    }
    {
        UnitHarvestCommand * typped_l = dynamic_cast<UnitHarvestCommand *>(cmd_p);
        if(typped_l)
        {
            return *typped_l;
        }
    }
    {
        EntityFlockMoveCommand * typped_l = dynamic_cast<EntityFlockMoveCommand *>(cmd_p);
        if(typped_l)
        {
            return *typped_l;
        }
    }

    throw std::logic_error("Could not convert command pointer to CommandVar (missing cast)");
}

Command const *getCommandFromVar(CommandVar const &var_p)
{
    Command const *cmd_l = nullptr;
    std::visit([&](auto && arg) { cmd_l = &arg; }, var_p);
    return cmd_l;
}

} // namespace octopus
