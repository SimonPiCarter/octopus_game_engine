#include "Library.hh"

#include "state/model/entity/BuildingModel.hh"
#include "state/model/entity/EntityModel.hh"
#include "state/model/entity/TempleModel.hh"
#include "state/model/entity/UnitModel.hh"
#include "state/model/entity/ResourceModel.hh"
#include "state/player/upgrade/Upgrade.hh"

namespace octopus
{

Library::~Library()
{
	for(auto &&pair_l : _mapEntityModelRegisteredAsEntity) { delete pair_l.second; }
	for(auto &&pair_l : _mapBuildingModel) { delete pair_l.second; }
	for(auto &&pair_l : _mapUnitModel) { delete pair_l.second; }
	for(auto &&pair_l : _mapUpgrade) { delete pair_l.second; }
}

void Library::registerEntityModel(std::string const &id_p, EntityModel const &model_p)
{
	_mapEntityModel[id_p] = new EntityModel(model_p);
	_mapEntityModel[id_p]->_id = id_p;
	_mapEntityModelRegisteredAsEntity[id_p] = _mapEntityModel[id_p];
}

EntityModel const & Library::getEntityModel(std::string const &id_p) const
{
	return *_mapEntityModel.at(id_p);
}

bool Library::hasEntityModel(std::string const &id_p) const
{
	return _mapEntityModel.find(id_p) != _mapEntityModel.end();
}

void Library::registerBuildingModel(std::string const &id_p, BuildingModel const &model_p)
{
	_mapBuildingModel[id_p] = new BuildingModel(model_p);
	_mapBuildingModel[id_p]->_id = id_p;
	_mapEntityModel[id_p] = _mapBuildingModel[id_p];
}

void Library::registerTempleModel(std::string const &id_p, TempleModel const &model_p)
{
	_mapBuildingModel[id_p] = new TempleModel(model_p);
	_mapBuildingModel[id_p]->_id = id_p;
	_mapEntityModel[id_p] = _mapBuildingModel[id_p];
}

BuildingModel const & Library::getBuildingModel(std::string const &id_p) const
{
	return *_mapBuildingModel.at(id_p);
}

bool Library::hasBuildingModel(std::string const &id_p) const
{
	return _mapBuildingModel.find(id_p) != _mapBuildingModel.end();
}

void Library::registerUnitModel(std::string const &id_p, UnitModel const &model_p)
{
	_mapUnitModel[id_p] = new UnitModel(model_p);
	_mapUnitModel[id_p]->_id = id_p;
	_mapEntityModel[id_p] = _mapUnitModel[id_p];
}

UnitModel const & Library::getUnitModel(std::string const &id_p) const
{
	return *_mapUnitModel.at(id_p);
}

UnitModel & Library::getUnitModel(std::string const &id_p)
{
	return *_mapUnitModel.at(id_p);
}

bool Library::hasUnitModel(std::string const &id_p) const
{
	return _mapUnitModel.find(id_p) != _mapUnitModel.end();
}

void Library::registerResourceModel(std::string const &id_p, ResourceModel const &model_p)
{
	_mapResourceModel[id_p] = new ResourceModel(model_p);
	_mapResourceModel[id_p]->_id = id_p;
	_mapEntityModel[id_p] = _mapResourceModel[id_p];
}

ResourceModel const & Library::getResourceModel(std::string const &id_p) const
{
	return *_mapResourceModel.at(id_p);
}

bool Library::hasResourceModel(std::string const &id_p) const
{
	return _mapResourceModel.find(id_p) != _mapResourceModel.end();
}

void Library::registerUpgrade(std::string const &id_p, Upgrade *upgrade_p)
{
	_mapUpgrade[id_p] = upgrade_p;
	_mapUpgrade[id_p]->_id = id_p;
}

Upgrade const & Library::getUpgrade(std::string const &id_p) const
{
	return *_mapUpgrade.at(id_p);
}

bool Library::hasUpgrade(std::string const &id_p) const
{
	return _mapUpgrade.find(id_p) != _mapUpgrade.end();
}

}
