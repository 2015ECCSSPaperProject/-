#include	"iextreme.h"
#include	"system/system.h"
#include	"../Manager/GameManager.h"
#include	"BasePlayer.h"

#include	"../../../share_data/Enum_public.h"
#include	"../stage/Stage.h"


/*	ベースプレイヤー	*/

//****************************************************************************************************************
//
//				 初	期	化	と	解	放	
//
//****************************************************************************************************************
BasePlayer::BasePlayer(int id) :m_id(id)
{
	pos = VECTOR_ZERO;
	move = VECTOR_ZERO;
}

void BasePlayer::Initialize(iex3DObj *obj)
{
	// 基本パラメータ初期化
	pos.x = -(float)(10 + (m_id * 4));// 仮
	angleY = 0.0f;
	scale = .5f;
	move = Vector3(0, 0, 0);
	speed = 1.5f;
	fallspeed = .1f;
	se_receive = 0;
	isJump = isLand = false;
	jump_pow = 0;
	invincible = false;

	// 3D実体
	model = obj->Clone();

	mynumber = m_id;

	// モーション番号
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
//		更新
//*************************************************************************************************************************
void BasePlayer::Update()
{
	stage->Collision(pos, &move, 5, 2);
	if (stage->Collision_rand(pos, &move, 0))
	{
		// 飛ばなくする
		pos.y += move.y, move.y = 0;
		isLand = true;
		isJump = false;
	}
	else
	{
		isLand = false;
	}
	// 座標更新
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



//　実態
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

		// 距離
		if (vec.Length() < 16)
		{
			vec.Normalize();
			Vector3 front(sinf(player[me]->Get_angleY()), 0, cosf(player[me]->Get_angleY()));

			// 角度
			if (Vector3Dot(vec, front) > .707f)
			{
				no = i;
				break;
			}
		}
	}
	return no;
}
