#ifndef RANDOMENGINE_H
#define RANDOMENGINE_H

#include <random>

class RandomEngine
{
public:
	static std::mt19937& getInstance();
	static int getRandomInt(int min, int max);

private:
	RandomEngine() = default;
	virtual ~RandomEngine() = default;

	// Private copy and move constructors/operators to prevent copies
	RandomEngine(const RandomEngine&);
	RandomEngine& operator=(const RandomEngine&);
	RandomEngine(RandomEngine&&);
	RandomEngine& operator=(RandomEngine&&);
};

#endif // RANDOMENGINE_H