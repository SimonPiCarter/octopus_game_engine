#include "Controller.h"
#include "Entity.h"
#include "option/Option.h"
#include "Model.h"
#include "ModelUpgrade.h"
#include "editor/GridBuild.h"
#include "levels/model/LevelModel.h"
#include "levels/wave/Wave.h"
#include "file/FileHeader.h"
#include "drawer/Drawer.h"
#include "drawer/TextureDrawer.h"
#include "drawer/MultiTextureDrawer.h"
#include "drawer/StaticPropsDrawer.h"

#include <gdextension_interface.h>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/godot.hpp>


void initialize_godoctopus_module(godot::ModuleInitializationLevel p_level) {
  if (p_level != godot::ModuleInitializationLevel::MODULE_INITIALIZATION_LEVEL_SCENE) {
    return;
  }

  // REGISTER CLASSES HERE LATER
    godot::ClassDB::register_class<godot::Controller>();
    godot::ClassDB::register_class<godot::EntityHandle>();
    godot::ClassDB::register_class<godot::Buff>();
    godot::ClassDB::register_class<godot::Entity>();
    godot::ClassDB::register_class<godot::Option>();
    godot::ClassDB::register_class<godot::Model>();
    godot::ClassDB::register_class<godot::ModelView>();
    godot::ClassDB::register_class<godot::LevelModel>();
    godot::ClassDB::register_class<godot::ModelUpgrade>();
    godot::ClassDB::register_class<godot::WaveContent>();
    godot::ClassDB::register_class<godot::Wave>();
    godot::ClassDB::register_class<godot::WavePool>();
    godot::ClassDB::register_class<godot::WavePattern>();
    godot::ClassDB::register_class<godot::FileHeader>();
    godot::ClassDB::register_class<godot::Drawer>();
    godot::ClassDB::register_class<godot::TextureDrawer>();
    godot::ClassDB::register_class<godot::MultiTextureDrawer>();
    godot::ClassDB::register_class<godot::StaticPropsDrawer>();
    godot::ClassDB::register_class<godot::GridBuild>();
}

void uninitialize_godoctopus_module(godot::ModuleInitializationLevel p_level) {
  if (p_level != godot::ModuleInitializationLevel::MODULE_INITIALIZATION_LEVEL_SCENE) {
    return;
  }
  // DO NOTHING
}

extern "C" {
// Initialization.
GDExtensionBool GDE_EXPORT godoctopus_library_init(GDExtensionInterfaceGetProcAddress p_get_proc_address, const GDExtensionClassLibraryPtr p_library, GDExtensionInitialization *r_initialization) {
    godot::GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);

    init_obj.register_initializer(initialize_godoctopus_module);
    init_obj.register_terminator(initialize_godoctopus_module);
    init_obj.set_minimum_library_initialization_level(godot::ModuleInitializationLevel::MODULE_INITIALIZATION_LEVEL_SCENE);

    return init_obj.init();
}
}
