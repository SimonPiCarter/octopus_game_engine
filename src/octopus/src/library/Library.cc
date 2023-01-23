#include "Library.hh"

#include "state/model/entity/BuildingModel.hh"
#include "state/model/entity/EntityModel.hh"
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
}

EntityModel const & Library::getEntityModel(std::string const &id_p) const
{
	return *_mapEntityModel.at(id_p);
}

void Library::registerBuildingModel(std::string const &id_p, BuildingModel const &model_p)
{
	_mapBuildingModel[id_p] = new BuildingModel(model_p);
}

BuildingModel const & Library::getBuildingModel(std::string const &id_p) const
{
	return *_mapBuildingModel.at(id_p);
}

void Library::registerUnitModel(std::string const &id_p, UnitModel const &model_p)
{
	_mapUnitModel[id_p] = new UnitModel(model_p);
}

UnitModel const & Library::getUnitModel(std::string const &id_p) const
{
	return *_mapUnitModel.at(id_p);
}

}
