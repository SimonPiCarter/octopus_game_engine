/*
 * RVOSimulator.cpp
 * RVO2 Library
 *
 * Copyright 2008 University of North Carolina at Chapel Hill
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Please send all bug reports to <geom@cs.unc.edu>.
 *
 * The authors may be contacted via:
 *
 * Jur van den Berg, Stephen J. Guy, Jamie Snape, Ming C. Lin, Dinesh Manocha
 * Dept. of Computer Science
 * 201 S. Columbia St.
 * Frederick P. Brooks, Jr. Computer Science Bldg.
 * Chapel Hill, N.C. 27599-3175
 * United States of America
 *
 * <http://gamma.cs.unc.edu/RVO2/>
 */

#include "RVOSimulator.hh"

#include "Agent.hh"
#include "KdTree.hh"
#include "Obstacle.hh"

#ifdef _OPENMP
#include <omp.h>
#endif
#include <thread>
#include <condition_variable>

namespace RVO {
	RVOSimulator::RVOSimulator(std::unordered_map<octopus::Handle, uint32_t> const &mapHandleIdx_p) :
		pool(12), defaultAgent_(NULL), globalTime_(0.0f), kdTree_(NULL), timeStep_(0.0f), mapHandleIdx_(mapHandleIdx_p)
	{
		kdTree_ = new KdTree(this);
	}

	std::vector<std::pair<octopus::Fixed , const Agent *> > const & RVOSimulator::getAgentNeighbors(unsigned long idx_p) const
	{
		return agents_[idx_p].getAgentNeighbors();
	}

	RVOSimulator::RVOSimulator(octopus::Fixed timeStep, octopus::Fixed neighborDist, uint32_t maxNeighbors,
		octopus::Fixed timeHorizon, octopus::Fixed timeHorizonObst, octopus::Fixed radius, octopus::Fixed maxSpeed,
		std::unordered_map<octopus::Handle, uint32_t> const &mapHandleIdx_p, const Vector2 &velocity) :
		pool(12), defaultAgent_(NULL), globalTime_(0.0f), kdTree_(NULL), timeStep_(timeStep), mapHandleIdx_(mapHandleIdx_p)
	{
		kdTree_ = new KdTree(this);
		defaultAgent_ = new Agent(this);

		defaultAgent_->maxNeighbors_ = maxNeighbors;
		defaultAgent_->maxSpeed_ = maxSpeed;
		defaultAgent_->neighborDist_ = neighborDist;
		defaultAgent_->radius_ = radius;
		defaultAgent_->timeHorizon_ = timeHorizon;
		defaultAgent_->timeHorizonObst_ = timeHorizonObst;
		defaultAgent_->velocity_ = velocity;
	}

	RVOSimulator::~RVOSimulator()
	{
		if (defaultAgent_ != NULL) {
			delete defaultAgent_;
		}

		for (uint32_t i = 0; i < obstacles_.size(); ++i) {
			delete obstacles_[i];
		}

		pool.stop();
		delete kdTree_;
	}

	uint32_t RVOSimulator::addAgent(const Vector2 &position)
	{
		if (defaultAgent_ == NULL) {
			return RVO_ERROR;
		}

		agents_.emplace_back(this);
		Agent *agent = &agents_.back();

		agent->position_ = position;
		agent->maxNeighbors_ = defaultAgent_->maxNeighbors_;
		agent->maxSpeed_ = defaultAgent_->maxSpeed_;
		agent->neighborDist_ = defaultAgent_->neighborDist_;
		agent->radius_ = defaultAgent_->radius_;
		agent->timeHorizon_ = defaultAgent_->timeHorizon_;
		agent->timeHorizonObst_ = defaultAgent_->timeHorizonObst_;
		agent->velocity_ = defaultAgent_->velocity_;

		agent->id_ = agents_.size() - 1;


		return agents_.size() - 1;
	}

	uint32_t RVOSimulator::addAgent(const Vector2 &position, octopus::Fixed neighborDist, uint32_t maxNeighbors, octopus::Fixed timeHorizon, octopus::Fixed timeHorizonObst, octopus::Fixed radius, octopus::Fixed maxSpeed, const Vector2 &velocity)
	{
		agents_.emplace_back(this);
		Agent *agent = &agents_.back();

		agent->position_ = position;
		agent->maxNeighbors_ = maxNeighbors;
		agent->maxSpeed_ = maxSpeed;
		agent->neighborDist_ = neighborDist;
		agent->radius_ = radius;
		agent->timeHorizon_ = timeHorizon;
		agent->timeHorizonObst_ = timeHorizonObst;
		agent->velocity_ = velocity;

		agent->id_ = agents_.size() - 1;

		return agents_.size() - 1;
	}

