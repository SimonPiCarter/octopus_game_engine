#ifndef __PlayerChoseOptionCommand__
#define __PlayerChoseOptionCommand__

#include "command/Command.hh"
#include <string>

#include "state/Handle.hh"
#include "state/DivinityType.hh"

namespace octopus
{
struct BuildingModel;

/// @brief This command will update the divinity chosen and eventually lvling it up
class PlayerChoseOptionCommand : public Command
{
public:
	PlayerChoseOptionCommand() {}
	PlayerChoseOptionCommand(unsigned long player_p, std::string const &key_p, unsigned long option_p);

	virtual void registerCommand(Step & step_p, State const &state_p) override;

	/// @brief
	virtual bool applyCommand(Step & step_p, State const &state_p, CommandData const *data_p, PathManager &pathManager_p) const override;

	virtual bool checkPlayer(State const &, unsigned long player_p) const override { return _player == player_p; }

	unsigned long getPlayer() const { return _player; }
	std::string const &getKey() const { return _key; }
	unsigned long getOption() const { return _option; }

private:
	unsigned long _player {0};
	std::string _key {""};
	unsigned long _option {0};
};

} // namespace octopus


#endif
