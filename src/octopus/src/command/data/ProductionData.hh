#ifndef __ProductionData__
#define __ProductionData__

#include "command/CommandData.hh"

namespace octopus
{

class UnitModel;

/// @brief This class is aimed at supporting
/// every command that involve production time
class ProductionData : public CommandData
{
public:
	ProductionData() :
		_progression(0.),
		_paid(false) {}

	double _progression;

	bool _paid;
};

/// @brief This class is used to track unit production
class UnitProductionData : public ProductionData
{
public:
	UnitProductionData(UnitModel const &model_p) :
		ProductionData(), _model(model_p) {}

	UnitModel const &_model;
};

} // namespace octopus


#endif
