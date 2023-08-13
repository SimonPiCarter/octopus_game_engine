#ifndef __InformativeStep__
#define __InformativeStep__

#include "CustomStep.hh"

namespace octopus
{
/// @brief this is a place holder step to tag the purely informative steps
/// those steps do no action at all on the state
class InformativeStep : public CustomKeyStep
{
public:
    InformativeStep() : CustomKeyStep("INFORMATIVE") {}
};

}

#endif
