#ifndef __Player__
#define __Player__

#include <iostream>
#include <vector>
#include "state/ResourceType.hh"

namespace octopus
{
	class Player
	{
		public:
			std::map<ResourceType, double> _resources;

			unsigned long _popCap {0};
			unsigned long _pop; {0}

			unsigned long _id;

			unsigned long _team;
	};
}

#endif
