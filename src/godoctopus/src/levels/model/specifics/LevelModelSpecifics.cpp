#include "LevelModelSpecifics.h"
#include "LMS_TutorialSurvival.h"

namespace godot
{

void add_specifics(std::string const &specific_p, std::list<octopus::Steppable *> &steps_r, octopus::Library const &lib_p,
	unsigned long playerCount_p, std::vector<GodotEntityInfo> const &entityInfo_p)
{
	if(specific_p == "") { return; }

	if(specific_p == "tutorial_survival")
	{
		specific_tutorial_survival(steps_r, lib_p, entityInfo_p);
	}
}

}
