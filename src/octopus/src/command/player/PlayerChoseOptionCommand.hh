#ifndef __PlayerChoseOptionCommand__
#define __PlayerChoseOptionCommand__

#include "command/Command.hh"
#include <string>

#include "state/Handle.hh"
#include "state/DivinityType.hh"

namespace octopus
{
struct BuildingModel;;

/// @brief This command will update the divinity chosen and eventually lvling it up
class PlayerChoseOptionCommand : public Command
{
public:
	PlayerChoseOptionCommand(unsigned long player_p, std::string const &key_p, unsigned long option_p);

	virtual void registerCommand(Step & step_p, State const &state_p) override;

	/// @brief
	virtual bool applyCommand(Step & step_p, State const &state_p, CommandData const *data_p, PathManager &pathManager_p) const override;

	virtual CommandData * newData() const override { return nullptr; }

	virtual bool checkPlayer(State const &, unsigned long player_p) const override { return _player == player_p; }

private:
	unsigned long const _player;
	std::string const _key;
	unsigned long const _option;
};

} // namespace octopus


#endif
