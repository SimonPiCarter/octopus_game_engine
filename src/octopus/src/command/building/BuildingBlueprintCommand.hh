#ifndef __BuildingBlueprintCommand__
#define __BuildingBlueprintCommand__

#include "command/Command.hh"

#include "state/Handle.hh"
#include "utils/Vector.hh"

namespace octopus
{
struct BuildingModel;;

/// @brief This command will spawn a blueprint after a resource check
class BuildingBlueprintCommand : public Command
{
public:
	BuildingBlueprintCommand(Vector const &pos_p,
		unsigned long player_p, BuildingModel const &model_p);

	virtual void registerCommand(Step & step_p, State const &state_p) override;

	/// @brief
	virtual bool applyCommand(Step & step_p, State const &state_p, CommandData const *data_p) const override;

	virtual CommandData * newData() const override { return nullptr; }

private:
	Vector const _pos;
	unsigned long const _player;

	BuildingModel const &_model;
};

} // namespace octopus


#endif
