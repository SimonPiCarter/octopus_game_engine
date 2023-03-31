#include "AnchorEffectTrigger.hh"

#include "library/Library.hh"
#include "state/entity/Entity.hh"
#include "state/model/entity/BuildingModel.hh"

#include "core/game/loader/visitor/FASStepVisitor.hh"

namespace fas
{

AnchorEffectTrigger::AnchorEffectTrigger(octopus::Library const &lib_p, FASStepVisitor &vis_p) :
    octopus::OnEachTrigger(new octopus::ListenerEntityModelFinished(&lib_p.getBuildingModel("anchor"), 0)),
    _vis(vis_p)
{}

void AnchorEffectTrigger::trigger(octopus::State const &, octopus::Step &, unsigned long count_p, octopus::TriggerData const &data_p) const
{
    octopus::ListenerEntityData const * entData_l = dynamic_cast<octopus::ListenerEntityData const *>(data_p._listenerData.at(0));
    octopus::Entity const * ent_l = entData_l->_entities.at(count_p);

    _vis.addAnchorEffectPos(ent_l->_pos);
}

} // namespace fas
