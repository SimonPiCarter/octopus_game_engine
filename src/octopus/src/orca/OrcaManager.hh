#ifndef __OrcaManager__
#define __OrcaManager__

#include "state/Handle.hh"
#include <cstddef>
#include <vector>
#include <unordered_map>
#include "utils/Fixed.hh"

namespace RVO
{
    class RVOSimulator;
    class Agent;
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
    OrcaManager(Fixed timeStep_p, Fixed neighborDist_p, uint32_t maxNeighbors_p, Fixed timeHorizon_p, Fixed timeHorizonObst_p);
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

    std::vector<std::pair<octopus::Fixed , const RVO::Agent *> > const & getAgentNeighbors(octopus::Handle const &handle_p) const;

private:
    Fixed const _timeStep;
    Fixed const _neighborDist;
    uint32_t const _maxNeighbors;
    Fixed const _timeHorizon;
    Fixed const _timeHorizonObst;

    unsigned long _gridStatus {0};

    /// @brief the simulation to use
    RVO::RVOSimulator *_sim {nullptr};

    /// @brief temporary mapping
    std::unordered_map<octopus::Handle, uint32_t> _mapHandleIdx;
};
} // namespace octopus

#endif
