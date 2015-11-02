#include	"iextreme.h"
#include	"system/system.h"
#include	"../Manager/GameManager.h"
#include	"BasePlayer.h"

#include	"../../../share_data/Enum_public.h"
#include	"../stage/Stage.h"


/*	�x�[�X�v���C���[	*/

//****************************************************************************************************************
//
//				 ��	��	��	��	��	��	
//
//****************************************************************************************************************
BasePlayer::BasePlayer(int id) :m_id(id)
{
	pos = VECTOR_ZERO;
	move = VECTOR_ZERO;
}

void BasePlayer::Initialize(iex3DObj *obj)
{
	// ��{�p�����[�^������
	pos.x = -(float)(10 + (m_id * 4));// ��
	angleY = 0.0f;
	scale = .5f;
	move = Vector3(0, 0, 0);
	speed = 1.5f;
	fallspeed = .1f;
	se_receive = 0;
	isJump = isLand = false;
	jump_pow = 0;
	invincible = false;

	// 3D����
	model = obj->Clone();

	mynumber = m_id;

	// ���[�V�����ԍ�
	motion_no = 0;
}

BasePlayer::~BasePlayer()
{
	Release();
}

void BasePlayer::Release()
{
	SAFE_DELETE(model);
}


//*************************************************************************************************************************
//		�X�V
//*************************************************************************************************************************
void BasePlayer::Update()
{
	stage->Collision(pos, &move, 5, 2);
	if (stage->Collision_rand(pos, &move, 0))
	{
		// ��΂Ȃ�����
		pos.y += move.y, move.y = 0;
		isLand = true;
		isJump = false;
	}
	else
	{
		isLand = false;
	}
	// ���W�X�V
	pos += move;
}

void BasePlayer::Render()
{
	model->Animation();
	model->SetScale(scale);
	model->SetAngle(angleY);
	model->SetPos(pos);
	model->Update();
	model->Render();
}


void BasePlayer::Set_motion(int no)
{
	if (model->GetMotion() != no)
	{
		model->SetMotion(no);
	}
}



//�@����
BasePlayer* player[PLAYER_MAX];
BasePlayer* kohai[PLAYER_MAX];





int PlayerManager::Check_attack(int me)
{
	int no = -1;
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		if (i == me)continue;
		else if (player[i]->isInvincible())continue;

		Vector3 vec(player[i]->Get_pos() - player[me]->Get_pos());

		// ����
		if (vec.Length() < 16)
		{
			vec.Normalize();
			Vector3 front(sinf(player[me]->Get_angleY()), 0, cosf(player[me]->Get_angleY()));

			// �p�x
			if (Vector3Dot(vec, front) > .707f)
			{
				no = i;
				break;
			}
		}
	}
	return no;
}
