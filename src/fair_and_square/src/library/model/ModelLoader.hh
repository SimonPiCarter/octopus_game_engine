#ifndef __Wave_ModelLoader__
#define __Wave_ModelLoader__

namespace octopus
{
    class Library;
} // namespace octopus

void loadModels(octopus::Library &lib_p);
void loadMinimalModels(octopus::Library &lib_p);

#endif