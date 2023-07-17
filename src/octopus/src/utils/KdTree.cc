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
 *
 * WARNING THIS CODE HAS BEEN MODIFIED
 */

#include "KdTree.hh"

#include "state/State.hh"
#include "state/entity/Entity.hh"

namespace octopus {
	KdTree::KdTree(std::vector<Entity*> const &entities_p) : entities_(entities_p) { }

	KdTree::~KdTree()
	{}

	void KdTree::buildAgentTree(std::function<bool(Entity const *)> const &fn_p)
	{
		agents_.clear();
		agentIndexPerHandle_.clear();
		for (size_t i = agents_.size(); i < entities_.size(); ++i)
		{
			if(entities_[i] && entities_[i]->isActive() && !entities_[i]->isIgnoringCollision() && fn_p(entities_[i]))
			{
				agents_.push_back(entities_[i]);
				if(agentIndexPerHandle_.size() <= entities_[i]->_handle)
				{
					agentIndexPerHandle_.resize(entities_[i]->_handle+1, -1);
				}
				agentIndexPerHandle_[entities_[i]->_handle] = agents_.size()-1;
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
		agentTree_[node].minX = agentTree_[node].maxX = agents_[begin]->_pos.x;
		agentTree_[node].minY = agentTree_[node].maxY = agents_[begin]->_pos.y;

		for (size_t i = begin + 1; i < end; ++i) {
			agentTree_[node].maxX = std::max(agentTree_[node].maxX, agents_[i]->_pos.x);
			agentTree_[node].minX = std::min(agentTree_[node].minX, agents_[i]->_pos.x);
			agentTree_[node].maxY = std::max(agentTree_[node].maxY, agents_[i]->_pos.y);
			agentTree_[node].minY = std::min(agentTree_[node].minY, agents_[i]->_pos.y);
		}

		if (end - begin > MAX_LEAF_SIZE) {
			/* No leaf node. */
			const bool isVertical = (agentTree_[node].maxX - agentTree_[node].minX > agentTree_[node].maxY - agentTree_[node].minY);
			const Fixed splitValue = (isVertical ? (agentTree_[node].maxX + agentTree_[node].minX) * 0.5 : (agentTree_[node].maxY + agentTree_[node].minY) * 0.5);

			size_t left = begin;
			size_t right = end;

			while (left < right) {
				while (left < right && (isVertical ? agents_[left]->_pos.x : agents_[left]->_pos.y) < splitValue) {
					++left;
				}

				while (right > left && (isVertical ? agents_[right - 1]->_pos.x : agents_[right - 1]->_pos.y) >= splitValue) {
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


	void registerUnit(std::vector<std::pair<Fixed , const Entity *> > &neighbors_p, size_t max_p,
		Entity const *source_p, Entity const *target_p, Fixed &distance_p,
		std::function<bool(Entity const *)> const &fn_p)
	{
		if (source_p != target_p && fn_p(target_p))
		{
			const Fixed distSq_l = square_length(source_p->_pos - target_p->_pos);

			if (distSq_l < distance_p) {
				if (neighbors_p.size() <= max_p || max_p == 0) {
					neighbors_p.push_back(std::make_pair(distSq_l, target_p));
				}

				size_t i = neighbors_p.size() - 1;

				while (i != 0 && distSq_l < neighbors_p[i - 1].first) {
					std::swap(neighbors_p[i], neighbors_p[i - 1]);
					--i;
				}

				neighbors_p[i] = std::make_pair(distSq_l, target_p);

				if (neighbors_p.size() == max_p) {
					distance_p = neighbors_p.back().first;
				}
			}
		}
	}

	std::vector<std::pair<Fixed , const Entity *> > KdTree::computeEntityNeighbors(Handle const & ent_p, Fixed const &range_p,
		size_t max_p, std::function<bool(Entity const *)> const &fn_p) const
	{
		std::vector<std::pair<Fixed , const Entity *> > result_l;
		if(ent_p < agentIndexPerHandle_.size())
		{
			Fixed rangeSquared_l = range_p*range_p;
			Entity const * agent_l = agents_[agentIndexPerHandle_[ent_p]];
			queryAgentTreeRecursive(result_l, agent_l, rangeSquared_l, max_p, 0, fn_p);
		}
		return result_l;
	}

	void KdTree::queryAgentTreeRecursive(std::vector<std::pair<Fixed , const Entity *> > &neighbors_p,
		Entity const *agent, Fixed &rangeSq, size_t max_p, size_t node, std::function<bool(Entity const *)> const &fn_p) const
	{
		if (agentTree_[node].end - agentTree_[node].begin <= MAX_LEAF_SIZE) {
			for (size_t i = agentTree_[node].begin; i < agentTree_[node].end; ++i) {
				// storage function
				registerUnit(neighbors_p, max_p, agent, agents_[i], rangeSq, fn_p);
			}
		}
		else {
			const Fixed distSqLeft = sqr(std::max<Fixed>(0.0, agentTree_[agentTree_[node].left].minX - agent->_pos.x))
											+ sqr(std::max<Fixed>(0.0, agent->_pos.x - agentTree_[agentTree_[node].left].maxX))
											+ sqr(std::max<Fixed>(0.0, agentTree_[agentTree_[node].left].minY - agent->_pos.y))
											+ sqr(std::max<Fixed>(0.0, agent->_pos.y - agentTree_[agentTree_[node].left].maxY));

			const Fixed distSqRight = sqr(std::max<Fixed>(0.0, agentTree_[agentTree_[node].right].minX - agent->_pos.x))
											 + sqr(std::max<Fixed>(0.0, agent->_pos.x - agentTree_[agentTree_[node].right].maxX))
											 + sqr(std::max<Fixed>(0.0, agentTree_[agentTree_[node].right].minY - agent->_pos.y))
											 + sqr(std::max<Fixed>(0.0, agent->_pos.y - agentTree_[agentTree_[node].right].maxY));

			if (distSqLeft < distSqRight) {
				if (distSqLeft < rangeSq) {
					queryAgentTreeRecursive(neighbors_p, agent, rangeSq, max_p, agentTree_[node].left, fn_p);

					if (distSqRight < rangeSq) {
						queryAgentTreeRecursive(neighbors_p, agent, rangeSq, max_p, agentTree_[node].right, fn_p);
					}
				}
			}
			else {
				if (distSqRight < rangeSq) {
					queryAgentTreeRecursive(neighbors_p, agent, rangeSq, max_p, agentTree_[node].right, fn_p);

					if (distSqLeft < rangeSq) {
						queryAgentTreeRecursive(neighbors_p, agent, rangeSq, max_p, agentTree_[node].left, fn_p);
					}
				}
			}

		}
	}

	bool teamCheck(unsigned long team_p, bool sameTeam_p, State const &state_p, Entity const *entity_p)
	{
		return (sameTeam_p && team_p == state_p.getPlayer(entity_p->_player)->_team)
			|| (!sameTeam_p && team_p != state_p.getPlayer(entity_p->_player)->_team);
	}

	const Entity * getClosestEntity(KdTree const &tree_p, Handle const & ent_p, Fixed const &range_p, std::function<bool(Entity const *)> const &fn_p)
	{
		std::vector<std::pair<Fixed , const Entity *> > result_l = tree_p.computeEntityNeighbors(ent_p, range_p, 1, fn_p);
		if(result_l.empty())
		{
			return nullptr;
		}
		return result_l[0].second;
	}

} // octopus
