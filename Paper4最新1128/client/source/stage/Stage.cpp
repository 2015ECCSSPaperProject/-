
#include "Stage.h"
#include "Area.h"
#include "../fstream/fstream_paper.h"
#include <string>
//#include "../player/Player.h"

Stage::Stage()
{}

Stage::~Stage()
{
	Release();
}

void Stage::Initialize()
{
	//show_model = new iexMesh("DATA/MATI/stage_machi.IMO");
	//collision_model = new iexMesh("DATA/MATI/stage_atari.IMO");

	//show_model = new iexMesh("../stage_machi.IMO");
	//collision_model = new iexMesh("../stage_machi.IMO");

	
	Load_mapdata();


	for( unsigned int i = 0; i < show_model.size(); i++ )
	{
		show_model[i]->Update();
	}



	area = new Area_mng;
	area->Push("./DATA/MATI/area/test area 1.IMO");
	area->Push("./DATA/MATI/area/test area 2.IMO");
}

void Stage::Release()
{
	for( unsigned int i = 0; i < show_model.size(); i++ )
		delete show_model[i];
	show_model.clear();
	for( unsigned int i = 0; i < collision_model.size(); i++ )
		delete collision_model[i];
	collision_model.clear();
	delete area; area = nullptr;
}

void Stage::Update()
{

}

void Stage::Render(iexShader *shader, char *name)
{
	if (shader)
	{
		for( unsigned int i = 0; i < show_model.size(); i++ )
		{
			show_model[i]->Render( shader, name );
		}
		area->Render(shader, name);
	}
	else
	{
		for( unsigned int i = 0; i < show_model.size(); i++ )
		{
			show_model[i]->Render();
		}
		area->Render();
	}
}

// �����蔻��

bool Stage::Collision_rand(const Vector3 &pos, Vector3 *move, float up)
{
	if (move->y >= 0)
		return false;

	Vector3 hit_pos, ray_pos(pos), vec(0, -1, 0);
	float dist(FLT_MAX);
	// xz�����ړ�
	ray_pos += *move;
	ray_pos.y = pos.y + 3.0f;

	if (collision_model[0]->RayPickUD(&hit_pos, &ray_pos, &vec, &dist) == -1)
		return false;

	if (pos.y + move->y >= hit_pos.y)
		return false;

	move->y = hit_pos.y - pos.y;
	return true;
}

void Stage::Collision(const Vector3 &pos, Vector3 *move, float radius, int recursive_counter)
{
	if (recursive_counter <= 0)
		return;

	Vector3 move_xz(move->x, 0, move->z); // move��x��z����

	Vector3 hit_pos, ray_pos, vec;
	float dist(FLT_MAX);
	// ���C�̎n�_���グ��
	ray_pos = pos;
	ray_pos.y += 1.0f;
	// ���C�̕��������߂�
	vec = move_xz;
	vec.y = 0;
	vec.Normalize();

	if (collision_model[0]->RayPick(&hit_pos, &ray_pos, &vec, &dist) == -1)
		return;

	Vector3 next_pos(pos + move_xz);
	// �ǂɂ߂荞�񂾗�
	Vector3 sink(next_pos - hit_pos);
	// sink�̖@�������̑傫��
	float sink_nl;
	vec.y = 0;
	vec.Normalize();
	sink_nl = -Vector3Dot(sink, vec);

	if (sink_nl + radius <= 0)
		return;

	sink_nl += radius;
	*move += vec * sink_nl;
}



unsigned int Stage::Area_Get_numof()
{
	return area->Get_num();
}

void Stage::Area_Is_work(unsigned int index, bool in)
{
	area->Set_Is_work(index, in);
}


void Stage::Load_mapdata()
{
	// �e�L�X�g�t�@�C�����J��
	std::ifstream ifs( "DATA/MATI/data.txt" );
	// �Ō�܂Ń��[�v
	while( ifs.eof() == false )
	{
		// ���b�V���t�@�C���̃p�X��ǂ�
		std::string filename;
		ifs >> filename;
		// IMO����Ȃ�������Ƃ΂�
		if( filename.find( "IMO" ) == std::string::npos )
			continue;
		// string����char�z��ɕϊ�
		char c[64] = {};
		strcat_s( c, 64, filename.c_str() );
		// imo���烁�b�V���쐬
		iexMesh* mesh = new iexMesh( c );
		// ���s������G���[
		if( mesh->GetMesh() == NULL )
		{
			char text[256] = {};
			sprintf_s( text, "%s �ǂݍ��ݎ��s", c );
			MessageBox( NULL, text, "�G���[", 0 );
			continue;
		}
		// �z�u�����
		int num;
		ifs >> num;
		// num�z�u
		for( int i = 0; i < num; i++ )
		{
			// �p�x�ƈʒu��ǂ�
			Vector3 angle, pos;
			ifs >> angle;
			angle *= 0.0174532924f; // �x���烉�W�A���ɕϊ�
			ifs >> pos;
			// ���W�n�ϊ�
			pos.x *= -1;
			angle *= -1;
			// ���b�V���쐬
			iexMesh *m;
			if( i == 0 ) // �ŏ��̈�͂��̂܂�
				m = mesh;
			else         // �c��̓N���[��
				m = mesh->Clone();
			// �p�x�ƈʒu�ݒ�
			m->SetAngle( angle.x, angle.y, angle.z );
			m->SetPos( pos.x, pos.y, pos.z );
			// �X�V
			m->Update();
			// �z��ɒǉ�
			show_model.push_back( m );
		}
	}
}

Stage *stage = nullptr;
