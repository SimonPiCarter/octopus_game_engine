#ifndef __TeamVisionStep__
#define __TeamVisionStep__

#include <map>

#include "state/Handle.hh"
#include "step/Steppable.hh"
#include "state/vision/PatternHandler.hh"

namespace octopus
{

/// @brief increase or decrease vision for the given VisionPattern
class TeamVisionStep : public Steppable
{
public:
	/// @brief dropped is the qty buffed
	TeamVisionStep(unsigned long team_p, VisionPattern const &pattern_p, bool increase_p, bool exploration_p)
		: _team(team_p), _pattern(pattern_p), _increase(increase_p), _exploration(exploration_p) {}

	virtual void apply(State &state_p) const override;
	virtual void revert(State &state_p, SteppableData const *) const override;

	virtual bool isNoOp() const override;
	virtual void visit(SteppableVisitor * visitor_p) const override
	{
		visitor_p->visit(this);
	}

	/// @brief the team
	unsigned long const _team;
	/// @brief the pattern to give or remove vision to
	VisionPattern const _pattern;
	/// @brief decrease or increase vision/exploration
	bool const _increase;
	/// @brief increase/decrease exploration or vision
	bool const _exploration;
};

} // namespace octopus


#endif
