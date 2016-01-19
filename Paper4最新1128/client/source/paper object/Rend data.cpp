//#include "iextreme.h"
//#include "../sound/SoundManager.h"
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

bool Rend_data::Move_check( float x, float y )
{
	// ���Ε����̈ړ��͂����Ȃ�
	if ((data[index].x == 0) || (data[index].x < 0 && x > 0) || (data[index].x > 0 && x < 0))
		x = 0;
	if ((data[index].y == 0) || (data[index].y < 0 && y > 0) || (data[index].y > 0 && y < 0))
		y = 0;
	move.Add(x, y);

	// �ړ��l����
	if ((data[index].x < 0 && data[index].x < move.x) || (data[index].x > 0 && data[index].x > move.x))
		return false;
	if ((data[index].y < 0 && data[index].y < move.y) || (data[index].y > 0 && data[index].y > move.y))
		return false;

	// ���̈ړ��l��
	if (index < num_data - 1)
	{
		// �����������ǂ����m�F�p
		//se->Play("����");

		move.Reset();
		index++;
		return false;
	}

	// ����
	index = 0;
	return true;
}

void Rend_data::Get_commnand(float *x, float *y)
{
	*x = data[index].x;
	*y = data[index].y;
}