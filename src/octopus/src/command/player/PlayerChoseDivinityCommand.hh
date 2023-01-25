#ifndef __PlayerChoseDivinityCommand__
#define __PlayerChoseDivinityCommand__

#include "command/Command.hh"

#include "state/Handle.hh"
#include "state/DivinityType.hh"

namespace octopus
{
class BuildingModel;

/// @brief This command will update the divinity chosen and eventually lvling it up
class PlayerChoseDivinityCommand : public Command
{
public:
	PlayerChoseDivinityCommand(Handle const &commandHandle_p, unsigned long player_p, DivinityType type_p, bool lvlUp_p);

	/// @brief
	virtual bool applyCommand(Step & step_p, State const &state_p, CommandData const *data_p) const override;

	virtual CommandData * newData() const override { return nullptr; }

private:
	unsigned long const _player;
	DivinityType const _type;
	bool const _lvlUp;
};

} // namespace octopus


#endif
