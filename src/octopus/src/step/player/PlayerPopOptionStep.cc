#include "PlayerPopOptionStep.hh"

#include "state/State.hh"
#include "state/player/Player.hh"
#include "logger/Logger.hh"

namespace octopus
{

PlayerPopOptionStepData::~PlayerPopOptionStepData()
{
    for(auto &&pair_l : _mapGeneratorPerState)
    {
        delete pair_l.second;
    }
    for(SteppableData * data_l : _data)
    {
        delete data_l;
    }
}

void PlayerPopOptionStep::apply(State &state_p, SteppableData *data_p) const
{
	Logger::getDebug() << "PlayerPopOptionStep :: apply " << this->_player<<std::endl;
	Player *player_l = state_p.getPlayer(_player);

    PlayerPopOptionStepData * data_l = dynamic_cast<PlayerPopOptionStepData *>(data_p);

    StepOptionsGenerator * generator_l = player_l->_options[_key];

    if(generator_l == nullptr)
    {
        throw std::logic_error("Error while poping option : no option with the given key "+_key);
    }

    std::vector<Steppable *> subSteps_l = generator_l->getSteppables(_choice, _player);

    // if sup steps data have not been created create them
    if(data_l->_data.empty())
    {
        for(Steppable * step_l : subSteps_l)
        {
            data_l->_data.push_back(step_l->newData());
        }
    }

    // apply chosen sub steps
    for(size_t i = 0 ; i < subSteps_l.size() ; ++i)
    {
        Steppable const * step_l = subSteps_l[i];
        SteppableData * stepData_l = data_l->_data[i];
        step_l->apply(state_p, stepData_l);
    }

    // save generator
    data_l->_mapGeneratorPerState[state_p._id] = generator_l;

    // pop option from state
    player_l->_options[_key] = nullptr;
}

void PlayerPopOptionStep::revert(State &state_p, SteppableData *data_p) const
{
	Logger::getDebug() << "PlayerPopOptionStep :: revert " << this->_player<<std::endl;
	Player *player_l = state_p.getPlayer(_player);

    PlayerPopOptionStepData * data_l = dynamic_cast<PlayerPopOptionStepData *>(data_p);

    if(player_l->_options[_key] != nullptr)
    {
        throw std::logic_error("Error while poping option : an option is present with the key "+_key);
    }
    if(data_l->_mapGeneratorPerState[state_p._id] == nullptr)
    {
        throw std::logic_error("Error while poping option : no option generator present for state.");
    }

    // restore generator in state
    player_l->_options[_key] = data_l->_mapGeneratorPerState[state_p._id];

    // pop option from data
    data_l->_mapGeneratorPerState.erase(state_p._id);

    StepOptionsGenerator * generator_l = player_l->_options[_key];

    std::vector<Steppable *> subSteps_l = generator_l->getSteppables(_choice, _player);

    // revert chosen sub steps (use > 0 to avoid unsigned -1 and infinite loop)
    for(size_t i = subSteps_l.size() ; i > 0 ; --i)
    {
        Steppable const * step_l = subSteps_l[i-1];
        SteppableData * stepData_l = data_l->_data[i-1];
        step_l->revert(state_p, stepData_l);
    }

    // if no more generator we remove data
    if(data_l->_mapGeneratorPerState.empty())
    {
        for(SteppableData * stepData_l : data_l->_data)
        {
            delete stepData_l;
        }
        data_l->_data.clear();
    }

}

} // namespace octopus

