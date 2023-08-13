#ifndef __ImpactStep__
#define __ImpactStep__

#include "step/custom/InformativeStep.hh"

namespace octopus
{

/// @brief This step will store the handle of an entity that got attacked
/// by another (as the main target)
/// stores the model id of the attacker
/// @note this is used to display impact
class ImpactStep : public InformativeStep
{
public:
    ImpactStep(std::string const &modelId_p, Vector const &pos_p) :
		_model(modelId_p),
		_pos(pos_p)
	{}

	/// @brief the model of the attacker
	/// @note store reference from model id (reference should always be valid)
	std::string const &_model;
	/// @brief store the position of the impact
	Vector const _pos;
};

}

#endif
