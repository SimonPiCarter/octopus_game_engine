#include "SimulationVisitor.h"

#include "step/custom/implem/WindUpStartStep.hh"
#include "drawer/entity/SimulationNode.h"

namespace godot
{

void SimulationVisitor::visit(octopus::CustomStep const *steppable_p)
{
	octopus::WindUpStartStep const *windupStart_l = dynamic_cast<octopus::WindUpStartStep const *>(steppable_p);

	if(windupStart_l)
	{
		_simulation->pop_wind_up(windupStart_l->_handle);
	}
}

} // namespace godot