
#include <vector>
#include "Inhabitant.h"

#include "iextreme.h"
#include "../fstream/fstream_paper.h"
#include "IEX_Math2.h"

Inhabitants::Inhabitants( const char *filename )
{
	std::ifstream ifs( filename );
	Vector3 position( 0, 0, 0 );
	float angle( 0 );

	iex3DObj *obj = new iex3DObj( "DATA/CHR/zattou/zattou.IEM" );
	ifs >> angle;
	angle = Degree_to_radian( angle );
	ifs >> position;
	obj->SetAngle( angle );
	obj->SetPos( position );
	obj_vector.push_back( obj );
	while( !ifs.eof() )
	{
		ifs >> angle;
		angle = Degree_to_radian( angle );
		ifs >> position;
		obj = obj->Clone();
		obj->SetAngle( angle );
		obj->SetPos( position );
		obj_vector.push_back( obj );
	}
}

Inhabitants::~Inhabitants()
{
	for( size_t i = 0; i < obj_vector.size(); i++ )
	{
		delete obj_vector[i];
	}
	obj_vector.clear();
}

void Inhabitants::Update()
{
	for( auto it : obj_vector )
	{
		it->Animation();
		it->Update();
	}
}

void Inhabitants::Render( iexShader *shader, char *name )
{
	for( auto it : obj_vector )
	{
		it->Render( shader, name );
	}
}
