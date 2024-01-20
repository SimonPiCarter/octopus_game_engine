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
		~Unit() { delete _unitData; }

		/// used when harvesting
		std::string _typeOfResource { "bloc" };
		Fixed _quantityOfResource { 0. };

		UnitModel const &_unitModel;

		/// @brief used to store specific unit data that might be modified
		UnitData * _unitData = nullptr;
		/// @brief used to store specific unit data that will be static
		StaticUnitData * _staticUnitData = nullptr;
};

}

#endif
