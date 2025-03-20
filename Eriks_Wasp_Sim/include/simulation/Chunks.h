#pragma once
#include "wasp.h"
#include "food.h"
#include <list>

using Food::FoodEntity;

/**
* @namespace Food
* @brief A namespace for code related to food entities.
*/
namespace Chunks
{
	const float CHUNK_SIZE = 16.0f;

	/**
	* @struct Chunk
	* @brief A struct representing a single dynamic chunk containing entities within the simulation.
	*/
	struct Chunk
	{
		glm::ivec3 coord;
		std::list<Wasp*> wasps;
		std::list<FoodEntity*> food;
	};

    struct IVec3Hash
    {
        std::size_t operator()(const glm::ivec3& v) const noexcept;
    };

    // 2. Define an equality functor for glm::ivec3
    struct IVec3Equal
    {
		bool operator()(const glm::ivec3& a, const glm::ivec3& b) const noexcept;
    };

	glm::ivec3 getChunkCoord(glm::vec3* position);
}