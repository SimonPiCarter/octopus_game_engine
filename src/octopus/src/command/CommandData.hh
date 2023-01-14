#ifndef __CommandData__
#define __CommandData__

namespace octopus
{

/// @brief empty class to be used to store command data in Commandable
class CommandData
{
public:
	virtual ~CommandData() {}
};

template<typename data>
class CommandDataWithData : public CommandData
{
public:
	CommandDataWithData() {}
	CommandDataWithData(data const &data_p) : _data(data_p) {}

	data _data;
};

} // namespace octopus


#endif
