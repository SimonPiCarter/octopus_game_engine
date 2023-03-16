#ifndef __OrcaManager__
#define __OrcaManager__

#include "state/Handle.hh"
#include <unordered_map>

namespace RVO
{
    class RVOSimulator;
}

namespace octopus
{

class State;
class Step;

/// @brief This class is responsible for the orca simulation management
/// It offers :
/// - loading the simulation from a state
/// - updating the simulation from a state and one step
/// - setting up a simulation step
/// - updating the step from a simulation step
class OrcaManager
{
public:
    OrcaManager(double timeStep_p, double neighborDist_p, size_t maxNeighbors_p, double timeHorizon_p, double timeHorizonObst_p);
    ~OrcaManager();

    /// @brief will load actors and obstacles from current state
    /// @param state_p
    void resetFromState(State const &state_p);

    /// @brief Will add or remove agents or obstacle based on the state and steps
    /// @param state_p
    /// @param step_p
    /// @warning state must be in the state just after this step application otherwise result are undefined
    void updateFromStep(State const &state_p, Step const &step_p);

    /// @brief perform the simulation step using prefered speed given in step
    /// @param step_p will be updated with result of simulation
    void setupStep(State const &state_p, Step &step_p);
    void doStep();
    void commitStep(State const &state_p, Step &step_p);

    /// @brief check if the manager should be reset, this happens as soon as any entity is spawned
    /// or died or if manager is null
    /// @param manager_p
    /// @param state_p
    /// @param step_p
    /// @return
    static bool ShouldReset(OrcaManager const *manager_p, State const &state_p, Step const &step_p);

private:
    double const _timeStep;
    double const _neighborDist;
    size_t const _maxNeighbors;
    double const _timeHorizon;
    double const _timeHorizonObst;

    unsigned long _gridStatus {0};

    /// @brief the simulation to use
    RVO::RVOSimulator *_sim {nullptr};

    /// @brief temporary mapping
    std::unordered_map<octopus::Handle, size_t> _mapHandleIdx;
};
} // namespace octopus

#endif