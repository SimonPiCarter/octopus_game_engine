#include "CommandHotkeys.hh"

#include "panel/Panel.hh"
#include "state/model/entity/BuildingModel.hh"
#include "state/model/entity/UnitModel.hh"

namespace cuttlefish
{

CommandHotkeys *CommandHotkeys::_instance = nullptr;

CommandHotkeys * CommandHotkeys::GetInstance()
{
    if(!_instance)
    {
        _instance = new CommandHotkeys();
    }
    return _instance;
}


void CommandHotkeys::addBuildingModelKeycode(std::string const &model_p, Sint32 const &key_p)
{
	_keyPerBuildingModel[model_p] = key_p;
}
void CommandHotkeys::addUnitModelKeycode(std::string const &model_p, Sint32 const &key_p)
{
	_keyPerUnitModel[model_p] = key_p;
}
void CommandHotkeys::addCommandTypeKeycode(CommandPicture::Type type_p, Sint32 const &key_p)
{
	_keyPerCommandType[type_p] = key_p;
}

Sint32 CommandHotkeys::getKeycode(CommandPicture const&command_p) const
{
	if(command_p._type == CommandPicture::Type::Production)
	{
		if(command_p._model->unitModel)
		{
			return _keyPerUnitModel.at(command_p._model->unitModel->_id);
		}
		else if(command_p._model->buildingModel)
		{
			return _keyPerBuildingModel.at(command_p._model->buildingModel->_id);
		}
		else
		{
			return SDLK_0;
		}
	}
	else
	{
		return _keyPerCommandType.at(command_p._type);
	}
}

std::string CommandHotkeys::getStringCode(Sint32 const &key_p)
{
	char c = '#';

	if(key_p >= SDLK_a && key_p <= SDLK_z)
	{
		c = 'A' + key_p - SDLK_a;
	}

	return std::string{c};
}

CommandHotkeys::CommandHotkeys()
{}

} // namespace cuttlefish
