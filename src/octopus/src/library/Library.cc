#include "Library.hh"

#include "state/model/entity/BuildingModel.hh"
#include "state/model/entity/EntityModel.hh"
#include "state/model/entity/TempleModel.hh"
#include "state/model/entity/UnitModel.hh"

namespace octopus
{

Library::~Library()
{
	for(auto &&pair_l : _mapEntityModel) { delete pair_l.second; }
	for(auto &&pair_l : _mapBuildingModel) { delete pair_l.second; }
	for(auto &&pair_l : _mapUnitModel) { delete pair_l.second; }
}

void Library::registerEntityModel(std::string const &id_p, EntityModel const &model_p)
{
	_mapEntityModel[id_p] = new EntityModel(model_p);
	_mapEntityModel[id_p]->_id = id_p;
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
}

void Library::registerTempleModel(std::string const &id_p, TempleModel const &model_p)
{
	_mapBuildingModel[id_p] = new TempleModel(model_p);
	_mapBuildingModel[id_p]->_id = id_p;
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
}

UnitModel const & Library::getUnitModel(std::string const &id_p) const
{
	return *_mapUnitModel.at(id_p);
}

bool Library::hasUnitModel(std::string const &id_p) const
{
	return _mapUnitModel.find(id_p) != _mapUnitModel.end();
}

}
