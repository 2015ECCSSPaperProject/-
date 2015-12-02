
#include "Random_mt.h"

std::mt19937 Random_mt::engine;

void Random_mt::Initialize()
{
	std::random_device device;
	engine.seed((unsigned long)device());
}

std::uniform_int_distribution<int> Random_mt::Make_distribution(int min_num, int max_num)
{
	return std::uniform_int_distribution<int>(min_num, max_num);
}

std::uniform_real_distribution<float> Random_mt::Make_distribution(float min_num, float max_num)
{
	return std::uniform_real_distribution<float>(min_num, max_num);
}

int Random_mt::Get(int min_num, int max_num)
{
	return std::uniform_int_distribution<int>(min_num, max_num)(engine);
}

float Random_mt::Get(float min_num, float max_num)
{
	return std::uniform_real_distribution<float>(min_num, max_num)(engine);
}
