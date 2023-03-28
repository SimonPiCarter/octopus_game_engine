#include "ProductionPanel.hh"

#include "command/building/BuildingUnitProductionCommand.hh"
#include "command/CommandQueue.hh"
#include "state/State.hh"
#include "state/player/Player.hh"
#include "state/entity/Entity.hh"
#include "state/entity/Unit.hh"
#include "state/entity/Resource.hh"
#include "state/model/entity/EntityModel.hh"
#include "state/model/entity/BuildingModel.hh"
#include "state/model/entity/UnitModel.hh"

namespace cuttlefish
{

ProductionPanel::ProductionPanel(Window* window_p, int x, int y, Texture const *icons_p, Texture const *barBack_p, Texture const *barFill_p, int maxIcons_p, Selection &selection_p) :
	_x(x),
	_y(y),
	_icons(icons_p),
	_selection(selection_p)
{
	for(size_t idx_l = 0 ; idx_l < maxIcons_p ; ++ idx_l)
	{
		_productionPictures.push_back(new ProductionPicture());
		_productionPictures.back()->_icon = new Picture(_icons, 64, 64, {1}, {1});
		_productionPictures.back()->_bar = new ProgressBar(new Picture(barBack_p, 62, 6, {1}, {1}), new Picture(barFill_p, 62, 6, {1}, {1}), 62, 6, 1);

		int posX_l = _x + idx_l * 65;
		// add offset to be at bottom of the panel
		int posY_l = _y;
		_productionPictures.back()->_icon->setDestination(posX_l, posY_l, 64, 64);
		// Pos + offset to have progress bar on the bottom
		_productionPictures.back()->_bar->setPosition(posX_l, posY_l + 56);
	}
}

ProductionPanel::~ProductionPanel()
{
	for(ProductionPicture *ptr_l : _productionPictures)
	{
		delete ptr_l;
	}
}

struct CommandSorter
{
	bool operator()(std::pair<octopus::BuildingUnitProductionCommand const *, octopus::UnitProductionData const *> const &a,
					std::pair<octopus::BuildingUnitProductionCommand const *, octopus::UnitProductionData const *> const &b) const
	{
		return a.second->_progression > b.second->_progression;
	}
};

void ProductionPanel::refresh(Window &window_p, octopus::State const &state_p)
{
	std::vector<std::pair<octopus::BuildingUnitProductionCommand const *, octopus::UnitProductionData const *> > vecCommands_l;
	std::vector<unsigned long> vecIdx_l;


	for(SpriteEntity * spirteEnt_l : _selection._sprites)
	{
		octopus::Entity const * ent_l = state_p.getEntity(spirteEnt_l->getHandle());
		if(!ent_l->getQueue().hasCommand())
		{
			continue;
		}
		auto it_l = ent_l->getQueue().getCurrentCommand();
		while(it_l != ent_l->getQueue().getEnd())
		{
			octopus::BuildingUnitProductionCommand const *cmd_l = dynamic_cast<octopus::BuildingUnitProductionCommand const *>(it_l->_cmd);
			octopus::UnitProductionData const *data_l = dynamic_cast<octopus::UnitProductionData const *>(it_l->_data);
			if(cmd_l && data_l)
			{
				vecCommands_l.push_back(std::make_pair(cmd_l, data_l));
				vecIdx_l.push_back(it_l->_id);
			}
			++it_l;
		}
	}

	std::sort(vecCommands_l.begin(), vecCommands_l.end(), CommandSorter());

	_productionIndex.clear();
	_productionHandle.clear();
	_productionToDisplay = std::min(vecCommands_l.size(), _productionPictures.size());

	for(size_t i = 0 ; i < _productionToDisplay ; ++i)
	{
		SpriteInfo const &info_l = _mapIcons.at(vecCommands_l[i].first->getModel()._id);
		_productionPictures[i]->_icon->setState(info_l.state);
		_productionPictures[i]->_icon->setFrame(info_l.frame);

		_productionPictures[i]->_bar->setProgress(100.*vecCommands_l[i].second->_progression/vecCommands_l[i].second->_completeTime);

		_productionIndex.push_back(vecIdx_l[i]);
		_productionHandle.push_back(vecCommands_l[i].first->getHandleCommand());
	}
}

void ProductionPanel::render(Window &window_p)
{
	for(size_t i = 0 ; i < _productionToDisplay ; ++i)
	{
		_productionPictures[i]->_icon->display(window_p);
		_productionPictures[i]->_bar->display(window_p);
	}
}

void ProductionPanel::addSpriteInfo(std::string const &model_p, int state_p, int frame_p)
{
	_mapIcons[model_p].state = state_p;
	_mapIcons[model_p].frame = frame_p;
}

IndexProductionClic ProductionPanel::getIndex(Window &window_p, int x, int y)
{
	for(size_t i = 0 ; i < _productionToDisplay ; ++i)
	{
		if(_productionPictures[i]->_icon->isInside(window_p, x, y))
		{
			return {_productionHandle.at(i), _productionIndex.at(i), true};
		}
	}
	return {0, 0, false};
}

} // namespace cuttlefish
