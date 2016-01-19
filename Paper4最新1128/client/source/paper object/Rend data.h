
#pragma once

// �j�鎞�ɕK�v�ȃ}�E�X�̈ړ��l
class Rend_data
{
public:
	struct Movedata
	{
		float x, y;
		inline Movedata() :x( 0 ), y( 0 )
		{}
		inline Movedata( float x, float y ) : x( x ), y( y )
		{}
	};
	// �}�E�X�̈ړ��R�}���h�쐬
	Rend_data( unsigned int num_move, Movedata data[] ) :num_data( num_move ), index( 0 )
	{
		this->data = new Movedata[num_data];
		for( size_t i = 0; i < num_data; i++ )
		{
			this->data[i] = data[i];
		}
	}
	~Rend_data()
	{
		delete[] data;
	}
	// out : new ���ĕԂ��܂�
	static void Load( Rend_data *&out, const char *filename );

	// �}�E�X�̈ړ��R�}���h����
	bool Move_check( float x, float y );

	// ���ׂ��R�}���h���擾
	void Get_commnand(float *x, float *y);

	void Reset()
	{
		index = 0;
		move.Reset();
	}
private:
	struct Mouse_move : public Movedata
	{
		inline Mouse_move() :Movedata()
		{}
		inline Mouse_move( float x, float y ) : Movedata( x, y )
		{}

		void Add( float x, float y )
		{
			this->x += x;
			this->y += y;
		}
		inline void Reset()
		{
			x = y = 0;
		}
	};

	Mouse_move move;
	const unsigned int num_data;
	unsigned int index;
	Movedata *data;
};
