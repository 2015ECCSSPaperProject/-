
#include "Rend data.h"
#include <fstream>

void Rend_data::Load( Rend_data *&out, const char *filename )
{
	std::ifstream ifs( filename );

	unsigned int numdata( 0 );
	ifs >> numdata;
	Movedata *data = new Movedata[numdata];

	for( unsigned int i = 0; i < numdata; i++ )
	{
		ifs >> data[i].x >> data[i].y;
	}

	out = new Rend_data( numdata, data );

	delete[] data;
}
