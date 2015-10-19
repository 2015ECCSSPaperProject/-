
#include "fstream_paper.h"
#include "../../IEX/iextreme.h"

std::fstream& operator >> (std::fstream& fs, Vector3& v3)
{
	fs >> v3.x;
	fs >> v3.y;
	fs >> v3.z;

	return fs;
}

std::ifstream& operator >> (std::ifstream& ifs, Vector3& v3)
{
	ifs >> v3.x;
	ifs >> v3.y;
	ifs >> v3.z;

	return ifs;
}
