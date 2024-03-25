#ifndef __ProductionData__
#define __ProductionData__

#include "command/CommandData.hh"

#include <string>

#include "state/model/entity/UnitModel.hh"
#include "state/player/Player.hh"
#include "state/player/upgrade/Upgrade.hh"

namespace octopus
{

/// @brief This class is aimed at supporting
/// every command that involve production time
class ProductionData : public CommandData
{
public:
	ProductionData() {}

	Fixed _progression {0};

	bool _paid {false};

	bool _canceled {false};

	/// @brief return the cost of the production
	virtual std::map<std::string, Fixed> getCost(Player const &player_p) const = 0;
	/// @brief return the cost of the production
	virtual Fixed getCompleteTime(Player const &player_p) const = 0;
	/// @brief return the model unique id of the production
	virtual const std::string &getIdModel() const = 0;
};

/// @brief This class is used to track unit production
class UnitProductionData : public ProductionData
{
public:
	UnitProductionData() {}
	UnitProductionData(UnitModel const &model_p) :
		_model(&model_p) {}

	UnitModel const * _model {nullptr};

	virtual std::map<std::string, Fixed> getCost(Player const &player_p) const { return octopus::getCost(*_model, player_p); }
	virtual Fixed getCompleteTime(Player const &player_p) const { return getProductionTime(*_model, player_p); }

	virtual const std::string &getIdModel() const { return _model->_id; }
};

/// @brief This class is used to track upgrade production
class UpgradeProductionData : public ProductionData
{
public:
	UpgradeProductionData() {}
	UpgradeProductionData(Upgrade const &upgrade_p) :
		ProductionData(), _upgrade(&upgrade_p) {}

	Upgrade const * _upgrade {nullptr};
	unsigned long _level = 0;

	virtual std::map<std::string, Fixed> getCost(Player const &) const { return octopus::getCost(*_upgrade, _level); }
	virtual Fixed getCompleteTime(Player const &player_p) const { return getProductionTime(*_upgrade, _level); }

	virtual const std::string &getIdModel() const { return _upgrade->_id; }
};

} // namespace octopus


#endif
