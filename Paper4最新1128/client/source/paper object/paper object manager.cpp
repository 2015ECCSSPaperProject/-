
#include <vector>
#include "iextreme.h"
#include "../../../share_data/Enum_public.h"
#include "paper object manager.h"

#include "../Player/BasePlayer.h"
#include "paper object.h"

Paper_obj_mng::Paper_obj_mng() : original_model(nullptr), number_of_objects(0)
{
	obj_array.clear();
}

Paper_obj_mng::~Paper_obj_mng()
{
	delete[] original_model;
	delete original_flyer;
	for (unsigned i = 0; i < obj_array.size(); i++)
		delete obj_array[i];
	SAFE_DELETE(mark);
}

void Paper_obj_mng::Initialize()
{
	Load();
	mark = new iex2DObj("DATA/Camera/mark.png");
}

void Paper_obj_mng::Release()
{
	delete[] original_model; original_model = nullptr;
	for (unsigned i = 0; i < obj_array.size(); i++) delete obj_array[i];
	obj_array.clear();
	delete original_flyer;
	//SAFE_DELETE(mark);
}

void Paper_obj_mng::Update()
{
	for (auto it = obj_array.begin(); it != obj_array.end(); it++)
	{
		(*it)->Update();
	}
}

void Paper_obj_mng::Render(BasePlayer *player, iexShader *shader, char *name)
{
	int i = 0;
	if (shader)
	{
		for (auto it : obj_array)
		{
			it->Render(shader, name);
			bool isTarget = (player->Get_action() == BasePlayer::ACTION_PART::REND && player->Get_poster_num() == i);
			if(mark)it->Render_mark(mark, isTarget);
			i++;
		}
	}
	else
	{
		for (auto it : obj_array)
		{
			it->Render();
			bool isTarget = (player->Get_action() == BasePlayer::ACTION_PART::REND && player->Get_poster_num() == i);
			if (mark)it->Render_mark(mark, isTarget);
			i++;
		}
	}
}

//**************************************************

void Paper_obj_mng::Rend( int index )
{
	obj_array[index]->Rend();
}


int Paper_obj_mng::Get_numof()
{
	return number_of_objects;
}

void Paper_obj_mng::Set_animframe(int index, int frame)
{
	obj_array[index]->Set_animframe(frame);
}

const Vector3& Paper_obj_mng::Get_pos(int index)
{
	return obj_array[index]->Get_pos();
}

KIND_PAPER_OBJECT Paper_obj_mng::Get_kind( int index )
{
	return obj_array[index]->Get_kind();
}

//**************************************************

unsigned int Paper_obj_mng::Get_receive_data_size()
{
	unsigned int size( 0 );

	for( auto it : obj_array )
	{
		size += it->Get_receive_data_size();
	}

	return size;
}

void Paper_obj_mng::Set_receive_data( char *in )
{
	for( auto it : obj_array )
	{
		it->Set_receive_data( in );
		in += it->Get_receive_data_size();
	}
}

//**************************************************

#include "poster/Poster.h"
#include "flyer/flyer.h"
#include "../fstream/fstream_paper.h"
#include "IEX_Math2.h"
#include <string>

void Paper_obj_mng::Load()
{
	number_of_objects = 0;

	//**************************************************
	/// 3DObj�̃t�@�C�������[�h
	//**************************************************
	//char work[256];

	// ���f�����[�h
	original_flyer = new iex3DObj( "DATA/paper object/flyer/flyer.IEM" );
	unsigned int num_models = 8;							// ���f���̎��
	original_model = new iexMesh[num_models];
	original_model[0].LoadIMO( "DATA/paper object/Poster/posuta-.IMO" );
	original_model[1].LoadIMO( "DATA/paper object/calendar/calendar.IMO" );
	original_model[2].LoadIMO( "DATA/paper object/magazin/magazin.IMO" );
	original_model[3].LoadIMO( "DATA/paper object/money/money.IMO" );
	original_model[4].LoadIMO( "DATA/paper object/seisho/seisho.IMO" );
	original_model[5].LoadIMO( "DATA/paper object/sign/sign.IMO" );
	original_model[6].LoadIMO( "DATA/paper object/toilet_paper/toilet_roll.IMO" );
	original_model[7].LoadIMO( "DATA/paper object/zasshi/zasshi.IMO" );

	// �ʒu���[�h
	Load_flyer();
	Load_poster_tmp<Poster>( "DATA/MATI/position/poster_pos.txt", &original_model[0] );
	Load_poster_tmp<Calendar>( "DATA/MATI/position/calendar_pos.txt", &original_model[1] );
	Load_poster_tmp<Magazin>( "DATA/MATI/position/magazin_pos.txt", &original_model[2] );
	Load_poster_tmp<Money>( "DATA/MATI/position/money_pos.txt", &original_model[3] );
	Load_poster_tmp<Seisho>( "DATA/MATI/position/seisho_pos.txt", &original_model[4] );
	Load_poster_tmp<Sign>( "DATA/MATI/position/sign_pos.txt", &original_model[5] );
	Load_poster_tmp<Toilet_paper>( "DATA/MATI/position/toilet_paper_pos.txt", &original_model[6] );
	Load_poster_tmp<Zasshi>( "DATA/MATI/position/zasshi.txt", &original_model[7] );

}

void Paper_obj_mng::Load_flyer()
{
	std::ifstream infs( "DATA/MATI/position/flyer pos.txt" );
	std::string str;

	unsigned int time = 0;
	while( !infs.eof() )
	{
		infs >> str; // ������ǂݍ���
		if( str == "time" ) // ��������
		{
			infs >> time;
		}
		else
		{
			// �p�x
			float angle = std::stof( str, 0 );
			// �ʒu
			Vector3 pos;
			infs >> pos.x;
			infs >> pos.y;
			infs >> pos.z;
			// �|�C���g (�ǂނ���)
			int point;
			infs >> point;
			// �t���C���[�쐬
			Flyer *p = new Flyer;
			p->Initialize( original_flyer, time );
			p->Set_pose( angle, pos );
			obj_array.push_back( p );
			this->number_of_objects++;
		}
	}
}

template<class POSTERCLASS>void Paper_obj_mng::Load_poster_tmp( char *filename, iexMesh *model )
{
	std::ifstream infs( filename );

	// �|�X�^�[�̈ʒu�Ƃ�
	POSTERCLASS *p( nullptr );
	float angle( 0 );
	Vector3 pos( 0, 0, 0 );
	int point( 0 );
	while( !infs.eof() )
	{
		// �ʒu
		infs >> angle;
		angle = Degree_to_radian( angle );
		infs >> pos;
		// �|�C���g (�ǂނ���)
		int point;
		infs >> point;
		// �쐬
		p = new POSTERCLASS;
		p->Initialize( model );
		p->Set_pose( angle, pos );
		obj_array.push_back( p );
		this->number_of_objects++;
	}
	infs.close();
}

Paper_obj_mng *paper_obj_mng = nullptr;
