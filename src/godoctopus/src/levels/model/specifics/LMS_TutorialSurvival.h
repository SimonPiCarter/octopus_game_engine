#ifndef __Godoctopus_LMS_TutorialSurvival__
#define __Godoctopus_LMS_TutorialSurvival__

#include <list>
#include <vector>

namespace octopus
{
    class Library;
    class Steppable;
} // namespace octopus

namespace godot {

struct GodotEntityInfo;

void specific_tutorial_survival(std::list<octopus::Steppable *> &steps_r, octopus::Library const &lib_p, std::vector<GodotEntityInfo> const &entityInfo_p);

}

#endif
