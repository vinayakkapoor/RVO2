/*
 * Roadmap.cpp
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

/*
 * Example file showing a demo with 100 agents split in four groups initially
 * positioned in four corners of the environment. Each agent attempts to move to
 * other side of the environment through a narrow passage generated by four
 * obstacles. There is a roadmap to guide the agents around the obstacles.
 */

#include <fstream>

#ifndef RVO_OUTPUT_TIME_AND_POSITIONS
#define RVO_OUTPUT_TIME_AND_POSITIONS 1
#endif

#ifndef RVO_SEED_RANDOM_NUMBER_GENERATOR
#define RVO_SEED_RANDOM_NUMBER_GENERATOR 1
#endif

#include <cmath>
#include <cstdlib>
#include <map>
#include <vector>

#if RVO_OUTPUT_TIME_AND_POSITIONS
#include <iostream>
#endif

#if RVO_SEED_RANDOM_NUMBER_GENERATOR
#include <ctime>
#endif

#if _OPENMP
#include <omp.h>
#endif

#include <RVO.h>

#ifndef M_PI
const float M_PI = 3.14159265358979323846f;
#endif

int fileNo = 0; //used in uniquely saving the file (e.g - trajectoryData_v[fileNo].txt)

class RoadmapVertex
{
public:
	RVO::Vector2 position;
	std::vector<int> neighbors;
	std::vector<float> distToGoal;
};

/* Store the roadmap. */
std::vector<RoadmapVertex> roadmap;

/* Store the goals of the agents. */
std::vector<int> goals;

void setupScenario(RVO::RVOSimulator *sim)
{
#if RVO_SEED_RANDOM_NUMBER_GENERATOR
	std::srand(static_cast<unsigned int>(std::time(NULL) + fileNo));
#endif

	using namespace std;
	ofstream trajFile;
	trajFile.open("./dataset/trajectoryData_v"+std::to_string((fileNo))+".txt", std::ios_base::app);

	/* Specify the global time step of the simulation. */
	sim->setTimeStep(0.25f);

	/* Add roadmap vertices. */
	RoadmapVertex v;

	/* Add the goal positions of agents. 
		Agent 1 is spawned in the second quadrant, and has its goal in the fourth quadrant
		Agent 2 is spawned in the third quadrant, and has its goal in the first quadrant */

	float xg_1 = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX))*2.0f + 1.0f; //random no between 1 and 3
	float yg_1 = -1.0f*((static_cast <float> (rand()) / static_cast <float> (RAND_MAX))*2.0f + 1.0f); //random no between -3 and -1
	float xg_2 = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX))*2.0f + 1.0f; //random no between 1 and 3
	float yg_2 = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX))*2.0f + 1.0f; //random no between 1 and 3

	v.position = RVO::Vector2(xg_1, yg_1);
	roadmap.push_back(v);
	v.position = RVO::Vector2(xg_2, yg_2);
	roadmap.push_back(v);

	float radius = 0.75f;
	float maxVel = 1.0f;

	
	trajFile << xg_1 << " " << yg_1 << " " << radius << " " << maxVel << "\n";
	trajFile << xg_2 << " " << yg_2 << " " << radius << " " << maxVel << "\n";
	trajFile.close();

	sim->setAgentDefaults(5.0f, 2, 1.0f, 1.0f, radius, maxVel);
	/* form of setAgentDefualts() (inspect the function for detailed information about the paramters) :
	void setAgentDefaults(float neighborDist, size_t maxNeighbors,
							  float timeHorizon, float timeHorizonObst,
							  float radius, float maxSpeed,
							  const Vector2 &velocity = Vector2());*/

	//define starting position of the agents
	float xs_1 = -1.0f*((static_cast <float> (rand()) / static_cast <float> (RAND_MAX))*2.0f + 1.0f); //random no between -3 and -1
	float ys_1 = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX))*2.0f + 1.0f; //random no between 1 and 3
	float xs_2 = -1.0f*((static_cast <float> (rand()) / static_cast <float> (RAND_MAX))*2.0f + 1.0f); //random no between -3 and -1
	float ys_2 = -1.0f*((static_cast <float> (rand()) / static_cast <float> (RAND_MAX))*2.0f + 1.0f); //random no between -3 and -1

	sim->addAgent(RVO::Vector2(xs_1, ys_1));
	goals.push_back(0);

	sim->addAgent(RVO::Vector2(xs_2, ys_2));
	goals.push_back(1);	
}

#if RVO_OUTPUT_TIME_AND_POSITIONS
void updateVisualization(RVO::RVOSimulator *sim)
{	
	using namespace std;
	//srand(static_cast<unsigned int>(std::time(nullptr)));
	
	ofstream trajFile;	
	trajFile.open("./dataset/trajectoryData_v"+std::to_string((fileNo))+".txt", std::ios_base::app);

	/* Output the current global time. */
	//std::cout << sim->getGlobalTime();
	trajFile << sim->getGlobalTime() << " ";

	/* Output the current position of all the agents. */

	trajFile << sim->getAgentPosition(0) << " ";
	trajFile << sim->getAgentPosition(1);

	/*for (size_t i = 0; i < sim->getNumAgents(); ++i)
	{
		std::cout << " " << sim->getAgentPosition(i);
		trajFile << sim->getAgentPosition(i) << ",";		
	}*/

	//std::cout << std::endl;
	trajFile << "\n";
	trajFile.close();
}
#endif

