#include	"iExtreme.h"
#include	"OKB.h"

/*�f�o�b�O.cpp*/

iex2DObj* Debug::_128;
int Debug::x128, Debug::y128;
iex2DObj* Debug::_256;
int Debug::x256, Debug::y256;

int Debug::speed = 1;

//��
iexMesh* Debug::ball;
iexMesh* Debug::Hitball;

void Debug::Init()
{
	_128 = new iex2DObj("DATA/debug/_128.png");
	_256 = new iex2DObj("DATA/debug/_256.png");

	x128 = 0; y128 = 0;
	x256 = 0; y256 = 0;

	speed = 1;

	//��
	//	����`��p���b�V���ǂݍ���
	ball = new iexMesh("DATA/debug/BALL.IMO");
	Hitball = new iexMesh("DATA/debug/BALLR.IMO");

}

void Debug::CleanUp()
{
	delete _128;
	delete _256;

	delete	ball;
	delete	Hitball;
}

void Debug::Draw128()
{
	if (GetKeyState('I') & 0x80)y128 -= speed;
	if (GetKeyState('K') & 0x80)y128 += speed;
	if (GetKeyState('J') & 0x80)x128 -= speed;
	if (GetKeyState('L') & 0x80)x128 += speed;

	if (GetKeyState('U') & 0x80)speed++;
	if (GetKeyState('O') & 0x80){ if (speed > 1)speed--; };

	_128->Render(x128, y128, 128, 128, 0, 0, 128, 128);

	char	str[64];
	sprintf(str, "I�� K�� J�� L�� �����ā@U��speed���@O��speed���@���݂�Speed%d", speed);
	IEX_DrawText(str, 1280 - 514, 720 - 128, 600, 20, 0xFFFFFF00);

	sprintf(str, "128�����X%d�Ey%d", x128, y128);
	IEX_DrawText(str, 1280 - 514, 720 - 96, 200, 20, 0xFFFFFF00);
}

void Debug::Draw128TRG()
{
	if (KeyBoardTRG(KB_I))y128 -= speed;
	if (KeyBoardTRG(KB_K))y128 += speed;
	if (KeyBoardTRG(KB_J))x128 -= speed;
	if (KeyBoardTRG(KB_L))x128 += speed;

	if (KeyBoardTRG(KB_U))speed++;
	if (KeyBoardTRG(KB_O)){ if (speed > 1)speed--; };

	_128->Render(x128, y128, 128, 128, 0, 0, 128, 128);

	char	str[64];
	sprintf(str, "I�� K�� J�� L�� �����ā@U��speed���@O��speed���@���݂�Speed%d", speed);
	IEX_DrawText(str, 1280 - 514, 720 - 64, 600, 20, 0xFFFFFF00);

	sprintf(str, "128�����X%d�Ey%d", x128, y128);
	IEX_DrawText(str, 1280 - 514, 720 - 32, 200, 20, 0xFFFFFF00);
}

void Debug::Draw256()
{
	if (GetKeyState('I') & 0x80)y256 -= speed;
	if (GetKeyState('K') & 0x80)y256 += speed;
	if (GetKeyState('J') & 0x80)x256 -= speed;
	if (GetKeyState('L') & 0x80)x256 += speed;

	if (GetKeyState('U') & 0x80)speed++;
	if (GetKeyState('O') & 0x80){ if (speed > 1)speed--; };

	_256->Render(x256, y256, 256, 256, 0, 0, 256, 256);

	char	str[64];
	sprintf(str, "I�� K�� J�� L�� �����ā@U��speed���@O��speed���@���݂�Speed%d", speed);
	IEX_DrawText(str, 1280 - 514, 720 - 64, 600, 20, 0xFFFFFF00);

	sprintf(str, "256�����X%d�Ey%d", x256, y256);
	IEX_DrawText(str, 1280 - 514, 720 - 32, 200, 20, 0xFFFFFF00);
}

void Debug::Draw256TRG()
{
	if (KeyBoardTRG(KB_I))y256 -= speed;
	if (KeyBoardTRG(KB_K))y256 += speed;
	if (KeyBoardTRG(KB_J))x256 -= speed;
	if (KeyBoardTRG(KB_L))x256 += speed;

	if (KeyBoardTRG(KB_U))speed++;
	if (KeyBoardTRG(KB_O)){ if (speed > 1)speed--; };

	_256->Render(x256, y256, 256, 256, 0, 0, 256, 256);

	char	str[64];
	sprintf(str, "I�� K�� J�� L�� �����ā@U��speed���@O��speed���@���݂�Speed%d", speed);
	IEX_DrawText(str, 1280 - 514, 720 - 64, 600, 20, 0xFFFFFF00);

	sprintf(str, "256�����X%d�Ey%d", x256, y256);
	IEX_DrawText(str, 1280 - 514, 720 - 32, 200, 20, 0xFFFFFF00);
}


//****************************************************************
//		�����蔻��̋���\������
//****************************************************************
void Debug::Ball_Render(Vector3 Pos, float Radius, bool Hit)
{
	iexMesh		*Obj;

	//  �����A�q�b�g�t���O�� true �̂Ƃ���
	if (Hit){
		//  �Ԃ������Z�b�g����
		Obj = Hitball;
	}
	//  �q�b�g�t���O�� false �̂Ƃ���
	else{
		//  ���������Z�b�g����
		Obj = ball;
	}

	// ���܂̈ʒu�Ƒ傫����ݒ肷��
	Obj->SetPos(Pos);
	Obj->SetScale(Radius);

	// ���̍X�V�ƕ`��������Ȃ�
	Obj->Update();
	Obj->Render(RS_COPY, 0.1f);

}