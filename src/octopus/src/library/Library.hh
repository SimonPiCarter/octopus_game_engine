#ifndef __LIBRARY__
#define __LIBRARY__

#include <unordered_map>
#include <string>

namespace octopus
{

struct EntityModel;
struct BuildingModel;
struct TempleModel;
struct ResourceModel;
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
	UnitModel & getUnitModel(std::string const &id_p);
	bool hasUnitModel(std::string const &id_p) const;

	void registerResourceModel(std::string const &id_p, ResourceModel const &model_p);
	ResourceModel const & getResourceModel(std::string const &id_p) const;
	bool hasResourceModel(std::string const &id_p) const;

	void registerUpgrade(std::string const &id_p, Upgrade *upgrade_p);
	Upgrade const & getUpgrade(std::string const &id_p) const;
	bool hasUpgrade(std::string const &id_p) const;
private:
	std::unordered_map<std::string, EntityModel *> _mapEntityModel;
	/// @brief store the entity really stored as entity to clean memory
	std::unordered_map<std::string, EntityModel *> _mapEntityModelRegisteredAsEntity;
	std::unordered_map<std::string, BuildingModel *> _mapBuildingModel;
	std::unordered_map<std::string, UnitModel *> _mapUnitModel;
	std::unordered_map<std::string, ResourceModel *> _mapResourceModel;
	std::unordered_map<std::string, Upgrade *> _mapUpgrade;
};

}

#endif
