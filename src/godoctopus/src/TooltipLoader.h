#ifndef __Godoctopus_Model__
#define __Godoctopus_Model__

#include <string>
#include <vector>

namespace octopus
{
	class Controller;
} // namespace octopus


namespace godot {

/// @brief load all tooltip paramters based on the model
std::vector<std::string> loadTooltipParameter(octopus::Controller const *controller_p, std::string const &model_p);

} // namespace godot

#endif
