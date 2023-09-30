#ifndef __Mission_SpawnTrigger__
#define __Mission_SpawnTrigger__

#include "controller/trigger/Trigger.hh"
#include "step/Step.hh"

template<typename Entity_t>
octopus::Steppable * makeSpawnStep(octopus::Handle const &handle_p, Entity_t const &ent_p);

template<typename Entity_t>
class SpawnerTrigger : public octopus::OneShotTrigger
{
public:
	SpawnerTrigger(std::list<octopus::Listener *> const &listeners_p, std::vector<Entity_t> const &entities_p) :
		OneShotTrigger(listeners_p),
		 _entities(entities_p)
	{}

	/// @brief trigger by adding the given steps to the step
	/// @param step_p the step to fill with steps from the trigger
	/// @param count_p the current count of the trigger in the same step (this can be used to access data for this occurence of trigger)
	void trigger(octopus::State const &state_p, octopus::Step &step_p, unsigned long, octopus::TriggerData const &) const override
	{
		for(Entity_t const &ent_l : _entities)
		{
			step_p.addSteppable(makeSpawnStep(getNextHandle(step_p, state_p), ent_l));
		}
	}

private:
	std::vector<Entity_t> const _entities;
};

#endif
