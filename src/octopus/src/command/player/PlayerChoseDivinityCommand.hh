#ifndef __PlayerChoseDivinityCommand__
#define __PlayerChoseDivinityCommand__

#include "command/Command.hh"

#include "state/Handle.hh"
#include "state/DivinityType.hh"

namespace octopus
{
struct BuildingModel;;

/// @brief This command will update the divinity chosen and eventually lvling it up
class PlayerChoseDivinityCommand : public Command
{
public:
	PlayerChoseDivinityCommand(unsigned long player_p, DivinityType type_p);

	virtual void registerCommand(Step & step_p, State const &state_p) override;

	/// @brief
	virtual bool applyCommand(Step & step_p, State const &state_p, CommandData const *data_p) const override;

	virtual CommandData * newData() const override { return nullptr; }

private:
	unsigned long const _player;
	DivinityType const _type;
};

} // namespace octopus


#endif
