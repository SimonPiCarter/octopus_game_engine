#include "OrcaManager.hh"

#include "step/Step.hh"
#include "step/entity/EntityMoveStep.hh"
#include "state/State.hh"
#include "state/entity/Entity.hh"

#include "rvo/RVOSimulator.hh"

namespace octopus
{
OrcaManager::OrcaManager(double timeStep_p, double neighborDist_p, size_t maxNeighbors_p, double timeHorizon_p, double timeHorizonObst_p) :
    _timeStep(timeStep_p),
    _neighborDist(neighborDist_p),
    _maxNeighbors(maxNeighbors_p),
    _timeHorizon(timeHorizon_p),
    _timeHorizonObst(timeHorizonObst_p)
{
    _sim = new RVO::RVOSimulator();
	/// Specify the global time step of the simulation.
	_sim->setTimeStep(_timeStep);

	/// Specify the default parameters for agents that are subsequently added.
	_sim->setAgentDefaults(_neighborDist, _maxNeighbors, _timeHorizon, _timeHorizonObst, 1.0, 0.5);
}

OrcaManager::~OrcaManager()
{
    delete _sim;
}

bool skipCollision(octopus::Entity const *ent_p)
{
    return !ent_p->_alive || ent_p->isIgnoringCollision();
}

void OrcaManager::resetFromState(State const &state_p)
{
    // tear down sim
    delete _sim;

    _mapHandleIdx.clear();

    // new sim
    _sim = new RVO::RVOSimulator();
	/// Specify the global time step of the simulation.
	_sim->setTimeStep(_timeStep);

	/// Specify the default parameters for agents that are subsequently added.
	_sim->setAgentDefaults(_neighborDist, _maxNeighbors, _timeHorizon, _timeHorizonObst, 1.0, 0.5);

    for(octopus::Entity const * ent_l : state_p.getEntities())
    {
        if(skipCollision(ent_l))
        {
            continue;
        }
        if(ent_l->_model._isStatic)
        {
            // load obstacle
	        _sim->addObstacle({
                RVO::Vector2(ent_l->_pos.x-ent_l->_model._ray, ent_l->_pos.y-ent_l->_model._ray),
                RVO::Vector2(ent_l->_pos.x+ent_l->_model._ray, ent_l->_pos.y-ent_l->_model._ray),
                RVO::Vector2(ent_l->_pos.x+ent_l->_model._ray, ent_l->_pos.y+ent_l->_model._ray),
                RVO::Vector2(ent_l->_pos.x-ent_l->_model._ray, ent_l->_pos.y+ent_l->_model._ray),
            });
        }
        else
        {
            // load agent
		    size_t idx_l = _sim->addAgent(RVO::Vector2(ent_l->_pos.x,ent_l->_pos.y));
            _sim->setAgentRadius(idx_l, ent_l->_model._ray);

            _mapHandleIdx[ent_l->_handle] = idx_l;
        }
    }

    // load obstacle
    _sim->addObstacle({
        RVO::Vector2(state_p.getWorldSize(), 0),
        RVO::Vector2(0, 0),
        RVO::Vector2(0, state_p.getWorldSize()),
        RVO::Vector2(state_p.getWorldSize(), state_p.getWorldSize()),
    });

	_sim->processObstacles();
}

void OrcaManager::setupStep(State const &state_p, Step &step_p)
{
    for(octopus::Entity const * ent_l : state_p.getEntities())
    {
        if(skipCollision(ent_l) || ent_l->_model._isStatic)
        {
            continue;
        }
        size_t idx_l = _mapHandleIdx[ent_l->_handle];
        if(ent_l->isFrozen())
        {
            _sim->setAgentMaxSpeed(idx_l, 0);
        }
        else
        {
            _sim->setAgentMaxSpeed(idx_l, ent_l->_model._stepSpeed);
        }
        _sim->setAgentPrefVelocity(idx_l, RVO::Vector2(0, 0));
        _sim->setAgentPosition(idx_l, RVO::Vector2(ent_l->_pos.x, ent_l->_pos.y));
    }

	std::vector<EntityMoveStep *> mapMoveStep_l(state_p.getEntities().size(), nullptr);
    for(octopus::EntityMoveStep *moveStep_l : step_p.getEntityMoveStep())
    {
        if(skipCollision(state_p.getEntity(moveStep_l->_handle)))
        {
            continue;
        }
        mapMoveStep_l[moveStep_l->_handle] = moveStep_l;
    }

	// fill up move steps when missing
	for(Entity const * ent_l : state_p.getEntities())
	{
        if(skipCollision(ent_l))
        {
            continue;
        }
		if(mapMoveStep_l[ent_l->_handle] == nullptr && ent_l->isActive() && !ent_l->isFrozen())
		{
			EntityMoveStep *step_l = new EntityMoveStep(ent_l->_handle, {0, 0});
			step_p.addEntityMoveStep(step_l);
			mapMoveStep_l[ent_l->_handle] = step_l;
		}
	}

    for(octopus::EntityMoveStep *moveStep_l : step_p.getEntityMoveStep())
    {
        if(skipCollision(state_p.getEntity(moveStep_l->_handle)))
        {
            continue;
        }
        size_t idx_l = _mapHandleIdx[moveStep_l->_handle];
        _sim->setAgentMoveStep(idx_l, moveStep_l);
        _sim->setAgentPrefVelocity(idx_l, RVO::Vector2(moveStep_l->_move.x, moveStep_l->_move.y));
    }

    for(octopus::EntityMoveStep const *moveStep_l : step_p.getPrev()->getEntityMoveStep())
    {
        if(skipCollision(state_p.getEntity(moveStep_l->_handle)))
        {
            continue;
        }
        size_t idx_l = _mapHandleIdx[moveStep_l->_handle];
        _sim->setAgentVelocity(idx_l, RVO::Vector2(moveStep_l->_move.x, moveStep_l->_move.y));
    }
}

void OrcaManager::doStep()
{
    _sim->doStep();
}

void OrcaManager::commitStep(State const &state_p, Step &step_p)
{
    for(octopus::EntityMoveStep *moveStep_l : step_p.getEntityMoveStep())
    {
        if(skipCollision(state_p.getEntity(moveStep_l->_handle)))
        {
            continue;
        }
        size_t idx_l = _mapHandleIdx[moveStep_l->_handle];
        moveStep_l->_move.x = _sim->getAgentVelocity(idx_l).x();
        moveStep_l->_move.y = _sim->getAgentVelocity(idx_l).y();
    }
}

} // namespace octopus
