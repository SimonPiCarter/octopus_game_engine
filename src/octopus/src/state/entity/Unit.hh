#ifndef __UNIT__
#define __UNIT__

#include "Entity.hh"
#include "state/model/entity/UnitModel.hh"

namespace octopus
{

class Unit : public Entity
{
	public:
		Unit(Vector const &pos_p, bool frozen_p, UnitModel const &model_p);

		/// used when harvesting
		std::string _typeOfResource { "bloc" };
		Fixed _quantityOfResource { 0. };

		UnitModel const &_unitModel;

		/// @brief used to store specific unit data that might be modified
		ClonableWrapper<UnitData> _unitData;
		/// @brief used to store specific unit data that will be static
		StaticUnitData const * _staticUnitData;
};

}

#endif
