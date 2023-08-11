#ifndef __ResourceModel__
#define __ResourceModel__

#include "EntityModel.hh"
#include "utils/Vector.hh"

#include <vector>

namespace octopus
{

	struct ResourceModel : public EntityModel
	{
		ResourceModel(Fixed ray_p, std::string const &type_p, Fixed qty_p)
			: EntityModel(true, ray_p, 0, 1000)
			, _type(type_p)
			, _qty(qty_p)
		{
			_isResource = true;
			_invulnerable = true;
		}

		Fixed _qty {0.};
		std::string _type { "bloc" };

		std::vector<Vector> _harvestPoint;
	};

} // octopus

#endif