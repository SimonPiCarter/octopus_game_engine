#ifndef __LIBRARY__
#define __LIBRARY__

#include <unordered_map>
#include <string>

namespace octopus
{

struct EntityModel;
struct BuildingModel;;
struct UnitModel;;

/// @brief This class is storing resources like
/// EntityModel, UnitModel, BuildingModel
class Library
{
public:
	~Library();

	void registerEntityModel(std::string const &id_p, EntityModel const &model_p);
	EntityModel const & getEntityModel(std::string const &id_p) const;
	void registerBuildingModel(std::string const &id_p, BuildingModel const &model_p);
	BuildingModel const & getBuildingModel(std::string const &id_p) const;
	void registerUnitModel(std::string const &id_p, UnitModel const &model_p);
	UnitModel const & getUnitModel(std::string const &id_p) const;

private:
	std::unordered_map<std::string, EntityModel *> _mapEntityModel;
	std::unordered_map<std::string, BuildingModel *> _mapBuildingModel;
	std::unordered_map<std::string, UnitModel *> _mapUnitModel;
};

}

#endif
