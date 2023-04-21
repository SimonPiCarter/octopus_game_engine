#include "Option.h"
#include "Controller.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/input.hpp>

// octopus
#include "state/player/Player.hh"

namespace godot {

Option::~Option()
{}


void Option::update(BuffOption const &option_p)
{
    _desc = "offset : {1}\ncoef : {2}%";
    _model_name = option_p._model.c_str();
    _modifier_name = "";
    _player = option_p._player;

    _params.clear();
    _params.append(octopus::to_string(option_p._buff._type).c_str());
    _params.append(std::to_string(int(option_p._buff._offset)).c_str());
    _params.append(std::to_string(int(option_p._buff._coef*100.)).c_str());

    _stats_name = octopus::to_string(option_p._buff._type).c_str();
}

void Option::update(ModifierOption const &option_p)
{
    _model_name = option_p._model.c_str();
    _player = option_p._player;
    _stats_name = "";

    std::visit([&](auto &&arg) { this->updateFromModifier(arg); }, option_p._mod);
}

void Option::updateFromModifier(octopus::NoModifier const &mod_p)
{
    _modifier_name = "NoModifier";
    _desc = "No modifier";
    _params.clear();
}

void Option::updateFromModifier(octopus::AoEModifier const &mod_p)
{
    _modifier_name = "AoEModifier";
    _desc = "dmg : {0}\nrange : {1}";
    _params.clear();
    _params.append(std::to_string(mod_p._ratio*100.).c_str());
    _params.append(std::to_string(octopus::to_double(mod_p._range)).c_str());
}

void Option::updateFromModifier(octopus::ChainingModifier const &mod_p)
{
    _modifier_name = "ChainingModifier";
    _desc = "Chains : {0}\nDmg : {1}%\nRange : {2}";
    _params.clear();
    _params.append(std::to_string(mod_p._nbOfTicks).c_str());
    _params.append(std::to_string(mod_p._ratio).c_str());
    _params.append(std::to_string(mod_p._range).c_str());
}

void Option::updateFromModifier(octopus::DotModifier const &mod_p)
{
    _modifier_name = "DotModifier";
    _desc = "Ticks : {0}\nTicks/s : {1}\nDmg : {2}";
    _params.clear();
    _params.append(std::to_string(mod_p._nbOfTicks).c_str());
    _params.append(std::to_string(1./(mod_p._tickRate/100.)).c_str());
    _params.append(std::to_string(mod_p._dmg).c_str());
}

void Option::updateFromModifier(octopus::LifeStealModifier const &mod_p)
{
    _modifier_name = "LifeStealModifier";
    _desc = "leach : {0}%";
    _params.clear();
    _params.append(std::to_string(mod_p._ratio*100.).c_str());
}

void Option::set_option(SingleOption const &option_p)
{
    std::visit([&](auto && arg) { update(arg); }, option_p);
}

TypedArray<String> Option::get_params() const
{
    return _params;
}

String Option::get_desc() const
{
    return _desc;
}

String Option::get_stats_name() const
{
    return _stats_name;
}

String Option::get_model_name() const
{
    return _model_name;
}

String Option::get_modifier_name() const
{
    return _modifier_name;
}

int Option::get_player() const
{
    return _player;
}

void Option::_bind_methods()
{
    UtilityFunctions::print("Binding Option methods");

    ClassDB::bind_method(D_METHOD("get_params"), &Option::get_params);
    ClassDB::bind_method(D_METHOD("get_desc"), &Option::get_desc);
    ClassDB::bind_method(D_METHOD("get_stats_name"), &Option::get_stats_name);
    ClassDB::bind_method(D_METHOD("get_model_name"), &Option::get_model_name);
    ClassDB::bind_method(D_METHOD("get_modifier_name"), &Option::get_modifier_name);
    ClassDB::bind_method(D_METHOD("get_player"), &Option::get_player);

    ADD_GROUP("Option", "Option_");
}

}
