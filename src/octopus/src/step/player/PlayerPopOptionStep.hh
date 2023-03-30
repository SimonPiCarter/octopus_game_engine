#ifndef __PlayerPopOptionStep__
#define __PlayerPopOptionStep__

#include <map>
#include <string>
#include <vector>
#include "step/Steppable.hh"
#include "state/player/StepOptionsGenerator.hh"

namespace octopus
{

class PlayerPopOptionStepData : public SteppableData
{
public:
	~PlayerPopOptionStepData();

	StepOptionsGenerator * _generator;
	std::vector<SteppableData *> _data;
};

/// @brief add options for the given divinity types for the player
class PlayerPopOptionStep : public Steppable
{
public:
	PlayerPopOptionStep(unsigned long player_p, std::string const &key_p, unsigned long choice_p)
		: _player(player_p), _key(key_p), _choice(choice_p) {}

	virtual void apply(State &state_p) const override;
	virtual void revert(State &state_p, SteppableData const *data_p) const override;

	virtual bool isNoOp() const override { return false; }
	virtual void visit(SteppableVisitor * visitor_p) const override
	{
		visitor_p->visit(this);
	}

	virtual SteppableData * newData(State const &state_p) const;

	unsigned long const _player;
	std::string const _key;
	unsigned long const _choice;
};

} // namespace octopus


#endif
