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

		Fixed _aggroDistance = 8;
};

}

#endif
