#include "PlayerAddBuildingModel.hh"

#include "state/State.hh"
#include "state/player/Player.hh"
#include "state/model/entity/BuildingModel.hh"
#include "logger/Logger.hh"

namespace octopus
{

bool shouldSkip(State const &state_p, unsigned long idx_p, std::string const &modelId_p)
{
	Player const * player_l = state_p.getPlayer(idx_p);

	for(BuildingModel const *model_l : player_l->_buildingModels)
	{
		if(model_l->_id == modelId_p)
		{
			return true;
		}
	}
    return false;
}

void PlayerAddBuildingModel::apply(State &state_p) const
{
	Logger::getDebug() << "PlayerAddBuildingModel :: apply id = " << this->_playerIdx << " model = " <<this->_model._id<<std::endl;

    // only apply if we should not skip
    if(!shouldSkip(state_p, this->_playerIdx, _model._id))
    {
	    Player * player_l = state_p.getPlayer(this->_playerIdx);
	    player_l->_buildingModels.push_back(&this->_model);
    }
}

void PlayerAddBuildingModel::revert(State &state_p, SteppableData const *data_p) const
{
	Logger::getDebug() << "PlayerAddBuildingModel :: revert id = " << this->_playerIdx << " model = " <<this->_model._id<<std::endl;
	PlayerAddBuildingModelData const * typpedData_l = dynamic_cast<PlayerAddBuildingModelData const *>(data_p);

	if(typpedData_l && typpedData_l->skipped)
	{
		Logger::getDebug() << "skipped"<<std::endl;
		return;
	}

	Player * player_l = state_p.getPlayer(this->_playerIdx);

	if(&this->_model != player_l->_buildingModels.back())
	{
		throw std::logic_error("PlayerAddBuildingModel revert were not called in a correct order");
	}
	player_l->_buildingModels.pop_back();

}

SteppableData * PlayerAddBuildingModel::newData(State const &state_p) const
{
	PlayerAddBuildingModelData *data_l = new PlayerAddBuildingModelData();
	Player const * player_l = state_p.getPlayer(this->_playerIdx);

    data_l->skipped = shouldSkip(state_p, this->_playerIdx, _model._id);

	return data_l;
}
} // namespace octopus