	uint32_t RVOSimulator::addObstacle(const std::vector<Vector2> &vertices)
	{
		if (vertices.size() < 2) {
			return RVO_ERROR;
		}

		const uint32_t obstacleNo = obstacles_.size();

		for (uint32_t i = 0; i < vertices.size(); ++i) {
			Obstacle *obstacle = new Obstacle();
			obstacle->point_ = vertices[i];

			if (i != 0) {
				obstacle->prevObstacle_ = obstacles_.back();
				obstacle->prevObstacle_->nextObstacle_ = obstacle;
			}

			if (i == vertices.size() - 1) {
				obstacle->nextObstacle_ = obstacles_[obstacleNo];
				obstacle->nextObstacle_->prevObstacle_ = obstacle;
			}

			obstacle->unitDir_ = normalize(vertices[(i == vertices.size() - 1 ? 0 : i + 1)] - vertices[i]);

			if (vertices.size() == 2) {
				obstacle->isConvex_ = true;
			}
			else {
				obstacle->isConvex_ = (leftOf(vertices[(i == 0 ? vertices.size() - 1 : i - 1)], vertices[i], vertices[(i == vertices.size() - 1 ? 0 : i + 1)]) >= 0.0f);
			}

			obstacle->id_ = obstacles_.size();

			obstacles_.push_back(obstacle);
		}

		return obstacleNo;
	}

	void RVOSimulator::doStep()
	{
		kdTree_->buildAgentTree();

		int nbThreads_l = 12;
		std::vector<int> indexes_l;

		for(int i = 0 ; i < nbThreads_l ; ++ i)
		{
			indexes_l.push_back(i*static_cast<int>(agents_.size()/nbThreads_l));
		}
		indexes_l.push_back(static_cast<int>(agents_.size()));

		int finished_l = 0;
		std::mutex terminationMutex_l;
		std::condition_variable termination_l;
		std::unique_lock<std::mutex> lock_l(terminationMutex_l);

		for(int n = 0 ; n < nbThreads_l ; ++ n)
		{
			int start_l = indexes_l.at(n);
			int end_l = indexes_l.at(n+1);
			pool.queueJob([&finished_l, &termination_l, &terminationMutex_l, start_l, end_l, this]()
			{
				for (int i = start_l; i < end_l; ++i)
				{
					if(agents_[i].active())
					{
						agents_[i].computeNeighbors();
						agents_[i].computeNewVelocity();
					}
				}
				std::unique_lock<std::mutex> lock_l(terminationMutex_l);
				finished_l++;
				termination_l.notify_all();
			}
			);
		}

		if(finished_l < nbThreads_l)
		{
			termination_l.wait(lock_l, [&]{ return finished_l >= nbThreads_l ; });
		}

		for (int i = 0; i < static_cast<int>(agents_.size()); ++i)
		{
			if(agents_[i].active())
			{
				agents_[i].update();
			}
		}

		globalTime_ += timeStep_;
	}

	uint32_t RVOSimulator::getAgentAgentNeighbor(uint32_t agentNo, uint32_t neighborNo) const
	{
		return agents_[agentNo].agentNeighbors_[neighborNo].second->id_;
	}

	uint32_t RVOSimulator::getAgentMaxNeighbors(uint32_t agentNo) const
	{
		return agents_[agentNo].maxNeighbors_;
	}

	octopus::Fixed RVOSimulator::getAgentMaxSpeed(uint32_t agentNo) const
	{
		return agents_[agentNo].maxSpeed_;
	}

	octopus::Fixed RVOSimulator::getAgentNeighborDist(uint32_t agentNo) const
	{
		return agents_[agentNo].neighborDist_;
	}

	uint32_t RVOSimulator::getAgentNumAgentNeighbors(uint32_t agentNo) const
	{
		return agents_[agentNo].agentNeighbors_.size();
	}

	uint32_t RVOSimulator::getAgentNumObstacleNeighbors(uint32_t agentNo) const
	{
		return agents_[agentNo].obstacleNeighbors_.size();
	}

	uint32_t RVOSimulator::getAgentNumORCALines(uint32_t agentNo) const
	{
		return agents_[agentNo].orcaLines_.size();
	}

	uint32_t RVOSimulator::getAgentObstacleNeighbor(uint32_t agentNo, uint32_t neighborNo) const
	{
		return agents_[agentNo].obstacleNeighbors_[neighborNo].second->id_;
	}

	const Line &RVOSimulator::getAgentORCALine(uint32_t agentNo, uint32_t lineNo) const
	{
		return agents_[agentNo].orcaLines_[lineNo];
	}

	const Vector2 &RVOSimulator::getAgentPosition(uint32_t agentNo) const
	{
		return agents_[agentNo].position_;
	}

	octopus::EntityMoveStep * RVOSimulator::getAgentMoveStep(uint32_t agentNo)
	{
		return agents_[agentNo].moveStep_;
	}

	const Vector2 &RVOSimulator::getAgentPrefVelocity(uint32_t agentNo) const
	{
		return agents_[agentNo].prefVelocity_;
	}

	octopus::Fixed RVOSimulator::getAgentRadius(uint32_t agentNo) const
	{
		return agents_[agentNo].radius_;
	}

