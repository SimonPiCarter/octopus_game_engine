#include "PlayerPopOptionStep.hh"

#include "state/State.hh"
#include "state/player/Player.hh"
#include "logger/Logger.hh"

namespace octopus
{

PlayerPopOptionStepData::~PlayerPopOptionStepData()
{
    delete _generator;
    for(SteppableData * data_l : _data)
    {
        delete data_l;
    }
}

void PlayerPopOptionStep::apply(State &state_p) const
{
	Logger::getDebug() << "PlayerPopOptionStep :: apply " << this->_player<<std::endl;
	Player *player_l = state_p.getPlayer(_player);

    StepOptionsGenerator * generator_l = player_l->_options[_key];

    if(generator_l == nullptr)
    {
        throw std::logic_error("Error while poping option : no option with the given key "+_key);
    }

    std::vector<Steppable *> subSteps_l = generator_l->getSteppables(_choice);

    // apply chosen sub steps
    for(Steppable * step_l : subSteps_l)
    {
        step_l->apply(state_p);
        delete step_l;
    }

    // pop option from state
    delete player_l->_options[_key];
    player_l->_options[_key] = nullptr;
}

void PlayerPopOptionStep::revert(State &state_p, SteppableData const *data_p) const
{
	Logger::getDebug() << "PlayerPopOptionStep :: revert " << this->_player<<std::endl;
	Player *player_l = state_p.getPlayer(_player);

    PlayerPopOptionStepData const * data_l = dynamic_cast<PlayerPopOptionStepData const *>(data_p);

    if(player_l->_options[_key] != nullptr)
    {
        throw std::logic_error("Error while poping option : an option is present with the key "+_key);
    }
    if(data_l->_generator == nullptr)
    {
        throw std::logic_error("Error while poping option : no option generator present for state.");
    }

    // restore generator in state
    player_l->_options[_key] = data_l->_generator->newCopy();

    StepOptionsGenerator * generator_l = player_l->_options[_key];

    std::vector<Steppable *> subSteps_l = generator_l->getSteppables(_choice);

    // revert chosen sub steps (use > 0 to avoid unsigned -1 and infinite loop)
    for(uint32_t i = subSteps_l.size() ; i > 0 ; --i)
    {
        Steppable const * step_l = subSteps_l[i-1];
        SteppableData const * stepData_l = data_l->_data[i-1];
        step_l->revert(state_p, stepData_l);
        delete subSteps_l[i-1];
    }
}

SteppableData * PlayerPopOptionStep::newData(State const &state_p) const
{
    PlayerPopOptionStepData * data_l = new PlayerPopOptionStepData();
    Player const *player_l = state_p.getPlayer(_player);

    data_l->_generator = player_l->_options.at(_key)->newCopy();

    std::vector<Steppable *> subSteps_l = data_l->_generator->getSteppables(_choice);

    data_l->_data.reserve(subSteps_l.size());
    for(Steppable * step_l : subSteps_l)
    {
        data_l->_data.push_back(step_l->newData(state_p));
        delete step_l;
    }

    return data_l;
}

} // namespace octopus

