#include "VectorMath.h"

float VectorMath::angleXZ(const glm::vec3& vec1, const glm::vec3& vec2)
{
    float dot = vec1.x * vec2.x + vec1.z * vec2.z;
    float det = vec1.x * vec2.z - vec1.z * vec2.x;
    return atan2(det, dot);
}