#include "LangEntries.hh"

namespace fas
{
LangEntries *LangEntries::_instance = nullptr;

LangEntries * LangEntries::GetInstance()
{
    if(!_instance)
    {
        _instance = new LangEntries();
    }
    return _instance;
}

std::string const &LangEntries::getEntry(std::string const &key_p) const
{
    return _mapEntries.at(key_p);
}

void LangEntries::loadFromFile(std::string const &filename_p)
{
    // NA for now
}

LangEntries::LangEntries()
{
    // hard coded entry for now
    _mapEntries["choiceSubPanel"] = "Option $$ type : $type_val $$ offset : $offset_val $$ coef : $coef_val $$ model : $model_val";
    _mapEntries["AoEModifierChoice"] = "AoEModifier $$ ratio : $ratio $$ range : $range";
    _mapEntries["ChainingModifierChoice"] = "ChainingModifier $$ nb of chains : $nbOfTicks $$ ratio : $ratio $$ range : $range";
    _mapEntries["DotModifierChoice"] = "DotModifier $$ nb of ticks : $nbOfTicks $$ seconds between ticks : $tickrate $$ damage : $damage";
    _mapEntries["LifeStealModifierChoice"] = "LifeStealModifier $$ ratio : $ratio %";

    _mapEntries["AoE Modifier Showcase"] = "Welcome to the AoE Modifier Showcase";
    _mapEntries["AoE Modifier Showcase main"] = "Triangles can't harm you here. $$ Your square has been empowered with an AoE modifier doing 50% damage in a 5tiles radius area. $$ Press any key to start...";
    _mapEntries["Chaining Modifier Showcase"] = "Welcome to the Chaining Modifier Showcase";
    _mapEntries["Chaining Modifier Showcase main"] = " Triangles can't harm you here. $$ Your square has been empowered with a chaining modifier bouncing to 5 targets with 80% damage decay. $$ Press any key to start...";
    _mapEntries["Dot Modifier Showcase"] = "Welcome to the Dot Modifier Showcase";
    _mapEntries["Dot Modifier Showcase main"] = "Triangles can't harm you here. $$ Your square has been empowered with a dot modifier dealing 10 damage every half second for a total of 50 damage. $$ Press any key to start...";
    _mapEntries["Lifesteal Modifier Showcase"] = "Welcome to the Lifesteal Modifier Showcase";
    _mapEntries["Lifesteal Modifier Showcase main"] = "Your square has been empowered with a lifesteal modifier healing him for 80% of the damage dealt. $$ Triangles WILL harm you here. $$ Press any key to start...";

    _mapEntries["wave_incoming_name"] = "Wary square";
    _mapEntries["wave_incoming_first"] = "A lot of belligerent shapes have been seen heading towards us, be wary!";
    _mapEntries["wave_incoming_second"] = "The next wave is going to come in 3 minutes.";

    _mapEntries["Show Anchor"] = "Anchor is low!";
    _mapEntries["Show Anchor main"] = "In one minute you won't have any anchor to maintain the gate to \"there\". $$ You need to build an anchor on the anchor spot showing on the map. $$ Be careful! It is guarded!";
    _mapEntries["press return"] = "Press Return to continue...";
}

} // namespace fas

