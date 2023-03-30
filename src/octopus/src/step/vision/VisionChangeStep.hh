#ifndef __VisionChangeStep__
#define __VisionChangeStep__

#include "step/Steppable.hh"
#include <unordered_map>
#include <list>

namespace octopus
{
class Step;
class VisionHandler;

class VisionChangeStep : public Steppable
{
public:
	/// @brief construct step using a movable map (to avoid any copy)
	VisionChangeStep(unsigned long team_p, std::unordered_map<size_t , std::unordered_map<size_t, long long> > &&delta_p, bool exploration_p=false);

	virtual void apply(State &state_p) const override;
	virtual void revert(State &state_p, SteppableData const *) const override;

	virtual bool isNoOp() const override
	{
		return _delta.empty();
	}
	virtual void visit(SteppableVisitor * visitor_p) const override
	{
		visitor_p->visit(this);
	}
private:
	unsigned long const _team;

	/// @brief sparse storage of the delta to be applied
	std::unordered_map<size_t , std::unordered_map<size_t, long long> > _delta;

	bool const _exploration;
};

/// @brief create VisionChangeStep from given step
std::list<VisionChangeStep *> newVisionChangeStep(State const &state_p, Step const &step_p, VisionHandler &handler_p);

}

#endif
