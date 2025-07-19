#include "Random.hpp"

std::mt19937 Random::gen(std::random_device{}());

int Random::RandInt(int min, int max)
{
    std::uniform_int_distribution<int> dist(min, max);
    return dist(gen);
}

float Random::RandFloat(float min, float max)
{
    std::uniform_real_distribution<float> dist(min, max);
    return dist(gen);
}
