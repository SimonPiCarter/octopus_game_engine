#include "ObjectiveStep.h"

#include "Controller.h"

namespace godot
{

void AddObjectiveStep::add_signals(Controller & controller) const
{
	if(_main)
	{
		controller.emit_signal("add_main_objective", String(_key.c_str()), String(_text.c_str()), _count);
	}
	else
	{
		controller.emit_signal("add_secondary_objective", String(_key.c_str()), String(_text.c_str()), _count);
	}
}

void RemoveObjectiveStep::add_signals(Controller & controller) const
{
	controller.emit_signal("remove_objective", String(_key.c_str()));
}

void FailObjectiveStep::add_signals(Controller & controller) const
{
	controller.emit_signal("set_fail_objective", String(_key.c_str()), _fail);
}

void CompleteObjectiveStep::add_signals(Controller & controller) const
{
	controller.emit_signal("set_complete_objective", String(_key.c_str()), _complete);
}

void IncrementObjectiveStep::add_signals(Controller & controller) const
{
	if(_increment)
	{
		controller.emit_signal("increment_objective", String(_key.c_str()));
	}
	else
	{
		controller.emit_signal("decrement_objective", String(_key.c_str()));
	}
}

}
