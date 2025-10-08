#include "Utils.h"
#include <vector>


std::vector<unsigned int> GenerateIndices(const float* vertices)
{
    std::vector<unsigned int> indices;
    for (unsigned int i = 0; i < 36; i++)
    {
        indices.push_back(i);
    }
    return indices;
}