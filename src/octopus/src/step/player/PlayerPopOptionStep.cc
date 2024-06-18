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
    for(Steppable * step_l : _subSteps)
    {
        delete step_l;
    }
}

void PlayerPopOptionStep::apply(State &state_p) const
{
	Logger::getDebug() << "PlayerPopOptionStep :: apply " << this->_player<<std::endl;
	Player *player_l = state_p.getPlayer(_player);

    if(player_l->_options.empty())
    {
        throw std::logic_error("Error while poping option : no option with the given key "+_key);
    }
    if(player_l->_options.front()->_key != _key)
    {
        throw std::logic_error("Error while poping option : the front option has different key : "+_key);
    }

    StepOptionsGenerator * generator_l = player_l->_options.front();

    std::vector<Steppable *> subSteps_l = generator_l->genSteppables(state_p, _choice);

    // apply chosen sub steps
    for(Steppable * step_l : subSteps_l)
    {
        step_l->apply(state_p);
        delete step_l;
    }

    // pop option from state
    delete player_l->_options.front();
    player_l->_options.pop_front();

	// generate steps for next option
	if(!player_l->_options.empty())
	{
		player_l->_options.front()->genOptions(state_p);
	}
}

void PlayerPopOptionStep::revert(State &state_p, SteppableData const *data_p) const
{
	Logger::getDebug() << "PlayerPopOptionStep :: revert " << this->_player<<std::endl;
	Player *player_l = state_p.getPlayer(_player);

    PlayerPopOptionStepData const * data_l = dynamic_cast<PlayerPopOptionStepData const *>(data_p);

    if(data_l->_generator == nullptr)
    {
        throw std::logic_error("Error while poping option : no option generator present for state.");
    }

    // restore generator in state
    StepOptionsGenerator * generator_l = data_l->_generator->newCopy();
    player_l->_options.push_front(generator_l);

    std::vector<Steppable *> subSteps_l = data_l->_subSteps;

    // revert chosen sub steps (use > 0 to avoid unsigned -1 and infinite loop)
    for(uint32_t i = subSteps_l.size() ; i > 0 ; --i)
    {
        Steppable const * step_l = subSteps_l[i-1];
        SteppableData const * stepData_l = data_l->_data[i-1];
        step_l->revert(state_p, stepData_l);
    }

	// gen options after restoring state
	player_l->_options.front()->genOptions(state_p);
}

SteppableData * PlayerPopOptionStep::newData(State const &state_p) const
{
    PlayerPopOptionStepData * data_l = new PlayerPopOptionStepData();
    Player const *player_l = state_p.getPlayer(_player);

    data_l->_generator = player_l->_options.front()->newCopy();

	// do not use new copy here as its options wont be generated
    data_l->_subSteps = player_l->_options.front()->genSteppables(state_p, _choice);

    data_l->_data.reserve(data_l->_subSteps.size());
    for(Steppable * step_l : data_l->_subSteps)
    {
        data_l->_data.push_back(step_l->newData(state_p));
    }

    return data_l;
}

} // namespace octopus

