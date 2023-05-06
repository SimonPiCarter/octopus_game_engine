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
    std::visit([&](auto && arg) { arg.applyCommand(step_p, state_p, data_p, pathManager_p); }, var_p);
    return isDone_l;
}
CommandData * newData(CommandVar const &var_p)
{
    CommandData * data_l = nullptr;
    std::visit([&](auto && arg) { data_l = arg.newData(); }, var_p);
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
    check_l = true;
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

} // namespace octopus
