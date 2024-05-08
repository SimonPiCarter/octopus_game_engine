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

std::string desc_buff(octopus::TimedBuff const &buff_p)
{
	switch(buff_p._type)
	{
		case octopus::TyppedBuff::Type::Speed:
			return "speed";
		case octopus::TyppedBuff::Type::FullReload:
			return "reload time";
		case octopus::TyppedBuff::Type::Damage:
			return "damage";
		case octopus::TyppedBuff::Type::Heal:
			return "heal";
		case octopus::TyppedBuff::Type::Armor:
			return "armor";
		case octopus::TyppedBuff::Type::HpMax:
			return "max hp";
		case octopus::TyppedBuff::Type::HpRegeneration:
			return "hp regeneration";
		case octopus::TyppedBuff::Type::Production:
			return "production";
		case octopus::TyppedBuff::Type::Harvest:
			return "harvest quantity";
		case octopus::TyppedBuff::Type::DamageReturn:
			return "damage return";
	}
	throw std::logic_error("missing desc_buff implem");
}

bool isBuff(octopus::Fixed const &delta_p, octopus::TyppedBuff::Type type_p)
{
    if(type_p == octopus::TyppedBuff::Type::FullReload)
    {
        return delta_p < 0;
    }
    return delta_p > 0;
}

bool isOffsetPercent(octopus::TyppedBuff::Type type_p)
{
    if(type_p == octopus::TyppedBuff::Type::DamageReturn)
    {
        return true;
    }
    return false;
}

void Option::pushOption(octopus::AnyBuff const &buff_p)
{
    if(std::holds_alternative<octopus::TimedBuff>(buff_p))
    {
        octopus::TimedBuff const &tBuff_l = std::get<octopus::TimedBuff>(buff_p);
        std::stringstream ss_l;
        std::string color_l = isBuff(tBuff_l._offset, tBuff_l._type) ? "green" : "red";
        octopus::Fixed offset_l = isOffsetPercent(tBuff_l._type) ? tBuff_l._offset*100 : tBuff_l._offset;
        std::string percent_l = isOffsetPercent(tBuff_l._type) ? "%" : "";
        if(offset_l > 0)
        {
            ss_l << "Increase "<<desc_buff(tBuff_l)<<" by [color="<<color_l<<"]{1}[/color]"<<percent_l<<".\n";
        }
        if(offset_l < 0)
        {
            ss_l << "Decrease "<<desc_buff(tBuff_l)<<" by [color="<<color_l<<"]{1}[/color]"<<percent_l<<".\n";
        }
        color_l = isBuff(tBuff_l._coef, tBuff_l._type) ? "green" : "red";
        if(tBuff_l._coef > 0)
        {
            ss_l << "Increase "<<desc_buff(tBuff_l)<<" by [color="<<color_l<<"]{2}[/color]%.\n";
        }
        if(tBuff_l._coef < 0)
        {
            ss_l << "Decrease "<<desc_buff(tBuff_l)<<" by [color="<<color_l<<"]{2}[/color]%.\n";
        }

        _params.push_back(TypedArray<String>());
        _params.back().append(octopus::to_string(tBuff_l._type).c_str());
        _params.back().append(std::to_string(std::abs(octopus::to_int(offset_l))).c_str());
        _params.back().append(std::to_string(std::abs(octopus::to_int(tBuff_l._coef*100.))).c_str());

        _stats_name.push_back(octopus::to_string(tBuff_l._type).c_str());
        _desc.push_back(ss_l.str().c_str());
    }
    else if(std::holds_alternative<octopus::ModifierAoEBuff>(buff_p))
    {
        octopus::ModifierAoEBuff const &aoeBuff_l = std::get<octopus::ModifierAoEBuff>(buff_p);
        std::stringstream ss_l;
        if(aoeBuff_l._deltaRatio > 0)
        {
            ss_l << "Increase damage ratio by [color=green]{1}[/color]%.\n";
        }
        if(aoeBuff_l._deltaRatio < 0)
        {
            ss_l << "Decrease damage ratio by [color=red]{1}[/color]%.\n";
        }
        if(aoeBuff_l._deltaRange > 0)
        {
            ss_l << "Increase damage range by [color=green]{2}[/color].\n";
        }
        if(aoeBuff_l._deltaRange < 0)
        {
            ss_l << "Decrease damage range by [color=red]{2}[/color].\n";
        }

        _params.push_back(TypedArray<String>());
        _params.back().append("ModifierAoEBuff");
        _params.back().append(std::to_string(std::abs(octopus::to_int(aoeBuff_l._deltaRatio*100))).c_str());
        _params.back().append(std::to_string(std::abs(octopus::to_int(aoeBuff_l._deltaRange))).c_str());

        _stats_name.push_back("ModifierAoEBuff");
        _desc.push_back(ss_l.str().c_str());
    }
}

void Option::update(NoOption const &)
{}

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
	set_option(option_p._player, option_p._div);
}

void Option::update(SurvivalOption const &option_p)
{
    _params.clear();
    _desc.clear();
    _stats_name.clear();

    _modifier_name = "";
    _model_name = fas::survivalSpecialTypeName(option_p._type).c_str();
    _player = option_p._player;
    _stats_name.push_back("");
    _params.push_back(TypedArray<String>());
	_desc.push_back((fas::survivalSpecialTypeName(option_p._type)+"_tooltip").c_str());
}


// Create custom split() function.
std::vector<std::string> customSplit(std::string str, char separator)
{
	std::vector<std::string> strings;
	using namespace std;
    int startIndex = 0, endIndex = 0;
    for (int i = 0; i <= str.size(); i++) {

        // If we reached the end of the word or the end of the input.
        if (str[i] == separator || i == str.size()) {
            endIndex = i;
            string temp;
            temp.append(str, startIndex, endIndex - startIndex);
            strings.push_back(temp);
            startIndex = endIndex + 1;
        }
    }
	return strings;
}


void Option::update(UpgradeOption const &option_p)
{
    _params.clear();
    _desc.clear();
    _stats_name.clear();

	if (option_p._upgrade.find("upgrade_unit") != std::string::npos)
	{
		std::vector<std::string> strings_l = customSplit(option_p._upgrade, '.');
		if(strings_l.size() > 2)
		{
			_modifier_name = strings_l[1].c_str();
			_model_name = strings_l[2].c_str();
		}
		else
		{
			_modifier_name = "";
			_model_name = option_p._upgrade.c_str();
		}
	}
	else
	{
		_modifier_name = "";
		_model_name = option_p._upgrade.c_str();
	}

    _player = option_p._player;
    _stats_name.push_back("");
    _params.push_back(TypedArray<String>());
	_desc.push_back((option_p._upgrade+"_tooltip").c_str());
}

void Option::update(SpawnUnitOption const &option_p)
{
    _params.clear();
    _desc.clear();
    _stats_name.clear();

    _modifier_name = "";
    _model_name = option_p._model.c_str();
    _player = option_p._player;
    _stats_name.push_back("");
    _params.push_back(TypedArray<String>());
	_desc.push_back((option_p._model+"_tooltip").c_str());
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
    _model_name = fas::divinityUpgradeName(option_p).c_str();
    _player = player_p;
    _stats_name.push_back("");
    _params.push_back(TypedArray<String>());
	_desc.push_back((fas::divinityUpgradeName(option_p)+"_tooltip").c_str());
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

int Option::get_rarity() const
{
    return _rarity;
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
    ClassDB::bind_method(D_METHOD("get_rarity"), &Option::get_rarity);

    ADD_GROUP("Option", "Option_");
}

}
