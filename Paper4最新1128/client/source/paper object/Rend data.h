
#pragma once

// 破る時に必要なマウスの移動値
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
	// マウスの移動コマンド作成
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

	// マウスの移動コマンド判定
	bool Move_check( float x, float y )
	{
		// 反対方向の移動はたさない
		if( ( data[index].x == 0 ) || ( data[index].x < 0 && x > 0 ) || ( data[index].x > 0 && x < 0 ) )
			x = 0;
		if( ( data[index].y == 0 ) || ( data[index].y < 0 && y > 0 ) || ( data[index].y > 0 && y < 0 ) )
			y = 0;
		move.Add( x, y );

		// 移動値判定
		if( ( data[index].x < 0 && data[index].x < move.x ) || ( data[index].x > 0 && data[index].x > move.x ) )
			return false;
		if( ( data[index].y < 0 && data[index].y < move.y ) || ( data[index].y > 0 && data[index].y > move.y ) )
			return false;

		// 次の移動値へ
		if( index < num_data-1 )
		{
			move.Reset();
			index++;
			return false;
		}

		// 成功
		index = 0;
		return true;
	}

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
