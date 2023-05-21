#ifndef __ProductionData__
#define __ProductionData__

#include "command/CommandData.hh"

#include "state/model/entity/UnitModel.hh"
#include "state/player/upgrade/Upgrade.hh"

namespace octopus
{

/// @brief This class is aimed at supporting
/// every command that involve production time
class ProductionData : public CommandData
{
public:
	ProductionData() {}
	ProductionData(Fixed const &completeTime_p) : _completeTime(completeTime_p) {}

	Fixed _progression {0};
	Fixed _completeTime {0};

	bool _paid {false};

	bool _canceled {false};

	/// @brief return the cost of the production
	virtual const std::map<std::string, Fixed> &getCost() const = 0;
	/// @brief return the model unique id of the production
	virtual const std::string &getIdModel() const = 0;
};

/// @brief This class is used to track unit production
class UnitProductionData : public ProductionData
{
public:
	UnitProductionData() {}
	UnitProductionData(UnitModel const &model_p) :
		ProductionData(model_p._productionTime), _model(&model_p) {}

	UnitModel const * _model {nullptr};

	virtual const std::map<std::string, Fixed> &getCost() const { return _model->_cost; }
	virtual const std::string &getIdModel() const { return _model->_id; }
};

/// @brief This class is used to track upgrade production
class UpgradeProductionData : public ProductionData
{
public:
	UpgradeProductionData() {}
	UpgradeProductionData(Upgrade const &upgrade_p) :
		ProductionData(upgrade_p._productionTime), _upgrade(&upgrade_p) {}

	Upgrade const * _upgrade {nullptr};

	virtual const std::map<std::string, Fixed> &getCost() const { return _upgrade->_cost; }
	virtual const std::string &getIdModel() const { return _upgrade->_id; }
};

} // namespace octopus


#endif
