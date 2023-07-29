#ifndef __ProceduralTerrain__
#define __ProceduralTerrain__

#include "Area.hh"
#include "SpawningGrid.hh"

#include "utils/RandomGenerator.hh"

namespace octopus
{

enum class TerrainShape
{
	Circle,
	Line,
	BrokenLine
};

enum class TerrainType
{
	Tree,
	Rocks
};

struct TerrainCoord
{
	int x = 0;
	int y = 0;
};

struct TerrainOption
{
	std::vector<TerrainCoord> cases;
	TerrainShape shape;
	TerrainType type;
};

struct GenerationParameters
{
	int minCircleSize = 2;
	int maxCircleSize = 4;
	int minLineSize = 3;
	int maxLineSize = 6;
	int minBrokenLineSize = 2;
	int maxBrokenLineSize = 4;
	int minRandomDrops = 0;
	int maxRandomDrops = 6;
};

void handleYDrops(RandomGenerator &rand_p, TerrainOption &option_p, int minRandomDrops_p, int maxRandomDrops_p);
void handleXDrops(RandomGenerator &rand_p, TerrainOption &option_p, int minRandomDrops_p, int maxRandomDrops_p);
void createCircleOption(RandomGenerator &rand_p, TerrainOption &option_p, int minCircleSize_p, int maxCircleSize_p);
void createLineOption(RandomGenerator &rand_p, TerrainOption &option_p, int minLineSize_p, int maxLineSize_p);
void createBrokenLineOption(RandomGenerator &rand_p, TerrainOption &option_p, int minLineBrokenSize_p, int maxLineBrokenSize_p);

/// @brief will return a terrain option
TerrainOption generateOneTerrainOption(RandomGenerator &rand_p, std::vector<Area> const &forbiddenArea_p, GenerationParameters const &params_p);

} // octopus

#endif
