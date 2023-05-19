#ifndef __PlayerChoseDivinityCommand__
#define __PlayerChoseDivinityCommand__

#include "command/Command.hh"

#include "state/Handle.hh"
#include <string>

namespace octopus
{
struct BuildingModel;;

/// @brief This command will update the divinity chosen and eventually lvling it up
class PlayerChoseDivinityCommand : public Command
{
public:
	PlayerChoseDivinityCommand() {}
	PlayerChoseDivinityCommand(unsigned long player_p, std::string type_p);

	virtual void registerCommand(Step & step_p, State const &state_p) override;

	/// @brief
	virtual bool applyCommand(Step & step_p, State const &state_p, CommandData const *data_p, PathManager &pathManager_p) const override;

	virtual bool checkPlayer(State const &, unsigned long player_p) const override { return _player == player_p; }

	unsigned long getPlayer() const { return _player; }
	std::string getType() const { return _type; }

private:
	unsigned long _player = 0;
	std::string _type;
};

} // namespace octopus


#endif
