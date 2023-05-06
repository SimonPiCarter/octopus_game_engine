#ifndef __CommandData__
#define __CommandData__

namespace octopus
{

/// @brief empty class to be used to store command data in Commandable
class CommandData
{
public:
	virtual ~CommandData() {}

	bool _ignoreCollision {false};
};

} // namespace octopus


#endif
