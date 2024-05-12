#include "LMS_TutorialSurvival.h"

// godoctopus
#include "levels/model/utils/EntitySpawner.h"
#include "controller/step/DialogStep.h"
#include "controller/step/ObjectiveStep.h"

// fas
#include "library/model/TimerDamage.hh"
#include "library/model/AnchorTrigger.hh"

// octopus
#include "controller/trigger/Listener.hh"
#include "controller/trigger/Trigger.hh"
#include "library/Library.hh"
#include "state/entity/Entity.hh"
#include "state/entity/Unit.hh"
#include "state/State.hh"
#include "state/vision/VisionHandler.hh"
#include "step/command/CommandQueueStep.hh"
#include "step/command/flying/FlyingCommandSpawnStep.hh"
#include "step/entity/spawn/UnitSpawnStep.hh"
#include "step/entity/spawn/UnitSpawnStep.hh"
#include "step/player/PlayerAddOptionStep.hh"
#include "step/player/PlayerSpendResourceStep.hh"
#include "step/Step.hh"
#include "step/trigger/TriggerSpawn.hh"
#include "step/vision/VisionChangeStep.hh"

using namespace octopus;

namespace godot {

void specific_tutorial_survival(std::list<octopus::Steppable *> &steps_r, octopus::Library const &lib_p, std::vector<GodotEntityInfo> const &entityInfo_p)
{
	// add resources
	std::map<std::string, Fixed> mapRes_l;
	mapRes_l["bloc"] = -400;
	mapRes_l["ether"] = -400;
	mapRes_l["Anchor"] = -240;
	steps_r.push_back(new PlayerSpendResourceStep(2, mapRes_l));

	// add anchor consumption
	std::vector<unsigned long> handles_l = getHandles(entityInfo_p, 2, "gate");
	for(unsigned long gateHandle_l : handles_l)
	{
		steps_r.push_back(new FlyingCommandSpawnStep(new TimerDamage(100, 0, 2, "Anchor", Handle(gateHandle_l))));
	}

	// spawn wave after 30 seconds
	steps_r.push_back(new TriggerSpawn(new OneShotFunctionTrigger(
			{new ListenerStepCount(3000)},
			[&lib_p](State const &state_p, Step &step_p, unsigned long, TriggerData const &)
	 		{
				step_p.addSteppable(new DialogStep("tuto_survival_wave_spawned"));
				// handles to be used if last to trigger end of wave
				std::unordered_set<Handle> handles_l;
				// spawn the given unit count
				for(int i = 0 ; i < 6 ; ++ i)
				{
					Unit unit_l(Vector(99, 140), false, lib_p.getUnitModel(i < 3 ? "circle" : "triangle"));
					unit_l._player = 1;
					Handle handle_l = getNextHandle(step_p, state_p);
					step_p.addSteppable(new UnitSpawnStep(handle_l, unit_l));
					step_p.addSteppable(new CommandSpawnStep(new EntityAttackMoveCommand(handle_l, handle_l, Vector(99, 90), 0, {Vector(99, 90)}, true, true )));
					handles_l.insert(handle_l);
				}

				step_p.addSteppable(new TriggerSpawn(new OneShotFunctionTrigger(
					{new ListenerEntityDied(handles_l)},
					[&lib_p](State const &state_p, Step &step_p, unsigned long, TriggerData const &)
					{
						step_p.addSteppable(new DialogStep("tuto_survival_wave_cleaned"));
						auto optionsGenerator_l = [](const octopus::State &state_p) {
							return generateOpeningOptions(
										std::string("0"),
										42,
										{},
										2,
										state_p);
						};

						step_p.addSteppable(new PlayerAddOptionStep(2, "0", new BuffGenerator("0", optionsGenerator_l, lib_p)));
						step_p.addSteppable(new CompleteObjectiveStep("tuto_survival_obj_defend_wave", true));
						step_p.addSteppable(new AddObjectiveStep("tuto_survival_obj_explore", "tuto_survival_obj_explore", 0, true));
					}
				)));
			}
	)));
}

}

