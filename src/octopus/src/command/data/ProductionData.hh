#ifndef __ProductionData__
#define __ProductionData__

#include "command/CommandData.hh"

#include "state/model/entity/UnitModel.hh"

namespace octopus
{

/// @brief This class is aimed at supporting
/// every command that involve production time
class ProductionData : public CommandData
{
public:
	ProductionData(unsigned long completeTime_p) :
		_progression(0.),
		_completeTime(completeTime_p),
		_paid(false),
		_canceled(false) {}

	double _progression;
	unsigned long const _completeTime;

	bool _paid;

	bool _canceled;
};

/// @brief This class is used to track unit production
class UnitProductionData : public ProductionData
{
public:
	UnitProductionData() : ProductionData(0) {}
	UnitProductionData(UnitModel const &model_p) :
		ProductionData(model_p._productionTime), _model(&model_p) {}

	UnitModel const * _model {nullptr};
};

} // namespace octopus


#endif
