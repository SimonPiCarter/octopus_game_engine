#ifndef __UNIT__
#define __UNIT__

#include "Entity.hh"
#include "state/ResourceType.hh"
#include "state/model/entity/UnitModel.hh"

namespace octopus
{

class Unit : public Entity
{
	public:
		Unit(Vector const &pos_p, bool frozen_p, UnitModel const &model_p);

		virtual void runCommands(Step & step_p, State const &state_p, PathManager &pathManager_p) override;

		/// used when harvesting
		ResourceType _typeOfResource { ResourceType::Food };
		double _quantityOfResource { 0. };

		UnitModel const &_unitModel;

		Fixed _aggroDistance = 8;
};

}

#endif
