/*
 * KdTree.cpp
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

#include "KdTree.hh"

#include "Agent.hh"
#include "RVOSimulator.hh"
#include "Obstacle.hh"

#include "state/State.hh"
#include "state/entity/Entity.hh"
#include "graph/GridNode.hh"
#include "graph/Grid.hh"
#include "utils/Box.hh"

namespace RVO {
	KdTree::KdTree(RVOSimulator *sim) : obstacleTree_(NULL), sim_(sim) { }

	KdTree::~KdTree()
	{
		deleteObstacleTree(obstacleTree_);
	}

	void KdTree::buildAgentTree()
	{
		agents_.clear();
		for (size_t i = agents_.size(); i < sim_->agents_.size(); ++i)
		{
			if(sim_->agents_[i].active())
			{
				agents_.push_back(&sim_->agents_[i]);
			}
		}

		if (!agents_.empty()) {
			agentTree_.resize(2 * agents_.size() - 1);
			buildAgentTreeRecursive(0, agents_.size(), 0);
		}
	}

	void KdTree::buildAgentTreeRecursive(size_t begin, size_t end, size_t node)
	{
		agentTree_[node].begin = begin;
		agentTree_[node].end = end;
		agentTree_[node].minX = agentTree_[node].maxX = agents_[begin]->position_.x();
		agentTree_[node].minY = agentTree_[node].maxY = agents_[begin]->position_.y();

		for (size_t i = begin + 1; i < end; ++i) {
			agentTree_[node].maxX = std::max(agentTree_[node].maxX, agents_[i]->position_.x());
			agentTree_[node].minX = std::min(agentTree_[node].minX, agents_[i]->position_.x());
			agentTree_[node].maxY = std::max(agentTree_[node].maxY, agents_[i]->position_.y());
			agentTree_[node].minY = std::min(agentTree_[node].minY, agents_[i]->position_.y());
		}

		if (end - begin > MAX_LEAF_SIZE) {
			/* No leaf node. */
			const bool isVertical = (agentTree_[node].maxX - agentTree_[node].minX > agentTree_[node].maxY - agentTree_[node].minY);
			const octopus::Fixed splitValue = (isVertical ? (agentTree_[node].maxX + agentTree_[node].minX) * 0.5 : (agentTree_[node].maxY + agentTree_[node].minY) * 0.5);

			size_t left = begin;
			size_t right = end;

			while (left < right) {
				while (left < right && (isVertical ? agents_[left]->position_.x() : agents_[left]->position_.y()) < splitValue) {
					++left;
				}

				while (right > left && (isVertical ? agents_[right - 1]->position_.x() : agents_[right - 1]->position_.y()) >= splitValue) {
					--right;
				}

				if (left < right) {
					std::swap(agents_[left], agents_[right - 1]);
					++left;
					--right;
				}
			}

			if (left == begin) {
				++left;
				++right;
			}

			agentTree_[node].left = node + 1;
			agentTree_[node].right = node + 2 * (left - begin);

			buildAgentTreeRecursive(begin, left, agentTree_[node].left);
			buildAgentTreeRecursive(left, end, agentTree_[node].right);
		}
	}

	void KdTree::buildObstacleTree()
	{
		deleteObstacleTree(obstacleTree_);

		std::vector<Obstacle *> obstacles(sim_->obstacles_.size());

		for (size_t i = 0; i < sim_->obstacles_.size(); ++i) {
			obstacles[i] = sim_->obstacles_[i];
		}

		obstacleTree_ = buildObstacleTreeRecursive(obstacles);
	}


	KdTree::ObstacleTreeNode *KdTree::buildObstacleTreeRecursive(const std::vector<Obstacle *> &obstacles)
	{
		if (obstacles.empty()) {
			return NULL;
		}
		else {
			ObstacleTreeNode *const node = new ObstacleTreeNode;

			size_t optimalSplit = 0;
			size_t minLeft = obstacles.size();
			size_t minRight = obstacles.size();

			for (size_t i = 0; i < obstacles.size(); ++i) {
				size_t leftSize = 0;
				size_t rightSize = 0;

				const Obstacle *const obstacleI1 = obstacles[i];
				const Obstacle *const obstacleI2 = obstacleI1->nextObstacle_;

				/* Compute optimal split node. */
				for (size_t j = 0; j < obstacles.size(); ++j) {
					if (i == j) {
						continue;
					}

					const Obstacle *const obstacleJ1 = obstacles[j];
					const Obstacle *const obstacleJ2 = obstacleJ1->nextObstacle_;

					const octopus::Fixed j1LeftOfI = leftOf(obstacleI1->point_, obstacleI2->point_, obstacleJ1->point_);
					const octopus::Fixed j2LeftOfI = leftOf(obstacleI1->point_, obstacleI2->point_, obstacleJ2->point_);

					if (j1LeftOfI >= -RVO_EPSILON && j2LeftOfI >= -RVO_EPSILON) {
						++leftSize;
					}
					else if (j1LeftOfI <= RVO_EPSILON && j2LeftOfI <= RVO_EPSILON) {
						++rightSize;
					}
					else {
						++leftSize;
						++rightSize;
					}

					if (std::make_pair(std::max(leftSize, rightSize), std::min(leftSize, rightSize)) >= std::make_pair(std::max(minLeft, minRight), std::min(minLeft, minRight))) {
						break;
					}
				}

				if (std::make_pair(std::max(leftSize, rightSize), std::min(leftSize, rightSize)) < std::make_pair(std::max(minLeft, minRight), std::min(minLeft, minRight))) {
					minLeft = leftSize;
					minRight = rightSize;
					optimalSplit = i;
				}
			}

			/* Build split node. */
			std::vector<Obstacle *> leftObstacles(minLeft);
			std::vector<Obstacle *> rightObstacles(minRight);

			size_t leftCounter = 0;
			size_t rightCounter = 0;
			const size_t i = optimalSplit;

			const Obstacle *const obstacleI1 = obstacles[i];
			const Obstacle *const obstacleI2 = obstacleI1->nextObstacle_;

			for (size_t j = 0; j < obstacles.size(); ++j) {
				if (i == j) {
					continue;
				}

				Obstacle *const obstacleJ1 = obstacles[j];
				Obstacle *const obstacleJ2 = obstacleJ1->nextObstacle_;

				const octopus::Fixed j1LeftOfI = leftOf(obstacleI1->point_, obstacleI2->point_, obstacleJ1->point_);
				const octopus::Fixed j2LeftOfI = leftOf(obstacleI1->point_, obstacleI2->point_, obstacleJ2->point_);

				if (j1LeftOfI >= -RVO_EPSILON && j2LeftOfI >= -RVO_EPSILON) {
					leftObstacles[leftCounter++] = obstacles[j];
				}
				else if (j1LeftOfI <= RVO_EPSILON && j2LeftOfI <= RVO_EPSILON) {
					rightObstacles[rightCounter++] = obstacles[j];
				}
				else {
					/* Split obstacle j. */
					const octopus::Fixed t = det(obstacleI2->point_ - obstacleI1->point_, obstacleJ1->point_ - obstacleI1->point_) / det(obstacleI2->point_ - obstacleI1->point_, obstacleJ1->point_ - obstacleJ2->point_);

					const Vector2 splitpoint = obstacleJ1->point_ + t * (obstacleJ2->point_ - obstacleJ1->point_);

					Obstacle *const newObstacle = new Obstacle();
					newObstacle->point_ = splitpoint;
					newObstacle->prevObstacle_ = obstacleJ1;
					newObstacle->nextObstacle_ = obstacleJ2;
					newObstacle->isConvex_ = true;
					newObstacle->unitDir_ = obstacleJ1->unitDir_;

					newObstacle->id_ = sim_->obstacles_.size();

					sim_->obstacles_.push_back(newObstacle);

					obstacleJ1->nextObstacle_ = newObstacle;
					obstacleJ2->prevObstacle_ = newObstacle;

					if (j1LeftOfI > 0.0) {
						leftObstacles[leftCounter++] = obstacleJ1;
						rightObstacles[rightCounter++] = newObstacle;
					}
					else {
						rightObstacles[rightCounter++] = obstacleJ1;
						leftObstacles[leftCounter++] = newObstacle;
					}
				}
			}

			node->obstacle = obstacleI1;
			node->left = buildObstacleTreeRecursive(leftObstacles);
			node->right = buildObstacleTreeRecursive(rightObstacles);
			return node;
		}
	}

	void KdTree::computeAgentNeighbors(Agent *agent, octopus::Fixed  &rangeSq) const
	{
		queryAgentTreeRecursive(agent, rangeSq, 0);
	}

	void KdTree::addObstacle(Obstacle const *obstacle_source, Agent *agent, octopus::Fixed const &rangeSq) const
	{
		using namespace octopus;
		Obstacle const *obstacle1 = obstacle_source;
		do
		{
			Obstacle const *obstacle2 = obstacle1->nextObstacle_;
			const Fixed agentLeftOfLine = leftOf(obstacle1->point_, obstacle2->point_, agent->position_);
			const Fixed distSqLine = sqr(agentLeftOfLine) / absSq(obstacle2->point_ - obstacle1->point_);

			if (distSqLine < rangeSq && agentLeftOfLine < 0.0) {
				/*
					* Try obstacle at this node only if agent is on right side of
					* obstacle (and can see obstacle).
					*/
				agent->insertObstacleNeighbor(obstacle1, rangeSq);
			}
			obstacle1 = obstacle2;
		} while(obstacle1 != obstacle_source);
	}

	void KdTree::computeObstacleNeighbors(Agent *agent, octopus::Fixed const &rangeSq) const
	{
		using namespace octopus;
		State const &state_l = *sim_->state_;
		Entity const *ent_l = agent->ent_;
		Vector const &pos_l = ent_l->_pos;
		Box<long long> box_l { to_int((pos_l.x-rangeSq)),
						   to_int((pos_l.x+rangeSq+0.999)),
						   to_int((pos_l.y-rangeSq)),
						   to_int((pos_l.y+rangeSq+0.999))
					};

		// out of bound obstacle
		addObstacle(sim_->obstacles_[0], agent, rangeSq);

		std::unordered_set<unsigned long> handledIdx_l;
		for(long long x = std::max(0ll, box_l._lowerX) ; x < box_l._upperX && x <state_l.getPathGrid().getSizeX() ; ++x)
		{
			for(long long y = std::max(0ll, box_l._lowerY) ; y < box_l._upperY && y < state_l.getPathGrid().getSizeY(); ++y)
			{
				GridNode const *node_l = state_l.getPathGrid().getNode(x, y);

				if(!node_l->isFree()
				&& handledIdx_l.find(node_l->getContent()->_handle.index) == handledIdx_l.end())
				{
					Entity const *obstacle_l = node_l->getContent();
					handledIdx_l.insert(obstacle_l->_handle.index);

					size_t idx_l = sim_->mapHandleIdx_.at(node_l->getContent()->_handle);

					addObstacle(sim_->obstacles_[idx_l], agent, rangeSq);
				}
			}
		}
		// old way
		//queryObstacleTreeRecursive(agent, rangeSq, obstacleTree_);
	}

	void KdTree::deleteObstacleTree(ObstacleTreeNode *node)
	{
		if (node != NULL) {
			deleteObstacleTree(node->left);
			deleteObstacleTree(node->right);
			delete node;
		}
	}

	void KdTree::queryAgentTreeRecursive(Agent *agent, octopus::Fixed  &rangeSq, size_t node) const
	{
		if (agentTree_[node].end - agentTree_[node].begin <= MAX_LEAF_SIZE) {
			for (size_t i = agentTree_[node].begin; i < agentTree_[node].end; ++i) {
				agent->insertAgentNeighbor(agents_[i], rangeSq);
			}
		}
		else {
			const octopus::Fixed distSqLeft = sqr(std::max<octopus::Fixed>(0.0, agentTree_[agentTree_[node].left].minX - agent->position_.x())) + sqr(std::max<octopus::Fixed>(0.0, agent->position_.x() - agentTree_[agentTree_[node].left].maxX)) + sqr(std::max<octopus::Fixed>(0.0, agentTree_[agentTree_[node].left].minY - agent->position_.y())) + sqr(std::max<octopus::Fixed>(0.0, agent->position_.y() - agentTree_[agentTree_[node].left].maxY));

			const octopus::Fixed distSqRight = sqr(std::max<octopus::Fixed>(0.0, agentTree_[agentTree_[node].right].minX - agent->position_.x())) + sqr(std::max<octopus::Fixed>(0.0, agent->position_.x() - agentTree_[agentTree_[node].right].maxX)) + sqr(std::max<octopus::Fixed>(0.0, agentTree_[agentTree_[node].right].minY - agent->position_.y())) + sqr(std::max<octopus::Fixed>(0.0, agent->position_.y() - agentTree_[agentTree_[node].right].maxY));

			if (distSqLeft < distSqRight) {
				if (distSqLeft < rangeSq) {
					queryAgentTreeRecursive(agent, rangeSq, agentTree_[node].left);

					if (distSqRight < rangeSq) {
						queryAgentTreeRecursive(agent, rangeSq, agentTree_[node].right);
					}
				}
			}
			else {
				if (distSqRight < rangeSq) {
					queryAgentTreeRecursive(agent, rangeSq, agentTree_[node].right);

					if (distSqLeft < rangeSq) {
						queryAgentTreeRecursive(agent, rangeSq, agentTree_[node].left);
					}
				}
			}

		}
	}

	void KdTree::queryObstacleTreeRecursive(Agent *agent, octopus::Fixed rangeSq, const ObstacleTreeNode *node) const
	{
		if (node == NULL) {
			return;
		}
		else {
			const Obstacle *const obstacle1 = node->obstacle;
			const Obstacle *const obstacle2 = obstacle1->nextObstacle_;

			const octopus::Fixed agentLeftOfLine = leftOf(obstacle1->point_, obstacle2->point_, agent->position_);

			queryObstacleTreeRecursive(agent, rangeSq, (agentLeftOfLine >= 0.0 ? node->left : node->right));

			const octopus::Fixed distSqLine = sqr(agentLeftOfLine) / absSq(obstacle2->point_ - obstacle1->point_);

			if (distSqLine < rangeSq) {
				if (agentLeftOfLine < 0.0) {
					/*
					 * Try obstacle at this node only if agent is on right side of
					 * obstacle (and can see obstacle).
					 */
					agent->insertObstacleNeighbor(node->obstacle, rangeSq);
				}

				/* Try other side of line. */
				queryObstacleTreeRecursive(agent, rangeSq, (agentLeftOfLine >= 0.0 ? node->right : node->left));

			}
		}
	}

	bool KdTree::queryVisibility(const Vector2 &q1, const Vector2 &q2, octopus::Fixed radius) const
	{
		return queryVisibilityRecursive(q1, q2, radius, obstacleTree_);
	}

	bool KdTree::queryVisibilityRecursive(const Vector2 &q1, const Vector2 &q2, octopus::Fixed radius, const ObstacleTreeNode *node) const
	{
		if (node == NULL) {
			return true;
		}
		else {
			const Obstacle *const obstacle1 = node->obstacle;
			const Obstacle *const obstacle2 = obstacle1->nextObstacle_;

			const octopus::Fixed q1LeftOfI = leftOf(obstacle1->point_, obstacle2->point_, q1);
			const octopus::Fixed q2LeftOfI = leftOf(obstacle1->point_, obstacle2->point_, q2);
			const octopus::Fixed invLengthI = octopus::Fixed::One() / absSq(obstacle2->point_ - obstacle1->point_);

			if (q1LeftOfI >= 0.0 && q2LeftOfI >= 0.0) {
				return queryVisibilityRecursive(q1, q2, radius, node->left) && ((sqr(q1LeftOfI) * invLengthI >= sqr(radius) && sqr(q2LeftOfI) * invLengthI >= sqr(radius)) || queryVisibilityRecursive(q1, q2, radius, node->right));
			}
			else if (q1LeftOfI <= 0.0 && q2LeftOfI <= 0.0) {
				return queryVisibilityRecursive(q1, q2, radius, node->right) && ((sqr(q1LeftOfI) * invLengthI >= sqr(radius) && sqr(q2LeftOfI) * invLengthI >= sqr(radius)) || queryVisibilityRecursive(q1, q2, radius, node->left));
			}
			else if (q1LeftOfI >= 0.0 && q2LeftOfI <= 0.0) {
				/* One can see through obstacle from left to right. */
				return queryVisibilityRecursive(q1, q2, radius, node->left) && queryVisibilityRecursive(q1, q2, radius, node->right);
			}
			else {
				const octopus::Fixed point1LeftOfQ = leftOf(q1, q2, obstacle1->point_);
				const octopus::Fixed point2LeftOfQ = leftOf(q1, q2, obstacle2->point_);
				const octopus::Fixed invLengthQ = octopus::Fixed::One() / absSq(q2 - q1);

				return (point1LeftOfQ * point2LeftOfQ >= 0.0 && sqr(point1LeftOfQ) * invLengthQ > sqr(radius) && sqr(point2LeftOfQ) * invLengthQ > sqr(radius) && queryVisibilityRecursive(q1, q2, radius, node->left) && queryVisibilityRecursive(q1, q2, radius, node->right));
			}
		}
	}
}
