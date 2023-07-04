#include "Option.h"
#include "Controller.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/input.hpp>

#include <sstream>

// octopus
#include "state/player/Player.hh"

namespace
{
/// @brief convert double to string and avoid trailing 0
std::string double_to_string(double const &val_p)
{
    std::stringstream ss_l;
    ss_l << val_p;
    return ss_l.str();
}

} // namespace

namespace godot {

Option::~Option()
{}

void Option::pushOption(octopus::TimedBuff const &buff_p)
{
    std::stringstream ss_l;
    if(buff_p._offset > 0)
    {
        ss_l << "+{1}\n";
    }
    if(buff_p._offset < 0)
    {
        ss_l << "{1}\n";
    }
    if(buff_p._coef > 0)
    {
        ss_l << "+{2}%\n";
    }
    if(buff_p._coef < 0)
    {
        ss_l << "{2}%\n";
    }

    _params.push_back(TypedArray<String>());
    _params.back().append(octopus::to_string(buff_p._type).c_str());
    _params.back().append(std::to_string(octopus::to_int(buff_p._offset)).c_str());
    _params.back().append(std::to_string(octopus::to_int(buff_p._coef*100.)).c_str());

    _stats_name.push_back(octopus::to_string(buff_p._type).c_str());
    _desc.push_back(ss_l.str().c_str());
}

void Option::update(BuffOption const &option_p)
{
    _model_name = option_p._model.c_str();
    _modifier_name = "";
    _player = option_p._player;

    pushOption(option_p._buff);
}

void Option::update(DoubleBuffOption const &option_p)
{
    _model_name = option_p._model.c_str();
    _modifier_name = "";
    _player = option_p._player;

    pushOption(option_p._buff1);
    pushOption(option_p._buff2);
}

void Option::update(ModifierOption const &option_p)
{
    _model_name = option_p._model.c_str();
    _player = option_p._player;
    _stats_name.push_back("");

    std::visit([&](auto &&arg) { this->updateFromModifier(arg); }, option_p._mod);
}

void Option::update(DivinityOption const &option_p)
{
    _modifier_name = "";
    _model_name = "long_square";
    _player = option_p._player;
    _stats_name.push_back("");
    _params.push_back(TypedArray<String>());
    if(option_p._div == "Divinity_1")
    {
        _desc.push_back("up tech I");
    }
    else if(option_p._div == "Divinity_2")
    {
        _desc.push_back("up tech II");
    }
}

void Option::updateFromModifier(octopus::NoModifier const &mod_p)
{
    _modifier_name = "NoModifier";
    _params.push_back(TypedArray<String>());
    _desc.push_back("No modifier");
}

void Option::updateFromModifier(octopus::AoEModifier const &mod_p)
{
    _modifier_name = "AoEModifier";
    _desc.push_back("dmg : {0}%\nrange : {1}");
    _params.push_back(TypedArray<String>());
    _params.back().append(double_to_string(octopus::to_double(mod_p._ratio*100.)).c_str());
    _params.back().append(double_to_string(octopus::to_double(mod_p._range)).c_str());
}

void Option::updateFromModifier(octopus::ChainingModifier const &mod_p)
{
    _modifier_name = "ChainingModifier";
    _desc.push_back("Chains : {0}\nDmg : {1}%\nRange : {2}");
    _params.push_back(TypedArray<String>());
    _params.back().append(std::to_string(mod_p._nbOfTicks).c_str());
    _params.back().append(double_to_string(octopus::to_double(mod_p._ratio*100)).c_str());
    _params.back().append(double_to_string(octopus::to_double(mod_p._range)).c_str());
}

void Option::updateFromModifier(octopus::DotModifier const &mod_p)
{
    _modifier_name = "DotModifier";
    _desc.push_back("Ticks : {0}\nTicks/s : {1}\nDmg : {2}");
    _params.push_back(TypedArray<String>());
    _params.back().append(std::to_string(mod_p._nbOfTicks).c_str());
    _params.back().append(double_to_string(octopus::to_double(1./(mod_p._tickRate/100.))).c_str());
    _params.back().append(double_to_string(octopus::to_double(mod_p._dmg)).c_str());
}

void Option::updateFromModifier(octopus::LifeStealModifier const &mod_p)
{
    _modifier_name = "LifeStealModifier";
    _desc.push_back("leach : {0}%");
    _params.push_back(TypedArray<String>());
    _params.back().append(double_to_string(octopus::to_double(mod_p._ratio*100.)).c_str());
}

void Option::updateFromModifier(octopus::CompositeModifier const &mod_p)
{
    _modifier_name = "CompositeModifier";
    _desc.push_back("NA");
}

void Option::updateFromModifier(octopus::SelfDamageModifier const &mod_p)
{
    _modifier_name = "SelfDamageModifier";
    _desc.push_back("NA");
}

void Option::set_option(SingleOption const &option_p)
{
    _params.clear();
    _desc.clear();
    _stats_name.clear();
    std::visit([&](auto && arg) { update(arg); }, option_p);
}

void Option::set_option(unsigned long player_p, fas::DivinityType const &option_p)
{
    _params.clear();
    _desc.clear();
    _stats_name.clear();

    _modifier_name = "";
    _model_name = "long_square";
    _player = player_p;
    _stats_name.push_back("");
    _params.push_back(TypedArray<String>());

    switch(option_p)
    {
        case fas::DivinityType::AttackSpeed:
            _desc.push_back("anchor AttackSpeed Divinity");
            break;
        case fas::DivinityType::Economic:
            _desc.push_back("anchor Economic Divinity");
            break;
        case fas::DivinityType::Heal:
            _desc.push_back("anchor Heal Divinity");
            break;
        case fas::DivinityType::Lifesteal:
            _desc.push_back("anchor Lifesteal Divinity");
            break;
        case fas::DivinityType::Production:
            _desc.push_back("anchor Production Divinity");
            break;
        case fas::DivinityType::Recycle:
            _desc.push_back("anchor Recycle Divinity");
            break;
    }
}

int Option::get_nb_desc() const
{
    return _desc.size();
}

TypedArray<String> Option::get_params(int i) const
{
    return _params.at(i);
}

String Option::get_desc(int i) const
{
    return _desc.at(i);
}

String Option::get_stats_name(int i) const
{
    return _stats_name.at(i);
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

    ClassDB::bind_method(D_METHOD("get_nb_desc"), &Option::get_nb_desc);
    ClassDB::bind_method(D_METHOD("get_params", "index"), &Option::get_params);
    ClassDB::bind_method(D_METHOD("get_desc", "index"), &Option::get_desc);
    ClassDB::bind_method(D_METHOD("get_stats_name", "index"), &Option::get_stats_name);
    ClassDB::bind_method(D_METHOD("get_model_name"), &Option::get_model_name);
    ClassDB::bind_method(D_METHOD("get_modifier_name"), &Option::get_modifier_name);
    ClassDB::bind_method(D_METHOD("get_player"), &Option::get_player);

    ADD_GROUP("Option", "Option_");
}

}
