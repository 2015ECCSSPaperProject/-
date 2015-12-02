
#ifndef RANDOM_H
#define RANDOM_H

#include <random>

class Random_mt
{
public:
	static std::mt19937 engine;

	static void Initialize();

	static std::uniform_int_distribution<int> Make_distribution(int min_num, int max_num);

	static std::uniform_real_distribution<float> Make_distribution(float min_num, float max_num);

	static int Get(int min_num, int max_num);

	static float Get(float min_num, float max_num);

	typedef std::uniform_int_distribution < int > INT;
	typedef std::uniform_int_distribution < float > FLOAT;
};

#endif // !RANDOM_H
