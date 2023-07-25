#ifndef __Godoctopus_Wave__
#define __Godoctopus_Wave__

#include <vector>
#include <string>
#include <godot_cpp/godot.hpp>
#include <godot_cpp/classes/node2d.hpp>

#include "WaveUnitCount.h"

namespace godot {

/// @brief class to store the unit content of a wave that should be spawned (can be a sub waved)
class WaveContent : public Node {
    GDCLASS(WaveContent, Node)

public:

    /////////////////
    // Godot methods
    /////////////////

    void addContent(String const &string_p, int count_p);

    void setStepsBeforeSpawn(int steps_p);

    /////////////////
    // CPP only methods
    /////////////////

    std::vector<WaveUnitCount> const &getContent() const;

    int getStepsBeforeSpawn() const;

    // Will be called by Godot when the class is registered
    // Use this to add properties to your class
    static void _bind_methods();
private:
    std::vector<WaveUnitCount> _content;

    int _stepsBeforeSpawn = 0;
};

/// @brief This class store different information about a wave
/// - main wave content
/// - scouting wave content
/// - list of upgrades to be completed when spawning the (early) wave
/// - Drop chance of the wave (used for the random selection of waves the higher the higher the chance to use this wave)
class Wave : public Node {
    GDCLASS(Wave, Node)

public:

    /////////////////
    // Godot methods
    /////////////////

    /// @brief set the main wave content
    void setMainWave(WaveContent * content_p);
    /// @brief get the main wave content
    WaveContent * getMainWave();

    /// @brief set the early wave content
    void setEarlyWave(WaveContent * content_p);
    /// @brief get the early wave content
    WaveContent * getEarlyWave();

    /// @brief Add an upgrade to the wave
    void addUpgrade(String const &upgrade_p);

    /// @brief Set the drop coef of this wave
    void setDropCoef(double const &coef_p);

    /////////////////
    // CPP only methods
    /////////////////

    std::vector<std::string> const &getUpgrades() const;

    double getDropCoef() const;

    // Will be called by Godot when the class is registered
    // Use this to add properties to your class
    static void _bind_methods();
private:
    WaveContent * _mainWave = nullptr;
    WaveContent * _earlyWave = nullptr;
    std::vector<std::string> _upgrades;
    double _dropCoef = 0;
};

/// @brief This class regroup list of waves
/// a wave can be randomly pooled from the ones given
class WavePool : public Node {
    GDCLASS(WavePool, Node)

public:

    /////////////////
    // Godot methods
    /////////////////

    /// @brief set the main wave content
    void addWave(Wave * wave_p);

    /////////////////
    // CPP only methods
    /////////////////

    std::vector<Wave *> const &getWaves() const;

    // Will be called by Godot when the class is registered
    // Use this to add properties to your class
    static void _bind_methods();
private:
    std::vector<Wave *> _waves;
};

WavePoolInfo convertToInfo(WavePool const *pool_p);

}

#endif //__Godoctopus_Wave__
