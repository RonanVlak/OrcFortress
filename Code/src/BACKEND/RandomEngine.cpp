#include "BACKEND/RandomEngine.h"

std::mt19937& RandomEngine::getInstance()
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    return gen;
}

int RandomEngine::getRandomInt(int min, int max)
{
    std::uniform_int_distribution<> dis(min, max);
    return dis(getInstance());
}


// Private copy and move constructors/operators to prevent copies
RandomEngine::RandomEngine(const RandomEngine&)
{
    // No copying allowed
}

RandomEngine& RandomEngine::operator=(const RandomEngine&)
{
    // No copying allowed
    return *this;
}

RandomEngine::RandomEngine(RandomEngine&&)
{
    // No moving allowed
}

RandomEngine& RandomEngine::operator=(RandomEngine&&)
{
    // No moving allowed
    return *this;
}
