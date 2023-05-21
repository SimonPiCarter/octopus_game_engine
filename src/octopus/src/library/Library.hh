#ifndef __LIBRARY__
#define __LIBRARY__

#include <unordered_map>
#include <string>

namespace octopus
{

struct EntityModel;
struct BuildingModel;
struct TempleModel;
struct UnitModel;
struct Upgrade;

/// @brief This class is storing resources like
/// EntityModel, UnitModel, BuildingModel, TempleModel, Upgrade
class Library
{
public:
	~Library();

	void registerEntityModel(std::string const &id_p, EntityModel const &model_p);
	EntityModel const & getEntityModel(std::string const &id_p) const;
	bool hasEntityModel(std::string const &id_p) const;

	void registerBuildingModel(std::string const &id_p, BuildingModel const &model_p);
	void registerTempleModel(std::string const &id_p, TempleModel const &model_p);
	BuildingModel const & getBuildingModel(std::string const &id_p) const;
	bool hasBuildingModel(std::string const &id_p) const;

	void registerUnitModel(std::string const &id_p, UnitModel const &model_p);
	UnitModel const & getUnitModel(std::string const &id_p) const;
	bool hasUnitModel(std::string const &id_p) const;

	void registerUpgrade(std::string const &id_p, Upgrade *upgrade_p);
	Upgrade const & getUpgrade(std::string const &id_p) const;
	bool hasUpgrade(std::string const &id_p) const;
private:
	std::unordered_map<std::string, EntityModel *> _mapEntityModel;
	std::unordered_map<std::string, BuildingModel *> _mapBuildingModel;
	std::unordered_map<std::string, UnitModel *> _mapUnitModel;
	std::unordered_map<std::string, Upgrade *> _mapUpgrade;
};

}

#endif
