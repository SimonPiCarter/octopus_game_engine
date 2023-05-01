#ifndef __BuildingBlueprintCommand__
#define __BuildingBlueprintCommand__

#include <vector>

#include "command/Command.hh"

#include "state/Handle.hh"
#include "utils/Vector.hh"

namespace octopus
{
struct BuildingModel;

/// @brief This command will spawn a blueprint after a resource check
/// can also spawn command for builders to build the building directly
class BuildingBlueprintCommand : public Command
{
public:
	BuildingBlueprintCommand(Vector const &pos_p, unsigned long player_p, BuildingModel const &model_p,
		std::vector<Handle> const &builders_p = {});

	virtual void registerCommand(Step & step_p, State const &state_p) override;

	/// @brief
	virtual bool applyCommand(Step & step_p, State const &state_p, CommandData const *data_p, PathManager &pathManager_p) const override;

	virtual CommandData * newData() const override { return nullptr; }

	virtual bool checkPlayer(State const &, unsigned long player_p) const override { return _player == player_p; }

	Vector const &getPos() const { return _pos; }
	unsigned long getPlayer() const { return _player; }
	BuildingModel const & getModel() const { return _model; }
	std::vector<Handle> const & getBuilders() const { return _builders; }

private:
	Vector const _pos;
	unsigned long const _player;

	BuildingModel const &_model;

	std::vector<Handle> const _builders;
};

} // namespace octopus


#endif
