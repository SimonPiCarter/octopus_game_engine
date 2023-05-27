#include "CommandQueue.hh"
#include "command/Command.hh"

#include <stdexcept>
#include <string>

namespace octopus
{
unsigned long long CommandQueue::getStepOfRegistation() const
{
    if(_commandQueue.empty())
    {
        return 0;
    }
    Command const * cmd_l = getCommandFromVar(_commandQueue.front()._var);
    return cmd_l->getStepOfRegistration();
}

} /// octopus
