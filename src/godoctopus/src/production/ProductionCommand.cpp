#include "ProductionCommand.h"
#include "Controller.h"

#include "command/building/BuildingUnitProductionCommand.hh"
#include "command/CommandQueue.hh"

namespace godot {

bool CommandSorter::operator()(CommandInfo const &a, CommandInfo const &b) const
{
    if(a.data->_progression != b.data->_progression)
    {
        return a.data->_progression > b.data->_progression;
    }
    if(a.posInQueue != b.posInQueue)
    {
        return a.posInQueue < b.posInQueue;
    }
    if(a.cmd->getHandleCommand() != b.cmd->getHandleCommand())
    {
        return a.cmd->getHandleCommand() < b.cmd->getHandleCommand();
    }
    return a.idx < b.idx;
}

}
