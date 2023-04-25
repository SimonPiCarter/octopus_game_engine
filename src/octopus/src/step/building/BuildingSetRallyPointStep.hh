#ifndef __BuildingSetRallyPointStep__
#define __BuildingSetRallyPointStep__

#include "state/Handle.hh"
#include "utils/Vector.hh"
#include "step/Steppable.hh"

namespace octopus
{

struct BuildingRallyPointData: public SteppableData
{
	/// @brief true if there is an active rally point
	bool _rallyPointActive;
	/// @brief position of the rally point
	Vector _rallyPoint;
	/// @brief true if there is an active rally point
	bool _rallyPointEntityActive;
	/// @brief entity pointed by the rally point
	Handle _rallyPointEntity;
};

/// @brief this is aimed at setting building rally point
class BuildingSetRallyPointStep : public Steppable
{
	public:
		BuildingSetRallyPointStep(Handle const &handle_p, Vector const &rallyPoint_p, bool rallyPointEntityActive_p, Handle const &rallyPointEntity_p)
            : _handle(handle_p), _rallyPoint(rallyPoint_p), _rallyPointEntityActive(rallyPointEntityActive_p), _rallyPointEntity(rallyPointEntity_p) {}

		virtual void apply(State &state_p) const override;
		virtual void revert(State &state_p, SteppableData const *) const override;

		virtual bool isNoOp() const override { return false; }
		virtual void visit(SteppableVisitor * visitor_p) const override
		{
			visitor_p->visit(this);
		}

        virtual SteppableData * newData(State const &) const;

		Handle const _handle;
		Vector const _rallyPoint;
		bool const _rallyPointEntityActive;
		Handle const _rallyPointEntity;
};
}

#endif