	octopus::Fixed RVOSimulator::getAgentTimeHorizon(uint32_t agentNo) const
	{
		return agents_[agentNo].timeHorizon_;
	}

	octopus::Fixed RVOSimulator::getAgentTimeHorizonObst(uint32_t agentNo) const
	{
		return agents_[agentNo].timeHorizonObst_;
	}

	const Vector2 &RVOSimulator::getAgentVelocity(uint32_t agentNo) const
	{
		return agents_[agentNo].velocity_;
	}

	octopus::Fixed RVOSimulator::getGlobalTime() const
	{
		return globalTime_;
	}

	uint32_t RVOSimulator::getNumAgents() const
	{
		return agents_.size();
	}

	uint32_t RVOSimulator::getNumObstacleVertices() const
	{
		return obstacles_.size();
	}

	const Vector2 &RVOSimulator::getObstacleVertex(uint32_t vertexNo) const
	{
		return obstacles_[vertexNo]->point_;
	}

	uint32_t RVOSimulator::getNextObstacleVertexNo(uint32_t vertexNo) const
	{
		return obstacles_[vertexNo]->nextObstacle_->id_;
	}

	uint32_t RVOSimulator::getPrevObstacleVertexNo(uint32_t vertexNo) const
	{
		return obstacles_[vertexNo]->prevObstacle_->id_;
	}

	octopus::Fixed RVOSimulator::getTimeStep() const
	{
		return timeStep_;
	}

	void RVOSimulator::processObstacles()
	{
		kdTree_->buildObstacleTree();
	}

	bool RVOSimulator::queryVisibility(const Vector2 &point1, const Vector2 &point2, octopus::Fixed radius) const
	{
		return kdTree_->queryVisibility(point1, point2, radius);
	}

	void RVOSimulator::setAgentDefaults(octopus::Fixed neighborDist, uint32_t maxNeighbors, octopus::Fixed timeHorizon, octopus::Fixed timeHorizonObst, octopus::Fixed radius, octopus::Fixed maxSpeed, const Vector2 &velocity)
	{
		if (defaultAgent_ == NULL) {
			defaultAgent_ = new Agent(this);
		}

		defaultAgent_->maxNeighbors_ = maxNeighbors;
		defaultAgent_->maxSpeed_ = maxSpeed;
		defaultAgent_->neighborDist_ = neighborDist;
		defaultAgent_->radius_ = radius;
		defaultAgent_->timeHorizon_ = timeHorizon;
		defaultAgent_->timeHorizonObst_ = timeHorizonObst;
		defaultAgent_->velocity_ = velocity;
	}

	void RVOSimulator::setAgentMaxNeighbors(uint32_t agentNo, uint32_t maxNeighbors)
	{
		agents_[agentNo].maxNeighbors_ = maxNeighbors;
	}

	void RVOSimulator::setAgentMaxSpeed(uint32_t agentNo, octopus::Fixed maxSpeed)
	{
		agents_[agentNo].maxSpeed_ = maxSpeed;
	}

	void RVOSimulator::setAgentNeighborDist(uint32_t agentNo, octopus::Fixed neighborDist)
	{
		agents_[agentNo].neighborDist_ = neighborDist;
	}

	void RVOSimulator::setAgentPosition(uint32_t agentNo, const Vector2 &position)
	{
		agents_[agentNo].position_ = position;
	}

	void RVOSimulator::setAgentMoveStep(uint32_t agentNo, octopus::EntityMoveStep *moveStep)
	{
		agents_[agentNo].moveStep_ = moveStep;
	}

	void RVOSimulator::setAgentEntity(uint32_t agentNo, octopus::Entity const *ent)
	{
		agents_[agentNo].ent_ = ent;
	}

	void RVOSimulator::setAgentWeight(uint32_t agentNo, octopus::Fixed weight)
	{
		agents_[agentNo].weight_ = weight;
	}

	void RVOSimulator::setAgentPrefVelocity(uint32_t agentNo, const Vector2 &prefVelocity)
	{
		agents_[agentNo].prefVelocity_ = prefVelocity;
	}

	void RVOSimulator::setAgentRadius(uint32_t agentNo, octopus::Fixed radius)
	{
		agents_[agentNo].radius_ = radius;
	}

	void RVOSimulator::setAgentTimeHorizon(uint32_t agentNo, octopus::Fixed timeHorizon)
	{
		agents_[agentNo].timeHorizon_ = timeHorizon;
	}

	void RVOSimulator::setAgentTimeHorizonObst(uint32_t agentNo, octopus::Fixed timeHorizonObst)
	{
		agents_[agentNo].timeHorizonObst_ = timeHorizonObst;
	}

	void RVOSimulator::setAgentVelocity(uint32_t agentNo, const Vector2 &velocity)
	{
		agents_[agentNo].velocity_ = velocity;
	}

	void RVOSimulator::setTimeStep(octopus::Fixed timeStep)
	{
		timeStep_ = timeStep;
	}
}
