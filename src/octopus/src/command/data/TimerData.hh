#ifndef __TimerData__
#define __TimerData__

#include "command/CommandData.hh"

#include "state/model/entity/UnitModel.hh"

namespace octopus
{

/// @brief This class is aimed at supporting
/// internal timer (step count)
class TimerData : public CommandData
{
public:
	unsigned long _time {0};
};


} // namespace octopus


#endif
