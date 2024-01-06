#include "OrcaManager.hh"

#include "step/Step.hh"
#include "step/entity/EntityMoveStep.hh"
#include "state/State.hh"
#include "state/entity/Entity.hh"

#include "rvo/RVOSimulator.hh"

namespace octopus
{
OrcaManager::OrcaManager(Fixed timeStep_p, Fixed neighborDist_p, uint32_t maxNeighbors_p, Fixed timeHorizon_p, Fixed timeHorizonObst_p) :
    _timeStep(timeStep_p),
    _neighborDist(neighborDist_p),
    _maxNeighbors(maxNeighbors_p),
    _timeHorizon(timeHorizon_p),
    _timeHorizonObst(timeHorizonObst_p)
{
    _sim = new RVO::RVOSimulator(_mapHandleIdx);
	/// Specify the global time step of the simulation.
	_sim->setTimeStep(_timeStep);

	/// Specify the default parameters for agents that are subsequently added.
	_sim->setAgentDefaults(_neighborDist, _maxNeighbors, _timeHorizon, _timeHorizonObst, Fixed::One(), Fixed(Fixed::OneAsLong()/2, true));
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
    _sim = new RVO::RVOSimulator(_mapHandleIdx);
    _sim->setState(state_p);
	/// Specify the global time step of the simulation.
	_sim->setTimeStep(_timeStep);

	/// Specify the default parameters for agents that are subsequently added.
	_sim->setAgentDefaults(_neighborDist, _maxNeighbors, _timeHorizon, _timeHorizonObst, Fixed::One(), Fixed(Fixed::OneAsLong()/2, true));

    // load obstacle out of map
    _sim->addObstacle({
        RVO::Vector2(state_p.getWorldSize(), 0),
        RVO::Vector2(0, 0),
        RVO::Vector2(0, state_p.getWorldSize()),
        RVO::Vector2(state_p.getWorldSize(), state_p.getWorldSize()),
    });

    for(octopus::Entity const * ent_l : state_p.getEntities())
    {
		if(!ent_l->isActive())
		{
			continue;
		}
        if(ent_l->_model._isStatic)
        {
            // load obstacle
	        uint32_t idx_l = _sim->addObstacle({
                RVO::Vector2(ent_l->_pos.x-ent_l->_model._ray*0.8, ent_l->_pos.y-ent_l->_model._ray*0.8),
                RVO::Vector2(ent_l->_pos.x+ent_l->_model._ray*0.8, ent_l->_pos.y-ent_l->_model._ray*0.8),
                RVO::Vector2(ent_l->_pos.x+ent_l->_model._ray*0.8, ent_l->_pos.y+ent_l->_model._ray*0.8),
                RVO::Vector2(ent_l->_pos.x-ent_l->_model._ray*0.8, ent_l->_pos.y+ent_l->_model._ray*0.8),
            });
            _mapHandleIdx[ent_l->_handle] = idx_l;
        }
        else
        {
            // load agent
		    uint32_t idx_l = _sim->addAgent(RVO::Vector2(ent_l->_pos.x,ent_l->_pos.y));
            _sim->setAgentRadius(idx_l, ent_l->_model._ray);

            _mapHandleIdx[ent_l->_handle] = idx_l;
        }
    }

	//_sim->processObstacles();
}

void OrcaManager::setupStep(State const &state_p, Step &step_p)
{
    _gridStatus = state_p.getPathGridStatus();
    // set or reset position, velocity and weight values
    for(octopus::Entity const * ent_l : state_p.getEntities())
    {
        if(ent_l->_model._isStatic || _mapHandleIdx.find(ent_l->_handle) == _mapHandleIdx.end())
        {
            continue;
        }
        uint32_t idx_l = _mapHandleIdx[ent_l->_handle];
        if(ent_l->isFrozen())
        {
            _sim->setAgentMaxSpeed(idx_l, 0);
        }
        else
        {
            _sim->setAgentMaxSpeed(idx_l, ent_l->_model._stepSpeed);
        }
        _sim->setAgentEntity(idx_l, ent_l);
        _sim->setAgentWeight(idx_l, 1);
        _sim->setAgentPrefVelocity(idx_l, RVO::Vector2(0, 0));
        _sim->setAgentPosition(idx_l, RVO::Vector2(ent_l->_pos.x, ent_l->_pos.y));
    }

    // register move steps
	std::vector<EntityMoveStep *> mapMoveStep_l(state_p.getEntities().size(), nullptr);
    for(octopus::EntityMoveStep *moveStep_l : step_p.getEntityMoveStep())
    {
        if(skipCollision(state_p.getEntity(moveStep_l->_handle)))
        {
            continue;
        }
        mapMoveStep_l[moveStep_l->_handle.index] = moveStep_l;
    }

	// fill up move steps when missing
    // also update weights to be lower
	for(Entity const * ent_l : state_p.getEntities())
	{
        if(skipCollision(ent_l) || _mapHandleIdx.find(ent_l->_handle) == _mapHandleIdx.end())
        {
            continue;
        }
		if(mapMoveStep_l[ent_l->_handle.index] == nullptr && ent_l->isActive() && !ent_l->isFrozen())
		{
            uint32_t idx_l = _mapHandleIdx[ent_l->_handle];
            _sim->setAgentWeight(idx_l, 0.01);

			EntityMoveStep *step_l = new EntityMoveStep(ent_l->_handle, {0, 0});
			step_p.addEntityMoveStep(step_l);
			mapMoveStep_l[ent_l->_handle.index] = step_l;
		}
	}

    // set move step and prefered velocity from move steps
    for(octopus::EntityMoveStep *moveStep_l : step_p.getEntityMoveStep())
    {
        if(skipCollision(state_p.getEntity(moveStep_l->_handle)) || _mapHandleIdx.find(moveStep_l->_handle) == _mapHandleIdx.end())
        {
            continue;
        }
        uint32_t idx_l = _mapHandleIdx[moveStep_l->_handle];
        _sim->setAgentMoveStep(idx_l, moveStep_l);
        _sim->setAgentPrefVelocity(idx_l, RVO::Vector2(moveStep_l->_move.x, moveStep_l->_move.y));
    }

    // set known velocity based on last move step
    for(octopus::EntityMoveStep const *moveStep_l : step_p.getPrev()->getEntityMoveStep())
    {
        if(skipCollision(state_p.getEntity(moveStep_l->_handle)) || _mapHandleIdx.find(moveStep_l->_handle) == _mapHandleIdx.end())
        {
            continue;
        }
        uint32_t idx_l = _mapHandleIdx[moveStep_l->_handle];
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
        if(skipCollision(state_p.getEntity(moveStep_l->_handle))
        || _mapHandleIdx.find(moveStep_l->_handle) == _mapHandleIdx.end())
        {
            continue;
        }
        uint32_t idx_l = _mapHandleIdx[moveStep_l->_handle];
        moveStep_l->_move.x = _sim->getAgentVelocity(idx_l).x();
        moveStep_l->_move.y = _sim->getAgentVelocity(idx_l).y();
    }
}

bool OrcaManager::ShouldReset(OrcaManager const *manager_p, State const &state_p, Step const &step_p)
{
    if(!manager_p)
    {
        return true;
    }
    if(step_p.getEntitySpawned() > 0)
    {
        return true;
    }
    if(manager_p->_gridStatus < state_p.getPathGridStatus())
    {
        return true;
    }
    return false;
}

std::vector<std::pair<octopus::Fixed , const RVO::Agent *> > const & OrcaManager::getAgentNeighbors(octopus::Handle const &handle_p) const
{
    if(!_sim)
    {
        static std::vector<std::pair<octopus::Fixed , const RVO::Agent *> > res_l;
        return res_l;
    }
    return _sim->getAgentNeighbors(_mapHandleIdx.at(handle_p));
}


} // namespace octopus
