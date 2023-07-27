#include "Wave.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/input.hpp>

#include <sstream>

// octopus
#include "state/player/Player.hh"


namespace godot {

/////////////////
// WaveContent
/////////////////


void WaveContent::addContent(String const &string_p, int count_p)
{
    _content.push_back({string_p.utf8().get_data(), count_p});
}

void WaveContent::setStepsBeforeSpawn(int steps_p)
{
    _stepsBeforeSpawn = steps_p;
}

std::vector<WaveUnitCount> const &WaveContent::getContent() const
{
    return _content;
}

int WaveContent::getStepsBeforeSpawn() const
{
    return _stepsBeforeSpawn;
}

void WaveContent::_bind_methods()
{
    UtilityFunctions::print("Binding WaveContent methods");

    ClassDB::bind_method(D_METHOD("addContent", "model", "count"), &WaveContent::addContent);
    ClassDB::bind_method(D_METHOD("setStepsBeforeSpawn", "steps"), &WaveContent::setStepsBeforeSpawn);

    ADD_GROUP("WaveContent", "WaveContent_");
}


/////////////////
// Wave
/////////////////

void Wave::setMainWave(WaveContent * content_p)
{
    _mainWave = content_p;
}

WaveContent * Wave::getMainWave()
{
    return _mainWave;
}

void Wave::setEarlyWave(WaveContent * content_p)
{
    _earlyWave = content_p;
}

WaveContent * Wave::getEarlyWave()
{
    return _earlyWave;
}

void Wave::addUpgrade(String const &upgrade_p)
{
    _upgrades.push_back(upgrade_p.utf8().get_data());
}

void Wave::setDropCoef(double const &coef_p)
{
    _dropCoef = coef_p;
}

std::vector<std::string> const &Wave::getUpgrades() const
{
    return _upgrades;
}

double Wave::getDropCoef() const
{
    return _dropCoef;
}

void Wave::_bind_methods()
{
    UtilityFunctions::print("Binding Wave methods");

    ClassDB::bind_method(D_METHOD("setMainWave", "wave"), &Wave::setMainWave);
    ClassDB::bind_method(D_METHOD("getMainWave"), &Wave::getMainWave);
    ClassDB::bind_method(D_METHOD("setEarlyWave", "wave"), &Wave::setEarlyWave);
    ClassDB::bind_method(D_METHOD("getEarlyWave"), &Wave::getEarlyWave);
    ClassDB::bind_method(D_METHOD("addUpgrade" ,"upgrade"), &Wave::addUpgrade);
    ClassDB::bind_method(D_METHOD("setDropCoef", "coef"), &Wave::setDropCoef);

    ADD_GROUP("Wave", "Wave_");
}

/////////////////
// WavePool methods
/////////////////

/// @brief set the main wave content
void WavePool::addWave(Wave * wave_p)
{
    _waves.push_back(wave_p);
}

std::vector<Wave *> const &WavePool::getWaves() const
{
    return _waves;
}

void WavePool::_bind_methods()
{
    UtilityFunctions::print("Binding Wave methods");

    ClassDB::bind_method(D_METHOD("addWave", "wave"), &WavePool::addWave);

    ADD_GROUP("WavePool", "WavePool_");
}

WavePoolInfo convertToInfo(WavePool const *pool_p)
{
    WavePoolInfo info_l;

    if(!pool_p)
    {
        return info_l;
    }

    // Cannot be const because getMainWave and getEarlyWave cannot be const (?)
    for(Wave * wave_l : pool_p->getWaves())
    {
        WaveInfo waveInfo_l;
        waveInfo_l.upgrades = wave_l->getUpgrades();
        waveInfo_l.dropCoef = wave_l->getDropCoef();
        waveInfo_l.mainWave.units = wave_l->getMainWave()->getContent();
        waveInfo_l.mainWave.steps = wave_l->getMainWave()->getStepsBeforeSpawn();
        waveInfo_l.earlyWave.units = wave_l->getEarlyWave()->getContent();
        waveInfo_l.earlyWave.steps = wave_l->getEarlyWave()->getStepsBeforeSpawn();
        info_l.infos.push_back(waveInfo_l);
    }

    return info_l;
}

}
