#include "ProductionCommand.h"
#include "Controller.h"

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
    if(a.handle != b.handle)
    {
        return a.handle < b.handle;
    }
    return a.idx < b.idx;
}

}
