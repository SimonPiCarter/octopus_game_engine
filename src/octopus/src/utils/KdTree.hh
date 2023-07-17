/*
 * KdTree.h
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
#ifndef _KD_TREE_H_
#define _KD_TREE_H_

#include "utils/Fixed.hh"
#include "state/Handle.hh"


#include <functional>

namespace octopus {

	class Entity;
	class State;

	/// @brief Defines <i>k</i>d-trees for agents and static obstacles in the simulation.
	class KdTree {
	public:


		/// @brief Constructs a <i>k</i>d-tree instance.
		explicit KdTree(std::vector<Entity*> const &entities_p);


		/// @brief Destroys this kd-tree instance.
		~KdTree();


		/// @brief Builds an entity <i>k</i>d-tree.
		void buildAgentTree(std::function<bool(Entity const *)> const &fn_p = [](Entity const *) { return true; });

		std::vector<std::pair<Fixed , const Entity *> > computeEntityNeighbors(Handle const & ent_p, Fixed const &rangeSq_p, size_t max_p,
			std::function<bool(Entity const *)> const &fn_p = [](Entity const *) { return true; }) const;

	private:

		void buildAgentTreeRecursive(size_t begin, size_t end, size_t node);

		void queryAgentTreeRecursive(std::vector<std::pair<Fixed , const Entity *> > &neighbors_p,
			Entity const *agent, Fixed &rangeSq, size_t max_p, size_t node, std::function<bool(Entity const *)> const &fn_p) const;

		std::vector<Entity*> const &entities_;
		std::vector<Entity const *> agents_;
		std::vector<size_t> agentIndexPerHandle_;

		static const size_t MAX_LEAF_SIZE = 5;


		/// @brief Defines an entity <i>k</i>d-tree node.
		class AgentTreeNode {
		public:

			/// @brief The beginning node number.
			size_t begin;


			/// @brief The ending node number.
			size_t end;


			/// @brief The left node number.
			size_t left;


			/// @brief The maximum x-coordinate.
			octopus::Fixed maxX;


			/// @brief The maximum y-coordinate.
			octopus::Fixed maxY;


			/// @brief The minimum x-coordinate.
			octopus::Fixed minX;


			/// @brief The minimum y-coordinate.
			octopus::Fixed minY;


			/// @brief The right node number.
			size_t right;
		};

		std::vector<AgentTreeNode> agentTree_;
	};

	bool teamCheck(unsigned long team_p, bool sameTeam_p, State const &state_p, Entity const *entity_p);
}

#endif
