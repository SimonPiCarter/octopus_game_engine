#ifndef __ProjectileMoveStep__
#define __ProjectileMoveStep__

#include "state/projectile/Projectile.hh"
#include "step/Steppable.hh"
#include <vector>

namespace octopus
{

/// @brief
class ProjectileMoveStep : public Steppable
{
public:
	virtual void apply(State &state_p) const override;
	virtual void revert(State &state_p, SteppableData const *) const override;

	virtual bool isNoOp() const override;
	virtual void visit(SteppableVisitor * visitor_p) const override
	{
		visitor_p->visit(this);
	}

	void setOver(uint32_t idx_p);
	void setMove(uint32_t idx_p, Vector &&vec_p);

	std::vector<Vector> const &getMove() const { return _move; }
	std::vector<bool> const &getOver() const { return _over; }

	void merge(ProjectileMoveStep &&other_p);
private:
	std::vector<Vector> _move;
	std::vector<bool> _over;
};

} // namespace octopus


#endif // __ProjectileMoveStep__