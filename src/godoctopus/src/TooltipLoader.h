#ifndef __Godoctopus_TooltipLoader__
#define __Godoctopus_TooltipLoader__

#include <string>
#include <vector>


namespace godot {

class Controller;

/// @brief load all tooltip paramters based on the model
std::vector<std::string> loadTooltipParameter(Controller const *controller_p, std::string const &model_p);

} // namespace godot

#endif
