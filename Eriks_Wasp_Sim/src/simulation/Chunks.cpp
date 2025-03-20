#include "Chunks.h"

using Chunks::Chunk;
using Chunks::IVec3Hash;
using Chunks::IVec3Equal;

std::size_t IVec3Hash::operator()(const glm::ivec3& v) const noexcept
{
    // Combine the individual hashes of x, y, and z
    // using a simple but reasonable method
    // (the magic constant 0x9e3779b97f4a7c15ULL is from boost::hash_combine)
    auto h1 = std::hash<int>()(v.x);
    auto h2 = std::hash<int>()(v.y);
    auto h3 = std::hash<int>()(v.z);

    std::size_t seed = 0;
    // Combine h1
    seed ^= h1 + 0x9e3779b97f4a7c15ULL + (seed << 6) + (seed >> 2);
    // Combine h2
    seed ^= h2 + 0x9e3779b97f4a7c15ULL + (seed << 6) + (seed >> 2);
    // Combine h3
    seed ^= h3 + 0x9e3779b97f4a7c15ULL + (seed << 6) + (seed >> 2);

    return seed;
}

bool IVec3Equal::operator()(const glm::ivec3& a, const glm::ivec3& b) const noexcept
{
    return (a.x == b.x && a.y == b.y && a.z == b.z);
}

glm::ivec3 Chunks::getChunkCoord(glm::vec3* position)
{
    // For negative positions, floor will go “down” (i.e. -0.2 becomes -1),
    // so watch out if that’s not what you want. Usually it’s correct for chunk logic.
    return glm::ivec3(
        static_cast<int>(std::floor(position->x / CHUNK_SIZE)),
        static_cast<int>(std::floor(position->y / CHUNK_SIZE)),
        static_cast<int>(std::floor(position->z / CHUNK_SIZE))
    );
}