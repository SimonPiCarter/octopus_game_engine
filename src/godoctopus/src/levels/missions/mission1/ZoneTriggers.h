#ifndef __Mission_1_ZoneTrigger__
#define __Mission_1_ZoneTrigger__

#include "controller/trigger/Trigger.hh"

#include <unordered_set>

namespace octopus
{
	class Library;
}

class RescueTrigger : public octopus::OneShotTrigger
{
public:
	RescueTrigger(std::list<octopus::Listener *> const &listeners_p, octopus::Library const &lib_p,
		std::unordered_set<octopus::Handle> const &heroHandles_p, unsigned long nbPlayer_p, std::vector<octopus::Handle> const &enemies_p);

	/// @brief trigger by adding the given steps to the step
	/// @param step_p the step to fill with steps from the trigger
	/// @param count_p the current count of the trigger in the same step (this can be used to access data for this occurence of trigger)
	void trigger(octopus::State const &state_p, octopus::Step &step_p, unsigned long, octopus::TriggerData const &) const override;

	octopus::Library const &_lib;
	std::unordered_set<octopus::Handle> const _heroHandles;
	unsigned long const _nbPlayer;
	std::vector<octopus::Handle> const _enemies;
};

class FirstAllyTrigger : public octopus::OneShotTrigger
{
public:
	FirstAllyTrigger(std::list<octopus::Listener *> const &listeners_p, octopus::Library const &lib_p,
		std::unordered_set<octopus::Handle> const &heroHandles_p, unsigned long nbPlayer_p);

	/// @brief trigger by adding the given steps to the step
	/// @param step_p the step to fill with steps from the trigger
	/// @param count_p the current count of the trigger in the same step (this can be used to access data for this occurence of trigger)
	void trigger(octopus::State const &state_p, octopus::Step &step_p, unsigned long, octopus::TriggerData const &) const override;

	octopus::Library const &_lib;
	std::unordered_set<octopus::Handle> const _heroHandles;
	unsigned long const _nbPlayer;
};

class BaseDiscoverTrigger : public octopus::OneShotTrigger
{
public:
	BaseDiscoverTrigger(std::list<octopus::Listener *> const &listeners_p, octopus::Library const &lib_p, unsigned long nbPlayer_p);

	/// @brief trigger by adding the given steps to the step
	/// @param step_p the step to fill with steps from the trigger
	/// @param count_p the current count of the trigger in the same step (this can be used to access data for this occurence of trigger)
	void trigger(octopus::State const &state_p, octopus::Step &step_p, unsigned long, octopus::TriggerData const &) const override;

	octopus::Library const &_lib;
	unsigned long const _nbPlayer;
};

#endif
