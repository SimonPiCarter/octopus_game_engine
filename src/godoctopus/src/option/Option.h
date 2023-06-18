#ifndef __Godoctopus_Option__
#define __Godoctopus_Option__

#include <thread>
#include <cstdint>
#include <godot_cpp/godot.hpp>
#include <godot_cpp/classes/node2d.hpp>

#include "library/model/bonus/BuffGenerators.hh"

namespace godot {

class Controller;

class Option : public Node {
    GDCLASS(Option, Node)

public:
    ~Option();

    void pushOption(octopus::TimedBuff const &buff_p);

    void update(BuffOption const &option_p);
    void update(DoubleBuffOption const &option_p);
    void update(ModifierOption const &option_p);
    void update(DivinityOption const &option_p);
    void updateFromModifier(octopus::NoModifier const &mod_p);
    void updateFromModifier(octopus::AoEModifier const &mod_p);
    void updateFromModifier(octopus::ChainingModifier const &mod_p);
    void updateFromModifier(octopus::DotModifier const &mod_p);
    void updateFromModifier(octopus::LifeStealModifier const &mod_p);
    void updateFromModifier(octopus::CompositeModifier const &mod_p);
    void updateFromModifier(octopus::SelfDamageModifier const &mod_p);

    void set_option(SingleOption const &option_p);

    // Will be called by Godot when the class is registered
    // Use this to add properties to your class
    static void _bind_methods();

    int get_nb_desc() const;
    TypedArray<String> get_params(int i) const;
    String get_desc(int i) const;
    String get_stats_name(int i) const;
    String get_modifier_name() const;
    String get_model_name() const;
    int get_player() const;

private:
    std::vector<TypedArray<String>> _params;
    std::vector<String> _desc;
    std::vector<String> _stats_name;
    String _modifier_name = "modifier_name";
    String _model_name = "model_name";
    int _player = 0;
};

}

#endif //__Godoctopus_Option__
