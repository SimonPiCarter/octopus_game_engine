#ifndef __WindUpStartStep__
#define __WindUpStartStep__

#include "step/custom/InformativeStep.hh"

namespace octopus
{

/// @brief this is used to start wind up animation
class WindUpStartStep : public InformativeStep
{
public:
    WindUpStartStep(Handle const &handle_p) :
		_handle(handle_p)
	{}

	/// @brief store the position of the impact
	Handle const _handle;
};

}

#endif
