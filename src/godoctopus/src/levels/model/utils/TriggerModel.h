#ifndef __Godoctopus_Trigger_Model__
#define __Godoctopus_Trigger_Model__

#include "controller/trigger/Trigger.hh"

#include "ModelTypes.h"

namespace octopus
{
	class Library;
} // namespace octopus


namespace godot
{

class TriggerModel : public octopus::OneShotTrigger
{
public:
	TriggerModel(std::list<octopus::Listener *> const &listeners_p, std::vector<GodotTriggerAction> const &actions_p, octopus::Library const &lib_p, unsigned long playerCount_p);

	void trigger(octopus::State const &, octopus::Step &step_p, unsigned long count_p, octopus::TriggerData const &data_p) const override;

private:
	std::vector<GodotTriggerAction> const _actions;
	octopus::Library const &_lib;
	unsigned long const _playerCount;
};

TriggerModel * newTriggerModel(GodotTrigger const &trigger_p, std::vector<GodotEntity> const &entities_p, octopus::Library const &lib_p, unsigned long playerCount_p);

} // namespace godot

#endif