#ifndef __WindUpStartStep__
#define __WindUpStartStep__

#include "step/custom/InformativeStep.hh"

namespace octopus
{

/// @brief this is used to start wind up animation
class WindUpStartStep : public InformativeStep
{
public:
    WindUpStartStep(Handle const &handle_p, Handle const &target_p) :
		_handle(handle_p), _target(target_p)
	{}

	/// @brief source of the wind up
	Handle const _handle;
	/// @brief target of the wind up
	Handle const _target;
};

}

#endif