void buildRoadmap(RVO::RVOSimulator *sim)
{
	/* Connect the roadmap vertices by edges if mutually visible. */
#ifdef _OPENMP
#pragma omp parallel for
#endif
	for (int i = 0; i < static_cast<int>(roadmap.size()); ++i)
	{
		for (int j = 0; j < static_cast<int>(roadmap.size()); ++j)
		{
			if (sim->queryVisibility(roadmap[i].position, roadmap[j].position, sim->getAgentRadius(0)))
			{
				roadmap[i].neighbors.push_back(j);
			}
		}

		/*
		 * Initialize the distance to each of the four goal vertices at infinity
		 * (9e9f).
		 */
		roadmap[i].distToGoal.resize(2, 9e9f);
	}

	/*
	 * Compute the distance to each of the four goals (the first four vertices)
	 * for all vertices using Dijkstra's algorithm.
	 */
#ifdef _OPENMP
#pragma omp parallel for
#endif
	for (int i = 0; i < 2; ++i)
	{
		std::multimap<float, int> Q;
		std::vector<std::multimap<float, int>::iterator> posInQ(roadmap.size(), Q.end());

		roadmap[i].distToGoal[i] = 0.0f;
		posInQ[i] = Q.insert(std::make_pair(0.0f, i));

		while (!Q.empty())
		{
			const int u = Q.begin()->second;
			Q.erase(Q.begin());
			posInQ[u] = Q.end();

			for (int j = 0; j < static_cast<int>(roadmap[u].neighbors.size()); ++j)
			{
				const int v = roadmap[u].neighbors[j];
				const float dist_uv = RVO::abs(roadmap[v].position - roadmap[u].position);

				if (roadmap[v].distToGoal[i] > roadmap[u].distToGoal[i] + dist_uv)
				{
					roadmap[v].distToGoal[i] = roadmap[u].distToGoal[i] + dist_uv;

					if (posInQ[v] == Q.end())
					{
						posInQ[v] = Q.insert(std::make_pair(roadmap[v].distToGoal[i], v));
					}
					else
					{
						Q.erase(posInQ[v]);
						posInQ[v] = Q.insert(std::make_pair(roadmap[v].distToGoal[i], v));
					}
				}
			}
		}
	}
}

void setPreferredVelocities(RVO::RVOSimulator *sim)
{
	/*
	 * Set the preferred velocity to be a vector of unit magnitude (speed) in the
	 * direction of the visible roadmap vertex that is on the shortest path to the
	 * goal.
	 */
#ifdef _OPENMP
#pragma omp parallel for
#endif
	for (int i = 0; i < static_cast<int>(sim->getNumAgents()); ++i)
	{
		float minDist = 9e9f;
		int minVertex = -1;

		for (int j = 0; j < static_cast<int>(roadmap.size()); ++j)
		{
			if (RVO::abs(roadmap[j].position - sim->getAgentPosition(i)) + roadmap[j].distToGoal[goals[i]] < minDist &&
				sim->queryVisibility(sim->getAgentPosition(i), roadmap[j].position, sim->getAgentRadius(i)))
			{

				minDist = RVO::abs(roadmap[j].position - sim->getAgentPosition(i)) + roadmap[j].distToGoal[goals[i]];
				minVertex = j;
			}
		}

		if (minVertex == -1)
		{
			/* No roadmap vertex is visible; should not happen. */
			sim->setAgentPrefVelocity(i, RVO::Vector2(0, 0));
		}
		else
		{
			if (RVO::absSq(roadmap[minVertex].position -
						   sim->getAgentPosition(i)) == 0.0f)
			{
				if (minVertex == goals[i])
				{
					sim->setAgentPrefVelocity(i, RVO::Vector2());
				}
				else
				{
					sim->setAgentPrefVelocity(i, RVO::normalize(roadmap[goals[i]].position - sim->getAgentPosition(i)));
				}
			}
			else
			{
				sim->setAgentPrefVelocity(i, RVO::normalize(roadmap[minVertex].position - sim->getAgentPosition(i)));
			}
		}

		/*
		 * Perturb a little to avoid deadlocks due to perfect symmetry.
		 */
		float angle = std::rand() * 2.0f * M_PI / RAND_MAX;
		float dist = std::rand() * 0.0001f / RAND_MAX;

		sim->setAgentPrefVelocity(i, sim->getAgentPrefVelocity(i) +
										 dist * RVO::Vector2(std::cos(angle), std::sin(angle)));
	}
}

bool reachedGoal(RVO::RVOSimulator *sim)
{
	/* Check if all agents have reached their goals. */
	for (size_t i = 0; i < sim->getNumAgents(); ++i)
	{
		if (RVO::absSq(sim->getAgentPosition(i) - roadmap[goals[i]].position) > 0.25f * 0.25f)
		{
			return false;
		}
	}

	return true;
}

int main(int argc, char* argv[])
{
	fileNo = atoi(argv[1]);	
	
	/* Create a new simulator instance. */
	RVO::RVOSimulator *sim = new RVO::RVOSimulator();

	/* Set up the scenario. */
	setupScenario(sim);

	/* Build the roadmap. */
	buildRoadmap(sim);

	/* Perform (and manipulate) the simulation. */
	do
	{
#if RVO_OUTPUT_TIME_AND_POSITIONS
		updateVisualization(sim);
#endif
		setPreferredVelocities(sim);
		sim->doStep();
	} while (!reachedGoal(sim));

	delete sim;

	return 0;
}


