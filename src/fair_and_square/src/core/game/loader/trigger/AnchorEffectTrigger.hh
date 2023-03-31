#ifndef __AnchorEffectTrigger__
#define __AnchorEffectTrigger__

#include <random>

#include "controller/trigger/Trigger.hh"
#include "controller/trigger/Listener.hh"

namespace octopus
{
class Library;
} // namespace octopus

namespace cuttlefish
{
class Window;
class World;
} // namespace cuttlefish

namespace fas
{
class FASStepVisitor;

class AnchorEffectTrigger : public octopus::OnEachTrigger
{
public:
	AnchorEffectTrigger(octopus::Library const &, FASStepVisitor &vis_p);
	virtual void trigger(octopus::State const &, octopus::Step &step_p, unsigned long, octopus::TriggerData const &data_p) const override;

private:
	FASStepVisitor &_vis;
};

} // namespace fas
#endif
